/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with support convert functions
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

typedef struct mapper {
	std::map<RTLIL::SigBit, genome::io_id_t> signal_map;
    std::map<genome::io_id_t, void*> in, out;
} mapper_t;

/**
 * Map signal to ID of gene in chromosome
 * @param bit RTLIL::SigBit signal for map
 * @param mapper mapper with RTLIL>GENE signal maps
 * @param chromosome is chromosome where signal is maped as gene or going to by map
 * @param output bool is this signal output of gate
 * @return int id of Gene
 */
genome::io_id_t map_signal(RTLIL::SigBit bit, mapper_t *mapper, genome::genome *chromosome, bool output = false) {
	
	/**
	 * Constants mapping
	 */
	if (bit == RTLIL::State::S0) {
		return 0;
	}

	if (bit == RTLIL::State::S1) {
		return 1;
	}

	/**
	 * Wires mapping
	 */
	if (mapper->signal_map.count(bit) == 0) {

		mapper->signal_map[bit] = chromosome->size();
		
        if (output) {
            mapper->out[chromosome->size()] = bit.wire;
        } else {
            mapper->in[chromosome->size()]  = bit.wire;
        }

        chromosome->add_dummy_gene();

	} else {
        if (output) {
            mapper->in.erase(mapper->signal_map[bit]);
		} else {
            mapper->out.erase(mapper->signal_map[bit]);
		}
    }

	return mapper->signal_map[bit];
}

/**
 * Map rtlil cell output to GENE
 * @param cell RTLIL::Cell* cell in RTLIL reprezentation
 * @param chromosome is chromosome where signal is maped as gene or going to by map
 * @param mapper mapper with RTLIL>GENE signal maps
 * @return io_id_t id of output in chromosome (PART OF GENE)
 */
genome::io_id_t rtlil2genome_out(RTLIL::Cell* cell, genome::genome *chromosome, mapper_t *mapper) {
    auto sig_y = cell->getPort(ID::Y);
    return map_signal(sig_y, mapper, chromosome, true);
}


/**
 * Map rtl cell inputs to chromosome
 * @param cell RTLIL::Cell* cell in RTLIL reprezentation
 * @param chromosome is chromosome where signal is maped as gene or going to by map
 * @param mapper mapper with RTLIL>GENE signal maps
 * @return std::vector<io_id_t> of inputs IDs
 */
std::vector<genome::io_id_t> rtlil2genome_inputs(RTLIL::Cell* cell, genome::genome *chromosome, mapper_t *mapper) {
	std::vector<genome::io_id_t> inputs;

    if (cell->type.in(ID($_BUF_), ID($_NOT_))) {
		auto sig_a = cell->getPort(ID::A);
        
        inputs.push_back(map_signal(sig_a, mapper, chromosome));

	} else if (cell->type.in(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_))) {
		auto sig_a = cell->getPort(ID::A);
		auto sig_b = cell->getPort(ID::B);

		inputs.push_back(map_signal(sig_a, mapper, chromosome));
		inputs.push_back(map_signal(sig_b, mapper, chromosome));

	} else if (cell->type.in(ID($_AOI3_), ID($_OAI3_))) {
		auto sig_a = cell->getPort(ID::A);
		auto sig_b = cell->getPort(ID::B);
		auto sig_c = cell->getPort(ID::C);

		inputs.push_back(map_signal(sig_a, mapper, chromosome));
		inputs.push_back(map_signal(sig_b, mapper, chromosome));
		inputs.push_back(map_signal(sig_c, mapper, chromosome));

	} else if (cell->type.in(ID($_AOI4_), ID($_OAI4_))) {
		auto sig_a = cell->getPort(ID::A);
		auto sig_b = cell->getPort(ID::B);
		auto sig_c = cell->getPort(ID::C);
		auto sig_d = cell->getPort(ID::D);

		inputs.push_back(map_signal(sig_a, mapper, chromosome));
		inputs.push_back(map_signal(sig_b, mapper, chromosome));
		inputs.push_back(map_signal(sig_c, mapper, chromosome));
		inputs.push_back(map_signal(sig_d, mapper, chromosome));

	} else if (cell->type.in(ID($_MUX_), ID($_NMUX_))) {
		auto sig_a = cell->getPort(ID::A);
		auto sig_b = cell->getPort(ID::B);
		auto sig_s = cell->getPort(ID::S);

		inputs.push_back(map_signal(sig_a, mapper, chromosome));
		inputs.push_back(map_signal(sig_b, mapper, chromosome));
		inputs.push_back(map_signal(sig_s, mapper, chromosome));
	}

	return inputs;
}

/**
 * Add rtlil cell to chromosome
 * @param rtlil_cell RTLIL::Cell* cell in RTLIL reprezentation
 * @param chromosome is chromosome where signal is maped as gene or going to by map
 * @param mapper mapper with RTLIL>GENE signal maps
 * @return genome::cell_gene_t (part of chromosome) must be added to chromosome manualy
 */
void rtlil2genome_cell(RTLIL::Cell* rtlil_cell, representation::representation *repres, mapper_t *mapper) {
	auto output = rtlil2genome_out(rtlil_cell, repres->chromosome, mapper);
	auto inputs = rtlil2genome_inputs(rtlil_cell, repres->chromosome, mapper);

	repres->add_cell(rtlil_cell->type, inputs, output);
}

/**
 * Convert RTLIL reprezentation to chromosome
 * Same basic is in: ABC
 * @param chromosome is chromosome where going to by stored created genes 
 * @param design is design what going to by converted to chromosome
 */
mapper_t design2genome(Design* design, representation::representation *repres) {
	mapper_t mapper;

	for (auto mod : design->selected_modules()) {
		
		mod->fixup_ports();

		if (mod->processes.size() > 0) {
			log("Skipping module %s because it contains processes.\n", log_id(mod));
			continue;
		}

		for (auto cell : mod->selected_cells()) {
			rtlil2genome_cell(cell, repres, &mapper);
			mod->remove(cell); //delete cell in reprezentation

		}
	}

	repres->chromosome->order(mapper.in, mapper.out);
	
	log("%d readed LOGIC cells\n", repres->chromosome->size());

	return mapper;
}
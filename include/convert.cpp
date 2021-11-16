typedef struct mapper {
	std::map<RTLIL::SigBit, int> signal_map;
    std::map<int, void*> in, out;
} mapper_t;

int map_signal(RTLIL::SigBit bit, mapper_t *mapper, genome::genome *chromosome, bool output = false) {
	if (mapper->signal_map.count(bit) == 0) {

		mapper->signal_map[bit] = chromosome->size();
		
        if (output) {
            mapper->out[chromosome->size()] = bit.wire;
        } else {
            mapper->in[chromosome->size()]  = bit.wire;
        }

        chromosome->add_dummy_cell();

	} else {
        if (output) {
            mapper->in.erase(mapper->signal_map[bit]);
		} else {
            mapper->out.erase(mapper->signal_map[bit]);
		}
    }

	return mapper->signal_map[bit];
}

genome::gates_types_t rtlil2genome_type(RTLIL::Cell* cell) {
    if (cell->type == ID($_AND_))
		return genome::GATE_AND;
	else if (cell->type == ID($_NAND_))
		return genome::GATE_NAND;
	else if (cell->type == ID($_OR_))
		return genome::GATE_OR;
	else if (cell->type == ID($_NOR_))
		return genome::GATE_NOR;
	else if (cell->type == ID($_XOR_))
		return genome::GATE_XOR;
	else if (cell->type == ID($_XNOR_))
		return genome::GATE_XNOR;
	else if (cell->type == ID($_ANDNOT_))
		return genome::GATE_ANDNOT;
	else if (cell->type == ID($_ORNOT_))
		return genome::GATE_ORNOT;
    else if (cell->type == ID($_NOT_))
        return genome::GATE_NOT;
    else if (cell->type == ID($_BUF_))
        return genome::GATE_BUF;
    else if (cell->type == ID($_AOI3_))
        return genome::GATE_AOI3;
    else if (cell->type == ID($_OAI3_))
        return genome::GATE_OAI3;
    else if (cell->type == ID($_AOI4_))
        return genome::GATE_AOI4;
    else if (cell->type == ID($_OAI4_))
        return genome::GATE_OAI4;
	else
		log_abort();
}

int rtlil2genome_out(RTLIL::Cell* cell, genome::genome *chromosome, mapper_t *mapper) {
    auto sig_y = cell->getPort(ID::Y);
    return map_signal(sig_y, mapper, chromosome, true);
}

genome::cell_gene_t rtlil2genome_chromosome(RTLIL::Cell* cell, genome::genome *chromosome, mapper_t *mapper) {
	genome::cell_gene_t gene;

    if (cell->type.in(ID($_BUF_), ID($_NOT_))) {
		auto sig_a = cell->getPort(ID::A);
        
        gene.I1 = map_signal(sig_a, mapper, chromosome);

	} else if (cell->type.in(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_))) {
		auto sig_a = cell->getPort(ID::A);
		auto sig_b = cell->getPort(ID::B);

		gene.I1 = map_signal(sig_a, mapper, chromosome);
		gene.I2 = map_signal(sig_b, mapper, chromosome);

	} else if (cell->type.in(ID($_AOI3_), ID($_OAI3_))) {
		auto sig_a = cell->getPort(ID::A);
		auto sig_b = cell->getPort(ID::B);
		auto sig_c = cell->getPort(ID::C);

		gene.I1 = map_signal(sig_a, mapper, chromosome);
		gene.I2 = map_signal(sig_b, mapper, chromosome);
		gene.I3 = map_signal(sig_c, mapper, chromosome);

	} else if (cell->type.in(ID($_AOI4_), ID($_OAI4_))) {
		auto sig_a = cell->getPort(ID::A);
		auto sig_b = cell->getPort(ID::B);
		auto sig_c = cell->getPort(ID::C);
		auto sig_d = cell->getPort(ID::D);

		gene.I1 = map_signal(sig_a, mapper, chromosome);
		gene.I2 = map_signal(sig_b, mapper, chromosome);
		gene.I3 = map_signal(sig_c, mapper, chromosome);
		gene.I4 = map_signal(sig_d, mapper, chromosome);
	}

	return gene;
}

void rtlil2genome_cell(RTLIL::Cell* rtlil_cell, genome::genome *chromosome, mapper_t *mapper) {
	genome::cell_t chromosome_cell;

	chromosome_cell.type   = rtlil2genome_type(rtlil_cell);
    chromosome_cell.id     = rtlil2genome_out(rtlil_cell, chromosome, mapper);
	chromosome_cell.gene   = rtlil2genome_chromosome(rtlil_cell, chromosome, mapper);

	chromosome->update_cell(chromosome_cell);
}

//http://www.clifford.at/yosys/files/yosys_presentation.pdf
void genome2design(genome::genome *chromosome, Design* design) {
	std::map<int, RTLIL::Wire*> assign_map;
	auto mod = design->selected_modules()[0];

	//map inputs
	for (auto input = chromosome->wire_in.begin(); input != chromosome->wire_in.end(); input++) {
		assign_map[input->first] = (RTLIL::Wire *) input->second;
	}

	//map outputs
	for (auto output = chromosome->wire_out.begin(); output != chromosome->wire_out.end(); output++) {
		assign_map[output->first] = (RTLIL::Wire *) output->second;
	}

	for (auto i = chromosome->last_input + 1; i < chromosome->size(); i++) {

		auto cell = chromosome->get_cell(i);

		RTLIL::Wire* a = assign_map[cell.gene.I1];
		RTLIL::Wire* b = assign_map[cell.gene.I2];
		RTLIL::Wire* y;

		if (assign_map.count(cell.id)) {
			y = assign_map[cell.id];
		} else {
			y = mod->addWire("$cgploss_y_" + std::to_string(cell.id));
			assign_map[cell.id] = y;
		}

		if (cell.type == genome::GATE_AND) {
			mod->addAnd(NEW_ID, a, b, y);
		} else if (cell.type == genome::GATE_OR) {
			mod->addOr(NEW_ID, a, b, y);
		}
	}


	mod->fixup_ports();
}

mapper_t design2genome(Design* design, genome::genome *chromosome) {
	mapper_t mapper;

	for (auto mod : design->selected_modules()) {
		if (mod->processes.size() > 0) {
			log("Skipping module %s because it contains processes.\n", log_id(mod));
			continue;
		}

		for (auto cell : mod->selected_cells()) {
			rtlil2genome_cell(cell, chromosome, &mapper);
			mod->remove(cell); //delete cell in reprezentation

		}

		chromosome->print(log);
		chromosome->order(mapper.in, mapper.out);


		log("%d readed LOGIC cells\n", chromosome->size());
	}

	return mapper;
}
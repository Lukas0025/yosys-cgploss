typedef struct mapper {
	std::map<Yosys::RTLIL::SigBit, int> signal_map;
	int                                 last = 0;
} mapper_t;

int map_signal(RTLIL::SigSpec bit, mapper_t *mapper) {
	if (mapper->signal_map.count(bit) == 0) {
		mapper->signal_map[bit] = mapper->last;
		mapper->last++;
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
    else if (cell->type == ID($_MUX_))
        return genome::GATE_MUX;
    else if (cell->type == ID($_NMUX_))
        return genome::GATE_MUX;
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

genome::cell_genome_t rtlil2genome_genome(RTLIL::Cell* cell, mapper_t *mapper) {
	genome::cell_genome_t genome;

    if (cell->type.in(ID($_BUF_), ID($_NOT_))) {
		auto sig_a = cell->getPort(ID::A);
        auto sig_y = cell->getPort(ID::Y);
        
        map_signal(sig_y, mapper); //me
        
        genome.I1 = map_signal(sig_a, mapper);

	} else if (cell->type.in(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_))) {
		auto sig_a = cell->getPort(ID::A);
		auto sig_b = cell->getPort(ID::B);
		auto sig_y = cell->getPort(ID::Y);

        map_signal(sig_y, mapper); //me

		genome.I1 = map_signal(sig_a, mapper);
		genome.I2 = map_signal(sig_b, mapper);

    } else if (cell->type.in(ID($_MUX_), ID($_NMUX_))) {
		auto sig_a = cell->getPort(ID::A);
		auto sig_b = cell->getPort(ID::B);
		auto sig_s = cell->getPort(ID::S);
		auto sig_y = cell->getPort(ID::Y);

        map_signal(sig_y, mapper); //me

        genome.I1 = map_signal(sig_a, mapper);
        genome.I2 = map_signal(sig_b, mapper);
        genome.I3 = map_signal(sig_s, mapper);

	} else if (cell->type.in(ID($_AOI3_), ID($_OAI3_))) {
		auto sig_a = cell->getPort(ID::A);
		auto sig_b = cell->getPort(ID::B);
		auto sig_c = cell->getPort(ID::C);
		auto sig_y = cell->getPort(ID::Y);

        map_signal(sig_y, mapper); //me

		genome.I1 = map_signal(sig_a, mapper);
		genome.I2 = map_signal(sig_b, mapper);
		genome.I3 = map_signal(sig_c, mapper);

	} else if (cell->type.in(ID($_AOI4_), ID($_OAI4_))) {
		auto sig_a = cell->getPort(ID::A);
		auto sig_b = cell->getPort(ID::B);
		auto sig_c = cell->getPort(ID::C);
		auto sig_d = cell->getPort(ID::D);
		auto sig_y = cell->getPort(ID::Y);

        map_signal(sig_y, mapper); //me

		genome.I1 = map_signal(sig_a, mapper);
		genome.I2 = map_signal(sig_b, mapper);
		genome.I3 = map_signal(sig_c, mapper);
		genome.I4 = map_signal(sig_d, mapper);
	}

	return genome;
}

genome::cell_t rtlil2genome_cell(RTLIL::Cell* rtlil_cell, mapper_t *mapper) {
	genome::cell_t genome_cell;

	genome_cell.type   = rtlil2genome_type(rtlil_cell);
	genome_cell.genome = rtlil2genome_genome(rtlil_cell, mapper);

	return genome_cell;
}
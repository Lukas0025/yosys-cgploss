#pragma once
#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"

#include "genome.h"

namespace convert {
	typedef struct mapper {
		std::map<Yosys::RTLIL::SigBit, int> signal_map;
		int                                 last = 0;
	} mapper_t;

	genome::cell_t rtlil2genome_cell(Yosys::RTLIL::Cell*);
	genome::gates_types_t rtlil2genome_type(Yosys::RTLIL::Cell* cell);
	genome::cell_genome_t rtlil2genome_genome(Yosys::RTLIL::Cell* cell, mapper_t *mapper);
	genome::cell_t rtlil2genome_cell(Yosys::RTLIL::Cell* rtlil_cell, mapper_t *mapper);
	int map_signal(Yosys::RTLIL::SigSpec bit, mapper_t *mapper);

	//genome2rtlil();
} 
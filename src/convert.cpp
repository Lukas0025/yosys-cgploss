#include "convert.h"

namespace convert {

	int map_signal(Yosys::RTLIL::SigSpec bit, mapper_t *mapper) {
		if (mapper->signal_map.count(bit) == 0) {
			mapper->signal_map[bit] = mapper->last;
			mapper->last++;
		}

		return mapper->signal_map[bit];
	}

	genome::gates_types_t rtlil2genome_type(Yosys::RTLIL::Cell* cell) {

	}

	genome::cell_genome_t rtlil2genome_genome(Yosys::RTLIL::Cell* cell, mapper_t *mapper) {
		genome::cell_genome_t genome;
		
		if (true) {
			auto sig_a = cell->getPort(Yosys::ID::A);
			auto sig_y = cell->getPort(Yosys::ID::Y);

			map_signal(sig_y, mapper); //me

			log("%d\n", mapper->last);

			genome.I1 = map_signal(sig_a, mapper);

		} else if (false) {
			auto sig_a = cell->getPort(Yosys::ID::A);
			auto sig_b = cell->getPort(Yosys::ID::B);
			auto sig_y = cell->getPort(Yosys::ID::Y);

			map_signal(sig_y, mapper); //me

			genome.I1 = map_signal(sig_a, mapper);
			genome.I2 = map_signal(sig_b, mapper);
			
		}

		return genome;
	}

	genome::cell_t rtlil2genome_cell(Yosys::RTLIL::Cell* rtlil_cell, mapper_t *mapper) {
		genome::cell_t genome_cell;

		genome_cell.type   = rtlil2genome_type(rtlil_cell);
		genome_cell.genome = rtlil2genome_genome(rtlil_cell, mapper);

		return genome_cell;
	}
}
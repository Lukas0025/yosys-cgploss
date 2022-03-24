#include "simulation.h";

namespace simulation {
	unsigned bits_count(io_t vec) {
		unsigned count = 0;

		for (unsigned i = 0; i < SIM_BYTES_COUNT; i++) {
			for (unsigned j = 0; j < 8; j++) {
				count      += vec.byte[i] &  1;
				vec.byte[i] = vec.byte[i] >> 1;
			}
		}

		return count;
	}

	unsigned one_max_loss(std::vector<io_t> xor_outputs) {
		return 0;
	}

}
#pragma once

#include "representation.h"

namespace representation {
	class aig : public representation {
		public:
			aig(genome::genome* chromosome) : representation(chromosome) {};
			genome::io_id_t add_cell(Yosys::RTLIL::IdString type, std::vector<genome::io_id_t> inputs, genome::io_id_t output);
	};
}
#pragma once

#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"

#include "genome.h"
#include "representation.h"
#include <vector>
#include <algorithm>

namespace evolution {
	class generation {
		public:
			unsigned create_kid(unsigned parrentA, unsigned parrentB, std::vector<genome::io_id_t> &crossovers, unsigned part_spec);
			void cross(unsigned parrentA, unsigned parrentB, unsigned parts = 2);
			unsigned add_individual(representation::representation *individual);
			void mutate(unsigned from, unsigned to, unsigned center, unsigned sigma);
			unsigned size();
			
			std::vector<representation::representation *> individuals;
	};
}
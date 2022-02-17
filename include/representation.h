/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * circuic representation interface class file
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"

#include "genome.h"
#include <string>

#define SAFE_TYPE_ID(X) (X + 1)

namespace representation {
	class representation {
		public:
			representation(genome::genome* chromosome) {
				this->chromosome = chromosome;
			}

			virtual genome::io_id_t add_cell(Yosys::RTLIL::IdString type, std::vector<genome::io_id_t> inputs, genome::io_id_t output) = 0;
			
			genome::genome* chromosome;
	};
}
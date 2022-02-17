/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * circuic representation interface class file
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#include "genome.h"
#include <string>

namespace representation {
	class representation {
		public:
			representation(genome::genome* chromosome) {
				this->chromosome = chromosome;
			}

			virtual genome::io_id_t add_cell(std::vector<genome::io_id_t> inputs, genome::io_id_t output) = 0;
			
			genome::genome* chromosome;
	};
}
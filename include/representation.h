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
#include <iostream>
#include <fstream>

#define SAFE_TYPE_ID(X) (X + 1)

namespace representation {
	class representation {
		public:
			representation(genome::genome* chromosome) {
				this->chromosome = chromosome;
			}

			virtual genome::io_id_t add_cell(Yosys::RTLIL::IdString type, std::vector<genome::io_id_t> inputs, genome::io_id_t output) = 0;
			
			virtual std::string parts_naming() {
				return "\"gate\": [\"none\"]";
			}

			void save(std::ofstream& sfile) {
				std::string file_str = "{\n\t\"parts\": [\"gate\", \"input\", \"input\"],\n\t\"partsNaming\": {\n\t\t";
				file_str += this->parts_naming() + "\n\t},";
				file_str += "\n\t\"inputs\": [";

				for (genome::io_id_t i = 0; i <= this->chromosome->last_input; i++) {
					file_str += std::to_string(i) + ",";
				}
				file_str.pop_back();

				file_str += "],";
				file_str += "\n\t\"outputs\": [";

				for (auto output = this->chromosome->wire_out.begin(); output != this->chromosome->wire_out.end(); output++) {
					file_str += std::to_string(output->first) + ",";
				}
				file_str.pop_back();

				file_str += "],\n";
				file_str += this->chromosome->to_string();
				file_str += "\n}";

				sfile << file_str;
			}
			
			genome::genome* chromosome;
	};
}
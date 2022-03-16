/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * circuic representation interface class file
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#pragma once

#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"

#include "genome.h"
#include "simulation.h"
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

			virtual Yosys::RTLIL::Cell* get_rtlil(genome::io_id_t id, Yosys::RTLIL::Module* mod, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map, std::string wire_namespace) = 0;

			virtual void simulate(std::vector<simulation::io_t> &gates_o) = 0;

			virtual unsigned mutate(unsigned center, unsigned sigma) = 0;

			virtual representation* clone() = 0;
			
			virtual std::string parts_naming() {
				return "\"gate\": [\"none\"]";
			}

			void set_rtlil_port(Yosys::RTLIL::Cell* gate, Yosys::RTLIL::IdString port, genome::io_id_t genome_port, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &assign_map) {
				if (genome_port == 0) {
					gate->setPort(port, Yosys::RTLIL::State::S0);
				} else if (genome_port == 1) {
					gate->setPort(port, Yosys::RTLIL::State::S1);
				} else {
					gate->setPort(port, assign_map[genome_port]);
				}
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
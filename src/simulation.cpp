/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with simulation support functions
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */


#include "simulation.h"

namespace simulation {
	unsigned bits_count(io_t vec, unsigned limit) {
		unsigned count = 0;

		for (unsigned i = 0; i < SIM_BYTES_COUNT; i++) {
			for (unsigned j = 0; j < 8; j++) {

				if (i * 8 + j >= limit) {
					return count;
				}

				count      += vec.byte[i] &  1;
				vec.byte[i] = vec.byte[i] >> 1;
			}
		}

		return count;
	}

	unsigned one_max_loss(std::vector<io_t> xor_outputs, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> rtl_ports, config::parse *config_parse) {
		unsigned max = 0;
	
		for (unsigned byte = 0; byte < SIM_BYTES_COUNT; byte++) {
			for (unsigned bit = 0; bit < 8; bit++) {
				unsigned port_id = 0;
				unsigned count   = 0;
				for (auto output: rtl_ports) {
					count                          += (xor_outputs[port_id].byte[byte] & 1) * config_parse->port_weight(output.second);
					xor_outputs[port_id].byte[byte] = xor_outputs[port_id].byte[byte] >> 1;
					port_id++;
				}

				if (count > max) max = count;
			}
		}

		return max;
	}

}
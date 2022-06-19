/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with config parse header
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#pragma once

#include "kernel/yosys.h"
#include "kernel/sigtools.h"

#include "genome.h"

#include <fstream>
#include <iostream>

namespace config {
	class parse {
		public:
			/**
			 * @brief Return weight of port
			 * @param port SigBit of port
			 * @return float weight
			 */
			float port_weight(Yosys::RTLIL::SigBit &port);

			/**
			 * @brief Parse file with port weights specifications
			 * @param configfile input stream of file
			 * @param rtl_ports available output ports in Genome
			 * @return unsigned status code (0 success else line of error)
			 */
			unsigned parse_file(std::ifstream &configfile, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports);

			/**
			 * @brief Check if string reprezenting unsigned number
			 * @param s string of number
			 * @return bool
			 */
			static bool is_number(const std::string& s);

			/**
			 * @brief Get bit size of wire (Port)
			 * @param name name of port
			 * @param rtl_ports available output ports in Genome
			 * @return unsigned bit size
			 */
			static unsigned wire_bits(const std::string& name, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports);


		private:
			/**
			 * @brief Generate MSB First for multi bit ports
			 * @param name name of ports
			 * @param rtl_ports available output ports in Genome
			 */
			void generate_msb_first(const std::string& name, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports);
			
			/**
			 * @brief Generate LSB First for multi bit ports
			 * @param name name of ports
			 * @param rtl_ports available output ports in Genome
			 */
			void generate_lsb_first(const std::string& name, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports);
			
			/**
			 * @brief Parse description of port weight
			 * @param port_name name of port
			 * @param line string of whole line
			 * @param start index of start of description
			 * @param rtl_ports available output ports in Genome
			 * @return bool is success
			 */
			bool parse_desc(const std::string& port_name, const std::string& line, unsigned start, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports);
			
			/**
			 * @brief Parse one line in file
			 * @param line string of whole line
			 * @param rtl_ports available output ports in Genome
			 * @return bool is success
			 */
			bool parse_line(const std::string& line, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports);

			/**
			 * @brief map of ports name and tayer weights
			 */
			std::map<std::string, std::vector<unsigned>> ports;
	};
}
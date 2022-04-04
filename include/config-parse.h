#pragma once

#include "kernel/yosys.h"
#include "kernel/sigtools.h"

#include "genome.h"

#include <fstream>
#include <iostream>

namespace config {
	class parse {
		public:
			unsigned port_weight(Yosys::RTLIL::SigBit &port);
			unsigned parse_file(std::ifstream &configfile, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports);
			static bool is_number(const std::string& s);
			static unsigned wire_bits(const std::string& name, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports);


		private:
			void generate_msb_first(const std::string& name, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports);
			void generate_lsb_first(const std::string& name, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports);
			bool parse_desc(const std::string& port_name, const std::string& line, unsigned start, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports);
			bool parse_line(const std::string& line, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports);
			std::map<std::string, std::vector<unsigned>> ports;
	};
}
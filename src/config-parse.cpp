/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with implementation of config parser
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */


#include "config-parse.h"
#include <algorithm>
#include <math.h>

namespace config {
    unsigned parse::parse_file(std::ifstream &config_file, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports) {
        std::string line;
        unsigned line_num = 1;

        while (config_file.good()) {
			std::getline(config_file, line);
			line.push_back(' ');

            if (!this->parse_line(line, rtl_ports)) {
                return line_num;
            }

			line_num++;
        }

        return 0;
    }

    bool parse::is_number(const std::string& s) {
		for (unsigned i = 0; i < s.size(); i++) {
			if ((s[i] > '9') || (s[i] < '0')) return false;
		}

		return true;
    }

    float parse::port_weight(Yosys::RTLIL::SigBit &port) {
        if (this->ports.count(port.wire->name.str()) != 0) {
            if ((int) this->ports[port.wire->name.str()].size() > port.offset) {
		        return this->ports[port.wire->name.str()][port.offset];
            }
	    }

        //default for unknew
        return exp2(port.offset);
    }

    unsigned parse::wire_bits(const std::string& name, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports) {
        unsigned count = 0;

        for (auto rtl_port : rtl_ports) {
			if (rtl_port.second.wire->name.str() == name) {
				count++;
			}
		}

        return count;
    }

    void parse::generate_msb_first(const std::string& name, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports) {
        unsigned bits_size = parse::wire_bits(name, rtl_ports);
        for (unsigned i = 0; i < bits_size; i++) {
            this->ports[name].push_back(1 << (bits_size - i - 1));
        }
    }

    void parse::generate_lsb_first(const std::string& name, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports) {
        for (unsigned i = 0; i < parse::wire_bits(name, rtl_ports); i++) {
            this->ports[name].push_back(1 << i);
        }
    }

    bool parse::parse_desc(const std::string& port_name, const std::string& line, unsigned start, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports) {

        std::string param = "";
        for (unsigned i = start; i < line.size(); i++) {
            if (isspace(line[i]) || (line[i] == '#')) {
				
				std::transform(param.begin(), param.end(), param.begin(), ::tolower);

                if (param.size() > 0) {
                    if (param == "lsb-first") {
                        this->generate_lsb_first(port_name, rtl_ports);
                        break;
                    } else if (param == "msb-first") {
                        this->generate_msb_first(port_name, rtl_ports);
                        break;
                    } else if (parse::is_number(param)) {
                        this->ports[port_name].push_back(stoi(param));
                    } else {
                        return false;
                    }
                }

                param = "";
                continue;
            }

            if (line[i] == '#') break;
            
            param.push_back(line[i]);
        }

        return this->ports[port_name].size() > 0;
    }

    bool parse::parse_line(const std::string& line, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports) {

        std::string port_name = "";
        for (unsigned i = 0; i < line.size(); i++) {
            if (isspace(line[i])) continue;
            else if (line[i] == ':') return parse_desc(port_name, line, i + 1, rtl_ports);
            else if (line[i] == '#') return port_name.size() == 0;
            else port_name.push_back(line[i]);
        }

		return true;
    }
}
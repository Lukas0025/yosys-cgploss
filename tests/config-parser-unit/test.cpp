/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * main file of program
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

//YOSYS
#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"

//CGP
#include "genome.h"
#include "generation.h"

#include <string>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <stdexcept>

//representations
#include "aig.h"

#include "config-parse.h"

//messages macros
#define info_message(...) log("[INFO] " __VA_ARGS__)
#define warning_message(...) log("[WARNING] " __VA_ARGS__)
#define error_message(...) log("[ERROR] " __VA_ARGS__)

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

#include "convert.cpp"

void assert_num(std::string name, unsigned out, unsigned exp) {
    if (exp != out) {
		log("Assert error %s expected %d returd %d\n", name.c_str(), exp, out);
        exit(1);
    }
}

Yosys::RTLIL::SigBit getSigBit(std::string name, int offset, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> &rtl_ports) {
	for (auto rtl_port : rtl_ports) {
		if (rtl_port.second.wire->name.str() == name) {
			if (rtl_port.second.offset == offset) return rtl_port.second;
		}
	}
}

struct cgploss : public Pass {
	cgploss() : Pass("cgploss") {}

	void help() override {
		log("\n");
		log("    cgploss test unit\n");
		log("\n");
	}

	void execute(vector<string> params, Design* design) override {
		auto chromosome = new genome::genome();
		representation::representation *repres = new representation::aig(chromosome);

		auto map    = design2genome(design, repres);
		auto parser = new config::parse();

		for (auto rtl_port : map.out) {
			log("port: %s\n", rtl_port.second.wire->name.c_str());
		}

		std::ifstream config_file;

		config_file.open("tests/config-parser-unit/config_1_bad.yaml");
		assert_num("config_1_bad.yaml", parser->parse_file(config_file, map.out), 6);
		config_file.close();

		config_file.open("tests/config-parser-unit/config_2_bad.yaml");
		assert_num("config_2_bad.yaml", parser->parse_file(config_file, map.out), 6);
		config_file.close();

		config_file.open("tests/config-parser-unit/config_4_bad.yaml");
		assert_num("config_4_bad.yaml", parser->parse_file(config_file, map.out), 6);
		config_file.close();

		config_file.open("tests/config-parser-unit/config_5_bad.yaml");
		assert_num("config_5_bad.yaml", parser->parse_file(config_file, map.out), 6);
		config_file.close();

		config_file.open("tests/config-parser-unit/config_3_ok.yaml");
		assert_num("config_3_ok.yaml", parser->parse_file(config_file, map.out), 0);
		config_file.close();

		auto port = getSigBit("\\sum", 0, map.out);

		assert_num("\\sum bit 0", parser->port_weight(port = getSigBit("\\sum", 0, map.out)), 128);
		assert_num("\\sum bit 7", parser->port_weight(port = getSigBit("\\sum", 7, map.out)), 1);

		assert_num("\\spi-data bit 0", parser->port_weight(port = getSigBit("\\spi_data", 0, map.out)), 1);
		assert_num("\\spi-data bit 3", parser->port_weight(port = getSigBit("\\spi_data", 3, map.out)), 8);

		assert_num("\\spi2-data bit 0", parser->port_weight(port = getSigBit("\\spi2_data", 0, map.out)), 1);
		assert_num("\\spi2-data bit 1", parser->port_weight(port = getSigBit("\\spi2_data", 1, map.out)), 2);

		assert_num("\\uart_rx bit 0", parser->port_weight(port = getSigBit("\\uart_rx", 0, map.out)), 1024);

		assert_num("\\uart_tx bit 0", parser->port_weight(port = getSigBit("\\uart_tx", 0, map.out)), 2);
		assert_num("\\uart_tx bit 1", parser->port_weight(port = getSigBit("\\uart_tx", 1, map.out)), 4);
		assert_num("\\uart_tx bit 2", parser->port_weight(port = getSigBit("\\uart_tx", 2, map.out)), 8);
		assert_num("\\uart_tx bit 3", parser->port_weight(port = getSigBit("\\uart_tx", 3, map.out)), 10);

		assert_num("\\uart2_tx bit 0", parser->port_weight(port = getSigBit("\\uart2_tx", 0, map.out)), 1);
		assert_num("\\uart2_tx bit 1", parser->port_weight(port = getSigBit("\\uart2_tx", 1, map.out)), 2);
		assert_num("\\uart2_tx bit 2", parser->port_weight(port = getSigBit("\\uart2_tx", 2, map.out)), 4);
		assert_num("\\uart2_tx bit 3", parser->port_weight(port = getSigBit("\\uart2_tx", 3, map.out)), 8);
    }
} cgploss;

PRIVATE_NAMESPACE_END
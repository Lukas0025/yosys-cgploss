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

#include <string>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <stdexcept>

//representations
#include "aig.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

#include "convert.cpp"

struct cgploss : public Pass {
	cgploss() : Pass("cgploss") {}

	void help() override {
		log("\n");
		log("    cgploss [options]\n");
		log("\n");
	}

	void execute(vector<string> params, Design* design) override {
		log("CGP EXTENSION\n");

		/* Params parsing */
		bool wire_test = false;
		for (auto param : params) {
			
			if (param == "-wire-test") {
				wire_test = true;
			}

		}

		/* CGP Code */
		auto chromosome = new genome::genome();
		auto repres     = new representation::aig(chromosome);

		try {
			auto map = design2genome(design, repres);

			if (!wire_test) {
				//CGP CODE
				std::ofstream myfile;
				myfile.open ("example.txt");
				repres->save(myfile);
				myfile.close();
			}
			
			genome2design(repres, design);
		} catch( const std::invalid_argument& e ) {
			log("error");
		}

	}

} cgploss;

PRIVATE_NAMESPACE_END
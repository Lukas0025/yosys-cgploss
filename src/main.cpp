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
		bool debug_indiv = false;
		for (auto param : params) {
			
			if (param == "-wire-test") {
				wire_test = true;
			}

			if (param == "-save-individuals") {
				debug_indiv = true;
			}

		}

		/* CGP Code */
		auto chromosome = new genome::genome();
		auto repres     = new representation::aig(chromosome);

		try {
			auto map = design2genome(design, repres);

			if (!wire_test) {

				/*auto generation = new evolution::generation(repres, generation_size);
				generation->selection(repres);


				if (debug_indiv) {
					std::ofstream reffile;
					reffile.open ("reference_generation0_individual0.cgp");
					repres->save(reffile);
					reffile.close();
				}

				//log("%d mutations", repres->mutate(10, 5));

				//CGP CODE


				std::vector<simulation::io_t> gates_out(repres->chromosome->size());

				gates_out[0].byte[0] = 0;
				gates_out[1].byte[0] = 255;

				//simulation test
				repres->simulate(gates_out);

				//log("%x\n", gates_out[4].byte[0]);*/
			}
			
			genome2design(repres, design);
		} catch( const std::invalid_argument& e ) {
			log("error");
		}

	}

} cgploss;

PRIVATE_NAMESPACE_END
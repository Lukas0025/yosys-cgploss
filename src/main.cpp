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
		for (int i = 1; i < params.size(); i++) {
			std::string param = params[i];

			if (param == "-wire-test") {
				wire_test = true;
			}
		}

		/* CGP Code */
		auto chromosome = new genome::genome();
		auto repres     = new representation::aig(chromosome);

		auto map = design2genome(design, repres);

		if (!wire_test) {
			//CGP CODE
			chromosome->print(log);
		}

		//genome2design(chromosome, design);

	}

} cgploss;

PRIVATE_NAMESPACE_END
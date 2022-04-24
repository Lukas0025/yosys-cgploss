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
#include <vector>
#include <stdio.h>

#include <iostream>
#include <fstream>

//representations
#include "simulation.h"

#define LAST_INPUT 10

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

void assert(std::string name, simulation::io_t output, unsigned pos, bool inverted = false) {
	simulation::io_t ref;

	SET_VARIANTS_BITS(ref, pos);

	if (inverted) {
		ref.vec = ~ref.vec;
	}

	for (unsigned h = 0; h < SIM_BYTES_COUNT; h++) {

		if (ref.byte[h] != output.byte[h]) {
			log("Error for %s\n", name.c_str());
			exit(1);
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

		std::vector<simulation::io_t> test_circuic(LAST_INPUT + 1);
		std::vector<unsigned>         variant_counter(LAST_INPUT  + 2);

		for (unsigned i = 0; i <= LAST_INPUT; i++) {
			SET_VARIANTS_BITS(test_circuic[i].vec, i);
		}

		bool done = false;
		unsigned i = 0;
		do {
			log("Iteration %d ------------------------\n", i);

			for (unsigned j = 0; j <= LAST_INPUT; j++) {
				log("I%d  - %d - ", j, variant_counter[j]);

				for (unsigned h = 0; h < SIM_BYTES_COUNT; h++) {
					log("%x", test_circuic[j].byte[h]);
				}

				log("\n"); 
			}

			assert("i0i0", test_circuic[0], 0);
			assert("i0i1", test_circuic[1], 1);
			assert("i0i2", test_circuic[2], 2);
			assert("i0i3", test_circuic[3], 3);
			assert("i0i4", test_circuic[4], 4);
			assert("i0i5", test_circuic[5], 5);
			assert("i0i6", test_circuic[6], 6);
			assert("i0i7", test_circuic[7], 7);
			
			if (i == 0) {
				assert("i0i8", test_circuic[8], 8);
			} else if (i == 1) {
				assert("i1i8", test_circuic[8], 8, true);
			}
			

			for (unsigned k = ONE_SIM_VARIANTS; k <= LAST_INPUT; k++) {
				variant_counter[k] = (variant_counter[k] + 1) % (1 << (k - ONE_SIM_VARIANTS));
					
				if (variant_counter[k] == 0) {
					test_circuic[k].vec      = ~test_circuic[k].vec;

					if (k == LAST_INPUT ) {
						if (variant_counter[LAST_INPUT  + 1]) {
							done = true;
						} 

						variant_counter[LAST_INPUT + 1]++;
					}
				}
			}

			i++;
		} while (!done);

        exit(0);
    }
} cgploss;

PRIVATE_NAMESPACE_END
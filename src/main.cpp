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

#define debug_indiv_to_file(debug_indiv_file, repres) if (debug_indiv) {repres->save(debug_indiv_file); }
#define debug_generation_to_file(debug_indiv_file, generation, name) if (debug_indiv) {debug_indiv_file << name; for (unsigned i = 0; i < generation->individuals.size(); i++) { debug_indiv_to_file(debug_indiv_file, generation->individuals[i]) } }


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
		bool wire_test                      = false;
		bool debug_indiv                    = false;
		unsigned param_selection_count      = 50;
		unsigned param_generation_size      = 500;
		unsigned param_max_one_loss         = 10;
		float    param_power_accuracy_ratio = 0.5;
		unsigned param_mutate_center        = 2;
		unsigned param_mutate_sigma         = 2;
		unsigned param_generations_count    = 100;
		unsigned param_parrents_count       = 1;

		for (auto param : params) {
			
			if (param == "-wire_test") {
				wire_test = true;
			}

			if (param == "-save_individuals") {
				debug_indiv = true;
			}

		}

		/* CGP Code */
		auto chromosome = new genome::genome();
		representation::representation *repres = new representation::aig(chromosome);

		try {
			auto map = design2genome(design, repres);

			if (!wire_test) {
				std::ofstream debug_indiv_file;
				debug_indiv_file.open("debug.json");

				debug_indiv_to_file(debug_indiv_file, repres);

				auto generation = new evolution::generation(repres, param_generation_size, param_max_one_loss, param_power_accuracy_ratio);
				auto parrent0   = generation->add_individual(repres->clone());

				//create generation using clone
				for (unsigned i = 0; i < param_generation_size; i++) {
					generation->clone(parrent0);
				}

				generation->mutate(parrent0 + 1, generation->size() - 1, param_mutate_center, param_mutate_sigma);

				debug_generation_to_file(debug_indiv_file, generation, "\n\nGENERATION 0\n\n");

				//score
				auto new_generation = generation->selection(param_selection_count);
				delete generation;

				generation = new_generation;
				for (unsigned i = 1; i < param_generations_count; i++) {
					if (param_parrents_count == 1) {
						//clone
						for (unsigned i = param_selection_count; i < param_generation_size; i++) {
							generation->clone(i % param_selection_count);
						}
					} else {
						//cross
					}

					generation->mutate(param_selection_count, generation->size() - 1, param_mutate_center, param_mutate_sigma);

					debug_generation_to_file(debug_indiv_file, generation, "\n\nGENERATION " + std::to_string(i) + "\n\n");

					//score
					auto new_generation = generation->selection(param_selection_count);
					delete generation;
					
					generation = new_generation;
					
				}

				debug_indiv_file.close();

				auto last_generation = generation->selection(1);
				repres = last_generation->individuals[0]->clone();
				repres->chromosome->cut_unused();
			}
			
			genome2design(repres, design);
		} catch( const std::invalid_argument& e ) {
			log("error");
		}

	}

} cgploss;

PRIVATE_NAMESPACE_END
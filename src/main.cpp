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
#include "config-parse.h"

#include <string>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <regex>

//representations
#include "aig.h"
#include "gates.h"
#include "mig.h"

#define debug_indiv_to_file(debug_indiv_file, repres) if (debug_indiv) {repres->save(debug_indiv_file); }
#define debug_generation_to_file(debug_indiv_file, generation, name) if (debug_indiv) {debug_indiv_file << name; for (unsigned i = 0; i < generation->individuals.size(); i++) { debug_indiv_to_file(debug_indiv_file, generation->individuals[i].repres) } }


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
		unsigned param_max_one_loss         = 0;
		float    param_power_accuracy_ratio = 0.5;
		float    param_max_abs_loss         = 0;
		unsigned param_mutate_center        = 2;
		unsigned param_mutate_sigma         = 2;
		unsigned param_generations_count    = 100;
		unsigned param_parrents_count       = 1;
		unsigned param_cross_parts          = 4;
		std::string config_file             = "";
		std::string param_repres            = "aig";

		for (auto param : params) {
			
			if (param == "-wire_test") {
				wire_test = true;
			} else if (param == "-save_individuals") {
				debug_indiv = true;
			} else if (param.rfind("-ports_weights=", 0) == 0) {
				config_file = param.substr(std::string("-ports_weights=").length());
			} else if (param.rfind("-selection_size=", 0) == 0) {
				auto parsed = param.substr(std::string("-selection_size=").length());

				if (!config::parse::is_number(parsed)) {
					log("[ERROR] Bad value for -selection_size using default\n");
				} else {
					param_selection_count = stoi(parsed);
				}

			} else if (param.rfind("-generation_size=", 0) == 0) {
				auto parsed = param.substr(std::string("-generation_size=").length());

				if (!config::parse::is_number(parsed)) {
					log("[ERROR] Bad value for -generation_size using default\n");
				} else {
					param_generation_size = stoi(parsed);
				}

			} else if (param.rfind("-max_one_error=", 0) == 0) {
				auto parsed = param.substr(std::string("-max_one_error=").length());

				if (!config::parse::is_number(parsed)) {
					log("[ERROR] Bad value for -max_one_error using default\n");
				} else {
					param_max_one_loss = stoi(parsed);
				}

			} else if (param.rfind("-generations=", 0) == 0) {
				auto parsed = param.substr(std::string("-generations=").length());

				if (!config::parse::is_number(parsed)) {
					log("[ERROR] Bad value for -generations using default\n");
				} else {
					param_generations_count = stoi(parsed);
				}

			} else if (param.rfind("-mutations_count=", 0) == 0) {
				auto parsed = param.substr(std::string("-mutations_count=").length());

				if (!config::parse::is_number(parsed)) {
					log("[ERROR] Bad value for -mutations_count using default\n");
				} else {
					param_mutate_center = stoi(parsed);
				}

			} else if (param.rfind("-mutations_count_sigma=", 0) == 0) {
				auto parsed = param.substr(std::string("-mutations_count_sigma=").length());

				if (!config::parse::is_number(parsed)) {
					log("[ERROR] Bad value for -mutations_count_sigma using default\n");
				} else {
					param_mutate_sigma = stoi(parsed);
				}

			} else if (param.rfind("-parrents=", 0) == 0) {
				auto parsed = param.substr(std::string("-parrents=").length());

				if (!config::parse::is_number(parsed)) {
					log("[ERROR] Bad value for -parrents using default\n");
				} else {
					param_parrents_count = stoi(parsed);
				}

				if (param_parrents_count > 2) {
					log("[ERROR] Bad value for -parrents, max value is 2, min value is 1. using 2\n");
					param_parrents_count = 2;
				} else if (param_parrents_count < 1) {
					log("[ERROR] Bad value for -parrents, max value is 2, min value is 1. using 1\n");
					param_parrents_count = 1;
				}

			} else if (param.rfind("-power_accuracy_ratio=", 0) == 0) {
				auto parsed = param.substr(std::string("-power_accuracy_ratio=").length());

				try {
        			param_power_accuracy_ratio = std::stof(parsed);
    			} catch (std::invalid_argument const& ex) {
					log("[ERROR] Bad value for -power_accuracy_ratio using default\n");
				}

			} else if (param.rfind("-max_abs_error=", 0) == 0) {
				auto parsed = param.substr(std::string("-max_abs_error=").length());

				try {
        			param_max_abs_loss = std::stof(parsed);
    			} catch (std::invalid_argument const& ex) {
					log("[ERROR] Bad value for -max_abs_error using default\n");
				}

			} else if (param.rfind("-representation=", 0) == 0) {
				auto parsed = param.substr(std::string("-representation=").length());

				if (parsed == "gates" || parsed == "mig" || parsed == "aig") {
					param_repres = parsed;
				} else {
					log("[ERROR] Bad value for -representation using default\n");
				}

			} else if (param.rfind("-cross_parts=", 0) == 0) {
				auto parsed = param.substr(std::string("-cross_parts=").length());

				if (!config::parse::is_number(parsed)) {
					log("[ERROR] Bad value for -cross_parts using default\n");
				} else {
					param_parrents_count = stoi(parsed);
				}

				if (param_parrents_count < 2) {
					log("[ERROR] Bad value for -cross_parts, min value is 2. using 2\n");
					param_parrents_count = 2;
				}

			} else {
				if (param != "cgploss") {
					log("[WARNING] ignorig argument %s\n", param.c_str());
				}	
			}

		}

		/* CGP Code */
		auto chromosome = new genome::genome();
		auto config     = new config::parse();
		representation::representation *repres = NULL;
		
		if      (param_repres == "aig")   repres = new representation::aig(chromosome);
		else if (param_repres == "gates") repres = new representation::gates(chromosome);
		else if (param_repres == "mig")   repres = new representation::mig(chromosome);

		try {
			auto map = design2genome(design, repres);

			if (config_file.size() > 0) {
				std::ifstream config_file_stream;
				config_file_stream.open(config_file);
				auto error = config->parse_file(config_file_stream, map.out);

				if (error) {
					log("[ERROR] config file parse error on line %d - in failsafe mode using default weights\n", error);
				}

				config_file_stream.close();
			}

			if (param_parrents_count == 2 && param_selection_count < 2) {
				log("[ERROR] minimal selection_count for two parrents is 2. setting to 2\n");
				param_selection_count = 2;
			}

			if (param_selection_count > param_generation_size) {
				log("[ERROR] generation cant be bigger that selection count. setting to selection count\n");
				param_generation_size = param_selection_count;
			}

			log("[INFO] Starting CPG with parameters:\n");
			log("       wire_test           : %d\n", wire_test);
			log("       save_individuals    : %d\n", debug_indiv);
			log("       ports_weights       : %s\n", config_file.c_str());
			log("       selection_size      : %d\n", param_selection_count);
			log("       generation_size     : %d\n", param_generation_size);
			log("       max_one_error       : %d\n", param_max_one_loss);
			log("       max_abs_error       : %f\n", param_max_abs_loss);
			log("       generations         : %d\n", param_generations_count);
			log("       mutations_count     : %d\n", param_mutate_center);
			log("       mutations_sigma     : %d\n", param_mutate_sigma);
			log("       parrents            : %d\n", param_parrents_count);
			log("       cross_parts         : %d\n", param_cross_parts);
			log("       power_accuracy_ratio: %f\n", param_power_accuracy_ratio);
			log("       representation      : %s\n\n", param_repres.c_str());

			if (!wire_test) {
				std::ofstream debug_indiv_file;

				if (debug_indiv) {
					debug_indiv_file.open("debug.json");
				}

				debug_indiv_to_file(debug_indiv_file, repres);

				auto generation = new evolution::generation(repres, param_max_one_loss, param_max_abs_loss, param_power_accuracy_ratio);
				auto parrent0   = generation->add_individual(repres->clone());

				//create generation using clone
				for (unsigned i = 0; i < param_generation_size; i++) {
					generation->clone(parrent0);
				}

				generation->mutate(parrent0 + 1, generation->size() - 1, param_mutate_center, param_mutate_sigma);

				debug_generation_to_file(debug_indiv_file, generation, "\n\nGENERATION 0\n\n");

				//score
				generation->selection(param_selection_count, config);

				for (unsigned generation_id = 1; generation_id < param_generations_count; generation_id++) {
					if (param_parrents_count == 1) {
						//clone
						for (unsigned i = param_selection_count; i < param_generation_size; i++) {
							generation->clone(i % param_selection_count);
						}
					} else {
						//cross
						unsigned parrentA = 0;
						unsigned parrentB = 1;
						while(generation->size() < param_generation_size) {
							generation->cross(parrentA, parrentB, param_cross_parts);

							parrentA = (parrentA + 3) % param_selection_count;
							parrentB = (parrentB + 1) % param_selection_count;
						}
					}

					generation->mutate(param_selection_count, generation->size() - 1, param_mutate_center, param_mutate_sigma);

					debug_generation_to_file(debug_indiv_file, generation, "\n\nGENERATION " + std::to_string(generation_id) + "\n\n");

					//score
					generation->selection(param_selection_count, config);

					log("[INFO] generation %i best individual score %f\n",  generation_id, generation->individuals[0].score);
				}

				repres = generation->individuals[0].repres->clone();
				repres->chromosome->cut_unused();

				debug_indiv_to_file(debug_indiv_file, repres);


				if (debug_indiv) {
					debug_indiv_file.close();
				}

				delete generation;
			}
			
			genome2design(repres, design);
		} catch( const std::invalid_argument& e ) {
			log("error");
		}

	}

} cgploss;

PRIVATE_NAMESPACE_END
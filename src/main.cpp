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

//Base libs
#include <string>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <regex>

//timer
#include <chrono>

//representations
#include "aig.h"
#include "gates.h"
#include "mig.h"

//about
#include "about.h"

//Debug macros
#define debug_indiv_to_file(debug_indiv_file, repres) if (debug_indiv) {repres->save(debug_indiv_file); }
#define debug_generation_to_file(debug_indiv_file, generation, name) if (debug_indiv) {debug_indiv_file << name; for (unsigned i = 0; i < generation->individuals.size(); i++) { debug_indiv_to_file(debug_indiv_file, generation->individuals[i].repres) } }

//messages macros
#define info_message(...) if (param_status) { log("[INFO] " __VA_ARGS__); }
#define warning_message(...) log("[WARNING] " __VA_ARGS__)
#define error_message(...) log("[ERROR] " __VA_ARGS__)

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

/* Params parsing */
bool wire_test;
bool debug_indiv;
bool param_profile;
bool save_final_indiv;
unsigned param_selection_count;
unsigned param_generation_size;
unsigned param_max_one_loss;
float    param_power_accuracy_ratio;
float    param_max_abs_loss;
unsigned param_mutate_center;
unsigned param_mutate_sigma;
unsigned param_generations_count;
unsigned param_parrents_count;
unsigned param_cross_parts;
unsigned param_l_back;
bool     param_status;
unsigned param_max_duration;
std::string config_file;
std::string param_repres;

//include convert functions
#include "convert.cpp"

struct cgploss : public Pass {
	cgploss() : Pass("cgploss") {}

	void help() override {
		about::print_help(log);
	}

	void execute(vector<string> params, Design* design) override {
		log("\nStarting cgploss extension. If you need more runtime information use -status\n\n");

		wire_test                  = false;
		debug_indiv                = false;
		save_final_indiv           = false;
		param_profile              = false;
		param_selection_count      = 50;
		param_generation_size      = 500;
		param_max_one_loss         = 0;
		param_power_accuracy_ratio = 0.5;
		param_max_abs_loss         = 0;
		param_mutate_center        = 2;
		param_mutate_sigma         = 2;
		param_generations_count    = 100;
		param_parrents_count       = 1;
		param_cross_parts          = 4;
		param_l_back               = 0;
		param_status               = false;
		param_max_duration         = 0;
		bool keep_delay            = false;
		std::string config_file    = "";
		std::string final_file     = "";
		std::string inital_file    = "";
		std::string param_repres   = "aig";

		for (auto param : params) {
			
			if (param == "-wire_test") {
				wire_test = true;
			} else if (param == "-save_individuals") {
				debug_indiv = true;
			} else if (param == "-keep_delay") {
				keep_delay = true;
			} else if (param.rfind("-save_final=", 0) == 0) {
				save_final_indiv = true;
				final_file = param.substr(std::string("-save_final=").length());
			} else if (param.rfind("-load_init=", 0) == 0) {
				inital_file = param.substr(std::string("-load_init=").length());
			} else if (param == "-status") {
				param_status = true;
			} else if (param == "-profile") {
				param_profile = true;
			} else if (param.rfind("-ports_weights=", 0) == 0) {
				config_file = param.substr(std::string("-ports_weights=").length());
			} else if (param.rfind("-selection_size=", 0) == 0) {
				auto parsed = param.substr(std::string("-selection_size=").length());

				if (!config::parse::is_number(parsed)) {
					error_message("Bad value for -selection_size using default\n");
				} else {
					param_selection_count = stoi(parsed);
				}

				if (param_selection_count < 1) {
					error_message("Bad value for -selection_size, min value is 1. using 1\n");
					param_selection_count = 1;
				}

			} else if (param.rfind("-max_duration=", 0) == 0) {
				auto parsed = param.substr(std::string("-max_duration=").length());

				if (!config::parse::is_number(parsed)) {
					error_message("Bad value for -max_duration using default\n");
				} else {
					param_max_duration = stoi(parsed);
				}

			} else if (param.rfind("-generation_size=", 0) == 0) {
				auto parsed = param.substr(std::string("-generation_size=").length());

				if (!config::parse::is_number(parsed)) {
					error_message("Bad value for -generation_size using default\n");
				} else {
					param_generation_size = stoi(parsed);
				}

				if (param_generation_size < 2) {
					error_message("Bad value for -generation_size, min value is 2. using 2\n");
					param_generation_size = 2;
				}

			} else if (param.rfind("-max_one_error=", 0) == 0) {
				auto parsed = param.substr(std::string("-max_one_error=").length());

				if (!config::parse::is_number(parsed)) {
					error_message("Bad value for -max_one_error using default\n");
				} else {
					param_max_one_loss = stoi(parsed);
				}

			} else if (param.rfind("-generations=", 0) == 0) {
				auto parsed = param.substr(std::string("-generations=").length());

				if (!config::parse::is_number(parsed)) {
					error_message("Bad value for -generations using default\n");
				} else {
					param_generations_count = stoi(parsed);
				}

			} else if (param.rfind("-mutations_count=", 0) == 0) {
				auto parsed = param.substr(std::string("-mutations_count=").length());

				if (!config::parse::is_number(parsed)) {
					error_message("Bad value for -mutations_count using default\n");
				} else {
					param_mutate_center = stoi(parsed);
				}

			} else if (param.rfind("-mutations_count_sigma=", 0) == 0) {
				auto parsed = param.substr(std::string("-mutations_count_sigma=").length());

				if (!config::parse::is_number(parsed)) {
					error_message("Bad value for -mutations_count_sigma using default\n");
				} else {
					param_mutate_sigma = stoi(parsed);
				}

			} else if (param.rfind("-parents=", 0) == 0) {
				auto parsed = param.substr(std::string("-parents=").length());

				if (!config::parse::is_number(parsed)) {
					error_message("Bad value for -parents using default\n");
				} else {
					param_parrents_count = stoi(parsed);
				}

				if (param_parrents_count > 2) {
					error_message("Bad value for -parents, max value is 2, min value is 1. using 2\n");
					param_parrents_count = 2;
				} else if (param_parrents_count < 1) {
					error_message("Bad value for -parents, max value is 2, min value is 1. using 1\n");
					param_parrents_count = 1;
				}

			} else if (param.rfind("-power_accuracy_ratio=", 0) == 0) {
				auto parsed = param.substr(std::string("-power_accuracy_ratio=").length());

				try {
        			param_power_accuracy_ratio = std::stof(parsed);
    			} catch (std::invalid_argument const& ex) {
					error_message("Bad value for -power_accuracy_ratio using default\n");
				}

			} else if (param.rfind("-max_abs_error=", 0) == 0) {
				auto parsed = param.substr(std::string("-max_abs_error=").length());

				try {
        			param_max_abs_loss = std::stof(parsed);
    			} catch (std::invalid_argument const& ex) {
					error_message("Bad value for -max_abs_error using default\n");
				}

			} else if (param.rfind("-representation=", 0) == 0) {
				auto parsed = param.substr(std::string("-representation=").length());

				if (parsed == "gates" || parsed == "mig" || parsed == "aig") {
					param_repres = parsed;
				} else {
					error_message("Bad value for -representation using default\n");
				}

			} else if (param.rfind("-cross_parts=", 0) == 0) {
				auto parsed = param.substr(std::string("-cross_parts=").length());

				if (!config::parse::is_number(parsed)) {
					error_message("Bad value for -cross_parts using default\n");
				} else {
					param_parrents_count = stoi(parsed);
				}

				if (param_parrents_count < 2) {
					error_message("Bad value for -cross_parts, min value is 2. using 2\n");
					param_parrents_count = 2;
				}

			} else if (param.rfind("-l-back=", 0) == 0) {
				auto parsed = param.substr(std::string("-l-back=").length());

				if (!config::parse::is_number(parsed)) {
					error_message("Bad value for -l-back using default\n");
				} else {
					param_l_back = stoi(parsed);
				}

			} else {
				if (param != "cgploss") {
					warning_message("ignorig argument %s\n", param.c_str());
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
					error_message("config file parse error on line %d - in failsafe mode using default weights\n", error);
				}

				config_file_stream.close();
			}

			if (param_parrents_count == 2 && param_selection_count < 2) {
				error_message("minimal selection_count for two parents is 2. setting to 2\n");
				param_selection_count = 2;
			}

			if (param_selection_count > param_generation_size) {
				error_message("generation cant be bigger that selection count. setting to selection count\n");
				param_generation_size = param_selection_count;
			}

			info_message("Starting CPG with parameters:\n");
			info_message("       wire_test           : %d\n", wire_test);
			info_message("       save_individuals    : %d\n", debug_indiv);
			info_message("       ports_weights       : %s\n", config_file.c_str());
			info_message("       selection_size      : %d\n", param_selection_count);
			info_message("       generation_size     : %d\n", param_generation_size);
			info_message("       max_one_error       : %d\n", param_max_one_loss);
			info_message("       max_abs_error       : %f\n", param_max_abs_loss);
			info_message("       generations         : %d\n", param_generations_count);
			info_message("       mutations_count     : %d\n", param_mutate_center);
			info_message("       mutations_sigma     : %d\n", param_mutate_sigma);
			info_message("       parents             : %d\n", param_parrents_count);
			info_message("       cross_parts         : %d\n", param_cross_parts);
			info_message("       power_accuracy_ratio: %f\n", param_power_accuracy_ratio);
			info_message("       representation      : %s\n", param_repres.c_str());
			info_message("       keep_delay          : %d\n", keep_delay);
			info_message("       l-back              : %d\n\n", param_l_back);

			int inital_delay = repres->chromosome->delay();

			if (!wire_test) {
				std::ofstream debug_indiv_file;

				if (debug_indiv) {
					debug_indiv_file.open("debug.json");
				}

				debug_indiv_to_file(debug_indiv_file, repres);

				auto generation  = new evolution::generation(repres, param_max_one_loss, param_max_abs_loss, param_power_accuracy_ratio, keep_delay ? inital_delay : -1);
				auto parrent0    = generation->add_individual(repres->clone());
				auto timer_start = std::chrono::high_resolution_clock::now();

				//create generation using clone
				for (unsigned i = 0; i < param_generation_size; i++) {
					generation->clone(parrent0);
				}

				generation->mutate(parrent0 + 1, generation->size() - 1, param_mutate_center, param_mutate_sigma, param_l_back);

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

					generation->mutate(param_selection_count, generation->size() - 1, param_mutate_center, param_mutate_sigma, param_l_back);

					debug_generation_to_file(debug_indiv_file, generation, "\n\nGENERATION " + std::to_string(generation_id) + "\n\n");

					//score
					generation->selection(param_selection_count, config);

					info_message("generation %i best individual score %f\n",  generation_id, generation->individuals[0].score);
					
					if (param_profile) {
						auto p_loss = generation->individuals[0].repres->power_loss();
						log("%i-%f-%f-%f;", p_loss, generation->individuals[0].mae, generation->individuals[0].wce, generation->individuals[0].score);
					}

					if (param_max_duration > 0) {
						auto timer_stop   = std::chrono::high_resolution_clock::now();
						auto act_duration = std::chrono::duration_cast<std::chrono::minutes>(timer_stop - timer_start);

						if (act_duration.count() >= param_max_duration) {
							info_message("maximal duration reached\n")
							break;
						}
					}
				}

				repres = generation->individuals[0].repres->clone();
				repres->chromosome->cut_unused();

				debug_indiv_to_file(debug_indiv_file, repres);

				if (save_final_indiv) {
					std::ofstream final_indiv_file;
					final_indiv_file.open(final_file);
					repres->save(final_indiv_file);
					final_indiv_file.close();
				}

				if (debug_indiv) {
					debug_indiv_file.close();
				}

				log("\nAproximation done. Used %d transitors. MAE is %0.2f and WCE is %0.2f. Delay is %d delta", generation->individuals[0].repres->power_loss(), generation->individuals[0].mae, generation->individuals[0].wce,  generation->individuals[0].repres->chromosome->delay());

				delete generation;
			}
			
			genome2design(repres, design);
		} catch( const std::invalid_argument& e ) {
			error_message("Error while loading circuic. Do you run techmap befere? The circuit may be now damaged. Error: %s", e.what());
		}

	}

} cgploss;

PRIVATE_NAMESPACE_END
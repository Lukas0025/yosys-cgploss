/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with generation header
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#pragma once

#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"

#include "genome.h"
#include "config-parse.h"
#include "representation.h"

#include <vector>
#include <algorithm>

namespace evolution {
	
	typedef struct {
		representation::representation* repres;
		float score;
	} individual_t;

	/**
	 * @brief Class reprezenting one generation 
	 */
	class generation {
		public:
			/**
			 * @brief Construct a new generation object
			 * @param reference reference individual with zero loss on error
			 * @param max_one_loss maximal allowed error per one combination
			 * @param max_abs_loss maximal allowed error mean combination
			 * @param power_accuracy_ratio ratio between accurancy and ratio (0..1) 0 means only accurancy only and 1 means only power optimalization
			 */
			generation(representation::representation *reference, unsigned max_one_loss, float max_abs_loss, float power_accuracy_ratio) {
				this->power_accuracy_ratio = power_accuracy_ratio;
				this->reference            = reference;
				this->max_one_loss         = max_one_loss;
				this->max_abs_loss         = max_abs_loss;

				for (auto output: reference->chromosome->wire_out) {
					reference_inverse_wire_out[output.second] = output.first;
				}

			}

			~generation() {
				for (auto individual: this->individuals) {
					delete individual.repres;
				}
			}
			
			/**
			 * @brief Create a kid from two parrents and insert it to generation
			 * @param parrentA index of parrent in generation
			 * @param parrentB index of parrent in generation
			 * @param crossovers vector of crossover points
			 * @param part_spec specification of used parts in kid 0 is A 1 is B Exmaple 0b101 means BAB
			 * @return unsigned index of inserted individual in generation
			 */
			unsigned create_kid(unsigned parrentA, unsigned parrentB, std::vector<genome::io_id_t> &crossovers, unsigned part_spec);

			/**
			 * @brief Cross two individuals
			 * @param parrentA index of parrent in generation
			 * @param parrentB index of parrent in generation
			 * @param parts Number of chromozome parts (crossover points +1)
			 */
			void cross(unsigned parrentA, unsigned parrentB, unsigned parts = 2);

			/**
			 * @brief Add individual reprezentation to generation
			 * @param individual pointer on individual reprezentation
			 * @return unsigned index of inserted individual in generation
			 */
			unsigned add_individual(representation::representation *individual);

			/**
			 * @brief Mutate individuals in generation
			 * @param from Index of first individual to mutate
			 * @param to Index of last individual to mutate
			 * @param center Center of random number of muatation in chromozome
			 * @param sigma Sigma of normal distribution random number of muatation in chromozome
			 * @param l_back range of mutation (current_input +- l_back)
			 */
			void mutate(unsigned from, unsigned to, unsigned center, unsigned sigma, unsigned l_back);

			/**
			 * @brief get size of generation (number of individuals)
			 * @return unsigned 
			 */
			unsigned size();

			/**
			 * @brief Compare two individuals by score
			 * @param a individual
			 * @param b individual
			 * @return boll if a > b
			 */
			static bool sort_individual_score_asc(const individual_t &a, const individual_t &b);

			/**
			 * @brief Score individual with simulation
			 * @param individual reprezenation of individual
			 * @param config_parse config praser with port weights
			 * @return float score
			 */
			float score_individual(representation::representation *individual, config::parse *config_parse);

			/**
			 * @brief replicate individual (copy individual to generation)
			 * @param parrent index of replicated individual in generation
			 * @return unsigned index of inserted individual
			 */
			unsigned clone(unsigned parrent);

			/**
			 * @brief Do selection on generation - score all individuals and remove all individuals what is under count. The best individuals will not be removed.
			 * @param count Count of individuals what will not be removed 
			 * @param config_parse config praser with port weights
			 */
			void selection(unsigned count, config::parse *config_parse);
			
			std::vector<individual_t> individuals;
			std::map<Yosys::RTLIL::SigBit, genome::io_id_t> reference_inverse_wire_out;
			representation::representation *reference;
			unsigned generation_size;
			unsigned max_one_loss;
			float    max_abs_loss;
			float    power_accuracy_ratio;
	};
}
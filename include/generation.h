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

	class generation {
		public:
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
			
			unsigned create_kid(unsigned parrentA, unsigned parrentB, std::vector<genome::io_id_t> &crossovers, unsigned part_spec);
			void cross(unsigned parrentA, unsigned parrentB, unsigned parts = 2);
			unsigned add_individual(representation::representation *individual);
			void mutate(unsigned from, unsigned to, unsigned center, unsigned sigma, unsigned l_back);
			unsigned size();
			static bool sort_individual_score_asc(const individual_t &a, const individual_t &b);
			float score_individual(representation::representation *individual, config::parse *config_parse);
			unsigned clone(unsigned parrent);
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
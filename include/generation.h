#pragma once

#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"

#include "genome.h"
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
			}

			~generation() {
				for (auto individual: this->individuals) {
					delete individual.repres;
				}
			}
			
			unsigned create_kid(unsigned parrentA, unsigned parrentB, std::vector<genome::io_id_t> &crossovers, unsigned part_spec);
			void cross(unsigned parrentA, unsigned parrentB, unsigned parts = 2);
			unsigned add_individual(representation::representation *individual);
			void mutate(unsigned from, unsigned to, unsigned center, unsigned sigma);
			unsigned size();
			static bool sort_individual_score_asc(const individual_t &a, const individual_t &b);
			float score_individual(representation::representation *individual);
			unsigned clone(unsigned parrent);
			void selection(unsigned count);
			
			std::vector<individual_t> individuals;
			representation::representation *reference;
			unsigned generation_size;
			unsigned max_one_loss;
			float    max_abs_loss;
			float    power_accuracy_ratio;
	};
}
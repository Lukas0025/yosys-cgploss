#include "generation.h"
#include <random>

namespace evolution {

	unsigned generation::create_kid(unsigned parrentA, unsigned parrentB, std::vector<genome::io_id_t> &crossovers, unsigned part_spec) {
		auto kid = this->individuals[parrentA]->clone();

		genome::io_id_t last_part = 0;
		unsigned part_mask        = 1;
		for (auto &crossover : crossovers) {
			if (part_spec & part_mask) { //genome from parrent B
				for (genome::io_id_t i = last_part; i <= crossover; i++) {
					kid->chromosome->update_gene(i, this->individuals[parrentB]->chromosome->get_gene(i));
				}
			}

			last_part = crossover + 1;
			part_mask = part_mask << 1;
		}

		if (part_spec & part_mask) { //part of chromosome ofter cross overs
			for (genome::io_id_t i = last_part; i < kid->chromosome->size(); i++) {
				kid->chromosome->update_gene(i, this->individuals[parrentB]->chromosome->get_gene(i));
			}
		}

		//copy outputs
		if (part_spec & part_mask) { //parrent B
			kid->chromosome->wire_out = this->individuals[parrentB]->chromosome->wire_out;
		}

		this->individuals.push_back(kid);
		
		return this->individuals.size() - 1;
	}

	unsigned generation::clone(unsigned parrent) {
		this->individuals.push_back(this->individuals[parrent]->clone());
		return this->individuals.size() - 1;
	}

	void generation::cross(unsigned parrentA, unsigned parrentB, unsigned parts) {
		std::random_device   rd{};
		std::mt19937         rand_gen{rd()};
		std::uniform_int_distribution<genome::io_id_t> rand_pos(this->individuals[parrentA]->chromosome->last_input + 1, this->individuals[parrentA]->chromosome->size());

		//parts random selection
		std::vector<genome::io_id_t> crossovers;
		for (unsigned part = 1; part < parts; part++) {
			crossovers.push_back(rand_pos(rand_gen));
		}

		//order parts by position
		std::sort(crossovers.begin(), crossovers.end());

		//remove duplicate
		crossovers.erase(std::unique(crossovers.begin(), crossovers.end()), crossovers.end());

		//generate kids
		unsigned kids_count = 1 << parts; // 2 ** parts

		for (unsigned kid = 0; kid < kids_count; kid++) {
			this->create_kid(parrentA, parrentB, crossovers, kid);
		}
	}

	bool generation::sort_individual_score_asc(const std::pair<float,void*> &a, const std::pair<float,void*> &b) {
		return (a.first < b.first);
	}

	float generation::score_individual(representation::representation *individual) {

		float loss = 0;

		std::vector<simulation::io_t> xor_outputs(individual->chromosome->wire_out.size());
		std::vector<simulation::io_t> test_circuic(individual->chromosome->size());
		std::vector<simulation::io_t> reference_circuic(this->reference->chromosome->size());
		std::vector<unsigned>         variant_counter(individual->chromosome->last_input + 2);

		for (unsigned i = 0; i <= individual->chromosome->last_input; i++) {
			SET_VARIANTS_BITS(test_circuic[i].vec, i);
			SET_VARIANTS_BITS(reference_circuic[i].vec, i);
		}

		unsigned total_error = 0;
		while(true) {
			individual->simulate(test_circuic);
			this->reference->simulate(reference_circuic);

			unsigned i = 0;
			for (auto output: individual->chromosome->wire_out) {
				xor_outputs[i].vec = test_circuic[output.first].vec ^ reference_circuic[output.first].vec;
				total_error += simulation::bits_count(xor_outputs[xor_outputs.size() - 1]);
				i++;
			}

			if (simulation::one_max_loss(xor_outputs) > this->max_one_loss) {
				loss = INFINITY;
				break;
			} else {
				loss += (1 - this->power_accuracy_ratio) * total_error + this->power_accuracy_ratio * individual->power_loss();
			}

			for (unsigned i = ONE_SIM_VARIANTS; i <= individual->chromosome->last_input; i++) {
				variant_counter[i] = (variant_counter[i] + 1) % (1 << (i - ONE_SIM_VARIANTS));
					
				if (variant_counter[i] == 0) {
					test_circuic[i].vec      = ~test_circuic[i].vec;
					reference_circuic[i].vec = ~reference_circuic[i].vec;

					if (i == individual->chromosome->last_input) {
						if (variant_counter[individual->chromosome->last_input + 1]) {
							break;
						} 

						variant_counter[individual->chromosome->last_input + 1]++;
					}
				}
				continue;
			}
			break;
		}

		return loss / (1 << (individual->chromosome->last_input + 1));
	}

	generation* generation::selection(unsigned count) {
		auto new_generation = new generation(this->reference, this->generation_size, this->max_one_loss, this->power_accuracy_ratio);

		//loss function calculation
		std::vector<std::pair<float,representation::representation*>> scores;

		for (auto individual : this->individuals) {
			scores.push_back({this->score_individual(individual), individual});
		}

		std::sort(scores.begin(), scores.end(), generation::sort_individual_score_asc);

		for (unsigned id = 0; id < count; id++) {
			new_generation->add_individual(scores[id].second);
		}

		return new_generation;
	}

	unsigned generation::add_individual(representation::representation *individual) {
		this->individuals.push_back(individual);

		return this->individuals.size() - 1;
	}

	unsigned generation::size() {
		return this->individuals.size();
	}

	void generation::mutate(unsigned from, unsigned to, unsigned center, unsigned sigma) {
		for (unsigned i = from; i <= to; i++) {
			this->individuals[i]->mutate(center, sigma);
		}
	}

}
/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with generation implementation
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */


#include "generation.h"
#include <random>
#include <algorithm>

namespace evolution {

	unsigned generation::create_kid(unsigned parrentA, unsigned parrentB, std::vector<genome::io_id_t> &crossovers, unsigned part_spec) {
		auto kid = this->individuals[parrentA].repres->clone();

		genome::io_id_t last_part = 0;
		unsigned part_mask        = 1;
		for (auto &crossover : crossovers) {
			if (part_spec & part_mask) { //genome from parrent B
				for (genome::io_id_t i = last_part; i <= crossover; i++) {
					kid->chromosome->update_gene(i, this->individuals[parrentB].repres->chromosome->get_gene(i));
				}
			}

			last_part = crossover + 1;
			part_mask = part_mask << 1;
		}

		if (part_spec & part_mask) { //part of chromosome ofter cross overs
			for (genome::io_id_t i = last_part; i < kid->chromosome->size(); i++) {
				kid->chromosome->update_gene(i, this->individuals[parrentB].repres->chromosome->get_gene(i));
			}
		}

		//copy outputs
		if (part_spec & part_mask) { //parrent B
			kid->chromosome->wire_out = this->individuals[parrentB].repres->chromosome->wire_out;
		}

		this->add_individual(kid);
		
		return this->individuals.size() - 1;
	}

	unsigned generation::clone(unsigned parrent) {
		this->add_individual(this->individuals[parrent].repres->clone());
		return this->individuals.size() - 1;
	}

	void generation::cross(unsigned parrentA, unsigned parrentB, unsigned parts) {
		std::random_device   rd{};
		std::mt19937         rand_gen{rd()};
		std::uniform_int_distribution<genome::io_id_t> rand_pos(this->individuals[parrentA].repres->chromosome->last_input + 1, this->individuals[parrentA].repres->chromosome->size() - 1);

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

	bool generation::sort_individual_score_asc(const individual_t &a, const individual_t &b) {
		return (a.score < b.score);
	}

	float generation::score_individual(representation::representation *individual, config::parse *config_parse) {

		std::vector<simulation::io_t> xor_outputs(individual->chromosome->wire_out.size());
		std::vector<simulation::io_t> test_circuic(individual->chromosome->size());
		std::vector<simulation::io_t> reference_circuic(this->reference->chromosome->size());
		std::vector<unsigned>         variant_counter(individual->chromosome->last_input + 2);

		for (unsigned i = 0; i <= individual->chromosome->last_input; i++) {
			SET_VARIANTS_BITS(test_circuic[i].vec, i);
			SET_VARIANTS_BITS(reference_circuic[i].vec, i);
		}

		unsigned total_error = 0;
		bool done = false;

		if (ONE_SIM_VARIANTS > individual->chromosome->last_input) {
			done = true;
		}

		do {
			individual->simulate(test_circuic);
			this->reference->simulate(reference_circuic);

			unsigned i = 0;
			for (auto output: individual->chromosome->wire_out) {
				xor_outputs[i].vec = test_circuic[output.first].vec ^ reference_circuic[this->reference_inverse_wire_out[output.second]].vec;
				total_error += simulation::bits_count(xor_outputs[i]) * config_parse->port_weight(output.second);
				i++;
			}

			if (simulation::one_max_loss(xor_outputs, individual->chromosome->wire_out, config_parse) > this->max_one_loss) {
				return INFINITY;
			}

			for (unsigned i = ONE_SIM_VARIANTS; i <= individual->chromosome->last_input; i++) {
				variant_counter[i] = (variant_counter[i] + 1) % (1 << (i - ONE_SIM_VARIANTS));
					
				if (variant_counter[i] == 0) {
					test_circuic[i].vec      = ~test_circuic[i].vec;
					reference_circuic[i].vec = ~reference_circuic[i].vec;

					if (i == individual->chromosome->last_input) {
						if (variant_counter[individual->chromosome->last_input + 1]) {
							done = true;
						} 

						variant_counter[individual->chromosome->last_input + 1]++;
					}
				}
			}
		} while (!done);

		unsigned variants_count = 1 << (individual->chromosome->last_input + 1);
		float    abs_error      = (float) total_error / variants_count;

		if (abs_error > this->max_abs_loss) {
			return INFINITY;
		}

		return (1 - this->power_accuracy_ratio) * abs_error + this->power_accuracy_ratio * individual->power_loss();
	}

	void generation::selection(unsigned count, config::parse *config_parse) {

		#pragma omp parallel for
		for (unsigned i = 0; i < this->individuals.size(); i++) {
			if (!this->individuals[i].repres->chromosome->valid()) {
				this->individuals[i].score = INFINITY;
				continue;
			}

			this->individuals[i].score = this->score_individual(this->individuals[i].repres, config_parse);
		}

		std::sort(this->individuals.begin(), this->individuals.end(), generation::sort_individual_score_asc);

		for (unsigned id = count; id < this->individuals.size(); id++) {
			delete this->individuals[id].repres;
		}

		this->individuals.erase(this->individuals.begin() + count, this->individuals.end());
	}

	unsigned generation::add_individual(representation::representation *individual) {
		this->individuals.push_back({individual, 0});

		return this->individuals.size() - 1;
	}

	unsigned generation::size() {
		return this->individuals.size();
	}

	void generation::mutate(unsigned from, unsigned to, unsigned center, unsigned sigma, unsigned l_back) {
		for (unsigned i = from; i <= to; i++) {
			this->individuals[i].repres->mutate(center, sigma, l_back);
		}
	}

}
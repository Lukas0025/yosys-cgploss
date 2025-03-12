/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with generation implementation
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */


#include "generation.h"
#include "types.h"
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

	void generation::score_individual(unsigned index, config::parse *config_parse) {

		this->individuals[index].mae = 0;
		this->individuals[index].wce = 0;
		types::Exponent variants_count(TO_REAL_INPUT(this->individuals[index].repres->chromosome->last_input + 1));
		unsigned        variants_count_sim  = (ONE_SIM_VARIANTS >= variants_count.exp) ? 1 <<  variants_count.exp : 1 << ONE_SIM_VARIANTS;

		std::vector<simulation::io_t> xor_outputs(this->individuals[index].repres->chromosome->wire_out.size());
		std::vector<simulation::io_t> test_circuic(this->individuals[index].repres->chromosome->size());
		std::vector<simulation::io_t> reference_circuic(this->reference->chromosome->size());
		std::vector<unsigned>         variant_counter(this->individuals[index].repres->chromosome->last_input + 2);

		// set const inputs
		SET_VECTOR_TO(test_circuic[0].vec,      VARIANTS_BITS_ALL0);
		SET_VECTOR_TO(test_circuic[1].vec,      VARIANTS_BITS_ALL1);
		SET_VECTOR_TO(reference_circuic[0].vec, VARIANTS_BITS_ALL0);
		SET_VECTOR_TO(reference_circuic[1].vec, VARIANTS_BITS_ALL1);

		// set varaible inputs
		for (unsigned i = TO_GENOME_INPUT(0); i <= this->individuals[index].repres->chromosome->last_input; i++) {
			SET_VARIANTS_BITS(test_circuic[i].vec, TO_REAL_INPUT(i));
			SET_VARIANTS_BITS(reference_circuic[i].vec, TO_REAL_INPUT(i));
		}

		float total_error = 0;
		bool done = false;

		if (ONE_SIM_VARIANTS >= variants_count.exp) {
			done = true;
		}

		do {
			this->individuals[index].repres->simulate(test_circuic);
			this->reference->simulate(reference_circuic);

			//Calc error using xor
			unsigned i = 0;
			for (auto output: this->individuals[index].repres->chromosome->wire_out) {
				xor_outputs[i].vec = test_circuic[output.first].vec ^ reference_circuic[this->reference_inverse_wire_out[output.second]].vec;
				total_error += simulation::bits_count(xor_outputs[i], variants_count_sim) * config_parse->port_weight(output.second);
				i++;
			}

			//WCE
			auto current_wce = simulation::one_max_loss(xor_outputs, this->individuals[index].repres->chromosome->wire_out, config_parse);
			if (this->individuals[index].wce < current_wce) {
				this->individuals[index].wce = current_wce;

				if (current_wce  > this->max_one_loss) {
					this->individuals[index].score = INFINITY;
					return;
				}
			}

			//DELAY
			if (this->max_delay > -1) {
				auto current_delay = this->individuals[index].repres->chromosome->delay();
				if (current_delay > this->max_delay) {
					this->individuals[index].score = INFINITY;
					return;
				}
			}

			//Update inputs for next simulation
			for (unsigned i = ONE_SIM_VARIANTS; i < TO_REAL_INPUT(this->individuals[index].repres->chromosome->last_input + 1); i++) {
				variant_counter[i] = (variant_counter[i] + 1) % (1 << (i - ONE_SIM_VARIANTS));
					
				if (variant_counter[i] == 0) {
					test_circuic[TO_GENOME_INPUT(i)].vec      = ~test_circuic[TO_GENOME_INPUT(i)].vec;
					reference_circuic[TO_GENOME_INPUT(i)].vec = ~reference_circuic[TO_GENOME_INPUT(i)].vec;

					if (TO_GENOME_INPUT(i) == this->individuals[index].repres->chromosome->last_input) {
						if (variant_counter[TO_REAL_INPUT(this->individuals[index].repres->chromosome->last_input + 1)]) {
							done = true;
						} 

						variant_counter[TO_REAL_INPUT(this->individuals[index].repres->chromosome->last_input + 1)]++;
					}
				}
			}
		} while (!done);

		this->individuals[index].mae = total_error / variants_count;

		if (this->individuals[index].mae > this->max_abs_loss) {
			this->individuals[index].score = INFINITY;
		} else {
			this->individuals[index].score = (1 - this->power_accuracy_ratio) * this->individuals[index].mae + this->power_accuracy_ratio * this->individuals[index].repres->power_loss();
		}
	}

	void generation::selection(unsigned count, config::parse *config_parse) {

		#pragma omp parallel for
		for (unsigned i = 0; i < this->individuals.size(); i++) {

			#ifndef PERFORMANCE
				if (!this->individuals[i].repres->chromosome->valid()) {
					this->individuals[i].score = INFINITY;
					continue;
				}
			#endif

			this->score_individual(i, config_parse);
		}

		std::sort(this->individuals.begin(), this->individuals.end(), generation::sort_individual_score_asc);

		for (unsigned id = count; id < this->individuals.size(); id++) {
			delete this->individuals[id].repres;
		}

		this->individuals.erase(this->individuals.begin() + count, this->individuals.end());
	}

	unsigned generation::add_individual(representation::representation *individual) {
		this->individuals.push_back({individual, 0, 0, 0});

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
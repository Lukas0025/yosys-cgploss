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

	/*generation generation::selection(representation::representation *reference, unsigned count) {
		auto new_generation = new generation();

		//todo: add loss function here

		for (unsigned id = 0; id < cont; id++) {
			//new_generation->add_individual();
		}

		return new_generation;
	}*/

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
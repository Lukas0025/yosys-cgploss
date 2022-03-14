#include "generation.h"

#define parrent_by_num(id, parrentA, parrentB) (id == 0 ? parrentA : parrentB)

namespace evolution {

	void generation::create_kid(unsigned parrentA, unsigned parrentB, std::vector<genome::io_id_t> &parts_index, unsigned part_spec) {
		auto kid = new representation::representation(this.individuals[parrentA]);

		genome::io_id_t last_part = 0;
		unsigned part_mask = 1;
		for (auto &part : parts_index) {
			for (genome::io_id_t i = last_part; i <= part; i++) {
				kid.chromosome.update_gene(i, this.individuals[parrent_by_num(part_spec & part_mask, parrentA, parrentB)].chromosome.get_gene())
			}

			last_part = part + 1;
			part_mask = part_mask << 1;
		}

		this.individuals.push_back(kid);
	}

	void generation::cross(unsigned parrentA, unsigned parrentB, unsigned parts = 2) {
		std::random_device   rd{};
		std::mt19937         rand_gen{rd()};
		std::uniform_int_distribution<genome::io_id_t> rand_pos(0, this.individuals[parrentA].size());

		//parts random selection
		std::vector<genome::io_id_t> parts_index;
		for (unsigned part = 1; part < parts; i++) {
			parts_index.push_back(rand_pos(rand_gen));
		}

		//order parts by position
		std::sort(parts_index.begin(), parts_index.end());

		//remove duplicate
		parts_index.erase(std::unique(parts_index.begin(), parts_index.end()), parts_index.end());

		//generate kids
		unsigned kids_count = 1 << parts // 2 ** parts

		for (unsigned kid = 1; kid < kids_count - 1; kid++) {
			this.create_kid(parrentA, parrentB, parts_index, kid);
		}
	}

	generation generation::selection(representation::representation *reference, unsigned count) {
		auto new_generation = new generation();

		//todo: add loss function here

		for (unsigned id = 0; id < cont; id++) {
			//new_generation->add_individual();
		}

		return new_generation;
	}

	unsigned generation::add_individual(representation::representation *individual) {
		this->individuals.push_back(individual);
	}
}
/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with chromosome implementation
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#include "genome.h"
#include <random>

namespace genome {
	genome::genome() {
		this->chromosome.clear();
		//insert logic 1 and 0 to inputs
		this->add_dummy_gene(); //log 0
		this->add_dummy_gene(); //log 1
	}

	genome* genome::clone() {
		auto chromosome = new genome();

		chromosome->chromosome = this->chromosome;
		chromosome->wire_out   = this->wire_out;
		chromosome->wire_in    = this->wire_in;
		chromosome->last_input = this->last_input;

		return chromosome;
	}

	void genome::swap_genes(io_id_t id_a, io_id_t id_b) {

		for(auto gene = this->chromosome.begin(); gene < this->chromosome.end(); gene++) {

			for (unsigned i = 0; i < this->gene_inputs_count; i++) {
				if (gene->Inputs[i] == id_a) {
					gene->Inputs[i] = id_b;
				} else if (gene->Inputs[i] == id_b) {
					gene->Inputs[i] = id_a;
				}
			}

		}

		std::swap(this->chromosome[id_a], this->chromosome[id_b]);
	}

	bool genome::valid() {

		io_id_t id = this->last_input + 1;
		while (id < this->size()) {
			id++;
		}

		return true;
	}

	unsigned genome::mutate(unsigned center, unsigned sigma, uint16_t type_min, uint16_t type_max) {
		std::random_device                     rd{};
		std::mt19937                           rand_gen{rd()};
		std::normal_distribution<>             rand_numof_muts(center, sigma);
		std::uniform_int_distribution<io_id_t> rand_mut_pos(this->last_input + 1, this->size() + this->wire_out.size() - 1);
		std::uniform_int_distribution<io_id_t> rand_type(type_min, type_max);

		int mutations = std::round(rand_numof_muts(rand_gen));
		if (mutations <= 0) return 0; //no mutations

		for (int mutation = 0; mutation < mutations; mutation++) {
			io_id_t rand_pos = rand_mut_pos(rand_gen);

			if (rand_pos >= this->size()) { //output mutation
				//select random output
				io_id_t i = 0;
				for (auto output = this->wire_out.begin(); output != this->wire_out.end(); output++) {

					if ((rand_pos - this->size()) == i) {
						//generate random for output
						std::uniform_int_distribution<io_id_t> rand_mut_outputs(this->last_input + 1, this->size() - 1);
						auto rand_out = rand_mut_outputs(rand_gen);

						if (this->wire_out.count(rand_out)) {
							std::swap(this->wire_out.at(rand_out), this->wire_out.at(output->first));
						} else {
							this->wire_out[rand_out] = output->second;
							this->wire_out.erase(output->first);
						}

						break;
					}

					i++;
				}
				
			} else { //regular gene
				auto mut_gene = this->get_gene_ptr(rand_pos);
				
				//generate random for gene inputs
				std::uniform_int_distribution<io_id_t> rand_mut_inputs(0, rand_pos - 1);

				for (unsigned i = 0; i < this->gene_inputs_count; i++) {
					mut_gene->Inputs[i] = rand_mut_inputs(rand_gen);
				}

				//type mutation
				mut_gene->type = rand_type(rand_gen);
			}
		}

		return mutations;
	}

	io_id_t genome::add_gene(gene_t gene) {
		this->chromosome.push_back(gene);

		return this->size() - 1;
	}

	io_id_t genome::add_dummy_gene() {
		gene_t dummy_gene;

		dummy_gene.type = DUMMY_GENE_TYPE;

		for (unsigned i = 0; i < this->gene_inputs_count; i++) {
			dummy_gene.Inputs[i] = IO_ID_T_UNUSED;
		}

		return this->add_gene(dummy_gene);
	}

	unsigned genome::size() {
		return this->chromosome.size();
	}

	void genome::update_gene(io_id_t pos, gene_t gene) {
		this->chromosome[pos] = gene;
	}

	bool genome::is_gene_ins_eqbelow(io_id_t pos, io_id_t threshold) {

		for (unsigned i = 0; i < this->gene_inputs_count; i++) {
			if (this->chromosome[pos].Inputs[i] > threshold) return false;
		}

		return true;
	}

	bool genome::sort_asc_by_ins() {
		io_id_t id             = this->last_input + 1;
		io_id_t only_inputs_to = this->last_input;

		while (id < this->size()) {
			io_id_t to_swap;
			bool    do_swap = false;

			for (auto i = id; i < this->size(); i++) {
				if (this->is_gene_ins_eqbelow(i, only_inputs_to)) {
					to_swap = i;
					do_swap = true;
					break;
				}
			}

			if (!do_swap) {
				if (id > (only_inputs_to + 1)) {
					only_inputs_to++;
					continue;
				}

				//same cell have as input own output or input is from gate after this gate (cycle)
				return false;
			}

			this->swap_genes(id, to_swap);

			//outputs mapping fix
			if (this->wire_out.count(id) && this->wire_out.count(to_swap)) {

				std::swap(this->wire_out.at(id), this->wire_out.at(to_swap));

			} else if (this->wire_out.count(id)) {

				this->wire_out[to_swap] = this->wire_out[id];
				this->wire_out.erase(id);

			} else if (this->wire_out.count(to_swap)) {

				this->wire_out[id] = this->wire_out[to_swap];
				this->wire_out.erase(to_swap);

			}
			
			id++;
		}

		return true;
	}

	unsigned genome::used_cost() {
		unsigned cost = 0;

		std::stack<io_id_t> stack;
		for (auto output: this->wire_out) {
			stack.push(output.first);
		}

		while (!stack.empty()) {
			auto gene_id = stack.top();
			stack.pop();

			if (gene_id > this->last_input) {
				auto gene = this->get_gene_ptr(gene_id);
					
				for (unsigned i = 0; i < this->gene_inputs_count; i++) {
					stack.push(gene->Inputs[i]);
				}

				cost += 1;
			}
		}

		return cost;
	}

	void genome::cut_unused() {
		std::stack<io_id_t> stack;
		std::vector<bool>   used(this->chromosome.size());
		std::fill(used.begin(), used.end(), false);

		for (auto output: this->wire_out) {
			stack.push(output.first);
		}

		while (!stack.empty()) {
			auto gene_id = stack.top();
			stack.pop();

			if (gene_id > this->last_input) {
				auto gene = this->get_gene_ptr(gene_id);
					
				for (unsigned i = 0; i < this->gene_inputs_count; i++) {
					stack.push(gene->Inputs[i]);
				}

				used[gene_id] = true;
			}
		}

		for (unsigned i = this->last_input + 1; i < used.size(); i++) {
			if (!used[i]) {
				this->swap_genes(i, this->chromosome.size() - 1);
				this->chromosome.erase(this->chromosome.end() - 1);
				used.erase(used.begin() + i);
				i--;
			}
		}


	}

	bool genome::order(std::map<io_id_t, Yosys::RTLIL::SigBit> inputs, std::map<io_id_t, Yosys::RTLIL::SigBit> outputs) {
		//set I/Os
		this->wire_in.clear();
		this->wire_out = outputs;

		//fisrt inputs on top
		io_id_t id = 2; // for const inputs
		while (inputs.size()) {

			auto input = inputs.begin();

			this->swap_genes(input->first, id);
			this->wire_in[id] = input->second;

			//swap in inputs
			if (inputs.count(id)) {
				inputs.erase(id);
			} else {
				inputs.erase(input->first);
			}

			//swap in outputs
			if (this->wire_out.count(id) && this->wire_out.count(input->first)) {

				std::swap(this->wire_out.at(id), this->wire_out.at(input->first));

			} else if (this->wire_out.count(id)) {

				this->wire_out[input->first] = this->wire_out[id];
				this->wire_out.erase(id);

			} else if (this->wire_out.count(input->first)) {

				this->wire_out[id] = this->wire_out[input->first];
				this->wire_out.erase(input->first);

			}

			id++;
		}

		this->last_input = id - 1;

		return this->sort_asc_by_ins();
	}

	std::string genome::gene_str(gene_t gene) {
		std::string res = " ";
		
		for (unsigned i = 0; i < this->gene_inputs_count; i++) {
			res += std::to_string(gene.Inputs[i]) + ",";
		}

		res.pop_back();

		return res;
	}

	std::string genome::to_string() {
		std::string  output = "\t\"chromosome\": [";

		for (unsigned i = this->last_input + 1; i < this->size(); i++) {
			auto gene = this->chromosome[i];
			output += "\n\t\t[" + std::to_string(gene.type) + "," + this->gene_str(gene) + "],";
		}

		if (this->last_input + 1 < this->size()) { //for 0 gates
			output.pop_back();
		}

		return output + "\n\t]";
	}

	std::string genome::raw_string() {
		
		std::string  output = "[";

		for (unsigned i = 0; i < this->size(); i++) {
			auto gene = this->chromosome[i];
			output += "[" + std::to_string(gene.type) + "," + this->gene_str(gene) + "],";
		}

		if (this->last_input + 1 < this->size()) { //for 0 gates
			output.pop_back();
		}

		return output + "]";

	}

	gene_t genome::get_gene(io_id_t id) {
		return this->chromosome[id];
	}

	gene_t* genome::get_gene_ptr(io_id_t id) {
		return &(this->chromosome[id]);
	}
}
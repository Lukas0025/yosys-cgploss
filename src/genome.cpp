/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with chromosome implementation
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#include "genome.h"

namespace genome {
	genome::genome() {
		this->chromosome.clear();
		//insert logic 1 and 0 to inputs
		this->add_dummy_gene(); //log 0
		this->add_dummy_gene(); //log 1
	}

	void genome::swap_genes(io_id_t id_a, io_id_t id_b) {

		for(auto gene = this->chromosome.begin(); gene < this->chromosome.end(); gene++) {
			if (gene->I1 == id_a) {
				gene->I1 = id_b;
			} else if (gene->I1 == id_b) {
				gene->I1 = id_a;
			}

			if (gene->I2 == id_a) {
				gene->I2 = id_b;
			} else if (gene->I2 == id_b) {
				gene->I2 = id_a;
			}
		}

		std::swap(this->chromosome[id_a], this->chromosome[id_b]);
	}

	io_id_t genome::add_gene(gene_t gene) {
		this->chromosome.push_back(gene);

		return this->size() - 1;
	}

	void genome::add_dummy_gene() {
		gene_t dummy_gene;

		dummy_gene.type = DUMMY_GENE_TYPE;
		dummy_gene.I1   = 999999999;
		dummy_gene.I2   = 999999999;

		this->add_gene(dummy_gene);
	}

	unsigned genome::size() {
		return this->chromosome.size();
	}

	void genome::update_gene(io_id_t pos, gene_t gene) {
		this->chromosome[pos] = gene;
	}

	bool genome::order(std::map<io_id_t, Yosys::RTLIL::SigBit> inputs, std::map<io_id_t, Yosys::RTLIL::SigBit> outputs) {
		//clear input/output maps
		this->wire_in.clear();
		this->wire_out.clear();

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
			if (outputs.count(id) && outputs.count(input->first)) {

				std::swap(outputs.at(id), outputs.at(input->first));

			} else if (outputs.count(id)) {

				outputs[input->first] = outputs[id];
				outputs.erase(id);

			} else if (outputs.count(input->first)) {

				outputs[id] = outputs[input->first];
				outputs.erase(input->first);

			}

			id++;
		}

		this->last_input = id - 1;

		//now sort others gates
		io_id_t only_inputs_to = this->last_input;
		while (id < this->size()) {

			io_id_t to_swap;
			bool    do_swap = false;
			for (auto i = id; i < this->size(); i++) {
				if (this->chromosome[i].I1 <= only_inputs_to && 
				    this->chromosome[i].I2 <= only_inputs_to) {
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

				//same cell have as input own output
				return false;
			}

			this->swap_genes(id, to_swap);

			//outputs mapping fix
			if (outputs.count(id) && outputs.count(to_swap)) {

				std::swap(outputs.at(id), outputs.at(to_swap));

			} else if (outputs.count(id)) {

				outputs[to_swap] = outputs[id];
				outputs.erase(id);

			} else if (outputs.count(to_swap)) {

				outputs[id] = outputs[to_swap];
				outputs.erase(to_swap);

			}
			
			id++;
		}

		//set outputs wires
		for (auto output = outputs.begin(); output != outputs.end(); output++) {
			this->wire_out[output->first] = output->second;
		}

		return true;
	}

	std::string genome::to_string() {
		std::string  output = "\t\"chromosome\": [";

		for (unsigned i = this->last_input + 1; i < this->size(); i++) {
			auto gene = this->chromosome[i];
			output += "\n\t\t[" + std::to_string(gene.type) + "," + std::to_string(gene.I1) + "," + std::to_string(gene.I2) + "],";
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
			output += "[" + std::to_string(gene.type) + "," + std::to_string(gene.I1) + "," + std::to_string(gene.I2) + "],";
		}

		if (this->last_input + 1 < this->size()) { //for 0 gates
			output.pop_back();
		}

		return output + "]";

	}

	gene_t genome::get_gene(io_id_t id) {
		return this->chromosome[id];
	}
}
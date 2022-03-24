/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * main file of program
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

//YOSYS
#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"

//CGP
#include "genome.h"
#include "generation.h"

#include <string>
#include <vector>
#include <stdio.h>

#include <iostream>
#include <fstream>

//representations
#include "aig.h"

#define individual_chromosome(X) generation0->individuals[X]->chromosome
#define SIGBIT(id) Yosys::RTLIL::SigBit((RTLIL::State) id)

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

void assert(std::string name, bool exp) {
    if (!exp) {
        log("Error for %s\n", name.c_str());
        exit(1);
    }
}

bool not_same_gene(genome::gene_t output, genome::gene_t expected) {
	return output.type != expected.type || output.Inputs[0] != expected.Inputs[0] || output.Inputs[2] != expected.Inputs[2];
}

struct cgploss : public Pass {
	cgploss() : Pass("cgploss") {}

	void help() override {
		log("\n");
		log("    cgploss test unit\n");
		log("\n");
	}

	void execute(vector<string> params, Design* design) override {

		std::map<genome::io_id_t, Yosys::RTLIL::SigBit> in, outA, outB;

        auto chromosomeA = new genome::genome();
		auto chromosomeB = new genome::genome();
        auto parrentA    = new representation::aig(chromosomeA);
		auto parrentB    = new representation::aig(chromosomeB);

        //circuic schema

        auto a = chromosomeA->add_dummy_gene(); //INPUT A
		chromosomeB->add_dummy_gene();          //INPUT A

        auto b = chromosomeA->add_dummy_gene(); //INPUT B
        chromosomeB->add_dummy_gene();          //INPUT B

		in[a] = SIGBIT(a);
		in[b] = SIGBIT(b);
        
        auto gate0 = parrentA->add_aig_gate(0b000, a, b);
		auto gate1 = parrentA->add_aig_gate(0b000, a, b);
		auto gate2 = parrentA->add_aig_gate(0b000, gate1, b);
		auto gate3 = parrentA->add_aig_gate(0b000, a, gate2);

		gate0 = parrentB->add_aig_gate(0b111, a, b);
        gate1 = parrentB->add_aig_gate(0b001, gate0, b);
		gate2 = parrentB->add_aig_gate(0b010, b, gate1);
		gate3 = parrentB->add_aig_gate(0b001, a, b);

		outA[gate2] = SIGBIT(gate2);
		outB[gate3] = SIGBIT(gate3);

		chromosomeA->order(in, outA);
		chromosomeB->order(in, outB);

        //create generation
		auto generation0 = new evolution::generation(parrentA, 0, 0, 0);

		auto IA = generation0->add_individual(parrentA);
		auto IB = generation0->add_individual(parrentB);

		//create kids
		generation0->cross(IA, IB, 4);

		log("generation size: %d\n", generation0->size());

		assert("generation > 2",   generation0->size() > 2);
		assert("generation <= 18", generation0->size() < 19);

		//is genes from parrents
		for (unsigned kid = IB + 1; kid < generation0->size(); kid++) {
			
			assert("kid have same size chromozome",  individual_chromosome(IA)->size() == individual_chromosome(kid)->size());

			for (genome::io_id_t i = individual_chromosome(IA)->last_input + 1; i < individual_chromosome(IA)->size(); i++) {
				if (not_same_gene(individual_chromosome(kid)->get_gene(i), individual_chromosome(IA)->get_gene(i)) &&
				    not_same_gene(individual_chromosome(kid)->get_gene(i), individual_chromosome(IB)->get_gene(i))) {
						log("kid have gene from parrent");
						exit(1);
				}

				if (individual_chromosome(kid)->wire_out != individual_chromosome(IA)->wire_out &&
				    individual_chromosome(kid)->wire_out != individual_chromosome(IB)->wire_out) {
						log("kid have output from parrent");
						exit(1);
				}
			}
		}

        exit(0);
    }
} cgploss;

PRIVATE_NAMESPACE_END
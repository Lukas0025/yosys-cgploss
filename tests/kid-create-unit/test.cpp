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

void assert_gene(std::string name, genome::gene_t output, genome::gene_t expected) {
    if (output.type != expected.type || output.Inputs[0] != expected.Inputs[0] || output.Inputs[2] != expected.Inputs[2]) {
        log("Error for %s expected %d, %d, %d returned %d, %d, %d\n", name.c_str(), expected.type, expected.Inputs[0], expected.Inputs[1], output.type, output.Inputs[0], output.Inputs[1]);
        exit(1);
    }
}

void nassert_gene(std::string name, genome::gene_t output, genome::gene_t expected) {
    if (output.type == expected.type && output.Inputs[0] == expected.Inputs[0] && output.Inputs[1] == expected.Inputs[1]) {
        log("Error for %s not expected %d, %d, %d returned %d, %d, %d\n", name.c_str(), expected.type, expected.Inputs[0], expected.Inputs[1], output.type, output.Inputs[0], output.Inputs[1]);
        exit(1);
    }
}

void assert_map(std::string name, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> output, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> expected) {
    if (output != expected) {
        log("Error for %s\n", name.c_str());
        exit(1);
    }
}

void nassert_map(std::string name, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> output, std::map<genome::io_id_t, Yosys::RTLIL::SigBit> expected) {
    if (output == expected) {
        log("Error for %s\n", name.c_str());
        exit(1);
    }
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

		//create kid0

		std::vector<genome::io_id_t> crossovers;

		crossovers.push_back(gate1);
		crossovers.push_back(gate2);

	    //                                                       UABA
		auto kid = generation0->create_kid(IA, IB, crossovers, 0b1010);

		assert_gene("kid 0 gene 0", individual_chromosome(kid)->get_gene(gate0), individual_chromosome(IA)->get_gene(gate0));
		assert_gene("kid 0 gene 1", individual_chromosome(kid)->get_gene(gate1), individual_chromosome(IA)->get_gene(gate1));
		assert_gene("kid 0 gene 2", individual_chromosome(kid)->get_gene(gate2), individual_chromosome(IB)->get_gene(gate2));
		assert_gene("kid 0 gene 3", individual_chromosome(kid)->get_gene(gate3), individual_chromosome(IA)->get_gene(gate3));
		assert_map ("kid 0 gene output", individual_chromosome(kid)->wire_out, individual_chromosome(IA)->wire_out);

		//create kid1
	    //                                                        UBBA
		auto kid1 = generation0->create_kid(IA, IB, crossovers, 0b0110);

		assert_gene("kid 1 gene 0", individual_chromosome(kid1)->get_gene(gate0), individual_chromosome(IA)->get_gene(gate0));
		assert_gene("kid 1 gene 1", individual_chromosome(kid1)->get_gene(gate1), individual_chromosome(IA)->get_gene(gate1));
		assert_gene("kid 1 gene 2", individual_chromosome(kid1)->get_gene(gate2), individual_chromosome(IB)->get_gene(gate2));
		assert_gene("kid 1 gene 3", individual_chromosome(kid1)->get_gene(gate3), individual_chromosome(IB)->get_gene(gate3));
		assert_map ("kid 1 gene output", individual_chromosome(kid1)->wire_out, individual_chromosome(IB)->wire_out);

		log("PA> %s\n", individual_chromosome(IA)->raw_string().c_str());
		log("PB> %s\n", individual_chromosome(IB)->raw_string().c_str());
		log("K1> %s\n", individual_chromosome(kid)->raw_string().c_str());
		log("K2> %s\n", individual_chromosome(kid1)->raw_string().c_str());

		//test notsame (deep copy test)
		nassert_gene("parrents gene 0", individual_chromosome(IA)->get_gene(gate0), individual_chromosome(IB)->get_gene(gate0));
		nassert_gene("parrents gene 1", individual_chromosome(IA)->get_gene(gate1), individual_chromosome(IB)->get_gene(gate1));
		nassert_gene("parrents gene 2", individual_chromosome(IA)->get_gene(gate2), individual_chromosome(IB)->get_gene(gate2));
		nassert_gene("parrents gene 3", individual_chromosome(IA)->get_gene(gate3), individual_chromosome(IB)->get_gene(gate3));
		nassert_map ("parrents gene output", individual_chromosome(IA)->wire_out, individual_chromosome(IB)->wire_out);


		nassert_gene("kids gene 3",      individual_chromosome(kid1)->get_gene(gate3), individual_chromosome(kid)->get_gene(gate3));
		nassert_map ("kids gene output", individual_chromosome(kid1)->wire_out,        individual_chromosome(kid)->wire_out);
		
		//kids same part test
		assert_gene("kids gene 0", individual_chromosome(kid1)->get_gene(gate0), individual_chromosome(kid)->get_gene(gate0));
		assert_gene("kids gene 1", individual_chromosome(kid1)->get_gene(gate1), individual_chromosome(kid)->get_gene(gate1));
		assert_gene("kids gene 2", individual_chromosome(kid1)->get_gene(gate2), individual_chromosome(kid)->get_gene(gate2));

        exit(0);
    }
} cgploss;

PRIVATE_NAMESPACE_END
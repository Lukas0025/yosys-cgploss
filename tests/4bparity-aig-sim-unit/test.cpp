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

#include <string>
#include <vector>
#include <stdio.h>

#include <iostream>
#include <fstream>

//representations
#include "aig.h"

#define NEW_GATE chromosome->add_dummy_gene()
#define SIGBIT(id) Yosys::RTLIL::SigBit((RTLIL::State) id)

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

genome::io_id_t io_id_of(genome::io_id_t pre, genome::genome *chromosome) {

	//map inputs
	for (auto input = chromosome->wire_in.begin(); input != chromosome->wire_in.end(); input++) {
		if (input->second == (RTLIL::State) pre) return input->first;
	}

	//map outputs
	for (auto output = chromosome->wire_out.begin(); output != chromosome->wire_out.end(); output++) {
		if (output->second == (RTLIL::State) pre) return output->first;
	}

	return 0;
}

void assert(std::string name, uint8_t output, uint8_t expected) {
    if ((output & 255) != (expected & 255)) {
        log("Error for %s expected %x returned %x\n", name.c_str(), expected, output);
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

		std::map<genome::io_id_t, Yosys::RTLIL::SigBit> in, out;

        auto chromosome = new genome::genome();
        auto circuic    = new representation::aig(chromosome);

        //circuic schema

        auto a = chromosome->add_dummy_gene(); //INPUT A
        auto b = chromosome->add_dummy_gene(); //INPUT B
        auto c = chromosome->add_dummy_gene(); //INPUT C
		auto d = chromosome->add_dummy_gene(); //INPUT D

		in[a] = SIGBIT(a);
		in[b] = SIGBIT(b);
		in[c] = SIGBIT(c);
		in[d] = SIGBIT(d);
        
        std::vector<genome::io_id_t> inputs;
        inputs.push_back(a);
        inputs.push_back(b);

        auto xor_ab = circuic->add_cell(ID($_XOR_), inputs, NEW_GATE);

        inputs[0]   = c;
		inputs[1]   = d;
        auto xor_cd = circuic->add_cell(ID($_XOR_), inputs, NEW_GATE);

        inputs[0]   = xor_ab;
        inputs[1]   = xor_cd;
        auto parity = circuic->add_cell(ID($_XOR_), inputs, NEW_GATE);

		out[parity] = SIGBIT(parity); 

		//order
		chromosome->order(in, out);

        //make simulation
        std::vector<simulation::io_t> gates_out(circuic->chromosome->size());

        gates_out[0].byte[0] = 0;      //log IN 0
        gates_out[1].byte[0] = 255;    //log IN 1
		gates_out[0].byte[1] = 0;      //log IN 0
        gates_out[1].byte[1] = 255;    //log IN 1

        gates_out[io_id_of(a, chromosome)].byte[0] = 0x00; //log IN A
		gates_out[io_id_of(a, chromosome)].byte[1] = 0xFF; //log IN A

        gates_out[io_id_of(b, chromosome)].byte[0] = 0xF0; //log IN B
		gates_out[io_id_of(b, chromosome)].byte[1] = 0xF0; //log IN B

        gates_out[io_id_of(c, chromosome)].byte[0] = 0xCC; //log IN C
		gates_out[io_id_of(c, chromosome)].byte[1] = 0xCC; //log IN C

		gates_out[io_id_of(d, chromosome)].byte[0] = 0xAA; //log IN D
		gates_out[io_id_of(d, chromosome)].byte[1] = 0xAA; //log IN D

        //simulation test
        circuic->simulate(gates_out);

        assert("Parity byte 0",    gates_out[io_id_of(parity, chromosome)].byte[0],   0x96);
		assert("Parity byte 1",    gates_out[io_id_of(parity, chromosome)].byte[1],   0x69);

        exit(0);
    }
} cgploss;

PRIVATE_NAMESPACE_END
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
#include <stdio.h>

//representations
#include "aig.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

void assert(std::string name, uint8_t output, uint8_t expected) {
    if ((output & 0xf) != (expected & 0xf)) {
        log("Error for %s expected %x returned %x\n", name.c_str(), expected & 0xf, output & 0xf);
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

        auto chromosome = new genome::genome();
        auto circuic    = new representation::aig(chromosome);

        //circuic schema

        auto a = chromosome->add_dummy_gene(); //INPUT A
        auto b = chromosome->add_dummy_gene(); //INPUT B
        
        auto and_g       = circuic->add_aig_gate(0b000, a, b);
        auto and_not_a_g = circuic->add_aig_gate(0b001, a, b);
        auto and_not_g   = circuic->add_aig_gate(0b010, a, b);
        auto nor_g       = circuic->add_aig_gate(0b011, a, b);
        auto nand_g      = circuic->add_aig_gate(0b100, a, b);
        auto or_not_g    = circuic->add_aig_gate(0b101, a, b);
        auto nand_not_g  = circuic->add_aig_gate(0b110, a, b);
        auto or_g        = circuic->add_aig_gate(0b111, a, b);
        
        //make simulation
        std::vector<simulation::io_t> gates_out(circuic->chromosome->size());

        gates_out[0].byte[0] = 0;      //log IN 0
        gates_out[1].byte[0] = 255;    //log IN 1
        gates_out[a].byte[0] = 0b1100; //log IN A
        gates_out[b].byte[0] = 0b1010; //log IN B

        chromosome->last_input = b;

        //simulation test
        circuic->simulate(gates_out);

        assert("AND Gate",       gates_out[and_g].byte[0],       0b1000);
        assert("AND NOT A Gate", gates_out[and_not_a_g].byte[0], 0b0010);
        assert("AND NOT Gate",   gates_out[and_not_g].byte[0],   0b0100);
        assert("NOR Gate",       gates_out[nor_g].byte[0],       0b0001);
        assert("NAND Gate",      gates_out[nand_g].byte[0],      0b0111);
        assert("OR NOT Gate",    gates_out[or_not_g].byte[0],    0b1101);
        assert("NAND NOT Gate",  gates_out[nand_not_g].byte[0],  0b1011);
        assert("OR Gate",        gates_out[or_g].byte[0],        0b1110);

        exit(0);
    }
} cgploss;

PRIVATE_NAMESPACE_END
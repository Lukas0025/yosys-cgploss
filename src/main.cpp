//YOSYS
#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"

//CGP
#include "genome.h"
#include "convert.h"

#include <string>
#include <map>
#include <set>

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

/**
 * Convert RTLIL Design to CGP genome
 * @param design Design to convert
 */
static void design_2_genome(RTLIL::Design *design, RTLIL::Module *module, bool report_bits) {
	// use a SigMap to convert nets to a unique representation
	SigMap sigmap(module);

	// count how many times a single-bit signal is used
	std::map<RTLIL::SigBit, int> bit_usage_count;

	// count output lines for this module (needed only for summary output at the end)
	int line_count = 0;

	log("Looking for stub wires in module %s:\n", RTLIL::id2cstr(module->name));

	// For all ports on all cells
	for (auto &cell_iter : module->cells_) {
		log("cell type %s:\n", cell_iter.second->type.c_str());
		for (auto &conn : cell_iter.second->connections()) {
			// Get the signals on the port
			// (use sigmap to get a uniqe signal name)
			RTLIL::SigSpec sig = sigmap(conn.second);

			// add each bit to bit_usage_count, unless it is a constant
			for (auto &bit : sig) {
				if (bit.wire != NULL)
					bit_usage_count[bit]++; 
			}
		}
	}

	// for each wire in the module
	for (auto &wire_iter : module->wires_)
	{
		RTLIL::Wire *wire = wire_iter.second;

		// .. but only selected wires
		if (!design->selected(module, wire))
			continue;

		// add +1 usage if this wire actually is a port
		int usage_offset = wire->port_id > 0 ? 1 : 0;

		// we will record which bits of the (possibly multi-bit) wire are stub signals
		std::set<int> stub_bits;

		// get a signal description for this wire and split it into separate bits
		RTLIL::SigSpec sig = sigmap(wire);

		// for each bit (unless it is a constant):
		// check if it is used at least two times and add to stub_bits otherwise
		for (int i = 0; i < GetSize(sig); i++)
			if (sig[i].wire != NULL && (bit_usage_count[sig[i]] + usage_offset) < 2)
				stub_bits.insert(i);

		// continue if no stub bits found
		if (stub_bits.size() == 0)
			continue;

		// report stub bits and/or stub wires, don't report single bits
		// if called with report_bits set to false.
		if (GetSize(stub_bits) == GetSize(sig)) {
			log("  found stub wire: %s\n", RTLIL::id2cstr(wire->name));
		} else {
			if (!report_bits)
				continue;
			log("  found wire with stub bits: %s [", RTLIL::id2cstr(wire->name));
			for (int bit : stub_bits)
				log("%s%d", bit == *stub_bits.begin() ? "" : ", ", bit);
			log("]\n");
		}

		// we have outputted a line, increment summary counter
		line_count++;
	}

	// report summary
	if (report_bits)
		log("  found %d stub wires or wires with stub bits.\n", line_count);
	else
		log("  found %d stub wires.\n", line_count);
}

struct cgploss : public Pass {
	cgploss() : Pass("cgploss") {}

	void help() override {
		log("\n");
		log("    cgploss [options]\n");
		log("\n");
	}

	void execute(vector<string> params, Design* design) override {
		log("CGP EXTENSION\n");

		//auto main_genome = new genome::genome();

		for (auto mod : design->selected_modules()) {
			if (mod->processes.size() > 0) {
				log("Skipping module %s because it contains processes.\n", log_id(mod));
				continue;
			}

			std::vector<RTLIL::Cell*> all_cells = mod->selected_cells();

			convert::mapper_t mapper;

			for (auto cell : all_cells) {

				auto celll = convert::rtlil2genome_cell(cell, &mapper);

				log("cell name is %s inputs is %d, %d\n", cell->type.c_str(), celll.genome.I1, celll.genome.I2);

				//main_genome->add_cell(convert::rtlil2genome_cell(cell));

				mod->remove(cell); //delete cell in reprezentation
			}

		}


	}

} cgploss;

PRIVATE_NAMESPACE_END
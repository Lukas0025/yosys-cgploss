//YOSYS
#include "kernel/yosys.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"

//CGP
#include "genome.h"

#include <string>
#include <map>
#include <set>

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

#include "convert.cpp"

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

			mapper_t mapper;

			for (auto cell : all_cells) {

				auto celll = rtlil2genome_cell(cell, &mapper);

				log("cell name is %s inputs is %d, %d\n", cell->type.c_str(), celll.genome.I1, celll.genome.I2);

				//main_genome->add_cell(convert::rtlil2genome_cell(cell));

				mod->remove(cell); //delete cell in reprezentation
			}

		}


	}

} cgploss;

PRIVATE_NAMESPACE_END
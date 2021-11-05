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

		auto main_genome = new genome::genome();

		design2genome(design, main_genome);

		main_genome->print(log);

	}

} cgploss;

PRIVATE_NAMESPACE_END
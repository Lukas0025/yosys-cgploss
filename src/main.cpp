/**
 * YOSYS CGP Loss 
 * @author Lukas Plevac (lukas@plevac.eu)
 * Main file
 */

#include "kernel/yosys.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

struct cgploss : public Pass {
	cgploss() : Pass("cgploss") {}

	void execute(vector<string> params, Design* design) override {
		log("CGP EXTENSION\n");
	}

} cgploss;

PRIVATE_NAMESPACE_END
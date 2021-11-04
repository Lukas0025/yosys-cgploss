#pragma once
#include <vector>

namespace genome {
	enum gates_types_t {
		GATE_BUF_OUTPUT,
		GATE_BUF,
		GATE_NOT,
		GATE_AND,
		GATE_NAND,
		GATE_OR,
		GATE_NOR,
		GATE_XOR,
		GATE_XNOR,
		GATE_ANDNOT,
		GATE_ORNOT,
		GATE_AOI3,
		GATE_OAI3,
		GATE_AOI4,
		GATE_OAI4,
		GATE_MUX
	};

	typedef struct {
		int I1;
		int I2;
		int I3;
		int I4;
	} cell_genome_t;

	typedef struct {
		gates_types_t type;
		cell_genome_t genome;
	} cell_t;

	class genome {
		public:
			genome();
			void   add_cell(cell_t cell);
			cell_t get_cell(int id);

			std::vector<cell_genome_t> genomes;
			std::vector<gates_types_t> types;
	};
}
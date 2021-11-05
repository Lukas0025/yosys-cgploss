#pragma once
#include <vector>
#include <set>
#include <algorithm>

namespace genome {
	enum gates_types_t {
		GATE_DUMMY,
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
		GATE_OAI4
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
		int id;
	} cell_t;

	class genome {
		public:
			genome();
			void   add_cell(cell_t cell);
			void   swap_id(int id_a, int id_b);
			void   add_dummy_cell();
			int    size();
			void   update_cell(cell_t cell);
			void   order(std::set<int> inputs, std::set<int> outputs);
			cell_t get_cell(int id);

			std::vector<cell_genome_t> genomes;
			std::vector<gates_types_t> env;
			std::set<int>              out, in;
			int last_input;
	};
}
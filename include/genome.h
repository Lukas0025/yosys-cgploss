/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with chromosome header
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#pragma once
#include <vector>
#include <map>
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

	typedef struct cell_gene {
		int I1 = -1;
		int I2 = -1;
		int I3 = -1;
		int I4 = -1;
	} cell_gene_t;

	typedef struct {
		gates_types_t type;
		cell_gene_t gene;
		int id;
	} cell_t;

	const std::set<int> I1Gates = { GATE_BUF, GATE_NOT };
	const std::set<int> I2Gates = { GATE_AND, GATE_NAND, GATE_OR, GATE_NOR, GATE_XOR, GATE_XNOR, GATE_ANDNOT, GATE_ORNOT };
	const std::set<int> I3Gates = { GATE_AOI3, GATE_OAI3 };
	const std::set<int> I4Gates = { GATE_AOI4, GATE_OAI4 };

	class genome {
		public:
			genome();

			/**
			 * Add cell to chromosome (On top)
			 * @param cell cell to insert to chromosome
			 */
			void   add_cell(cell_t cell);
			
			/**
			 * Swap two ID of cells in chromosome
			 * Swap position in chromosome and replace all links to old position to new
			 * @param id_a id of gene (cell) to swap
			 * @param id_b id of second gene to swap
			 */
			void   swap_id(int id_a, int id_b);

			/**
			 * Add dummy cell to chromosome (On top)
			 * do nothing and have gene [-1, -1, -1, -1]
			 */
			void   add_dummy_cell();

			/**
			 * Get size of chromosome
			 */
			int    size();

			/**
			 * Update cell (GENE) in chromosome
			 * @param cell what to be updated
			 * id in chromesome is cell.id
			 */
			void   update_cell(cell_t cell);

			/**
			 * Update cell (GENE) in chromosome
			 * @param cell what to be updated
			 * id in chromesome is cell.id
			 */
			bool   order(std::map<int, void*> inputs, std::map<int, void*> outputs);

			/**
			 * Print chromosome on stdout (DEBUG)
			 * @param printer function with print string to stdout
			 */
			void   print(void (*printer)(const char* format, ...));
			
			/**
			 * Get cell (GENE and TYPE) on position
			 * @param int id position of cell in chromosome
			 * @return cell_t
			 */
			cell_t get_cell(int id);
			
			/**
			 * Chromosome
			 */
			std::vector<cell_gene_t> chromosome;
			
			/**
			 * Envoroment (Types of gates)
			 */
			std::vector<gates_types_t> env;

			/**
			 * map of input and output wires from RTLIL
			 */
			std::map<int, void*> wire_out, wire_in;

			/**
			 * last gate in chromosome of type input
			 */
			int last_input;
	};
}
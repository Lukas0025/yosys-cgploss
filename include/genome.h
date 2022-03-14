/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * file with chromosome header
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

#include "kernel/yosys.h"
#include "kernel/sigtools.h"

#define DUMMY_GENE_TYPE 0

namespace genome {
	typedef uint32_t io_id_t;
	typedef uint16_t type_t;

	typedef struct gene {
		type_t  type;
		io_id_t I1;
		io_id_t I2;
	} gene_t;

	class genome {
		public:
			genome();

			/**
			 * Add gene to chromosome (On top)
			 * @param gene to insert to chromosome
			 */
			io_id_t add_gene(gene_t gene);
			
			/**
			 * Swap two genes by position in chromosome
			 * Swap position in chromosome and replace all links to old position to new
			 * @param id_a position of gene to swap
			 * @param id_b position of second gene to swap
			 */
			void swap_genes(io_id_t id_a, io_id_t id_b);

			/**
			 * Add dummy cell to chromosome (On top)
			 * special type what do nothing
			 */
			io_id_t add_dummy_gene();

			/**
			 * Get size of chromosome (Number of genes)
			 */
			unsigned size();

			/**
			 * Update gene in chromosome
			 * @param gene to update
			 * @param pos position of gene in chromosome
			 */
			void update_gene(io_id_t pos, gene_t gene);

			/**
			 * Perform mutation on chromosome
			 * 
			 * @param center center of normal distribution of number of mutations
			 * @param sigma sigma of normal distribution of number of mutations
			 * @param type_min minimum ID of type of gene (for mutation)
			 * @param type_max maximum ID of tzpe of gene (for mutation)
			 * @return unsigned number of mutations
			 */
			unsigned mutate(unsigned center, unsigned sigma, uint16_t type_min, uint16_t type_max);

			/**
			 * Order genes in chromosome for CGP
			 * gene can have inputs only under self
			 * @param inputs inputs map
			 */
			bool order(std::map<io_id_t, Yosys::RTLIL::SigBit> inputs, std::map<io_id_t, Yosys::RTLIL::SigBit> outputs);

			/**
			 * Print chromosome on stdout (DEBUG)
			 * @param printer function with print string to stdout
			 */
			std::string  to_string();

			std::string raw_string();
			
			/**
			 * Get gene on position
			 * @param pos position of cell in chromosome
			 * @return cell_t
			 */
			gene_t get_gene(io_id_t pos);

			gene_t* get_gene_ptr(io_id_t pos);
			
			/**
			 * Chromosome
			 */
			std::vector<gene_t> chromosome;

			/**
			 * map of input and output wires from RTLIL
			 */
			std::map<io_id_t, Yosys::RTLIL::SigBit> wire_out, wire_in;

			/**
			 * last gate in chromosome of type input
			 */
			io_id_t last_input;
	};
}
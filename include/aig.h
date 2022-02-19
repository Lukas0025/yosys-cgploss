#pragma once

#include "representation.h"

namespace representation {
	class aig : public representation {
		public:
			aig(genome::genome* chromosome) : representation(chromosome) {};
			genome::io_id_t add_cell(Yosys::RTLIL::IdString type, std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			
			/**
			 * @brief Get name of parts in genome
			 * 
			 * @return string 
			 */
			std::string parts_naming() {
				return "\"gate\": [\"none\", \"A && B\", \"!A && B\", \"A && !B\", \"!A && !B\", \"!(A && B)\", \"!(!A && B)\", \"!(A && !B)\", \"!(!A && !B)\"]";
			}

		private:
			genome::io_id_t add_mux(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_nmux(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_aoi3(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_oai3(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_aoi4(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_oai4(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_and(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_andnot(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_nand(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_not(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_or(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_ornot(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_nor(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_xor(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
			genome::io_id_t add_xnor(std::vector<genome::io_id_t> inputs, genome::io_id_t output);
	};
}
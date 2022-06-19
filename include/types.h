/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * support data types header file
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#pragma once

namespace types {
	/**
	 * @brief Reprezenting number as 2**exp
	 */
	class Exponent {
		public:
			/**
			 * @brief Exponent of reprezented number
			 */
			unsigned exp;

			Exponent(unsigned exponent) {
				this->exp = exponent;
			}

			Exponent() {
				this->exp = 0;
			}

			/**
			 * Get real value of number
			 * @return unsigned
			 */
			unsigned real() {
				return 1 << this->exp;			
			}
			
			Exponent operator*(const Exponent& b) {
				Exponent c(this->exp);
				c.exp += b.exp;
				return c;
      			}

			Exponent operator/(const Exponent& b) {
				Exponent c(this->exp);
				c.exp -= b.exp;
				return c;	
      			}


			friend float operator/(const float& lhs, const Exponent& rhs) {
				unsigned do_div = rhs.exp;
				float out = lhs;

				while (do_div) {
					out = out / 2;
					do_div--;
				}

				return out;
			}

	};
}
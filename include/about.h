/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * about program header file
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#include <string>

namespace about {

    const std::string name           = "cgploss";
    const unsigned    version        = 1;
    const std::string authors        = "Lukas Plevac <xpleva07@vutbr.cz, lukas@plevac.eu>";
    const std::string build_time     = __DATE__ " " __TIME__;
    const int         optimalization = __OPTIMIZE_SIZE__;


    /**
     * @brief Print help informations using logger function
     * @param logger logger for print text to output
     */
    void print_help(void (*logger)(const char *, ...));
}
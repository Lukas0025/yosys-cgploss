/**
 * yosys-cgploss - Create circuics using Genetic (CGP)
 * about program implementation file
 * @author Lukas Plevac <xpleva07@vutbr.cz>
 */

#include "about.h"

namespace about {
    void print_help(void (*logger)(const char *, ...)) {
        logger("%s V%d\n", about::name.c_str(), about::version);
        logger("Yosys extension for aproximate circuits using genetic algoritms.\n\n");
        logger("Build %s with O%d\n", about::build_time.c_str(), about::optimalization);
        logger("Authors: %s\n\n", about::authors.c_str());
        logger("Using: %s [options]\n", about::name.c_str());
        logger("Options:\n");
        logger("-wire-test                 test load and save part, do not use CGP only load and save [DEBUG]\n");
        logger("-save_individuals=file     create debug file with all individuals [DEBUG]\n");
        logger("-keep_delay                Keep delay of circuic do not make it worse");
        logger("-ports_weights=file        ports weights file\n");
        logger("-selection_size=size       size of selected individuals on end of generation\n");
        logger("-generation_size=size      number of individuals in generation\n");
        logger("-max_one_error=0..inf      maximal accepted error of circuic (one combination - WCE)\n");
        logger("-max_abs_error=num         maximal accepted abs error of circuic (all combinations - MAE)\n");
        logger("-generations=count         count of generations\n");
        logger("-mutations_count=count     number of mutation for center of normal distribution\n");
        logger("-mutations_count_sigma=num sigma for normal distribution\n");
        logger("-parents=1..2              number of parents for kid\n");
        logger("-power_accuracy_ratio=0..1 float number for loss (1 - power_accuracy_ratio) * abs_error + power_accuracy_ratio * power_loss\n");
        logger("-cross_parts=2..inf        number of crossover points for cross individuals\n");
        logger("-l-back=num                number for mutation. how many back gates can use this gate \n");
        logger("-status                    show complete generations ids and best loss\n");
        logger("-profile                   for profile extesion print data in format {tranzistros}-{MAE}-{WCE}-{score};\n");
        logger("-max_duration=1..inf       maximal time duration of optimalization in minutes\n");
        logger("-representation={aig, gates, mig}  reprezenation of circuic for CGP\n\n");
        logger("Example ports weights file:\n");
        logger("# example comment\n");
        logger("# Generic weights\n");
        logger("\\sum:       msb-first\n");
        logger("\\spi_data:  lsb-first\n\n");
        logger("# Custom multi-bits port LSB [3], ..., MSB [0]\n");
        logger("\\sum2:      1, 2, 4, 8\n\n");
        logger("# Custom one-bits port\n");
        logger("\\sum3 :      100\n\n");	
    }
}
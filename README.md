# CGP Loss optimalization YOSYS extension

use genetic algoritms for optimalize circuics


## Getting Started

CGP Loss optimalization shortly `cgploss` is YOSYS extension. For run it you need YOSYS (https://github.com/YosysHQ/yosys) after you build and install YOSYS you can use this extension. Firstly you need load `cgploss` as extension in yosys

```bash
yosys -m cgploss.so
```
`cgploss.so` is builded YOSYS extension what you can build or download from github (Release builds).

## Using

```
cgploss [options]

options:
	-wire-test                 test load and save part, do not use CGP only load and save [DEBUG]
	-save_individuals=file     create debug file with all individuals
	-ports_weights=file        ports weights file
	-selection_size=size       size of selected individuals on end of generation
	-generation_size=size      number of infividuals in generation
	-max_one_error=0..inf      maximal accepted error of circuic (one combination)
	-generations=count         count of generations
	-mutations_count=count     number of mutation for center of normal distribution
	-mutations_count_sigma=num sigma for normal distribution
	-parrents=1..2             number of parrents for individual
	-power_accuracy_ratio=0..1 float number for loss (1 - power_accuracy_ratio) * abs_error + power_accuracy_ratio * power_loss
	-max_abs_error=num         maximal accepted abs error of circuic (all combinations)
	-representation={aig, gates, mig}  repreyenation of circuic for CGP
	-cross_parts=2..inf        number of crossovers for cross individuals
```

#### example run
```bash
yosys> read_verilog code.v
yosys> techmap
yosys> cgploss -ports_weights=ports.cfg -max_one_error=1 -generations=100
```

ports.cfg
```
# Test desctiption file for CPG
# This file test normal working file
#

# generic argments
\sum:       msb-first 
\spi_data:  lsb-first
\spi2_data: LSB-FIRST

# custom arguments
\uart_rx: 1024

#  LSB = 0, MSB = 2
#       LSB MSB
\uart_tx: 2 4 8 10
```

## Building

For build you need forder with YOSYS source codes, this forder is included as git submodule `yosys`. Build as it self can be run using makefile using simple make command

need packages `build-essential clang`

```bash
make
```

## Running tests

need package `iverilog`

```bash
make tests
```

## Makefile targets

```
cgploss.so       build cgploss extension (default)
multicore        build cgploss extension with multicore support
tests            run tests
stop-tests       run tests when fail stops
yosys/yosys      build yosys submodule
run              run yosys with extension
clean            clean repo
```

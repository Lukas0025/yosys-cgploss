# CGP Loss optimalization YOSYS extension (CGP Aproximation)

Use genetic algoritms for optimalize/aproximate circuics with accepted error on output or without it. This tool was created as a part of bachelor thesis at BUT FIT, this thesis is available at https://www.fit.vut.cz/study/thesis/22357/ .


## Getting Started

CGP Loss optimalization shortly `cgploss` is YOSYS extension. For run it you need YOSYS (https://github.com/YosysHQ/yosys) after you build and install YOSYS you can use this extension. Firstly you need load `cgploss` as extension in yosys. The goal of CGP Loss is create optimalized circuic with small output error and minimalize amount of electic power to run this circuic.

```bash
yosys -m cgploss.so
```
`cgploss.so` is builded YOSYS extension what you can build or download from github (Release builds).

## Using

```
cgploss [options]

options:
	# Basic
	-representation={aig, gates, mig}               reprezenation of circuic for CGP
	-max_one_error=num,         -wce=num            maximal accepted error of circuic (one combination) (worst case error) (0 is default)
	-max_abs_error=num,         -mae=num            maximal accepted abs error of circuic (all combinations) (mean average error) (0 is default)
	-generations=num,           -gcount=num         count of generations (0 is unlimited) (0 is default)
	-power_accuracy_ratio=0..1, -par=0..1           float number for loss (1 - power_accuracy_ratio) * MAE + power_accuracy_ratio * power_loss (1 is default)
	-max_duration=num,          -timeout=num        maximal time duration of optimalization in minutes (0 is unlimited) (0 is default)
	-ports_weights=file,        -ports=file         ports weights file (weights of outputs, if not presented used 2**(port bit offset))
	
	# Evolution settings
	-selection_size=num,        -ssize=num          size of selected individuals on end of generation
	-generation_size=size,      -gsize=num          number of individuals in generation

	# Mutations settings
	-mutations_count=num                            number of mutation for center of normal distribution
	-mutations_count_sigma=num                      sigma for normal distribution
	-l-back=num                                     number for mutation. how many back gates can use this gate (0 is unlimited)
	
	# Cross settings
	-parents=1..2                                   number of parents for kid
	-cross_parts=2..inf                             number of crossover points for cross individuals
	
	# Status commnads
	-status                                         show complete generations ids and best loss
	-profile                                        for profile extesion print data in format {tranzistros}-{MAE}-{WCE}-{score};

	# Debug
	-wire-test                                      test load and save part, do not use CGP only load and save [DEBUG]
	-save_individuals                               create debug file with all individuals [DEBUG]
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

## After clone

After clone you need init all git submodules for success build. You can init submodule using this commands.

```bash
git submodule init
git submodule update
```

## Building

For build you need forder with YOSYS source codes, this forder is included as git submodule `yosys`. Build as it self can be run using makefile using simple make command.

need packages `build-essential clang`

```bash
make
# or multicore with package 𝑙𝑖𝑏𝑜𝑚𝑝−𝑑𝑒𝑣
make multicore
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
test dir=name    start test with name
yosys/yosys      build yosys submodule
run              run yosys with extension
clean            clean repo
```

# CGP Loss optimalization YOSYS extension

use genetic algoritms for optimalize circuics


## Getting Started

CGP Loss optimalization shortly `cgploss` is YOSYS extension. For run it you need YOSYS (https://github.com/YosysHQ/yosys) after you build and install YOSYS you can use this extension. Firstly you need load `cgploss` as extension in yosys

```bash
yosys -m cgploss.so
```
`cgploss.so` is builded YOSYS extension what you can build or download from github (Release builds).

## Using

```bash
yosys> read_verilog code.v
yosys> techmap
yosys> abc
yosys> cgploss
```

## Building

For build you need forder with YOSYS source codes, this forder is included as git submodule `yosys`. Build as it self can be run using makefile using simple make command

```bash
make
```

## Makefile targets

```
cgploss.so       build cgploss extension (default)
yosys/yosys      build yosys submodule
run              run yosys with extension
clean            clean repo
```

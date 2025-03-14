import os
import json

RUNS = 30
GENS = 100000
Files = ["claBlock1.v", "claBlock2.v", "claBlock3.v", "claBlock4.v"]
BEST_SIZE = None
outFinalFIle = "evo_"

for inFile in Files:
    for run in range(RUNS):
        outFIle = outFinalFIle + ".tmp" 
        os.system("yosys/yosys -m cgploss.so -p \"read_verilog {}; techmap; cgploss -representation=mig -power_accuracy_ratio=1 -max_abs_error=0 -max_one_error=0 -generations={} -save_final={} -keep_delay\"".format(inFile, GENS, outFIle))

        chrom = None
        with open(outFIle) as f:
            chrom = json.load(f)

        if BEST_SIZE is None or len(chrom["chromosome"]) < BEST_SIZE:
            os.system("mv {} {}_{}.json".format(outFIle, outFinalFIle, inFile))
            BEST_SIZE = len(chrom["chromosome"])

print()
print("Done bets ind is from {} GATES".format(BEST_SIZE))



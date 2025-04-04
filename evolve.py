import os
import json
from joblib import Parallel, delayed

RUNS = 30
GENS = 100000
Files = ["claBlock2Full.v", "claBlock3Full.v", "claBlock4Full.v"]
outFinalFIle = "evo"

def process(job):
    inFile = job[0]
    
    outFIle = "{}_{}_{}.json".format(outFinalFIle, job[0], job[1])
    os.system("yosys/yosys -m cgploss.so -p \"read_verilog {}; techmap; cgploss -representation=mig -power_accuracy_ratio=1 -max_abs_error=0 -max_one_error=0 -generations={} -save_final={} -keep_delay\"".format(inFile, GENS, outFIle))

    chrom = None
    with open(outFIle) as f:
        chrom = json.load(f)

    return (len(chrom["chromosome"]), outFIle, inFile)

jobs = [(file, runI) for file in Files for runI in range(RUNS)]

results = Parallel(n_jobs=70)(delayed(process)(job) for job in jobs)

best = {}
bestName = {}
for result in results:
    if result[2] not in best or best[result[2]] > result[1]:
        best[result[2]] = result[1]
        bestName[result[2]] = result[0]

for i, val in enumerate(best):
    print("For {} is best {} gates".format(i, val))
    os.system("mv {} {}_{}.json", bestName[i], outFinalFIle, i)

for result in results:
    os.system("rm -f {}", result[0])

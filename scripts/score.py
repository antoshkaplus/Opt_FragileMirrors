import sys
import subprocess as sp
from multiprocessing import cpu_count
from multiprocessing import Pool
import argparse
import time


SCORE_START = "Score  = "
TEST_COUNT = 100
VERSION = None

CPU_COUNT = cpu_count()
if CPU_COUNT > 2: CPU_COUNT-= 2

parser = argparse.ArgumentParser()
parser.add_argument("version")
parser.add_argument("--test-count", type=int, default=100)
parser.add_argument("--cpu", dest='cpu_count', type=int, default=CPU_COUNT)

args, others = parser.parse_known_args()

TEST_COUNT = args.test_count
VERSION = args.version
CPU_COUNT = args.cpu_count

def worker(i):
    print("start seed: ", i)
    command = ["java", "-jar", "scripts/FragileMirrorsVis.jar", "-exec", f"./bin/{VERSION} {' '.join(others)}", "-novis", "-seed", str(i)]
    s = sp.check_output(command).decode(sys.stdout.encoding)
    lines = s.split("\n")
    for ln in lines:
        if ln.startswith(SCORE_START):
            return ln[len(SCORE_START):]


class Timer:
    def __enter__(self):
        self.enter_time = time.perf_counter()

    def __exit__(self, exc_type, exc_val, exc_tb):
        period = time.perf_counter() - self.enter_time
        print(f'Time spent: {period:0.2f} sec.')


pool = Pool(CPU_COUNT)
# TODO: Starting a separate process,
#  running java program from it that calls c++ solver can take quite some time.
#  Should compute performance inside the solver.
with Timer():
    # Do not use zero seed - messes up random generator of the runner.
    result = pool.map(worker, (i for i in range(1, TEST_COUNT+1)))
with open(f"./scores/{VERSION}_{TEST_COUNT}.txt", "w") as out:
    out.write(str(len(result)) + "\n")
    for index, score in enumerate(result):
        out.write(f'{index},{round(float(score),3)}\n')

import argparse
import subprocess
from dataclasses import dataclass
from pathlib import Path
import serial
import os

parser = argparse.ArgumentParser(description='Process some integers.')
parser.add_argument('--program', help='File to run')
args = parser.parse_args()


openocd = Path('~/openocd')
openocd_bin = openocd / 'src/openocd'
tcl = openocd / 'tcl'
scripts = f"-s {tcl}"
configs = f"-f {tcl/'interface/picoprobe.cfg'} -f {tcl/'target/rp2040.cfg'}"
program = Path(args.program)
run = f'-c "program {program} reset exit"'
cmd = f'{openocd_bin} {scripts} {configs} {run}'

print('Flashing...')
ser = serial.Serial('/dev/ttyACM0', 115200)
a = subprocess.run(cmd, shell=True, capture_output=True, check=True)
print(a)
print('Waiting for output...')
while True:
    if data := ser.read_all():
        print(data.decode('utf-8'), end='')

import argparse
import subprocess
from dataclasses import dataclass
from pathlib import Path
import serial
import os
import re
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
# print(a)
print('Waiting for output...')
while True:
    if data_raw := ser.read_until('\r\n'.encode()):
        data = data_raw.decode('utf-8')
        if number := re.search(r'-?\d+', data):
            val = int(number[0])
            if val == 0:
                ...
        # data = data.split('\r\n')
        # val = int(data[0])
            # if val % 100 == 0:
            print(f'{val}')

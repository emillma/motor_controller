from pathlib import Path
from re import U
import time
from shutil import copy

pico_dir = Path("D:\\")
uf2_file = Path(__file__).parents[2].joinpath('build/spi_slave.uf2')
while True:
    print("ready to flash")

    while not pico_dir.is_dir():
        time.sleep(0.1)
        # continue
        # try:
        #     import serial
        #     with serial.Serial('COM14', 115200, timeout=1) as ser:
        #         data = ser.readline()
        #         if data:
        #             print(data.decode('utf8'), sep='', end='')
        # except Exception as e:
        #     if isinstance(e, KeyboardInterrupt):
        #         quit()
    print("flashing!")
    copy(uf2_file, pico_dir.joinpath(uf2_file.name))
    while pico_dir.is_dir():
        time.sleep(0.1)
    print("done")

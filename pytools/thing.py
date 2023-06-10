import serial
from serial.tools import list_ports 

import time

# try:
#     con = serial.serial_for_url(
#         url="COM5", baudrate=1200, parity="N", stopbits=1
#     )
# except serial.SerialException:
#     pass

while True:
    try:
        con = serial.serial_for_url(
            url="COM5", baudrate=32000, parity="N", stopbits=1
        )
        break
    except serial.SerialException:
        time.sleep(1)
list_ports.comports()
# time.sleep(2)
n = 2048
t0 = time.perf_counter()
tot = 0
while True:
    # data = con.read_until(b"\n")
    # print(data)

    # data = con.read(n)
    if data:=con.read(con.in_waiting):
        tot+=len(data)
        print(f"{tot /(time.perf_counter()-t0):.2f}")
        # t0 = time.perf_counter()
    else:
        time.sleep(0.001)
  
import serial
import serial.tools.list_ports
import time

def get_url():
    for _ in range(20):
        ports = serial.tools.list_ports.comports()
        if (num := len(ports)) == 1:
            return ports[0].device
        time.sleep(0.2)
    raise Exception(f"Found {num} ports, expected 1")
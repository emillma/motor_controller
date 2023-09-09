from pathlib import Path
import asyncio
import time
import serial
from websockets.legacy.server import Serve, WebSocketServerProtocol
import serial.tools.list_ports

def get_url():
    for _ in range(20):
        ports = serial.tools.list_ports.comports()
        if (num := len(ports)) == 1:
            return ports[0].device
        time.sleep(0.2)
    raise Exception(f"Found {num} ports, expected 1")
    


def load_script(script: bytes):
    path = Path("D:\\").joinpath("flash.uf2")
    for idx in range(20):
        
        print(idx)
        
        if path.parent.exists():
            path.write_bytes(script)
            return
        try:
            serial.serial_for_url(url=get_url(), baudrate=1200)
        except Exception:
            pass
        time.sleep(0.2)
        
    raise Exception("Could not flash")


async def forward(websocket: WebSocketServerProtocol):
    loop = asyncio.get_running_loop()
    for _ in range(20):
        try:
            ser = serial.serial_for_url(url=get_url(), baudrate=9000, timeout=0.1)
            break
        except Exception:
            time.sleep(0.2)
    else:
        raise Exception("Could not open serial port")

    async def reader():
        while websocket.open:
            if data := await loop.run_in_executor(None, ser.read, 4096):
                await websocket.send(data)

    async def writer():
        while websocket.open:
            if data := await websocket.recv():
                await loop.run_in_executor(None,ser.write, data)
                
                
    # async def watchdog():
    #     while websocket.open:
    #         await asyncio.sleep(0.5)
    #         ser.write(b"\xff\x01\xff\xff")
            
    await asyncio.gather(reader(), writer())
    ser.close()


async def handle(websocket: WebSocketServerProtocol):
    if websocket.path == "/flash":
        data = await websocket.recv()
        load_script(data)
        await websocket.send(b"OK")

    elif websocket.path == "/forward":
        await forward(websocket)
        
    else:
        raise Exception("Unknown path")


async def main():
    while True:
        async with Serve(handle, "localhost", 8765, ping_timeout=None):
            await asyncio.Future()


if __name__ == "__main__":
    asyncio.run(main())

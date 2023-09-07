from pathlib import Path
import asyncio
from shutil import copy
import serial
from websockets.legacy.server import Serve, WebSocketServerProtocol
import concurrent.futures
import serial.tools.list_ports


def get_url():
    ports = serial.tools.list_ports.comports()
    assert len(ports) == 1
    return ports[0].device
    
def load_script(script: bytes):
    try:
        Path("D:\\").joinpath("flash.uf2").write_bytes(script)
        return
    except Exception:
        pass
    ser = serial.serial_for_url(url=get_url(), baudrate=1200)
    ser.close()
    Path("D:\\").joinpath("flash.uf2").write_bytes(script)
    


async def handle(websocket: WebSocketServerProtocol):
    data = await websocket.recv()
    load_script(data)
    ser = serial.serial_for_url(url=get_url(), baudrate=9600, timeout=0.01)
    loop = asyncio.get_running_loop()

    async def reader():
        while True:
            if data := await loop.run_in_executor(None, ser.read(4096)):
                await websocket.send(data)

    async def writer():
        while True:
            if data := await websocket.recv():
                await loop.run_in_executor(None, ser.write(data))

    async with asyncio.TaskGroup() as tg:
        tg.create_task(reader(), name="1")
        tg.create_task(writer(), name="2")


async def main():
    while True:
        async with Serve(handle, "localhost", 8765, ping_timeout=None):
            await asyncio.Future()


if __name__ == "__main__":
    asyncio.run(main())

from pathlib import Path
import asyncio
from shutil import copy
import serial
from websockets.legacy.server import Serve, WebSocketServerProtocol
import concurrent.futures
import serial.tools.list_ports


class AsyncSerial:
    @staticmethod
    def url():
        try:
            return serial.tools.list_ports.comports()[0].device
        except IndexError:
            return None

    def __init__(self, baudrate, timeout):
        kwargs = {
            "do_not_open": True,
            "timeout": timeout,
            "baudrate": baudrate,
        }
        # list serials ports
        self.con = serial.serial_for_url(url=self.url(), **kwargs)
        self.loop = None

    async def __aenter__(self):
        print("Opening serial port")
        self.con.open()
        self.loop = asyncio.get_running_loop()
        return self

    async def __aexit__(self, *exc):
        print("Closing serial port")
        self.con.close()

    async def read_until(self, pattern):
        coro = self.loop.run_in_executor(None, self.con.read_until, pattern, 4096)
        return await coro

    async def read(self, n):
        coro = self.loop.run_in_executor(None, self.con.read, n)
        return await coro

    async def write(self, data: bytes):
        coro = self.loop.run_in_executor(None, self.con.write, data)
        await asyncio.wait_for(coro, timeout=2)

    @staticmethod
    async def load_script(script: bytes):
        try:
            async with AsyncSerial(1200, 0.01):
                pass
        except serial.SerialException as e:
            pass
        path = Path("D:\\").joinpath("flash.uf2")
        for _ in range(100):
            if path.parent.is_dir():
                break
            await asyncio.sleep(0.05)
        else:
            raise Exception("Bootsel not working")
        with open(path, "wb") as f:
            f.write(script)
        await asyncio.sleep(1)


async def handle(websocket: WebSocketServerProtocol):
    data = await websocket.recv()
    await AsyncSerial.load_script(data)
    async with AsyncSerial(9600, 0.01) as ser:

        async def reader():
            while True:
                if data := await ser.read(4096):
                    await websocket.send(data)

        async def writer():
            while True:
                if data := await websocket.recv():
                    await ser.write(data)

        async with asyncio.TaskGroup() as tg:
            tg.create_task(reader(), name="1")
            tg.create_task(writer(), name="2")


async def main():
    while True:
        async with Serve(handle, "localhost", 8765, ping_timeout=None):
            await asyncio.Future()


if __name__ == "__main__":
    asyncio.run(main())

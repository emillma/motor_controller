from dataclasses import dataclass, field
import itertools
from pathlib import Path
import asyncio
import websockets
from shutil import copy
from serial.tools import list_ports
import serial
import serial_asyncio
from websockets.legacy.server import Serve, WebSocketServerProtocol
from websockets.exceptions import ConnectionClosedOK
import logging
import concurrent.futures

pico_dir = Path("D:\\")


class AsyncSerial:
    def __init__(self, baudrate=None):
        kwargs = {"do_not_open": True}

        if baudrate is not None:
            kwargs["baudrate"] = baudrate

        self.con = serial.serial_for_url(url="COM3", **kwargs)
        self.pool = None
        self.loop = None

    async def __aenter__(self):
        self.pool = concurrent.futures.ThreadPoolExecutor().__enter__()
        self.loop = asyncio.get_running_loop()
        for _ in range(10):
            try:
                self.con.open()
                break
            except serial.SerialException:
                await asyncio.sleep(0.2)
        else:
            raise serial.SerialException("Could not open serial port")
        return self

    async def __aexit__(self, *exc):
        self.pool.__exit__(*exc)
        if self.con.is_open:
            self.con.close()

    async def read_until(self, pattern):
        return await self.loop.run_in_executor(self.pool, self.con.read_until, pattern)

    def write(self, data: bytes):
        self.con.write(data)

    @staticmethod
    async def trigger_bootsel():
        con = serial.serial_for_url(url="COM3", baudrate=1200, do_not_open=True)
        try:
            con.open()
        except serial.SerialException:
            pass
        finally:
            con.close()


async def handle(websocket: WebSocketServerProtocol):
    await AsyncSerial.trigger_bootsel()

    while not pico_dir.is_dir():
        await asyncio.sleep(0.1)

    pico_dir.joinpath("flash.uf2").write_bytes(await websocket.recv())

    async with AsyncSerial() as ser:

        async def reader():
            while True:
                await websocket.send(await ser.read_until(b"\n"))

        async def writer():
            while True:
                ser.write(await websocket.recv())

        await asyncio.gather(reader(), writer())


async def main():
    async with Serve(handle, "localhost", 8765):
        await asyncio.Future()


if __name__ == "__main__":
    asyncio.run(main())

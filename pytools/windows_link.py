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
from websockets.exceptions import ConnectionClosedError
from asyncio.exceptions import IncompleteReadError

pico_dir = Path("D:\\")

done_event = asyncio.Event()

class AsyncSerial:
    def __init__(self, baudrate=None):
        kwargs = {"do_not_open": True}

        if baudrate is not None:
            kwargs["baudrate"] = baudrate

        self.con = serial.serial_for_url(url="COM3", **kwargs)
        self.pool = None
        self.loop = None

    async def __aenter__(self):
        self.pool = concurrent.futures.ThreadPoolExecutor(max_workers=10).__enter__()
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
        coro = self.loop.run_in_executor(self.pool, self.con.read_until, pattern)
        try:
            return await asyncio.wait_for(coro, timeout=2)
        except asyncio.TimeoutError:
            return b""


    async def write(self, data: bytes):
        coro = self.loop.run_in_executor(self.pool, self.con.write, data)
        try:
            await asyncio.wait_for(coro, timeout=2)
        except asyncio.TimeoutError:
            raise serial.SerialTimeoutException("Write timeout")

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
                await ser.write(await websocket.recv())

        async def keep_alive():
            while True:
                await asyncio.sleep(1)
                await websocket.keepalive_ping()
        try:
            async with asyncio.TaskGroup() as tg:
                tg.create_task(reader(), name="1")
                tg.create_task(writer(), name="2")
                tg.create_task(keep_alive(), name="3")
        except ExceptionGroup as _:
            await websocket.close()
        done_event.set()

async def main():
    while True:
        async with Serve(handle, "localhost", 8765):
            await asyncio.Future()
            # done_event.clear()



if __name__ == "__main__":
    asyncio.run(main())

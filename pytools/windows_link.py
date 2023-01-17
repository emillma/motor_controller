from pathlib import Path
from shutil import copy
import asyncio
from serial.tools import list_ports
import serial_asyncio
import websockets.exceptions
import websockets.client
import json

pico_dir = Path("D:\\")
uf2_file = Path(__file__).parents[1].joinpath("build/ins_link.uf2")
assert uf2_file.is_file()


async def attempt_flash():
    while True:
        if pico_dir.is_dir():
            print("flashing!")
            copy(uf2_file, pico_dir.joinpath(uf2_file.name))
            print("done")
        await asyncio.sleep(1)


async def serial_read():
    try:
        ports = list_ports.comports()
        if len(ports) == 1:
            name = ports[0].name
            reader, writer = await serial_asyncio.open_serial_connection(url=name)
        while True:
            data = await reader.readuntil(b"\n")
            print(data)
    except Exception as e:
        print("serial closed")
        print(e)
        await asyncio.sleep(1)
        await serial_read()


async def websocket_read():
    try:
        async with websockets.client.connect("ws://localhost:8050/stuff") as ws:
            await ws.send(json.dumps({"type": "subscribe", "topic": "pico_in"}))
            while True:
                await ws.send(
                    json.dumps(
                        {"type": "publish", "topic": "pico_out", "data": "hello"}
                    )
                )
                await asyncio.sleep(1)
    except websockets.exceptions.ConnectionClosedOK as err:
        print(f"Closed: {err.reason}")

    except websockets.exceptions.ConnectionClosedError as err:
        print(f"Closed: {err.reason}")

    finally:
        await asyncio.sleep(1)
        await websocket_read()


async def main():
    await asyncio.gather(
        # attempt_flash(),
        # serial_read(),
        websocket_read(),
    )


if __name__ == "__main__":
    asyncio.run(main())

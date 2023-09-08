import itertools
from pathlib import Path
from websockets.legacy.client import WebSocketClientProtocol
from wsl_link import connect_over_ws, build_and_flash
import asyncio
import aioconsole
from io import BytesIO
import re
from pprint import pprint
import numpy as np


def parse_stim(data: bytearray):
    pat = re.compile(b"\xa7.*?(?=\xa7)", flags=re.DOTALL)
    for match in pat.finditer(data):
        print(match)


async def reader(sock: WebSocketClientProtocol):
    rec: dict[int, bytearray] = dict()
    buffer = bytearray()
    pat = re.compile(b"\xff([\x00-\xfd])(.*?)(?=\xff[\x00-\xfd])", flags=re.DOTALL)

    async for message in sock:
        msgs = message.split(b"\xa7")
        print([len(m) for m in msgs])
        here = True

        # for v in [_ for _ in message.split(b"\r") if _]:
        #     try:
        #         print(v.decode())
        #     except UnicodeDecodeError:
        #         print(v)
        # print(message)
        # buffer.extend(message)
        # while match := pat.search(buffer):
        #     buffer = buffer[match.end() :]
        #     key = int.from_bytes(match[1])
        #     val = re.sub(b"\xff\xfe", b"\xff", match[2])
        #     if key == 1:
        #         print(val)
        #     elif key == 90:

        # here = True
    # if key == 90:
    #     print(val)
    # for line in val.split(b"\r"):
    #     try:
    #         print(line.decode())
    #     except UnicodeDecodeError:
    #         print(line)

    # rec.setdefault(key, bytearray()).extend(val)
    # buffer = buffer[match.end() :]
    # parse_stim(rec[90])


async def writer(sock: WebSocketClientProtocol):
    await asyncio.sleep(1)
    await sock.send(b"\x00")
    await asyncio.sleep(1)

    async def echo():
        pass
        # for i in itertools.count():
        # line = b"\xff\x02" + f"ask {i}".encode() + b"\xff\xff"
        # await sock.send(line)
        # await asyncio.sleep(1)

    async def stim():
        pass
        # await sock.send(b"\xff\x03SERVICEMODE\r\xff\xff")
        # for _ in itertools.count():
        #     line: str = await aioconsole.ainput()
        #     await sock.send(b"\xff\x03" + line.encode() + b"\r\xff\xff")

    await asyncio.gather(echo(), stim())


async def main():
    build_dir = Path(__file__).parents[1] / "build"
    build_dir.mkdir(exist_ok=True)
    project_dir = Path(__file__).parents[1] / "hello_world"

    await build_and_flash(build_dir, project_dir)

    print("Connecting")
    await connect_over_ws(reader, writer)


if __name__ == "__main__":
    asyncio.run(main())

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
        buffer.extend(message)
        while match := pat.search(buffer):
            key = int.from_bytes(match[1])
            val = re.sub(b"\xff\xfe", b"\xff", match[2])
            buffer = buffer[match.end() :]
            # print(key, len(val), [len(v) for v in val.split(b"\x93")])
            for char in val:
                if char == ord("\r"):
                    print()
                else:
                    try:
                        print(chr(char), end="")
                    except UnicodeDecodeError:
                        print(char, end="")


async def writer(sock: WebSocketClientProtocol):
    await asyncio.sleep(1)
    await sock.send(b"\x00")  # start

    async def echo():
        pass
        # for i in itertools.count():
        #     line = b"\xff\x02" + f"echo {i}".encode() + b"\xff\xff"
        #     await sock.send(line)
        #     await asyncio.sleep(1)

    async def stim():
        for i in range(4):
            await sock.send(b"SERVICEMODE\r")

        for _ in itertools.count():
            line: str = await aioconsole.ainput()
            await sock.send(line.encode() + b"\r")

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

import itertools
from pathlib import Path
from websockets.legacy.client import WebSocketClientProtocol
from wsl_link import connect_over_ws
import asyncio
import aioconsole
from io import BytesIO
import re
from pprint import pprint
import numpy as np


async def reader(sock: WebSocketClientProtocol):
    rec: dict[int, bytearray] = dict()
    buffer = bytearray()
    async for message in sock:
        buffer.extend(message)
        pat = re.compile(b"\xff([\x00-\xfd])(.*?)(?=\xff[\x00-\xfd])", flags=re.DOTALL)
        while match := pat.search(buffer):
            data = re.sub(b"\xff\xfe", b"\xff", match[2])
            rec.setdefault(int.from_bytes(match[1]), bytearray()).extend(data)
            buffer = buffer[match.end() :]
        sep = b"\xb5\x62\x02\x15"
        for k, v in rec.items():
            print(f"{k}:", f"{len(v)}")


async def writer(sock: WebSocketClientProtocol):
    await asyncio.sleep(2)
    # await sock.send(b"SERVICEMODE\r")
    for i in itertools.count():
        line: str = await aioconsole.ainput()
        await sock.send(line.encode() + b"\r")


if __name__ == "__main__":
    build_dir = Path(__file__).parents[1] / "build"
    build_dir.mkdir(exist_ok=True)
    project_dir = Path(__file__).parents[1] / "hello_world"
    asyncio.run(connect_over_ws(build_dir, project_dir, reader, writer))

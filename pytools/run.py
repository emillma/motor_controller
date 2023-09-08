import itertools
from websockets.legacy.client import WebSocketClientProtocol
from wsl_link import connect_over_ws
import asyncio
import aioconsole
from io import BytesIO
import re
from pprint import pprint
import numpy as np


async def reader(sock: WebSocketClientProtocol):
    received = dict()
    buffer = bytearray()
    async for message in sock:
        buffer.extend(message)
        pat = re.compile(b"\xff([\x00-\xfd])(.*?)(?=\xff[\x00-\xfd])", flags=re.DOTALL)

        while match := pat.search(buffer):
            d = re.sub(b"\xff\xfe", b"\xff", match.group(2))
            # data = data.split(b"\xb5\x62\x02\x15")
            received.setdefault(int.from_bytes(match.group(1)), bytearray()).extend(d)
            buffer = buffer[match.end() :]
        # sep = b"\xb5\x62\x02\x15"
        for k, v in received.items():
            print(f"{k}:", [len(i) for i in v.split(b"\xa7")])


async def writer(sock: WebSocketClientProtocol):
    await asyncio.sleep(2)
    # await sock.send(b"SERVICEMODE\r")
    for i in itertools.count():
        line: str = await aioconsole.ainput()
        await sock.send(line.encode() + b"\r")


if __name__ == "__main__":
    asyncio.run(connect_over_ws(reader, writer))

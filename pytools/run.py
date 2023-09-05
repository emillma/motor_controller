import itertools
from websockets.legacy.client import WebSocketClientProtocol
from wsl_link import connect_over_ws
import asyncio
import aioconsole
from io import BytesIO
import re
from pprint import pprint
import numpy as np

pat = b"\xa7" + b"(.{9})." * 4
pat = b"\xa7(.{9}).(.{9}).(.{9}).(.{9})."


async def reader(sock: WebSocketClientProtocol):
    buffer = bytearray()
    async for message in sock:
        buffer.extend(message)
        bc = np.bincount(buffer)
        bc_args = np.argsort(bc)[-10:]
        print(" ".join(f"{a:x}:{bc[a]}" for a in bc_args))
        buffer = buffer[-10000:]
        # buffer.extend(message)

        # print(message.replace(b''))
        # for meas in buffer.split(b"\xa7"):
        # print(np.bincount(meas))
        # buffer = meas

        # print(message)
        # pprint(message)
        # print(message)

        # print(len(message))
        # buffer.extend(message)
        # # print(bytes(buffer))

        # before, sep, after = buffer.partition(b"\xa7")
        # if sep:
        #     print(len(before))
        #     buffer = after


async def writer(sock: WebSocketClientProtocol):
    await asyncio.sleep(2)
    # await sock.send(b"SERVICEMODE\r")
    for i in itertools.count():
        line: str = await aioconsole.ainput()
        await sock.send(line.encode() + b"\r")


if __name__ == "__main__":
    asyncio.run(connect_over_ws(reader, writer))

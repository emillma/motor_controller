import itertools
from websockets.legacy.client import WebSocketClientProtocol
from wsl_link import connect_over_ws
import asyncio
import aioconsole
from io import BytesIO
import re
from pprint import pprint


async def reader(sock: WebSocketClientProtocol):
    # binary io buffer
    buffer = bytearray()
    # imu_z_pattern = rb'\xa7'
    async for message in sock:
        for line in message.split(b"\r"):
            if line:
                try:
                    line = line.decode()
                except UnicodeDecodeError:
                    pass
                print(line)

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
    await asyncio.sleep(5)

    # await sock.send(b"UTILITYMODE\0x0D")
    # await sock.send(b"IMUZ\r")
    for i in itertools.count():
        # awai
        line = await aioconsole.ainput()

        # await sock.send(b"SERVICEMODE\r")
        # await sock.send(b"UTILITYMODE\r")
        await sock.send(str(line).encode() + b"\r")
        await asyncio.sleep(1)


if __name__ == "__main__":
    asyncio.run(connect_over_ws(reader, writer))

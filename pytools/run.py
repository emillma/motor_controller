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


class StimHandler:
    buffer = bytearray()
    stim_regx = re.compile(
        b"\x93"
        b"(?P<gyro_data>.{9})"
        b"(?P<gyro_status>.)"
        b"(?P<acc_data>.{9})"
        b"(?P<acc_status>.)"
        b"(?P<incl_data>.{9})"
        b"(?P<incl_status>.)"
        b"(?P<counter>.)"
        b"(?P<latency>.{2})"
        b"(?P<crc>.{4})"
        b"(?=\x93)",
        flags=re.DOTALL,
    )

    @classmethod
    def parse(cls, data):
        cls.buffer.extend(data)
        messages = []
        for m in cls.stim_regx.finditer(cls.buffer):
            messages.append(m)
        try:
            cls.buffer = cls.buffer[m.end() :]
        except UnboundLocalError:
            pass
        return messages

    @classmethod
    def show(cls, matches: list[re.Match]):
        counts = [int.from_bytes(m["counter"], "little") for m in matches]
        return counts


def decode(data: bytes):
    try:
        return data.decode()
    except UnicodeDecodeError:
        print(data)


async def reader(sock: WebSocketClientProtocol):
    rec: dict[int, bytearray] = dict()
    buffer = bytearray()
    msg_regx = re.compile(b"\xff([\x00-\xfd])(.*?)(?=\xff[\x00-\xfd])", flags=re.DOTALL)
    async for message in sock:
        buffer.extend(message)
        while match := msg_regx.search(buffer):
            buffer = buffer[match.end() :]
            key = int.from_bytes(match[1], "little")
            val = re.sub(b"\xff\xfe", b"\xff", match[2])
            if key == 1:
                print(key, decode(val))

            if key == 90:
                messages = StimHandler.parse(val)
                print(key, StimHandler.show(messages))

            if key in {30, 31}:
                print(key, val)

            # print(key)
            # # print(key, len(val), [len(v) for v in val.split(b"\x93")])
            # for char in val:
            #     if char == ord("\r"):
            #         print()
            #     else:
            #         try:
            #             print(chr(char), end="")
            #         except UnicodeDecodeError:
            #             print(char, end="")


async def writer(sock: WebSocketClientProtocol):
    await asyncio.sleep(1)
    return
    await sock.send(b"\x00")  # start

    async def stim():
        pass
        # for i in range(4):
        #     await sock.send(b"SERVICEMODE\r")

        # for _ in itertools.count():
        #     line: str = await aioconsole.ainput()
        #     await sock.send(line.encode() + b"\r")

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

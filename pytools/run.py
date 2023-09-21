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
    def show_counter(cls, matches: list[re.Match]):
        counts = [int.from_bytes(m["counter"], "little") for m in matches]
        current = []
        ranges = []

        for c in counts:
            if not current:
                current.append(c)
            elif c == current[-1] + 1:
                current.append(c)
            else:
                ranges.append(current)
                current = [c]
        if current:
            ranges.append(current)
        return ",".join(
            [f"{r[0]}-{r[-1]}" if len(r) > 1 else str(r[0]) for r in ranges]
        )

    @classmethod
    def show_latency(cls, matches: list[re.Match]):
        latencies = [int.from_bytes(m["latency"], "big") for m in matches]
        return np.mean(latencies)


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
            elif key == 90:
                # ptin
                # print(key, len(val))
                messages = StimHandler.parse(val)
                print(key, len(val), StimHandler.show_latency(messages))

            elif key in {30, 31}:
                print(key, val)
            else:
                print(key, len(val), val)
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

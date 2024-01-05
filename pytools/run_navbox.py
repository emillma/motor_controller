import itertools
from pathlib import Path
from websockets.legacy.client import WebSocketClientProtocol
from wsl_link import connect_over_ws, build_and_flash
import asyncio

# import aioconsole
from io import BytesIO
import re
from pprint import pprint
import numpy as np
import time


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
        return (
            f"{np.mean(latencies):.3f}, {np.std(latencies):.3f},"
            f" {np.amax(latencies):.3f}"
        )


def decode(data: bytes):
    return data.decode(errors="backslashreplace")


async def reader(sock: WebSocketClientProtocol):
    rec: dict[int, bytearray] = dict()
    received = 0 #2617297, 2616907, 3382763
    buffer = bytearray()
    msg_regx = re.compile(b"\xde\xad\xbe(.)(.*?)(?=\xde\xad\xbe)", flags=re.DOTALL)

    t0 = time.perf_counter()
    total = 0
    file = Path("/workspaces/home/pico/pytools/data/stim.bin")
    file.parent.mkdir(exist_ok=True)
    async for message in sock:
        received += len(message)
        buffer.extend(message)
        while match := msg_regx.search(buffer):
            buffer = buffer[match.end() :]
            key = int.from_bytes(match[1], "little")
            data = match[2]
            with open(file, "ab") as f:
                f.write(data)
            if key == 9:
                messages = StimHandler.parse(match[2])
                print(key, len(data), StimHandler.show_counter(messages))
            else:
                print(key, len(data))


async def writer(sock: WebSocketClientProtocol):
    return


async def main():
    build_dir = Path(__file__).parents[1] / "build"
    build_dir.mkdir(exist_ok=True)
    project_dir = Path(__file__).parents[1] / "navbox"

    await build_and_flash(build_dir, project_dir)

    print("Connecting")
    await connect_over_ws(reader, writer)


if __name__ == "__main__":
    asyncio.run(main())

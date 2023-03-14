import itertools
from pathlib import Path
import asyncio
import shlex
import time
from websockets.legacy.client import Connect, WebSocketClientProtocol
import sys

build_dir = Path(__file__).parents[1] / "build"
program_path = build_dir / "my_program.uf2"


async def hello():
    async with Connect("ws://host.docker.internal:8765") as websocket:
        await websocket.send("Hello world!")
        print(await websocket.recv())


async def bootsel():
    await asyncio.sleep(2)


async def make():
    cmd = "make -j8"
    proc = await asyncio.create_subprocess_exec(
        *shlex.split(cmd),
        cwd=build_dir,
    )
    await proc.wait()
    try:
        assert proc.returncode == 0
        assert program_path.is_file()
        assert program_path.stat().st_size > 0
    except:
        return None
    return program_path.read_bytes()


async def main():
    task = asyncio.create_task(make())
    async with Connect(
        "ws://host.docker.internal:8765", ping_timeout=None
    ) as websocket:
        if not (data := await task):
            return

        await websocket.send(data)

        async def reader():
            async for message in websocket:
                print(message)

        async def writer():
            for i in itertools.count():
                await asyncio.sleep(0.5)

                await websocket.send(b"hello" + chr(ord("a") + i % 26).encode())

        await asyncio.gather(reader(), writer())


asyncio.run(main())

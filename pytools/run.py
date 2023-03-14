from pathlib import Path
import asyncio
import shlex
import time
from websockets.legacy.client import Connect, WebSocketClientProtocol

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
    assert proc.returncode == 0
    assert program_path.is_file()
    assert program_path.stat().st_size > 0

    return program_path.read_bytes()


async def main():
    task = asyncio.create_task(make())
    async with Connect(
        "ws://host.docker.internal:8765", ping_timeout=None
    ) as websocket:
        await task
        await websocket.send(program_path.read_bytes())

        async def reader():
            async for message in websocket:
                print(message)

        async def writer():
            while True:
                await asyncio.sleep(4)
                # await websocket.send(b"H")

        await asyncio.gather(reader(), writer())


asyncio.run(main())

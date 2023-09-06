from pathlib import Path
import asyncio
import shlex
from websockets.legacy.client import Connect

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


async def connect_over_ws(reader, writer):
    task = asyncio.create_task(make())
    async with Connect("ws://host.docker.internal:8765", ping_timeout=None) as sock:
        if not (data := await task):
            return
        await sock.send(data)
        print("RUNNING")
        await asyncio.gather(reader(sock), writer(sock))

from pathlib import Path
import asyncio
import shlex
from websockets.legacy.client import Connect


async def hello():
    async with Connect("ws://host.docker.internal:8765") as websocket:
        await websocket.send("Hello world!")
        print(await websocket.recv())


async def bootsel():
    await asyncio.sleep(2)


async def cmake(build_dir: Path, project_dir: Path, build_type="Release"):
    cmd = f"cmake -DCMAKE_BUILD_TYPE=Debug {project_dir}"
    proc = await asyncio.create_subprocess_exec(*shlex.split(cmd), cwd=build_dir)
    await proc.wait()
    assert proc.returncode == 0


async def make(build_dir: Path):
    cmd = "make -j8"
    proc = await asyncio.create_subprocess_exec(*shlex.split(cmd), cwd=build_dir)
    await proc.wait()
    assert proc.returncode == 0


async def get_binary(build_dir: Path, project_dir: Path, build_type="Release"):
    await cmake(build_dir, project_dir, build_type)
    await make(build_dir)
    assert len(uf2s := list(build_dir.glob("*.uf2"))) == 1
    assert len(binary := uf2s[0].read_bytes()) > 0
    return binary


async def connect_over_ws(build_dir: Path, project_dir: Path, reader, writer):
    build_task = asyncio.create_task(get_binary(build_dir, project_dir))

    async with Connect("ws://host.docker.internal:8765", ping_timeout=None) as sock:
        binary = await build_task
        await sock.send(binary)
        await asyncio.gather(reader(sock), writer(sock))

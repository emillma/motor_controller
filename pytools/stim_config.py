import asyncio
from pathlib import Path
import aioconsole
from wsl_link import build_and_flash, connect_over_ws
from websockets.legacy.client import WebSocketClientProtocol


async def main():
    build_dir = Path(__file__).parents[1] / "build"
    build_dir.mkdir(exist_ok=True)
    project_dir = Path(__file__).parents[1] / "stim_config"

    await build_and_flash(build_dir, project_dir)

    async def reader(sock: WebSocketClientProtocol):
        while True:
            data: bytes = await sock.recv()
            data = data.replace(b"\r", b"\n").decode(errors="replace")
            print(data, sep="")

    async def writer(sock: WebSocketClientProtocol):
        await asyncio.sleep(2)
        await sock.send(b"SERVICEMODE\r")
        while True:
            if data := await aioconsole.ainput():
                data: str
                await sock.send(data.encode() + b"\r")

    await connect_over_ws(reader, writer)


if __name__ == "__main__":
    asyncio.run(main())

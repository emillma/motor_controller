import asyncio
from pathlib import Path
from wsl_link import build_and_flash, connect_over_ws
from websockets.legacy.client import WebSocketClientProtocol




async def reader(sock: WebSocketClientProtocol):
    async for msg in sock:
        print(msg.decode(errors='replace'))
        
        
async def writer(sock: WebSocketClientProtocol):
    while True:
        data = await asyncio.get_running_loop().run_in_executor(None, input)
        await sock.send((data + '\r\n').encode())
        
async def main():
    build_dir = Path(__file__).parents[1] / "build"
    build_dir.mkdir(exist_ok=True)
    project_dir = Path(__file__).parents[1] / "pps_sim"
    await build_and_flash(build_dir, project_dir)
    
    await connect_over_ws(reader, writer)

if __name__ == "__main__":
    asyncio.run(main())

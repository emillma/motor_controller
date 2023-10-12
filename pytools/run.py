import asyncio
from pathlib import Path
from wsl_link import build_and_flash, connect_over_ws


async def main():
    build_dir = Path(__file__).parents[1] / "build"
    build_dir.mkdir(exist_ok=True)
    project_dir = Path(__file__).parents[1] / "tracker_box"

    await build_and_flash(build_dir, project_dir)


if __name__ == "__main__":
    asyncio.run(main())

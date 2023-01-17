"""
Remember to set 'Cmake: Build Task' in settings
"""

from pathlib import Path
import time
picofolder = Path(__file__).parent


files = picofolder.rglob('*.pio')
for file in files:
    CR = 13
    file.write_bytes(bytes([b for b in file.read_bytes() if b != CR]))

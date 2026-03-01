#!/usr/bin/env python3
"""
Clears a 580x180 pixel box in the top-left corner of PNG files (sets to transparent).
Usage: python clear_top_left.py image1.png image2.png ...
"""

import sys
from pathlib import Path
from PIL import Image


def clear_top_left(filepath: str) -> None:
    path = Path(filepath)
    if not path.exists():
        print(f"[SKIP] File not found: {filepath}")
        return
    if path.suffix.lower() != ".png":
        print(f"[SKIP] Not a PNG: {filepath}")
        return

    img = Image.open(path).convert("RGBA")
    # Create a transparent box and paste it into the top-left
    box = Image.new("RGBA", (580, 180), (0, 0, 0, 0))
    img.paste(box, (0, 0))
    img.save(path)
    print(f"[DONE] {filepath}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python clear_top_left.py image1.png image2.png ...")
        sys.exit(1)

    for f in sys.argv[1:]:
        clear_top_left(f)
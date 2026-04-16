# Chess Diagram Editor

[![Build](https://github.com/yoyold/ChessDiagramEditor/actions/workflows/build.yml/badge.svg)](https://github.com/yoyold/ChessDiagramEditor/actions/workflows/build.yml)

A Qt6-based chess diagram editor with a visual board editor, FEN/PGN import, and image export.

Uses the well-known **Cburnett piece set** from Wikimedia Commons (CC BY-SA 3.0 / BSD).

## Features

- **Visual editing** — Select a piece from the palette and click squares to place it. Right-click to remove.
- **FEN support** — Load positions from FEN or copy the current position as FEN.
- **PGN support** — Paste a complete PGN game and replay all moves.
- **Image export** — Export the diagram as PNG, JPEG, or BMP at any resolution (200–4000 px).
- **Clipboard** — Copy the diagram directly to the clipboard.
- **Color schemes** — 7 board colors: Classic, Blue, Green, Purple, Gray, Brown, High Contrast.
- **Board flip** — View from White's or Black's perspective.

## Requirements

- **Qt 6.x** (Widgets and SVG modules)
- **CMake 3.16+**
- **C++17 compiler**
- **PowerShell** (for downloading the pieces)

## Setup

### 1. Download pieces

Before the first build, download the SVG pieces from Wikimedia Commons:

```powershell
PowerShell -ExecutionPolicy Bypass -File download_pieces.ps1
```

This downloads 12 SVG files into the `pieces/` folder.

### 2. Build

**In Qt Creator:**
1. Open `CMakeLists.txt` as a project
2. Select a Qt 6.x kit (with SVG module)
3. Click "Run"

**Command line:**
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Usage

### Placing pieces
1. Select a piece from the **piece palette** on the right (White or Black)
2. Click a square to place the piece
3. Click the same piece again to remove it
4. Right-click always removes
5. **Eraser (✖)** for targeted removal

### Importing positions

**From FEN:** Enter a FEN string in the bar at the bottom and press Enter or click "Load".

**From PGN:** Click "Load PGN..." and paste the PGN text.

### Exporting diagrams

1. Choose a **format** (PNG, JPEG, BMP)
2. Set the **size** in pixels (200–4000)
3. Click "Save as file..." or "Copy to clipboard"

### Keyboard shortcuts

| Shortcut | Action |
|---|---|
| Ctrl+E | Export image |
| Ctrl+Shift+C | Copy image to clipboard |
| Ctrl+C | Copy FEN |
| Ctrl+F | Load FEN |
| Ctrl+P | Load PGN |
| Ctrl+R | Starting position |
| Ctrl+L | Clear board |
| F | Flip board |

## License

Application: MIT License  
Piece set: CC BY-SA 3.0 / BSD — Author: Colin M.L. Burnett (Cburnett), Wikimedia Commons

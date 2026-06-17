# GV Player

A high-quality, open-source video player for macOS built with C++17, Qt 6, and libmpv.

![Platform](https://img.shields.io/badge/platform-macOS-lightgrey)
![License](https://img.shields.io/badge/license-MIT-blue)
![Version](https://img.shields.io/badge/version-1.0.0-green)

## Features

- Plays virtually any video format via libmpv/FFmpeg
- Hardware-accelerated decoding (`hwdec=auto-copy`)
- High-quality zoom with `ewa_lanczossharp` scaling, deband, and ICC color management
- Mouse wheel zoom + click-and-drag pan when zoomed in
- Playback speed control: 1×, 2×, 4×, 8×
- Seek: −5 min, −5 s, +5 s
- Progress slider with live time display
- Dark minimal UI
- Retina / HiDPI aware (uses `devicePixelRatioF`)

### Supported Formats

MP4, MOV, MKV, AVI, M4V, WebM, FLV, WMV, TS, M2TS, MTS, MPEG, VOB, 3GP, OGV, HEVC/H.265/x265, and anything else supported by FFmpeg.

> Non-standard AVI files tagged as x265 are automatically remuxed to MKV using ffmpeg before playback.

## Requirements

- macOS 13 or later (Apple Silicon or Intel)
- [Homebrew](https://brew.sh)

## Building from Source

### 1. Install dependencies

```bash
brew install cmake ninja pkg-config qt mpv
```

### 2. Add Qt to PATH

```bash
echo 'export PATH="$(brew --prefix qt)/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

### 3. Configure

```bash
cmake \
  -S . \
  -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
```

### 4. Build

```bash
cmake --build build
```

### 5. Run

```bash
open build/GVPlayer.app
```

## Release Build

```bash
cmake \
  -S . \
  -B build-release \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"

cmake --build build-release
macdeployqt build-release/GVPlayer.app -dmg
```

## Project Structure

```
GVPlayer/
├── CMakeLists.txt
├── LICENSE
├── README.md
├── IMPROVEMENTS.md
├── resources/
│   └── GVPlayer.icns
└── src/
    ├── main.cpp
    ├── MainWindow.h
    ├── MainWindow.cpp
    ├── MpvWidget.h
    └── MpvWidget.cpp
```

## Architecture

| Component | Role |
|---|---|
| `MpvWidget` | Embeds libmpv inside a `QOpenGLWidget`; owns the mpv handle and render context |
| `MainWindow` | Qt Widgets UI — controls, progress slider, zoom label |
| `main.cpp` | Sets up OpenGL 4.1 Core Profile + sRGB surface, launches the window |

## Video Quality Settings

When zooming in, GV Player uses mpv's highest-quality rendering pipeline:

| Setting | Value | Purpose |
|---|---|---|
| `scale` / `cscale` / `dscale` | `ewa_lanczossharp` | Best-in-class scaling filter |
| `scale-antiring` | `0.7` | Reduces ringing artefacts |
| `sigmoid-upscaling` | `yes` | Perceptually accurate upscaling |
| `linear-downscaling` | `yes` | Downscale in linear light |
| `correct-downscaling` | `yes` | Avoids aliasing when zooming out |
| `dither` | `fruit` | Highest-quality dithering algorithm |
| `dither-depth` | `auto` | Matches actual display bit depth |
| `temporal-dither` | `yes` | Reduces dither noise over time |
| `deband` | `yes` (4 iterations) | Removes banding visible when zoomed in |
| `icc-profile` | `auto` | Reads the system display color profile |
| `target-prim` / `target-trc` | `auto` | Correct color space transform |

## License

MIT — see [LICENSE](LICENSE).

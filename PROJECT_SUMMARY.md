# GV Player — Project Summary

A professional, open-source video player for macOS built with C++17, Qt 6, and libmpv. The final goal is a standalone `.app` distributed as a `.dmg`.

---

## Concept

A local video file player for macOS focused on:

- Wide format support (MP4, MKV, AVI, MOV, WebM, …)
- H.265 / HEVC / x265 support
- Fast seeking
- Frame-by-frame navigation (planned)
- Playback speed control
- High-quality zoom and pan
- Simple dark UI
- Installable DMG for macOS

---

## Implemented Features

| Feature | How to use |
|---|---|
| Play / Pause | Play button or Space |
| Seek −5 s | `◀ 5s` button |
| Seek +5 s | `5s ▶` button |
| Seek −5 min | `⏮ 5m` button |
| Scrub | Progress slider |
| Zoom in | `+` button or scroll wheel up |
| Zoom out | `−` button or scroll wheel down |
| Reset zoom | `0` button |
| Pan (when zoomed) | Click and drag |
| Playback speed | `1×` `2×` `4×` `8×` buttons |
| Open file | `Open` button |

---

## Supported Formats

Via mpv / FFmpeg: MP4, MOV, MKV, AVI, M4V, WebM, FLV, WMV, TS, M2TS, MTS, MPEG, MPG, VOB, 3GP, OGV, HEVC, H.265, x265, and everything else FFmpeg supports.

---

## Technology Stack

| Layer | Technology |
|---|---|
| Language | C++17 |
| UI framework | Qt 6 Widgets |
| Video rendering | Qt OpenGL Widgets / `QOpenGLWidget` |
| Video engine | libmpv |
| Build system | CMake + Ninja |
| Package manager | Homebrew |
| IDE | Visual Studio Code |
| Distribution | macdeployqt → DMG |

---

## Why Qt + libmpv?

**Qt 6** provides a native, well-structured macOS UI in C++ without Objective-C.

**libmpv** provides:
- Enormous format/codec coverage via FFmpeg
- H.265 / HEVC support
- Hardware-accelerated decoding
- Built-in seek, frame step, speed, zoom, pan, audio tracks, subtitles
- Better performance than HTML/WebView players

---

## Development Environment Setup

### Prerequisites

Verify Xcode Command Line Tools:
```bash
xcode-select -p
# If missing:
xcode-select --install
```

### Install dependencies

```bash
brew install cmake ninja pkg-config qt mpv
```

### Add Qt to PATH

```bash
echo 'export PATH="$(brew --prefix qt)/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

### VS Code extensions

- C/C++ (Microsoft)
- CMake Tools (Microsoft)

---

## Project Structure

```
GVPlayer/
├── CMakeLists.txt
├── LICENSE
├── README.md
├── IMPROVEMENTS.md
├── PROJECT_SUMMARY.md
├── resources/
│   └── GVPlayer.icns
└── src/
    ├── main.cpp
    ├── MainWindow.h
    ├── MainWindow.cpp
    ├── MpvWidget.h
    └── MpvWidget.cpp
```

---

## Build Commands

Configure:
```bash
cmake \
  -S . \
  -B build \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
```

Build:
```bash
cmake --build build
```

Run:
```bash
open build/GVPlayer.app
```

---

## mpv Configuration

| Option | Value | Reason |
|---|---|---|
| `config` | `no` | Do not load user mpv config |
| `terminal` | `no` | No terminal output |
| `input-default-bindings` | `no` | App manages all keybindings |
| `input-vo-keyboard` | `no` | No direct keyboard handling by mpv |
| `vo` | `libmpv` | Embedded rendering |
| `hwdec` | `auto-copy` | Hardware decoding with safe memory copy |
| `vd-lavc-dr` | `no` | Disable direct rendering (macOS compatibility) |
| `keep-open` | `yes` | Hold last frame at end of file |
| `osc` | `no` | Disable mpv's built-in OSD controls |
| `scale`/`cscale`/`dscale` | `ewa_lanczossharp` | Best scaling filter |
| `sigmoid-upscaling` | `yes` | Perceptual upscaling |
| `correct-downscaling` | `yes` | Avoid aliasing when zooming out |
| `linear-downscaling` | `yes` | Linear-light downscaling |
| `deband` | `yes` | Remove banding |
| `dither` | `fruit` | High-quality dithering |
| `icc-profile` | `auto` | System display color profile |
| `target-prim`/`target-trc` | `auto` | Correct color space |

---

## OpenGL Setup

File: `src/main.cpp`

- **OpenGL 4.1 Core Profile** (highest version macOS supports)
- **Double buffer** + **VSync** (`swapInterval=1`)
- **sRGB color space** surface for accurate color rendering

---

## Release / Distribution

Build release:
```bash
cmake \
  -S . \
  -B build-release \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"

cmake --build build-release
```

Bundle Qt libraries:
```bash
macdeployqt build-release/GVPlayer.app
```

Inspect linked libraries:
```bash
otool -L build-release/GVPlayer.app/Contents/MacOS/GVPlayer
```

Fix library paths if needed:
```bash
install_name_tool -change <old> <new> GVPlayer.app/Contents/MacOS/GVPlayer
```

Create DMG:
```bash
macdeployqt build-release/GVPlayer.app -dmg
```

### Distribution Checklist

- [ ] Bundle all Qt libraries
- [ ] Bundle libmpv
- [ ] Bundle FFmpeg dependencies
- [ ] App icon (`.icns`) ✓
- [ ] `Info.plist` configured ✓
- [ ] App name and version set ✓
- [ ] Sign with Apple Developer Certificate
- [ ] Enable Hardened Runtime
- [ ] Submit to Apple Notary Service
- [ ] Staple the notarization ticket
- [ ] Create and sign DMG
- [ ] Test on a Mac without Homebrew

---

## Current Version

```
1.0.0
```

Bundle identifier:
```
com.yassersky.gvplayer
```

---

## Ultimate Goal

Produce a standalone, professional macOS video player:

```
GVPlayer.dmg
```

The user:
1. Opens the DMG
2. Drags GV Player to Applications
3. Runs it — no Qt, mpv, or Homebrew required
4. Opens and plays any common video format
5. Full playback control via UI

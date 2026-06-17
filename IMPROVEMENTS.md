# Changelog

This file summarizes the improvements added to GV Player.

## Video Display Fix

- Fixed audio-without-video issue by completing libmpv's draw cycle inside `QOpenGLWidget`.
- Added `mpv_render_context_update()` before rendering and `mpv_render_context_report_swap()` after.
- Enabled OpenGL context sharing in Qt via `Qt::AA_ShareOpenGLContexts`.
- Set hardware decoding to `hwdec=auto-copy` and disabled `vd-lavc-dr` for better macOS compatibility.

## AVI/x265 Non-Standard File Support

- Detects AVI files tagged as `x265` that FFmpeg/mpv reports as unknown codec.
- Creates a temporary MKV copy via `ffmpeg` with the video stream forced as HEVC.
- Plays the remuxed temporary file automatically instead of the original.
- Deletes the temporary file on player close.

> Requires `ffmpeg` and `ffprobe` to be installed on the system.

## Playback Controls

- Play / Pause toggle.
- Seek −5 seconds.
- Seek +5 seconds.
- Seek −5 minutes.
- Progress slider for scrubbing.
- Progress slider auto-updates during playback.

## Timer Display

- Timer next to the progress slider showing current position and total duration:

```
12s / 226s
```

## Playback Speed

- Speed buttons: `1×` `2×` `4×` `8×`
- Speed resets to `1×` when a new file is opened.
- Active speed button highlighted in blue.

## Zoom and Pan

- Zoom buttons: `−` `0` `+`
- Zoom percentage label (e.g. `100%`, `115%`, `200%`).
- Mouse wheel zoom.
- Click-and-drag pan after zooming in.
- Natural drag direction: dragging up moves the image up.
- `0` button resets zoom and pan to default.

## Zoom Quality

- Upscale/downscale filter: `ewa_lanczossharp` (best built-in filter in mpv).
- Anti-ringing: `scale-antiring=0.7` / `cscale-antiring=0.7` / `dscale-antiring=0.7`.
- `correct-downscaling=yes` and `sigmoid-upscaling=yes`.
- `linear-downscaling=yes` — downscaling in linear light for better accuracy.
- High-quality dithering: `dither=fruit`, `dither-depth=auto`, `temporal-dither=yes`.
- Banding removal: `deband=yes` (4 iterations, threshold 48, range 16, grain 12).
- Color management: `icc-profile=auto`, `target-prim=auto`, `target-trc=auto`.
- OpenGL upgraded to **4.1 Core Profile** (highest supported on macOS) with **sRGB color space**.

## UI Improvements

- Controls enabled/disabled based on whether a file is loaded.
- Visual indicators for current zoom level and playback speed.
- Consistent dark minimal interface.

## Branding & Open Source

- Renamed from MacVideoReader to **GV Player**.
- All UI strings translated to English.
- App icon generated (`.icns`) for macOS bundle.
- MIT license added.
- `README.md` and `.gitignore` added for GitHub publishing.

## Build Command

```bash
cmake --build build
```

Run:

```bash
open build/GVPlayer.app
```

#!/usr/bin/env bash
#
# Builds a self-contained GV Player .app and packages it as a DMG on the Desktop.
#
# The resulting bundle embeds Qt, libmpv, all media libraries, AND the
# ffmpeg/ffprobe command-line tools (used to remux non-standard x265 AVI
# files), so it runs on any Mac without Homebrew installed.
#
# Usage:  ./scripts/package_dmg.sh
#
set -euo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_DIR"

APP_NAME="GVPlayer"
VOLUME_NAME="GV Player 1.0.0"
BUILD_DIR="build-release"
APP="$BUILD_DIR/$APP_NAME.app"
FRAMEWORKS="$APP/Contents/Frameworks"
HELPERS="$APP/Contents/Helpers"
DMG_OUT="$HOME/Desktop/$APP_NAME.dmg"

QT_PREFIX="$(brew --prefix qt)"

echo "==> Configuring & building Release"
cmake -S . -B "$BUILD_DIR" -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$QT_PREFIX"
cmake --build "$BUILD_DIR"

echo "==> Bundling Qt frameworks (macdeployqt)"
# macdeployqt prints harmless rpath errors for optional Qt modules; ignore them.
"$QT_PREFIX/bin/macdeployqt" "$APP" || true

# macdeployqt sometimes leaves libbrotlicommon's id pointing at Homebrew.
if [ -f "$FRAMEWORKS/libbrotlicommon.1.dylib" ]; then
  install_name_tool -id "@executable_path/../Frameworks/libbrotlicommon.1.dylib" \
    "$FRAMEWORKS/libbrotlicommon.1.dylib" 2>/dev/null || true
fi

echo "==> Bundling ffmpeg / ffprobe and rewriting their library paths"
python3 - "$APP" << 'PYEOF'
import os, shutil, subprocess, stat, sys

APP = sys.argv[1]
FRAMEWORKS = os.path.join(APP, "Contents/Frameworks")
HELPERS = os.path.join(APP, "Contents/Helpers")
os.makedirs(HELPERS, exist_ok=True)

def sh(*a): return subprocess.run(a, capture_output=True, text=True)

def hb_deps(binary):
    deps = []
    for line in sh("otool", "-L", binary).stdout.splitlines()[1:]:
        path = line.strip().split(" (")[0].strip()
        if path.startswith("/opt/homebrew") or path.startswith("/usr/local"):
            deps.append(path)
    return deps

def make_writable(p):
    os.chmod(p, os.stat(p).st_mode | stat.S_IWUSR)

existing = set(os.listdir(FRAMEWORKS))
queue = []

for tool in ("ffmpeg", "ffprobe"):
    src = os.path.realpath(shutil.which(tool))
    dst = os.path.join(HELPERS, tool)
    shutil.copy2(src, dst)
    os.chmod(dst, 0o755)
    queue.append((dst, "exec"))
    print(f"    bundled {tool}")

while queue:
    binpath, kind = queue.pop()
    for dep in hb_deps(binpath):
        base = os.path.basename(dep)
        target = os.path.join(FRAMEWORKS, base)
        if base not in existing:
            shutil.copy2(os.path.realpath(dep), target)
            make_writable(target)
            sh("install_name_tool", "-id", f"@loader_path/{base}", target)
            existing.add(base)
            queue.append((target, "lib"))
            print(f"    copied dependency {base}")
        newref = (f"@executable_path/../Frameworks/{base}"
                  if kind == "exec" else f"@loader_path/{base}")
        sh("install_name_tool", "-change", dep, newref, binpath)
print("    library paths rewritten")
PYEOF

echo "==> Ad-hoc signing everything (required on Apple Silicon)"
find "$FRAMEWORKS" -name "*.dylib" -exec codesign --force --sign - {} \;
codesign --force --sign - "$HELPERS/ffmpeg"
codesign --force --sign - "$HELPERS/ffprobe"
codesign --force --deep --sign - "$APP"
codesign --verify --deep "$APP" && echo "    signature valid"

echo "==> Creating DMG at $DMG_OUT"
STAGING="$(mktemp -d)"
cp -R "$APP" "$STAGING/"
ln -s /Applications "$STAGING/Applications"
hdiutil create -volname "$VOLUME_NAME" -srcfolder "$STAGING" \
  -ov -format UDZO "$DMG_OUT"
rm -rf "$STAGING"

echo ""
echo "Done. DMG ready: $DMG_OUT"

# Windows installer

This produces a Windows installer named `SoftProjector_<version>_WinInstaller.exe`
(e.g. `SoftProjector_2.2_WinInstaller.exe`), like the one on softprojector.org.

## Automated (recommended) — GitHub Actions

The workflow `.github/workflows/windows-installer.yml` builds on a Windows
runner, deploys the Qt runtime with `windeployqt`, and compiles this Inno Setup
script.

- **Get a downloadable release link:** push a tag of the form `v<version>`:
  ```
  git tag v2.2
  git push origin v2.2
  ```
  The installer is built and attached to the matching GitHub **Release**, giving
  a real "Download Installer" link on the repo's Releases page.

- **Test without releasing:** Actions tab → *Windows Installer* → *Run workflow*,
  enter a version. The installer is uploaded as a downloadable build **artifact**.

The version in the installer name comes from the tag (or the manual input). The
app's own version is hardcoded in `src/sources/softprojector.cpp`
(`version_string = "2.2"`); keep the tag in sync with it.

## Manual (on a Windows machine)

Requires Qt 6.8.x for MSVC, the MSVC toolchain, and
[Inno Setup 6](https://jrsoftware.org/isdl.php).

```bat
cd src
qmake softProjector.pro CONFIG+=release
nmake
windeployqt --release --qmldir src\qml src\win32_build\bin\SoftProjector.exe
"%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe" /DAppVersion=2.2 packaging\windows\softprojector.iss
```

The installer lands in `packaging\windows\output\`.

## Notes

- Installs to `Program Files\SoftProjector`. The `spData.sqlite` database is not
  shipped — SoftProjector creates it in `C:\ProgramData\SoftProjector` on first
  launch (see `src/sources/main.cpp`), so it survives reinstalls/upgrades.
- This packaging is fork-only and cannot be built or verified from the Linux dev
  environment; it must run on Windows (locally or via the CI runner).

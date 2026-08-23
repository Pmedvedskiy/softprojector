# Windows installer

This produces a Windows installer named `SoftProjector_<version>_WinInstaller.exe`
(e.g. `SoftProjector_2.2.1_WinInstaller.exe`), like the one on softprojector.org.

## Automated (recommended): GitHub Actions

The workflow `.github/workflows/windows-installer.yml` builds on a Windows
runner, deploys the Qt runtime and the Visual C++ runtime with `windeployqt`,
copies the app translations, and compiles this Inno Setup script.

- **Get a downloadable release link:** push a tag of the form `v<version>`:
  ```
  git tag v2.2.1
  git push origin v2.2.1
  ```
  The installer is built and attached to the matching GitHub **Release**, giving
  a real "Download Installer" link on the repo's Releases page.

- **Test without releasing:** Actions tab, *Windows Installer*, *Run workflow*,
  enter a version. The installer is uploaded as a downloadable build **artifact**.
  (The button only appears when the workflow file exists on the default branch.)

The version in the installer name comes from the tag (or the manual input). The
app's own version is hardcoded in `src/sources/softprojector.cpp`
(`version_string = "2.2"`).

## Manual (on a Windows machine)

Requires Qt 6.8.x for MSVC, the MSVC toolchain, and
[Inno Setup 6](https://jrsoftware.org/isdl.php).

```bat
cd src
qmake softProjector.pro CONFIG+=release
nmake
cd ..
windeployqt --release --compiler-runtime --qmldir src\qml src\win32_build\bin\SoftProjector.exe
mkdir src\win32_build\bin\translations
copy src\translations\softpro_*.qm src\win32_build\bin\translations\
"%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe" /DAppVersion=2.2.1 packaging\windows\softprojector.iss
```

The installer lands in `packaging\windows\output\`.

## Notes

- Installs to `Program Files\SoftProjector` (needs admin rights to install).
- The `spData.sqlite` database is not shipped. SoftProjector creates it in
  `%ProgramData%\SoftProjector` on first launch (see `src/sources/main.cpp`), so
  it survives reinstalls and upgrades and is shared by every Windows account.
- To keep an existing song database from an older SoftProjector, copy its
  `spData.sqlite` into `C:\ProgramData\SoftProjector\` before the first launch.
- The Visual C++ runtime installer (`vc_redist.x64.exe`) is run silently at the
  end of setup when present, so the app starts on a machine without dev tools.
- This packaging is fork-only and cannot be built or verified from the Linux dev
  environment; it must run on Windows (locally or via the CI runner).

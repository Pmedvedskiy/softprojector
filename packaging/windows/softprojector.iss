; Inno Setup script for SoftProjector (Windows installer)
;
; Packages a windeployqt'd build of SoftProjector into a single
; SoftProjector_<version>_WinInstaller.exe.
;
; Build it with:
;   ISCC.exe /DAppVersion=2.2 packaging\windows\softprojector.iss
; (the CI workflow .github/workflows/windows-installer.yml does this for you)
;
; Paths below are relative to this .iss file (packaging\windows\).
; The app's spData.sqlite database is NOT shipped: when installed under
; Program Files, SoftProjector creates it in C:\ProgramData\SoftProjector on
; first launch (see src/sources/main.cpp).

#ifndef AppVersion
  #define AppVersion "2.2"
#endif

[Setup]
AppName=SoftProjector
AppVersion={#AppVersion}
AppPublisher=SoftProjector
AppPublisherURL=https://github.com/Pmedvedskiy/softprojector
DefaultDirName={autopf}\SoftProjector
DefaultGroupName=SoftProjector
DisableProgramGroupPage=yes
UninstallDisplayIcon={app}\SoftProjector.exe
SetupIconFile=..\..\src\softprojector.ico
OutputDir=output
OutputBaseFilename=SoftProjector_{#AppVersion}_WinInstaller
Compression=lzma2
SolidCompression=yes
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Additional icons:"; Flags: unchecked

[Files]
; The entire windeployqt output folder (exe + Qt DLLs + plugins + qml).
Source: "..\..\src\win32_build\bin\*"; DestDir: "{app}"; Flags: recursesubdirs createallsubdirs

[Icons]
Name: "{group}\SoftProjector"; Filename: "{app}\SoftProjector.exe"
Name: "{group}\Uninstall SoftProjector"; Filename: "{uninstallexe}"
Name: "{autodesktop}\SoftProjector"; Filename: "{app}\SoftProjector.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\SoftProjector.exe"; Description: "Launch SoftProjector"; Flags: nowait postinstall skipifsilent

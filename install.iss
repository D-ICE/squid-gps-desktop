; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

#define version "1.5"

[Setup]
AppName=Squid GPS Desktop
AppVersion={#version}
WizardStyle=modern
DefaultDirName={autopf}\squid-gps-desktop
DefaultGroupName=Squid
Compression=lzma2
SolidCompression=yes
OutputBaseFilename=SquidGPSDesktop-v{#version}

[Files]
Source: "install/*"; DestDir: "{app}"; Flags: recursesubdirs

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"

[Icons]
Name: "{userdesktop}\squid-gps-desktop"; Filename: "{app}\squid-gps-desktop.exe"; WorkingDir: "{app}"; Tasks: desktopicon
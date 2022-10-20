# Squid GPS Desktop

A simple application to connect NMEA-0183 complient sensors to [Squid X](https://www.squid-sailing.com/en/)

## How does it work ?

The application listens for NMEA frames on UDP and sends it to Squid X.  
Supported frames are:
* GGA: latitude, longitude
* GNS: latitude, longitude
* GLL: latitude, longitude
* RMA: latitude, longitude
* RMB: latitude, longitude, heading
* RMC: latitude, longitude, heading, speed
* VTG: heading, speed
* VHW: heading, speed
* HDT: heading
* HDG: heading

## Tests

We relied on the [NMEASimulator](https://github.com/panaaj/nmeasimulator) project
for development. Tests are executed with that tool running as UDP client on port
78001.

The squid related tests, also require a local SquidX running with the Squid gps
connection enabled.

## Package

To packge with an installer, we rely on [windeployqt](https://doc.qt.io/qt-6/windows-deployment.html).
After a successful build, copy the `squid-gps-desktop.exe` in the install folder. Then run:
```
C:\Qt6\6.4.0\mingw_64\bin\windeployqt.exe --qmldir ..\src\squid-gps-desktop\ .\squid-gps-desktop.exe
cp C:\Qt6\Tools\mingw1120_64\bin\libgcc_s_seh-1.dll .
cp C:\Qt6\Tools\mingw1120_64\bin\libstdc++-6.dll .
cp C:\Qt6\Tools\mingw1120_64\bin\libwinpthread-1.dll .
```
Finally, use the `install.iss` with innosetup to compile the installer.

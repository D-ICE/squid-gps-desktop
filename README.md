# Squid GPS Desktop

A simple application to connect NMEA-0183 complient sensors to [Squid X](https://www.squid-sailing.com/en/)

## How does it work ?

The application listens for NMEA frames on UDP and sends it to Squid X.  
Supported frames are:
*

## Tests

We relied on the [NMEASimulator](https://github.com/panaaj/nmeasimulator) project
for development. Tests are executed with that tool running as UDP client on port
78001.

The squid related tests, also require a local SquidX running with the Squid gps
connection enabled.

# MURI-HASP_2019
This is the 2019 HASP Repository. This will be used to store flight computer code for the SHADOMS payload.

This code operates the Teensy 3.5/3.6 Microcontroller for the 2019 HASP flight.
The goal of this flight is to collect data from the Plantower PMS-5003, the Alphasense OPC N3,
and the LOAC-R. Then, this data will be compared to examine how each particle detector operates
in the conditions of the upper atmosphere. This examination will be utilized to determine counter
effectiveness and calibration needs for future MURI research.

The code operates the particle counter power, writes the data of the Plantower to disk,
regulates temperature using active heating, runs a Coperinicus GPS, and controls uplink
and downlink with the main HASP gondola.

In this setup, both the Alphasense and the LOAC are running in a standalone mode where they 
record their own data.

# Four-Way Traffic Light Controller

**C | TI TM4C123G LaunchPad | Code Composer Studio**

*Rice University | ELEC 220 | Spring 2022*

## Project Overview

Designed and implemented a four-way traffic light controller using embedded C and the TI TM4C123G LaunchPad microcontroller. The system simulates traffic flow through a four-way intersection with coordinated signal sequencing and an emergency override mechanism.


## Hardware Components

- TI TM4C123G LaunchPad
- 4 breadboards
- 12 LEDs (4 red, 4 yellow, 4 green)
- 12 resistors (10 kΩ)
- SPST tilt switch
- Jumper wires

## System Operation

The controller manages four sets of traffic signals, representing northbound, southbound, eastbound, and westbound traffic.

**Normal Operation**
- Opposing directions operate with synchronized signals.
- Green signals transition to yellow before turning red.
- Traffic directions alternate through a repeating timed sequence.

**Emergency Override**
- A tilt switch serves as an emergency input.
- When activated, all four traffic signals turn red.
- The controller stops normal sequencing to simulate an emergency shutdown.

## Software Implementation

Developed in embedded C using Texas Instruments Code Composer Studio.

- Configured GPIO pins through direct register access and memory-mapped addresses.
- Used bitwise operations to control individual LEDs.
- Implemented timing and sequencing through counter-based loops and conditional statements.
- Monitored tilt-switch input to activate the emergency override.

## Circuit Diagram

The circuit consists of four LED assemblies connected to the TM4C123G LaunchPad through GPIO pins. Each assembly represents one direction of traffic, while the tilt switch provides an emergency control input.

![Four-Way Traffic Light Circuit Diagram](images/traffic-schematic)

[View Original Circuit Schematic](https://www.hackster.io/beats-by-dray/rush-hour-c27880#schematics)

## Project Demo

[![Watch the Traffic Light Controller Demo](https://img.youtube.com/vi/n9hjhn0f1n4/hqdefault.jpg)](https://youtube.com/shorts/n9hjhn0f1n4)

*Click the image to watch the system demonstration.*

## Project Documentation

[View Full Project on Hackster.io](https://www.hackster.io/beats-by-dray/rush-hour-c27880)

## Acknowledgments

Developed as a team project at Rice University under the guidance of Dr. Ray Simar.


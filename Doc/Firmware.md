## Firmware thrust++

Firmware is written using C++ and [ChibiOS RTOS](https://www.chibios.org) (ver. ChibiOS RT 7.1.0 : HAL 8.1.0). 

The protocol implemented for communication with the window application is a derivative of the MSP protocol used if flight controller firmware 
(see, e.g., iNav or Betaflight). The terminal variables, reading and setting them is also derived from the flight controller firmware.

At the current stage of development the rpm measurements done using IR sensor needs some work. Therefore, the ESC telemetry should be  currently used as a source of the revolution speed measurement.

# ThrustTool #

The ThrustTool is a DIY tool for measurements of an electric motor with propeller.
It allows to measure thrust, torque, rotation speed, battery voltage and current.

The project is by no means finished (especially firmware thrust++ and ThrustApp). However in current stage it works quite well allowing to obtain the results
(e.g., see auto generated report (in latex) [t-motor f1507 with duct](ThrustApp/data/f1507_with_duct.pdf)).

### The project includes:

* [the 3d-printed tool](Doc/Tool.md)
* [custom board based on STM32 F401 mcu](Doc/Board.md)
* [firmware for the custom board](Doc/Firmware.md)
* [QT application for measurement managing and reporting](Doc/ThrustApp.md)

### 3D render of the tool
![alt text](Doc/figs/thtool-1.png)




## Warning ##

The project is delivered as is. All responsibilities lies on the side of the user. 
Remember to take adequate security measures when testing any motor (especially powerful one).

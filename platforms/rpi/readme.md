# **Raspberry implementatin of WASM based turn Indicator**
## **Description**
`rpi_left_right_indicator` python program reads from a WASM binary which provides logic to control the LED's as per the push button inputs. This code is written in such a way that it also reads keyboard inputs to control the LED's.
## **Setup**
### **Software Prerequisites**
Install required python depedencies
```sh
pip3 install pywasm3
```
Install GPIO library for Raspberry Pi
```sh
sudo apt-get install rpi.gpio
```
Make sure the `turn_indicator.wasm` file is present in the same place where you are running this python program.
### **Hardware Connection**
You can specify any pins that are connected to the Raspberry Pi. By default, the program expects the hardware pheriperals to be connected to the following pins.

| Pheriperal's pin          | GPIO Pin | Physical Pin |
| :------------------------ | :------: | -----------: |
| Left LED Anode            |    4     |            7 |
| Left LED Cathode          |   GND    |            6 |
| Right LED Anode           |    3     |            5 |
| Right LED Cathode         |   GND    |            9 |
| Left Button Terminal 1    |    20    |           38 |
| Left Button Terminal 2    |   GND    |           14 |
| Right Button Terminal 1   |    21    |           40 |
| Right Button Terminal 2   |   GND    |           20 |
| Parking Button Terminal 1 |    16    |           36 |
| Parking Button Terminal 2 |   GND    |           25 |
>Note : You can connect the ground (GND) pins to any of the available GND pins on a device, as they are usually electrically connected together. Alternatively, you can short multiple ground wires together to create a single wire that can be connected to a device's GND pin. 

**Raspberry Pi pinout for reference** (Common for Pi 3B and 4)
```
,--------------------------------.
| ooooooooooooooooooo40   J8   +====
| 1ooooooooooooooooooo        | USB
|  Wi                         +====
|  Fi                            |
|        ,----.               +====
| |D|    |SoC |               | USB
| |S|    |    |               +====
| |I|    `----'                  |
|                   |C|     +======
|                   |S|     |   Net
| pwr        |HDMI| |I||A|  +======
`-| |--------|    |----|V|-------'

   3V3  (1) (2)  5V    
 GPIO2  (3) (4)  5V    
 GPIO3  (5) (6)  GND   
 GPIO4  (7) (8)  GPIO14
   GND  (9) (10) GPIO15
GPIO17 (11) (12) GPIO18
GPIO27 (13) (14) GND   
GPIO22 (15) (16) GPIO23
   3V3 (17) (18) GPIO24
GPIO10 (19) (20) GND   
 GPIO9 (21) (22) GPIO25
GPIO11 (23) (24) GPIO8 
   GND (25) (26) GPIO7 
 GPIO0 (27) (28) GPIO1 
 GPIO5 (29) (30) GND   
 GPIO6 (31) (32) GPIO12
GPIO13 (33) (34) GND   
GPIO19 (35) (36) GPIO16
GPIO26 (37) (38) GPIO20
   GND (39) (40) GPIO21
   ```

## **Run**
Once the setup is done, you can run the python program using
```
python rpi_left_right_indicator.py
```
Now, you can either press the push buttons or type '`l` or `L`' or '`r` or `R`' or `'p'` or `'P'` followed by `enter` to turn on left,right and parking indicators respectively.
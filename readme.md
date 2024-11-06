# Parallel gripper build guide

This is a build guide for the gripper used in the paper "Perception, Control and Hardware for In-Hand Slip-Aware Object Manipulation with Parallel Grippers", preprint available at: https://arxiv.org/abs/2410.19660. Everything is provided as is. 

The main repo for that project can be found at https://github.com/Gabrieleenx/Slip-Aware-Object-Manipulation-with-Parallel-Grippers 

![Alt Text](images/gripper.jpg)
## Overview 

This gripper was designed with the goal of having fast low-level force control and a high communication rate with a PC. We are controlling the gripper at 500 Hz, and it is possible to go faster. This readme is organized as follows:

* Overview of construction 
* Overview of code
* Part list
* Build guide

## Overview of construction
The construction of the gripper is fairly simple, it is after all only a parallel gripper. The main selling point is that we are using a bldc motor together with an encoder, which allows us to control it FOC and get good motor characteristics, we are using SimpleFOC https://simplefoc.com/. The motor is then connected to a 2:1 gear reduction, which dives a pully with a belt on it. The fingers of the gripper run along 2 linear bearings. The motor is controlled by an ESP32 S3 microcontroller. We also deigned a quick connector for the UR10 mount.

![Alt Text](images/drawing_inside.png)

## Overview of code

All the code running on the ESP32 is in the gripper_code folder. The code to communicate with it from a pc can be found at https://github.com/Gabrieleenx/Slip-Aware-Object-Manipulation-with-Parallel-Grippers. The code differs slightly from the simpleFOC library, we implemented our own encoder class, this was to utilize the ESP32 built in pulse counter peripherals, instead of using interupts. And we have a bare minimum serial communication setup. Some of the reasoning to why, is that we had problems with the ESP32 S3 Nano from Arduino, where it would randomly stop commnicating. Unlike many other development boards it does not have a UART to USB chip and the USB communication is handeled internally on the ESP32. To get it to work properly we had to compile our code in debug mode to force it to use another part of the USB peripheral, (there probably is a better solution, but it worked and I had already spent way too much time debugging this).

We are using ardunio IDE 2.3.2 on Ubuntu 20, with simpleFOC library 2.3.4. Compiled with debug mode (Hardware CDC). 

The gripper outputs angle of the motor [RAD], and take commands on target voltage [-20.0 to 20.0], the sign determines the driection. 

## Part list 

### Compnents 

2x NSK Linear Guides LH100120ANK1B01PN1, LH, https://se.rs-online.com/web/p/linear-guides/4979411

1x Arduino nano esp32 (not with pre soldered pins) https://store.arduino.cc/en-se/products/nano-esp32?srsltid=AfmBOoq2j8PJsWTmrSQZ9lXL6lKOGxVrW5B-yabFGoDSFEEn9P-Iw4cd

1x iPower Motor GM5208-24 Brushless Gimbal Motor https://eu.robotshop.com/products/ipower-motor-gm5208-24-brushless-gimbal-motor

1x AMT103-D2048-I8000-S https://www.mouser.se/ProductDetail/Same-Sky/AMT103-D2048-I8000-S?qs=Jm2GQyTW%2FbhTj4QqD8fAHQ%3D%3D&mgh=1&gad_source=1&gclid=Cj0KCQiAoeGuBhCBARIsAGfKY7zdJklvF7jDlw4l8NdwcRQ16M22bnk4nwzr7osbEtRulv0hirsh0TkaAoGrEALw_wcB

1x DRV8302 Motor Drive Module https://www.aliexpress.com/i/4000126430773.html?gatewayAdapt=glo2fra (This is completly overkill and something like https://simplefoc.com/simplefoc_mini_product_v1 would be more appropiate, but was out of stock at the time, if you want current sensing, make sure the driver hardware supports it)

1x OPTIBELT 4 T2,5 / 265 Timing Belt, 106 Teeth, 265mm Length, 4mm Width https://se.rs-online.com/web/p/timing-belts/2171565 

2x OPTIBELT Timing Belt Pulley, Aluminium 4 mm, 6 mm Belt Width x 2.5mm Pitch, 22 Tooth https://se.rs-online.com/web/p/belt-pulleys/2362794 (these do not come pre-drilled!)

1x RS PRO POM 30 Teeth Spur Gear, 1 Module, 6mm Bore Diam, 30mm Pitch Diam, 14mm Hub Diam https://se.rs-online.com/web/p/spur-gears/5217540?searchId=529fef86-fb48-4f9c-933e-d6b7e4e49f6e&gb=s

1x RS PRO POM 60 Teeth Spur Gear, 1 Module, 8mm Bore Diam, 60mm Pitch Diam, 18mm Hub Diam https://se.rs-online.com/web/p/spur-gears/5217641?searchId=f93c0d57-8f67-4b74-b46d-4ea63853b34f&gb=s


1x Bosch Rexroth 0.4m Long Steel Closed Bush Shaft, 6mm Shaft Diam. , Hardness 60HRC, h6 Tolerance https://se.rs-online.com/web/p/linear-shafts/7243399 (This needs to be cut, and it is quite hard, alternativly is to buy an aluminium shaft which is much easier to work with)

I2C level shifter (atleast 2 channels) https://www.electrokit.com/nivaomvandlare-4-kanaler-bidirektionell-i2c?gad_source=1&gclid=Cj0KCQiA_qG5BhDTARIsAA0UHSI5S-Kx_KQxKBlJcykZjvEIinTe4hLcBaB7p3R2S9C23zfkMdlB-KEaAmAnEALw_wcB

### Wires and screws 
M1.6 [2x 6mm]

M2.5 [2x 6mm, 2x 8mm, 2x 10 mm, 8x 16 mm, 4x 20mm]

M3 [13x 8mm, 8x 10mm, 2x 14mm,4x 20mm, 3x 30mm, 1x 40mm, 2x 50mm]

M4 [3x 25 mm (or slightly smaller)]

You will need nuts (also 2x square nuts for m3), wires and dupont cables. 

### 3D printed parts
See stl_files folder for all the stl files. Almost all parts are printed in PLA and most with around 30% infil. Some of the parts has been slightly modified from the gripper in the pictures, this minimize the amount of post modification needed to the parts. 

Parts printed in PLA:
* belt_tensioner.stl
* bottom_part.stl
* drill_pully_holder.stl
* finger.stl (printed at 100 % infil)
* gear_spacer.stl  (printed at 100 % infil)
* middle_part.stl
* motor_to_gear.stl
* top_part.stl

The following parts where printed with an SLS printer (plastic), but they could be printed with normal FDM printing. 
* body_shell_1.stl
* body_shell_2.stl
* quick_adapter.stl
* UR_10_mount.stl 

The contact pads are printed with dual material
* contact_pad.stl outer part TPU95
* contact_pad.stl inner part PLA

### Other parts

* You will need a powersuply that can deliver 20 V and atleast 1A (preferably more, I use a bench power suply with adjustable volatage and current limits). 
* You will also need to get or create power cables to the motor driver, I have a small connector close to the gripper so that it is easy to diconnect. 
* I would recommed getting velcro strips for cable managment, I bolted some of them under the gipper to reduce forces on the contacts.
* You will need a USB cable, make sure it is a decent cable, otherwise power and communication issues might occure. 

## Build/Assembly guide

This an assembly guide for the gripper, an overview of the parts can be seen below:

![Alt Text](images/all_parts.jpg)


Start with cuting the 6mm shaft into 34mm, 60mm, and 48 mm lengths.  
![Alt Text](images/shafts.jpg)

Then drill 6mm holes in the pulley, they will be pressfitted on, so sligthly smaller than 6mm is preferable. You can use the 3D printed fixture to secure the pulley when drilling. It is important that the hole is in the center and straight, would recomend using a pillar drill.  

![Alt Text](images/drill_pulley.jpg)


Put pulleys and bearings on, the gear big gear is put on with a spacer (3D printed) and two 2.5 mm diameter holes are drilled into the base of the gear, in each hole a M3, 6mm thread lenght countersunk screw are screwed in. The shaft is filed with flat spots for the scres to press against. The pulley is put on by pressfit and is tapped with a hammer at the base to secure it. 

2x m3 6mm thread countersunk screws. 

Insert M3 nuts,

![Alt Text](images/insert_nut2.jpg)

![Alt Text](images/insert_nut1.jpg)

![Alt Text](images/insert_nut3.jpg)


Insert the shaft with the big gear to the top part, and with the belt on.

![Alt Text](images/assemble_gear_belt.jpg)

Put the lower loop on the bearing 
1x m2.5 10 mm long, 

![Alt Text](images/belt_tension2.jpg)


Assemble the part, but only losely, this can be a bit finiky. 
2x m2.5, 20mm screw

![Alt Text](images/belt_tension1.jpg)

Wiggle in the two linear bearings, this it also a bit finiky. screw in with 2x m3 20mm and 4x m3 10mm, also be careful so that the slider doesn't fall of, some version of linear bearing do go easily together again (not sure about this one).   

![Alt Text](images/linear_screw.jpg)


Put the belt over the other pulley, and add the second loop 1x m2.5 10 mm, 2x m2.5, 20mm screw

![Alt Text](images/belt_tension3.jpg)

Install 2x m3 nuts, then install the middle plate with 2x m3 20 mm screws, make sure things can spin smoothly. 

![Alt Text](images/middle_insert_nut.jpg)
![Alt Text](images/assemble_middle_part.jpg)

Put the assembly asside for now. 

Drill a hole in the small gear so that the motor got gear adapter can connect to it. 

![Alt Text](images/drill_small_gear.jpg)

Mount the adapter on the motor, 4x m3 8mm 

![Alt Text](images/motor_to_gear.jpg)
Mount the gear to the motor with an M3 8mm screw. 
![Alt Text](images/gear_on_motor.jpg)


Insert 2x m2,5 nuts and 3x m4 nuts in the botom part

![Alt Text](images/bottom_insert_nuts.jpg)

Mount the motor, I use 5x m3 8mm screws 


![Alt Text](images/install_motor.jpg)

We can now mount the 2 assemblies together, with 2x m3 14mm and 2x m3 30 mm

![Alt Text](images/assmble_bottom_and_top.jpg)


Install the encoder with the tools provided and 2x m2.5 8mm screw. Make sure things still rotate freely. 

![Alt Text](images/encoder.jpg)

Nextup, wire all the electronics, schematics, the dirver mounts with 3x m3 8mm and the aruino nano ESP32 with 2x m1.5 6mm. Take care so that the pins on the arduino is soldered on the correct side. See figure below. You might need to flip the motor connector to get it rotating the correct direction. I bent the dupont connector 90 deg and put heat shrink on it, the normal dupont connector is to large in some instances, you could also solder the wires directly on. 
![Alt Text](images/drawing.png)

![Alt Text](images/all_ecelctronics.jpg)

![Alt Text](images/screw_nano.jpg)

I also put electrical tape around the voltage level shifter.

Mount the quick connector, 3x m4 25 mm, dont forget to place a m3 nut inside. The other part can den be attached with a single m3 20mm screw

![Alt Text](images/quick_connector.jpg)

Now tension the belt (make sure the belt tracks well on the pullys) and put on the finger base, 

![Alt Text](images/add_finger_base.jpg)

Each finger has a m3 50mm and square nut to increase stiffness. 

![Alt Text](images/finger_1.jpg)

Mount each finger with 4x m2.5 16mm screws

Drill hole in shell to put velcro strips for cable managment, I secure the velcro with a m2,5 screw and nut. Put 2x m3 nuts in the shell, one is on the outside and one is on the insisde. Mount the shells with 4x m3 10mm, 1x m3 30 mm, and 1x m3 40mm 

![Alt Text](images/shell1.jpg)


Thhe gripper should now be finished, if you have problems with belt skipping, then you need to tension the belt more. 

![Alt Text](images/gripper_done.jpg)


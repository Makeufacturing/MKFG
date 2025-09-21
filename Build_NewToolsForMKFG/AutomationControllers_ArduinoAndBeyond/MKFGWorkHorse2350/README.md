[MKFG](../../../) / [Build](../../) / [Controllers](../)

# MKFG WorkHorse 2350

#### Control Board

<img src="./CircuitBoard/_hero.jpg" alt="hero image" width="512" style="max-width: 512px;" /> 

In trying to create many of the needed automation tools for makeufacturing, it became clear that the "Arduino+shields" approach to making was itself a significant hurdle. Arduino is great as a core set of features to run code on a **modern microcontroller** and quickly plug in a huge range of I2C sensors, but to make a basic system that's also capable of **driving actuators** (DC motors, servos, solenoids, and steppers), handling **12V power** (along with a set of step-down regulators to 5V and 3.3V), and a basic feature complete **user interface for automation** (physical buttons, encoder knob, small screen, RGB status light, and a small speaker) **is still intimidating and highly complex.**

But actually putting these elements together can itself be a feat for many novice makers. High power systems can be difficult and dangerous to make. User interfaces are non-standardized and a design challenge in their own right. And controlling actuators like stepper motors, solenoids, and servos can be complex and error-prone.

After building out a few of these stacks of boards and shields for makeufacturing test systems, it became clear that all of that overhead was a steep hill to climb when **ideally a single board like this would just be a standard component for makeufacturing**. So we made one... not because we want to sell "yet another Arduino board" but instead because it **keeps the focus on actually making the tools that are needed**, not sourcing 5 different boards and spending your time wiring them all together and trying to make them talk to one another.

For many mkfg builds, the **specific core needs include**:

- <b>control</b>: a microcontroller such as an Arduino / Pi Pico / Teensy / ESP32 / etc.
- <b>inputs</b>: a universal I2C port for various plug-n-play sensors via Qwiic/STEMMA QT
- <b>user interface</b>: a common user interface with buttons, push-button knob/encoder, basic screen, multicolor LED, and notification speaker
- <b>automation outputs</b>: medium power actuator control outputs &mdash; upto 2 stepper motors, 2 DC motors/solenoids/vibe, and 2 servos
- <b>power</b>: integrated 12V power handling to safely supply the full range of needed voltages &mdash; 12V motors / 5V servos / 3.3V I2C/GPIO

So we made a simple credit card-sized board, the **MKFG WorkHorse**, that includes all of that together; it's built around **open source hardware, software, and mechanical engineering projects** that allow any curious tinkerers to get their hands dirty and build whatever is needed.

- <img src="./CircuitBoard/MKFG-WorkHorse-2350-1.2.7_schematic.png" alt="hero image" width="128" style="max-width: 128px;" /> <a href="./CircuitBoard/MKFG-WorkHorse-2350-1.2.7_schematic.pdf">MKFG WorkHorse schematic, v1.2.7</a> (PDF)
- <img src="./CircuitBoard/MKFG-WorkHorse-2350-1.2.7_pcb.png" alt="hero image" width="128" style="max-width: 128px;" /> <a href="./CircuitBoard/MKFG-WorkHorse-2350-1.2.7_KICAD_ARCHIVE.zip">MKFG WorkHorse PCB files, v1.2.7</a> (KiCAD Archive, ready to fab via JLCPCB)



#### Arduino Library

<img src="./ArduinoLibrary/_hero.png" alt="hero image" width="512" style="max-width: 512px;" /> 

Since we have a dedicated control board, we can now create a standardized library that takes care of all the low-level details in just a couple lines of code!

Install the <a href="./ArduinoLibrary/MKFG/">MKFG Arduino library</a> so your project can use it.

Then in your project, just `#include <MKFG.h>` and create an instance variable via `MKFGWorkHorse2350 mkfg = MKFGWorkHorse2350();`.

Calling `mkfg.setup()` initializes motor drivers, encoder, I2C OLED display, buttons, RGBW LED, motion control interrupts, etc. so everything is ready to go. Then in the loop() function of your code, call `mkfg.loop()` to handle anything that needs frequent updates (like checking of button states).

The MKFG library will then notify you when buttons are pressed or the knob is turned (via predefined callback functions). And when you want to initialize the TMC2209 stepper drivers or tell a motor to goto a position, you can just use the example code (it only takes a couple more lines) and it'll handle it for you. 

#### 3D Printable Case

<img src="./3DPrintableCase/_hero.png" alt="hero image" width="512" style="max-width: 512px;" /> 

While the control board is relatively safe (operating at 12V), it's still a good idea to keep it in a case to reduce the chance that it could get harmed or shorted if something (especially something metal) bumps into it. 

You can download the <a href="./3DPrintableCase/">MKFG case files</a> and then print them out on a 3D printer. Printing in **PETg** is recommended for electronic cases to handle higher temperatures if components get warm during use. You'll also want to have **four M3 bolts** handy to secure the top/bottom parts together. 

<img src="./3DPrintableCase/media/mkfg_workhorse_case1.jpg" width="512"/> 

 <img src="./3DPrintableCase/media/mkfg_workhorse_case2.jpg" height="128"/> <img src="./3DPrintableCase/media/mkfg_workhorse_case3.jpg" height="128"/>   

**To construct the case**:

* Print all parts (3 x buttons, 1 of everything else)
* Make sure the 3 heatsinks are placed on the control board's motor driver chips
* Place the buttons and LED diffuser into the case top
* Add the circuit board to the case top
* Place the case bottom (fits snugly, but nothing should be forced)
* Screw together with 4 x M3 bolts (7-10mm long)
* Pop on the encoder knob. 

<img src="./3DPrintableCase/media/mkfg_workhorse_case4.jpg" width="512"/> 

Voilà! Your MKFG board is now ready for action.







---



### :open_book: Open Source & Creative Commons

**Makeufacturing is fully open source**. It's released under 2 licenses for complete coverage:

* **All source code** (Arduino projects, C code, web code, etc.) is released under **[GNU GPL v3](https://www.gnu.org/licenses/gpl-3.0.en.html)**.

* **Everything else** (documentation, images, videos, write-ups, CAD files, drawings, etc.) is released under **[CC BY-SA 4.0](https://creativecommons.org/licenses/by-sa/4.0/)**.



### :speech_balloon: Questions / Suggestions / Feedback

Have an idea or found a bug? Let us know by **[filing an issue](https://github.com/Makeufacturing/MKFG/issues)** or sharing your **[thoughts/questions](https://github.com/Makeufacturing/MKFG/discussions)** with the community!



### :hand: Safety Disclaimer

> Working with automated equipment, electronics, power tools, hazardous chemicals, and DIY manufacturing systems requires proper precautions. Always wear appropriate safety gear including eye protection, gloves, and respiratory equipment when needed. Consult qualified professionals before working with electrical systems, chemicals, or complex machinery. Keep bystanders clear of operating equipment. Never leave automated systems unattended during operation. Ensure proper ventilation when working with fumes, dust, or chemical vapors. This information is for educational purposes only and does not replace professional safety training or equipment manufacturer instructions. This site and its contributors will not be held liable. **Use at your own risk.**



### :heart: Your support keeps us going :heart:

The Makeufacturing initiative is made possible by **[Makefast](https://makefastworkshop.com)**, a small, family-run prototyping and product development workshop located in Delaware, Ohio. After many attempts at manufacturing our own desktop fabrication products, it became clear how exciting (and technically difficult!) it was to create high quality products at scale out of our home using only DIY/Maker-level tools. We decided to openly catalog and share these learnings in the hopes that other makers around the world may benefit and further grow this **new, highly accessible, industrial revolution**.

If you appreciate this approach and want to see it grow, please consider contributing below. Your financial support allows us to put more time and effort into makeufacturing so that **more people can make more awesome things in more parts of the world**!

**[Support Makeufacturing with a contribution of any amount](https://buy.stripe.com/5kQfZi9WNeac3ba6trcQU02)**

Thanks, and **keep making awesome things!**
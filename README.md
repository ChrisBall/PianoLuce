# PianoLuce

PianoLuce is a keypress detector for pianos, based on the arduino platform. This is a halted project, but others trying to achieve
similar aims may find these resources useful.

This project took inspiration from the development on the Magnetic Resonator Piano (http://www.eecs.qmul.ac.uk/~andrewm/mrp.html) and similar, commercial keypress detectors like the Moog Piano Bar.

Most of the development was in aid of a performance by Marta Mołodyńska-Wheeler in June 2017, where live visuals wer generated from her PhD performance of Scriabin's 24 preludes.

My special thanks to: Alex & Marta, for their time, patience, and escape rooms; to James Medd (http://jamesmedd.co.uk/) for his assistance designing a mounting system; and to Alexander Lang (http://langster1980.blogspot.co.uk/), for his support.


## Description

PianoLuce uses side-mounted IR reflection detectors to estimate the velocity of keypresses. An IR reflection sensor (QRE1113) above each key measures reflected IR light. These sensors are switched on and read in sequence via 16 channel 4067 analog multiplexers. Each PCB spans an octave, and boards communicate via I2C, although SPI may be possible. A master controller is responsible for controlling calibration, receiving keypress data from each slave board, and sending midi.

## Resources
- Arduino Code
- Eagle Files
- Mount design and cad files

## Flaws

As mentioned, PianoLuce is only partially developed, and there are many factors to consider in an improved future version. Please bear these in mind before using this project as a basis for your own.
- Currently, keypresses on black keys are detected with ambient IR, which is obviously highly variable. More reliable measurement can be made by adding white stickers to each black key, and altering the current code to light the appropriate IR LED. A simpler to install system may use two reflection sensors to sense the void left by the black key when it is pressed.
- I2C is not designed for long range transmission: although the current system works (multi-slave setup), it may be susceptible to noise, and there is significant latency introduced by evaluating 7 slave boards. Improved data transmission between master and slaves should be implemented.

# PianoLuce

PianoLuce is a keypress detector for pianos, based on the arduino platform. This is a halted project, but others trying to achieve
similar aims may find these resources useful.

This project took inspiration from the development on the Magnetic Resonator Piano (http://www.eecs.qmul.ac.uk/~andrewm/mrp.html) and similar, commercial keypress detectors like the Moog Piano Bar.

Most of the development was in aid of a performance by Marta Mołodyńska-Wheeler in June 2017, where live visuals wer generated from her PhD performance of Scriabin's 24 preludes.

My special thanks to: Alex & Marta (http://www.martamolo.com/), for their time, patience, and escape rooms; to James Medd (http://jamesmedd.co.uk/) for his assistance designing a mounting system, and coming up with the name, PianoLuce; and to Alexander Lang (http://langster1980.blogspot.co.uk/), for his usual excellent support.


## Description

PianoLuce uses side-mounted IR reflection detectors to estimate the velocity of keypresses. An IR reflection sensor (QRE1113) above each key measures reflected IR light. These sensors are switched on and read in sequence via 16 channel 4067 analog multiplexers. Each PCB spans an octave, and boards communicate via I2C, although SPI may be possible. A master controller is responsible for controlling calibration, receiving keypress data from each slave board, getting data from a sustain pedal, and sending midi over USB.

## Resources
- [Arduino Code](https://github.com/ChrisBall/PianoLuce/tree/master/Arduino)
- [Eagle Files](https://github.com/ChrisBall/PianoLuce/tree/master/Eagle)
- [Mount design and cad files](https://github.com/ChrisBall/PianoLuce/tree/master/SVG)

## Flaws and possible improvements

As mentioned, PianoLuce is only partially developed, and there are many factors to consider in an improved future version. Please bear these points in mind before using this project as a basis for your own.
- Currently, keypresses on black keys are detected with ambient IR, which is obviously highly variable. More reliable measurement can be made by adding white stickers to each black key, and altering the current code to light the appropriate IR LED. A simpler to install system may use two reflection sensors to sense the void left by the black key when it is pressed.
- I2C is not designed for long range transmission: although the current system works (multi-slave setup), it may be susceptible to noise, and there is significant latency introduced by evaluating 7 slave boards. Improved data transmission between master and slaves should be implemented.
- The electronic design, while functional, could be simplified, made cheaper and smaller, and use a faster microcontroller from the STM32 series.
- The 10 pin IDC connectors chosen for board interconnects may disturb the performance of experienced pianists, particularly those with smaller hands as they will use the full length of each key. A lower profile interconnect would be ideal.
- There is no visual feedback: this was intentional, as it was deemed undesirable for our purposes, but in most cases it should be an option. LED feedback for each key is simple to add.

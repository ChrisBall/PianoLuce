# PianoLuce

PianoLuce is a keypress detector for pianos, based on the Arduino platform. This project is on hiatus, having achieved it's main aim, but others trying to achieve similar results may find these resources useful.

![](https://chrisball.github.io/PianoLuce/Media/WholeSetup.jpg "PianoLuce in use")

This project took inspiration from the development on the [Magnetic Resonator Piano](http://www.eecs.qmul.ac.uk/~andrewm/mrp.html) and similar, commercial keypress detectors like the Moog Piano Bar.

Most of the development was in aid of a performance by Marta Mołodyńska-Wheeler in June 2017, where live visuals were generated from her PhD performance of Scriabin's 24 preludes. The vision of the project was to re-create a modern day version of [Clavier à lumières](https://en.wikipedia.org/wiki/Clavier_%C3%A0_lumi%C3%A8res) by creating a non-intrusive device which could be attached to an expensive piano and create colored visuals.  Alex (Marta's husband), CEO of the IOT company [Kolomolo](www.kolomolo.com), connected with me during his research and commissioned the work.

My special thanks to: [Alex & Marta](http://www.martamolo.com/), for their time, patience, and escape rooms; to [James Medd](http://jamesmedd.co.uk/) for his assistance designing a mounting system, and coming up with the name, PianoLuce; and to [Alexander Lang](http://langster1980.blogspot.co.uk/), for his usual excellent support.


## Description

PianoLuce uses side-mounted IR reflection detectors to estimate the velocity of keypresses. An IR reflection sensor (QRE1113) above each key measures reflected IR light. These sensors are switched on and read in sequence by an ATMEGA328P via 16 channel 4067 analog multiplexers. Each PCB spans an octave (12 notes), and boards communicate via I2C, although SPI may be possible. A master controller is responsible for controlling calibration, receiving keypress data from each slave board, getting data from a sustain pedal, and sending midi over USB.

## Resources
- [Arduino Code](https://github.com/ChrisBall/PianoLuce/tree/master/Arduino)
- [Eagle Files](https://github.com/ChrisBall/PianoLuce/tree/master/Eagle)
- [Mount design and cad files](https://github.com/ChrisBall/PianoLuce/tree/master/SVG)
- [Other Media](https://github.com/ChrisBall/PianoLuce/tree/master/Media)

## Flaws and possible improvements

As mentioned, PianoLuce is only partially developed, and there are many factors to consider in an improved future version. Please bear these points in mind before using this project as a basis for your own.
- Currently, keypresses on black keys are detected with ambient IR, which is obviously highly variable. More reliable measurement can be made by adding white stickers to each black key, and altering the current code to light the appropriate IR LED. A simpler to install system may use two reflection sensors to sense the void left by the black key when it is pressed.
- I2C is not designed for long range transmission: although the current system works (multi-slave setup), it may be susceptible to noise, and there is significant latency introduced by evaluating 7 slave boards. Improved data transmission between master and slaves should be implemented.
- The electronic design, while functional, could be simplified, made cheaper and smaller, and use a faster microcontroller from the STM32 series.
- The 10 pin IDC connectors chosen for board interconnects may disturb the performance of experienced pianists, particularly those with smaller hands as they will use the full length of each key. A lower profile interconnect would be ideal.
- There is no visual feedback: this was intentional, as it was deemed undesirable for our purposes, but in most cases it should be an option. LED feedback for each key is simple to add.

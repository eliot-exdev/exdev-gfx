# About

This is a little technical demo of a voxelspace renderer written in C99.  
It was created for testing purposes and private research only.  
Currently I cannot say how it will develop in the future.

This demo provides 5 different worlds taken from the game Comanche developed by Novalogic.

- World 1: pyramid
- World 2: desert
- World 3: winter
- World 4: canyon
- World 5: rocky

You can select them by passing program arguments.

Have fun!

# Contact

Author: André Geisler  
Email: eliot@exdev.de

# Requirements

* MorphOs: any MorphOs system should work.  
  I tested it on a Mac Mini G4 Silent Upgrade with MorphOs 3.14.  
  You will need a 640x80x8 screen for fullscreen mode.
* Amiga Os: tested on Amiga 4000T, 060, Cybervision 64/3D, Aos 3.1.4.  
  It requires a stack size of 16384 or greater and a 320x240x8 screen mode.
* Linux: tested on Ubuntu 20.10 (amd64)
  It requires X11 server.

# Usage

Please start the binary (voxelspace_mos, voxelspace_060) from your shell.  
To get a complete arguments list of the program please start it with '-h'.  
You can quit the program by hitting ESC.  
Stack size of 16384 ius required

## Program arguments

voxelspace [ARGUMENTS]...  
arguments:  
-h, --help              print help message and exit  
-w, --world <number>    select world 1, 2, 3, 4 or 5 (default 1)  
-d, --demo              enable demo mode  
-v, --version           print version  

## Controls

w                   move up  
s                   move down  
a                   strafe left  
d                   strafe right  
cursors             navigate forward/backward and turn left/right  

## Options

ESC                 quit  
F1                  enable/disable show fps  
F2                  reduce rendering quality  
F3                  increase rendering quality  
F4                  decrease distance  
F5                  increase distance  
F6                  enable/disable skip x  

## ToDo

- go on with 3D rendering
- Mac Os X version (aarch64)

# History

- 0.6 2020-12-05 before XMas release
    - fixed crash due float overflow
    - implemented set render quality at runtime (+dither x option)
    - optimized a little bit more
    - implemented strafing left/right
    - add simple font rendering and add first font "mia1" (thanks to my daughter)
    - implemented showing frames per second

- 0.5 2020-11-22 8bit release
    - rendering in 8 bit, which should be faster
    - add demo option
    - add 5th world (rocky)
    - add linux version

- 0.4 2020-10-27 Before lockdown release
    - much faster world loading

- 0.3 2020-10-24 Third public release
    - this might be a little bit faster

- 0.2 2020-10-21 Second public release
    - add Amiga 060 binary
    - implemented better key event handling
    - add 4th world (canyon)

- 0.1 2020-10-15 First public release
    - hello world!
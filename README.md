# Snake for the La Fortuna Board #

Snake game for the LaFortuna from the University of Southampton.

You can either compile and flash from source or flash the included hex file in `bin/snake.hex`.

## Compiling ##
Use the makefile included by running `make`. This will compile from source to a hex file which is then flashed to the La Fortuna.

## Flashing ##
Flashing is easiest done by using the [lfutil](https://gist.github.com/Jamie-/c81a9d6a1c120fdd96a6090d72d207c9) script I've been expanding upon.

With this all you need to do is run `lfutil --load bin/snake.hex`.

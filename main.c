#include <avr/interrupt.h>
#include <util/delay.h>
#include "printf/printf.h"
#include "draw.h"
#include "input.h"

/* Game definitions */
#define TILESIZE 10
#define LOOPSPEED 200
#define DEBUG 0

/* Height and width are swapped to make landscape */
const uint8_t grid_width = LCDHEIGHT / TILESIZE;
const uint8_t grid_height = LCDWIDTH / TILESIZE;

/* Position values */
volatile uint8_t x = 5;  /* Current x value */
volatile uint8_t y = 5; /* Current y value */
volatile uint8_t px = 1; /* Previous x value */
volatile uint8_t py = 1; /* Previous y value */

volatile unsigned long tmillis = 0UL; /* Used to count milliseconds for game loop */

/* Current snake movement direction */
enum direction {
  NORTH,
  SOUTH,
  EAST,
  WEST
};

/* Current direction of movement */
volatile enum direction d = EAST;

/* Pre-loop initialisation code */
void init() {
  /* 8MHz clock, no prescaling (DS, p. 48) */
  CLKPR = (1 << CLKPCE);
  CLKPR = 0;

  init_lcd();
  /* Setup input pins */
  PORTC |= SWW | SWS | SWE | SWN;

  /* Setup game loop timer */
  TCCR0A = _BV(WGM01);  /* CTC Mode */
  TCCR0B = _BV(CS01) | _BV(CS00); /* Prescaler: F_CPU / 64 */
  OCR0A = (uint8_t)(F_CPU / (64.0 * 1000) - 1); /* 1 ms */
  TIMSK0 |= _BV(OCIE0A);  /* Enable timer interrupt */
}

/* Fill body of snake in given tile */
void fillBody(uint8_t gx, uint8_t gy) {
  fillSquare(10 * gx, 10 * gy, TILESIZE, ORANGE);
}

/* Clears a given tile */
void clearTile(uint8_t gx, uint8_t gy) {
  fillSquare(10 * gx, 10 * gy, TILESIZE, BLACK);
}

/* Draw game walls */
void drawWalls() {
  uint16_t col = GREEN;
  fillRect(0, 0, grid_width * TILESIZE - 1, TILESIZE - 1, col);
  fillRect(0, grid_height * TILESIZE - TILESIZE + 1, grid_width * TILESIZE, TILESIZE, col);
  fillRect(0, 0, TILESIZE - 1, grid_height * TILESIZE - 1, col);
  fillRect(grid_width * TILESIZE - TILESIZE + 1, 0, TILESIZE, grid_height * TILESIZE, col);
}

/* Main loop */
void main() {
  init();
  drawWalls();

  sei(); /* Enable global interupts */
  /* Button scanning Loop */
  do {
    if (NORTH_PRESSED) d = NORTH;
    if (SOUTH_PRESSED) d = SOUTH;
    if (EAST_PRESSED) d = EAST;
    if (WEST_PRESSED) d = WEST;
    _delay_ms(10);
  } while (x < grid_width-1 && x > 0 && y < grid_height-1 && y > 0);
  cli(); /* Disable global interupts */

  /* Game Over*/
  clear_screen();
  printf("Game Over!");
}

/* Game Loop */
ISR( TIMER0_COMPA_vect ) {
  cli();
  if (tmillis >= LOOPSPEED) {
    clearTile(px, py);
    fillBody(x, y);
    if (DEBUG) {
      display_move(0, 0);
      printf("X: %d, Y: %d",x ,y);
    }
    px = x;
    py = y;
    if (d == EAST) x++;
    if (d == SOUTH) y++;
    if (d == NORTH) y--;
    if (d == WEST) x--;
    tmillis = 0;
  } else {
    tmillis++;
  }
  sei();
}

























/**/

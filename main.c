#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h> //rand
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
volatile uint8_t y = 5;  /* Current y value */
volatile uint8_t px = 1; /* Previous x value */
volatile uint8_t py = 1; /* Previous y value */
volatile uint8_t fx = 0; /* Food x location */
volatile uint8_t fy = 0; /* Food y location */

volatile uint8_t score = 0;

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
  PORTE |= SWC;

  /* Setup game loop timer */
  TCCR0A = _BV(WGM01);  /* CTC Mode */
  TCCR0B = _BV(CS01) | _BV(CS00); /* Prescaler: F_CPU / 64 */
  OCR0A = (uint8_t)(F_CPU / (64.0 * 1000) - 1); /* 1 ms */
  TIMSK0 |= _BV(OCIE0A);  /* Enable timer interrupt */
}

/* Draw splash screen */
void drawSplash() {
  printf("Welcome to Snake!\n\n");
  printf("Press the center button to start.");
}

/* Fill body of snake in given tile */
void fillBody(uint8_t gx, uint8_t gy) {
  fillSquare(TILESIZE * gx, TILESIZE * gy, TILESIZE, ORANGE);
}

/* Draw food in given tile */
void drawFood(uint8_t gx, uint8_t gy) {
  fillSquare(TILESIZE * gx, TILESIZE * gy, TILESIZE, DARK_RED);
}

/* Clears a given tile */
void clearTile(uint8_t gx, uint8_t gy) {
  fillSquare(TILESIZE * gx, TILESIZE * gy, TILESIZE, BLACK);
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

  drawSplash();
  do {} while (!CENTER_PRESSED);
  clear_screen();

  for (;;) {
    drawWalls();
    /* Set font colouring */
    set_fg(BLACK);
    set_bg(GREEN);

    sei(); /* Enable global interupts */
    /* Button scanning loop */
    do {
      if (NORTH_PRESSED) d = NORTH;
      if (SOUTH_PRESSED) d = SOUTH;
      if (EAST_PRESSED) d = EAST;
      if (WEST_PRESSED) d = WEST;
      _delay_ms(10);
    } while (x < grid_width-1 && x > 0 && y < grid_height-1 && y > 0);
    cli(); /* Disable global interupts */

    /* Game Over */
    set_bg(BLACK);
    set_fg(YELLOW);
    clear_screen();
    display_move(130, 80);
    printf("Game Over!");
    display_move(135, 100);
    set_fg(WHITE);
    printf("Score: %d", score);
    set_fg(YELLOW);
    display_move(50, 120);
    printf("Press the centre button to play again.");

    /* Wait for user to restart game */
    do {} while (!CENTER_PRESSED);
    clear_screen();
    x=5;y=5;px=1;py=1;fx=0;fy=0;
    score = 0;
    d = EAST;
  }
}

/* Game Loop */
ISR( TIMER0_COMPA_vect ) {
  cli();
  if (tmillis >= LOOPSPEED) {
    display_move(TILESIZE * 10, 1);
    printf("Score: %d", score);

    if (fx == 0 && fy == 0) {
      /* Place new food */
      fx = rand() / (RAND_MAX / grid_width) + 1;
      fy = rand() / (RAND_MAX / grid_height) + 1;
    }
    if (x == fx && y == fy) {
      score++;
      fx = 0;
      fy = 0;
    }

    clearTile(px, py);
    if (fx != 0 && fy != 0) drawFood(fx, fy);
    fillBody(x, y);
    //printScore();

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

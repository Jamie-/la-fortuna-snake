#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h> //rand
#include <stdbool.h>
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
volatile bool gameOver = false;
volatile bool paused = false;
volatile bool centre = false;

volatile unsigned long tmillis = 0UL; /* Used to count milliseconds for game loop */

/* TAIL QUEUE */
#define maxTail 40
typedef struct {
  uint8_t x;
  uint8_t y;
} Position;

Position tailArray[maxTail];
int tFront = 0;
int tRear = -1;
int tLength = 0;

void addTail(Position p) {
  if (tLength < maxTail) {
    if (tRear == maxTail - 1) tRear = -1;
    tailArray[++tRear] = p;
    tLength++;
  }
}

Position pollTail() {
  Position p = tailArray[tFront++];
  if (tFront == maxTail) tFront = 0;
  tLength--;
  return p;
}
/* /TAIL QUEUE */

/* Current snake movement direction */
enum direction {
  NORTH,
  SOUTH,
  EAST,
  WEST
};

/* Current direction of movement */
volatile enum direction d = EAST;
/* Previous direction */
volatile enum direction pd = EAST;

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

/* Check if tile is in snake body */
bool isTileInBody(uint8_t gx, uint8_t gy) {
  bool hitTail = false;
  uint8_t count = 0;
  uint8_t i = tFront;
  while (count < tLength - 1 && !hitTail) {
    Position c = tailArray[i];
    hitTail = c.x == gx && c.y == gy;
    i++;
    if (i >= maxTail) i = 0;
    count++;
  }
  return hitTail;
}

/* Draw splash screen */
void drawSplash() {
  display_move(0, 60);
  printf("  _________ _______      _____   ____  __.___________\n");
  printf("/   _____/ \\      \\    /  _  \\ |    |/ _|\\_   _____/\n");
  printf("\\_____  \\  /   |   \\  /  /_\\  \\|      <   |    __)_\n");
  printf("/        \\/    |    \\/    |    \\    |  \\  |        \\\n");
  printf("/_______  /\\____|__  /\\____|__  /____|__ \\/_______  /\n");
  printf("        \\/         \\/         \\/        \\/        \\/");
  display_move(110, 160);
  printf("Welcome to Snake!");
  display_move(60, 180);
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
  fillRect(0, grid_height * TILESIZE - TILESIZE, grid_width * TILESIZE, TILESIZE, col);
  fillRect(0, 0, TILESIZE - 1, grid_height * TILESIZE - 1, col);
  fillRect(grid_width * TILESIZE - TILESIZE, 0, TILESIZE, grid_height * TILESIZE, col);
}

/* Main */
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

    Position start;
    start.x = x;
    start.y = y;
    addTail(start);
    do { /* Place food - making sure not under snake */
      fx = rand() % (grid_width - 2) + 1;
      fy = rand() % (grid_height - 2) + 1;
    } while (isTileInBody(x, y));

    _delay_ms(100); /* Wait to allow user to release button so game doesn't start paused */
    sei(); /* Enable global interupts for button checking */
    //bool centreToggle = false;

    /* Game loop */
    do {
      /* Run direction checks */
      if (
        (d == NORTH && pd == SOUTH) ||
        (d == SOUTH && pd == NORTH) ||
        (d == WEST && pd == EAST) ||
        (d == EAST && pd == WEST)
      ) {
        d = pd;
      } else {
        pd = d;
      }
      /* Perform food detection (i.e. eat food!) */
      if (x == fx && y == fy) {
        score++;
        do { /* Place new food - making sure not under snake */
          fx = rand() % (grid_width - 2) + 1;
          fy = rand() % (grid_height - 2) + 1;
        } while (isTileInBody(fx, fy) || (x == fx && y == fy));
      } else {
        Position p = pollTail();
        clearTile(p.x, p.y);
      }

      /* Update positions of objects on screen */
      drawFood(fx, fy);
      fillBody(x, y);
      Position h;
      h.x = x;
      h.y = y;
      addTail(h);

      /* Update score */
      display_move(140, 1);
      printf("Score: %d", score);

      /* Print current position to screen */
      if (DEBUG) {
        display_move(1, 1);
        printf("X: %d, Y: %d  ",fx ,fy);
      }

      /* Run tail collision detection */
      gameOver = isTileInBody(x, y);

      /* Handle movement*/
      px = x;
      py = y;
      if (d == EAST) x++;
      if (d == SOUTH) y++;
      if (d == NORTH) y--;
      if (d == WEST) x--;

      do {
        if (centre) {
          //centreToggle = true;
          paused = !paused;
          set_fg(WHITE);
          set_bg(BLACK);
          display_move(30, 30);
          printf("GAME PAUSED!");
        } else {
          //centreToggle = false;
          drawFood(fx, fy);
          fillBody(x, y);
        }

        _delay_ms(LOOPSPEED);
      } while (paused);
    } while (x < grid_width-1 && x > 0 && y < grid_height-1 && y > 0 && !gameOver);

    cli(); /* Disable global interupts */

    /* Game Over */
    set_bg(BLACK);
    set_fg(WHITE);
    clear_screen();
    display_move(130, 80);
    printf("Game Over!");
    display_move(135, 100);
    set_fg(YELLOW);
    printf("Score: %d", score);
    set_fg(WHITE);
    display_move(50, 120);
    printf("Press the centre button to play again.");

    /* Wait for user to restart game */
    do {} while (!CENTER_PRESSED);
    clear_screen();
    x=5;y=5;px=1;py=1;fx=0;fy=0;
    score = 0;
    d = EAST;
    pd = EAST;
    tFront = 0;
    tRear = -1;
    tLength = 0;
    gameOver = false;
    paused = false;
  }
}

/* Button scanning interrupt every 1ms */
ISR( TIMER0_COMPA_vect ) {
  cli();
  centre = CENTER_PRESSED;
  if (!paused) {
    if (NORTH_PRESSED && d != SOUTH) d = NORTH;
    if (SOUTH_PRESSED && d != NORTH) d = SOUTH;
    if (EAST_PRESSED && d != WEST)   d = EAST;
    if (WEST_PRESSED && d != EAST)   d = WEST;
  }
  sei();
}























/**/

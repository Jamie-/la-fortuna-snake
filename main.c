#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h> //rand
#include <stdbool.h>
#include "draw.h"
#include "input.h"

#define DEBUG 0 /* Debug mode, set to 1 for debug data */

/* Position values */
volatile uint8_t x = 5;  /* Current x value */
volatile uint8_t y = 5;  /* Current y value */
volatile uint8_t px = 1; /* Previous x value */
volatile uint8_t py = 1; /* Previous y value */
volatile uint8_t fx = 0; /* Food x location */
volatile uint8_t fy = 0; /* Food y location */

/* Shroom variables */
uint8_t sx = 0;      /* Shroom x location */
uint8_t sy = 0;      /* Shroom y location */
uint8_t shroomTimeout = 0;    /* Cycles until shroom respawns */
uint8_t shroomCycleCount = 0; /* Cycles since last shroom respawn */
bool showShroom = false;      /* Shroom shown */

volatile uint8_t score = 0;
volatile bool gameOver = false;

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

    shroomTimeout = rand() % 30 + 1; /* Set shroom timeout */

    sei(); /* Enable global interupts for button checking */

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
        } while (isTileInBody(fx, fy));
      } else {
        Position p = pollTail();
        clearTile(p.x, p.y);
      }

      /* Check shroom timeout */
      if (shroomCycleCount == shroomTimeout) {
        if (showShroom) clearTile(sx, sy); /* Clear old shroom sprite */
        showShroom = !showShroom; /* Toggle show shroom */
        do { /* Place new food - making sure not under snake */
          sx = rand() % (grid_width - 2) + 1;
          sy = rand() % (grid_height - 2) + 1;
        } while (isTileInBody(fx, fy));
        shroomTimeout = rand() % 40 + 1;
        shroomCycleCount = 0;
      } else {
        shroomCycleCount++;
      }

      /* Update positions of objects on screen */
      drawApple(fx, fy);
      if (showShroom) drawShroom(sx, sy);
      if (tLength > 0) fillBody(px, py);
      fillHead(x, y);
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

      gameOver = isTileInBody(x, y); /* Run tail collision detection */
      gameOver = x == sx && y == sy; /* Shroom check */

      /* Handle movement*/
      px = x;
      py = y;
      if (d == EAST) x++;
      if (d == SOUTH) y++;
      if (d == NORTH) y--;
      if (d == WEST) x--;

      /* Speed up game as score increases */
      if (score <= 5) _delay_ms(250);
      else if (score <= 10) _delay_ms(200);
      else if (score <= 15) _delay_ms(150);
      else _delay_ms(100);
    } while (x < grid_width-1 && x > 0 && y < grid_height-1 && y > 0 && !gameOver);

    cli(); /* Disable global interupts */

    /* Game Over */
    set_bg(BLACK);
    set_fg(WHITE);
    drawWalls();
    //clear_screen();
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
    showShroom = false;
    shroomCycleCount = 0;
  }
}

/* Button scanning interrupt every 1ms */
ISR( TIMER0_COMPA_vect ) {
  cli();
  if (NORTH_PRESSED && d != SOUTH) d = NORTH;
  if (SOUTH_PRESSED && d != NORTH) d = SOUTH;
  if (EAST_PRESSED && d != WEST)   d = EAST;
  if (WEST_PRESSED && d != EAST)   d = WEST;
  sei();
}























/**/

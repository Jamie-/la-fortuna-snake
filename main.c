#include <avr/io.h>
#include <util/delay.h>
#include "printf/printf.h"
#include "draw.h"

/* Game definitions */
#define TILESIZE 10

/* Height and width are swapped to make landscape */
uint8_t grid_width = LCDHEIGHT / TILESIZE;
uint8_t grid_height = LCDWIDTH / TILESIZE;

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

  uint8_t x = 5;  /* Current x value */
  uint8_t y = 5; /* Current y value */
  uint8_t px = -1; /* Previous x value */
  uint8_t py = -1; /* Previous y value */
  //fillBody(grid_width - 2, grid_height-2);

  /* Game Loop */
  do {
    fillBody(x, y);
    px = x;
    py = y;
    if (d == EAST) x++;
    if (d == SOUTH) y++;
    if (d == NORTH) y--;
    if (d == WEST) x--;
    _delay_ms(200);
    clearTile(px, py);
  } while (x < grid_width-1 && x > 0 && y < grid_height-1 && y > 0);

  /* Game Over*/
  clear_screen();
  printf("Game Over!");
}




























/**/

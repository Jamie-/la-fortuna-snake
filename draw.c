#include "draw.h"
#include "sprite.h"

/* Height and width are swapped to make landscape */
const uint8_t grid_width = LCDHEIGHT / TILESIZE;
const uint8_t grid_height = LCDWIDTH / TILESIZE;

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

/* Fill rectangle on screen */
void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t col) {
  rectangle r;
  r.top = y;
  r.bottom = y + h;
  r.left = x;
  r.right = x + w;
  fill_rectangle(r, col);
}

/* Fill square on screen */
void fillSquare(uint16_t x, uint16_t y, uint16_t s, uint16_t col) {
  fillRect(x, y, s-1, s-1, col);
}

/* Draw single pixel to screen */
void drawPixel(uint16_t x, uint16_t y, uint16_t col) {
  fillRect(x, y, 0, 0, col);
}

/* Fill body of snake in given tile */
void fillBody(uint8_t gx, uint8_t gy) {
  fillSquare(TILESIZE * gx, TILESIZE * gy, TILESIZE, ORANGE);
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

/* Draw sprite from colour array to screen */
void drawSprite(uint16_t x, uint16_t y, const uint16_t *sprite) {
  x *= TILESIZE;
  y *= TILESIZE;
  uint16_t ox = x + TILESIZE;
  uint8_t i;
  for (i = 0; i < 10 * TILESIZE; i++) {
    drawPixel(x, y, sprite[i]);
    x++;
    if (x >= ox) {
      x -= TILESIZE;
      y++;
    }
  }
}

/* Draw sprite from colour array to screen rotated 90 */
void drawSprite90(uint16_t x, uint16_t y, const uint16_t *sprite) {
  x = x * TILESIZE + 9;
  y *= TILESIZE;
  uint16_t oy = y + TILESIZE;
  uint8_t i;
  for (i = 0; i < 10 * TILESIZE; i++) {
    drawPixel(x, y, sprite[i]);
    y++;
    if (y >= oy) {
      y -= TILESIZE;
      x--;
    }
  }
}

/* Draw sprite from colour array to screen rotated 180 */
void drawSprite180(uint16_t x, uint16_t y, const uint16_t *sprite) {
  x *= TILESIZE;
  y = y * TILESIZE + 9;
  uint16_t ox = x + TILESIZE;
  uint8_t i;
  for (i = 0; i < 10 * TILESIZE; i++) {
    drawPixel(x, y, sprite[i]);
    x++;
    if (x >= ox) {
      x -= TILESIZE;
      y--;
    }
  }
}

/* Draw sprite from colour array to screen rotated 270 */
void drawSprite270(uint16_t x, uint16_t y, const uint16_t *sprite) {
  x *= TILESIZE;
  y *= TILESIZE;
  uint16_t oy = y + TILESIZE;
  uint8_t i;
  for (i = 0; i < 10 * TILESIZE; i++) {
    drawPixel(x, y, sprite[i]);
    y++;
    if (y >= oy) {
      y -= TILESIZE;
      x++;
    }
  }
}

/* Draw apple sprite */
void drawApple(uint16_t x, uint16_t y) {
  drawSprite(x, y, apple_sprite);
}

/* Draw shroom sprite */
void drawShroom(uint16_t x, uint16_t y) {
  drawSprite(x, y, shroom_sprite);
}

/* Draw snake blob sprite */
void drawSnakeBlob(uint16_t x, uint16_t y, enum direction d) {
  if (d == NORTH) drawSprite(x, y, snake_blob_sprite);
  else if (d == SOUTH) drawSprite180(x, y, snake_blob_sprite);
  else if (d == WEST) drawSprite270(x, y, snake_blob_sprite);
  else /* EAST */ drawSprite90(x, y, snake_blob_sprite);
}

/* Draw snake head sprite */
void drawSnakeHead(uint16_t x, uint16_t y, enum direction d) {
  if (d == NORTH) drawSprite(x, y, snake_head_sprite);
  else if (d == SOUTH) drawSprite180(x, y, snake_head_sprite);
  else if (d == WEST) drawSprite270(x, y, snake_head_sprite);
  else /* EAST */ drawSprite90(x, y, snake_head_sprite);
}

/* Draw snake tail sprite */
void drawSnakeTail(uint16_t x, uint16_t y, enum direction d) {
  if (d == NORTH) drawSprite(x, y, snake_tail_sprite);
  else if (d == SOUTH) drawSprite180(x, y, snake_tail_sprite);
  else if (d == WEST) drawSprite270(x, y, snake_tail_sprite);
  else /* EAST */ drawSprite90(x, y, snake_tail_sprite);
}

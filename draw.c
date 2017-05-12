#include "draw.h"
#include "sprite.h"

void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t col) {
  rectangle r;
  r.top = y;
  r.bottom = y + h;
  r.left = x;
  r.right = x + w;
  fill_rectangle(r, col);
}

void fillSquare(uint16_t x, uint16_t y, uint16_t s, uint16_t col) {
  fillRect(x, y, s-1, s-1, col);
}

void drawPixel(uint16_t x, uint16_t y, uint16_t col) {
  fillRect(x, y, 0, 0, col);
}

void drawSprite(uint16_t x, uint16_t y, const uint16_t *sprite) {
  x *= 10;
  y *= 10;
  uint16_t ox = x + 10;
  uint8_t i;
  for (i = 0; i < 100; i++) {
    drawPixel(x, y, sprite[i]);
    x++;
    if (x >= ox) {
      x -= 10;
      y++;
    }
  }
}

void drawApple(uint16_t x, uint16_t y) {
  drawSprite(x, y, apple_sprite);
}


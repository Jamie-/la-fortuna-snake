#include "draw.h"

void fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t col) {
  rectangle r;
  r.top = y;
  r.bottom = y + h;
  r.left = x;
  r.right = x + w;
  fill_rectangle(r, col);
}

void fillSquare(uint16_t x, uint16_t y, uint16_t s, uint16_t col) {
  fillRect(x, y, s, s, col);
}

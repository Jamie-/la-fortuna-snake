#include <stdint.h>
#include "lcd/lcd.h"
#include "printf/printf.h"

#define TILESIZE 10

extern const uint8_t grid_width;
extern const uint8_t grid_height;

void drawSplash();
void drawWalls();
void fillHead(uint8_t gx, uint8_t gy);
void fillBody(uint8_t gx, uint8_t gy);
void clearTile(uint8_t gx, uint8_t gy);
void drawApple(uint16_t x, uint16_t y);
void drawShroom(uint16_t x, uint16_t y);

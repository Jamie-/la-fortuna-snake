#include <avr/io.h>

#define SWC _BV(PE7)
#define SWW _BV(PC5)
#define SWS _BV(PC4)
#define SWE _BV(PC3)
#define SWN _BV(PC2)

#define CENTER_PRESSED  ((PINE & SWC) == 0)
#define WEST_PRESSED  ((PINC & SWW) == 0)
#define SOUTH_PRESSED ((PINC & SWS) == 0)
#define EAST_PRESSED  ((PINC & SWE) == 0)
#define NORTH_PRESSED ((PINC & SWN) == 0)

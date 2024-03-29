#ifndef PRESS_F_FONT_C
#define PRESS_F_FONT_C

#include <string.h>

#include "font.h"
#include "hw/system.h"

/* Define some helpers to make the font more readable from this source file */
/* Each font character is effectively 5x5 pixels, but is stored as 8x5 */
#define ______ 0x00
#define _____1 0x08
#define ____1_ 0x10
#define ____11 0x18
#define ___1__ 0x20
#define ___1_1 0x28
#define ___11_ 0x30
#define ___111 0x38
#define __1___ 0x40
#define __1__1 0x48
#define __1_1_ 0x50
#define __1_11 0x58
#define __11__ 0x60
#define __11_1 0x68
#define __111_ 0x70
#define __1111 0x78
#define _1____ 0x80
#define _1___1 0x88
#define _1__1_ 0x90
#define _1__11 0x98
#define _1_1__ 0xa0
#define _1_1_1 0xa8
#define _1_11_ 0xb0
#define _1_111 0xb8
#define _11___ 0xc0
#define _11__1 0xc8
#define _11_1_ 0xd0
#define _11_11 0xd8
#define _111__ 0xe0
#define _111_1 0xe8
#define _1111_ 0xf0
#define _11111 0xf8

typedef u8 char_t[5];

typedef struct font_t
{
  char_t zero, one, two, three, four, five, six, seven, eight, nine,
    g, question_mark, t, space, m, x, block, colon, hyphen,
    goalie_a, goalie_b, ball,
    line000, line180, line030, line045, line060, line240, line225, line210;
} font_t;

static font_t font_fairchild =
{
   {
      _11111,
      _1___1,
      _1___1,
      _1___1,
      _11111
   },
   {
      ___1__,
      ___1__,
      ___1__,
      ___1__,
      ___1__
   },
   {
      _11111,
      _____1,
      _11111,
      _1____,
      _11111
   },
   {
      _11111,
      _____1,
      _11111,
      _____1,
      _11111
   },
   {
      _1___1,
      _1___1,
      _11111,
      _____1,
      _____1
   },
   {
      _11111,
      _1____,
      _11111,
      _____1,
      _11111
   },
   {
      _11111,
      _1____,
      _11111,
      _1___1,
      _11111
   },
   {
      _11111,
      _____1,
      _____1,
      _____1,
      _____1
   },
   {
      _11111,
      _1___1,
      _11111,
      _1___1,
      _11111
   },
   {
      _11111,
      _1___1,
      _11111,
      _____1,
      _11111
   },
   {
      _11111,
      _1____,
      _1__11,
      _1___1,
      _11111
   },
   {
      _11111,
      _____1,
      ___111,
      ______,
      ___1__
   },
   {
      _11111,
      ___1__,
      ___1__,
      ___1__,
      ___1__
   },
   {
      ______,
      ______,
      ______,
      ______,
      ______
   },
   {
      _11111,
      _1_1_1,
      _1_1_1,
      _1_1_1,
      _1_1_1
   },
   {
      _1___1,
      __1_1_,
      ___1__,
      __1_1_,
      _1___1
   },
   /* 
      Interestingly, this graphic has all bits ON despite only reading 5 of
      the 8 bits per line.
   */
   {
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF
   },
   {
      ______,
      ___1__,
      ______,
      ___1__,
      ______
   },
   {
      ______,
      ______,
      _11111,
      ______,
      ______
   },
   {
      __1_1_,
      __1_1_,
      __1_1_,
      __1_1_,
      __1_1_
   },
   {
      _1_1__,
      _1_1__,
      _1_1__,
      _1_1__,
      _1_1__
   },
   {
      _11___,
      _11___,
      ______,
      ______,
      ______
   },
   {
      _____1,
      _____1,
      _____1,
      _____1,
      _____1
   },
   {
      ___1__,
      ___1__,
      ___1__,
      ___1__,
      ___1__
   },
   {
      ____1_,
      ____1_,
      ___1__,
      __1___,
      __1___
   },
   {
      _____1,
      ____1_,
      ___1__,
      __1___,
      _1____
   },
   {
      ______,
      ____11,
      ___1__,
      _11___,
      ______
   },
   {
      ______,
      _11___,
      ___1__,
      ____11,
      ______
   },
   {
      _1____,
      __1___,
      ___1__,
      ____1_,
      _____1
   },
   {
      __1___,
      __1___,
      ___1__,
      ____1_,
      ____1_
   }
};

static font_t font_cute =
{
   {
      ___11_,
      __1__1,
      _1___1,
      _1__1_,
      __11__
   },
   {
      _____1,
      ____1_,
      ___1__,
      __1___,
      _1____,
   },
   {
      __11__,
      _1__1_,
      ____1_,
      __11__,
      _1__11
   },
   {
      ___11_,
      _____1,
      _1_1_1,
      _1__1_,
      __11__
   },
   {
      _1__1_,
      _1__1_,
      _1__1_,
      __1111,
      ____1_
   },
   {
      ___111,
      ___1__,
      _1__1_,
      _1__1_,
      __11__
   },
   {
      _1____,
      _111__,
      _1__1_,
      _1__1_,
      __11__
   },
   {
      _1111_,
      ____1_,
      ___1__,
      __1___,
      _1____
   },
   {
      ____11,
      __11_1,
      _1__1_,
      _1__1_,
      __11__
   },
   {
      __11__,
      _1__1_,
      _1__1_,
      __111_,
      ____1_,
   },
   {
      __111_,
      _1____,
      _1__11,
      _1___1,
      __111_
   },
   {
      __111_,
      _1___1,
      ___11_,
      ______,
      ___1__
   },
   {
      _11111,
      ___1__,
      ___1__,
      ___1__,
      ___1__
   },
   {
      ______,
      ______,
      ______,
      ______,
      ______
   },
   {
      __1_1_,
      _1_1_1,
      _1_1_1,
      _1___1,
      _1___1
   },
   {
      _1___1,
      __1_1_,
      ___1__,
      __1_1_,
      _1___1
   },
   {
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF
   },
   {
      ______,
      ____1_,
      ______,
      __1___,
      ______
   },
   {
      ______,
      ______,
      _11111,
      ______,
      ______
   },
   {
      __1_1_,
      __1_1_,
      __1_1_,
      __1_1_,
      __1_1_
   },
   {
      _1_1__,
      _1_1__,
      _1_1__,
      _1_1__,
      _1_1__
   },
   {
      _11___,
      _11___,
      ______,
      ______,
      ______
   },
   {
      _____1,
      _____1,
      _____1,
      _____1,
      _____1
   },
   {
      ___1__,
      ___1__,
      ___1__,
      ___1__,
      ___1__
   },
   {
      ____1_,
      ____1_,
      ___1__,
      __1___,
      __1___
   },
   {
      _____1,
      ____1_,
      ___1__,
      __1___,
      _1____
   },
   {
      ______,
      ____11,
      ___1__,
      _11___,
      ______
   },
   {
      ______,
      _11___,
      ___1__,
      ____11,
      ______
   },
   {
      _1____,
      __1___,
      ___1__,
      ____1_,
      _____1
   },
   {
      __1___,
      __1___,
      ___1__,
      ____1_,
      ____1_
   }
};

static font_t font_skinny =
{
   {
      __111_,
      __1_1_,
      __1_1_,
      __1_1_,
      __111_
   },
   {
      ___1__,
      ___1__,
      ___1__,
      ___1__,
      ___1__
   },
   {
      __111_,
      ____1_,
      __111_,
      __1___,
      __111_
   },
   {
      __111_,
      ____1_,
      __111_,
      ____1_,
      __111_
   },
   {
      __1_1_,
      __1_1_,
      __111_,
      ____1_,
      ____1_
   },
   {
      __111_,
      __1___,
      __111_,
      ____1_,
      __111_
   },
   {
      __111_,
      __1___,
      __111_,
      __1_1_,
      __111_
   },
   {
      __111_,
      ____1_,
      ____1_,
      ____1_,
      ____1_
   },
   {
      __111_,
      __1_1_,
      __111_,
      __1_1_,
      __111_
   },
   {
      __111_,
      __1_1_,
      __111_,
      ____1_,
      __111_
   },
   {
      __111_,
      __1___,
      __1_1_,
      __1_1_,
      __111_
   },
   {
      __111_,
      ____1_,
      ___11_,
      ______,
      ___1__
   },
   {
      __111_,
      ___1__,
      ___1__,
      ___1__,
      ___1__
   },
   {
      ______,
      ______,
      ______,
      ______,
      ______
   },
   {
      __1_1_,
      __111_,
      __111_,
      __1_1_,
      __1_1_
   },
   {
      __1_1_,
      __1_1_,
      ___1__,
      __1_1_,
      __1_1_
   },
   {
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF
   },
   {
      ______,
      ___1__,
      ______,
      ___1__,
      ______
   },
   {
      ______,
      ______,
      _11111,
      ______,
      ______
   },
   {
      __1_1_,
      __1_1_,
      __1_1_,
      __1_1_,
      __1_1_
   },
   {
      _1_1__,
      _1_1__,
      _1_1__,
      _1_1__,
      _1_1__
   },
   {
      _11___,
      _11___,
      ______,
      ______,
      ______
   },
   {
      _____1,
      _____1,
      _____1,
      _____1,
      _____1
   },
   {
      ___1__,
      ___1__,
      ___1__,
      ___1__,
      ___1__
   },
   {
      ____1_,
      ____1_,
      ___1__,
      __1___,
      __1___
   },
   {
      _____1,
      ____1_,
      ___1__,
      __1___,
      _1____
   },
   {
      ______,
      ____11,
      ___1__,
      _11___,
      ______
   },
   {
      ______,
      _11___,
      ___1__,
      ____11,
      ______
   },
   {
      _1____,
      __1___,
      ___1__,
      ____1_,
      _____1
   },
   {
      __1___,
      __1___,
      ___1__,
      ____1_,
      ____1_
   }
};

void font_load(f8_system_t *system, const u8 id)
{
  if (system)
  {
    font_t *font = &font_fairchild;
    u8 hack = PF_COLOR_RED | PF_FONT_LINE_180;

    switch (id)
    {
    case FONT_CUTE:
      font = &font_cute;
      break;
    case FONT_SKINNY:
      font = &font_skinny;
    }

    /* Place the font data in memory. */
    f8_write(system, 0x767, font, sizeof(font_t));

    /**
     * The on-board games use the '1' character to form the vertical lines
     * that make up the playfield. This edit causes the game to instead use
     * the graphic for a straight paddle, which is identical.

     * This lets the correct lines get drawn even if the '1' character is
     * edited in the chosen font.
     */
    f8_write(system, 0x01FD, &hack, sizeof(hack));
    f8_write(system, 0x0453, &hack, sizeof(hack));
  }
}

void font_reset(f8_system_t *system)
{
  font_load(system, FONT_FAIRCHILD);
}

#endif

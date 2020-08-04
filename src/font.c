#ifndef PRESS_F_FONT_C
#define PRESS_F_FONT_C

#include <string.h>

#include "font.h"

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
   char_t zero, one, two, three, four, five, six, seven, eight, nine;
   /* char_t g, question_mark, t, space, m, x, block, colon, hyphen; */
   /* char_t goalie_a, goalie_b, apostrophe; */
} font_t;

static const font_t font_fairchild =
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
   }
};

static const font_t font_cute =
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
   }
};

static const font_t font_skinny =
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
   }
};

void font_load(channelf_t *system, const u8 id)
{
   if (system)
   {
      const font_t *font = &font_fairchild;

      switch (id)
      {
      case FONT_CUTE:
         font = &font_cute;
         break;
      case FONT_SKINNY:
         font = &font_skinny;
      }
      memcpy(&system->rom[0x767], font, sizeof(*font));
   }
}

void font_reset(channelf_t *system)
{
   font_load(system, 0);
}

#endif

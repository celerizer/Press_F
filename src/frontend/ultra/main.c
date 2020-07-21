#ifndef PRESS_F_ULTRA_MAIN_C
#define PRESS_F_ULTRA_MAIN_C

#include <nusys.h>
#include "rom.h"
#include "stage_emu.h"
#include "main.h"

/* "stage" identifier constants */
#define PRESS_F_ULTRA_IDLE -1
#define PRESS_F_ULTRA_RUN  0
#define PRESS_F_ULTRA_MENU 1

/* The stage number  */
volatile int stage;

NUContData contdata[4];

channelf_t ultra_channelf;

/*
================================================================================
   The call-back function 

   pendingGfx which is passed from Nusystem as the argument of the call-back   
   function is the total number of RCP tasks that are currently processing
   and waiting for the process.
================================================================================
*/

void cb_emu(int pending_gfx)
{
   /*
      Provide the display process if 2 or less RCP tasks are processing or 
      waiting for the process.
   */
   if(pending_gfx < 3)
   {
      update_emu();
      task_emu();
   }
}

/*
================================================================================
   Main
================================================================================
*/
void mainproc(void)
{
   /* The initialization of graphic  */
   nuGfxInit();

   /* The initialization of the controller manager  */
   nuContInit();

   /* Initialize the emulator state */
   memset(&ultra_channelf, 0, sizeof(ultra_channelf));
   memcpy(&ultra_channelf.rom[0x0000], bios_a, 0x0400);
   memcpy(&ultra_channelf.rom[0x0400], bios_b, 0x0400);
   //memcpy(&ultra_channelf.rom[0x0800], pac_bin, pac_bin_len);
   pressf_init(&ultra_channelf);
   //ultra_channelf.rom[0x0015] = 0x2B;
   //ultra_channelf.rom[0x0016] = 0x2B;

   stage = 0;

   while (TRUE)
   {
      switch (stage)
      {
      case PRESS_F_ULTRA_RUN:
         stage = PRESS_F_ULTRA_IDLE;
         init_emu();
         nuGfxFuncSet((NUGfxFunc)cb_emu);
         nuGfxDisplayOn();
         break;

      /* Not yet implemented */
      case PRESS_F_ULTRA_MENU:
         break;
      }
      
      while (stage == PRESS_F_ULTRA_IDLE);

      /* Clear the display */
      nuGfxDisplayOff();
   }
}

#endif

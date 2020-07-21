#ifndef PRESS_F_ULTRA_STAGE_EMU_C
#define PRESS_F_ULTRA_STAGE_EMU_C

#include <nusys.h>
#include "../../input.h"
#include "../../sound.h"

#include "main.h"
#include "graphic.h"
#include "stage_emu.h"

enum
{
   PF_MODE_STEP,
   PF_MODE_RUN,
   PF_MODE_TURBO,

   PF_MODE_LAST
};

/*
================================================================================
   Runtime data
================================================================================
*/
static u8 pf_frame_advance = FALSE;
static u8 pf_mode          = PF_MODE_RUN;

/* 
   64x64 4 bits-per-texel textures
*/
static u8 pf_texture_a[64 * 64 / 2];
static u8 pf_texture_b[64 * 64 / 2];

/* 
   RGBA5551 color lookup table
   Uses 32 bits of buffer at the beginning for 64-bit alignment
*/
static u16 pf_texture_lut[] =
{
   0x0D15, 0xEA5E,
   0xF995, 0x0657, 0x97E9, 0x49FD,
   0xE739, 0x0657, 0x97E9, 0x49FD,
   0xCEBF, 0x0657, 0x97E9, 0x49FD,
   0x0001, 0xFFFF, 0xFFFF, 0xFFFF,
};

/*
================================================================================
   Helper functions
================================================================================
*/

u32 align_64(void* dram)
{
   return ((u32)dram + 0x0C) & 0xFFFFFFF0;
}

/*
   Loads the palette used for the framebuffer into TMEM
*/
void load_palette(void)
{
  gDPSetTextureLUT(glistp++, G_TT_RGBA16);
  gDPLoadTLUT_pal16(glistp++, 0, align_64(pf_texture_lut));
}

/*
   Loads the texture used for a framebuffer segment into TMEM
*/
void load_texture_chunk(u32 index)
{
  gDPLoadTextureTile_4b
  (
    glistp++,                            /* Display list pointer */
    index ? pf_texture_b : pf_texture_a, /* Address of texture   */
    G_IM_FMT_CI,                         /* Texture image format */
    64,                                  /* Texture width        */
    64,                                  /* Texture height       */
    0,
    0,
    64 - 1,
    64 - 1,
    0,
    G_TX_WRAP | G_TX_NOMIRROR,
    G_TX_WRAP | G_TX_NOMIRROR,
    G_TX_NOMASK,
    G_TX_NOMASK,
    G_TX_NOLOD,
    G_TX_NOLOD
  );
}

void render(Dynamic* dynamicp)
{
  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->projection)),
    G_MTX_PROJECTION|G_MTX_LOAD|G_MTX_NOPUSH);
  gSPMatrix(glistp++,OS_K0_TO_PHYSICAL(&(dynamicp->modeling)),
    G_MTX_MODELVIEW|G_MTX_LOAD|G_MTX_NOPUSH);

  /* Set the texturing parameter */
  gSPTexture(glistp++,0x8000,0x8000,0,G_TX_RENDERTILE,G_ON);

  /* The synchronous setting between the rendering and  sub-attribute*/
  gDPPipeSync(glistp++);

  /* Set the RDP cycle type */
  gDPSetCycleType(glistp++, G_CYC_COPY);

  /* Set the rendering mode of the blender within RDP */
  gDPSetRenderMode(glistp++, G_RM_NOOP, G_RM_NOOP2);

  /* Set the textureLOD */
  gDPSetTextureLOD(glistp++, G_TL_TILE);

  /* Set the perspective of the texture map */
  gDPSetTexturePersp(glistp++, G_TP_NONE);

  /* Set the detail type */
  gDPSetTextureDetail(glistp++, G_TD_CLAMP);

  /* Set the texture filter type */
  gDPSetTextureFilter(glistp++, G_TF_BILERP);

  /* Set the conversion mode of the color space */
  gDPSetTextureConvert(glistp++, G_TC_FILT);

  /* Set the compare mode of the alpha value */
  gDPSetAlphaCompare(glistp++, G_AC_NONE);
  
  /* Set the dithering mode of the color data */
  gDPSetColorDither(glistp++, G_CD_DISABLE);

  /* Set the dithering mode of the alpha value */
  gDPSetAlphaDither(glistp++, G_AD_NOISE);

  load_palette();

  load_texture_chunk(0);
  gSPTextureRectangle
  (
    glistp++,
    (96 << 2),
    (88 << 2),
    ((96 + 64) << 2) - 1,
    ((88 + 64) << 2) - 1,
    G_TX_RENDERTILE,
    0,
    0,
    4 << 10,
    1 << 10
  );

  load_texture_chunk(1);
  gSPTextureRectangle
  (
    glistp++,
    (160 << 2),
    (88 << 2),
    ((160 + 64) << 2) - 1,
    ((88 + 64) << 2) - 1,
    G_TX_RENDERTILE,
    0,
    0,
    4 << 10,
    1 << 10
  );

  gSPTexture(glistp++,0, 0, 0, 0, G_OFF);
}

/*
================================================================================
   Stage logic
================================================================================
*/

void init_emu(void)
{
  osAiSetFrequency(PF_FREQUENCY);
}

/* Make the display list and activate the task  */
void task_emu(void)
{
  Dynamic* dynamicp;

  /* Specify the display list buffer  */
  dynamicp = &gfx_dynamic[gfx_gtask_no];
  glistp = &gfx_glist[gfx_gtask_no][0];

  /*  The initialization of RCP  */
  gfxRCPInit();

  /* Clear the frame and Z-buffer  */
  gfxClearCfb();

  /* projection,modeling matrix set */
  guOrtho(&dynamicp->projection,
    -(float)SCREEN_WD/2.0F, (float)SCREEN_WD/2.0F,
    -(float)SCREEN_HT/2.0F, (float)SCREEN_HT/2.0F,
    1.0F, 10.0F, 1.0F);
  guRotate   (&dynamicp->modeling,  0.0F, 0.0F, 0.0F, 1.0F);
  guTranslate(&dynamicp->translate, 0.0F, 0.0F, 0.0F);

  /* Draw a square  */
  render(dynamicp);

  gDPFullSync(glistp++);
  gSPEndDisplayList(glistp++);

  /* Activate the task and switch display buffers. */
  nuGfxTaskStart(&gfx_glist[gfx_gtask_no][0],
		 (s32)(glistp - gfx_glist[gfx_gtask_no]) * sizeof (Gfx),
		 NU_GFX_UCODE_F3DEX , NU_SC_NOSWAPBUFFER);

  /* Change character representation positions  */
  nuDebConTextPos(0,0,0);

  /* Display characters*/
  nuDebConPrintf(0, " Press F - Ultra64\n PC0: %04X PC1: %04X\n DC0: %04X DC1: %04X\n Acc: %02X Isa: %02X Sta: %02X\n r00: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n r10: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n r20: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n r30: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X\n io0: %02X io1:%02X io4:%02X\n%04X %04X %04X",
    ultra_channelf.pc0, ultra_channelf.pc1, ultra_channelf.dc0, ultra_channelf.dc1,

    ultra_channelf.c3850.accumulator, ultra_channelf.c3850.isar, ultra_channelf.c3850.status_register,

    ultra_channelf.c3850.scratchpad[0x00], ultra_channelf.c3850.scratchpad[0x01], ultra_channelf.c3850.scratchpad[0x02], ultra_channelf.c3850.scratchpad[0x03],
    ultra_channelf.c3850.scratchpad[0x04], ultra_channelf.c3850.scratchpad[0x05], ultra_channelf.c3850.scratchpad[0x06], ultra_channelf.c3850.scratchpad[0x07],
    ultra_channelf.c3850.scratchpad[0x08], ultra_channelf.c3850.scratchpad[0x09], ultra_channelf.c3850.scratchpad[0x0a], ultra_channelf.c3850.scratchpad[0x0b],
    ultra_channelf.c3850.scratchpad[0x0c], ultra_channelf.c3850.scratchpad[0x0d], ultra_channelf.c3850.scratchpad[0x0e], ultra_channelf.c3850.scratchpad[0x0f],

    ultra_channelf.c3850.scratchpad[0x10], ultra_channelf.c3850.scratchpad[0x11], ultra_channelf.c3850.scratchpad[0x12], ultra_channelf.c3850.scratchpad[0x13],
    ultra_channelf.c3850.scratchpad[0x14], ultra_channelf.c3850.scratchpad[0x15], ultra_channelf.c3850.scratchpad[0x16], ultra_channelf.c3850.scratchpad[0x17],
    ultra_channelf.c3850.scratchpad[0x18], ultra_channelf.c3850.scratchpad[0x19], ultra_channelf.c3850.scratchpad[0x1a], ultra_channelf.c3850.scratchpad[0x1b],
    ultra_channelf.c3850.scratchpad[0x1c], ultra_channelf.c3850.scratchpad[0x1d], ultra_channelf.c3850.scratchpad[0x1e], ultra_channelf.c3850.scratchpad[0x1f],

    ultra_channelf.c3850.scratchpad[0x20], ultra_channelf.c3850.scratchpad[0x21], ultra_channelf.c3850.scratchpad[0x22], ultra_channelf.c3850.scratchpad[0x23],
    ultra_channelf.c3850.scratchpad[0x24], ultra_channelf.c3850.scratchpad[0x25], ultra_channelf.c3850.scratchpad[0x26], ultra_channelf.c3850.scratchpad[0x27],
    ultra_channelf.c3850.scratchpad[0x28], ultra_channelf.c3850.scratchpad[0x29], ultra_channelf.c3850.scratchpad[0x2a], ultra_channelf.c3850.scratchpad[0x2b],
    ultra_channelf.c3850.scratchpad[0x2c], ultra_channelf.c3850.scratchpad[0x2d], ultra_channelf.c3850.scratchpad[0x2e], ultra_channelf.c3850.scratchpad[0x2f],

    ultra_channelf.c3850.scratchpad[0x30], ultra_channelf.c3850.scratchpad[0x31], ultra_channelf.c3850.scratchpad[0x32], ultra_channelf.c3850.scratchpad[0x33],
    ultra_channelf.c3850.scratchpad[0x34], ultra_channelf.c3850.scratchpad[0x35], ultra_channelf.c3850.scratchpad[0x36], ultra_channelf.c3850.scratchpad[0x37],
    ultra_channelf.c3850.scratchpad[0x38], ultra_channelf.c3850.scratchpad[0x39], ultra_channelf.c3850.scratchpad[0x3a], ultra_channelf.c3850.scratchpad[0x3b],
    ultra_channelf.c3850.scratchpad[0x3c], ultra_channelf.c3850.scratchpad[0x3d], ultra_channelf.c3850.scratchpad[0x3e], ultra_channelf.c3850.scratchpad[0x3f],

    ultra_channelf.io[0], ultra_channelf.io[1], ultra_channelf.io[4], pf_texture_a, pf_texture_b, align_64(pf_texture_lut));

  /* Write characters on the frame buffer */
  nuDebConDisp(NU_SC_SWAPBUFFER);

  /* Switch display list buffers  */
  gfx_gtask_no ^= 1;
}

u8 update_input(void)
{
   /* Get data from all controllers */
   nuContDataGetExAll(contdata);

   /* Handle console input */
   set_input_button(0, INPUT_TIME,       (contdata[0].button & L_TRIG)     ? TRUE : FALSE);
   set_input_button(0, INPUT_MODE,       (contdata[0].button & R_TRIG)     ? TRUE : FALSE);
   set_input_button(0, INPUT_HOLD,       (contdata[0].button & B_BUTTON)   ? TRUE : FALSE);
   set_input_button(0, INPUT_START,      (contdata[0].button & A_BUTTON)   ? TRUE : FALSE);

   /* Handle left hand-controller input */
   set_input_button(1, INPUT_RIGHT,      (contdata[0].button & R_JPAD)     ? TRUE : FALSE);
   set_input_button(1, INPUT_LEFT,       (contdata[0].button & L_JPAD)     ? TRUE : FALSE);
   set_input_button(1, INPUT_BACK,       (contdata[0].button & D_JPAD)     ? TRUE : FALSE);
   set_input_button(1, INPUT_FORWARD,    (contdata[0].button & U_JPAD)     ? TRUE : FALSE);
   set_input_button(1, INPUT_ROTATE_CCW, (contdata[0].button & L_CBUTTONS) ? TRUE : FALSE);
   set_input_button(1, INPUT_ROTATE_CW,  (contdata[0].button & R_CBUTTONS) ? TRUE : FALSE);
   set_input_button(1, INPUT_PULL,       (contdata[0].button & U_CBUTTONS) ? TRUE : FALSE);
   set_input_button(1, INPUT_PUSH,       (contdata[0].button & D_CBUTTONS) ? TRUE : FALSE);

   /* Handle right hand-controller input */
   set_input_button(4, INPUT_RIGHT,      (contdata[1].button & R_JPAD)     ? TRUE : FALSE);
   set_input_button(4, INPUT_LEFT,       (contdata[1].button & L_JPAD)     ? TRUE : FALSE);
   set_input_button(4, INPUT_BACK,       (contdata[1].button & D_JPAD)     ? TRUE : FALSE);
   set_input_button(4, INPUT_FORWARD,    (contdata[1].button & U_JPAD)     ? TRUE : FALSE);
   set_input_button(4, INPUT_ROTATE_CCW, (contdata[1].button & L_CBUTTONS) ? TRUE : FALSE);
   set_input_button(4, INPUT_ROTATE_CW,  (contdata[1].button & R_CBUTTONS) ? TRUE : FALSE);
   set_input_button(4, INPUT_PULL,       (contdata[1].button & U_CBUTTONS) ? TRUE : FALSE);
   set_input_button(4, INPUT_PUSH,       (contdata[1].button & D_CBUTTONS) ? TRUE : FALSE);

   /* Switch emulation speed mode when START is pressed */
   if (contdata[0].trigger & START_BUTTON)
   {
      if (!pf_frame_advance)
      {
         pf_mode++;
         if (pf_mode == PF_MODE_LAST)
           pf_mode = PF_MODE_STEP;
      }
      pf_frame_advance = FALSE;
   }
   else if (contdata[0].trigger & Z_TRIG)
      pf_frame_advance = TRUE;
   else if (pf_frame_advance)
      return FALSE;

   return TRUE;
}

void update_press_f(void)
{
   /* Handle emulation */
   switch (pf_mode)
   {
   /* 1 frame = 1 frame */
   case PF_MODE_RUN:
      pressf_run(&ultra_channelf);
      break;

   /* 1 frame = 1 operation */
   case PF_MODE_STEP:
      pressf_step(&ultra_channelf);
      break;

   /* 1 frame = 2 frames */
   case PF_MODE_TURBO:
      pressf_run(&ultra_channelf);
      pressf_run(&ultra_channelf);
      break;
   }
}

void update_video(void)
{
   draw_frame_ultra(ultra_channelf.vram, pf_texture_a, pf_texture_b);
}

void update_audio(void)
{
   sound_write();
   osAiSetNextBuffer(samples, sizeof(samples));
}

void update_emu(void)
{ 
   if (update_input())
   {
      update_press_f();
      update_video();
      update_audio();
   }
}

#endif

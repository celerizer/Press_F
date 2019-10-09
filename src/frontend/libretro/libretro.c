#include <libretro.h>
#include <streams/file_stream.h>
#include <string/stdstring.h>

#include "emu.h"
#include "file.h"
#include "input.h"
#include "screen.h"
#include "sound.h"

static channelf_t retro_channelf;

static u16  screen_buffer[128 * 64];
static char system_dir   [1024];

/* libretro callbacks */
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;
static retro_log_printf_t log_cb;
static struct retro_rumble_interface rumble;
static retro_video_refresh_t video_cb;

void display_message(const char *msg)
{
   char *str = (char*)calloc(4096, sizeof(char));
   struct retro_message rmsg;

   snprintf(str, 4096, "%s", msg);
   rmsg.frames = 300;
   rmsg.msg = str;
   environ_cb(RETRO_ENVIRONMENT_SET_MESSAGE, &rmsg);
}

bool load_system_file(char *filename, u8 *rom_data, u16 rom_size)
{
   char rom_filename[4096];

   snprintf(rom_filename, 4096, "%s%c%s", system_dir, '/', filename);
   rom_filename[sizeof(rom_filename) - 1] = '\0';
   if (filestream_exists(rom_filename))
   {
      RFILE *file = filestream_open(rom_filename, RETRO_VFS_FILE_ACCESS_READ, RETRO_VFS_FILE_ACCESS_HINT_NONE);

      if (file)
      {
         filestream_read(file, rom_data, rom_size);
         filestream_close(file);
      }
      if (log_cb)
         log_cb(RETRO_LOG_INFO, "[QUASI88]: Loaded %s from system_dir (0x%08X)\n", filename, rom_size);
   }
   else
   {
      if (log_cb)
         log_cb(RETRO_LOG_ERROR, "[QUASI88]: Couldn't find %s in system_dir\n", filename);
      return false;
   }
  
  return true;
}

/* libretro API */

void retro_init(void)
{
   char *dir = NULL;

   pressf_init(&retro_channelf);
   
   if (!environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log_cb))
      log_cb = NULL;
   if (!environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) || !dir)
   {
      if (log_cb)
         log_cb(RETRO_LOG_ERROR, "[PRESS F]: Couldn't find system dir\n");
   }
   else
   {
      snprintf(system_dir, sizeof(system_dir), "%s", dir);
      load_system_file("sl31253.rom", &retro_channelf.rom[0x0000], 0x400);
      load_system_file("sl31254.rom", &retro_channelf.rom[0x0400], 0x400);
   }
}

void retro_reset(void)
{
   pressf_reset(&retro_channelf);
}

bool retro_load_game(const struct retro_game_info *info)
{
   if (info && !string_is_empty(info->path))
      return load_cartridge(&retro_channelf, info->data, info->size);
   else
      return true;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num_info)
{
   return false;
}

void retro_unload_game()
{
   
}

void handle_input(void)
{
   input_poll_cb();
   set_input_button(0, INPUT_TIME,       input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L));
   set_input_button(0, INPUT_MODE,       input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT));
   set_input_button(0, INPUT_HOLD,       input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R));
   set_input_button(0, INPUT_START,      input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START));

   set_input_button(1, INPUT_RIGHT,      input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT));
   set_input_button(1, INPUT_LEFT,       input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT));
   set_input_button(1, INPUT_BACK,       input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN));
   set_input_button(1, INPUT_FORWARD,    input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP));
   set_input_button(1, INPUT_ROTATE_CCW, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y));
   set_input_button(1, INPUT_ROTATE_CW,  input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A));
   set_input_button(1, INPUT_PULL,       input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X));
   set_input_button(1, INPUT_PUSH,       input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B));

   set_input_button(4, INPUT_RIGHT,      input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT));
   set_input_button(4, INPUT_LEFT,       input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT));
   set_input_button(4, INPUT_BACK,       input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN));
   set_input_button(4, INPUT_FORWARD,    input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP));
   set_input_button(4, INPUT_ROTATE_CCW, input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y));
   set_input_button(4, INPUT_ROTATE_CW,  input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A));
   set_input_button(4, INPUT_PULL,       input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X));
   set_input_button(4, INPUT_PUSH,       input_state_cb(1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B));
}

void retro_run(void)
{
   handle_input();
   pressf_run(&retro_channelf);

   sound_write();
   audio_batch_cb(samples, samples_this_frame);
   sound_empty();

   draw_frame_rgb565(retro_channelf.vram, screen_buffer);
   video_cb(screen_buffer, 128, 64, 128 * 2);
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "Press F";
   info->library_version  = GIT_VERSION;
   info->need_fullpath    = false;
   info->valid_extensions = "chf|bin";
   info->block_extract    = false;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   memset(info, 0, sizeof(*info));
   info->geometry.base_width   = 128;
   info->geometry.base_height  = 64;
   info->geometry.max_width    = 128;
   info->geometry.max_height   = 64;
   info->geometry.aspect_ratio = 2;
   info->timing.fps            = 60;
   info->timing.sample_rate    = 44100;
}

void retro_deinit()
{
}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned in_port, unsigned device)
{
}

void retro_set_environment(retro_environment_t cb)
{
   static const struct retro_variable vars[] = 
   {
      { "press_f_option", "Option; enabled|disabled"},
      { NULL, NULL },
   };
   static const struct retro_controller_description port[] = {
      { "Hand-Controller", RETRO_DEVICE_JOYPAD },
      { 0 },
   };
   static const struct retro_controller_info ports[] = {
      { port, 2 },
      { port, 2 },
      { NULL, 0 },
   };
   struct retro_input_descriptor desc[] = {
      /* Console buttons */
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,      "TIME (1)" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "MODE (2)" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      "HOLD (3)" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "START (4)" },

      /* Left controller */
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "Right" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "Backward" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "Forward" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      "Rotate Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "Rotate Right" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "Pull Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      "Plunge Down" },

      /* Right controller */
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "Right" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "Left" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "Backward" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "Forward" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      "Rotate Left" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "Rotate Right" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "Pull Up" },
      { 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      "Plunge Down" },

      { 0 },
   };
   enum retro_pixel_format rgb565 = RETRO_PIXEL_FORMAT_RGB565;
   bool support_no_game = true;
   
   environ_cb = cb;
   cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);
   cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO,   (void*)ports);
   cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT,      &rgb565);
   cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME,   &support_no_game);
   cb(RETRO_ENVIRONMENT_SET_VARIABLES,         (void*)vars);
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
   audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

size_t retro_serialize_size(void)
{
   /* We want to use everything except the ROM data */
   return sizeof(channelf_t) - (ROM_CART_SIZE + ROM_BIOS_SIZE * 2);
}

/* Store savestates as big-endian and byteswap on LE platforms */
void serialize_add_value(void **dest, const void *src, u8 size)
{
//#ifdef MSB_FIRST
   memcpy(*dest, src, size);
   *dest += size;
/*#else
   u8 i, j;

   for (i = size; i != 0; --i)
   {
      *((u8*)*dest) = ((u8*)src)[i];
      *dest += 1;
   }
#endif*/
}

void unserialize_add_value(void *dest, const void **src, u8 size)
{
//#ifdef MSB_FIRST
   memcpy(dest, *src, size);
   *src += size;
/*#else
   u8 i, j;

   for (i = size; i != 0; --i)
   {
      ((u8*)dest)[i] = *((u8*)*src);
      *src += 1;
   }
#endif*/
}

void serialize_add_bytes(void **dest, const void *src, u32 size)
{
   memcpy(*dest, src, size);
   *dest += size;
}

void unserialize_add_bytes(void *dest, const void **src, u32 size)
{
   memcpy(dest, *src, size);
   *src += size;
}

bool retro_serialize(void *data, size_t size)
{
   void** i = &data;

   serialize_add_value(i, &retro_channelf.dc0,   2);
   serialize_add_value(i, &retro_channelf.dc1,   2);
   serialize_add_value(i, &retro_channelf.pc0,   2);
   serialize_add_value(i, &retro_channelf.pc1,   2);
   serialize_add_bytes(i, &retro_channelf.c3850, sizeof(c3850_t));
   serialize_add_bytes(i, &retro_channelf.io,    IO_PORTS);
   serialize_add_bytes(i, &retro_channelf.vram,  VRAM_SIZE);

   return true;
}

bool retro_unserialize(const void *data, size_t size)
{
   const void** i = &data;

   unserialize_add_value(&retro_channelf.dc0,   i, 2);
   unserialize_add_value(&retro_channelf.dc1,   i, 2);
   unserialize_add_value(&retro_channelf.pc0,   i, 2);
   unserialize_add_value(&retro_channelf.pc1,   i, 2);
   unserialize_add_bytes(&retro_channelf.c3850, i, sizeof(c3850_t));
   unserialize_add_bytes(&retro_channelf.io,    i, IO_PORTS);
   unserialize_add_bytes(&retro_channelf.vram,  i, VRAM_SIZE);
   force_draw_frame();

   return true;
}

void *retro_get_memory_data(unsigned type)
{
   if (type == RETRO_MEMORY_SYSTEM_RAM)
      return &retro_channelf.c3850.scratchpad;
   else if (type == RETRO_MEMORY_VIDEO_RAM)
      return &retro_channelf.vram;
   else
      return NULL;
}

size_t retro_get_memory_size(unsigned type)
{
   if (type == RETRO_MEMORY_SYSTEM_RAM)
      return 64;
   else if (type == RETRO_MEMORY_VIDEO_RAM)
      return VRAM_SIZE;
   else
      return 0;
}

void retro_cheat_reset(void)
{
}

void retro_cheat_set(unsigned a, bool b, const char *c)
{
}

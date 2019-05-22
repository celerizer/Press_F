#include <libretro.h>
#include <streams/file_stream.h>
#include <string/stdstring.h>

#include "file.h"
#include "screen.h"
#include "emu.h"

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
  
   struct retro_input_descriptor desc[] = {
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "Right" },

      { 0 },
   };
   environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);
}

void retro_reset(void)
{
}

bool retro_load_game(const struct retro_game_info *info)
{
   load_cartridge(&retro_channelf, info->data, info->size);
   
   return true;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num_info)
{
   return false;
}

void retro_unload_game()
{
   
}

void retro_run(void)
{
   pressf_step(&retro_channelf);
   draw_frame_rgb565(retro_channelf.vram, screen_buffer);
   video_cb(screen_buffer, 128, 64, 128 * 2);
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "Press F";
   info->library_version  = "0.0";
   info->need_fullpath    = false;
   info->valid_extensions = "bin";
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
      { "Retro Joypad", RETRO_DEVICE_JOYPAD },
      { 0 },
   };
   static const struct retro_controller_info ports[] = {
      { port, 2 },
      { NULL, 0 },
   };
   
   environ_cb = cb;
   cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);
   cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars);
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
   return 0;
}

bool retro_serialize(void *data, size_t size)
{
   return false;
}

bool retro_unserialize(const void *data, size_t size)
{
   return false;
}

void *retro_get_memory_data(unsigned type)
{
   if (type == RETRO_MEMORY_SYSTEM_RAM)
      return &retro_channelf.c3850.scratchpad;
   else
      return NULL;
}

size_t retro_get_memory_size(unsigned type)
{
   if (type == RETRO_MEMORY_SYSTEM_RAM)
      return 64;
   else
      return 0;
}

void retro_cheat_reset(void)
{
}

void retro_cheat_set(unsigned a, bool b, const char *c)
{
}
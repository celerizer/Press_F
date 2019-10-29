#ifndef PRESS_F_F8_DEVICE_H
#define PRESS_F_F8_DEVICE_H

#include "../types.h"

typedef struct f8_device_t
{
   u16 pc0;
   u16 pc1;

   /* What virtual address range does this device map to? */ 
   u16 address;
   u16 length;
   
   /* Device-specific behavior */
   void *device;
   void *init (void *device);
   void *free (void *device);
   void *reset(void *device);
} f8_device_t;

#endif

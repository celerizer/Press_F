#ifndef PRESS_F_2102_C
#define PRESS_F_2102_C

#include <string.h>

#include "f8_device.h"
#include "2102.h"

/* Fairchild 2102/2102L (1024 x 1 Static RAM) */
/*    ==================    */
/* A6 ||  1        16 || A7 */
/* A5 ||  2        15 || A6 */
/* RW ||  3        14 || A9 */
/* A1 ||  4        15 || CS */
/* A2 ||  5        12 || O  */
/* A3 ||  6        11 || I  */
/* A4 ||  7        10 || NA */
/* A0 ||  8         9 || NA */
/*    ==================    */

/* The pins are wired oddly in Videocarts that use this chip. */

/*              7   6   5   4   3   2   1   0  */
/* port a (p20) OUT  -   -   -  IN  A2  A3  RW */
/* port b (p21) A9  A8  A7  A1  A6  A5  A4  A0 */

/* Used for the last 2 addressing bits and to send/receive data */
/* --- */
/* Hooked to port 25 on Videocart 10 */
/* Hooked to port 21 on Videocart 18 */
void f2012_function_a(f8_device_t *device, u8 *port_data)
{
   f2102_t *m_f2102 = (f2102_t*)device->device;
   u8 *address;
   u8 addressed_bit;

   /* The other function handles all addressing bits besides 2 and 3 */
   m_f2102->selected_address &= 0x03F3;
 
   /* P2 -> A2 */
   m_f2102->selected_address += (*port_data & B00000100);
   /* P1 -> A3 */
   m_f2102->selected_address += (*port_data & B00000010) << 2;

   address       = &device->data[m_f2102->selected_address / 8];
   addressed_bit = m_f2102->selected_address % 8;

   /* Are we writing data? */
   if (*port_data & 0x01)
   {
   //   *address = (*port_data & 0x08) ? *address &
   }
     
}

/* Used to set 8 of the addressing bits. Does not modify port data. */
/* --- */
/* Hooked to port 24 on Videocart 10 */
/* Hooked to port 20 on Videocart 18 */
void f2012_function_b(f8_device_t *device, u8 *port_data)
{
   f2102_t *m_f2102 = (f2102_t*)device;
   u8 data = *port_data;

   /* The other function handles addressing bits 2 and 3 */
   m_f2102->selected_address &= 0x000C;

   /* Rearrange the other bits into this weird order */
   /* P0 -> A0 */
   m_f2102->selected_address +=  data & B00000001;
   /* P4 -> A1 */
   m_f2102->selected_address += (data & B00010000) >> 3;
   /* P1 -> A4 */
   m_f2102->selected_address += (data & B00000010) << 3;
   /* P2 -> A5 */
   m_f2102->selected_address += (data & B00000100) << 3;
   /* P3 -> A6 */
   m_f2102->selected_address += (data & B00001000) << 3;
   /* P5 -> A7 */
   m_f2102->selected_address += (data & B00100000) << 2;
   /* P6 -> A8 */
   m_f2102->selected_address += (data & B01000000) << 2;
   /* P7 -> A9 */
   m_f2102->selected_address += (data & B10000000) << 2;
}

void f2102_init(f8_device_t *device)
{
   f2102_t *m_f2102 = (f2102_t*)device->device;

   m_f2102 = (f2102_t*)malloc(sizeof(f2102_t));
   device->data = (u8*)calloc(F2102_SIZE, 1);
}

#endif

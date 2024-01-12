#ifndef PRESS_F_2102_C
#define PRESS_F_2102_C

#include <stdlib.h>
#include <string.h>

#include "2102.h"

static const char *name = "Fairchild 2102/2102L (1024 x 1 Static RAM)";
static int type = F8_DEVICE_2102;

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

#define BIT_RW  B00000001
#define BIT_IN  B00001000
#define BIT_OUT B10000000

F8D_OP_OUT(f2102_out_address)
{
  f2102_t *m_f2102 = (f2102_t*)device->device;
  u16 address = 0;

  /* P0 -> A0 */
  address |= (value.u & B00000001);
  /* P4 -> A1 */
  address |= (value.u & B00010000) >> 3;
  /* P1 -> A4 */
  address |= (value.u & B00000010) << 3;
  /* P2 -> A5 */
  address |= (value.u & B00000100) << 3;
  /* P3 -> A6 */
  address |= (value.u & B00001000) << 3;
  /* P5 -> A7 */
  address |= (value.u & B00100000) << 2;
  /* P6 -> A8 */
  address |= (value.u & B01000000) << 2;
  /* P7 -> A9 */
  address |= (value.u & B10000000) << 2;

  m_f2102->address = (m_f2102->address & B00001100) | address;

  *io_data = value;
}

/**
 * Used for the last 2 addressing bits and to send/receive data.
 * Hooked to port 25 on Videocart 10
 * Hooked to port 21 on Videocart 18
 */
F8D_OP_OUT(f2102_out_write)
{
  f2102_t *m_f2102 = (f2102_t*)device->device;
  u16 address = 0;
  f8_byte *data;
  int bit;

  /* P2 -> A2 */
  address |= (value.u & B00000100);
  /* P1 -> A3 */
  address |= (value.u & B00000010) << 2;

  m_f2102->address = (m_f2102->address & B11110011) | address;

  data = &device->data[address / 8];
  bit = (1 << (address % 8));

  /* Are we writing data? */
  if (value.u & BIT_RW)
    data->u = (value.u & BIT_IN) ? (data->u & bit) : (data->u & ~bit);
  /* No, we're reading it. */
  else
    io_data->u = (data->u & bit) ? (value.u & BIT_OUT) : (value.u & ~BIT_OUT);
}

void f2102_init(f8_device_t *device)
{
   if (!device)
     return;
   else
   {
     f2102_t *m_f2102 = (f2102_t*)calloc(sizeof(f2102_t), 1);
     device->device = m_f2102;
     device->data = m_f2102->data;
     device->name = name;
     device->type = type;
     device->flags = F8_NO_ROMC;
   }
}

void f2012_serialize(void *buffer, unsigned size, unsigned *offset, f8_device_t *device)
{
  /*
  f2102_t *m_f2102 = (f2102_t*)device->device;

  if (buffer)
    memcpy(buffer + *offset, m_f2102->data, sizeof(f2102_t));
  *offset += sizeof(f2102_t);
  */
}

void f2102_unserialize(void *buffer, unsigned size, unsigned *offset, f8_device_t *device)
{
  /*
  f2102_t *m_f2102 = (f2102_t*)device->device;

  if (buffer)
    memcpy(m_f2102->data, buffer + *offset, sizeof(f2102_t));
  *offset += sizeof(f2102_t);
  */
}

#endif

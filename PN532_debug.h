#ifndef __DEBUG_H__
#define __DEBUG_H__

//#define PN532_DEBUG

#include <stdio.h>

#ifdef PN532_DEBUG
#ifndef PN532_LOG
#define PN532_LOG printf
#endif
#define DMSG(...)       PN532_LOG(__VA_ARGS__)
#define DMSG_STR(str)   PN532_LOG("%s\n", str)
#define DMSG_HEX(num)   PN532_LOG(" %02X", (uint8_t)(num))
#define DMSG_INT(num)   PN532_LOG(" %d", (int)(num))
#else
#define DMSG(args...)
#define DMSG_STR(str)
#define DMSG_HEX(num)
#define DMSG_INT(num)
#endif

#endif

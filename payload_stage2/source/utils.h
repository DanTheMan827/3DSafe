/*
*   utils.h
*/

#pragma once

#include "types.h"
#include <stdbool.h>

u32 waitInput(void);
void mcuShutDown(void);
void mcuReboot(void);
void error(const char *message, bool fatal);
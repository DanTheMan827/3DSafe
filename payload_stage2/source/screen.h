#pragma once 

#include "types.h"

#define SPACING_Y   10
#define SPACING_X   8
#define COLOR_TITLE 0xFF9900
#define COLOR_WHITE 0xFFFFFF
#define COLOR_RED   0x0000FF
#define COLOR_BLACK 0x000000

void setFramebuffers();
void clearScreens();
void turnOnBacklight();
void drawCharacter(char character, int posX, int posY, u32 color);
int drawString(const char *string, int posX, int posY, u32 color);
void drawImage(char * path, u16 width, u16 height, s16 x, s16 y);
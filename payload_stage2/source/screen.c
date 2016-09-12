#include "screen.h"
#include "types.h"
#include "font.h"
#include "i2c.h"
#include <string.h>
#include "memory.h"
#include "fatfs/ff.h"

#define SCREEN_SIZE	400 * 240 * 3 / 4 //yes I know this is more than the size of the bootom screen
#define SCREEN_TOP_WIDTH 400
#define SCREEN_TOP_HEIGHT 240

static const struct fb {
    u8 *top_left;
    u8 *top_right;
    u8 *bottom;
} *const fb = (struct fb *)0x23FFFE00;

void drawCharacter(char character, int posX, int posY, u32 color)
{
    u8 *const select = fb->top_left;

    for(int y = 0; y < 8; y++)
    {
        char charPos = font[character * 8 + y];

        for(int x = 7; x >= 0; x--)
            if ((charPos >> x) & 1)
            {
                int screenPos = (posX * SCREEN_TOP_HEIGHT * 3 + (SCREEN_TOP_HEIGHT - y - posY - 1) * 3) + (7 - x) * 3 * SCREEN_TOP_HEIGHT;

                select[screenPos] = color >> 16;
                select[screenPos + 1] = color >> 8;
                select[screenPos + 2] = color;
            }
    }
}

int drawString(const char *string, int posX, int posY, u32 color)
{
    for(int i = 0, line_i = 0; i < strlen(string); i++, line_i++)
    {
        if(string[i] == '\n')
        {
            posY += SPACING_Y;
            line_i = 0;
            i++;
        }
        else if(line_i >= (SCREEN_TOP_WIDTH - posX) / SPACING_X)
        {
            // Make sure we never get out of the screen.
            posY += SPACING_Y;
            line_i = 2; //Little offset so we know the same string continues.
            if(string[i] == ' ') i++; //Spaces at the start look weird
        }

        drawCharacter(string[i], posX + line_i * SPACING_X, posY, color);
    }

    return posY;
}

void setFramebuffers()
{
    //Gateway
    *(vu32*)0x80FFFC0 = 0x18300000;  // framebuffer 1 top left
    *(vu32*)0x80FFFC4 = 0x18300000;  // framebuffer 2 top left
    *(vu32*)0x80FFFC8 = 0x18300000;  // framebuffer 1 top right
    *(vu32*)0x80FFFCC = 0x18300000;  // framebuffer 2 top right
    *(vu32*)0x80FFFD0 = 0x18346500;  // framebuffer 1 bottom
    *(vu32*)0x80FFFD4 = 0x18346500;  // framebuffer 2 bottom
    *(vu32*)0x80FFFD8 = 1;  // framebuffer select top
    *(vu32*)0x80FFFDC = 1;  // framebuffer select bottom

    //CakeBrah
    *(vu32*)0x23FFFE00 = 0x18300000;
    *(vu32*)0x23FFFE04 = 0x18300000;
    *(vu32*)0x23FFFE08 = 0x18346500;
}

void clearScreens(Screen screen)
{
	if (screen == SCREEN_BOTH) {
		clearScreens(SCREEN_TOP);
		clearScreens(SCREEN_BOTTOM);
	}
	else if (screen == SCREEN_TOP) {
		memset32((void *)0x18300000, 0, 0x46500); //top
	}
	else if (screen == SCREEN_BOTTOM) {
		memset32((void *)0x18346500, 0, 0x38400); //bottom
	}
    
    
}

void turnOnBacklight()
{
    i2cWriteRegister(3, 0x22, 0x2A); // 0x2A -> boot into firm with no backlight
}

void drawImage(char * path, u16 width, u16 height, s16 x, s16 y, Screen screen) {
	if (screen == SCREEN_BOTH) {
		drawImage(path, width, height, x, y, SCREEN_TOP);
		drawImage(path, width, height, x, y, SCREEN_BOTTOM);
		return;
	}

	/*
	Open the image file
	*/
	UINT bytes_read = 0;
	FIL file;
	if (f_open(&file, path, FA_READ | FA_OPEN_EXISTING) != FR_OK) {
		return;
	}
	
	//Get pointer to the screen framebuffer
	u8 *const aScreen = (screen == SCREEN_TOP) ? fb->top_left : fb->bottom;
	
	/*
	Fast draw method for fullscreen images
	*/
	if ( 
			(width == 400 && height == 240 && x == 0 && y == 0 && screen == SCREEN_TOP) 
			||
			(width == 320 && height == 240 && x == 0 && y == 0 && screen == SCREEN_BOTTOM)
	
	) {
		//Read the whole file directly to the framebuffer and return
		f_read(&file, aScreen, (width*height*3), &bytes_read);
		return;
	}
	
	/*
	Slower draw method for smaller than full screen images
	*/
	
	//Reverse width and height
	int temp = width;
	width = height;
	height = temp;
	
	//Calculate offset from top edge of screen depending on width of image and y position
	int yDiff = SCREEN_TOP_HEIGHT - width - y;
	
	//How many bytes to read at a time (enough for one whole row of pixels at a time)
	int readBytes = width * 3;
	
	//For each row of pixels...
	for (int row=0; row<height; row++) {
		//Calculate the offset from the left edge of the screen
		int screenPixelOffset = (row+x)*SCREEN_TOP_HEIGHT;
		//Add the offset from the top edge of the screen
		screenPixelOffset += yDiff;
		//Multiply this by three to get the byte offset within the framebuffer
		int screenByteOffset = screenPixelOffset * 3;
		//Read one row of pixels into the framebuffer
		f_read(&file, &aScreen[screenByteOffset], readBytes, &bytes_read);
	}
	
	f_close(&file);
}
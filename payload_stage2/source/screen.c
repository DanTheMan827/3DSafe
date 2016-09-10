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

void clearScreens(void)
{
    memset32((void *)0x18300000, 0, 0x46500);
    memset32((void *)0x18346500, 0, 0x38400);
}

void turnOnBacklight()
{
    i2cWriteRegister(3, 0x22, 0x2A); // 0x2A -> boot into firm with no backlight
}

void drawImageData(u8* image, u16 width, u16 height, s16 x, s16 y) {
	//For some reason, images are drawn one pixel higher than specified. Move the whole image by one pixel to compensate
	y++;

	//Pointer to framebuffer
	u8 *const select = fb->top_left;
	
	/*
	For each pixel column
	*/
	for (int sPixelX=0; sPixelX < width; sPixelX++) {
		/*
		For each pixel row
		*/
		for (int sPixelY=0; sPixelY < height; sPixelY++) {
			//Work out the distance from the beginning of the image of the source pixel
			int sPixel = (sPixelY * width) + sPixelX;
			
			//Offset in bytes from the beginning of the data
			int sPixelOffset = sPixel * 3;
			
			//Calculate the coordinates of the pixel in the framebuffer
			int dPixelX = sPixelX + x;
			int dPixelY = (SCREEN_TOP_HEIGHT - sPixelY) - y;
			
			//Work out the distance from the beginning of the framebuffer to the pixel
			int dPixel = (dPixelX * SCREEN_TOP_HEIGHT) + dPixelY;
			
			//Offset in bytes from the beginning of the framebuffer
			int dPixelOffset = dPixel * 3;
			
			//For each byte in the pixel (RGB)...
			for (int byte = 0; byte < 3; byte++) {
				//Copy the value of the pixel to the framebuffer
				select[dPixelOffset+byte] = image[sPixelOffset+byte];
			}
		}
	}
}

void drawImage(char * path, u16 width, u16 height, s16 x, s16 y) {

	/*
	Open the image file
	*/
	UINT bytes_read = 0;
	FIL file;
	if (f_open(&file, path, FA_READ | FA_OPEN_EXISTING) != FR_OK)
		return;
	
	//Width of columns for reading image into memory in chunks
	int colWidth = 40;

	//Size of data to read in each read operation
	size_t readSize = colWidth * 3;

	//File read buffer
	u8 im[readSize];

	/*
	For each row of pixels in the image
	*/
	for (int row=0; row<height; row++) {
		//Work out how many columns are needed by dividing the image by the column width
		int cols = width/colWidth;
		
		/*
		For each chunk of pixels in this row
		*/
		for (int col=0; col<cols; col++) {
			
			//Read the data for this chunk from the file
			if (f_read(&file, im, readSize, &bytes_read) == FR_OK) {
				//Reposition the chunk depending on its position within the image and the coordinates for the image as a whole
				int drawX = x + (col * colWidth);// colOffset;
				int drawY = y + row;
				
				//Draw the image data
				drawImageData(im, colWidth, 1, drawX, drawY);
			}
		}
	}
	
	f_close(&file);
}
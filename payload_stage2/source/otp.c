/*
OTP verification code by AuroraWright, and taken from SafeA9LHInstaller
https://github.com/AuroraWright/SafeA9LHInstaller
*/

#include "otp.h"
#include "fatfs/ff.h"
#include "utils.h"
#include "screen.h"
#include <string.h>
#include "crypto.h"

#define OTP_OFFSET        0x24000000
#define SECTOR_OFFSET     0x24100000
#define FIRM0_OFFSET      0x24200000
#define FIRM0_SIZE        0xF3000

u32 fileRead(void *dest, const char *path)
{
	FATFS otpFS;
	f_mount(&otpFS, "0:", 0);

    FIL file;
    u32 size;

    if(f_open(&file, path, FA_READ) == FR_OK)
    {
        unsigned int read;
        size = f_size(&file);
        f_read(&file, dest, size, &read);
        f_close(&file);
    }
    else {
    	size = 0;

//     	clearScreens();
// 		drawString("Couldn't open OTP", 10, 10, COLOR_RED);
// 		waitInput();
    }

	f_mount(NULL, "0:", 0);
    return size;
}

bool otpIsValid(char * path) {
	if(fileRead((void *)OTP_OFFSET, path) != 256) {
// 		clearScreens();
// 		drawString("Invalid OTP", 10, 10, COLOR_RED);
// 		waitInput();
		return false;
	}
	
	setupKeyslot0x11(false, (void *)OTP_OFFSET);
	getNandCTR();
	
	readFirm0((u8 *)FIRM0_OFFSET, FIRM0_SIZE);
    if(memcmp((void *)FIRM0_OFFSET, "FIRM", 4) != 0) {
//     	clearScreens();
// 		drawString("Error: failed to setup FIRM encryption", 10, 10, COLOR_RED);
// 		waitInput();
		return false;
    }

	getSector((u8 *)SECTOR_OFFSET);

	u32 i;
	for(i = 0; i < 3; i++) {
		if(memcmp((void *)(SECTOR_OFFSET + 0x10), key2s[i], 0x10) == 0) break;
	}

	if(i == 3) {
// 		clearScreens();
// 		drawString("OTP or NAND key sector are invalid", 10, 10, COLOR_RED);
// 		waitInput();
		return false;
	}

	return true;
}
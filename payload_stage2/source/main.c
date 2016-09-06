#include "types.h"
#include "i2c.h"
#include "screen.h"
#include "utils.h"
#include "fatfs/ff.h"
#include "memory.h"
#include "../build/bundled.h"
#include "buttons.h"

#define PAYLOAD_ADDRESS 0x23F00000
#define A11_PAYLOAD_LOC 0x1FFF4C80 //keep in mind this needs to be changed in the ld script for arm11 too
#define A11_ENTRY       0x1FFFFFF8

static void ownArm11(u32 screenInit)
{
    memcpy((void *)A11_PAYLOAD_LOC, arm11_bin, arm11_bin_size);

    //Let the ARM11 code know if it needs to screen init
    *(vu32 *)(A11_PAYLOAD_LOC + 8) = screenInit;

    *(vu32 *)A11_ENTRY = 1;
    *(vu32 *)0x1FFAED80 = 0xE51FF004;
    *(vu32 *)0x1FFAED84 = A11_PAYLOAD_LOC;
    *(vu8 *)0x1FFFFFF0 = 2;
    while(*(vu32 *)A11_ENTRY);
}

static inline void prepareForBoot()
{
	setFramebuffers();
	ownArm11(1);
	clearScreens();
	turnOnBacklight(); // Always screen init because CBM9 doesn't have it, and that gave me a heart attack.
}

void bootPayload() {
	FIL payload;
	unsigned int br;

	if(f_open(&payload, "arm9loaderhax.bin", FA_READ) == FR_OK)
    {
//         prepareForBoot();
        f_read(&payload, (void *)PAYLOAD_ADDRESS, f_size(&payload), &br);
        ((void (*)())PAYLOAD_ADDRESS)();
        i2cWriteRegister(I2C_DEV_MCU, 0x20, 1);
    }
    else
    {
//         prepareForBoot();
		clearScreens();
        error("Couldn't find the payload.\nMake sure to either:\n 1) Have SD card plugged in\n 2) Have arm9loaderhax.bin at SD root");
    }
}

void drawPin(char * entered) {
	clearScreens();
	drawString("Enter PIN", 10, 10, COLOR_RED);
	drawString(entered, 10, 30, COLOR_WHITE);
}

void main()
{
    FATFS fs;
    f_mount(&fs, "0:", 0); //This never fails due to deferred mounting
    
    prepareForBoot();
    
    int pinPos = 0;
    char pin[] = "BARRAL";
    int pinlen = strlen(pin);
    char entered[pinlen+1];// = "----";
    
    for (int i=0; i<pinlen; i++) {
    	entered[i] = '-';
    }
    
    entered[pinlen] = '\0';
    
//     drawString(entered, 10, 30, COLOR_WHITE);
    
    while (pinPos < pinlen) {
    	drawPin(entered);
    
    	char append;
    
    	u32 key = waitInput();
    	
    	if (key == BUTTON_A) {
    		append = 'A';
    	}
    	else if (key == BUTTON_B) {
    		append = 'B';
    	}
    	else if (key == BUTTON_X) {
    		append = 'X';
    	}
    	else if (key == BUTTON_Y) {
    		append = 'Y';
    	}
    	else if (key == BUTTON_UP) {
    		append = 'U';
    	}
    	else if (key == BUTTON_DOWN) {
    		append = 'D';
    	}
    	else if (key == BUTTON_LEFT) {
    		append = 'L';
    	}
    	else if (key == BUTTON_RIGHT) {
    		append = 'R';
    	}
    	else {
    		append = '-';
    	}
    	
    	if (append != '-') {
    		entered[pinPos] = append;		
			pinPos++;
    	}
    }
    
    if (strcmp(pin, entered) == 0) {
    	drawPin(entered);
    	bootPayload();
    }
    else {
    	clearScreens();
    	error("Incorrect PIN");
//     	bootPayload();
    }    
}

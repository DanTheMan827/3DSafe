/*
3DSafe incorporates parts of GodMode9 by d0k3 for reading and writing the PIN from/to NAND. Credit for the code in 3DSafe which is taken from GodMode9 and modified by mashers is given to d0k and the other contributors to the GodMode9 project. This includes the following components of 3DSafe:
- godmode.c
- godmode.h
- fatfs (modified to mount/read/write 3DS NAND partitions)
- nand
*/

#include "godmode.h"
#include "fs.h"
#include "nand/nand.h"

//Flag to determine whether godmode was successfully entered (at any point)
bool godMode = false;

bool enterGodMode() {
	godMode = false;

	if (!InitSDCardFS()) {
		return false;
	}
	
	if (!InitNandCrypto()) {
		return false;
	}
	
	if (!InitExtFS()) {
		return false;
	}
	
	godMode = true;
	return true;
}

bool getPINFromNAND(char storedPin[9]) {
	//Read a file from NAND
	u8 pinData[8];
	size_t pinFileSize = FileGetData("1:/pin.txt", pinData, 8, 0);

	if (pinFileSize > 0) {
// 		char pinChars[8];
		
		for (int i=0; i<8; i++) {
			storedPin[i] = pinData[i] + 0;
		}
		
		storedPin[8] = '\0';
		
// 		memcpy(storedPin, &pinChars, 8);
		
// 		drawString(storedPin, 10, 10, COLOR_RED);
// 		waitInput();
// 		clearScreens();
	
		return true;
// 		drawString("Read some bytes from NAND", 10, 10, COLOR_RED);
	}
	else {
		return false;
// 		drawString("Failed to read from NAND", 10, 10, COLOR_RED);
	}

// 	waitInput();
// 	clearScreens();
}
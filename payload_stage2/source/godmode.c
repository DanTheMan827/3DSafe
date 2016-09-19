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
// bool godMode = false;

bool enterGodMode() {
// 	godMode = false;

/*
	if (!InitSDCardFS()) {
		return false;
	}
	
	if (!InitNandCrypto()) {
		return false;
	}
	*/
	
	InitSDCardFS();
	InitNandCrypto();
	
	if (!InitExtFS()) {
		return false;
	}
	
// 	godMode = true;
	return true;
}


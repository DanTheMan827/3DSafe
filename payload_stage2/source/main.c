#include "types.h"
#include "i2c.h"
#include "screen.h"
#include "utils.h"
#include "fatfs/ff.h"
#include "memory.h"
#include "../build/bundled.h"
#include "buttons.h"
#include <string.h>
#include "godmode.h"
#include "otp.h"
#include "fs.h"

#define PAYLOAD_ADDRESS 0x23F00000
#define A11_PAYLOAD_LOC 0x1FFF4C80 //keep in mind this needs to be changed in the ld script for arm11 too
#define A11_ENTRY       0x1FFFFFF8

bool drewPINImage = false;

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
	clearScreens(SCREEN_BOTH);
	turnOnBacklight(); // Always screen init because CBM9 doesn't have it, and that gave me a heart attack.
}

/*
Translate buttons received by waitInput() into char equivalents representing the pressed button (used for building the entered PIN)
*/
char translateButton(u32 key) {
	if (key == BUTTON_A) {
		return 'A';
	}
	else if (key == BUTTON_B) {
		return 'B';
	}
	else if (key == BUTTON_X) {
		return 'X';
	}
	else if (key == BUTTON_Y) {
		return 'Y';
	}
	else if (key == BUTTON_UP) {
		return 'U';
	}
	else if (key == BUTTON_DOWN) {
		return 'D';
	}
	else if (key == BUTTON_LEFT) {
		return 'L';
	}
	else if (key == BUTTON_RIGHT) {
		return 'R';
	}
	else {
		return '-';
	}
}

//Forward declaration
void bootPayload();

/*
Prompt the user to enter a new PIN
*/
void setNewPIN(bool force) {
	//Flag to keep us inside the while loop to keep entering characters
	u32 getPIN = 1;
	//Buffer for the entered values
	char entered[8];
	//Start at the beginning of the buffer
	u32 pinPos = 0;

	//Clear the buffer
	for (int i=0; i<8; i++) {
		entered[i] = '\0';
	}
	
	//While still entering characters
	while (getPIN == 1) {
		/*
		Draw a prompt to enter some characters for the PIN
		*/
		clearScreens(SCREEN_TOP);
		drawString("Enter new PIN using ABLR and D-Pad (max. 8 characters)", 10, 10, COLOR_RED);
		drawString("Press START when done", 10, 30, COLOR_WHITE);
		drawString(entered, 10, 50, COLOR_WHITE);

		//Wait for the user to press a button
		u32 key = waitInput();
	
		/*
		If the user presses START, break out of the while loop
		*/
		if (key == BUTTON_START) {
			if (pinPos == 0) {
				clearScreens(SCREEN_TOP);
			
				if (force) {
					continue;
				}
				else {
					drawString("Your PIN was not changed", 10, 10, COLOR_RED);
					drawString("Press any key to continue", 10, 30, COLOR_WHITE);
					waitInput();
					displayOptions();
					return;
				}
			}
			else {
				getPIN = 0;
			}
		}
		
		/*
		If the user presses any other button...
		*/
		else {
			//Translate the pressed key to a character
			char append = translateButton(key);
	
			/*
			The user pressed a button which can exist in the PIN
			*/
			if (append != '-') {
				//Change the character in the entered PIN at the current position to the character entered
				entered[pinPos] = append;		
				//Go to the next character position in the PIN
				pinPos++;
				
				/*
				If 8 characters have been entered, break out of the while loop
				*/
				if (pinPos >= 8) {
					getPIN = 0;
				}
			}
		}
	}

	clearScreens(SCREEN_TOP);

	/*
	Enter god mode if necessary
	*/
	if (!godMode) {
		/*
		Enter Godmode to gain access to SysNAND
		*/
		if(!enterGodMode()) {
			error("Could not gain access to SysNAND");
		}
	}

	/*
	Open the pin file in NAND
	*/
	FIL pinFile;

	if(f_open(&pinFile, "1:/pin.txt", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
		/*
		Write the PIN to the file
		*/
		unsigned int bw;
		f_write (&pinFile, entered, 8, &bw);
		f_sync(&pinFile);
		f_close(&pinFile);
	
		/*
		Show success message and then boot payload
		*/
		if (bw > 0) {
			drawString("PIN changed to:", 10, 10, COLOR_RED);
			drawString(entered, 10, 30, COLOR_WHITE);
			drawString("Press any key to boot payload", 10, 50, COLOR_WHITE);

			waitInput();
			bootPayload();
		}
		
		/*
		Show save error
		*/
		else {
			error("The PIN could not be saved (no bytes written)");
		}
	}
	
	/*
	Show PIN file open error
	*/
	else {
		error("The PIN could not be saved (file open fail)");
	}
}

/*
Show the options menu to the user
*/
void displayOptions() {	
	//Buffer for read keypresses
	u32 key;
	//Stay in the while loop until a valid option was selected
	u32 validOption = 0;

	while (validOption == 0) {
		/*
		Display the options on the screen
		*/
		clearScreens(SCREEN_TOP);
		drawString("3DSafe Options", 10, 10, COLOR_RED);
		drawString("START: Boot payload", 10, 30, COLOR_WHITE);
		drawString("    A: Change PIN", 10, 40, COLOR_WHITE);
		drawString("    B: Power off", 10, 50, COLOR_WHITE);
		drawString("    X: SafeA9LHInstaller", 10, 60, COLOR_WHITE);

		//Wait for input
		key = waitInput();
		
		/*
		If the button pressed corresponds to a menu option, break out of the while loop
		*/
		if (key == BUTTON_START || key == BUTTON_A || key == BUTTON_B || key == BUTTON_X) {
			validOption = 1;
		}
	}

	clearScreens(SCREEN_TOP);

	/*
	User opted to boot the payload
	*/
	if (key == BUTTON_START) {
		/*
		If not in god mode, mount the SD card so the payload will be found
		*/
		if (!godMode) {			
			FATFS afs;
			f_mount(&afs, "0:", 0);
		}
	
		//Boot the payload
		bootPayload();
	}
	
	/*
	User opted to change the PIN
	*/
	else if (key == BUTTON_A) {
		setNewPIN(false);
	}
	
	/*
	User opted to power off
	*/
	else if (key == BUTTON_B) {
		mcuShutDown();
	}
	
	/*
	User opted to run SafeA9LHInstaller
	*/
	else if (key == BUTTON_X) {
		sa9lhi();
		displayOptions();
	}
}

/*
Boot /arm9loaderhax from SD card
*/
void bootPayload() {
	/*
	Try to open the payload file
	*/
	FIL payload;
	unsigned int br;

	if(f_open(&payload, "arm9loaderhax.bin", FA_READ) == FR_OK)
	{
		/*
		Read the payload and boot it
		*/
		f_read(&payload, (void *)PAYLOAD_ADDRESS, f_size(&payload), &br);					
		((void (*)())PAYLOAD_ADDRESS)();
		i2cWriteRegister(I2C_DEV_MCU, 0x20, 1);
	}
	
	/*
	Display an error if the payload can't be found
	*/
	else
	{
		error("Couldn't find the payload.\nMake sure to either:\n 1) Have SD card plugged in\n 2) Have arm9loaderhax.bin at SD root");
	}
}

/*
Draw the 'enter pin' prompt along with whatever has already been entered
*/
void drawPin(char * entered) {
	if (!drewPINImage) {
		clearScreens(SCREEN_TOP);
		drawString("Enter PIN", 10, 10, COLOR_RED);	
	}

	drawString(entered, 10, 30, COLOR_WHITE);
}

int main()
{
    /*
    Screen init
    */
    prepareForBoot();
    
    /*
    DEBUG: Allow skipping past everything for brick protection during development
    */
//     drawString("Press X to skip 3DSafe, any other button to enter 3DSafe", 10, 10, COLOR_RED);
//     u32 key = waitInput();
//     if (key == BUTTON_X) {
// 		FATFS afs;
// 		f_mount(&afs, "0:", 0); //This never fails due to deferred mounting
//     	bootPayload();
//     	return 0;
//     }
//     clearScreens(SCREEN_TOP);
    
    /*
    OTP BYPASS
    First, mount the SD card, check if the otp exists, and store the result in RES.
    Then unmount the SD card (leaving it mounted might interfere with entering god mode later)
    */
    FATFS otpFS;
	f_mount(&otpFS, "0:", 0);
    FIL otp;
    char * otpPath = "otp.bin";
    FRESULT RES = f_open(&otp,otpPath, FA_READ);
    f_mount(NULL, "0:", 0);
    
    /*
    An otp.bin was found. Check if it is valid for this console
    */
    if(RES == FR_OK) {
    	if (otpIsValid(otpPath)) {
			//Inform the user that the PIN lock has been bypassed
			drawString("PIN LOCK BYPASSED. Press any key to enter 3DSafe options", 10, 10, COLOR_RED);
			//Wait for a keypress
			waitInput();
			clearScreens(SCREEN_TOP);
			//Jump straight to the options menu without asking for the PIN
			displayOptions();
			return 0;
		}
		else {
			//Inform the user that the OTP is invalid
			clearScreens(SCREEN_TOP);
			drawString("INVALID otp.bin. Press any key to proceed to enter PIN.", 10, 10, COLOR_RED);
			//Wait for a keypress
			waitInput();
			clearScreens(SCREEN_TOP);
			
			//Continue as normal from this point (request PIN)
		}
    }
    
	/*
	Enter Godmode to gain access to SysNAND
	*/
	if(!enterGodMode()) {
		error("Could not gain access to SysNAND");
	}
	
	clearScreens(SCREEN_TOP);
	
//Test images
// 	drawImage("0:/logo.bin", 192, 192, 50, 10, SCREEN_BOTH);
// 	waitInput();
// 	clearScreens(SCREEN_TOP);
// 	
// 	drawImage("1:/bg.bin", 400, 240, 0, 0, SCREEN_TOP);
// 	waitInput();
// 	clearScreens(SCREEN_TOP);

	drewPINImage = drawImage("1:/3dsafepinrequest.bin", 400, 204, 0, 0, SCREEN_TOP);

	drawImage("1:/3dsafelost.bin", 320, 240, 0, 0, SCREEN_BOTTOM);
	
	/*
	Try to read the PIN file from SysNAND
	*/
	//File read buffer
	char pin[9];
    
    if (!getPINFromNAND(pin)) {
	    //If no PIN could be read from NAND, try creating a new one
    	setNewPIN(true);
    	
    	//If we still can't get a PIN from NAND, show an error and die
    	if (!getPINFromNAND(pin)) {	
			error("Failed to get PIN from NAND. You should use otp.bin bypass to regain access to your device");
		}
    }
	
	/*
	DEBUG: print the read PIN on the screen
	*/
// 	drawString(pin, 10, 10, COLOR_RED);
// 	waitInput();
// 	clearScreens(SCREEN_TOP);
	
	//Get the length of the PIN
	int pinlen = strlen(pin);
	
	/*
	Start at the beginning of the PIN and with an empty input buffer
	*/
	int pinPos = 0;
	char entered[pinlen+1];

	/*
	Clear the input buffer to dashes (for display purposes)
	*/
	for (int i=0; i<pinlen; i++) {
		entered[i] = '-';
	}
	entered[pinlen] = '\0';

	/*
	While the user has not finished entering the PIN...
	*/	
	while (pinPos < pinlen) {
		//Draw the prompt to enter the PIN and whatever has already been entered
		drawPin(entered);

		//Wait for the user to press a button
		u32 key = waitInput();
		
		//Translate the pressed key to a character
		char append = translateButton(key);
				
		/*
		The user pressed a button which can exist in the PIN
		*/
		if (append != '-') {
			//Change the character in the entered PIN at the current position to the character entered
			entered[pinPos] = append;		
			//Go to the next character position in the PIN
			pinPos++;
		}
	}

	/*
	If the entered PIN matches what is expected, display the options to the user
	*/
	if (strcmp(pin, entered) == 0) {
		drawPin(entered);
		displayOptions();
	}
	
	/*
	If the entered PIN does not match what is expected, show an error
	*/
	else {		
		clearScreens(SCREEN_TOP);
		
		error("Incorrect PIN\n \nIf you have forgotten your PIN, place your\notp.bin at the root of your SD card. The\nfilename must be in lower case, and the\nOTP must match this device. You will then\nbe able to reset your 3DSafe PIN");
		
// 		drawString("Incorrect PIN", 10, 10, COLOR_RED);
		
// 		drawString("Incorrect PIN\n\nIf you have forgotten your PIN, place your\notp.bin at the root of your SD card. The\nfilename must be in lower case, and the\nOTP must match this device. You will then\nbe able to reset your 3DSafe PIN", 10, 30, COLOR_WHITE);
// 		drawString("", 10, 40, COLOR_WHITE);
// 		drawString("", 10, 50, COLOR_WHITE);
// 		drawString("", 10, 60, COLOR_WHITE);		
// 		drawString(".", 10, 70, COLOR_WHITE);
// 		
// 		drawString("Press any key to power off", 10, 90, COLOR_RED);
		
// 		waitInput();
// 		
// 		mcuShutDown();
	}
    
}
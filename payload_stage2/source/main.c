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

//Forward declarations
void bootPayload();
void displayOptions();

void drawPINText(char * entered) {
	drawString(entered, 10, 30, COLOR_WHITE);
}

void drawPINGfx(char * entered) {
	bool success = true;
		
	if (drawImage("0:/3dsafe/pinbottom.bin", 400, 36, 0, 204, SCREEN_TOP)) {
		int len = strlen(entered);
		int stringWidth = (len*36) + ((len-1)*16);
		int drawX = (400/2)-(stringWidth/2);
	
		for (int p=0; p<len; p++) {
			char * filename;
	
			char c = entered[p];
		
			if (c == 'A') {
				filename = "0:/3dsafe/a.bin";
			}
			else if (c == 'B') {
				filename = "0:/3dsafe/b.bin";
			}
			else if (c == 'X') {
				filename = "0:/3dsafe/x.bin";
			}
			else if (c == 'Y') {
				filename = "0:/3dsafe/y.bin";
			}
			else if (c == 'L') {
				filename = "0:/3dsafe/l.bin";
			}
			else if (c == 'R') {
				filename = "0:/3dsafe/r.bin";
			}
			else if (c == 'U') {
				filename = "0:/3dsafe/u.bin";
			}
			else if (c == 'D') {
				filename = "0:/3dsafe/d.bin";
			}
			else if (c == '-') {
				filename = "0:/3dsafe/underscore.bin";
			}
		
			if (!drawImage(filename, 36, 36, drawX, 204, SCREEN_TOP)) {
				success = false;
				break;
			}
		
			drawX+=(36+16);
		}
	}
	else {
		success = false;
	}
	
	
	if (!success) {
		drawPINText(entered);
	}
}

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
	
	/*
	Draw a prompt to enter some characters for the PIN
	*/
	bool drewGraphicalChangePrompt = drawImage("0:/3dsafe/changepin.bin", 400, 240, 0, 0, SCREEN_TOP);
	
	if (!drewGraphicalChangePrompt) {
		clearScreens(SCREEN_TOP);
		drawString("Enter new PIN using ABXY and D-Pad (max. 8 characters)", 10, 10, COLOR_RED);
		drawString("Press START when done", 10, 30, COLOR_WHITE);
		if (!force) {
			drawString("Press SELECT to cancel", 10, 40, COLOR_WHITE);
		}
	}
	
	//While still entering characters
	while (getPIN == 1) {
		//Draw what has been entered so far
		if (drewGraphicalChangePrompt) {
			drawPINGfx(entered);
		}
		else {
			drawPINText(entered);
		}
	
// 		drawString(entered, 10, 50, COLOR_WHITE);

		//Wait for the user to press a button
		u32 key = waitInput();
	
		/*
		If the user presses START and some characters have been entered, break out of the while loop
		*/
		if (key == BUTTON_START && pinPos > 0) {
			getPIN = 0;
		}
		else if (key == BUTTON_SELECT && !force) {
			if (!drawImage("0:/3dsafe/pinnotchanged.bin", 400, 240, 0, 0, SCREEN_TOP)) {
				clearScreens(SCREEN_TOP);
				drawString("Your PIN was not changed", 10, 10, COLOR_RED);
				drawString("Press any key to continue", 10, 30, COLOR_WHITE);
			}
			
			waitInput();
			displayOptions();
			return;
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

// 	clearScreens(SCREEN_TOP);

	/*
	Enter god mode if necessary
	*/
// 	if (!godMode) {
// 		/*
// 		Enter Godmode to gain access to SysNAND
// 		*/
// 		if(!enterGodMode()) {
// 			error("Could not gain access to SysNAND");
// 		}
// 	}

	/*
	Open the pin file in NAND
	*/
	FIL pinFile;

	if(f_open(&pinFile, "1:/3dsafe/pin.txt", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
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
			if (drawImage("0:/3dsafe/pinchanged.bin", 400, 240, 0, 0, SCREEN_TOP)) {
				drawPINGfx(entered);
			}
			else {
				clearScreens(SCREEN_TOP);
				drawString("PIN changed to:", 10, 10, COLOR_RED);
				drawString(entered, 10, 30, COLOR_WHITE);
				drawString("Press any key to continue", 10, 50, COLOR_WHITE);
			}

			waitInput();
			displayOptions();
			return;
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

void showAbout() {
	clearScreens(SCREEN_TOP);
	drawString("About 3DSafe", 10, 10, COLOR_TITLE);
	drawString("3DSafe 0.4 by mashers", 10, 30, COLOR_WHITE);
	drawString("GitHub repo: http://goo.gl/QLsBx3", 10, 40, COLOR_WHITE);

	drawString("Payloads based on ShadowNAND by RShadowHand", 10, 60, COLOR_WHITE);
	drawString("GitHub repo: http://goo.gl/DYP4IA", 10, 70, COLOR_WHITE);
	
	drawString("NAND read/write from GodMode9 by d0k3", 10, 90, COLOR_WHITE);
	drawString("GitHub repo: http://goo.gl/Ejhjf8", 10, 100, COLOR_WHITE);
	
	drawString("Incorporates SafeA9LHInstaller by AuroraWright", 10, 120, COLOR_WHITE);
	drawString("GitHub repo: http://goo.gl/XkRYAQ", 10, 130, COLOR_WHITE);
	
	drawString("Press any key to continue", 10, 150, COLOR_WHITE);
	
	waitInput();
	
	//https://github.com/d0k3/GodMode9
}

/*
Show the options menu to the user
*/
void displayOptions() {	
	//Buffer for read keypresses
	u32 key;
	//Stay in the while loop until a valid option was selected
	u32 validOption = 0;
	
	
	/*
		Display the options on the screen
	*/
	if (!drawImage("0:/3dsafe/options.bin", 400, 240, 0, 0, SCREEN_TOP)) {
		clearScreens(SCREEN_TOP);
		drawString("3DSafe Options", 10, 10, COLOR_RED);
		drawString("START: Boot payload", 10, 30, COLOR_WHITE);
		drawString("    A: Change PIN", 10, 40, COLOR_WHITE);
		drawString("    B: Power off", 10, 50, COLOR_WHITE);
		drawString("    X: SafeA9LHInstaller", 10, 60, COLOR_WHITE);
		drawString("    Y: About 3DSafe", 10, 70, COLOR_WHITE);
	}

	while (validOption == 0) {
		//Wait for input
		key = waitInput();
		
		/*
		If the button pressed corresponds to a menu option, break out of the while loop
		*/
		if (key == BUTTON_START || key == BUTTON_A || key == BUTTON_B || key == BUTTON_X || key == BUTTON_Y) {
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
// 		if (!godMode) {			
// 			FATFS afs;
// 			f_mount(&afs, "0:", 0);
// 		}
	
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
		sa9lhi(true);
		displayOptions();
	}
	
	/*
	User opted to view about page
	*/
	else if (key == BUTTON_Y) {	
		showAbout();
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
	if (drewPINImage) {
		drawPINGfx(entered);
	}

	else {
		clearScreens(SCREEN_TOP);
		drawString("Enter your PIN. If you have forgotten your PIN, press START.", 10, 10, COLOR_RED);
		drawPINText(entered);
	}

	
}

void drawLostImage() {	
	drawImage("0:/3dsafe/lost.bin", 320, 240, 0, 0, SCREEN_BOTTOM);
}

void showIncorrectPIN() {
	if (drawImage("0:/3dsafe/incorrect.bin", 400, 240, 0, 0, SCREEN_TOP)) {
		waitInput();
		mcuShutDown();
	}
	else {
		clearScreens(SCREEN_TOP);
		error("Incorrect PIN\n \nIf you have forgotten your PIN, place your\notp.bin at the root of your SD card. The\nOTP must match this device. You will then\nbe able to reset your 3DSafe PIN");
	}
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
    drawString("Press X to skip 3DSafe, any other button to enter 3DSafe", 10, 10, COLOR_RED);
    u32 key = waitInput();
    if (key == BUTTON_X) {
		FATFS afs;
		f_mount(&afs, "0:", 0); //This never fails due to deferred mounting
    	bootPayload();
    	return 0;
    }
    clearScreens(SCREEN_TOP);
    
    /*
	Enter Godmode to gain access to SysNAND
	*/
	if(!enterGodMode()) {
		drawString("Could not gain access to sysNAND\nPress any key to run SafeA9LHInstaller.\nFrom here you can install a different A9LH payload.", 10, 10, COLOR_RED);
		waitInput();
		FATFS afs;
		f_mount(&afs, "0:", 0);
		otpIsValid("OTP.BIN", OTP_LOCATION_DISK);
		clearScreens(SCREEN_TOP);
		sa9lhi(false);
	}
    
    /*
    OTP BYPASS
    First, mount the SD card, check if the otp exists, and store the result in RES.
    Then unmount the SD card (leaving it mounted might interfere with entering god mode later)
    */
//     FATFS otpFS;
// 	f_mount(&otpFS, "0:", 0);
    FIL otp;
    char * otpPath = "0:/OTP.BIN";
    
    /*
    An otp.bin was found. Check if it is valid for this console
    */
    if(f_open(&otp,otpPath, FA_READ) == FR_OK) {
    	drawLostImage();
    
    	if (otpIsValid(otpPath, OTP_LOCATION_DISK)) {
    		FIL nandOTP;
    		char *sysOTPPath = "1:/OTP.BIN";
    	
    		if(f_open(&nandOTP, sysOTPPath, FA_READ) != FR_OK) {    			
    			f_close(&nandOTP);
    			
    			if(f_open(&nandOTP, sysOTPPath, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
    				unsigned int bw;
					f_write (&nandOTP, (void*)OTP_OFFSET, 256, &bw);
					f_sync(&nandOTP);
					f_close(&nandOTP);
					
					if (bw == 0) {
						f_unlink(sysOTPPath);
					}
    			}
    		}
    	
			//Inform the user that the PIN lock has been bypassed
			if (!drawImage("0:/3dsafe/bypass.bin", 400, 240, 0, 0, SCREEN_TOP)) {
				drawString("PIN LOCK BYPASSED. Press any key to enter 3DSafe options", 10, 10, COLOR_RED);
			}
			
			//Wait for a keypress
			waitInput();
			clearScreens(SCREEN_TOP);
			//Jump straight to the options menu without asking for the PIN
			displayOptions();
			return 0;
		}
		else {
			//Inform the user that the OTP is invalid
			if (!drawImage("0:/3dsafe/invalidotp.bin", 400, 240, 0, 0, SCREEN_TOP)) {
				clearScreens(SCREEN_TOP);
				drawString("INVALID otp.bin. Press any key to proceed to enter PIN.", 10, 10, COLOR_RED);
			}
			
			//Wait for a keypress
			waitInput();
			clearScreens(SCREEN_TOP);
						
			//Continue as normal from this point (request PIN)
		}
    }
    
    //Unmount existing FS in case it interferes with entering God Mode
//     f_mount(NULL, "0:", 0);
    
	
	
	/*
	Create directory for 3DSafe files (if necessary)
	*/
	f_mkdir("1:/3dsafe");
	
	clearScreens(SCREEN_TOP);
	
	drewPINImage = drawImage("0:/3dsafe/pinrequest.bin", 400, 240, 0, 0, SCREEN_TOP);
	drawLostImage();
	
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
		
		if (key == BUTTON_START) {
			showIncorrectPIN();
			return 0;
		}
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
			}
		}
	}
	
	//Draw the PIN one last time to show the last character
	drawPin(entered);

	/*
	If the entered PIN matches what is expected, display the options to the user
	*/
	if (strcmp(pin, entered) == 0) {
		displayOptions();
	}
	
	/*
	If the entered PIN does not match what is expected, show an error
	*/
	else {	
		showIncorrectPIN();
		
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
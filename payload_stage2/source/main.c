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
// #include "crypto.h"

#define PAYLOAD_ADDRESS 0x23F00000
#define A11_PAYLOAD_LOC 0x1FFF4C80 //keep in mind this needs to be changed in the ld script for arm11 too
#define A11_ENTRY       0x1FFFFFF8

#define DISABLE_PATH "1:/3dsafe/disable"

#define SD_LOST_PATH "0:/3dsafe/lost.bin"
#define SYS_LOST_PATH "1:/3dsafe/lost.bin"

#define SD_EBOOT_PATH "0:/3dsafe/emergency.bin"
#define SYS_EBOOT_PATH "1:/3dsafe/emergency.bin"
#define SYS_TEMP_EBOOT_PATH "1:/3dsafe/emergencytemp.bin"

#define PIN_MAX_LENGTH 10

typedef enum {
    PIN_STATUS_ALWAYS,
    PIN_STATUS_NEVER
} PINStatus;

PINStatus pinStatus = PIN_STATUS_ALWAYS;

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
	drawString(entered, 10, 60, COLOR_WHITE);
}

void drawPINGfx(char * entered) {
	bool success = true;
		
	if (drawImage("0:/3dsafe/pinbottom.bin", 400, 36, 0, 204, SCREEN_TOP)) {
		int len = strlen(entered);
		int stringWidth = (len*36);// + ((len-1)*16);
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
// 			else if (c == '-') {
// 				filename = "0:/3dsafe/underscore.bin";
// 			}
		
			if (!drawImage(filename, 36, 36, drawX, 204, SCREEN_TOP)) {
				success = false;
				break;
			}
		
			drawX+=32;//(36+16);
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
	int bufferSize = PIN_MAX_LENGTH+1;
	char entered[bufferSize];
	//Start at the beginning of the buffer
	u32 pinPos = 0;

	//Clear the buffer
	for (int i=0; i<bufferSize; i++) {
		entered[i] = '\0';
	}
	
	/*
	Draw a prompt to enter some characters for the PIN
	*/
	char * imagePath = (force) ? "0:/3dsafe/forcechangepin.bin" : "0:/3dsafe/changepin.bin";
	
	bool drewGraphicalChangePrompt = drawImage(imagePath, 400, 240, 0, 0, SCREEN_TOP);
	
	if (!drewGraphicalChangePrompt) {
		clearScreens(SCREEN_TOP);
		drawString("Enter new PIN using ABXY and D-Pad", 10, 10, COLOR_RED);
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
				If enough characters have been entered, break out of the while loop
				*/
				if (pinPos >= PIN_MAX_LENGTH) {
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
		f_write (&pinFile, entered, PIN_MAX_LENGTH, &bw);
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
			error("The PIN could not be saved (no bytes written)", true);
		}
	}
	
	/*
	Show PIN file open error
	*/
	else {
		error("The PIN could not be saved (file open fail)", true);
	}
}

void showAbout() {
	clearScreens(SCREEN_TOP);
	drawString("About 3DSafe", 10, 10, COLOR_TITLE);
	drawString("3DSafe 0.11 by mashers", 10, 30, COLOR_WHITE);
	drawString("GitHub repo: http://goo.gl/QLsBx3", 10, 40, COLOR_WHITE);

	drawString("Payloads based on ShadowNAND by RShadowHand", 10, 60, COLOR_WHITE);
	drawString("GitHub repo: http://goo.gl/DYP4IA", 10, 70, COLOR_WHITE);
	
	drawString("NAND read/write from GodMode9 by d0k3", 10, 90, COLOR_WHITE);
	drawString("GitHub repo: http://goo.gl/Ejhjf8", 10, 100, COLOR_WHITE);
	
	drawString("Incorporates SafeA9LHInstaller by AuroraWright", 10, 120, COLOR_WHITE);
	drawString("GitHub repo: http://goo.gl/XkRYAQ", 10, 130, COLOR_WHITE);
	
	drawString("Press any key to continue", 10, 150, COLOR_WHITE);
	
	waitInput();
}

void togglePin() {
	if (pinStatus == PIN_STATUS_ALWAYS) {
		FIL f;
	
		if(f_open(&f, DISABLE_PATH, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
			pinStatus = PIN_STATUS_NEVER;
		}
		
		f_close(&f);
	}
	else if (pinStatus == PIN_STATUS_NEVER) {
		if (f_unlink(DISABLE_PATH) == FR_OK) {
			pinStatus = PIN_STATUS_ALWAYS;
		}
	}
}

bool drawPINStatusImage() {
	char * pinStatusImagePath = NULL;
	
	if (pinStatus == PIN_STATUS_ALWAYS) {
		pinStatusImagePath = "0:/3dsafe/pinalways.bin";
	}
	else if (pinStatus == PIN_STATUS_NEVER) {
		pinStatusImagePath = "0:/3dsafe/pinnever.bin";
	}
	
	if (pinStatusImagePath != NULL) {
		return drawImage(pinStatusImagePath, 20, 26, 171, 108, SCREEN_TOP);
	}
	
	return false;
}

void updateNANDFiles() {
	bool updatedLost = false;
	bool updatedEBoot = false;

	FIL lostFile;

	if(f_open(&lostFile, SD_LOST_PATH, FA_READ) == FR_OK) {
		f_close(&lostFile);
		f_unlink(SYS_LOST_PATH);
		drawLostImage();
		updatedLost = true;
	}
	
	FIL eBootFile;
	
	if(f_open(&eBootFile, SD_EBOOT_PATH, FA_READ) == FR_OK) {
		f_close(&eBootFile);
		

		int chunkSize = 1024;// * 10;
		u32 readOffset = 0;
		
		u8 data[chunkSize];
		
		size_t eBootSize = FileGetSize(SD_EBOOT_PATH);
		
		bool create = true;
		bool success = true;
		
		while (readOffset < eBootSize) {		
			size_t bytesToRead;
			
			if (eBootSize - readOffset < chunkSize)
				bytesToRead = eBootSize - readOffset;
			else
				bytesToRead = chunkSize;

			size_t read = FileGetData(SD_EBOOT_PATH, &data, bytesToRead, readOffset);
			
			if (read==0) {				
				success = false;
				break;
			}
		
			if (FileSetData(SYS_TEMP_EBOOT_PATH, data, bytesToRead, readOffset, create)) {
				create = false;
				readOffset += bytesToRead;
			}
			else {				
				success = false;
				break;
			}			
		}
		
		if (success) {
			updatedEBoot = true;
		
			f_unlink(SYS_EBOOT_PATH);
			f_rename (SYS_TEMP_EBOOT_PATH, SYS_EBOOT_PATH);
		}
		else {
			error("The emergency payload could not be copied from\nyour SD card to CTRNAND. You should copy the\nfile manually using GodMode9 to the 3dsafe\ndirectory on CTRNAND.", false);
		}
	}
	
	if (updatedLost || updatedEBoot) {
		if (!drawImage("0:/3dsafe/nandupdated.bin", 400, 240, 0, 0, SCREEN_TOP)) {
			clearScreens(SCREEN_TOP);
			drawString("NAND files updated", 10, 10, COLOR_TITLE);
			drawString("The following files have been successfully\nupdated on your CTRNAND, and will be available\nfor use even if the SD card is not present.", 10, 30, COLOR_WHITE);
			drawString("Press any key to continue", 10, 230, COLOR_WHITE);
		}
		
		int yPos = 144;
		
		if (updatedLost) {
			drawString("Owner contact details (lost.bin)", 10, yPos, COLOR_WHITE);
			yPos += 10;
		}
		
		if (updatedEBoot) {
			drawString("Emergency boot payload (emergency.bin)", 10, yPos, COLOR_WHITE);
		}
		
		waitInput();
	}
	else {
		error("No files were updated on your NAND. This is\nprobably because the required files are not\npresent on your SD card.", false);
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
	
// 	bool drewOptionsGfx = false;
	bool drewPINStatusGfx = false;
	
	/*
		Display the options on the screen
	*/
	if (drawImage("0:/3dsafe/options.bin", 400, 240, 0, 0, SCREEN_TOP)) {
// 		drewOptionsGfx = true;
		drewPINStatusGfx = drawPINStatusImage();
		
		if (!drewPINStatusGfx) {
			if (pinStatus == PIN_STATUS_ALWAYS) {
				drawString("Always", 103, 137, COLOR_WHITE);
			}
			else if (pinStatus == PIN_STATUS_NEVER) {
				drawString("Never", 103, 137, COLOR_WHITE);
			}
		}
	}
	else {
		clearScreens(SCREEN_TOP);
		
		drawString("3DSafe Options", 10, 10, COLOR_RED);
		
		drawString(" START: Boot payload", 10, 30, COLOR_WHITE);
		
		if (pinStatus == PIN_STATUS_ALWAYS) {
			drawString("SELECT: Toggle lock (current: always)", 10, 40, COLOR_WHITE);
		}
		else if (pinStatus == PIN_STATUS_NEVER) {
			drawString("SELECT: Toggle lock (current: never)", 10, 40, COLOR_WHITE);
		}
		
		drawString("     A: Change PIN", 10, 50, COLOR_WHITE);
		drawString("     B: Power off", 10, 60, COLOR_WHITE);
		drawString("     X: SafeA9LHInstaller", 10, 70, COLOR_WHITE);
		drawString("     Y: About 3DSafe", 10, 80, COLOR_WHITE);
		drawString("     R: Update NAND files\n        (lost image/emergency boot)", 10, 90, COLOR_WHITE);
	}

	while (validOption == 0) {
		//Wait for input
		key = waitInput();
		
		/*
		If the button pressed corresponds to a menu option, break out of the while loop
		*/
		if (key == BUTTON_START || key == BUTTON_A || key == BUTTON_B || key == BUTTON_X || key == BUTTON_Y || key == BUTTON_R1 || key == BUTTON_L1) {
			validOption = 1;
		}
		
		else if (key == BUTTON_SELECT) {
			togglePin();
			
			if (drewPINStatusGfx) {
				drawPINStatusImage();
			}
			else {
				displayOptions();
				return;
			}
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
	
	/*
	User opted to view about page
	*/
	else if (key == BUTTON_R1) {	
		updateNANDFiles();
		displayOptions();
	}
	
	/*
	User opted to dump SHA
	*/
	else if (key == BUTTON_L1) {	
		if (saveSHA()) {
			if (!drawImage("0:/3dsafe/shadumped.bin", 400, 240, 0, 0, SCREEN_TOP)) {
				clearScreens(SCREEN_TOP);
				drawString("The SHA bypass file has been dumped to your SD\ncard. This file will bypass the requirement to\nenter your PIN.\n \nYou should now do the following:\n \n1. Reboot your 3DS to ensure the bypass works\n2. Store the SHA file safely somewhere other\n   than your 3DS\n3. Remove the SHA file from your 3DS SD card\n \nPress any key to continue.", 10, 10, COLOR_WHITE);
			}
		
			waitInput();
		}
		else {
			error("SHA dump failed.", false);
		}
		
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
	unsigned int br=0;

	bool foundPayload = false;

	/*
	Prioritise the payload on the SD card
	*/
	if(f_open(&payload, "arm9loaderhax.bin", FA_READ) == FR_OK) {	
		foundPayload = true;
	}
	
	/*
	Fallback payload on CTRNAND if no SD payload found
	*/
	else if(f_open(&payload, SYS_EBOOT_PATH, FA_READ) == FR_OK) {
		foundPayload = true;
	}
	
	if (foundPayload) {	
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
		error("Couldn't find the payload.\nMake sure to either:\n 1) Have SD card plugged in\n 2) Have arm9loaderhax.bin at SD root", true);
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
	if (!drawImage(SYS_LOST_PATH, 320, 240, 0, 0, SCREEN_BOTTOM)) {
		if (drawImage(SD_LOST_PATH, 320, 240, 0, 0, SCREEN_BOTTOM) ) {
			FIL nandLost;
			

			if(f_open(&nandLost, SYS_LOST_PATH, FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
				unsigned int bw;
				f_write (&nandLost, fb->bottom, 320*240*3, &bw);
				f_sync(&nandLost);
				f_close(&nandLost);
		
				if (bw == 0) {
					f_unlink(SYS_LOST_PATH);
				}
			}
		}
	}
}

void showIncorrectPIN() {
	if (drawImage("0:/3dsafe/incorrect.bin", 400, 240, 0, 0, SCREEN_TOP)) {
		waitInput();
		mcuShutDown();
	}
	else {
		clearScreens(SCREEN_TOP);
		error("Incorrect PIN\n \nIf you have forgotten your PIN, place your\notp.bin at the root of your SD card. The\nOTP must match this device. You will then\nbe able to reset your 3DSafe PIN", true);
	}
}

bool getPINFromNAND(char storedPin[PIN_MAX_LENGTH+1]) {
	//Read a file from NAND
	u8 pinData[PIN_MAX_LENGTH];
	size_t pinFileSize = FileGetData("1:/3dsafe/pin.txt", pinData, PIN_MAX_LENGTH, 0);

	if (pinFileSize > 0) {
		for (int i=0; i<PIN_MAX_LENGTH; i++) {
			storedPin[i] = pinData[i] + 0;
		}
		
		storedPin[PIN_MAX_LENGTH] = '\0';
	
		return true;
	}
	else {
		return false;
	}
}

#define CFG_BOOTENV    (*(vu32 *)0x10010000)

void bootOrOptions() {
	if (HID_PAD == BUTTON_UP) {
		drawLostImage();
		displayOptions();
	}
	else {
		bootPayload();
	}
}

int main()
{
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
	Enter Godmode to gain access to SysNAND
	*/
	if(!enterGodMode()) {
		/*
		Screen init
		*/
		prepareForBoot();
	
		FATFS afs;
		f_mount(&afs, "0:", 0);
	
		if (!drawImage("3dsafe/nogodmode.bin", 400, 240, 0, 0, SCREEN_TOP)) {
			drawString("Could not gain access to sysNAND\nPress any key to run SafeA9LHInstaller.\nFrom here you can install a different A9LH payload.", 10, 10, COLOR_RED);
		}
		
		waitInput();
		
// 		otpIsValid("OTP.BIN", OTP_LOCATION_DISK);
		clearScreens(SCREEN_TOP);
		sa9lhi(false);
		return 0;
	}
	
	/*
    Screen init
    */
    prepareForBoot();
	
	FIL disable;
	
	if(f_open(&disable, DISABLE_PATH, FA_READ) == FR_OK) {
		pinStatus = PIN_STATUS_NEVER;
	}
	
	if (pinStatus == PIN_STATUS_NEVER || CFG_BOOTENV) {
		bootOrOptions();
		return 0;
	}
	
	/*
	SHA/OTP BYPASS
	*/
	SHACheckResult SHAResult = checkSHA();
	
	if (SHAResult != SHACheckResultNoFile) {
		drawLostImage();
	
		if (SHAResult == SHACheckResultValid) {
			if (!drawImage("0:/3dsafe/bypass.bin", 400, 240, 0, 0, SCREEN_TOP)) {
				clearScreens(SCREEN_TOP);
				drawString("PIN LOCK BYPASSED.\n \nYour device is not currently protected by\n3DSafe because sha.bin/otp.bin is in the root\nof your SD card. You should remove this\nfile to ensure your device is protected\nby 3DSafe.\n \nPress any key to continue.", 10, 10, COLOR_RED);
			}
			
			waitInput();
			
			displayOptions();
			return 0;
		}
		else if (SHAResult == SHACheckResultInvalid) {
			error("SHA bypass failed. Press any key to enter PIN.", false);
		}
	}
        
	
	
	/*
	Create directory for 3DSafe files (if necessary)
	*/
	f_mkdir("1:/3dsafe");
	
	clearScreens(SCREEN_TOP);
	
	drawLostImage();
	
	/*
	Try to read the PIN file from SysNAND
	*/
	//File read buffer
	char pin[PIN_MAX_LENGTH+1];
    
    if (!getPINFromNAND(pin)) {
	    //If no PIN could be read from NAND, try creating a new one
    	setNewPIN(true);
    	
    	//If we still can't get a PIN from NAND, show an error and die
    	if (!getPINFromNAND(pin)) {	
			error("Failed to get PIN from NAND. You should use otp.bin bypass to regain access to your device", true);
		}
    }
    
    drewPINImage = drawImage("0:/3dsafe/pinrequest.bin", 400, 240, 0, 0, SCREEN_TOP);
	
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
		entered[i] = '\0';
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
		u32 volatile i=0;
	
		while(i<5000000) {
			i++;
		}
		
		char last = entered[pinlen-1];
		
		u32 key = HID_PAD;
		char current = translateButton(key);
		
		if (current == last) {
// 			clearScreens(SCREEN_TOP);
// 			drawString("Held last key - show options", 10, 40, COLOR_WHITE);
// 			waitInput();
		
			displayOptions();
		}
		else {
// 			clearScreens(SCREEN_TOP);
// 			drawString("Didn't hold last key - boot", 10, 40, COLOR_WHITE);
// 			waitInput();
		
			bootPayload();
		}
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
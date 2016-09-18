# 3DSafe
*In-NAND PIN lock for 3DS*


## Installation instructions
1. Make a NAND backup. This is essential, as if something goes wrong during installation you will be able to restore it (but you'll need a hardmod to do this)
2. Store your NAND backup safely along with your OTP. You will need these if you forget the PIN.
3. Download and extract the latest 3DSafe release, and copy payload_stage1.bin and payload_stage2.bin to the a9lh folder on the root directory of your 3DS SD card
4. Copy the 3dsafe folder from the 3DSafe release to the root of your SD card
4. Run SafeA9LHInstaller according to its instructions to install the payloads
5. Power off your 3DS and power back on, and you will be prompted to set a new PIN
6. After entering a PIN, you will be able to boot your 3DS
7. Reboot your 3DS and confirm that your PIN unlocks it
8. Once you are back at the 3DSafe options menu, press the R button. This will copy the emergency boot payload to your NAND, so you will be able to boot without your SD card inserted
9. Power off your 3DS. Remove the SD card and power back on. Enter your PIN, and then press START to boot the payload. Confirm that you are able to boot without the SD card inserted. Then power off your 3DS
9. Copy your otp.bin to the root of your SD card, reinsert the SD card into your 3DS, and power on
10. When 3DSafe starts up, confirm that your OTP has successfully circumvented the PIN lock. This will ensure that you have a valid OTP to bypass the PIN lock if you ever forget your 3DSafe PIN
11. Delete the otp.bin from the root of your SD card. Make sure you have multiple backups of this file elsewhere than your 3DS SD card. It's only a small file, so I recommend emailing it to yourself, putting it on a Dropbox, uploading it to a web host, burning it to a disc, putting it on a memory stick, or as many of these as is possible.


## How to create a 'lost' message for the bottom screen
If a file called /3dsafe/lost.bin is found on the SD card, 3DSafe will display this on the bottom screen. Since it's an image, you can use it to display whatever you want. However, I recommend that you use it to display your contact details in case your 3DS is lost. Here's how to do that.

1. Create a 320x240 image in any paint programme. The file MUST be the correct size or it won't display properly (or might cause weird effects when 3DSafe tries to load it)
2. Write your contact details or whatever you want into the image
3. Save the image as a non-transparent PNG
4. Open this page: https://xem.github.io/3DShomebrew/tools/image-to-bin.html
5. Click on choose 'Choose File' and select the image you created
6. Make sure 'rotate 90°' is ticked, and 'preserve transparency' is unticked
7. Click 'Load'
8. Your image will appear on the screen, rotated 90 degrees (this is normal)
9. Right click on 'Download your BIN file' and save the file as lost.bin
10. Place lost.bin in /3dsafe on your SD card

3DSafe will automatically detect the presence of this file and display it. If you have previously used a different lost.bin, the old one will still be displayed because it is being loaded from CTRNAND. To update it, you need to press the R button from the 3DSafe options screen. The copy of lost.bin on NAND will then be updated to match the one from the SD card.
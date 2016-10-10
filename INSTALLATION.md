# 3DSafe
*In-NAND PIN lock for 3DS*


## Installation instructions
### Preliminary reading
Make sure you have read these [important warnings and disclaimer](WARNINGS.md) before beginning installation.

### Installing over older A9LH payloads
If you are on the old version 1 of arm9loaderhax, then you will need to update to the latest version before installing 3DSafe to ensure all the correct files are in place. To do so, please follow [this guide](https://github.com/Plailect/Guide/wiki/Updating-arm9loaderhax). After this, you can continue to follow these instructions to install 3DSafe.

### Installation procedure
1. Make a NAND backup. This is essential, as if something goes wrong during installation you will be able to restore it (but you'll need a hardmod to do this)
2. Store your NAND backup safely.
3. Download and extract the latest 3DSafe release, and copy payload_stage1.bin and payload_stage2.bin to the a9lh folder on the root directory of your 3DS SD card
4. Copy the 3dsafe folder from the 3DSafe release to the root of your SD card
5. Check the MD5 sums of both payloads on the SD card using [these instructions](MD5.md).
6. Run SafeA9LHInstaller according to its instructions to install the payloads
7. Power off your 3DS and power back on, and you will be prompted to set a new PIN
8. After entering a PIN, you will be able to boot your 3DS
9. Reboot your 3DS and confirm that your PIN unlocks it
10. Once you are back at the 3DSafe options menu, press the R button. This will copy the emergency boot payload to your NAND, so you will be able to boot without your SD card inserted
11. Power off your 3DS. Remove the SD card and power back on. Enter your PIN, and then press START to boot the payload. Confirm that you are able to boot without the SD card inserted. Then power off your 3DS
12. Re-insert your SD card and power up your 3DS
13. Enter your PIN and enter the 3DSafe options. Press the L button to dump sha.bin to the root of your SD card. This file is used to bypass the PIN lock if you forget your PIN.
14. Once sha.bin has been dumped to your SD card, reboot your 3DS. You should see a message saying that the PIN was bypassed
15. Copy sha.bin to your computer and keep it very safe. You will need it if you forget your PIN. It's only a small file, so I recommend emailing it to yourself, putting it on a Dropbox, uploading it to a web host, burning it to a disc, putting it on a memory stick, or as many of these as is possible.
16. Delete sha.bin from your 3DS SD card, reboot, and confirm that the PIN lock is no longer bypassed


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

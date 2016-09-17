# 3DSafe
*In-NAND PIN lock for 3DS*

3DSafe is an arm9loaderhax payload which will lock your sysnand with a PIN. The PIN request is displayed as soon as the 3DS is powered on. Because the 3DSafe payload is the A9LH stage1/stage2 payload, it is stored in NAND itself, not on the SD card. The PIN is also stored in NAND, so there is no way to edit or remove the PIN by removing the SD card or modifying files on it. After successfully entering the PIN, arm9loaderhax.bin is loaded from the SD card.

# If you forget your PIN
Because everything to do with 3DSafe is in NAND, you cannot remove the PIN lock or change the PIN until you have already got past the request for the PIN. For this reason, a bypass is included. This involves getting your console-specific OTP, placing it at /otp.bin on your 3DS SD card, and then booting. 3DSafe will detect the presence of the OTP file and bypass the PIN request, allowing you to change the PIN and boot the console.

# You must safeguard your PIN and your OTP.bin
I cannot stress this enough. If you install 3DSafe, forget your PIN and lose your OTP, your 3DS will be a brick. There is absolutely no way to circumvent the PIN request without the OTP.bin. The only thing you would be able to do in this situation would be to hardmod your 3DS and use the hardmod to write a NAND backup which does not have 3DSafe installed (or one in which you know the PIN). If you forget your PIN, lose your OTP.bin and don't have a NAND backup you can restore using a hardmod, your 3DS will be *permanently bricked*.

I reiterate: ***BEFORE*** **installing 3DSafe, make two NAND backups, verify that the md5sums match, do the same for your OTP, and then store your NAND backup and OTP in several safe locations. If you don't do this and forget your PIN, your 3DS is BRICKED.**

# Testing and disclaimer
I have tested this on my EUR n3DS. I make absolutely no guarantee that it will work for anybody else. Since you are writing these payloads to sysNAND, there is a possibility that you will brick your 3DS. I take absolutely no responsibility for this. Do not install this unless you know exactly what you are doing. I highly recommend that you take a NAND backup before installing this, and preferably have a hardmod before installing.

# How to install
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


# How to update
1. Download and extract the latest 3DSafe release, and copy payload_stage1.bin and payload_stage2.bin to the a9lh folder on the root directory of your 3DS SD card
2. Delete the 3dsafe folder on the root of your SD card, and copy the 3dsafe folder from the 3DSafe release to the root of your SD card
3. Boot your 3DS and enter your PIN
4. From the 3DSafe options screen, press X to run SafeA9LHInstaller
5. Press SELECT to begin updating. The process takes less than a second.
6. When prompted, press any key to shut down your 3DS
7. Power on and confirm that the update was successful by checking the version number on the About page

# How to create a 'lost' message for the bottom screen
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


# Credits

This project is based on ShadowNAND by @RShadowhand, from which it is forked. All credit for the original payload is inherited from this project and the projects on which it is based in turn. The modifications in 3DSafe are by @mashers.

3DSafe incorporates parts of GodMode9 by d0k3 for reading and writing the PIN from/to NAND. Credit for the code in 3DSafe which is taken from GodMode9 and modified by mashers is given to d0k and the other contributors to the GodMode9 project. This includes the following components of 3DSafe:
* godmode.c
* godmode.h
* fatfs (modified to mount/read/write 3DS NAND partitions)
* nand

3DSafe includes an integrated version of SafeA9LHInstaller by AuroraWright.

The emergency boot payload is a cut-down version of Luma3DS by AuroraWright.

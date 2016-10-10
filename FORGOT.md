# 3DSafe
*In-NAND PIN lock for 3DS*


## What to do if you forget your PIN
### SHA bypass
The first thing to try if you forget your PIN is the SHA bypass. This requires the sha.bin file which you should have created after installing 3DSafe.

1. Power off your 3DS
2. Insert your SD card into your computer
3. Copy the sha.bin file to the root of your SD card
4. Put the SD card back in your 3DS and power it on
5. You should be greeted with a warning the the PIN lock was bypassed
6. Change your PIN to something you will remember
7. Delete sha.bin from your 3DS SD card

### Re-flash NAND
If you do not have the sha.bin (e.g. if you lost it or did not create it during 3DSafe installation) then you will have to re-flash your NAND with a NAND backup which does not contain 3DSafe (or one in which you remember the PIN). Since 3DSafe will not permit the system to boot, you will not be able to use software like hourglass9 or decrypt9 to re-flash the NAND. You will therefore have to do the following.

1. Install a 3DS hard mod. You can find instructions on how to do this on gbatemp, as well as a list of hard mod installers in your region if you don't want to do it yourself.
2. Connect the hardmod to your computer as per the instructions for re-flashing the NAND
3. Write a NAND backup to the 3DS which does not contain 3DSafe
4. Disconnect the 3DS, power it off and back on again, and you should be able to access your 3DS


### If you have neither sha.bin nor a NAND backup
If you have neither of these files then unfortunately your 3DS is essentially bricked, as you will not be able to bypass the PIN lock unless you can somehow remember your PIN. This is why the installation instructions stress the need to back up both of these files safely during 3DSafe installation.
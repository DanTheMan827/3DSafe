# 3DSafe
*In-NAND PIN lock for 3DS*

3DSafe is an arm9loaderhax payload which will lock your sysnand with a PIN. The PIN request is displayed as soon as the 3DS is powered on. Because the 3DSafe payload is the A9LH stage1/stage2 payload, it is stored in NAND itself, not on the SD card. The PIN is also stored in NAND, so there is no way to edit or remove the PIN by removing the SD card or modifying files on it. After successfully entering the PIN, arm9loaderhax.bin is loaded from the SD card.


# Contents of this file
* What to do if you forget your PIN
* A warning about safeguarding your PIN and OTP
* Testing and disclaimer
* [Installation](INSTALLATION.md)
* [Updating](UPDATE.md)
* [Usage](USAGE.md)
* Credits


# If you forget your PIN
Because everything to do with 3DSafe is in NAND, you cannot remove the PIN lock or change the PIN until you have already got past the request for the PIN. For this reason, a bypass is included. This involves getting your console-specific OTP, placing it at /otp.bin on your 3DS SD card, and then booting. 3DSafe will detect the presence of the OTP file and bypass the PIN request, allowing you to change the PIN and boot the console.


# You must safeguard your PIN and your OTP.bin
I cannot stress this enough. If you install 3DSafe, forget your PIN and lose your OTP, your 3DS will be a brick. There is absolutely no way to circumvent the PIN request without the OTP.bin. The only thing you would be able to do in this situation would be to hardmod your 3DS and use the hardmod to write a NAND backup which does not have 3DSafe installed (or one in which you know the PIN). If you forget your PIN, lose your OTP.bin and don't have a NAND backup you can restore using a hardmod, your 3DS will be *permanently bricked*.

I reiterate: ***BEFORE*** **installing 3DSafe, make two NAND backups, verify that the md5sums match, do the same for your OTP, and then store your NAND backup and OTP in several safe locations. If you don't do this and forget your PIN, your 3DS is BRICKED.**


# Testing and disclaimer
I have tested this on my EUR n3DS. I make absolutely no guarantee that it will work for anybody else. Since you are writing these payloads to sysNAND, there is a possibility that you will brick your 3DS. I take absolutely no responsibility for this. Do not install this unless you know exactly what you are doing. I highly recommend that you take a NAND backup before installing this, and preferably have a hardmod before installing.


# Credits
This project is based on ShadowNAND by @RShadowhand, from which it is forked. All credit for the original payload is inherited from this project and the projects on which it is based in turn. The modifications in 3DSafe are by @mashers.

3DSafe incorporates parts of GodMode9 by d0k3 for reading and writing the PIN from/to NAND. Credit for the code in 3DSafe which is taken from GodMode9 and modified by mashers is given to d0k and the other contributors to the GodMode9 project. This includes the following components of 3DSafe:
* godmode.c
* godmode.h
* fatfs (modified to mount/read/write 3DS NAND partitions)
* nand

3DSafe includes an integrated version of SafeA9LHInstaller by AuroraWright.

The emergency boot payload is a cut-down version of Luma3DS by AuroraWright.

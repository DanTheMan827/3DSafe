# 3DSafe
*In-NAND PIN lock for 3DS*

3DSafe is an arm9loaderhax payload which will lock your sysnand with a PIN. The PIN request is displayed as soon as the 3DS is powered on. Because the 3DSafe payload is the A9LH stage1/stage2 payload, it is stored in NAND itself, not on the SD card. There is no way to change the PIN or circumvent it by removing the SD card or modifying files on it. After successfully entering the PIN, arm9loaderhax.bin is loaded from the SD card.

Currently, the PIN is loaded from a file on SD (/pin.txt). This means that editing the file will change the PIN, and deleting it will remove the requirement to enter the PIN. This will change once NAND reading and writing has been added, at which point the PIN will be stored in NAND and it will not be possible to change it until the PIN has been entered.

# Warning before installation

If you install this payload and then forget the PIN, you will be unable to access your 3DS under all but the following circumstance:

1. You have a backup of your sysNAND before you installed 3DSafe, AND:
2. You have a hardmod through which you can flash this sysNAND backup

Unless you have both of these, if you forget the PIN, your 3DS is completely useless. There is no failsafe, no backdoor. The idea of 3DSafe is that it is impossible to circumvent the PIN request.

# Testing and disclaimer

I have tested this on my EUR n3DS. I make absolutely no guarantee that it will work for anybody else. Since you are writing these payloads to sysNAND, there is a possibility that you will brick your 3DS. I take absolutely no responsibility for this. Do not install this unless you know exactly what you are doing. I highly recommend that you take a NAND backup before installing this, and preferably have a hardmod before installing.

##How to Install
1. Make a NAND backup. This is essential, as if something goes wrong during installation you will be able to restore it (but you'll need a hardmod to do this)
2. From the 'out' directory of this repo, copy payload_stage1.bin and payload_stage2.bin to the a9lh folder on the root directory of your 3DS SD card
3. Create a file called pin.txt at the root of your SD card, and enter your PIN. ONLY enter the characters A, B, X, Y, U, D, L or R. These correspond to the ABXY and D-Pad buttons on the 3DS. If you enter any other characters, it will not be possible to correctly enter the PIN.
4. Run SafeA9LHInstaller according to its instructions to install the payloads
5. Power off your 3DS and power back on, and you will be prompted to enter the PIN
6. After entering the PIN, whatever A9LH payload you already had at /arm9loaderhax.bin will be booted

##Planned features:

* Store PIN in NAND

## Credits

This project is based on ShadowNAND by @RShadowhand, from which it is forked. All credit for the original payload is inherited from this project and the projects on which it is based in turn. The modifications in 3DSafe are by @mashers.

# 3DSafe
*In-NAND PIN lock for 3DS*

3DSafe is an arm9loaderhax payload which will lock your sysnand with a PIN. The PIN request is displayed as soon as the 3DS is powered on. After successfully entering the PIN, arm9loaderhax.bin is loaded from the SD card.

Currently, the PIN is hard-coded in payload_stage2/source/main.c in the variable char pin[]. If you want to change the PIN, you will need to edit the value of this variable, recompile 3DSafe from source, and reinstall the payloads using SafeA9LHInstaller.

Commits will be added shortly which will allow the PIN to be stored in NAND and changed on the fly without having to reinstall anything.

# Warning before installation

If you install this payload and then forget the PIN, you will be unable to access your 3DS under all but the following circumstance:
1. You have a backup of your sysNAND before you installed 3DSafe
2. You have a hardmod through which you can flash this sysNAND backup

Unless you have both of these, if you forget the PIN, your 3DS is completely useless. There is no failsafe, no backdoor. The idea of 3DSafe is that it is impossible to circumvent the PIN request.

##How to Install
1. Set the pin as above to whatever you want it to be (this will change to a more user-friendly method in due course)
2. Compile the payloads by typing 'make' into your console
3. From the 'out' directory, copy payload_stage1.bin and payload_stage2.bin to the a9lh folder on the root directory of your 3DS SD card
4. Run SafeA9LHInstaller according to its instructions to install the payloads
5. Power off your 3DS and power back on, and you will be prompted to enter the PIN
6. After entering the PIN, whatever A9LH payload you already had at /arm9loaderhax.bin will be booted

##Planned features:

* Store PIN in NAND
* Change PIN without recompiling/reinstalling

## Credits

This project is based on ShadowNAND by @RShadowhand, from which it is forked. All credit for the original payload is inherited from this project and the projects on which it is based in turn. The modifications in 3DSafe are by @mashers.

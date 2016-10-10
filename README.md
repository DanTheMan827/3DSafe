# 3DSafe
*In-NAND PIN lock for 3DS*

3DSafe is an arm9loaderhax payload which will lock your sysnand with a PIN. The PIN request is displayed as soon as the 3DS is powered on. Because the 3DSafe payload is the A9LH stage1/stage2 payload, it is stored in NAND itself, not on the SD card. The PIN is also stored in NAND, so there is no way to edit or remove the PIN by removing the SD card or modifying files on it. After successfully entering the PIN, arm9loaderhax.bin is loaded from the SD card.


# Contents
* [Important warnings and disclaimer](WARNINGS.md)
* [Installation](INSTALLATION.md)
* [Updating](UPDATE.md)
* [Usage](USAGE.md)
* [Credits](CREDITS.md)


# [Help, I forgot my PIN!](FORGOT.md)
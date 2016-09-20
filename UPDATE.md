# 3DSafe
*In-NAND PIN lock for 3DS*

If you have not yet installed 3DSafe but are using a different A9LH payload, please use the [installation instructions](INSTALLATION.md). If you are already using 3DSafe and want to update to a newer version, please continue with the following instructions.

## Updating
1. Download and extract the latest 3DSafe release, and copy payload_stage1.bin and payload_stage2.bin to the a9lh folder on the root directory of your 3DS SD card
2. If you have created an owner details image (lost.bin), copy this from your SD card to your computer
3. Delete the 3dsafe folder on the root of your SD card, and copy the 3dsafe folder from the 3DSafe release to the root of your SD card
4. If you copied lost.bin to your computer in step 2, copy it back to the 3dsafe folder on your SD card
5. Check the MD5 sums of both payloads on the SD card using [these instructions](MD5.md).
6. Boot your 3DS and enter your PIN
7. From the 3DSafe options screen, press X to run SafeA9LHInstaller
8. Press SELECT to begin updating. The process takes less than a second.
9. When prompted, press any key to shut down your 3DS
10. Power on and confirm that the update was successful by checking the version number on the About page
11. If you are using the default 3DSafe emergency boot payload, you should update it in case there were changes made since the last version of 3DSafe you had installed. You can do this by pressing the R button from the 3DSafe options menu

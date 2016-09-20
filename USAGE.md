# 3DSafe
*In-NAND PIN lock for 3DS*

## Usage
Under normal circumstances after installing and setting up 3DSafe, booting your 3DS will cause you to be prompted to enter your PIN. After doing so, your 3DS will boot up as normal. Rebooting your 3DS (for example, after exiting System Settings) will not cause the PIN request to appear.

In order to change the settings of 3DSafe, you will need to do one of the following:

* If you are cold booting your 3DS, hold the last key of your PIN.
* If you are rebooting your 3DS, hold the UP key.

Carrying out either of these actions will cause the 3DSafe options menu to appear. From here you can configure the following settings.

* **START: Boot payload**. This option will boot the arm9loaderhax.bin on your SD card to start up your 3DS. If none is present, 3DSafe will attempt to boot the emergency payload from CTRNAND, which will boot Luma to SysNAND. If this is not present either, then you will be presented with an error message.
* **SELECT: Toggle PIN**. This option toggles the PIN lock on or off. If the PIN lock is turned off, then your 3DS will boot your arm9loaderhax payload (or emergency payload) immediately without asking for the PIN and without showing any options. To return to the 3DSafe options menu (for example to re-enable the PIN lock or change other options), power on your 3DS while holding the UP button. This will present the 3DSafe options menu on boot.
* **R: Update NAND files**. If you have the either emergency.bin (the emergency arm9loaderhax payload) or lost.bin (the owner contact details image) in the 3dsafe folder on your SD card, then choosing this option will cause these files to be copied to CTRNAND. This will mean that you will be able to view the owner contact details and boot the emergency payload even if you don't have an SD card inserted.
* **A: Change PIN**. Change the PIN which is requested when booting your 3DS.
* **B: Power off**. Powers off the console.
* **X: SafeA9LHInstaller**. Runs the built-in version of SafeA9LHInstaller. This is useful for updating 3DSafe, or installing a different A9LH payload to remove 3DSafe.
* **Y: About 3DSafe**. Displays version information and credits.
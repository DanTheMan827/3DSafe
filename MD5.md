# 3DSafe
*In-NAND PIN lock for 3DS*

## Checking 3DSafe payload MD5 sums
The release notes for each version of 3DSafe now include the MD5 sums for each of the payload files. It is important to check the MD5 sums of each file and compare then to the sums shown in the release notes. This will ensure that the files were downloaded, extracted and copied without corruption. Installing a corrupt payload could brick your 3DS, so it is extremely important to check the MD5 sums before installing or updating 3DSafe.
1. Copy the payload files to your SD card as per the installation/update instructions.
2. Open a new browser window and go to [onlinemd5.com](http://onlinemd5.com).
3. Copy the MD5 sum for payload_stage1.bin into the 'compare with' field.
4. Drag payload_stage1.bin from your SD card into the box titled "Click to select a file, or drag and drop it here" on the onlinemd5 web page.
5. Click the 'Compare' button, and ensure you see a green tick next to the 'Compare with' field, which will confirm that the MD5 sums match.
6. Repeat steps 3-5 with payload_stage2.bin.

Once you have confirmed that the MD5 sums for both payloads match what is shown in the release notes, you can continue to install or update 3DSafe.
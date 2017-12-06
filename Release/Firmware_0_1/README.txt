PS E:\Dropbox\Projects\DevBoards\BTLE Breakout Board\workspace\Release\Firmware_0_1> ls


    Directory: E:\Dropbox\Projects\DevBoards\BTLE Breakout Board\workspace\Release\Firmware_0_1


Mode                LastWriteTime         Length Name
----                -------------         ------ ----
-a----        3/30/2016   7:59 PM             75 app_valid_setting_apply.hex
-a----        6/23/2016  10:55 PM          37341 bootloader.hex
-a----        6/23/2016  11:20 PM            389 create_merged_hex.bat
-a----        6/23/2016  10:17 PM          72935 nrf51422_xxac_s110.hex
-a----        6/23/2016  11:20 PM            513 Program.ps1
-a----        6/13/2016  12:17 PM           2167 README.txt
-a----        2/27/2015   9:44 AM         254101 s110_nrf51_8.0.0_softdevice.hex


PS E:\Dropbox\Projects\DevBoards\BTLE Breakout Board\workspace\Release\Firmware_0_1> .\create_merged_hex.bat
Parsing .\s110_nrf51_8.0.0_softdevice.hex file.
Parsing .\bootloader.hex file.
Merging files.
Writing merged file.
Parsing intermediate_1.hex file.
Parsing .\nrf51422_xxac_s110.hex file.
Merging files.
Writing merged file.
Parsing intermediate_2.hex file.
Parsing .\app_valid_setting_apply.hex file.
Merging files.
Writing merged file.
PS E:\Dropbox\Projects\DevBoards\BTLE Breakout Board\workspace\Release\Firmware_0_1> .\Program.ps1
& C:\Program Files (x86)\Nordic Semiconductor\nrf51\bin\nrfjprog.exe  --eraseall
& C:\Program Files (x86)\Nordic Semiconductor\nrf51\bin\nrfjprog.exe  --program .\merge\SD_SEC_BL_DA_Embedded.hex
C:\Program Files (x86)\Nordic Semiconductor\nrf51\bin\nrfjprog.exe  -r
Applying system reset.
Erasing all flash (code region 0, code region 1 and UICR).
Applying system reset.
Parsing hex file(s).
Checking that there is no data to be written in region 0.
Checking that there are no UICR conflicts.
Checking that the area to download is erased, and erasing if necessary.
Programming.
Applying system reset.
PS E:\Dropbox\Projects\DevBoards\BTLE Breakout Board\workspace\Release\Firmware_0_1>




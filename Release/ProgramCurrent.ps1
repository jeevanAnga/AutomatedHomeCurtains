
param (
   [string]$debugger = ""#518005338
   )
###& cd E:\Dropbox\Projects\DistilleryAutomation\workspace\vs_solution\Release
Set-Variable proj "C:\Program Files (x86)\Nordic Semiconductor\nrf51\bin\nrfjprog.exe"
#Set-Variable debugger "-s 518005338"

Write-Host "& $proj $debugger --eraseall"
Write-Host "& $proj $debugger --program .\merge\SD_SEC_BL_DA_Embedded.hex"
Write-Host "$proj $debugger -r"

& $proj $debugger --eraseall -s 518005338
& $proj $debugger --program E:/Dropbox/Projects/DFTS/Target/nRF51822/s110_nrf51_8.0.0/s110_nrf51_8.0.0_softdevice.hex -s 518005338
& $proj $debugger --program E:/Dropbox/Projects/DevBoards/BTLE Breakout Board/workspace/nRF51_SDK_10.0.0_dc26b5e/examples/ble_peripheral/ble_bluchip/pca10028/s110_with_dfu/arm4/_build/nrf51422_xxac_s110.hex -s 518005338
& $proj $debugger -r -s 518005338
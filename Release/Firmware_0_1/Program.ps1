
param (
   [string]$debugger = ""#518005338
   )
###& cd E:\Dropbox\Projects\DistilleryAutomation\workspace\vs_solution\Release
Set-Variable proj "C:\Program Files (x86)\Nordic Semiconductor\nrf51\bin\nrfjprog.exe"
#Set-Variable debugger "-s 518005338"

Write-Host "& $proj $debugger --eraseall"
Write-Host "& $proj $debugger --program .\merge\SD_SEC_BL_DA_Embedded.hex"
Write-Host "$proj $debugger -r"

& $proj $debugger --eraseall
& $proj $debugger --program .\bluchip.hex
& $proj $debugger -r
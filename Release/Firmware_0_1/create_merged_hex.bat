@echo off
setlocal
set mergehex="C:\Program Files (x86)\Nordic Semiconductor\nrf5x\bin\mergehex.exe"
mergehex.exe --merge .\s110_nrf51_8.0.0_softdevice.hex .\bootloader.hex --output intermediate_1.hex
mergehex.exe --merge intermediate_1.hex .\nrf51422_xxac_s110.hex --output intermediate_2.hex
mergehex.exe --merge intermediate_2.hex .\app_valid_setting_apply.hex --output bluchip.hex
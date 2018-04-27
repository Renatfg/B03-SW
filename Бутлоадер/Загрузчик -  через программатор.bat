avrdude -V -c usbtiny -p at90usb1286 -U flash:w:BootloaderHID.hex:i
avrdude -c usbtiny -p at90usb1286 -U lfuse:w:0xde:m -U hfuse:w:0xdb:m -U efuse:w:0xf0:m
pause >null
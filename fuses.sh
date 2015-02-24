avrdude -c buspirate -P /dev/ttyUSB0 -p m2560 -U efuse:w:0xFD:m -U hfuse:w:0xD8:m -U lfuse:w:0xFF:m

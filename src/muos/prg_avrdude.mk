
AVRDUDE = avrdude

AVRDUDE_PART = -p $(MCU)

AVRDUDE_FLAGS = -c $(AVRDUDE_PROGRAMMER) $(AVRDUDE_PART) -b $(AVRDUDE_BAUDRATE) -P $(AVRDUDE_PORT)
AVRDUDE_DEFAULT_SCK = -B 8
#AVRDUDE_128kHz_SCK = -B 16
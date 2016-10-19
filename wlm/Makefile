CC = xtensa-lx106-elf-gcc
CFLAGS = -I. -mlongcalls
LDLIBS = -nostdlib -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lpp -lphy -Wl,--end-group -lgcc
LDFLAGS = -Teagle.app.v6.ld

wlm-0x00000.bin:wlm
	esptool.py elf2image $^

wlm: wlm.o \
     hc_04.o

flash: wlm-0x00000.bin
	esptool.py --port /dev/ttyUSB1 write_flash 0 wlm-0x00000.bin 0x40000 wlm-0x40000.bin

clean:
	rm -f wlm wlm.o wlm-0x00000.bin wlm-0x40000.bin

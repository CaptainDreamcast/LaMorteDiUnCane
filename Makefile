TARGET = 1ST_READ
OBJS = main.o gamescreen.o player.o bullet.o collision.o enemies.o \
gamelogic.o kidnapper.o logoscreen.o retryscreen.o shadow.o \
stage.o  titlescreen.o ui.o manualenemy.o \
romdisk.o 
OPTFLAGS=-O3 -fomit-frame-pointer -fno-delayed-branch -DDREAMCAST -Wall -Werror
KOS_CFLAGS+= $(OPTFLAGS) 
KOS_ROMDISK_DIR = romdisk_boot

all: complete

complete: clean build_images build_develop

deploy: clean build_images to_romdisk build_deploy

build_images: copy_debug genlevels

copy_debug:
	$(KOS_BASE)/utils/kmgenc/kmgenc -a4 $(wildcard assets/debug/*.png)
	find assets/debug -name '*.kmg' | xargs $(KOS_BASE)/addons/libtari/tools/bin/kompressor
	$(KOS_BASE)/utils/kmgenc/kmgenc -a4 $(wildcard assets/fonts/*.png)
	find assets/fonts -name '*.kmg' | xargs $(KOS_BASE)/addons/libtari/tools/bin/kompressor
	mkdir romdisk_boot
	mkdir romdisk_boot/fonts
	cp assets/fonts/* romdisk_boot/fonts
	find romdisk_boot/fonts/ -name '*.png' | xargs rm -f
	find romdisk_boot/fonts/ -name '*.kmg' | xargs rm -f
	mkdir romdisk_boot/debug
	cp assets/debug/*.pkg romdisk_boot/debug
	mkdir filesystem
	mkdir filesystem/assets

to_romdisk:
	cp -r filesystem/* romdisk_boot

genlevels:
	cp -r assets/* filesystem/assets
	
	find filesystem/assets/ -name '*.png' | xargs $(KOS_BASE)/utils/kmgenc/kmgenc -a4 
	find filesystem/assets/ -name '*.kmg' | xargs $(KOS_BASE)/addons/libtari/tools/bin/kompressor
	find filesystem/assets/ -name '*.png' | xargs rm -f
	find filesystem/assets/ -name '*.kmg' | xargs rm -f
	find filesystem/assets/ -name '*.xcf' | xargs rm -f
	

clean:
	-rm -f $(TARGET).elf $(OBJS)
	-rm -f $(TARGET).BIN
	-rm -f assets/debug/*.pkg
	-rm -r -f romdisk_boot
	-rm -r -f filesystem
	-rm -f romdisk.img

build_develop: $(OBJS) 
	$(KOS_CC) $(KOS_CFLAGS) -DDEVELOP -I${KOS_BASE}/../extensions/include $(KOS_LDFLAGS) \
	-o $(TARGET).elf $(KOS_START) \
	$(OBJS) -lkmg $(OPTIONAL_LIBS) -lm -ltremor -ltari $(OBJEXTRA) $(KOS_LIBS)
	$(KOS_OBJCOPY) -O binary $(TARGET).elf $(TARGET).BIN

build_deploy: $(OBJS) 
	$(KOS_CC) $(KOS_CFLAGS) -I${KOS_BASE}/../extensions/include $(KOS_LDFLAGS) \
	-o $(TARGET).elf $(KOS_START) \
	$(OBJS) -lkmg $(OPTIONAL_LIBS) -lm -ltremor -ltari $(OBJEXTRA) $(KOS_LIBS)
	$(KOS_OBJCOPY) -O binary $(TARGET).elf $(TARGET).BIN

include $(KOS_BASE)/Makefile.rules

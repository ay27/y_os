EntryPoint	=	0x30400

#boot
boot_asm	=	boot/boot.asm
boot_bin	=	boot/boot.bin
loader_asm  =   boot/loader.asm
loader_bin	= 	boot/LOADER.BIN

#kernel
global_c	=	kernel/global.c
i8259_c		=	kernel/i8259.c
kernel_asm	=	kernel/kernel.asm
kernel_bin	=	kernel/KERNEL.BIN
main_c		=	kernel/main.c
protect_c	=	kernel/protect.c
start_c		=	kernel/start.c


#lib
fun_asm		=	lib/fun_asm.inc
io_lib_c	=	lib/io_lib.c
num_lib_c	=	lib/num_lib.c
process_lib_c=	lib/process_lib.c
protect_asm	=	lib/protect_asm.inc
string_lib_c=	lib/string_lib.c

ASM		=	$(boot_asm) $(loader.asm) $(kernel_asm)
BIN		=	$(boot_bin) $(loader_bin) $(kernel_bin)
#OBJS	=	kernel.o start.o fun_asm.o io_lib.o i8259.o protect.o \
#			protect_asm.o
# 必须把kernel.o放在第一位！
OBJS	=	kernel.o i8259.o protect.o start.o main.o global.o \
			fun_asm.o io_lib.o protect_asm.o string_lib.o num_lib.o process_lib.o



.PHONY:everything clean

everything:$(BIN)
	dd if=$(boot_bin) of=y.img bs=512 count=1 conv=notrunc
	sudo mount -o loop y.img /mnt/floppy
	sudo cp $(loader_bin) $(kernel_bin) /mnt/floppy
	sync
	sudo umount /mnt/floppy
	make clean


$(boot_bin):$(boot_asm)
	nasm -i boot/boot.include/ $< -o $@

$(loader_bin):$(loader_asm)
	nasm -i boot/boot.include/ $< -o $@

$(kernel_bin):$(OBJS)
	ld -s -Ttext $(EntryPoint) -o $@ $(OBJS)
	rm $(OBJS)


#kernel
global.o:$(global_c)
	gcc -I include/ -c -o $@ $<


i8259.o:$(i8259_c)
	gcc -I include/ -c -o $@ $<


kernel.o:$(kernel_asm)
	nasm -i include/ -f elf -o $@ $<


main.o:$(main_c)
	gcc -I include/ -c -o $@ $<


protect.o:$(protect_c)
	gcc -I include/ -c -o $@ $<


start.o:$(start_c)
	gcc -I include/ -c -o $@ $<


#lib
fun_asm.o:$(fun_asm)
	nasm -i include/ -f elf -o $@ $<


io_lib.o:$(io_lib_c)
	gcc -I include/ -c -o $@ $<


num_lib.o:$(num_lib_c)
	gcc -I include/ -c -o $@ $<


process_lib.o:$(process_lib_c)
	gcc -I include/ -c -o $@ $<


protect_asm.o:$(protect_asm)
	nasm -i include/ -f elf -o $@ $<


string_lib.o:$(string_lib_c)
	gcc -I include/ -c -o $@ $<


clean:
	rm $(BIN)

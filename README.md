# simple-multithreaded-kernel

The code here was made during the course `Developing a Multithreaded Kernel From Scratch` available on Udemy platform.

### Boot 

1 - The Bios is executed directly from ROM
2 - The Bios loads the bootloader into address 0x7c00
3 - The bootloader loads the kernel

### Bootloader

A bootloader is a small program responsible for loading the kernel of an operating system (Like GRUB for linux).

### Intel Real Mode

Is the simplest execution mode of Intel x86 processors, introduced with the Intel 8086 in 1978. When a modern CPU powers on, it always starts in real mode for backward compatibility.

It has no memory protection and direct hardware access, typically used by bootloaders. It's only a temporary stage before entering protected mode.

It has access to only 1MB of memory.

### Protected Mode

Protected mode is a 32-bit operating mode of x86 CPUs that enables memory protection, multitasking and advanced OS features. Modern 32-bit operating systems like windows XP 32-bit or 32-bit Linux runs in protected mode.
In real mode, any code can access any memory but in protected mode the CPU enforces rules.


### Paging memory scheme

In general, paging memory scheme is a way an operating system manages memory by splitting it into fixed-size blocks and mapping them so programs think they have their own continuos memory.
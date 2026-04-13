# simple-multithreaded-kernel

The code here was made during the course `Developing a Multithreaded Kernel From Scratch` available on Udemy platform.

## Boot 

1 - The Bios is executed directly from ROM
2 - The Bios loads the bootloader into address 0x7c00
3 - The bootloader loads the kernel

## Bootloader

A bootloader is a small program responsible for loading the kernel of an operating system (Like GRUB for linux).

## Intel Real Mode

Is the simplest execution mode of Intel x86 processors, introduced with the Intel 8086 in 1978. When a modern CPU powers on, it always starts in real mode for backward compatibility.

It has no memory protection and direct hardware access, typically used by bootloaders. It's only a temporary stage before entering protected mode.

It has access to only 1MB of memory.

## Protected Mode

Protected mode is a 32-bit operating mode of x86 CPUs that enables memory protection, multitasking and advanced OS features. Modern 32-bit operating systems like windows XP 32-bit or 32-bit Linux runs in protected mode.
In real mode, any code can access any memory but in protected mode the CPU enforces rules.

### Entering Protected Mode

As stated in the OSDev Wiki, before switching to protected mode
we must:
- Disable interrupts, including NMI
- Enable the A20 Line
- Load tthe Global Descriptor Table with segment descriptors suitable for code, data and stack.

Wheter the CPU is in Real Mode or in Protected Mode is defined by the lowest bit of the CR0 or MSW register.

This example loads a descriptor table into the processor's GDTR register, and then sets the lowest bit of CR0:

<pre>
cli            ; disable interrupts
lgdt [gdtr]    ; load GDT register with start address of Global Descriptor Table
mov eax, cr0 
or al, 1       ; set PE (Protection Enable) bit in CR0 (Control Register 0)
mov cr0, eax

; Perform far jump to selector 08h (offset into GDT, pointing at a 32bit PM code segment descriptor) 
; to load CS with proper PM32 descriptor)
jmp 08h:PModeMain

PModeMain:
; load DS, ES, FS, GS, SS, ESP
</pre>

In order to test if the program is indeed in Protected mode you can run the command inside gdb console:

<pre> target remote | qemu-system-x86_64 -hda ./boot.bin -S -gdb stdio </pre>

And with the command "Info registers" you can check the CS register pointing to 0x8.

## Paging memory scheme

In general, paging memory scheme is a way an operating system manages memory by splitting it into fixed-size blocks and mapping them so programs think they have their own continuos memory.

## A20 line

By enabling the A20 it allows to extended memory access, following the OSDev wiki, it can be enabled like so: 

<pre>
in al, 0x92
or al, 2
out 0x92, al
</pre>

### Text mode

Text mode is a display mode where the screen is treated as a grid of characters rather than individual
pixels.
- The screen is divided into rows and columns (e.g., 80x25).
- You write ascii characters into memory starting at address 0xB8000 for coloured displays
- Or for monochrome displays address 0xB0000
- Each ascii character written to this memory has its pixel equivalent outputted to the monitor

Each cell contains: 
- A character (Like ASCII symbols, letters, numbers)
- An attribute (color, brightness, blinking, etc.)

Instead of drawing individual pixels, you put characters at any row or collumn. 

Each character takes up two bytes
- Byte 0 = ascii character e.g. 'A'
- Byte 1 = Colour code

Example: We want to set row 0 column 0 to a black "A"
0xb8000 = 'A'
0xb8001 = 0x00

Example: Row 0 column 1 to black "B"
0xb8002 = "B"
0xb8003 = 0x00

The advantages of Text Mode:

Simplicity
 - Easier to implement than graphics mode.
 - No need for complex drivers or rendering logic.
 - Perfect for early OS stages (bootloaders, kernels)

Low resource usage
 - Uses very little memory and CPU.

Direct hardware access
- Write directly to video memory
- No abstraction layers required

Fast output
- Writing characters is faster than drawing individual pixels.

Stable and Reliable
- Works even before full hardware initialization
- Supported by legacy BIOS and early boot environments

Good for debugging
- When graphics drivers aren't ready, text mode is still usable
- Many kernels print logs this way during development.

### Interupt Descriptor Table

The Interrupt Descriptor Table (IDT) is a core data strucuture in x86-based operating systems that 
tells the CPU how to handle interrupts and exceptions.

The IDT is basically a lookup table of handlers, the CPU has interrupt numbers like:
- 0 -> divided-by-zero exception
- 13 -> general protection fault
- 32+ -> hardware interrupts (like keyboard, timer)

The IDT maps each of these numbers to a function (handler) in the kernel.

In Real mode we use interrupt vector table, but in Protected mode the Interrupt Descriptor Table is
used.

### Programmable interrupt controller

Allows Hardware to interrupt the processor
- The programmable interrupt controller allows different types of hardware to interrupt the 
processor such as the hard disk, keyboard and more..

IRQ's are mapped to a starting interrupt for example lets choose 0x20.
- IRQ 0 would then be interrupt 0x20
- IRQ 1 would then be interrupt 0x21
- IRQ 2 would then be interrupt 0x22

# Master vs Slave

The system has two PIC (Programmable interrupt Controller) one for master ports and the other for 
slave ports
- The master handles IRQ 0-7
- The lsave handles IRQ 8-15

### What is the Heap ?

The Heap is a giant memory region that can be shared in a controlled manner, you can ask the heap 
for memory and tell the heap when your done with that memory, heap implementations are essentially 
system memory managers.

The heap is a part of process memory where variables are allocated and freed manually (or 
automatically via a runtime/GC) during execution.

- in C: malloc, calloc
- in C++: new
- In higher-level languages: objects are often allocated automatically

OS provides memory to the processs using system calls like:
- brk / sbrk (traditional heap growth)
- mmap (modern allocation) 

Usually a heap allocator:
- Keeps track of gree/used blocks
- Splits and merges memory chunks
- Avoids fragmentation (ideally)

# Limits for a 32 bit kernel

In protected mode the processor is in a 32 bit state, so we have access only to 32 bit memory 
addresses allowing us to address to a maximum of 4.29gb or 429496726 bytes of ram regardless of how 
much system RAM is installed.

# The simplest heap implementation

- Start with a address and call it a "current_address" point it somewhere free i.e. 0x01000000
- Any call to malloc gets the current address and stores it in a temporary called "tmp"
- Now the current address is incremented by the size provided to "malloc"
- Temporary variable called "tmp" that contains the allocated address is returned.
- current_address now contains the next address for "malloc" to return when "malloc" is called 
again.

This is easy to implement, however in this way, memory won't be released and the system may require
a reset, also there is no bound checking.

<pre>
void* current_address = (void*)(0x01000000);
void* malloc(int size)
{
    void* tmp = current_address;
    current_address += size;
    return tmp;
}

void free(void* ptr)
{
    // Due to this design, a free function cannot be implemented
}
</pre>

# What is paging ?

Is a memory management technique used by operating systems to efficiently handle a program's memory
without requiring it to be stored in one continuous block of physical RAM.

Paging divides memory into fixed-size chunks:
- Pages -> blocks of virtual memory (used by programs)
- Frames -> blocks of physical memory RAM

Instead of placing a program in one contiguous chunk of RAM, the OS splits the program into pages, 
loads those pages into any available frames in RAM and keeps track of where each page is stored 
using a page table. The OS translates it into physical address.

## What is it used for ?

### Efficient memory usage
- No need for contiguous allocation -> avoids external fragmentation
- Makes better use of available RAM

### Virtual Memory
- Enables systems to use more memory than phisically available
- Pages not currently needed can be stored on disk (swap space)

#### Process isolation & protection
- Each process has its own page table

## Downsides
- Page table overhead (uses memory)
- Address translation cost
- Page faults (When a page isn't in RAM -> slower disk access)
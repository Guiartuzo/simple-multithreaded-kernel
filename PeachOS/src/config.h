#ifndef CONFIG_H
#define CONFIG_H

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10
#define PEACHOS_TOTAL_INTERRUPTS 512

/* 100Mb heap size */
#define PEACHOS_HEAP_SIZE_BYTES 104857600
#define PEACHOS_HEAP_BLOCK_SIZE 4096

/* The following values were taken from OS Dev wiki, section Memory Map (x86) */
#define PEACHOS_HEAP_ADDRESS 0x01000000        //< From OS Dev, Extended memory - RAM free for use
#define PEACHOS_HEAP_TABLE_ADDRESS 0x00007E00  //< From OS Dev, usable memory / 480 Kb

#define PEACHOS_SECTOR_SIZE 512

#endif
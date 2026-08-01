#define MAX_MEMORY_SIZE             (256*1024)
#define IVT_SIZE                    320
#define AVAILABLE_MEMORY            (MAX_MEMORY_SIZE - IVT_SIZE)

#define M4_PROCESS_STACK_SIZE       2048
#define M4_MAIN_STACK_SIZE          (16*1024)
#define M4_TOTAL_STACK_SIZE         (M4_PROCESS_STACK_SIZE + M4_MAIN_STACK_SIZE + 8)

/*
 * HSM M4 RAM is 256KB total. The HSM ROM loads the firmware as a single
 * contiguous flat binary at address 0x0, so ALL loadable content (.text,
 * .data, .rodata) must fit in HSM RAM. Only NOLOAD sections (.bss, .sysmem,
 * .bss.filebuf) may overflow into L2 OCRAM (0x70000000), which the M4 can
 * access at runtime via the mpu_firewall (L2OCRAM_BANK0_SLV) config.
 */
#define HSM_RAM_CODE_DATA_SIZE          (AVAILABLE_MEMORY - M4_TOTAL_STACK_SIZE)
#define HSM_RAM_UNINIT_DATA_SIZE        (16*1024)

/* make sure below retain is there in your linker command file, it keeps the vector table in the final binary */
--retain="*(.vectors)"
/* This is the stack that is used by code running within main()
 * In case of NORTOS,
 * - This means all the code outside of ISR uses this stack
 * In case of FreeRTOS
 * - This means all the code until vTaskStartScheduler() is called in main()
 *   uses this stack.
 * - After vTaskStartScheduler() each task created in FreeRTOS has its own stack
 */
--stack_size=8192
/* This is the heap size for malloc() API in NORTOS and FreeRTOS
 * This is also the heap used by pvPortMalloc in FreeRTOS
 */
--heap_size=1024

SECTIONS
{
    /* This has the M4F entry point and vector table, this MUST be at 0x0 */
    .vectors:   {} palign(8) > VECTORS
    /* Code resides in HSM secure RAM (loaded at 0x0 by the HSM ROM) */
    .text:      {} palign(8) > HSM_RAM_CODE_DATA

    .bss:       {} palign(8) > HSM_RAM_UNINIT|OCRAM_CODE     /* This is where uninitialized globals go */
    RUN_START(__BSS_START)
    RUN_END(__BSS_END)

    .data:      {} palign(8) > HSM_RAM_CODE_DATA     /* This is where initialized globals and static go */
    .rodata:    {} palign(8) > HSM_RAM_CODE_DATA     /* This is where const's go */
    .sysmem:    {} palign(8) > HSM_RAM_UNINIT|OCRAM_CODE     /* This is where the malloc heap goes */
    .stack:     {
        . += M4_MAIN_STACK_SIZE;
        M4_MAIN_STACK_ADDRESS = .;
    } palign(8) > STACK     /* This is where the main() stack goes */

    .processStack: {
        . += M4_PROCESS_STACK_SIZE;
        M4_PROCESS_STACK_ADDRESS = .;
    } palign(8) > STACK

    /* Sections needed for C++ projects */
    .ARM.exidx:     {} palign(8) > HSM_RAM_CODE_DATA  /* Needed for C++ exception handling */
    .init_array:    {} palign(8) > HSM_RAM_CODE_DATA  /* Contains function pointers called before main */
    .fini_array:    {} palign(8) > HSM_RAM_CODE_DATA /* Contains function pointers called after main */
    .bss.sipc_hsm_queue_mem   (NOLOAD) : {} > MAILBOX_HSM
    .bss.sipc_secure_host_queue_mem   (NOLOAD) : {} > MAILBOX_R5F
    .bss.filebuf (NOLOAD) : {} > OCRAM_BANK0

    /*-----------------------------------------------------------------------------
     * ASSETS:
     * - The assets are always placed at the beginning of the SECURE_RAM. This is
     *   fixed address where ROM shares some important information for HSMRt functioning.
     *-----------------------------------------------------------------------------*/
    .assets (NOLOAD):
    {
    } > ASSETS
}

MEMORY
{
    VECTORS             : ORIGIN = 0x00000000 , LENGTH = IVT_SIZE
    HSM_RAM_CODE_DATA   : ORIGIN = end(VECTORS), LENGTH = HSM_RAM_CODE_DATA_SIZE
    HSM_RAM_UNINIT      : ORIGIN = end(HSM_RAM_CODE_DATA), LENGTH = HSM_RAM_UNINIT_DATA_SIZE
    STACK               : ORIGIN = end(HSM_RAM_UNINIT), LENGTH = M4_TOTAL_STACK_SIZE
    MAILBOX_HSM         : ORIGIN = 0x44000000 , LENGTH = 0x000003CE
    MAILBOX_R5F         : ORIGIN = 0x44000400 , LENGTH = 0x000003CE
    ASSETS              : ORIGIN = 0x46050000 , LENGTH = 0x000003C0
    /*
     * L2 OCRAM bank0 is a 512KB region at 0x70000000 that is opened to the HSM
     * M4 core by the mpu_firewall (L2OCRAM_BANK0_SLV) config in wolfssl.syscfg.
     * Only NOLOAD sections (uninitialized .bss, malloc heap) overflow here; code
     * and initialized data must stay in HSM RAM (0x0) because the HSM ROM loads
     * a single contiguous image at 0x0.
     */
    OCRAM_CODE         : ORIGIN = 0x70020000 , LENGTH = 0x47800
    OCRAM_BANK0        : ORIGIN = 0x70067800 , LENGTH = 0x18000 /** For storing dthe test input and output buffers*/
    OCRAM_BANK1        : ORIGIN = 0x70080000 , LENGTH = 0x2500 /** For storing dthe test memory logs*/
}

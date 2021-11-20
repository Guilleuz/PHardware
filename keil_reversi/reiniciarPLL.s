
	AREA Datos, DATA, ALIGN=4
	
	
PLL_BASE        EQU     0xE01FC080      ; PLL Base Address
PLLCON_OFS      EQU     0x00            ; PLL Control Offset
PLLCFG_OFS      EQU     0x04            ; PLL Configuration Offset
PLLSTAT_OFS     EQU     0x08            ; PLL Status Offset
PLLFEED_OFS     EQU     0x0C            ; PLL Feed Offset
PLLCON_PLLE     EQU     (1<<0)          ; PLL Enable
PLLSTAT_PLOCK   EQU     (1<<10)         ; PLL Lock Status	
PLLCFG_Val      EQU     0x00000024
PLLCON_PLLC     EQU     (1<<1)          ; PLL Connect
	
	AREA Codigo, CODE
    EXPORT reiniciarPLL
	PRESERVE8 {TRUE}
ENTRY

reiniciarPLL

    LDR     R0, =PLL_BASE
    MOV     R1, #0xAA
    MOV     R2, #0x55

;  Configure and Enable PLL
    MOV     R3, #PLLCFG_Val
    STR     R3, [R0, #PLLCFG_OFS] 
    MOV     R3, #PLLCON_PLLE
    STR     R3, [R0, #PLLCON_OFS]
    STR     R1, [R0, #PLLFEED_OFS]
    STR     R2, [R0, #PLLFEED_OFS]

;  Wait until PLL Locked
PLL_Loop        
    LDR     R3, [R0, #PLLSTAT_OFS]
    ANDS    R3, R3, #PLLSTAT_PLOCK
    BEQ     PLL_Loop

;  Switch to PLL Clock
    MOV     R3, #(PLLCON_PLLE:OR:PLLCON_PLLC)
    STR     R3, [R0, #PLLCON_OFS]
    STR     R1, [R0, #PLLFEED_OFS]
    STR     R2, [R0, #PLLFEED_OFS]
	BX    	LR 
    END

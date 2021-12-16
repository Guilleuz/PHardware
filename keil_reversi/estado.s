	AREA Codigo, CODE
    EXPORT get_CPSR
	PRESERVE8 {TRUE}
	ENTRY
	
get_CPSR
	stmdb SP!, {LR}
	mrs r0, cpsr ; return CPSR
	ldmia SP!, {PC}
	END
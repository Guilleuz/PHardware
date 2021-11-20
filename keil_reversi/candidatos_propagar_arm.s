	AREA datos, DATA
vector 	DCB 0, 0, 0, 3, 3, 3, 6, 6, 6	
	ALIGN 4
	
	AREA Codigo, CODE
	EXPORT candidatos_propagar_arm
	PRESERVE8 {TRUE}
		
ENTRY

 ;propaga el valor de una determinada celda en C
 ;para actualizar las listas de candidatos
 ;de las celdas en su su fila, columna y región */

 ;Recibe como parametro la cuadricula, y la fila y columna de
 ;la celda a propagar; no devuelve nada


 ;Parámetros:
 ;r0 = @cuadricula 
 ;r1 = fila
 ;r2 = columna
 ;Registros utilizados:
 ;r4 = valor de cuadricula al recorrer la región
 ;r5 = #1
 ;r6 = valor de cuadricula[fila][i] o cuadricula[i][columna]
 ;r7 = i
 ;r8 = @cuadricula[fila][i]
 ;r9 = @cuadricula[i][columna]
 ;r10 = valor celda + BIT_CANDIDATOS - 1

candidatos_propagar_arm
	stmdb SP!, {r4-r10, LR}
	
	mov r7, #0 				; r7 = i
	add r8, r0, r1, LSL #5 	; r8 = @cuadricula[fila][0]
	add r9, r0, r2, LSL #1 	; r9 = @cuadricula[0][columna]
	add r10, r8, r2, LSL #1 ; r10 = @cuadricula[fila][columna]	
	ldrh r10, [r10]			; r10 = celda
	and r10, r10, #0x000f 	; r10 = valor celda
	add r10, r10, #6 		; r10 = valor + BIT_CANDIDATOS - 1
	mov r5, #1 				; r5 = 1

	; propagamos el candidato en la fila y la columna
BUCLE
	cmp r7, #9
	beq finBUC
	ldrh r6, [r8]  
	orr r6, r6, r5, LSL r10 ; r6 = cuadricula[fila][i] | (#1 << r10)
	strh r6, [r8], #2		; guardamos r6 en @r8 con un postincremento de 2
	ldrh r6, [r9]
	orr r6, r6, r5, LSL r10 ; r6 = cuadricula[i][columna] | (#1 << r10)
	strh r6, [r9], #32		; guardamos r6 en @r9 con un postincremento de 32
	add r7, r7, #1 			; i++
	b BUCLE
finBUC

	;Cambio en valores de algunos registros:
	;r6 = @cuadricula[init_i + i][init_j + j]
	;r8 = j
	;r9 = @cuadricula[init_i][init_j]
 

	LDR r9, =vector 		; r9 = @vector
	ldrb r7, [r9, r1] 		; r7 = vector[fila] = init_i
	ldrb r8, [r9, r2] 		; r8 = vector[columna] = init_j
	add r9, r0, r7, LSL #5 	; @cuadricula[init_i][0]
	add r9, r9, r8, LSL #1 	; @cuadricula[init_i][init_j]
	mov r7, #0 				; r7 = i

	; propagamos el candidato en la region



BUC_I
	cmp r7, #3
	beq finBUC_I
	mov r8, #0 				; r8 = j = 0
	add r6, r9, r7, LSL #5 	; r6 = @cuadricula[init_i + i][init_j]
BUC_J
	cmp r8, #3
	beq finBUC_J
	ldrh r4, [r6] 			; r4 = @cuadricula[init_i + i][init_j + j]
	orr r4, r4, r5, LSL r10 ; r4 = cuadricula[init_i + i][init_j + j] | (#1 << r10)
	strh r4, [r6], #2 		; guardamos r4 en @r6 con un postincremento de 2	
	add r8, r8, #1 			; j++
	b BUC_J
finBUC_J
	add r7, r7, #1 			; i++
	b BUC_I
finBUC_I
	ldmia SP!, {r4-r10, PC}
	END

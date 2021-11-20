    AREA Codigo, CODE
    EXPORT candidatos_actualizar_arm_c
	IMPORT candidatos_propagar_c
	PRESERVE8 {TRUE}
	ENTRY

;calcula todas las listas de candidatos (9x9)
;necesario tras borrar o cambiar un valor (listas corrompidas)
;retorna el numero de celdas vacias */

;Init del sudoku en codigo arm invocando a propagar en C
;Recibe la cuadricula como primer parametro
;y devuelve en r0 el número de celdas vacias

;Parámetros:
;r0 = @cuadricula 
;Registros utilizados:
;r4 = @cuadricula
;r5 = celdas_vacias
;r6 = i
;r7 = j
;r8 = @cuadricula[i][j]
;r9 = valor de cuadricula[i][j]
 
;Resultado:
;r0 = celdas_vacías
 

candidatos_actualizar_arm_c
    stmdb SP!, {r4-r9, FP, LR}
    mov r4, r0              ; r4 = @cuadricula
    mov r5, #0              ; r5 = celdas vacias
    mov r6, #0              ; r6 = i = 0

    ; borramos los candidatos de todo el sudoku
Filas_1
    cmp r6, #9
    beq finB1_1
    mov r7, #0              ; r7 = j = 0
Columnas_1    
    cmp r7, #9
    beq finB2_1
    add r8, r4, r6, LSL #5  ; r8 = @base + i * 16 
	add r8, r8, r7, LSL #1  ; r8 = (@base + i * 16) + j * 2
    ldrh r9, [r8]           ; r9 = valor de cuadricula[i][j]
    and r9, r9, #0x0000007F ; r9 = r9 AND #0x0000007F, ponemos todos los bits de candidatos a 0
    strh r9, [r8]           ; actualizamos el valor de la celda
    add r7, r7, #1          ; j++
    b Columnas_1
finB2_1
    add r6, r6, #1          ; i++
    b Filas_1
finB1_1

    ; propagamos los candidatos
    ; contamos celdas vacias

    mov r6, #0              ; i = 0
Filas_2
    cmp r6, #9
    beq finB1_2
    mov r7, #0              ; r7 = j = 0
Columnas_2  
    cmp r7, #9
    beq finB2_2
    add r8, r4, r6, LSL #5  ; r8 = @base + i * 16 
    add r8, r8, r7, LSL #1  ; r8 = (@base + i * 16) + j * 2
    ldrh r9, [r8]           ; r9 = celda[i][j]
    and r9, r9, #0x0000000f ; r9 = valor celda
    cmp r9, #0
    ; si el valor es 0 contamos una celda vacia
    addeq r5, r5, #1        ; celdas_vacias++
	; si no, propagamos el candidato
    ; Pasamos los parámetros para la llamada a candidatos_propagar_c
    movne r0, r4            ; r0 = @base
    movne r1, r6            ; r1 = i
    movne r2, r7            ; r2 = j
    blne candidatos_propagar_c
    add r7, r7, #1          ; j++
    b Columnas_2
finB2_2
    add r6, r6, #1          ; i++
    b Filas_2
finB1_2

    ; devolvemos el numero de celdas vacias
    mov r0, r5              ; r5 = celdas_vacias
    ldmia SP!, {r4-r9, FP, PC}
	END

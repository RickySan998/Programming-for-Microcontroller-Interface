 	.syntax unified
 	.cpu cortex-m3
 	.thumb
 	.align 2
 	.global	distrBF
 	.thumb_func

@ EE2024 Assignment 1, Sem 1, AY 2017/18
@ (c) CK Tham, ECE NUS, 2017
@ Group 7 Wednesday
@ Ricky Sanjaya(A1061326M)
@ Lim Boon Chia(A0161373J)
@ Register Tab
// R4 = row counter
// R5 = column counter
// R6 = address of the first element of the array to be fetched from
// R7 = Value of N
// R8 = Holds offset calculated from desired row and column
// R9 = Holds value fetched from retrieve function (Dj or dij value)
// R10 = current minimum delay
// R11 = holds the next delay value
// R12 = The nth node which has mininum delay

distrBF:

	STMDB R13!,{R4-R12,R14}
	MOV R4,#10
	PUSH {R4}
	POP {R4}
	MOV R4, #0 // initialise row counter R4 as 0
	LDR R7, [R3]

@ Subroutine tsteps
tsteps:
	LDR R10, =0X7FFFFFFF //initial R10 as minimum value
	MOV R11, #0 // initial R11 as next value 0
	MOV R12, #1 // R12 refers to which node has the minimum delay
	MOV R5,#0 // initial column count R5 as 0

@ Subroutine minnode
minnode:
	MOV R6, R1 //move address of first element of array dij to R6
	MOV R11,#0
	PUSH {R10}// save the R10 before modify it in subroutine
	BL RETRIEVE
	POP {R10} //restore R10
	ADD R11, R11, R9

	MOV R6, R2 //move address of first element of array Dj to R6
	PUSH {R10}
	BL RETRIEVE
	POP {R10}
	ADD R11, R11, R9
	CMP R11, R10 //compare R11 to minimum R10
	ITTT MI
	MOVMI R10, R11 //replace R10 with R11 if R11 smaller than R10
	MOVMI R12, R5
	ADDMI R12,R12,#1

	ADD R5, R5, #1 //increment of column by 1
	CMP R5, R7
	BMI minnode // loop subroutine minnode if R5 smaller than R7

	STR R10, [R3]
	ADD R3, R3, #4
	STR R12, [R3]
	ADD R3, R3, #4

	ADD R4, R4, #1 //increment of row by 1
	CMP R4, R0
	BMI tsteps // loop subroutine tsteps if R4 smaller than R0

	POP {R4-R12, R14}
	BX	LR

@ Subroutine RETRIEVE
RETRIEVE:
	MOV R10,#4
	MLA R8, R4, R7, R5
	MUL R8, R8, R10
	LDR R9, [R6,R8]

	BX LR

	NOP
	.end

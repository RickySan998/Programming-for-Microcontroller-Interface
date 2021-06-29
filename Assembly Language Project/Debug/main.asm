   1              		.syntax unified
   2              		.cpu cortex-m3
   3              		.fpu softvfp
   4              		.eabi_attribute 20, 1
   5              		.eabi_attribute 21, 1
   6              		.eabi_attribute 23, 3
   7              		.eabi_attribute 24, 1
   8              		.eabi_attribute 25, 1
   9              		.eabi_attribute 26, 1
  10              		.eabi_attribute 30, 6
  11              		.eabi_attribute 34, 1
  12              		.eabi_attribute 18, 4
  13              		.thumb
  14              		.file	"main.c"
  15              		.text
  16              	.Ltext0:
  17              		.cfi_sections	.debug_frame
  18              		.section	.rodata
  19              		.align	2
  20              	.LC2:
  21 0000 25642025 		.ascii	"%d %d\012\000"
  21      640A00
  22 0007 00       		.align	2
  23              	.LC0:
  24 0008 09000000 		.word	9
  25 000c 05000000 		.word	5
  26 0010 07000000 		.word	7
  27 0014 08000000 		.word	8
  28 0018 00000000 		.word	0
  29 001c 07000000 		.word	7
  30 0020 09000000 		.word	9
  31 0024 06000000 		.word	6
  32 0028 09000000 		.word	9
  33 002c 0A000000 		.word	10
  34 0030 0A000000 		.word	10
  35 0034 08000000 		.word	8
  36 0038 08000000 		.word	8
  37 003c 07000000 		.word	7
  38 0040 02000000 		.word	2
  39 0044 0A000000 		.word	10
  40 0048 08000000 		.word	8
  41 004c 08000000 		.word	8
  42 0050 09000000 		.word	9
  43 0054 00000000 		.word	0
  44              		.align	2
  45              	.LC1:
  46 0058 01000000 		.word	1
  47 005c 17000000 		.word	23
  48 0060 0E000000 		.word	14
  49 0064 0F000000 		.word	15
  50 0068 00000000 		.word	0
  51 006c 12000000 		.word	18
  52 0070 13000000 		.word	19
  53 0074 08000000 		.word	8
  54 0078 0E000000 		.word	14
  55 007c 09000000 		.word	9
  56 0080 13000000 		.word	19
  57 0084 02000000 		.word	2
  58 0088 11000000 		.word	17
  59 008c 10000000 		.word	16
  60 0090 02000000 		.word	2
  61 0094 13000000 		.word	19
  62 0098 16000000 		.word	22
  63 009c 11000000 		.word	17
  64 00a0 09000000 		.word	9
  65 00a4 06000000 		.word	6
  66              		.section	.text.main,"ax",%progbits
  67              		.align	2
  68              		.global	main
  69              		.thumb
  70              		.thumb_func
  72              	main:
  73              	.LFB0:
  74              		.file 1 "../src/main.c"
   1:../src/main.c **** #include "stdio.h"
   2:../src/main.c **** 
   3:../src/main.c **** extern void distrBF(int T_arg, int* dij_arr, int* Dj_arr, int* n_res_arr);
   4:../src/main.c **** 
   5:../src/main.c **** // EE2024 Assignment 1, Sem 1, AY 2017/18
   6:../src/main.c **** // (c) CK Tham, ECE NUS, 2017
   7:../src/main.c **** 
   8:../src/main.c **** #define T 4
   9:../src/main.c **** #define N 5
  10:../src/main.c **** 
  11:../src/main.c **** int main(void)
  12:../src/main.c **** {
  75              		.loc 1 12 0
  76              		.cfi_startproc
  77              		@ args = 0, pretend = 0, frame = 200
  78              		@ frame_needed = 1, uses_anonymous_args = 0
  79 0000 80B5     		push	{r7, lr}
  80              	.LCFI0:
  81              		.cfi_def_cfa_offset 8
  82              		.cfi_offset 14, -4
  83              		.cfi_offset 7, -8
  84 0002 B2B0     		sub	sp, sp, #200
  85              	.LCFI1:
  86              		.cfi_def_cfa_offset 208
  87 0004 00AF     		add	r7, sp, #0
  88              	.LCFI2:
  89              		.cfi_def_cfa_register 7
  13:../src/main.c **** 	// Variable definitions
  14:../src/main.c **** 	int i;
  15:../src/main.c **** 
  16:../src/main.c **** 	// Note: different initialisation list is needed for different T, N
  17:../src/main.c **** 	int dij[T][N]={{9,5,7,8,0},
  90              		.loc 1 17 0
  91 0006 40F20003 		movw	r3, #:lower16:.LC0
  92 000a C0F20003 		movt	r3, #:upper16:.LC0
  93 000e 07F17401 		add	r1, r7, #116
  94 0012 1A46     		mov	r2, r3
  95 0014 4FF05003 		mov	r3, #80
  96 0018 0846     		mov	r0, r1
  97 001a 1146     		mov	r1, r2
  98 001c 1A46     		mov	r2, r3
  99 001e FFF7FEFF 		bl	memcpy
  18:../src/main.c **** 	               {7,9,6,9,10},
  19:../src/main.c **** 	               {10,8,8,7,2},
  20:../src/main.c **** 				   {10,8,8,9,0}};
  21:../src/main.c **** 
  22:../src/main.c **** 	int Dj[T][N]={{1,23,14,15,0},
 100              		.loc 1 22 0
 101 0022 40F20003 		movw	r3, #:lower16:.LC1
 102 0026 C0F20003 		movt	r3, #:upper16:.LC1
 103 002a 07F12401 		add	r1, r7, #36
 104 002e 1A46     		mov	r2, r3
 105 0030 4FF05003 		mov	r3, #80
 106 0034 0846     		mov	r0, r1
 107 0036 1146     		mov	r1, r2
 108 0038 1A46     		mov	r2, r3
 109 003a FFF7FEFF 		bl	memcpy
  23:../src/main.c **** 	              {18,19,8,14,9},
  24:../src/main.c **** 	              {19,2,17,16,2},
  25:../src/main.c **** 				  {19,22,17,9,6}};
  26:../src/main.c **** 
  27:../src/main.c **** 	int n_res[T][2]={{N,0},
 110              		.loc 1 27 0
 111 003e 4FF00503 		mov	r3, #5
 112 0042 7B60     		str	r3, [r7, #4]
 113 0044 4FF00003 		mov	r3, #0
 114 0048 BB60     		str	r3, [r7, #8]
 115 004a 4FF00003 		mov	r3, #0
 116 004e FB60     		str	r3, [r7, #12]
 117 0050 4FF00003 		mov	r3, #0
 118 0054 3B61     		str	r3, [r7, #16]
 119 0056 4FF00003 		mov	r3, #0
 120 005a 7B61     		str	r3, [r7, #20]
 121 005c 4FF00003 		mov	r3, #0
 122 0060 BB61     		str	r3, [r7, #24]
 123 0062 4FF00003 		mov	r3, #0
 124 0066 FB61     		str	r3, [r7, #28]
 125 0068 4FF00003 		mov	r3, #0
 126 006c 3B62     		str	r3, [r7, #32]
  28:../src/main.c **** 	                 {0,0},
  29:../src/main.c **** 	                 {0,0},
  30:../src/main.c **** 					 {0,0}};
  31:../src/main.c **** 
  32:../src/main.c **** 	// Call assembly language function
  33:../src/main.c **** 	distrBF(T,(int*)dij,(int*)Dj,(int*)n_res);
 127              		.loc 1 33 0
 128 006e 07F17401 		add	r1, r7, #116
 129 0072 07F12402 		add	r2, r7, #36
 130 0076 07F10403 		add	r3, r7, #4
 131 007a 4FF00400 		mov	r0, #4
 132 007e FFF7FEFF 		bl	distrBF
  34:../src/main.c **** 
  35:../src/main.c **** 	// Print out results on console
  36:../src/main.c **** 	for ( i=0; i<T ; i++ )
 133              		.loc 1 36 0
 134 0082 4FF00003 		mov	r3, #0
 135 0086 C7F8C430 		str	r3, [r7, #196]
 136 008a 1EE0     		b	.L2
 137              	.L3:
  37:../src/main.c **** 		printf("%d %d\n", n_res[i][0], n_res[i][1]);
 138              		.loc 1 37 0 discriminator 2
 139 008c 40F20003 		movw	r3, #:lower16:.LC2
 140 0090 C0F20003 		movt	r3, #:upper16:.LC2
 141 0094 D7F8C420 		ldr	r2, [r7, #196]
 142 0098 4FEAC202 		lsl	r2, r2, #3
 143 009c 07F1C800 		add	r0, r7, #200
 144 00a0 8218     		adds	r2, r0, r2
 145 00a2 52F8C41C 		ldr	r1, [r2, #-196]
 146 00a6 D7F8C420 		ldr	r2, [r7, #196]
 147 00aa 4FEAC202 		lsl	r2, r2, #3
 148 00ae 07F1C800 		add	r0, r7, #200
 149 00b2 8218     		adds	r2, r0, r2
 150 00b4 52F8C02C 		ldr	r2, [r2, #-192]
 151 00b8 1846     		mov	r0, r3
 152 00ba FFF7FEFF 		bl	printf
  36:../src/main.c **** 	for ( i=0; i<T ; i++ )
 153              		.loc 1 36 0 discriminator 2
 154 00be D7F8C430 		ldr	r3, [r7, #196]
 155 00c2 03F10103 		add	r3, r3, #1
 156 00c6 C7F8C430 		str	r3, [r7, #196]
 157              	.L2:
  36:../src/main.c **** 	for ( i=0; i<T ; i++ )
 158              		.loc 1 36 0 is_stmt 0 discriminator 1
 159 00ca D7F8C430 		ldr	r3, [r7, #196]
 160 00ce 032B     		cmp	r3, #3
 161 00d0 DCDD     		ble	.L3
 162              	.L4:
  38:../src/main.c **** 
  39:../src/main.c **** 	// Enter an infinite loop, just incrementing a counter
  40:../src/main.c **** 	volatile static int loop = 0;
  41:../src/main.c **** 	while (1) {
  42:../src/main.c **** 		loop++;
 163              		.loc 1 42 0 is_stmt 1 discriminator 1
 164 00d2 40F20003 		movw	r3, #:lower16:loop.3821
 165 00d6 C0F20003 		movt	r3, #:upper16:loop.3821
 166 00da 1B68     		ldr	r3, [r3, #0]
 167 00dc 03F10102 		add	r2, r3, #1
 168 00e0 40F20003 		movw	r3, #:lower16:loop.3821
 169 00e4 C0F20003 		movt	r3, #:upper16:loop.3821
 170 00e8 1A60     		str	r2, [r3, #0]
  43:../src/main.c **** 	}
 171              		.loc 1 43 0 discriminator 1
 172 00ea F2E7     		b	.L4
 173              		.cfi_endproc
 174              	.LFE0:
 176              		.bss
 177              		.align	2
 178              	loop.3821:
 179 0000 00000000 		.space	4
 180              		.text
 181              	.Letext0:
DEFINED SYMBOLS
                            *ABS*:00000000 main.c
C:\Users\Ricky\AppData\Local\Temp\cc9Mkn8i.s:19     .rodata:00000000 $d
C:\Users\Ricky\AppData\Local\Temp\cc9Mkn8i.s:20     .rodata:00000000 .LC2
C:\Users\Ricky\AppData\Local\Temp\cc9Mkn8i.s:23     .rodata:00000008 .LC0
C:\Users\Ricky\AppData\Local\Temp\cc9Mkn8i.s:45     .rodata:00000058 .LC1
C:\Users\Ricky\AppData\Local\Temp\cc9Mkn8i.s:67     .text.main:00000000 $t
C:\Users\Ricky\AppData\Local\Temp\cc9Mkn8i.s:72     .text.main:00000000 main
C:\Users\Ricky\AppData\Local\Temp\cc9Mkn8i.s:178    .bss:00000000 loop.3821
C:\Users\Ricky\AppData\Local\Temp\cc9Mkn8i.s:177    .bss:00000000 $d
                     .debug_frame:00000010 $d

UNDEFINED SYMBOLS
memcpy
distrBF
printf

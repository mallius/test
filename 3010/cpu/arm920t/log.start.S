
start.o:     file format elf32-littlearm

Disassembly of section .text:

00000000 <_start>:
   0:	ea000014 	b	58 <start_code>
   4:	e59ff014 	ldr	pc, [pc, #20]	; 20 <_undefined_instruction>
   8:	e59ff014 	ldr	pc, [pc, #20]	; 24 <_software_interrupt>
   c:	e59ff014 	ldr	pc, [pc, #20]	; 28 <_prefetch_abort>
  10:	e59ff014 	ldr	pc, [pc, #20]	; 2c <_data_abort>
  14:	e59ff014 	ldr	pc, [pc, #20]	; 30 <_not_used>
  18:	e59ff014 	ldr	pc, [pc, #20]	; 34 <_irq>
  1c:	e59ff014 	ldr	pc, [pc, #20]	; 38 <_fiq>

00000020 <_undefined_instruction>:
  20:	00000240 	.word	0x00000240

00000024 <_software_interrupt>:
  24:	000002a0 	.word	0x000002a0

00000028 <_prefetch_abort>:
  28:	00000300 	.word	0x00000300

0000002c <_data_abort>:
  2c:	00000360 	.word	0x00000360

00000030 <_not_used>:
  30:	000003c0 	.word	0x000003c0

00000034 <_irq>:
  34:	00000420 	.word	0x00000420

00000038 <_fiq>:
  38:	00000440 	.word	0x00000440
  3c:	deadbeef 	.word	0xdeadbeef

00000040 <_TEXT_BASE>:
  40:	33f80000 	.word	0x33f80000

00000044 <_armboot_start>:
  44:	00000000 	.word	0x00000000

00000048 <_bss_start>:
  48:	00000000 	.word	0x00000000

0000004c <_bss_end>:
  4c:	00000000 	.word	0x00000000

00000050 <IRQ_STACK_START>:
  50:	0badc0de 	.word	0x0badc0de

00000054 <FIQ_STACK_START>:
  54:	0badc0de 	.word	0x0badc0de

00000058 <start_code>:
  58:	e10f0000 	mrs	r0, CPSR
  5c:	e3c0001f 	bic	r0, r0, #31	; 0x1f
  60:	e38000d3 	orr	r0, r0, #211	; 0xd3
  64:	e129f000 	msr	CPSR_fc, r0
  68:	eb000104 	bl	480 <LED_on>
  6c:	e3a00453 	mov	r0, #1392508928	; 0x53000000
  70:	e3a01000 	mov	r1, #0	; 0x0
  74:	e5801000 	str	r1, [r0]
  78:	e3e01000 	mvn	r1, #0	; 0x0
  7c:	e59f0440 	ldr	r0, [pc, #1088]	; 4c4 <led_loop+0x1c>
  80:	e5801000 	str	r1, [r0]
  84:	e59f143c 	ldr	r1, [pc, #1084]	; 4c8 <led_loop+0x20>
  88:	e59f043c 	ldr	r0, [pc, #1084]	; 4cc <led_loop+0x24>
  8c:	e5801000 	str	r1, [r0]
  90:	e59f0438 	ldr	r0, [pc, #1080]	; 4d0 <led_loop+0x28>
  94:	e3a01005 	mov	r1, #5	; 0x5
  98:	e5801000 	str	r1, [r0]
  9c:	ee111f10 	mrc	15, 0, r1, cr1, cr0, {0}
  a0:	e3811103 	orr	r1, r1, #-1073741824	; 0xc0000000
  a4:	ee011f10 	mcr	15, 0, r1, cr1, cr0, {0}
  a8:	e3a01313 	mov	r1, #1275068416	; 0x4c000000
  ac:	e3a02a7f 	mov	r2, #520192	; 0x7f000
  b0:	e2822021 	add	r2, r2, #33	; 0x21
  b4:	e5812004 	str	r2, [r1, #4]
  b8:	eb000052 	bl	208 <cpu_init_crit>
  bc:	e24f00c4 	sub	r0, pc, #196	; 0xc4
  c0:	e51f1088 	ldr	r1, [pc, #-136]	; 40 <_TEXT_BASE>
  c4:	e1500001 	cmp	r0, r1
  c8:	0a00003f 	beq	1cc <stack_setup>
  cc:	e3a011f1 	mov	r1, #1073741884	; 0x4000003c
  d0:	e3a00000 	mov	r0, #0	; 0x0
  d4:	e5810000 	str	r0, [r1]
  d8:	e3a0103c 	mov	r1, #60	; 0x3c
  dc:	e5910000 	ldr	r0, [r1]
  e0:	e3500000 	cmp	r0, #0	; 0x0
  e4:	1a00002b 	bne	198 <relocate>
  e8:	e59f03e4 	ldr	r0, [pc, #996]	; 4d4 <led_loop+0x2c>
  ec:	e3a011f1 	mov	r1, #1073741884	; 0x4000003c
  f0:	e5810000 	str	r0, [r1]
  f4:	e3a0144e 	mov	r1, #1308622848	; 0x4e000000
  f8:	e59f23d8 	ldr	r2, [pc, #984]	; 4d8 <led_loop+0x30>
  fc:	e5812000 	str	r2, [r1]
 100:	e3a02011 	mov	r2, #17	; 0x11
 104:	e5812004 	str	r2, [r1, #4]
 108:	e5912004 	ldr	r2, [r1, #4]
 10c:	e3a02006 	mov	r2, #6	; 0x6
 110:	e5812020 	str	r2, [r1, #32]
 114:	e5912020 	ldr	r2, [r1, #32]
 118:	e3a020ff 	mov	r2, #255	; 0xff
 11c:	e5c12008 	strb	r2, [r1, #8]
 120:	e3a03000 	mov	r3, #0	; 0x0

00000124 <nand1>:
 124:	e2833001 	add	r3, r3, #1	; 0x1
 128:	e353000a 	cmp	r3, #10	; 0xa
 12c:	1afffffc 	bne	124 <nand1>

00000130 <nand2>:
 130:	e5912020 	ldr	r2, [r1, #32]
 134:	e3120004 	tst	r2, #4	; 0x4
 138:	0afffffc 	beq	130 <nand2>
 13c:	e5912004 	ldr	r2, [r1, #4]
 140:	e3822002 	orr	r2, r2, #2	; 0x2
 144:	e5812004 	str	r2, [r1, #4]
 148:	e59fd0b4 	ldr	sp, [pc, #180]	; 204 <DW_STACK_START>
 14c:	e3a0b000 	mov	fp, #0	; 0x0
 150:	e59f0384 	ldr	r0, [pc, #900]	; 4dc <led_loop+0x34>
 154:	e3a01000 	mov	r1, #0	; 0x0
 158:	e3a02806 	mov	r2, #393216	; 0x60000
 15c:	ebfffffe 	bl	0 <nand_read_ll>
 160:	e3100000 	tst	r0, #0	; 0x0
 164:	0a000000 	beq	16c <ok_nand_read>

00000168 <bad_nand_read>:
 168:	eafffffe 	b	168 <bad_nand_read>

0000016c <ok_nand_read>:
 16c:	e3a00000 	mov	r0, #0	; 0x0
 170:	e59f1364 	ldr	r1, [pc, #868]	; 4dc <led_loop+0x34>
 174:	e3a02b01 	mov	r2, #1024	; 0x400

00000178 <go_next>:
 178:	e4903004 	ldr	r3, [r0], #4
 17c:	e4914004 	ldr	r4, [r1], #4
 180:	e1330004 	teq	r3, r4
 184:	1a000002 	bne	194 <loop3>
 188:	e2522004 	subs	r2, r2, #4	; 0x4
 18c:	0a00000e 	beq	1cc <stack_setup>
 190:	1afffff8 	bne	178 <go_next>

00000194 <loop3>:
 194:	eafffffe 	b	194 <loop3>

00000198 <relocate>:
 198:	e59f1334 	ldr	r1, [pc, #820]	; 4d4 <led_loop+0x2c>
 19c:	e1500001 	cmp	r0, r1
 1a0:	1afffffb 	bne	194 <loop3>
 1a4:	e24f0f6b 	sub	r0, pc, #428	; 0x1ac
 1a8:	e51f1170 	ldr	r1, [pc, #-368]	; 40 <_TEXT_BASE>
 1ac:	e51f2170 	ldr	r2, [pc, #-368]	; 44 <_armboot_start>
 1b0:	e51f3170 	ldr	r3, [pc, #-368]	; 48 <_bss_start>
 1b4:	e0432002 	sub	r2, r3, r2
 1b8:	e0802002 	add	r2, r0, r2

000001bc <copy_loop>:
 1bc:	e8b007f8 	ldm	r0!, {r3, r4, r5, r6, r7, r8, r9, sl}
 1c0:	e8a107f8 	stmia	r1!, {r3, r4, r5, r6, r7, r8, r9, sl}
 1c4:	e1500002 	cmp	r0, r2
 1c8:	dafffffb 	ble	1bc <copy_loop>

000001cc <stack_setup>:
 1cc:	e51f0194 	ldr	r0, [pc, #-404]	; 40 <_TEXT_BASE>
 1d0:	e2400701 	sub	r0, r0, #262144	; 0x40000
 1d4:	e2400080 	sub	r0, r0, #128	; 0x80
 1d8:	e2400a02 	sub	r0, r0, #8192	; 0x2000
 1dc:	e240d00c 	sub	sp, r0, #12	; 0xc

000001e0 <clear_bss>:
 1e0:	e51f01a0 	ldr	r0, [pc, #-416]	; 48 <_bss_start>
 1e4:	e51f11a0 	ldr	r1, [pc, #-416]	; 4c <_bss_end>
 1e8:	e3a02000 	mov	r2, #0	; 0x0

000001ec <clbss_l>:
 1ec:	e5802000 	str	r2, [r0]
 1f0:	e2800004 	add	r0, r0, #4	; 0x4
 1f4:	e1500001 	cmp	r0, r1
 1f8:	dafffffb 	ble	1ec <clbss_l>
 1fc:	e51ff004 	ldr	pc, [pc, #-4]	; 200 <_start_armboot>

00000200 <_start_armboot>:
 200:	00000000 	.word	0x00000000

00000204 <DW_STACK_START>:
 204:	33f0fffc 	.word	0x33f0fffc

00000208 <cpu_init_crit>:
 208:	e3a00000 	mov	r0, #0	; 0x0
 20c:	ee070f17 	mcr	15, 0, r0, cr7, cr7, {0}
 210:	ee080f17 	mcr	15, 0, r0, cr8, cr7, {0}
 214:	ee110f10 	mrc	15, 0, r0, cr1, cr0, {0}
 218:	e3c00c23 	bic	r0, r0, #8960	; 0x2300
 21c:	e3c00087 	bic	r0, r0, #135	; 0x87
 220:	e3800002 	orr	r0, r0, #2	; 0x2
 224:	e3800a01 	orr	r0, r0, #4096	; 0x1000
 228:	ee010f10 	mcr	15, 0, r0, cr1, cr0, {0}
 22c:	e1a0c00e 	mov	ip, lr
 230:	ebfffffe 	bl	0 <lowlevel_init>
 234:	e1a0e00c 	mov	lr, ip
 238:	e1a0f00e 	mov	pc, lr
 23c:	e1a00000 	nop			(mov r0,r0)

00000240 <undefined_instruction>:
 240:	e51fd204 	ldr	sp, [pc, #-516]	; 44 <_armboot_start>
 244:	e24dd802 	sub	sp, sp, #131072	; 0x20000
 248:	e24dd701 	sub	sp, sp, #262144	; 0x40000
 24c:	e24dd088 	sub	sp, sp, #136	; 0x88
 250:	e58de000 	str	lr, [sp]
 254:	e14fe000 	mrs	lr, SPSR
 258:	e58de004 	str	lr, [sp, #4]
 25c:	e3a0d013 	mov	sp, #19	; 0x13
 260:	e169f00d 	msr	SPSR_fc, sp
 264:	e1a0e00f 	mov	lr, pc
 268:	e1b0f00e 	movs	pc, lr
 26c:	e24dd048 	sub	sp, sp, #72	; 0x48
 270:	e88d1fff 	stm	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip}
 274:	e51f2238 	ldr	r2, [pc, #-568]	; 44 <_armboot_start>
 278:	e2422802 	sub	r2, r2, #131072	; 0x20000
 27c:	e2422701 	sub	r2, r2, #262144	; 0x40000
 280:	e2422088 	sub	r2, r2, #136	; 0x88
 284:	e892000c 	ldm	r2, {r2, r3}
 288:	e28d0048 	add	r0, sp, #72	; 0x48
 28c:	e28d5034 	add	r5, sp, #52	; 0x34
 290:	e1a0100e 	mov	r1, lr
 294:	e885000f 	stm	r5, {r0, r1, r2, r3}
 298:	e1a0000d 	mov	r0, sp
 29c:	ebfffffe 	bl	0 <do_undefined_instruction>

000002a0 <software_interrupt>:
 2a0:	e51fd264 	ldr	sp, [pc, #-612]	; 44 <_armboot_start>
 2a4:	e24dd802 	sub	sp, sp, #131072	; 0x20000
 2a8:	e24dd701 	sub	sp, sp, #262144	; 0x40000
 2ac:	e24dd088 	sub	sp, sp, #136	; 0x88
 2b0:	e58de000 	str	lr, [sp]
 2b4:	e14fe000 	mrs	lr, SPSR
 2b8:	e58de004 	str	lr, [sp, #4]
 2bc:	e3a0d013 	mov	sp, #19	; 0x13
 2c0:	e169f00d 	msr	SPSR_fc, sp
 2c4:	e1a0e00f 	mov	lr, pc
 2c8:	e1b0f00e 	movs	pc, lr
 2cc:	e24dd048 	sub	sp, sp, #72	; 0x48
 2d0:	e88d1fff 	stm	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip}
 2d4:	e51f2298 	ldr	r2, [pc, #-664]	; 44 <_armboot_start>
 2d8:	e2422802 	sub	r2, r2, #131072	; 0x20000
 2dc:	e2422701 	sub	r2, r2, #262144	; 0x40000
 2e0:	e2422088 	sub	r2, r2, #136	; 0x88
 2e4:	e892000c 	ldm	r2, {r2, r3}
 2e8:	e28d0048 	add	r0, sp, #72	; 0x48
 2ec:	e28d5034 	add	r5, sp, #52	; 0x34
 2f0:	e1a0100e 	mov	r1, lr
 2f4:	e885000f 	stm	r5, {r0, r1, r2, r3}
 2f8:	e1a0000d 	mov	r0, sp
 2fc:	ebfffffe 	bl	0 <do_software_interrupt>

00000300 <prefetch_abort>:
 300:	e51fd2c4 	ldr	sp, [pc, #-708]	; 44 <_armboot_start>
 304:	e24dd802 	sub	sp, sp, #131072	; 0x20000
 308:	e24dd701 	sub	sp, sp, #262144	; 0x40000
 30c:	e24dd088 	sub	sp, sp, #136	; 0x88
 310:	e58de000 	str	lr, [sp]
 314:	e14fe000 	mrs	lr, SPSR
 318:	e58de004 	str	lr, [sp, #4]
 31c:	e3a0d013 	mov	sp, #19	; 0x13
 320:	e169f00d 	msr	SPSR_fc, sp
 324:	e1a0e00f 	mov	lr, pc
 328:	e1b0f00e 	movs	pc, lr
 32c:	e24dd048 	sub	sp, sp, #72	; 0x48
 330:	e88d1fff 	stm	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip}
 334:	e51f22f8 	ldr	r2, [pc, #-760]	; 44 <_armboot_start>
 338:	e2422802 	sub	r2, r2, #131072	; 0x20000
 33c:	e2422701 	sub	r2, r2, #262144	; 0x40000
 340:	e2422088 	sub	r2, r2, #136	; 0x88
 344:	e892000c 	ldm	r2, {r2, r3}
 348:	e28d0048 	add	r0, sp, #72	; 0x48
 34c:	e28d5034 	add	r5, sp, #52	; 0x34
 350:	e1a0100e 	mov	r1, lr
 354:	e885000f 	stm	r5, {r0, r1, r2, r3}
 358:	e1a0000d 	mov	r0, sp
 35c:	ebfffffe 	bl	0 <do_prefetch_abort>

00000360 <data_abort>:
 360:	e51fd324 	ldr	sp, [pc, #-804]	; 44 <_armboot_start>
 364:	e24dd802 	sub	sp, sp, #131072	; 0x20000
 368:	e24dd701 	sub	sp, sp, #262144	; 0x40000
 36c:	e24dd088 	sub	sp, sp, #136	; 0x88
 370:	e58de000 	str	lr, [sp]
 374:	e14fe000 	mrs	lr, SPSR
 378:	e58de004 	str	lr, [sp, #4]
 37c:	e3a0d013 	mov	sp, #19	; 0x13
 380:	e169f00d 	msr	SPSR_fc, sp
 384:	e1a0e00f 	mov	lr, pc
 388:	e1b0f00e 	movs	pc, lr
 38c:	e24dd048 	sub	sp, sp, #72	; 0x48
 390:	e88d1fff 	stm	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip}
 394:	e51f2358 	ldr	r2, [pc, #-856]	; 44 <_armboot_start>
 398:	e2422802 	sub	r2, r2, #131072	; 0x20000
 39c:	e2422701 	sub	r2, r2, #262144	; 0x40000
 3a0:	e2422088 	sub	r2, r2, #136	; 0x88
 3a4:	e892000c 	ldm	r2, {r2, r3}
 3a8:	e28d0048 	add	r0, sp, #72	; 0x48
 3ac:	e28d5034 	add	r5, sp, #52	; 0x34
 3b0:	e1a0100e 	mov	r1, lr
 3b4:	e885000f 	stm	r5, {r0, r1, r2, r3}
 3b8:	e1a0000d 	mov	r0, sp
 3bc:	ebfffffe 	bl	0 <do_data_abort>

000003c0 <not_used>:
 3c0:	e51fd384 	ldr	sp, [pc, #-900]	; 44 <_armboot_start>
 3c4:	e24dd802 	sub	sp, sp, #131072	; 0x20000
 3c8:	e24dd701 	sub	sp, sp, #262144	; 0x40000
 3cc:	e24dd088 	sub	sp, sp, #136	; 0x88
 3d0:	e58de000 	str	lr, [sp]
 3d4:	e14fe000 	mrs	lr, SPSR
 3d8:	e58de004 	str	lr, [sp, #4]
 3dc:	e3a0d013 	mov	sp, #19	; 0x13
 3e0:	e169f00d 	msr	SPSR_fc, sp
 3e4:	e1a0e00f 	mov	lr, pc
 3e8:	e1b0f00e 	movs	pc, lr
 3ec:	e24dd048 	sub	sp, sp, #72	; 0x48
 3f0:	e88d1fff 	stm	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip}
 3f4:	e51f23b8 	ldr	r2, [pc, #-952]	; 44 <_armboot_start>
 3f8:	e2422802 	sub	r2, r2, #131072	; 0x20000
 3fc:	e2422701 	sub	r2, r2, #262144	; 0x40000
 400:	e2422088 	sub	r2, r2, #136	; 0x88
 404:	e892000c 	ldm	r2, {r2, r3}
 408:	e28d0048 	add	r0, sp, #72	; 0x48
 40c:	e28d5034 	add	r5, sp, #52	; 0x34
 410:	e1a0100e 	mov	r1, lr
 414:	e885000f 	stm	r5, {r0, r1, r2, r3}
 418:	e1a0000d 	mov	r0, sp
 41c:	ebfffffe 	bl	0 <do_not_used>

00000420 <irq>:
 420:	e24ee004 	sub	lr, lr, #4	; 0x4
 424:	e51fd3dc 	ldr	sp, [pc, #-988]	; 50 <IRQ_STACK_START>
 428:	e92d5fff 	push	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
 42c:	e59fe0ac 	ldr	lr, [pc, #172]	; 4e0 <led_loop+0x38>
 430:	e59ff0ac 	ldr	pc, [pc, #172]	; 4e4 <led_loop+0x3c>

00000434 <int_return>:
 434:	e8fd9fff 	ldm	sp!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, pc}^
 438:	e1a00000 	nop			(mov r0,r0)
 43c:	e1a00000 	nop			(mov r0,r0)

00000440 <fiq>:
 440:	e51fd3f4 	ldr	sp, [pc, #-1012]	; 54 <FIQ_STACK_START>
 444:	e24dd048 	sub	sp, sp, #72	; 0x48
 448:	e88d1fff 	stm	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip}
 44c:	e28d703c 	add	r7, sp, #60	; 0x3c
 450:	e9476000 	stmdb	r7, {sp, lr}^
 454:	e587e000 	str	lr, [r7]
 458:	e14f6000 	mrs	r6, SPSR
 45c:	e5876004 	str	r6, [r7, #4]
 460:	e5870008 	str	r0, [r7, #8]
 464:	e1a0000d 	mov	r0, sp
 468:	ebfffffe 	bl	0 <do_fiq>
 46c:	e8dd7fff 	ldm	sp, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, sl, fp, ip, sp, lr}^
 470:	e1a00000 	nop			(mov r0,r0)
 474:	e59de03c 	ldr	lr, [sp, #60]
 478:	e28dd048 	add	sp, sp, #72	; 0x48
 47c:	e25ef004 	subs	pc, lr, #4	; 0x4

00000480 <LED_on>:
 480:	e59f0060 	ldr	r0, [pc, #96]	; 4e8 <led_loop+0x40>
 484:	e59f1060 	ldr	r1, [pc, #96]	; 4ec <led_loop+0x44>
 488:	e5801000 	str	r1, [r0]
 48c:	e59f005c 	ldr	r0, [pc, #92]	; 4f0 <led_loop+0x48>
 490:	e3a010ff 	mov	r1, #255	; 0xff
 494:	e5801000 	str	r1, [r0]
 498:	e59f0054 	ldr	r0, [pc, #84]	; 4f4 <led_loop+0x4c>
 49c:	e3a01011 	mov	r1, #17	; 0x11
 4a0:	e5801000 	str	r1, [r0]
 4a4:	e3a02a01 	mov	r2, #4096	; 0x1000

000004a8 <led_loop>:
 4a8:	e2422001 	sub	r2, r2, #1	; 0x1
 4ac:	e3520000 	cmp	r2, #0	; 0x0
 4b0:	1afffffc 	bne	4a8 <led_loop>
 4b4:	e59f0038 	ldr	r0, [pc, #56]	; 4f4 <led_loop+0x4c>
 4b8:	e3a01e1e 	mov	r1, #480	; 0x1e0
 4bc:	e5801000 	str	r1, [r0]
 4c0:	e1a0f00e 	mov	pc, lr
 4c4:	4a000008 	.word	0x4a000008
 4c8:	00007fff 	.word	0x00007fff
 4cc:	4a00001c 	.word	0x4a00001c
 4d0:	4c000014 	.word	0x4c000014
 4d4:	deadbeef 	.word	0xdeadbeef
 4d8:	00007770 	.word	0x00007770
 4dc:	33f80000 	.word	0x33f80000
 4e0:	00000434 	.word	0x00000434
 4e4:	00000000 	.word	0x00000000
 4e8:	56000010 	.word	0x56000010
 4ec:	00295551 	.word	0x00295551
 4f0:	56000018 	.word	0x56000018
 4f4:	56000014 	.word	0x56000014
 4f8:	e1a00000 	.word	0xe1a00000
 4fc:	e1a00000 	.word	0xe1a00000

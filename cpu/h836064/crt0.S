; Start up routine for h8s2633 on DSS board
;
	.h8300hn
	.section .text
	.global	_start
_start:
	mov.w	#_stack,sp
	mov.l	#_text_end,er0
	mov.l	#_data_start,er3
	mov.l	#_data_end,er1
	cmp.l	er1,er3
	bhs	.L8
.L6:
	mov.b	@er0+,r1l
	mov.b	r1l,@er3
	adds	#1,er3
	mov.l	#_data_end,er1
	cmp.l	er1,er3
	blo	.L6
.L8:
	mov.l	#_bss_start,er3
	mov.l	#_bss_end,er0
	cmp.l	er0,er3
	bhs	.L9
	sub.b	r0l,r0l
.L7:
	mov.b	r0l,@er3
	adds	#1,er3
	mov.l	#_bss_end,er1
	cmp.l	er1,er3
	blo	.L7
.L9:
	jsr	@_lowlevelinit
	jsr	@_os_main
lexec_end:
	bra	lexec_end



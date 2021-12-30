;********************************************************
;*							*
;*	SCREEN.INC	Include File for SCREEN.MAC	*
;*	Genie IIs  CP/M 2.2XV1 Version 1.0  31.07.86	*
;*	Copyright (c) Klaus K{mpf Softwareentwicklung	*
;*					1984,1985,1986	*
;*	Modification History				*
;*	0.0  18.05.86  Original Version			*
;*	1.0  31.07.86  Added this header		*
;*							*
;********************************************************
	.printl Including SCREEN.INC

ctrl_table:
	defw bell		;7
	defw back		;8
	defw tab		;9
	defw lf			;10
	defw curup		;11
	defw currgt		;12
	defw crret		;13
;
	defw curdo		;22
	defw null		;23
	defw null		;24
	defw null		;25
	defw clsspc		;26
	defw escape		;27
	defw null		;28
	defw null		;29
	defw chome		;30
	defw newlin		;31

null:   xor  a
	ret
;
; ESC _ jumptable
; (for ESC _ 2 .. ESC _ 5)
;
parjmp:
	defw german
	defw ascii
	defw fknorm
	defw fkspcl

esc_table:
	defb ')'		;Enable Alternate
	defb '('		;Disable Alternate
	defb 'v'		;Enable vertical Wrap
	defb 'w'		;Disable vertical Wrap
	defb '*'		;Clear all to Null (Space)
	defb '?'		;Transmit Cursor Position
	defb 'M'		;Transmit Terminal ID
	defb 'E'		;Insert Line
	defb 'R'		;Delete Line
	defb 'T'		;Clear to EOL to space
	defb 'Y'		;Clear to EOP to space
	defb '='		;Direct Cursor posit.
	defb '.'		;Define Cursor attribute
	defb 'G'		;Define Video attribute
	defb '_'		;Set pagewidth,charset

; Type 0 : Needs Cursor (jmp comes from escap1)
; Type 1 : Doesn't need Cursor (jmp comes from escap1)
; Type 2 : Address is continuation Address
;	  (jmp comes from crt)

jump_table:
	defw setpar
	defb 2
	defw vidatr
	defb 2
	defw curatr
	defb 2
	defw curadr
	defb 2

	defw ceopsp
	defb 0
	defw ceolsp
	defb 0
	defw dellin
	defb 0
	defw inslin
	defb 0
	defw trnsid
	defb 1
	defw curpos
	defb 0
	defw clsspc
	defb 0
	defw wrpoff
	defb 1
	defw wrpon
	defb 1
	defw altoff
	defb 1
	defw alton
	defb 1
;
charset_length	equ	96*8			;=768+64 = 6 1/2 records
swapset_length	equ	8*8
@charset:	defs	charset_length,0
@swap_set:	defs	swapset_length,0

;********************************************************
;*							*
;*	SCREEN.INC  Include file fro SCREEN.MAC		*
;*	Genie III  CP/M 2.2XV3  Version 1.0  31.07.86	*
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
; (for ESC _ 0 .. ESC _ 9)
;
parjmp:
	defw staty
	defw statn
	defw null
	defw null
	defw fknorm
	defw fkspcl
	defw clkon
	defw clkoff

esc_table:
	defb ')'		;Enable Alternate
	defb '('		;Disable Alternate
	defb 'U'		;Enable Monitor
	defb 'X'		;Disable Monitor
	defb 'q'		;Enable Insertmode
	defb 'r'		;Disable Insertmode
	defb 'v'		;Enable vertical Wrap
	defb 'w'		;Disable vertical Wrap
	defb '*'		;Clear all to Null
	defb '+'		;Clear all to Space
	defb '?'		;Transmit Cursor Position
	defb 'M'		;Transmit Terminal ID
	defb 'P'		;Print Screen
	defb 'N'		;Enable Page Edit
	defb 'O'		;Enable Line Edit
	defb 'Q'		;Insert Character
	defb 'W'		;Delete Character
	defb 'E'		;Insert Line
	defb 'R'		;Delete Line
	defb 'T'		;Clear to EOL to space
	defb 't'		;Clear to EOL to null
	defb 'Y'		;Clear to EOP to space
	defb 'y'		;Clear to EOP to null
	defb 'g'		;Display User Line
	defb 'h'		;Display Status Line
	defb '='		;Direct Cursor posit.
	defb '.'		;Define Cursor attribute
	defb 'G'		;Define Video attribute
	defb 'e'		;Define substitute character
	defb 'Z'		;Send 25th Line
	defb 'f'		;Load Userline
	defb '_'		;Set pagewidth,charset
	defb '6'		;Send line

; Type 0 : Needs Cursor (jmp comes from escap1)
; Type 1 : Doesn't need Cursor (jmp comes from escap1)
; Type 2 : Adress is continuation Adress
;	  (jmp comes from crt)

jump_table:
	defw sndlin
	defb 0
	defw setpar
	defb 2
	defw lduser
	defb 2
	defw send25
	defb 2
	defw subchr
	defb 2
	defw vidatr
	defb 2
	defw curatr
	defb 2
	defw curadr
	defb 2

	defw dissta
	defb 1
	defw disusr
	defb 1
	defw ceopnl
	defb 0
	defw ceopsp
	defb 0
	defw ceolnl
	defb 0
	defw ceolsp
	defb 0
	defw dellin
	defb 0
	defw inslin
	defb 0
	defw delchr
	defb 0
	defw inschr
	defb 0
	defw edline
	defb 1
	defw edpage
	defb 1
	defw print
	defb 1
	defw trnsid
	defb 1
	defw curpos
	defb 0
	defw clsspc
	defb 0
	defw clsnul
	defb 0
	defw wrpoff
	defb 1
	defw wrpon
	defb 1
	defw insoff
	defb 1
	defw inson
	defb 1
	defw monoff
	defb 1
	defw monon
	defb 1
	defw altoff
	defb 1
	defw alton
	defb 1

catrtb:
	defb 2bh
	defb 60h
	defb 00h
	defb 6bh
	defb 0bh

scrtop	equ	a$vid_start			;first unprot. pos.
scrlst	equ	a$vid_start+23*80		;start of last line
scrbot	equ	a$vid_start+24*80-1		;last unprot. pos.

; File CPMHEAD.INC
; Include File for CP/M 2.2 Operating System Calls
;
;
	.printl Including Header File for CP/M Operations
;
;
?boot	equ	0
@iobyte	equ	3
@drive	equ	4
?bdos	equ	5
@fcb	equ	05ch
@buffer	equ	080h
;				; BIOS-Calls
conin	equ	1
conout	equ	2
reader	equ	3
punch	equ	4
list	equ	5
drctio	equ	6
getio	equ	7
setio	equ	8
prtstr	equ	9
getbuf	equ	10
const	equ	11
getvers	equ	12
reset	equ	13
seldsk	equ	14
open	equ	15
close	equ	16
first	equ	17
next	equ	18
delete	equ	19
redseq	equ	20
wrtseq	equ	21
make	equ	22
rename	equ	23
logvec	equ	24
curdsk	equ	25
setdma	equ	26
getalv	equ	27
setro	equ	28
getro	equ	29
setattr	equ	30
getdpb	equ	31
usrnum	equ	32
redrnd	equ	33
wrtrnd	equ	34
;
ctrlc	equ	3
bell	equ	7
back	equ	8
cleft	equ	8
tab	equ	9
lf	equ	10
cdown	equ	10
cup	equ	11
cright	equ	12
cr	equ	13
ctrlz	equ	26
esc	equ	27
space	equ	' '
del	equ	7fh
;
bdos_call	MACRO	code,value
		 ifnb	<value>
		  ld	de,value
		 endif
		 ld	c,code
		 call	?bdos
		endm
;
print		macro	text
		 bdos_call	prtstr,text
		endm

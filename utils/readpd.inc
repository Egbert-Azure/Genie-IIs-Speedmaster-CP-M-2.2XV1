	.printl Including Readpd.inc

read_pdrive:
	LD	DE,pdrive_fcb			;Read Names to name_buffer
	LD	C,open
	CALL	?bdos
	inc	a				;Successful open ?
	jr	nz,openok			;Y: jp
	ld	a,1				;N: try on A:
	ld	(pdrive_fcb),a
	ld	c,open
	call	?bdos
	inc	a				;Successful ?
	jp	z,open_error			;N: Error

openok:
	ld	b,16				;get 16 records from pdrive.sys
	ld	hl,name_buffer
	ld	(pd_dma_adr),hl
read_pd_loop:
	push	bc
	LD	DE,<pd_dma_adr: dw 0>
	ld	c,setdma
	call	?bdos
	LD	DE,pdrive_fcb
	LD	c,redseq
	CALL	?bdos
	pop	bc
	or	a
	jp	nz,read_error
	ld	hl,(pd_dma_adr)
	ld	de,128
	add	hl,de
	ld	(pd_dma_adr),hl
	djnz	read_pd_loop

	LD	HL,iobuf			;Parameter given ?
	ld	a,(hl)
	inc	hl
	or	a
	jp	z,syntax_error			;N: error

	cp	4				;Min. 4 Chars
	jp	c,syntax_error
	cp	20				;Max. 19 Chars
	jp	nc,syntax_error

	push	hl
	ld	e,a
	ld	d,0
	add	hl,de
	ld	b,16
setpd1:
	ld	(hl),' '			;Pad with blanks
	inc	hl
	djnz	setpd1
	pop	hl

	ld	a,(hl)
	inc	hl
	cp	20h
	jp	nz,syntax_error

	ld	a,(hl)
	ld	(drive_name),a			;save drive name
	inc	hl
	SUB	041H
	JP	C,drive_error
	CP	4				;A-D ?
	JP	NC,drive_error
	ld	(drive_code),a			;save drive name value

	ld	a,(hl)
	cp	':'
	jp	nz,syntax_error

	LD	DE,name_buffer
	ld	c,0				;C is Namecounter
setpd2:
	LD	HL,iobuf+4			;Start of NAME
	LD	A,(DE)
	CP	01aH				;End of Table ?
	jp	z,notfound_error
	LD	b,16				;Compare 16 Chars
setpd3:
	LD	A,(DE)
	CP	(HL)
	JR	NZ,nextnm
	INC	DE
	INC	HL
	DJNZ	setpd3
	JR	found

nextnm:
	inc	de				;Inc DE to next name
	djnz	nextnm
	inc	c				;Inc Namecounter
	ld	a,c
	cp	128				;Max 128 Names
	jr	c,setpd2
	jp	notfound_error

found:
	LD	A,C
	ADD	A,16
	ld	(pd_record_no),a
	ld	de,pd_info
	ld	c,setdma
	call	?bdos
	LD	DE,pdrive_fcb			;Read 1 Record
	LD	c,redrnd
	CALL	?bdos

	ld	a,(pd_info+16h)			;Internal manageable ?
	or	a
	jp	nz,extern_error

	ld	a,(pd_info+0dh)			;System tracks ??
	cp	2
	jp	c,systracks_error
	ret
;----------------------------------------------------------
syntax_error:
	ld	de,syntax_text
	jp	error
notfound_error:
	ld	de,notfound_text
	jp	error
drive_error:
	ld	de,drive_text
	jp	error
open_error:
	ld	de,open_text
	jp	error
read_error:
	ld	de,read_text
	jp	error
extern_error:
	ld	de,extern_text
	jp	error
systracks_error:
	ld	de,systracks_text
	jp	error

syntax_text:
	defm	'Falsche Parameterangabe.'
	defb	cr,lf
	defm	'Benutze ID X:NAME zur Einstellung des Laufwerks X'
	defm	' auf den Datensatz NAME.$'
notfound_text:
	defm	'Gew{hlter Datensatz nicht vorhanden.$'
drive_text:
	defm	'Unzul{ssige oder falsche Laufwerksbezeichnung.$'
open_text:
	defm	'Datenfile PDRIVE.SYS nicht gefunden !$'
read_text:
	defm	'Kann Datensatz von PDRIVE.SYS nicht lesen.$'
extern_text:
	defm	'Dieses Format wird nicht direkt unterst}tzt.$'
systracks_text:
	defm	'Das gew{hlte Format hat zuwenig Systemspuren.$'
;
pdrive_fcb:
	defb	0
	defm	'PDRIVE  ','SYS'
	defs	21,0
pd_record_no:
	defs	4,0

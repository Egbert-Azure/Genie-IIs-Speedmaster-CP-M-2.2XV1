	.printl Including Format.inc
;
; build format track
; track_number, side_number, length_code, sector_length
;
build_track:
	di
	ld	b,10				;10 sectors
	ld	hl,sector_number
	ld	(hl),0				;start with sector # 0

	ld	de,<gap_address: dw 0>		;set start gap
	inc	de
	ld	hl,track_buffer
	call	fill_gap

seclop:
	push	bc
	push	de
	call	fill_byte

	ld	(hl),0feh			;ID Start
	inc	hl
	ld	(hl),<track_number: 0>		;track #
	inc	hl
	ld	(hl),<side_number: 0>		;side #
	inc	hl
	ld	(hl),<sector_number: 0>		;sector #
	ld	a,(sector_number)
	inc	a
	ld	(sector_number),a		;save next sector#
	inc	hl
	ld	(hl),<length_code: 0>		;sector length code 256/512
	inc	hl
	ld	(hl),0f7h			;CRC (writes 2 bytes)
	inc	hl

	call	fill_byte

	ld	(hl),0fbh			;dam nondeleted
	inc	hl
	ld	bc,<sector_length: dw 0>	;sector length (256/512)
	ld	(hl),0e5h			;fill sector with 0e5h
	ld	d,h
	ld	e,l
	inc	de
	ldir

	ld	(hl),0f7h			;CRC
	inc	hl
	pop	de
	pop	bc
	djnz	seclop

	ld	de,(gap_address)
	ld	a,(de)
	ld	(hl),a
	inc	hl
	ld	d,h
	ld	e,l
	ld	bc,1000
	ldir

	ret
;----------------------------------------------------------
fill_gap:
	push	bc
	ld	a,(de)
	ld	b,a
	inc	de
	ld	a,(de)
	inc	de
fill_gap_loop:
	ld	(hl),a
	inc	hl
	djnz	fill_gap_loop
	pop	bc
	ret
;
fill_byte:
	ld	a,(de)
	or	a
	jr	z,fill_end
	call	fill_gap
	jr	fill_byte

fill_end:
	inc	de
	inc	de
	ret
;----------------------------------------------------------
;
; Gap table:
; 1 byte fill rest of track
; 2 bytes: repeat count , fill byte
; 0000 being last entry
;
sd_gap_table:
	defb	0ffh
	defw	0ff20h

	defw	0ff0bh
	defw	00006h
	defw	00000h

	defw	0ff0bh
	defw	00006h
	defw	00000h

dd_gap_table:			;DDEN, 512 Bytes/Sector
	defb	04eh
	defw	04e28h

	defw	04e16h
	defw	0000ch
	defw	0f503h
	defw	00000h

	defw	04e16h
	defw	0000ch
	defw	0f503h
	defw	00000h
;
;----------------------------------------------------------
; write format track to disk
;
write_track:
	call	switch_in
	call	resel
	ld	hl,a$command
	ld	(hl),0f4h			;write track
	call	delay
	bit	0,a				;busy ?
	jr	z,frmt4
	ld	de,track_buffer
	ld	bc,a$data
	ld	a,(de)
	inc	de
frmt1:
	bit	0,(hl)
	jr	z,frmt4
	bit	1,(hl)
	jr	z,frmt1
frmt2:
	ld	(bc),a
	ld	a,(de)
	inc	de
frmt3:
	bit	1,(hl)
	jr	nz,frmt2
	bit	1,(hl)
	jr	nz,frmt2
	bit	1,(hl)
	jr	nz,frmt2
	bit	0,(hl)
	jr	z,frmt4
	bit	1,(hl)
	jr	nz,frmt2
	bit	7,(hl)
	jr	z,frmt3
frmt4:
	ld	a,(hl)
	ld	(hl),0d0h
	ld	(format_final_status),a
	and	0110_0100b
	jp	nz,format_error
	jp	switch_out

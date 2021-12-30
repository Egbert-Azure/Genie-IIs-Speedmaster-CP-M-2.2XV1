;
; @  absolute data
; ?@ address of data
; ?  address of routine
;
	.printl Including BDOS for CP/M 2.2XV3
;
;
ctrlc	equ	3
ctrle	equ	5
xon	equ	17
ctrlr	equ	18
xoff	equ	19
ctrlu	equ	21
ctrlx	equ	24

	.COMMENT !

bell	equ	7
back	equ	8
tab	equ	9
lf	equ	10
cr	equ	13
ctrlp	equ	16
space	equ	32
del	equ	127
;


COMMENT END:	!


max_func_#	equ	41		;Highest Function#
version_#	equ	22h		;CP/M 2.2

b_cboot		equ	bios+00h
b_wboot		equ	bios+03h
b_const		equ	bios+06h
b_conin		equ	bios+09h
b_conout	equ	bios+0ch
b_list		equ	bios+0fh
b_punch		equ	bios+12h
b_reader	equ	bios+15h
b_home		equ	bios+18h
b_seldsk	equ	bios+1bh
b_settrk	equ	bios+1eh
b_setsec	equ	bios+21h
b_setdma	equ	bios+24h
b_read		equ	bios+27h
b_write		equ	bios+2ah
b_listst	equ	bios+2dh
b_sectrn	equ	bios+30h
;
offset_flags	equ	 9
offset_ex	equ	12
offset_s1	equ	13
offset_s2	equ	14
offset_rc	equ	15
offset_dx	equ	16
offset_cr	equ	32
offset_r0	equ	33
offset_rc_cr	equ	17
;
bdos_serial_#:
	defs	6,0
bdos:
	jp	bdos1
err_loc_table:
	defw	?err_bad_sector
	defw	?err_select
	defw	?err_disk_ro
	defw	?err_file_ro
;
bdos1:
	EX	DE,HL
	LD	(@entry_word),HL
	EX	DE,HL
	LD	A,E
	LD	(@entry_byte),A
	LD	HL,0
	LD	(@exit_value),HL
	ADD	HL,SP
	LD	(@user_stack),HL
	LD	SP,stack
	XOR	A
	LD	(@fcb_disk_#),A
	LD	(@auto_selected?),A
	LD	HL,?bdos_exit
	PUSH	HL
	LD	A,C
	CP	max_func_#
	RET	NC
	LD	C,E
	LD	HL,bdos_loc_table
	LD	E,A
	LD	D,0
	ADD	HL,DE
	ADD	HL,DE
	LD	E,(HL)
	INC	HL
	LD	D,(HL)
	LD	HL,(@entry_word)
	EX	DE,HL
	JP	(HL)
bdos_loc_table:
	defw	b_wboot
	defw	?console_input
	defw	?console_output
	defw	?reader_input
	defw	b_punch			;punch_output
	defw	b_list			;list_output
	defw	?direct_io
	defw	?get_io_byte
	defw	?set_io_byte
	defw	?print_string
	defw	?read_console_buffer
	defw	?get_console_status
	defw	?return_version_#
	defw	?reset_disk_system
	defw	?select_disk
	defw	?open_file
	defw	?close_file
	defw	?search_first
	defw	?search_next
	defw	?delete_file
	defw	?read_sequ
	defw	?write_sequ
	defw	?make_file
	defw	?rename_file
	defw	?return_login_vec
	defw	?return_current_disk
	defw	?set_dma
	defw	?get_allocation_addr
	defw	?write_protect_disk
	defw	?get_ro_vec
	defw	?set_file_attr
	defw	?get_parameter_addr
	defw	?set_get_user_code
	defw	?read_random
	defw	?write_random
	defw	?compute_file_size
	defw	?set_random_record
	defw	?reset_disk
	defw	?dummy
	defw	?dummy
	defw	?write_zero_random
;
;
?err_bad_sector:
	LD	HL,txt_bad_sector
	CALL	show_error
	CP	ctrlc
	jp	bios+03ch			;JP	Z,?boot
	RET
;
?err_select:
	LD	HL,txt_select
	jp	?err_bad_sector+3		;JP	err_boot
;
?err_disk_ro:
	LD	HL,txt_disk_ro
	JP	err_boot
;
?err_file_ro:
	LD	HL,txt_file_ro
err_boot:
	CALL	show_error
	JP	?boot
;
error_text:
	defm	'Bdos Err On '
@disk_name:
	defm	' : $'
txt_bad_sector:
	defm	'Bad Sector$'
txt_select:
	defm	'Select$'
txt_file_ro:
	defm	'File '
txt_disk_ro:
	defm	'R/O$'
;
show_error:
	PUSH	HL
	CALL	print_cr_lf
	LD	A,(@current_disk)
	ADD	A,'A'
	LD	(@disk_name),A
	LD	BC,error_text
	CALL	print_string
	POP	BC
	CALL	print_string
get_char:
	LD	HL,@status_char
	LD	A,(HL)
	LD	(HL),0
	OR	A
	RET	NZ
	JP	b_conin
;
input_char:
	CALL	get_char
	CALL	check_char
	RET	C
	PUSH	AF
	LD	C,A
	CALL	?console_output
	POP	AF
	RET
;
check_char:
	CP	cr
	RET	Z
	CP	lf
	RET	Z
	CP	tab
	RET	Z
	CP	back
	RET	Z
	CP	space
	RET
;
char_xoff?:
	LD	A,(@status_char)
	OR	A
	JP	NZ,true_status
	CALL	b_const
	AND	001H
	RET	Z
	CALL	b_conin
	CP	xoff
	JP	NZ,save_status
	CALL	b_conin
	CP	ctrlc
	JP	Z,?boot
	XOR	A
	RET
;
save_status:
	LD	(@status_char),A
true_status:
	LD	A,001H
	RET
;
print_char:
	LD	A,(@chars_in_line)
	OR	A
	JP	NZ,adjust_tab
	PUSH	BC
	CALL	char_xoff?
	POP	BC
	PUSH	BC
	CALL	b_conout
	POP	BC
	PUSH	BC
	LD	A,(@print_flag)
	OR	A
	CALL	NZ,b_list
	POP	BC
adjust_tab:
	LD	A,C
	LD	HL,@output_counter
	CP	07FH
	RET	Z
	INC	(HL)
	CP	space
	RET	NC
	DEC	(HL)
	LD	A,(HL)
	OR	A
	RET	Z
	LD	A,C
	CP	back
	JP	NZ,not_back
	DEC	(HL)
	RET
;
not_back:
	CP	lf
	RET	NZ
	LD	(HL),0
	RET
;
show_char:
	LD	A,C
	CALL	check_char
	JP	NC,?console_output
	PUSH	AF
	LD	C,'^'				;CTRL-Prefix
	CALL	print_char
	POP	AF
	OR	040H
	LD	C,A
?console_output:
	LD	A,C
	CP	tab
	JP	NZ,print_char
show_tab:
	LD	C,space
	CALL	print_char
	LD	A,(@output_counter)
	AND	007H
	JP	NZ,show_tab
	RET
;
print_backspace:
	CALL	print_back
	LD	C,space
	CALL	b_conout
print_back:
	LD	C,back
	JP	b_conout
;
clear_line:
	LD	C,'#'
	CALL	print_char
	CALL	print_cr_lf
clear_loop:
	LD	A,(@output_counter)
	LD	HL,@input_start
	CP	(HL)
	RET	NC
	LD	C,space
	CALL	print_char
	JP	clear_loop
;
print_cr_lf:
	LD	C,cr
	CALL	print_char
	LD	C,lf
	JP	print_char
;
print_string:
	LD	A,(BC)
	CP	'$'
	RET	Z
	INC	BC
	PUSH	BC
	LD	C,A
	CALL	?console_output
	POP	BC
	JP	print_string
;
?read_console_buffer:
	LD	A,(@output_counter)
	LD	(@input_start),A
	LD	HL,(@entry_word)
	LD	C,(HL)
	INC	HL
	PUSH	HL
	LD	B,0
read_next1:
	PUSH	BC
	PUSH	HL
read_next2:
	CALL	get_char
	AND	07FH
	POP	HL
	POP	BC
	CP	cr
	JP	Z,end_of_input
	CP	lf
	JP	Z,end_of_input
;CTRL-H					Backspace & delete
	CP	back
	JP	NZ,check_del
	LD	A,B
	OR	A
	JP	Z,read_next1
	DEC	B
	LD	A,(@output_counter)
	LD	(@chars_in_line),A
	JP	retype_line
;DEL					delete & echo
check_del:
	CP	del
	JP	NZ,check_ctrle
	LD	A,B
	OR	A
	JP	Z,read_next1
	LD	A,(HL)
	DEC	B
	DEC	HL
	JP	show_input
;CTRL-E					physical end of line
check_ctrle:
	CP	ctrle
	JP	NZ,check_ctrlp
	PUSH	BC
	PUSH	HL
	CALL	print_cr_lf
	XOR	A
	LD	(@input_start),A
	JP	read_next2
;CTRL-P					printer on/off
check_ctrlp:
	CP	ctrlp
	JP	NZ,check_ctrlx
	PUSH	HL
	LD	HL,@print_flag
	LD	A,001H
	SUB	(HL)
	LD	(HL),A
	POP	HL
	jp	bios+039h			;JP	read_next1
;CTRL-X					delete line
check_ctrlx:
	CP	ctrlx
	JP	NZ,check_ctrlu
	POP	HL
ctrlx_loop:
	LD	A,(@input_start)
	LD	HL,@output_counter
	CP	(HL)
	JP	NC,?read_console_buffer
	DEC	(HL)
	CALL	print_backspace
	JP	ctrlx_loop
;CTRL-U					remove line
check_ctrlu:
	CP	ctrlu
	JP	NZ,check_ctrlr
	CALL	clear_line
	POP	HL
	JP	?read_console_buffer
;CTRL-R					retype line
check_ctrlr:
	CP	ctrlr
	JP	NZ,save_char
retype_line:
	PUSH	BC
	CALL	clear_line
	POP	BC
	POP	HL
	PUSH	HL
	PUSH	BC
retype_loop:
	LD	A,B
	OR	A
	JP	Z,delete_tabs
	INC	HL
	LD	C,(HL)
	DEC	B
	PUSH	BC
	PUSH	HL
	CALL	show_char
	POP	HL
	POP	BC
	JP	retype_loop
;
delete_tabs:
	PUSH	HL
	LD	A,(@chars_in_line)
	OR	A
	JP	Z,read_next2
	LD	HL,@output_counter
	SUB	(HL)
	LD	(@chars_in_line),A
back_tab:
	CALL	print_backspace
	LD	HL,@chars_in_line
	DEC	(HL)
	JP	NZ,back_tab
	JP	read_next2
;
save_char:
	INC	HL
	LD	(HL),A
	INC	B
show_input:
	PUSH	BC
	PUSH	HL
	LD	C,A
	CALL	show_char
	POP	HL
	POP	BC
	LD	A,(HL)
	CP	ctrlc
	LD	A,B
	JP	NZ,not_ctrlc
	CP	1
	JP	Z,?boot
not_ctrlc:
	CP	C
	JP	C,read_next1
end_of_input:
	POP	HL
	LD	(HL),B
	LD	C,cr
	JP	print_char
;
?console_input:
	CALL	input_char
	JP	return_byte
;
?reader_input:
	CALL	b_reader
	JP	return_byte
;
?direct_io:
	LD	A,C
	INC	A
	JP	Z,direct_conin
	INC	A
	JP	Z,b_const
	JP	b_conout
;
direct_conin:
	CALL	b_const
	OR	A
	JP	Z,ret_to_user
	CALL	b_conin
	JP	return_byte
;
?get_io_byte:
	LD	A,(@io_byte)
	JP	return_byte
;
?set_io_byte:
	LD	HL,@io_byte
	LD	(HL),C
	RET
;
?print_string:
	EX	DE,HL
	LD	C,L
	LD	B,H
	JP	print_string
;
?get_console_status:
	CALL	char_xoff?
;
return_byte:
	LD	(@exit_value),A
?dummy:
	RET
;
read_write_error:
	LD	A,001H
	JP	return_byte
;
@chars_in_line:		defb	0
@input_start:		defb	0
@output_counter:	defb	0
@print_flag:		defb	0
@status_char:		defb	0
@user_stack:		defw	0
;
			defs	48,0
stack:
;----------------------------------------------------------
;
@user_#:		defb	0
@current_disk:		defb	0
@entry_word:		defw	0
@exit_value:		defw	0
;
select_error:
	LD	HL,err_loc_table+2
error_entry:
	LD	E,(HL)
	INC	HL
	LD	D,(HL)
	EX	DE,HL
	JP	(HL)
;
move_de_hl:
	INC	C
move_loop:
	DEC	C
	RET	Z
	LD	A,(DE)
	LD	(HL),A
	INC	DE
	INC	HL
	JP	move_loop
;
physical_select:
	LD	A,(@current_disk)
	LD	C,A
	CALL	b_seldsk
	LD	A,H
	OR	L
	RET	Z

	LD	E,(HL)
	INC	HL
	LD	D,(HL)
	INC	HL
	LD	(?@end_of_entries),HL
	INC	HL
	INC	HL
	LD	(?@track_#),HL
	INC	HL
	INC	HL
	LD	(?@track_record_#),HL
	INC	HL
	INC	HL

	EX	DE,HL
	LD	(?@xltab),HL
	LD	HL,?@dirbuf
	LD	C,008H
	CALL	move_de_hl

	LD	HL,(?@dpb)
	EX	DE,HL
	LD	HL,@spt
	LD	C,00FH
	CALL	move_de_hl

	LD	HL,(@dsm)
	LD	A,H
	LD	HL,@large_dsm?
	LD	(HL),0FFH
	OR	A
	JP	Z,select_new_ok
	LD	(HL),0
select_new_ok:
	LD	A,0FFH
	OR	A
	RET
;
home_disk:
	CALL	b_home
	XOR	A
	LD	HL,(?@track_#)
	LD	(HL),A
	INC	HL
	LD	(HL),A
	LD	HL,(?@track_record_#)
	LD	(HL),A
	INC	HL
	LD	(HL),A
	RET
;
read_record:
	CALL	b_read
	JP	test_rw_status
;
write_record:
	CALL	b_write

test_rw_status:
	OR	A
	RET	Z
bad_sector_error:
	LD	HL,err_loc_table
	JP	error_entry
;
set_dir_trk_sec:
	LD	HL,(@dir_entry_#)
	LD	C,002H
	CALL	shift_right_hl
	LD	(@absolute_record_#),HL
	LD	(@dir_record_#),HL
set_dsk_trk_sec:
	LD	HL,@absolute_record_#
	LD	C,(HL)
	INC	HL
	LD	B,(HL)
	LD	HL,(?@track_record_#)
	LD	E,(HL)
	INC	HL
	LD	D,(HL)
	LD	HL,(?@track_#)
	LD	A,(HL)
	INC	HL
	LD	H,(HL)
	LD	L,A
track_dec:
	LD	A,C
	SUB	E
	LD	A,B
	SBC	A,D
	JP	NC,track_inc
	PUSH	HL
	LD	HL,(@spt)
	LD	A,E
	SUB	L
	LD	E,A
	LD	A,D
	SBC	A,H
	LD	D,A
	POP	HL
	DEC	HL
	JP	track_dec
;
track_inc:
	PUSH	HL
	LD	HL,(@spt)
	ADD	HL,DE
	JP	C,track_found
	LD	A,C
	SUB	L
	LD	A,B
	SBC	A,H
	JP	C,track_found
	EX	DE,HL
	POP	HL
	INC	HL
	JP	track_inc
;
track_found:
	POP	HL
	PUSH	BC
	PUSH	DE
	PUSH	HL
	EX	DE,HL
	LD	HL,(@off)
	ADD	HL,DE
	LD	B,H
	LD	C,L
	CALL	b_settrk
	POP	DE
	LD	HL,(?@track_#)
	LD	(HL),E
	INC	HL
	LD	(HL),D
	POP	DE
	LD	HL,(?@track_record_#)
	LD	(HL),E
	INC	HL
	LD	(HL),D
	POP	BC
	LD	A,C
	SUB	E
	LD	C,A
	LD	A,B
	SBC	A,D
	LD	B,A
	LD	HL,(?@xltab)
	EX	DE,HL
	CALL	b_sectrn
	LD	C,L
	LD	B,H
	JP	b_setsec
;
get_block_offset:
	LD	HL,@bsh
	LD	C,(HL)			;2^c = #of records per block
	LD	A,(@current_sequ_rec)
block_1_loop:
	OR	A			;cy=0
	RRA
	DEC	C
	JP	NZ,block_1_loop
	LD	B,A			;b=block # of current sequ rec.
	LD	A,008H
	SUB	(HL)
	LD	C,A			;2^c = # of blocks per extend
	LD	A,(@current_extend_#)
block_2_loop:
	DEC	C
	JP	Z,add_a_b
	OR	A			;cy=0
	RLA
	JP	block_2_loop
;
add_a_b:
	ADD	A,B
	RET
;
get_block_#:
	LD	HL,(@entry_word)
	LD	DE,offset_dx
	ADD	HL,DE
	ADD	HL,BC
	LD	A,(@large_dsm?)
	OR	A
	JP	Z,get_block_word
	LD	L,(HL)
	LD	H,0
	RET
;
get_block_word:
	ADD	HL,BC
	LD	E,(HL)
	INC	HL
	LD	D,(HL)
	EX	DE,HL
	RET
;
set_@block_#:
	CALL	get_block_offset
	LD	C,A
	LD	B,0
	CALL	get_block_#
	LD	(@block_#),HL
	RET
;
check_@block_#:
	LD	HL,(@block_#)
	LD	A,L
	OR	H
	RET
;
set_@absolute_record#:
	LD	A,(@bsh)
	LD	HL,(@block_#)
rec_offset_loop:
	ADD	HL,HL
	DEC	A
	JP	NZ,rec_offset_loop
	LD	(@block_start_record),HL
	LD	A,(@blm)
	LD	C,A
	LD	A,(@current_sequ_rec)
	AND	C
	OR	L
	LD	L,A
	LD	(@absolute_record_#),HL
	RET
;
get_extend_pointer:
	LD	HL,(@entry_word)
	LD	DE,offset_ex
	ADD	HL,DE
	RET
;
get_rc_cr_pointer:
	LD	HL,(@entry_word)
	LD	DE,offset_rc
	ADD	HL,DE
	EX	DE,HL
	LD	HL,offset_rc_cr
	ADD	HL,DE
	RET
;
get_record_#s:
	CALL	get_rc_cr_pointer
	LD	A,(HL)
	LD	(@current_sequ_rec),A
	EX	DE,HL
	LD	A,(HL)
	LD	(@ext_record_count),A
	CALL	get_extend_pointer
	LD	A,(@exm)
	AND	(HL)
	LD	(@current_extend_#),A
	RET
;
update_record_#s:
	CALL	get_rc_cr_pointer
	LD	A,(@sequential?)
	CP	002H				;zero fill ?
	JP	NZ,update_sequ_#
	XOR	A
update_sequ_#:
	LD	C,A
	LD	A,(@current_sequ_rec)
	ADD	A,C
	LD	(HL),A				;set new sequ rec to fcb
	EX	DE,HL
	LD	A,(@ext_record_count)
	LD	(HL),A				;set new ext rec cnt to fcb
	RET
;
; HL = HL / 2^C
shift_right_hl:
	INC	C
shift_right_loop:
	DEC	C
	RET	Z
	LD	A,H
	OR	A
	RRA
	LD	H,A
	LD	A,L
	RRA
	LD	L,A
	JP	shift_right_loop
;
checksum_dirbuf:
	LD	C,128
	LD	HL,(?@dirbuf)
	XOR	A
checksum_loop:
	ADD	A,(HL)
	INC	HL
	DEC	C
	JP	NZ,checksum_loop
	RET
;
shift_left_hl:
	INC	C
shift_left_loop:
	DEC	C
	RET	Z
	ADD	HL,HL
	JP	shift_left_loop
;
set_disk_vec:
	PUSH	BC
	LD	A,(@current_disk)
	LD	C,A
	LD	HL,1
	CALL	shift_left_hl
	POP	BC
	LD	A,C
	OR	L
	LD	L,A
	LD	A,B
	OR	H
	LD	H,A
	RET
;
check_disk_ro:
	LD	HL,(@ro_vec)
	LD	A,(@current_disk)
	LD	C,A
	CALL	shift_right_hl
	LD	A,L
	AND	001H
	RET
;
?write_protect_disk:
	LD	HL,@ro_vec
	LD	C,(HL)
	INC	HL
	LD	B,(HL)
	CALL	set_disk_vec
	LD	(@ro_vec),HL
	LD	HL,(@drm)
	INC	HL
	EX	DE,HL
	LD	HL,(?@end_of_entries)
	LD	(HL),E
	INC	HL
	LD	(HL),D
	RET
;
entry_ro?:
	CALL	get_entry_pointer
fcb_ro?:
	LD	DE,9
	ADD	HL,DE
	LD	A,(HL)
	RLA
	RET	NC
file_ro_error:
	LD	HL,err_loc_table+6
	JP	error_entry
;
disk_ro?:
	CALL	check_disk_ro
	RET	Z
disk_ro_error:
	LD	HL,err_loc_table+4
	JP	error_entry
;
get_entry_pointer:
	LD	HL,(?@dirbuf)
	LD	A,(@dir_offset)
add_hl_and_a:
	ADD	A,L
	LD	L,A
	RET	NC
	INC	H
	RET
;
get_s2:
	LD	HL,(@entry_word)
	LD	DE,offset_s2
	ADD	HL,DE
	LD	A,(HL)
	RET
;
zero_s2:
	CALL	get_s2
	LD	(HL),0
	RET
;
set_s2:
	CALL	get_s2
	OR	080H
	LD	(HL),A
	RET
;
entry_#_free?:				;Y: NC
	LD	HL,(@dir_entry_#)
	EX	DE,HL
	LD	HL,(?@end_of_entries)
	LD	A,E
	SUB	(HL)
	INC	HL
	LD	A,D
	SBC	A,(HL)
	RET
;
new_end_of_entries?:
	CALL	entry_#_free?
	RET	C
	INC	DE
	LD	(HL),D
	DEC	HL
	LD	(HL),E
	RET
;
sub_de_hl:
	LD	A,E
	SUB	L
	LD	L,A
	LD	A,D
	SBC	A,H
	LD	H,A
	RET
;
update_csv:
	LD	C,0FFH
test_csv:
	LD	HL,(@dir_record_#)
	EX	DE,HL
	LD	HL,(@cks)
	CALL	sub_de_hl
	RET	NC
	PUSH	BC
	CALL	checksum_dirbuf
	LD	HL,(?@csv)
	EX	DE,HL
	LD	HL,(@dir_record_#)
	ADD	HL,DE
	POP	BC
	INC	C
	JP	Z,update_csv_byte
	CP	(HL)
	RET	Z
	CALL	entry_#_free?
	RET	NC
	CALL	?write_protect_disk
	RET
;
update_csv_byte:
	LD	(HL),A
	RET
;
dir_write:
	CALL	update_csv
	CALL	set_dma_to_dirbuf
	LD	C,1			;Directory write
	CALL	write_record
	JP	restore_dma
;
dir_read:
	CALL	set_dma_to_dirbuf
	CALL	read_record
restore_dma:
	LD	HL,@bdos_dma_address
	JP	set_dma_to_hl
;
set_dma_to_dirbuf:
	LD	HL,?@dirbuf
set_dma_to_hl:
	LD	C,(HL)
	INC	HL
	LD	B,(HL)
	JP	b_setdma
;
move_dirbuf_to_dma:
	LD	HL,(?@dirbuf)
	EX	DE,HL
	LD	HL,(@bdos_dma_address)
	LD	C,128
	JP	move_de_hl
;
all_entries_compared?:
	LD	HL,@dir_entry_#
	LD	A,(HL)
	INC	HL
	CP	(HL)
	RET	NZ
	INC	A
	RET
;
reset_entry_#:
	LD	HL,0FFFFH
	LD	(@dir_entry_#),HL
	RET
;
check_dir_record:
	LD	HL,(@drm)
	EX	DE,HL
	LD	HL,(@dir_entry_#)
	INC	HL
	LD	(@dir_entry_#),HL
	CALL	sub_de_hl
	JP	NC,check_dir_offset
	JP	reset_entry_#
;
check_dir_offset:
	LD	A,(@dir_entry_#)
	AND	003H
	LD	B,5
dir_offset_loop:
	ADD	A,A
	DEC	B
	JP	NZ,dir_offset_loop
	LD	(@dir_offset),A
	OR	A
	RET	NZ
	PUSH	BC
	CALL	set_dir_trk_sec
	CALL	dir_read
	POP	BC
	JP	test_csv
;
get_alv_bit:
	LD	A,C
	AND	007H
	INC	A
	LD	E,A
	LD	D,A
	LD	A,C
	RRCA
	RRCA
	RRCA
	AND	01FH
	LD	C,A
	LD	A,B
	ADD	A,A
	ADD	A,A
	ADD	A,A
	ADD	A,A
	ADD	A,A
	OR	C
	LD	C,A
	LD	A,B
	RRCA
	RRCA
	RRCA
	AND	01FH
	LD	B,A
	LD	HL,(?@alv)
	ADD	HL,BC
	LD	A,(HL)
get_alv_loop:
	RLCA
	DEC	E
	JP	NZ,get_alv_loop
	RET
;
change_alv_bit:
	PUSH	DE
	CALL	get_alv_bit
	AND	0FEH
	POP	BC
	OR	C
set_alv_loop:
	RRCA
	DEC	D
	JP	NZ,set_alv_loop
	LD	(HL),A
	RET
;
update_alv:
	CALL	get_entry_pointer
	LD	DE,offset_dx
	ADD	HL,DE
	PUSH	BC
	LD	C,17
update_alv_loop:
	POP	DE
	DEC	C
	RET	Z
	PUSH	DE
	LD	A,(@large_dsm?)
	OR	A
	JP	Z,word_value
	PUSH	BC
	PUSH	HL
	LD	C,(HL)
	LD	B,0
	JP	test_block_#
;
word_value:
	DEC	C
	PUSH	BC
	LD	C,(HL)
	INC	HL
	LD	B,(HL)
	PUSH	HL
test_block_#:
	LD	A,C
	OR	B
	JP	Z,no_block_alloc
	LD	HL,(@dsm)
	LD	A,L
	SUB	C
	LD	A,H
	SBC	A,B
	CALL	NC,change_alv_bit
no_block_alloc:
	POP	HL
	INC	HL
	POP	BC
	JP	update_alv_loop
;
logical_select:
	LD	HL,(@dsm)
	LD	C,3
	CALL	shift_right_hl
	INC	HL
	LD	B,H
	LD	C,L			;bc = @dsm / 8

	LD	HL,(?@alv)
clr_alv_loop:
	LD	(HL),0
	INC	HL
	DEC	BC
	LD	A,B
	OR	C
	JP	NZ,clr_alv_loop

	LD	HL,(@al0_al1)
	EX	DE,HL
	LD	HL,(?@alv)
	LD	(HL),E
	INC	HL
	LD	(HL),D

	CALL	home_disk

	LD	HL,(?@end_of_entries)
	LD	(HL),3
	INC	HL
	LD	(HL),0

	CALL	reset_entry_#
login_next_entry:
	LD	C,0FFH
	CALL	check_dir_record
	CALL	all_entries_compared?
	RET	Z
	CALL	get_entry_pointer
	LD	A,0E5H
	CP	(HL)			;any entry ?
	JP	Z,login_next_entry	;Y: not allowed

	LD	A,(@user_#)
	CP	(HL)			;right user ?
	JP	NZ,entry_not_right
	INC	HL			;inc dir entry pointer
	LD	A,(HL)
	SUB	'$'			;temporary file ?
	JP	NZ,entry_not_right	;N: jp
	DEC	A			;Y: set error
	LD	(@exit_value),A
entry_not_right:
	LD	C,001H
	CALL	update_alv
	CALL	new_end_of_entries?
	JP	login_next_entry
;
return_found_flag:
	LD	A,(@file_found?)
	JP	return_byte
;
check_extend_#:
	PUSH	BC
	PUSH	AF
	LD	A,(@exm)
	CPL
	LD	B,A
	LD	A,C
	AND	B
	LD	C,A
	POP	AF
	AND	B
	SUB	C
	AND	01FH
	POP	BC
	RET
;
search_dir_first:
	LD	A,0FFH
	LD	(@file_found?),A
	LD	HL,@compare_count
	LD	(HL),C
	LD	HL,(@entry_word)
	LD	(?@search_fcb),HL
	CALL	reset_entry_#
	CALL	home_disk
search_dir_next:
	LD	C,0
	CALL	check_dir_record
	CALL	all_entries_compared?
	JP	Z,entry_not_found

	LD	HL,(?@search_fcb)
	EX	DE,HL
	LD	A,(DE)
	CP	0E5H			;make file ?
	JP	Z,search_make			;Y: match any
	PUSH	DE
	CALL	entry_#_free?
	POP	DE
	JP	NC,entry_not_found	;Y: not found
search_make:
	CALL	get_entry_pointer
	LD	A,(@compare_count)
	LD	C,A
	LD	B,0
compare_next_loop:
	LD	A,C
	OR	A
	JP	Z,return_dir_code
	LD	A,(DE)
	CP	'?'			;joker ?
	JP	Z,compare_next_charac
	LD	A,B
	CP	13
	JP	Z,compare_next_charac
	CP	12
	LD	A,(DE)
	JP	Z,compare_extend
	SUB	(HL)
	AND	07FH
	JP	NZ,search_dir_next
	JP	compare_next_charac
;
compare_extend:
	PUSH	BC
	LD	C,(HL)
	CALL	check_extend_#
	POP	BC
	JP	NZ,search_dir_next
compare_next_charac:
	INC	DE
	INC	HL
	INC	B
	DEC	C
	JP	compare_next_loop
;
return_dir_code:
	LD	A,(@dir_entry_#)
	AND	003H
	LD	(@exit_value),A
	LD	HL,@file_found?
	LD	A,(HL)
	RLA
	RET	NC
	XOR	A
	LD	(HL),A
	RET
;
entry_not_found:
	CALL	reset_entry_#
	LD	A,0FFH
	JP	return_byte
;
erase_files:
	CALL	disk_ro?
	LD	C,00CH
	CALL	search_dir_first
erase_loop:
	CALL	all_entries_compared?
	RET	Z
	CALL	entry_ro?
	CALL	get_entry_pointer
	LD	(HL),0E5H		;free entry
	LD	C,0			;clear alv, free blocks
	CALL	update_alv
	CALL	dir_write
	CALL	search_dir_next
	JP	erase_loop
;
allocate_next_block:
	LD	D,B			;DE=higher blocks counter
	LD	E,C			;BC=lower blocks counter
allocate_next_loop:
	LD	A,C
	OR	B			;any lower blocks left ?
	JP	Z,allocate_higher	;N: jp
	DEC	BC
	PUSH	DE
	PUSH	BC
	CALL	get_alv_bit
	RRA				;block free ?
	JP	NC,allocate_block	;Y: allocate it
	POP	BC
	POP	DE
allocate_higher:
	LD	HL,(@dsm)
	LD	A,E
	SUB	L
	LD	A,D
	SBC	A,H			;any higher blocks left ?
	JP	NC,allocate_lower	;N: jp
	INC	DE
	PUSH	BC
	PUSH	DE
	LD	B,D
	LD	C,E
	CALL	get_alv_bit
	RRA
	JP	NC,allocate_block
	POP	DE
	POP	BC
	JP	allocate_next_loop
;
allocate_block:
	RLA				;shift bits back into position
	INC	A			;set bit 0
	CALL	set_alv_loop		;set new alv
	POP	HL
	POP	DE
	RET
;
allocate_lower:
	LD	A,C			;any lower blocks left ?
	OR	B
	JP	NZ,allocate_next_loop	;Y: check those
	LD	HL,0			;N: no block left
	RET
;
write_complete_entry:
	LD	C,0			;start at 0
	LD	E,32			;move 32 chars
write_partial_entry:
	PUSH	DE
	LD	B,0
	LD	HL,(@entry_word)
	ADD	HL,BC
	EX	DE,HL
	CALL	get_entry_pointer
	POP	BC
	CALL	move_de_hl
write_dir_record:
	CALL	set_dir_trk_sec
	JP	dir_write
;
.rename_file:
	CALL	disk_ro?
	LD	C,12			;compare filename
	CALL	search_dir_first
	LD	HL,(@entry_word)
	LD	A,(HL)			;get old dr code
	LD	DE,offset_dx
	ADD	HL,DE
	LD	(HL),A			;set new dr code
rename_loop:
	CALL	all_entries_compared?
	RET	Z
	CALL	entry_ro?
	LD	C,16			;start at byte 16 (2nd filename)
	LD	E,12			;write 12 chars
	CALL	write_partial_entry
	CALL	search_dir_next
	JP	rename_loop
;
.set_file_attr:
	LD	C,12			;compare filename
	CALL	search_dir_first
set_attr_loop:
	CALL	all_entries_compared?
	RET	Z
	LD	C,0			;start at 0
	LD	E,12			;write 12 bytes (filename only)
	CALL	write_partial_entry
	CALL	search_dir_next
	JP	set_attr_loop
;
.open_file:
	LD	C,15			;compare only for 1st
	CALL	search_dir_first
	CALL	all_entries_compared?
	RET	Z
open_entry:
	CALL	get_extend_pointer
	LD	A,(HL)
	PUSH	AF
	PUSH	HL
	CALL	get_entry_pointer
	EX	DE,HL
	LD	HL,(@entry_word)
	LD	C,32
	PUSH	DE
	CALL	move_de_hl
	CALL	set_s2
	POP	DE
	LD	HL,offset_ex
	ADD	HL,DE
	LD	C,(HL)
	LD	HL,offset_rc
	ADD	HL,DE
	LD	B,(HL)
	POP	HL
	POP	AF
	LD	(HL),A
	LD	A,C
	CP	(HL)
	LD	A,B
	JP	Z,set_record_count
	LD	A,0
	JP	C,set_record_count
	LD	A,80h
set_record_count:
	LD	HL,(@entry_word)
	LD	DE,offset_rc
	ADD	HL,DE
	LD	(HL),A
	RET
;
close_word_block:
	LD	A,(HL)
	INC	HL
	OR	(HL)
	DEC	HL
	RET	NZ
	LD	A,(DE)
	LD	(HL),A
	INC	DE
	INC	HL
	LD	A,(DE)
	LD	(HL),A
	DEC	DE
	DEC	HL
	RET
;
.close_file:
	XOR	A
	LD	(@exit_value),A
	LD	(@dir_entry_#),A
	LD	(@dir_entry_#+1),A
	CALL	check_disk_ro		;disk read only ?
	RET	NZ			;Y: cant close
	CALL	get_s2
	AND	080H			;s2 set ?
	RET	NZ			;Y: cant close
	LD	C,15			;search for 1st
	CALL	search_dir_first
	CALL	all_entries_compared?
	RET	Z
	LD	BC,offset_dx
	CALL	get_entry_pointer
	ADD	HL,BC
	EX	DE,HL			;de=entry pointer
	LD	HL,(@entry_word)
	ADD	HL,BC			;hl=fcb pointer
	LD	C,16			;16 block #s in entry
close_block_loop:
	LD	A,(@large_dsm?)
	OR	A
	JP	Z,word_block_close	;Y: jp
byte_block_close:
	LD	A,(HL)
	OR	A
	LD	A,(DE)
	JP	NZ,close_byte_block
	LD	(HL),A
close_byte_block:
	OR	A
	JP	NZ,same_byte_block?
	LD	A,(HL)
	LD	(DE),A
same_byte_block?:
	CP	(HL)
	JP	NZ,cant_close
	JP	close_next_block
;
word_block_close:
	CALL	close_word_block
	EX	DE,HL
	CALL	close_word_block
	EX	DE,HL
	LD	A,(DE)
	CP	(HL)
	JP	NZ,cant_close
	INC	DE
	INC	HL
	LD	A,(DE)
	CP	(HL)
	JP	NZ,cant_close
	DEC	C
close_next_block:
	INC	DE
	INC	HL
	DEC	C
	JP	NZ,close_block_loop

	LD	BC,-20
	ADD	HL,BC
	EX	DE,HL			;(de) = fcb extend
	ADD	HL,BC			;(hl) = entry extend
	LD	A,(DE)
	CP	(HL)			;fcb extend < entry extend ?
	JP	C,write_closed_entry	;Y: write new dir
	LD	(HL),A			;N: set new entry extend
	LD	BC,3
	ADD	HL,BC
	EX	DE,HL			;de = entry record count
	ADD	HL,BC			;hl = fcb record count
	LD	A,(HL)
	LD	(DE),A			;set entry record count
write_closed_entry:
	LD	A,0FFH
	LD	(@entry_closed?),A
	JP	write_dir_record
;
cant_close:
	LD	HL,@exit_value
	DEC	(HL)
	RET
;
.make_file:
	CALL	disk_ro?
	LD	HL,(@entry_word)
	PUSH	HL
	LD	HL,@make_fcb			;match any entry
	LD	(@entry_word),HL
	LD	C,001H				;search only free entry
	CALL	search_dir_first
	CALL	all_entries_compared?
	POP	HL
	LD	(@entry_word),HL
	RET	Z				;ret if no entry free
	EX	DE,HL
	LD	HL,offset_rc
	ADD	HL,DE
	LD	C,17				;clear rc and dx's
	XOR	A
make_clear_loop:
	LD	(HL),A
	INC	HL
	DEC	C
	JP	NZ,make_clear_loop
	LD	HL,offset_s1
	ADD	HL,DE
	LD	(HL),A
	CALL	new_end_of_entries?
	CALL	write_complete_entry
	JP	set_s2
;
open_next_extend:
	XOR	A
	LD	(@entry_closed?),A
	CALL	.close_file			;close current extend
	CALL	all_entries_compared?
	RET	Z				;ret if extend not found
	LD	HL,(@entry_word)
	LD	BC,offset_ex
	ADD	HL,BC
	LD	A,(HL)
	INC	A				;inc current extend #
	AND	01FH
	LD	(HL),A				;set new extend #
	JP	Z,new_ext_too_big		;jp if extend # too large

	LD	B,A
	LD	A,(@exm)
	AND	B				;Mask new extend #
	LD	HL,@entry_closed?
	AND	(HL)
	JP	Z,search_next_extend
	JP	entry_found
;
new_ext_too_big:
	LD	BC,2
	ADD	HL,BC				;(hl) = s2
	INC	(HL)
	LD	A,(HL)				;get s2
	AND	00FH
	JP	Z,cant_extend_file
search_next_extend:
	LD	C,15				;search only 1st
	CALL	search_dir_first
	CALL	all_entries_compared?		;next extend found ?
	JP	NZ,entry_found		;Y: ok
	LD	A,(@read_or_write?)
	INC	A				;N: read ?
	JP	Z,cant_extend_file		;Y: error

	CALL	.make_file			;N: create new extend
	CALL	all_entries_compared?
	JP	Z,cant_extend_file
	JP	new_extend_opened
;
entry_found:
	CALL	open_entry
new_extend_opened:
	CALL	get_record_#s
	XOR	A
	JP	return_byte
;
cant_extend_file:
	CALL	read_write_error
	JP	set_s2
;
.read_sequ:
	LD	A,1				;inc record # by 1
	LD	(@sequential?),A
read_rnd_seq:
	LD	A,0FFH
	LD	(@read_or_write?),A		;set read
	CALL	get_record_#s
	LD	A,(@current_sequ_rec)
	LD	HL,@ext_record_count
	CP	(HL)				;in current extend ?
	JP	C,read_next_record		;Y: jp
	CP	080H				;more extend ?
	JP	NZ,end_of_file			;N: error
	CALL	open_next_extend
	XOR	A
	LD	(@current_sequ_rec),A
	LD	A,(@exit_value)
	OR	A				;succesfull next extend ?
	JP	NZ,end_of_file			;N: error
read_next_record:
	CALL	set_@block_#
	CALL	check_@block_#
	JP	Z,end_of_file
	CALL	set_@absolute_record#
	CALL	set_dsk_trk_sec
	CALL	read_record
	JP	update_record_#s
;
end_of_file:
	JP	read_write_error
;
.write_sequ:
	LD	A,1				;inc record # by 1
	LD	(@sequential?),A
write_rnd_seq:
	LD	A,0
	LD	(@read_or_write?),A		;set write
	CALL	disk_ro?
	LD	HL,(@entry_word)
	CALL	fcb_ro?
	CALL	get_record_#s
	LD	A,(@current_sequ_rec)
	CP	080H				;in current extend ?
	JP	NC,read_write_error		;N: error
	CALL	set_@block_#
	CALL	check_@block_#
	LD	C,0				;normal write flag
	JP	NZ,block_write_normal
	CALL	get_block_offset
	LD	(@block_offset),A
	LD	BC,0				;Allocate next HIGHER
	OR	A				;block offset = 0 ?
	JP	Z,write_new_entry		;Y: new extend
	LD	C,A
	DEC	BC				;bc = block offset -1
	CALL	get_block_#			;get current block #
	LD	B,H
	LD	C,L				;bc = block #
write_new_entry:
	CALL	allocate_next_block		;next block to BC
	LD	A,L
	OR	H				;successfull alloc ?
	JP	NZ,write_new_block		;Y: ok
disk_full:
	LD	A,002H
	JP	return_byte
;
write_new_block:
	LD	(@block_#),HL
	EX	DE,HL
	LD	HL,(@entry_word)
	LD	BC,offset_dx
	ADD	HL,BC
	LD	A,(@large_dsm?)
	OR	A
	LD	A,(@block_offset)
	JP	Z,new_block_word
	CALL	add_hl_and_a
	LD	(HL),E			;set new block #
	JP	block_write_new
;
new_block_word:
	LD	C,A
	LD	B,0
	ADD	HL,BC
	ADD	HL,BC
	LD	(HL),E
	INC	HL
	LD	(HL),D			;set new block #
block_write_new:
	LD	C,002H			;first write
block_write_normal:
	LD	A,(@exit_value)
	OR	A
	RET	NZ
	PUSH	BC
	CALL	set_@absolute_record#
	LD	A,(@sequential?)
	DEC	A
	DEC	A			;zero fill ?
	JP	NZ,write_actual_data	;N: jp
	POP	BC
	PUSH	BC
	LD	A,C
	DEC	A
	DEC	A			;first write ?
	JP	NZ,write_actual_data	;N: jp
					;Y: zero buffer
	PUSH	HL
	LD	HL,(?@dirbuf)
	LD	D,A
zero_write_buffer:
	LD	(HL),A
	INC	HL
	INC	D
	JP	P,zero_write_buffer
	CALL	set_dma_to_dirbuf
	LD	HL,(@block_start_record)
	LD	C,002H			;first write
block_write_loop:
	LD	(@absolute_record_#),HL
	PUSH	BC
	CALL	set_dsk_trk_sec
	POP	BC
	CALL	write_record
	LD	HL,(@absolute_record_#)
	LD	C,0
	LD	A,(@blm)
	LD	B,A
	AND	L
	CP	B
	INC	HL
	JP	NZ,block_write_loop
	POP	HL
	LD	(@block_#),HL
	CALL	restore_dma
write_actual_data:
	CALL	set_dsk_trk_sec
	POP	BC
	PUSH	BC
	CALL	write_record
	POP	BC
	LD	A,(@current_sequ_rec)
	LD	HL,@ext_record_count
	CP	(HL)				;record in current extend ?
	JP	C,still_in_current_ext		;Y: ok
	LD	(HL),A				;N: set new ext record cnt
	INC	(HL)				;inc ext rec count
	LD	C,002H
still_in_current_ext:
	nop
	nop
	nop
	nop
	nop
	PUSH	AF
	CALL	get_s2
	AND	07FH
	LD	(HL),A
	POP	AF
	CP	07FH
	JP	NZ,jp_update_record_#s
	LD	A,(@sequential?)
	CP	001H
	JP	NZ,jp_update_record_#s
	CALL	update_record_#s
	CALL	open_next_extend
	LD	HL,@exit_value
	LD	A,(HL)
	OR	A
	JP	NZ,clear_exit_value
	DEC	A
	LD	(@current_sequ_rec),A
clear_exit_value:
	LD	(HL),0
jp_update_record_#s:
	JP	update_record_#s
;
trns_rnd_to_seq:
	XOR	A			;dont inc record #
	LD	(@sequential?),A
zero_trans_r_to_s:
	PUSH	BC
	LD	HL,(@entry_word)
	EX	DE,HL			;de = fcb_pointer
	LD	HL,offset_r0
	ADD	HL,DE			;hl = r0_pointer
	LD	A,(HL)			;get low byte of rnd rec #
	AND	07FH
	PUSH	AF			;save rnd rec #  mod  128
	LD	A,(HL)
	RLA
	INC	HL
	LD	A,(HL)
	RLA				;A = rnd rec #  div 128 (=ext #)
	AND	01FH
	LD	C,A			;C = extend #
	LD	A,(HL)			;A = high byte of rnd rec #
	RRA
	RRA
	RRA
	RRA
	AND	00FH
	LD	B,A			;B = rnd rec #  div  4096 (=32*128)
					;(max 32 extends with 128 records each)
	POP	AF			;A = rnd rec #  mod  128 (=curr rec #)
	INC	HL
	LD	L,(HL)			;L = overflow of rnd rec #
	INC	L
	DEC	L			;overflow <> 0 ?
	LD	L,006H			;seek past physical end of disk
	JP	NZ,rnd_seek_error	;Y: jp
	LD	HL,offset_cr
	ADD	HL,DE
	LD	(HL),A			;set current record #
	LD	HL,offset_ex
	ADD	HL,DE
	LD	A,C
	SUB	(HL)			;same extend # ?
	JP	NZ,different_extend	;N: jp
	LD	HL,offset_s2
	ADD	HL,DE
	LD	A,B
	SUB	(HL)
	AND	07FH
	JP	Z,correct_rnd_access
different_extend:
	PUSH	BC
	PUSH	DE
	CALL	.close_file
	POP	DE
	POP	BC
	LD	L,003H			;cannont close current extend
	LD	A,(@exit_value)
	INC	A
	JP	Z,bad_rnd_access
	LD	HL,offset_ex
	ADD	HL,DE
	LD	(HL),C
	LD	HL,offset_s2
	ADD	HL,DE
	LD	(HL),B
	CALL	.open_file
	LD	A,(@exit_value)
	INC	A
	JP	NZ,correct_rnd_access
	POP	BC
	PUSH	BC
	LD	L,004H			;seek to unwritten extend
	INC	C
	JP	Z,bad_rnd_access
	CALL	.make_file
	LD	L,005H			;cant create new extend
	LD	A,(@exit_value)
	INC	A
	JP	Z,bad_rnd_access
correct_rnd_access:
	POP	BC
	XOR	A
	JP	return_byte
;
bad_rnd_access:
	PUSH	HL
	CALL	get_s2
	LD	(HL),0C0H
	POP	HL
rnd_seek_error:
	POP	BC
	LD	A,L
	LD	(@exit_value),A
	JP	set_s2
;
.read_random:
	LD	C,0FFH			;read flag
	CALL	trns_rnd_to_seq
	CALL	Z,read_rnd_seq
	RET
;
.write_random:
	LD	C,0			;write flag
	CALL	trns_rnd_to_seq
	CALL	Z,write_rnd_seq
	RET
;
trans_seq_to_rnd:
	EX	DE,HL
	ADD	HL,DE
	LD	C,(HL)
	LD	B,0
	LD	HL,offset_ex
	ADD	HL,DE
	LD	A,(HL)
	RRCA
	AND	080H
	ADD	A,C
	LD	C,A
	LD	A,0
	ADC	A,B
	LD	B,A
	LD	A,(HL)
	RRCA
	AND	00FH
	ADD	A,B
	LD	B,A
	LD	HL,offset_s2
	ADD	HL,DE
	LD	A,(HL)
	ADD	A,A
	ADD	A,A
	ADD	A,A
	ADD	A,A
	PUSH	AF
	ADD	A,B
	LD	B,A
	PUSH	AF
	POP	HL
	LD	A,L
	POP	HL
	OR	L
	AND	001H
	RET
;
.compute_file_size:
	LD	C,00CH
	CALL	search_dir_first
	LD	HL,(@entry_word)
	LD	DE,offset_r0
	ADD	HL,DE
	PUSH	HL
	LD	(HL),D
	INC	HL
	LD	(HL),D
	INC	HL
	LD	(HL),D
compute_size_loop:
	CALL	all_entries_compared?
	JP	Z,pop_hl
	CALL	get_entry_pointer
	LD	DE,offset_rc
	CALL	trans_seq_to_rnd
	POP	HL
	PUSH	HL
	LD	E,A
	LD	A,C
	SUB	(HL)
	INC	HL
	LD	A,B
	SBC	A,(HL)
	INC	HL
	LD	A,E
	SBC	A,(HL)
	JP	C,not_greater
	LD	(HL),E
	DEC	HL
	LD	(HL),B
	DEC	HL
	LD	(HL),C
not_greater:
	CALL	search_dir_next
	JP	compute_size_loop
;
pop_hl:
	POP	HL
	RET
;
?set_random_record:
	LD	HL,(@entry_word)
	LD	DE,offset_cr
	CALL	trans_seq_to_rnd
	LD	HL,offset_r0
	ADD	HL,DE
	LD	(HL),C
	INC	HL
	LD	(HL),B
	INC	HL
	LD	(HL),A
	RET
;
login_current_disk:
	LD	HL,(@login_vec)
	LD	A,(@current_disk)
	LD	C,A
	CALL	shift_right_hl
	PUSH	HL
	EX	DE,HL
	CALL	physical_select
	POP	HL
	CALL	Z,select_error
	LD	A,L
	RRA
	RET	C
	LD	HL,(@login_vec)
	LD	C,L
	LD	B,H
	CALL	set_disk_vec
	LD	(@login_vec),HL
	JP	logical_select
;
?select_disk:
	LD	A,(@entry_byte)
	LD	HL,@current_disk
	CP	(HL)
	RET	Z
	LD	(HL),A
	JP	login_current_disk
;
auto_select:
	LD	A,0FFH
	LD	(@auto_selected?),A
	LD	HL,(@entry_word)
	LD	A,(HL)
	AND	01FH
	DEC	A
	LD	(@entry_byte),A
	CP	01EH			;match any entry ? (dr='?')
	JP	NC,no_auto_slct
	LD	A,(@current_disk)
	LD	(@default_disk),A
	LD	A,(HL)
	LD	(@fcb_disk_#),A
	AND	0E0H
	LD	(HL),A
	CALL	?select_disk
no_auto_slct:
	LD	A,(@user_#)
	LD	HL,(@entry_word)
	OR	(HL)
	LD	(HL),A
	RET
;
?return_version_#:
	LD	A,version_#
	JP	return_byte
;
?reset_disk_system:
	LD	HL,0
	LD	(@ro_vec),HL
	LD	(@login_vec),HL
	XOR	A
	call	bios+042h			;LD	(@current_disk),A
	LD	HL,?disk_buffer
	LD	(@bdos_dma_address),HL
	CALL	restore_dma
	JP	login_current_disk
;
?open_file:
	CALL	zero_s2
	CALL	auto_select
	JP	.open_file
;
?close_file:
	CALL	auto_select
	jp	bios+033h		;JP	.close_file
;
?search_first:
	LD	C,0
	EX	DE,HL
	LD	A,(HL)
	CP	03fh
	JP	Z,search_now
	CALL	get_extend_pointer
	LD	A,(HL)
	CP	03fh
	CALL	NZ,zero_s2
	CALL	auto_select
	LD	C,15
search_now:
	CALL	search_dir_first
	JP	move_dirbuf_to_dma
;
?search_next:
	LD	HL,(?@search_fcb)
	LD	(@entry_word),HL
	CALL	auto_select
	CALL	search_dir_next
	JP	move_dirbuf_to_dma
;
?delete_file:
	CALL	auto_select
	CALL	erase_files
	JP	return_found_flag
;
?read_sequ:
	CALL	auto_select
	JP	.read_sequ
;
?write_sequ:
	CALL	auto_select
	JP	.write_sequ
;
?make_file:
	CALL	zero_s2
	CALL	auto_select
	JP	.make_file
;
?rename_file:
	CALL	auto_select
	CALL	.rename_file
	JP	return_found_flag
;
?return_login_vec:
	LD	HL,(@login_vec)
	JP	return_word
;
?return_current_disk:
	LD	A,(@current_disk)
	JP	return_byte
;
?set_dma:
	EX	DE,HL
	LD	(@bdos_dma_address),HL
	JP	restore_dma
;
?get_allocation_addr:
	LD	HL,(?@alv)
	JP	return_word
;
?get_ro_vec:
	LD	HL,(@ro_vec)
	JP	return_word
;
?set_file_attr:
	CALL	auto_select
	CALL	.set_file_attr
	JP	return_found_flag
;
?get_parameter_addr:
	LD	HL,(?@dpb)
;
return_word:
	LD	(@exit_value),HL
	RET
;
?set_get_user_code:
	LD	A,(@entry_byte)
	CP	0FFH
	JP	NZ,set_user_code
	LD	A,(@user_#)
	JP	return_byte
;
set_user_code:
	AND	01FH
	jp	bios+036h			;LD	(@user_#),A
	RET
;
?read_random:
	CALL	auto_select
	JP	.read_random
;
?write_random:
	CALL	auto_select
	JP	.write_random
;
?compute_file_size:
	CALL	auto_select
	JP	.compute_file_size
;
?reset_disk:
	LD	HL,(@entry_word)
	LD	A,L
	CPL
	LD	E,A
	LD	A,H
	CPL
	LD	HL,(@login_vec)
	AND	H
	LD	D,A
	LD	A,L
	AND	E
	LD	E,A
	LD	HL,(@ro_vec)
	EX	DE,HL
	LD	(@login_vec),HL
	LD	A,L
	AND	E
	LD	L,A
	LD	A,H
	AND	D
	LD	H,A
	LD	(@ro_vec),HL
	RET
;
?bdos_exit:
	LD	A,(@auto_selected?)
	OR	A
	JP	Z,ret_to_user
	LD	HL,(@entry_word)
	LD	(HL),0
	LD	A,(@fcb_disk_#)
	OR	A
	JP	Z,ret_to_user
	LD	(HL),A
	LD	A,(@default_disk)
	LD	(@entry_byte),A
	CALL	?select_disk
ret_to_user:
	LD	HL,(@user_stack)
	LD	SP,HL
	LD	HL,(@exit_value)
	LD	A,L
	LD	B,H
	RET
;
?write_zero_random:
	CALL	auto_select
	LD	A,002H			;set zero fill flag
	LD	(@sequential?),A
	LD	C,0
	CALL	zero_trans_r_to_s
	CALL	Z,write_rnd_seq
	RET
;
@make_fcb:		defb	0e5h
@ro_vec:		defw	0
@login_vec:		defw	0
@bdos_dma_address:	defw	?disk_buffer
?@end_of_entries:	defw	0
?@track_#:		defw	0
?@track_record_#:	defw	0
?@dirbuf:		defw	0
?@dpb:			defw	0
?@csv:			defw	0
?@alv:			defw	0
@spt:			defw	0
@bsh:			defb	0
@blm:			defb	0
@exm:			defb	0
@dsm:			defw	0
@drm:			defw	0
@al0_al1:		defw	0
@cks:			defw	0
@off:			defw	0
?@xltab:		defw	0
@entry_closed?:		defb	0
@read_or_write?:	defb	0
@file_found?:		defb	0
@sequential?:		defb	0
@entry_byte:		defb	0
@block_offset:		defb	0
@compare_count:		defb	0
?@search_fcb:		defw	0
			defw	0
@large_dsm?:		defb	0
@auto_selected?:	defb	0
@default_disk:		defb	0
@fcb_disk_#:		defb	0
@ext_record_count:	defb	0
@current_extend_#:	defb	0
@current_sequ_rec:	defw	0
@block_#:
@absolute_record_#:	defw	0
@block_start_record:	defw	0
@dir_offset:		defb	0
@dir_entry_#:		defw	0
@dir_record_#:		defw	0
			defs	18,0

bios:

*-----------------------------------------------------------
* Title         : Knights of Valour protection simulation
* Written by    : IQ_132
* Date          : 2023
* Description   : Simulate the protection routines required by this game in 68k assembly
*-----------------------------------------------------------
    ORG    $4f1000
START:                  ; first instruction of program

COMMAND_WORD		equ		$4f0ff8       		; 2 bytes
DATA_WORD		    equ	    $4f0ffc       		; 2 bytes

RESPONSE_VALUE 		equ		$880000

_unused_ram         equ     $81e000
_unused_ram2        equ     $81e080

_slots			    equ	    _unused_ram	        ; 40 bytes
_response		    equ	    _unused_ram2+$0     ; 4 bytes
_key                equ     _unused_ram2+$4     ; 4 bytes
_slot_select		equ	    _unused_ram2+$8     ; 2 bytes
_sim_value		    equ	    _unused_ram2+$a		; 2 bytes

_background_select	equ		_unused_ram2+$c		; 2 bytes
_text_select		equ		_unused_ram2+$e		; 2 bytes
_damage_level		equ		_unused_ram2+$10	; 2 bytes


_protection_routine
    movem.l     D0-D2/A0, -(A7)

	movea.l		COMMAND_WORD, A0
	move.w		(A0), D0

    move.w      D0, D1
    lsr.w       #$8, D1
    cmp.b       #$ff, D1
    bne.s       _skip_calc_key_ff
    	move.l      #$ffffffff, _key
_skip_calc_key_ff
    move.w      _key, D1

    ; apply key to command
    eor.b       D1, D0
    andi.w      #$ff, D0

    ; apply key to sim value
	movea.l		DATA_WORD, A0
	move.w		(A0), D2
	eor.w       D1, D2
	move.w      D2, _sim_value

	move.l		#RESPONSE_VALUE, _response

	; kov
	cmp.b		#$99, D0
	beq			_99_routine
	cmp.b		#$9d, D0
	beq			_9d_routine
	cmp.b		#$b0, D0
	beq			_b0_routine	
	cmp.b		#$b4, D0
	beq			_b4_routine	
	cmp.b		#$ba, D0
	beq			_ba_routine
	cmp.b		#$c0, D0
	beq			_c0_routine
	cmp.b		#$c3, D0
	beq			_c3_routine
	cmp.b		#$cb, D0
	beq			_cb_routine
	cmp.b		#$cc, D0
	beq			_cc_routine
	cmp.b		#$d0, D0
	beq			_d0_routine
	cmp.b		#$dc, D0
	beq			_dc_routine
	cmp.b		#$d6, D0
	beq			_d6_routine
	cmp.b		#$e0, D0
	beq			_9d_routine
	cmp.b       #$e5, D0
	beq         _e5_routine	
	cmp.b		#$e7, D0
	beq		    _e7_routine
	cmp.b		#$f0, D0
	beq			_f0_routine
	cmp.b		#$f8, D0
	beq		    _f8_routine
	cmp.b		#$fe, D0
	beq			_fe_routine
	cmp.b		#$fc, D0
	beq			_fc_routine
	; kovplus
	cmp.b		#$c5, D0
	beq			_c5_routine
	; kovsgqyz
	cmp.b		#$b7, D0
	beq			_b4_routine
	cmp.b		#$cd, D0
	beq			_d0_routine
	cmp.b		#$11, D0
	beq			_dc_routine
	cmp.b		#$9e, D0
	beq			_9d_routine
	cmp.b		#$ab, D0
	beq			_f8_routine

	; fall through if not found...
_end_protection_routine
    moveq.l     #$0, D0    ; update key!
    moveq.l     #$0, D1
    move.b      _key, D0
    add.w       #$1, D0
    andi.w      #$ff, D0
    cmpi.b      #$ff, D0
    bne.s       _skip_reset_key
    moveq.l     #$1, D0
_skip_reset_key
    move.b      D0, D1
    lsl.w       #$8, D0
    move.b      D1, D0
    move.w      D0, D1
    swap        D0
    move.w      D1, D0
    move.l      D0, _key
    move.l      _response, D1 ; apply key to response!
    eor.l       D0, D1
    move.l      D1, _response
    movem.l     (A7)+, D0-D2/A0
	rts


_99_routine
    move.l      #$0, _key
	bra.s       _end_protection_routine


_9d_routine ; & e0
	move.l		#$a00000, D0
	move.w		_sim_value, D0
	andi.w		#$1f, D0
	lsl.w		#$6, D0
	move.l		D0, _response
	bra	        _end_protection_routine


_b0_routine
	lea.l		B0_TABLE, A0
	move.w		_sim_value, D0
	andi.l		#$7, D0
	move.b		(A0,D0.w), D0
	move.l		D0, _response
	bra			_end_protection_routine


_b4_routine
	lea.l		_slots, A0
	move.w		_sim_value, D0
	move.w		D0, D1
	move.w		D0, D2
	andi.w		#$f, D0		; sim_value & f
	andi.w		#$f0, D1	; sim_value >> 4) & f
	andi.w		#$f00, D2	; sim_value >> 8) & f
	lsl.w		#$2, D0
	lsr.w		#$2, D1
	lsr.w		#$6, D2
	move.l		(A0,D0.w), D0
	move.l		(A0,D1.w), D1
	add.l		D1, D0
	andi.l      #$ffffff, D0
	move.l		D0, (A0,D2)
	bra         _end_protection_routine


_ba_routine
	lea.l		BA_TABLE, A0
	move.w		_sim_value, D0
	andi.l		#$3f, D0
	move.b		(A0,D0.w), D0
	move.l		D0, _response
	bra			_end_protection_routine


_c0_routine
	move.w		_sim_value, _text_select
	bra			_end_protection_routine


_c3_routine
	move.l		#$904000, D0
	moveq.l		#$0, D1
	move.w		_sim_value, D1
	lsl.l		#$8, D1
	add.l		D1, D0
	moveq.l		#$0, D1
	move.w		_text_select, D1
	lsl.l		#$2, D1
	add.l		D1, D0
	move.l		D0, _response
	bra			_end_protection_routine


_c5_routine ; kovplus
	move.w		_sim_value, D0
	andi.w		#$f, D0
	lsl.w		#$2, D0
	lea.l		_slots, A0
	move.l		(A0,D0.w), D1
	subq.l		#$1, D1
	move.l		D1, (A0,D0.w)
	bra         _end_protection_routine


_cb_routine
	move.w		_sim_value, _background_select
	bra			_end_protection_routine


_cc_routine
	move.l		#$900000, D0
	move.w		_sim_value, D0
	andi.w		#$f, D0
	lsl.w		#$8, D0
	move.w		_background_select, D1
	lsl.w		#$2, D1
	add.w		D1, D0
	move.l		D0, _response
	bra		_end_protection_routine


_d0_routine
	move.l		#$a01000, D0
	moveq.l		#$0, D1
	move.w		_sim_value, D1
	lsl.w		#$5, D1
	add.w		D1, D0
	move.l		D0, _response
	bra			_end_protection_routine


_d6_routine
	move.w		_sim_value, D0
	andi.w		#$f, D0
	lsl.w		#$2, D0
	lea.l		_slots, A0
	move.l		(A0,D0.w), D1
	addq.l		#$1, D1
	move.l		D1, (A0,D0.w)
	bra         _end_protection_routine


_dc_routine
	move.l		#$a00800, D0
	moveq.l		#$0, D1
	move.w		_sim_value, D1
	lsl.w		#$6, D1
	add.w		D1, D0
	move.l		D0, _response
	bra			_end_protection_routine


_e5_routine
	move.w		_slot_select, D0
	lea.l		_slots, A0
	move.l		(A0,D0.w), D1
	move.w		_sim_value, D1
	move.l		D1, (A0,D0.w)
	bra         _end_protection_routine


_e7_routine
	move.w		_sim_value, D0
	andi.w      #$f000, D0
	rol.w		#$6, D0
	move.w		D0, _slot_select
	lea.l		_slots, A0
	move.l		(A0,D0.w), D1
	swap		D1
	move.w		_sim_value, D1
	andi.w		#$ff, D1
	swap		D1
	move.l		D1, (A0,D0.w)
	bra         _end_protection_routine


_f8_routine
	move.w		_sim_value, D0
	andi.w		#$f, D0
	lsl.w		#$2, D0
	lea.l		_slots, A0
	move.l		(A0,D0.w), _response
	bra         _end_protection_routine


_f0_routine
	move.l		#$c000, _response
	bra			_end_protection_routine


_fc_routine
	moveq.l		#$0, D0
	moveq.l		#$1, D1
	move.w		_sim_value, D0
	move.w		_damage_level, D1
	mulu.w		D1, D0	; word multiply seems good enough
	lsr.w		#$6, D0
	move.l		D0, _response
	bra			_end_protection_routine


_fe_routine
	move.w		_sim_value, _damage_level
	bra			_end_protection_routine


B0_TABLE
	dc.b	$2, $0, $1, $4, $3, $ff, $ff, $ff

BA_TABLE
	dc.b	$00, $29, $2c, $35, $3a, $41, $4a, $4e, $57, $5e, $77, $79, $7a, $7b, $7c, $7d
	dc.b	$7e, $7f, $82, $81, $84, $85, $86, $87, $88, $89, $8a, $8b, $8c, $8d, $8e, $90
	dc.b	$95, $96, $97, $98, $99, $9a, $9b, $9c, $9e, $a3, $d4, $a9, $af, $b5, $bb, $c1

    END    START        ; last line of source



*~Font name~Courier New~
*~Font size~18~
*~Tab type~1~
*~Tab size~4~

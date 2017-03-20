;---------------------------------------------------------------------------
; CARILLON PLAYER Header file for GBDK
; (c)2000-2001 Aleksi Eeben (email: aleksi@cncd.fi)
; Ported to GBDK by nitro2k01 (http://gbdev.gg8.se/forums/)
;---------------------------------------------------------------------------
        
		.globl	_CP_Mute_Chan
		.globl	_CP_Reset_Chan
.tts	=	200
		
.area	_CODE_1

;MILLS:
;As carillon does not have any functions to mute channels, 
;I figured out this "thing" that writes to the ram (0xc7c0)used by carillon...
;it just works...

_CP_Mute_Chan:	

	LDA	HL,2(SP)
	LD	A,(HL)
	CP	#0
	JR	Z,MCH1
	CP	#1
	JR	Z,MCH2
	CP	#2
	JR	Z,MCH3
	CP	#3
	JR	Z,MCH4
	;////////////////////////////////
MCH1:
	LD	HL,#0xc7c7	;BASE+7
	LD	A,#0x00
	LD  (HL),A		;CHAN 1 OFF
	ret
MCH2:
	LD	HL,#0xc7cD	;BASE+D
	LD	A,#0x00
	LD  (HL),A		;CHAN 2 OFF
	ret
MCH3:
	LD	HL,#0xc7d3	;BASE+13
	LD	A,#0x00
	LD  (HL),A		;CHAN 3 OFF
	ret
MCH4:
	LD	HL,#0xc7da	;BASE+1A
	LD  (HL),A		;CHAN 4 OFF
	ret

;MILLS:
;This just resets all parameters after playing a SFX, 
;so that carillon keeps playing
_CP_Reset_Chan:
	LDA	HL,2(SP)
	LD	A,(HL)
	CP	#0
	JR	Z,CH1
	CP	#1
	JR	Z,CH2
	CP	#2
	JR	Z,CH3
	CP	#3
	JR	Z,CH4
	;////////////////////////////////
CH1:    ;SOUND REG
	LD	A,#0x00
	LD  (#0xFF10),A		;CHAN 1 RESET
	LD  (#0xFF11),A	
	LD  (#0xFF12),A	
	LD  (#0xFF13),A	
	LD  (#0xFF14),A	
	ret
CH2:
	LD	A,#0x00
	LD  (#0xFF16),A		;CHAN 2 RESET
	LD  (#0xFF17),A	
	LD  (#0xFF18),A	
	LD  (#0xFF19),A	
	ret
CH3:
	LD	A,#0x00
	LD  (#0xFF1A),A		;CHAN 3 RESET
	LD  (#0xFF1B),A	
	LD  (#0xFF1C),A	
	LD  (#0xFF1D),A	
	LD  (#0xFF1E),A
	ret
CH4:
	LD	A,#0x00
	LD  (#0xFF20),A		;CHAN 4 RESET
	LD  (#0xFF21),A	
	LD  (#0xFF22),A	
	LD  (#0xFF23),A	
	ret
	
	
	
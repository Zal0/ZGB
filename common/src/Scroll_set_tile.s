;--------------------------------------
; SET TILE asm function for ZGB ENGINE
; Mills
;--------------------------------------

	.globl 	_ZGB_SET_TILE
	
.area	_CODE_1
	
_ZGB_SET_TILE:	
	
	LDA	HL,3(SP)	; Skip return address
	DEC HL
	LD	D,#0
	LD	B,#0
	LD	E,(HL)		; A = First	argument(X)
	;LD	E,A			; E = X
	LD	A,(HL+)		; A = Second argument (Y)
	; Multiply Y*32
	ADD	A,A			; Y*2 
	ADD	A,A			; Y*4
	ADD	A,A			; Y*8
	LD	C,A			; BC = Y
	LD	A,(HL+)		; A = Third argument (TILE)
	LD  HL,#0x9800	; MAP address to HL
	ADD	HL,BC		; Add BC to 9800
	ADD	HL,BC		; Add BC 
	ADD	HL,BC		; Add BC 
	ADD	HL,BC		; Add BC = + 32*Y
	ADD	HL,DE		; Add DE to 9800 = + 32*Y+X
	; 	
	LD  (HL),A		; Tile Number to map address
	
	RET


	

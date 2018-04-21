	case on

dummy	start
	end

tables	privdata

;
; each line is 160 pixels.
; top/bottom margin is 4 lines (4*160 pixels)

rows	anop
	dc i2'0*8*160+4*160+$2000'
	dc i2'1*8*160+4*160+$2000'
	dc i2'2*8*160+4*160+$2000'
	dc i2'3*8*160+4*160+$2000'
	dc i2'4*8*160+4*160+$2000'
	dc i2'5*8*160+4*160+$2000'
	dc i2'6*8*160+4*160+$2000'
	dc i2'7*8*160+4*160+$2000'
	dc i2'8*8*160+4*160+$2000'
	dc i2'9*8*160+4*160+$2000'
	dc i2'10*8*160+4*160+$2000'
	dc i2'11*8*160+4*160+$2000'
	dc i2'12*8*160+4*160+$2000'
	dc i2'13*8*160+4*160+$2000'
	dc i2'14*8*160+4*160+$2000'
	dc i2'15*8*160+4*160+$2000'
	dc i2'16*8*160+4*160+$2000'
	dc i2'17*8*160+4*160+$2000'
	dc i2'18*8*160+4*160+$2000'
	dc i2'19*8*160+4*160+$2000'
	dc i2'20*8*160+4*160+$2000'
	dc i2'21*8*160+4*160+$2000'
	dc i2'22*8*160+4*160+$2000'
	dc i2'23*8*160+4*160+$2000'
	dc i2'24*8*160+4*160+$2000'

;
; columns are simply x * 2
;



	end
;
;ReverseScrollRegion(line1, line2)
;
ReverseScrollRegion	START
line2	equ 9
line1	equ 7
_rtlb	equ 3
_d	equ 1

	using tables

;	brk $ea

	phb
	phd
	tsc
	tcd

	lda <line1
	bmi exit

	lda <line2
	cmp #25
	bcs exit

	lda <line1
	cmp <line2
	bcs exit

	asl line1
	asl line2

; count = (line2 - line1) * 8 * 160 - 1

	ldx <line2
	lda >rows,x
	clc ; dec a 
	ldx <line1
	sbc >rows,x
	pha


;
; mvn:
; while (--a != -1)
; 	dest[y++] = dest[x++];
;
; mvp:
; while(--a != -1)
;  dest[y--] = dest[x--];



; y = dest address = $2000 + 640 + (line2 + 1) * 8 * 160 -1
; x = src address = dest - 160*8


	ldx <line2
	lda >rows,x
	dec a
	tay
	sec
	sbc #160*8
	tax

	pla			; length

	mvp $e10000,$e10000

;
; now empty first line
;
	ldx <line1
	jsr _ClearLine

exit	anop

	lda <_rtlb
	sta <line1
	lda <_rtlb+2
	sta <line2
	
	pld
	pla
	pla
	plb
	rtl


	END


;
; ScrollRegion(line1, line2);
;
ScrollRegion	START

	using tables

line2	equ 9
line1	equ 7
_rtlb	equ 3
_d	equ 1

;	brk $ea

	phb
	phd
	tsc
	tcd

	lda <line1
	bmi exit

	lda <line2
	cmp #25
	bcs exit

	lda <line1
	cmp <line2
	bcs exit

	asl line1
	asl line2

; a = (line2 - line1) * 8 * 160 - 1

	ldx <line2
	lda >rows,x
	clc ; dec a 
	ldx <line1
	sbc >rows,x
	pha


; src address = $2000 + line1


;	ldx #$e12000+1920	; src address
;	ldy #$e12000+640	; dest address

; dest = $2000 + 640 + line1 * 8 * 160
; src = dest + 1280

	ldx <line1
	lda >rows,x
	tay
	clc
	adc #8*160
	tax

	pla			; length

	mvn $e10000,$e10000

;
; now empty last line
;
	ldx line2
	dex
	dex
	jsr _ClearLine

exit	anop

	lda <_rtlb
	sta <line1
	lda <_rtlb+2
	sta <line2
	
	pld
	pla
	pla
	plb
	rtl


	END

;
; scroll 1 line
;
;
ScrollDown START

	using tables

	phb
;
; mvn:
; while (--a != -1)
; 	dest[y++] = dest[x++];
;
; mvp:
; while(--a != -1)
;  dest[y--] = dest[x--];

	lda #29440-1		; length -1 ->23*8*160
	ldx #$e12000+1920	; src address
	ldy #$e12000+640	; dest address

;	ldx #$2000+1280
;	ldy #$2000

	mvn $e10000,$e10000

;
; now empty outline #24
;
	ldx #23
	jsr _ClearLine


	plb
	rtl
	
	END


; clear line in X (which has been pre-multiplied)
_ClearLine	PRIVATE

	using tables

	lda >rows,x
	tax
	lda #0
	sta >$e10000,x	; stick a 0 in there
	txy		; destination address (src + 2)
	iny
	iny	

	lda #8*160-3	; length -1
	mvn $e10000,$e10000	
	rts
	END
;
; ClearLine(int line_no); clears the line
;
ClearLine	START

line	equ 7
_rtlb	equ 3
_d	equ 1


;	brk $ea
	phb

	phd
	tsc
	tcd

	lda <line
	cmp #24
	bcs exit

	asl a
	tax
	jsr _ClearLine

exit	anop

	lda <_rtlb+2
	sta <_rtlb+4
	lda <_rtlb
	sta <_rtlb+2

	pld
	pla
	plb
	rtl

	END


;
; ClearScreen2(int start, int end);
;
ClearScreen2	START

	using tables

line2	equ 9
line1	equ 7
_rtlb	equ 3
_d	equ 1



;	brk $ea
	phb

	phd
	tsc
	tcd

	lda <line1
	bmi exit

	lda line2
	cmp #25
	bcs exit

	lda <line1
	cmp <line2
	bcs exit

	asl line1
	asl line2

	ldx <line2
	lda >rows,x
	clc
	ldx <line1
	sbc >rows,x
	dec a
	dec a
	pha ; save length


	ldx <line2
	lda >rows,x
	tax
	tay
	iny
	iny

	lda #0
	sta >$e1000,x
	pla ; restore length

	mvn $e10000,$e10000


exit	anop
	lda <_rtlb+2
	sta <line2
	lda <_rtlb
	sta <line1

	pld
	pla
	pla
	plb
	rtl

	END

;
; ClearLine2(int line, int start, int end);
;

;
; clear a section of a given line.
;
ClearLine2	START

	using tables

end	equ	11
start	equ	9
line	equ	7
_rtlb	equ	3
_d	equ	1

	phb
	phd
	tsc
	tcd

	pea $e1e1
	plb
	plb


	lda start
	bmi exit

	lda end
	cmp #81
	bcs exit

	lda start
	cmp end
	bcs exit

	sec
	lda end
	sbc start
;	bmi exit
;	beq exit
	tay ; counter

	asl start ; x 2
	asl line

	ldx <line
	lda >rows,x

	clc
	adc start
	tax


loop	anop
	
	stz |0,x
	stz |160,x
	stz |320,x
	stz |480,x
	stz |640,x
	stz |800,x
	stz |960,x
	stz |1120,x
	inx
	inx
	dey
	bne loop

exit	anop

	lda <_rtlb+2
	sta <end
	lda <_rtlb
	sta <start

	pld
	pla
	pla
	pla

	plb
	rtl
	END


;
; void DrawCursor(int Xpos, int Ypos, int state);
;
;
DrawCursor	START


state	equ	13
Ypos	equ	11
Xpos	equ	9
_rtlb	equ	5
tmp	equ	3
_d	equ	1

	phb
	pha		;tmp
	phd
	tsc
	tcd


	lda <Ypos
	and #$00ff
	xba		; x 256
	sta <tmp
	asl a		; x 512
	asl a		; x 1024
	clc
	adc <tmp	; a = Y * 1280
	
	adc #11*160	; last row of it
	sta <tmp
	adc <Xpos	; + X * 2
	adc <Xpos
	tax

	lda #$ffff	; assume to draw it
	ldy <state
	bne draw
        lda #0		; nope!
	
draw	anop

	sta >$e12000,x



	pld
	pla		;tmp
	pla
	sta 5,s
	pla
	sta 5,s
	pla
	plb
	rtl

	END



;
; clear the screen to black (or whatnot)
;

ClearScreen	START

	phb
	pea $e1e1
	plb
	plb
	lda #0
	ldy #32000
loop	anop
	sta |$2000-2,y
	dey
	dey
	bne loop
	plb
	rtl

	END
;
; draw a character at the current Xpos & Ypos
;
;
; PrintChar(x, y, char, andMask, xorMask)
PrintChar	START

	using tables

xorMask	equ 15
andMask	equ 13
char	equ 11
yy	equ 9
xx	equ 7
_rtlb	equ 3
_d	equ 1

;	brk $ea
	phb

	phd
	tsc
	tcd

; sanity check on Xpos && Ypos

	ldx xx
	cpx #80
	bcc ok2

exit2	anop
	brl exit

ok2	anop
	ldy yy
	cpy #24
	bcs exit2

	lda <char
	cmp #$7f
	bcs space
	sec
	sbc #$20
	bmi space
	asl a		; x2
	asl a		; x4
	asl a		; x8
	asl a		; x16
	sta char
	bra ok


space	anop
	stz <char

ok	anop

	pea $e1e1
	plb
	plb

	asl xx
	asl yy

	ldx yy
	lda >rows,x
	clc
	adc xx
	tay 


;
; $e12000 + pos = location to start drawing the character
;
	
;
; each char takes up 16 bytes :. the offset is (char - $20) * 16
;

	ldx char
	lda >CHAR_SPACE,x
	eor <xorMask
	and <andMask
	sta |0,y	; blast it to the screen
	
	lda >CHAR_SPACE+2,x
	eor <xorMask
	and <andMask
	sta |160,y      
	
	lda >CHAR_SPACE+4,x
	eor <xorMask
	and <andMask
	sta |320,y      

	lda >CHAR_SPACE+6,x
	eor <xorMask
	and <andMask
	sta |480,y      

	lda >CHAR_SPACE+8,x
	eor <xorMask
	and <andMask
	sta |640,y      

	lda >CHAR_SPACE+10,x
	eor <xorMask
	and <andMask
	sta |800,y      

	lda >CHAR_SPACE+12,x
	eor <xorMask
	and <andMask
	sta |960,y      

	lda >CHAR_SPACE+14,x
	eor <xorMask
	and <andMask
	sta |1120,y     

exit	anop

	lda <_rtlb+2
	sta <xorMask
	lda <_rtlb
	sta <andMask
	pld
   	pla
	pla
	pla
	pla
	pla
	plb
	rtl
	end
	
;Cursor	START

;StartCursor	ENTRY

;	pha
;;	~ReadBParam #$2f
;	pla
;	sta blinkRate

;	stz savedX
;	stz savedY

;;	~SetHeartBeat #CursorHB
;	rtl


;StopCursor	ENTRY
;;	~DelHeartBeat #CursorHB
;	rtl

;notSafe		ENTRY
;		ds 2
;blinkRate	ds 2
;savedX		ds 2
;savedY		ds 2

;CursorHB	anop
;	dc i4'0'
;count	dc i2'30'
;	dc h'5AA5'

;	longa off

;	phb
;	phk
;	plb
;	php

;;	long ai
;	lda notSafe
;	bne exit

;	lda Xpos
;	cmp savedX
;	bne moved
;	lda Ypos
;	cmp savedY
;	bne moved
;	bra exit

;moved	jsr drawCursor


;exit	anop

;	plp
;	plb
;	clc
;	rtl

;drawCursor	anop
	
;	rts

;	END

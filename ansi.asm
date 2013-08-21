	case on

dummy	start
	end

;
;ReverseScrollRegion(line1, line2)
;
ReverseScrollRegion	START
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
	cmp #0
	bcc exit

	lda <line2
	cmp #24
	bcs exit

	lda <line1
	cmp <line2
	bcs exit

; a = (line2 - line1) * 8 * 160 - 1

	lda <line2
	sec
	sbc <line1
; a = a * 256 * 5
	xba	; a = a * 256
; 5x = 4x + x

	pha ; save
	asl a ; x2
	asl a ; x4
	clc
	adc 1,s
	sta 1,s
	pla
	dec a	; a = a - 1

	pha	; save

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


	lda <line2
	inc a
	xba	; x256
	pha	; save
	asl a	; x 2
	asl a	; x 4
	clc
	adc #$2000+4*160
	adc 1,s
	dec a			; - 1
	sta 1,s			;store it...
	sec
	sbc #1280
	tax			; src address
	ply			; dest address
	pla			; length

	mvp $e10000,$e10000

;
; now empty first line
;
	pei (line1)
	jsl ClearLine

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
	cmp #0
	bcc exit

	lda <line2
	cmp #24
	bcs exit

	lda <line1
	cmp <line2
	bcs exit

; a = (line2 - line1) * 8 * 160 - 1

	lda <line2
	sec
	sbc <line1
; a = a * 256 * 5
	xba	; a = a * 256
; 5x = 4x + x

	pha ; save
	asl a ; x2
	asl a ; x4
	clc
	adc 1,s
	sta 1,s
	pla
	dec a	; a = a - 1

	pha	; save

; src address = $2000 + line1


;	ldx #$e12000+1920	; src address
;	ldy #$e12000+640	; dest address

; dest = $2000 + 640 + line1 * 8 * 160
; src = dest + 1280

	lda <line1
	xba	; x256
	pha	; save
	asl a	; x 2
	asl a	; x 4
	clc
	adc #$2000+4*160
	adc 1,s
	sta 1,s			;store it...
	adc #1280
	tax			; src address
	ply			; dest address
	pla			; length

	mvn $e10000,$e10000

;
; now empty last line
;
	pei (line2)
	jsl ClearLine

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
	pea 23
	jsl ClearLine


	plb
	rtl
	
	END

;
; ClearLine(int line_no); clears the line
;
ClearLine	START

line	equ 9
_rtlb	equ 5
tmp	equ 3
_d	equ 1


;	brk $ea
	phb
	pha

	phd
	tsc
	tcd

; line * 8 * 160 + 4*160
; = line * 1024 + line * 256 + 4*160
	lda <line

	and #$00ff
	xba		; x256
	sta <tmp
	asl a		; x 512
	asl a		; x 1024
	clc
	adc <tmp
	sta <tmp
                             
	clc
	adc #4*160
	sta <tmp
	clc
	adc #$2000	; graphics begin @ $e12000

	tax
	lda #0
	sta >$e10000,x	; stick a 0 in there
	txy		; destination address (src + 2)
	iny
	iny	


	lda #8*160-3	; length -1
	mvn $e10000,$e10000

	lda <_rtlb+2
	sta <_rtlb+4
	lda <_rtlb
	sta <_rtlb+2

	pld
	pla
	pla
	plb
	rtl

	END


;extern void ClearScreenFrom(int Y);
;
;
;
ClearScreenFrom	START

Y	equ 9
_rtlb	equ 5
tmp	equ 3
_d	equ 1


;	brk $ea
	phb
	pha

	phd
	tsc
	tcd

; line * 8 * 160 + 4*160
; = line * 1024 + line * 256 + 4*160
	lda <Y

	and #$00ff
	xba		; x256
	sta <tmp
	asl a		; x 512
	asl a		; x 1024
	clc
	adc <tmp
	sta <tmp

	clc
	adc #4*160
	sta <tmp
	clc
	adc #$2000	; graphics begin @ $e12000

	tax
	lda #0
	sta >$e10000,x	; stick a 0 in there
	txy		; destination address (src + 2)
	iny
	iny


	phy
	lda #$9d00-2	; end of graphics + $2000
	sec
	sbc 1,s
	ply

; a now equals length to clear

	mvn $e10000,$e10000

	lda <_rtlb+2
	sta <_rtlb+4
	lda <_rtlb
	sta <_rtlb+2

	pld
	pla
	pla
	plb
	rtl

	END

;
;extern void ClearLineFrom(int Y, int X);
;

;
; clear the end of the line from a given x & y
;
ClearLineFrom	START

X	equ	11
Y	equ	9
_rtlb	equ	5
tmp	equ	3
_d	equ	1

	phb
	pha
	phd
	tsc
	tcd

	pea $e1e1
	plb
	plb


;
; if X is >= 80, skip it...
;
	lda <X
	cmp #80
	bcs exit

	lda <Y
	and #$00ff
	xba		; x 256
	sta <tmp
	asl a		; x 512
	asl a		; x 1024
	clc
	adc <tmp	; Y * 8 * 160
	clc
	adc #160*4
	sta <tmp

; each char takes up 2 bytes
	lda <X
	asl a		; x 2
	clc
	adc <tmp
	sta <tmp

	tax

	lda #80
	sec
	sbc <X
loop	anop
	
	dec a
	bmi exit

	stz $2000,x
	stz $2000+160,x
	stz $2000+320,x
	stz $2000+480,x
	stz $2000+640,x
	stz $2000+800,x
	stz $2000+960,x
	stz $2000+1120,x
	inx
	inx
	bra loop

exit	anop

	pld
	pla
	pla
	sta 3,s
	pla
	sta 3,s
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
	ldy #0
loop	anop
	sta $2000,y
	iny
	iny
	cpy #32000
	bcc loop
	plb
	rtl

;
; this will, in 1 atomic action, blast the screen with 0
;

	phb
	lda #0
	sta >$e12000
	lda #32000-3	;length
	ldx #$2000
	ldy #$2002
	mvn $e10000,$e10000

	plb
	rtl
	END
;
; draw a character at the current Xpos & Ypos
;
;

PrintChar	START

andMask	equ 11
char	equ 9
_rtlb	equ 5
pos	equ 3
_d	equ 1

;	brk $ea
	phb
	pha

	phd
	tsc
	tcd

; sanity check on Xpos && Ypos
	lda Xpos
	cmp #80
	bcc _ok
	lda Ypos
	cmp #24
	bcc _ok
	brl exit
_ok	anop


	pea $e1e1
	plb
	plb

;
; each line has a width of 160 pixels
;
; start drawing the letter at $e12000 + (Ypos x 160 x 8) + Xpos


; 160 * 8 = 1240 = 1024 + 256
	lda >Ypos
	and #$00ff
	xba		; x 256
	sta <pos	; store temporarily

	asl a		; x 512
	asl a		; x 1024
	clc
	adc <pos
	sta <pos
	
;
; lower 1/2 row additional
;
	clc
	adc #160*4
	sta <pos

;
; for x offset, 160 bytes per row / 80 chars = 2 bytes/letter
;
	lda >Xpos
	asl a		; x 2
	clc
	adc <pos
	sta <pos
	tay

;
; $e12000 + pos = location to start drawing the character
;
	
	
	lda <char

	cmp #$20	; smallest printing char
	bcc exit	; not printable....
	cmp #127	; 126 is biggest
	bcs exit	; not printable

;
; each char takes up 16 bytes :. the offset is (char - $20) * 16
;

	sec
	sbc #$20
	asl a		; x2
	asl a		; x4
	asl a		; x8
	asl a		; x16

	tax
	lda >CHAR_SPACE,x
	eor >xor_mask
	and <andMask
	sta |$2000,y	; blast it to the screen
	
	lda >CHAR_SPACE+2,x
	eor >xor_mask
	and <andMask
	sta |$2000+160,y      
	
	lda >CHAR_SPACE+4,x
	eor >xor_mask
	and <andMask
	sta |$2000+320,y      

	lda >CHAR_SPACE+6,x
	eor >xor_mask
	and <andMask
	sta |$2000+480,y      

	lda >CHAR_SPACE+8,x
	eor >xor_mask
	and <andMask
	sta |$2000+640,y      

	lda >CHAR_SPACE+10,x
	eor >xor_mask
	and <andMask
	sta |$2000+800,y      

	lda >CHAR_SPACE+12,x
	eor >xor_mask
	and <andMask
	sta |$2000+960,y      

	lda >CHAR_SPACE+14,x
	eor >xor_mask
	and <andMask
	sta |$2000+1120,y     

exit	anop

	lda <_rtlb+2
	sta <andMask
	lda <_rtlb
	sta <char
	pld
   	pla
	pla
	pla
	plb
	rtl
	end
	
Cursor	START

StartCursor	ENTRY

	pha
;	~ReadBParam #$2f
	pla
	sta blinkRate

	stz savedX
	stz savedY

;	~SetHeartBeat #CursorHB
	rtl


StopCursor	ENTRY
;	~DelHeartBeat #CursorHB
	rtl

notSafe		ENTRY
		ds 2
blinkRate	ds 2
savedX		ds 2
savedY		ds 2

CursorHB	anop
	dc i4'0'
count	dc i2'30'
	dc h'5AA5'

	longa off

	phb
	phk
	plb
	php

;	long ai
	lda notSafe
	bne exit

	lda Xpos
	cmp savedX
	bne moved
	lda Ypos
	cmp savedY
	bne moved
	bra exit

moved	jsr drawCursor


exit	anop

	plp
	plb
	clc
	rtl

drawCursor	anop
	
	rts

	END

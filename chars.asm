	case on

dummy	start	
	end

;
; these are bitmaps for the characters
;
;
;

CHAR_SPACE	START
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
;CHAR_BANG	START
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000000000000000'
	dc b'0000001100000000'
	dc b'0000000000000000'

;CHAR_DQ	START       
	dc b'0000110011000000'
	dc b'0000110011000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'

; #
	dc b'0000110011000000'
	dc b'0000110011000000'
	dc b'0011111111110000'
	dc b'0000110011000000'
	dc b'0011111111110000'
	dc b'0000110011000000'
	dc b'0000110011000000'
	dc b'0000000000000000'

; $
	dc b'0000001100000000'
	dc b'0000111111110000'
	dc b'0011001100000000'
	dc b'0000111111000000'
	dc b'0000001100110000'
	dc b'0011111111000000'
	dc b'0000001100000000'
	dc b'0000000000000000'

;%
	dc b'0011110000000000'
	dc b'0011110000110000'
	dc b'0000000011000000'
	dc b'0000001100000000'
	dc b'0000110000000000'
	dc b'0011000011110000'
	dc b'0000000011110000'
	dc b'0000000000000000'


; &
	dc b'0000110000000000'
	dc b'0011001100000000'
	dc b'0011001100000000'
	dc b'0000110000000000'
	dc b'0011001100110000'
	dc b'0011000011000000'
	dc b'0000111100110000'
	dc b'0000000000000000'

;'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
; (

	dc b'0000001100000000'
	dc b'0000110000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0000110000000000'
	dc b'0000001100000000'
	dc b'0000000000000000'

; )

	dc b'0000001100000000'
	dc b'0000000011000000'
	dc b'0000000000110000'
	dc b'0000000000110000'
	dc b'0000000000110000'
	dc b'0000000011000000'
	dc b'0000001100000000'
	dc b'0000000000000000'


;*

	dc b'0000001100000000'
	dc b'0011001100110000'
	dc b'0000111111000000'
	dc b'0000001100000000'
	dc b'0000111111000000'
	dc b'0011001100110000'
	dc b'0000001100000000'
	dc b'0000000000000000'

;+

	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0011111111110000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000000000000000'

;,
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000110000000000'
	dc b'0000000000000000'
 


;-

	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0011111111110000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'

;.

	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000001100000000'
	dc b'0000000000000000'

;/

	dc b'0000000000000000'
	dc b'0000000000110000'
	dc b'0000000011000000'
	dc b'0000001100000000'
	dc b'0000110000000000'
	dc b'0011000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'


;0
	dc b'0000111111000000'
	dc b'0011000000110000'
	dc b'0011000011110000'
	dc b'0011001100110000'
	dc b'0011110000110000'
	dc b'0011000000110000'
	dc b'0000111111000000'
	dc b'0000000000000000'
 
;1
	dc b'0000001100000000'
	dc b'0000111100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000111111000000'
	dc b'0000000000000000'

;2
	dc b'0000111111000000'
	dc b'0011000000110000'
	dc b'0000000000110000'
	dc b'0000001111000000'
	dc b'0000110000000000'
	dc b'0011000000000000'
	dc b'0011111111110000'
	dc b'0000000000000000'
 

;3
	dc b'0011111111110000'
	dc b'0000000000110000'
	dc b'0000000011000000'
	dc b'0000001111000000'
	dc b'0000000000110000'
	dc b'0011000000110000'
	dc b'0000111111000000'
	dc b'0000000000000000'
 
;4
	dc b'0000000011000000'
	dc b'0000001111000000'
	dc b'0000110011000000'
	dc b'0011000011000000'
	dc b'0011111111110000'
	dc b'0000000011000000'
	dc b'0000000011000000'
	dc b'0000000000000000'
 
;5
	dc b'0011111111110000'
	dc b'0011000000000000'
	dc b'0011111111000000'
	dc b'0000000000110000'
	dc b'0000000000110000'
	dc b'0011000000110000'
	dc b'0000111111000000'
	dc b'0000000000000000'

;6

	dc b'0000001111110000'
	dc b'0000110000000000'
	dc b'0011000000000000'
	dc b'0011111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000111111000000'
	dc b'0000000000000000'

;7
	dc b'0011111111110000'
	dc b'0000000000110000'
	dc b'0000000011000000'
	dc b'0000001100000000'
	dc b'0000110000000000'
	dc b'0000110000000000'
	dc b'0000110000000000'
	dc b'0000000000000000'

;8
	dc b'0000111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000111111000000'
	dc b'0000000000000000'
	
;9
	dc b'0000111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000111111110000'
	dc b'0000000000110000'
	dc b'0000000011000000'
	dc b'0011111100000000'
	dc b'0000000000000000'

;:
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000001100000000'
	dc b'0000000000000000'
	dc b'0000001100000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'

;;

	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000001100000000'
	dc b'0000000000000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000110000000000'
	dc b'0000000000000000'

;<

	dc b'0000000011000000'
	dc b'0000001100000000'
	dc b'0000110000000000'
	dc b'0011000000000000'
	dc b'0000110000000000'
	dc b'0000001100000000'
	dc b'0000000011000000'
	dc b'0000000000000000'

;=
	dc b'0000000000000000'
	dc b'0000000000000000'
 	dc b'0011111111110000'
	dc b'0000000000000000'
	dc b'0011111111110000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'

;>
	dc b'0000110000000000'
	dc b'0000001100000000'
	dc b'0000000011000000'
	dc b'0000000000110000'
	dc b'0000000011000000'
	dc b'0000001100000000'
	dc b'0000110000000000'
	dc b'0000000000000000'

;?
	dc b'0000111111000000'
	dc b'0011000000110000'
	dc b'0000000011000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000000000000000'
	dc b'0000001100000000'
	dc b'0000000000000000'


;@

	dc b'0000111111000000'
	dc b'0011000000110000'
	dc b'0011001100110000'
	dc b'0011001100110000'
	dc b'0011001111000000'
	dc b'0011000000000000'
	dc b'0000111111110000'
	dc b'0000000000000000'



;CHAR_A	START
	dc b'0000001100000000'
	dc b'0000110011000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011111111110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000000000000000'


;CHAR_B	START
	dc b'0011111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011111111000000'
	dc b'0000000000000000'

;CHAR_C	START
	dc b'0000111111000000'
	dc b'0011000000110000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000110000'
	dc b'0000111111000000'
	dc b'0000000000000000'

;CHAR_D	START
	dc b'0011111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011111111000000'
	dc b'0000000000000000'

;CHAR_E	START
	dc b'0011111111110000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011111111000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011111111110000'
	dc b'0000000000000000'

;CHAR_F	START
	dc b'0011111111110000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011111111000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0000000000000000'

;CHAR_G	START
	dc b'0000111111110000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000011110000'
	dc b'0011000000110000'
	dc b'0000111111110000'
	dc b'0000000000000000'

;CHAR_H	START
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011111111110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000000000000000'

;CHAR_I	START
	dc b'0000111111000000'
	dc b'0000001100000000'
   	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000111111000000'
	dc b'0000000000000000'

;CHAR_J	START
     	dc b'0000000000110000'
     	dc b'0000000000110000'
	dc b'0000000000110000'
	dc b'0000000000110000'
	dc b'0000000000110000'
	dc b'0011000000110000'
	dc b'0000111111000000'
	dc b'0000000000000000'

;CHAR_K	START
	dc b'0011000011000000'
	dc b'0011001100000000'
	dc b'0011110000000000'
	dc b'0011000000000000'
	dc b'0011110000000000'
	dc b'0011001100000000'
	dc b'0011000011000000'
	dc b'0000000000000000'


;CHAR_L	START
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011111111110000'
	dc b'0000000000000000'

;CHAR_M	START
	dc b'0011000000110000'
	dc b'0011110011110000'
	dc b'0011001100110000'
	dc b'0011001100110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000000000000000'

;CHAR_N	START
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011110000110000'
	dc b'0011001100110000'
	dc b'0011000011110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000000000000000'

;CHAR_O	START
	dc b'0000111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000111111000000'
	dc b'0000000000000000'

;CHAR_P	START
	dc b'0011111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011111111000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0000000000000000'


;CHAR_Q	START
	dc b'0000111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011001100110000'
	dc b'0011000011000000'
	dc b'0000111100110000'
	dc b'0000000000000000'

;CHAR_R	START
	dc b'0011111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011111111000000'
	dc b'0011001100000000'
	dc b'0011000011000000'
	dc b'0011000000110000'
	dc b'0000000000000000'
;CHAR_S	START
	dc b'0000111111000000'
	dc b'0011000000110000'
	dc b'0011000000000000'
	dc b'0000111111000000'
	dc b'0000000000110000'
	dc b'0011000000110000'
	dc b'0000111111000000'
	dc b'0000000000000000'
;CHAR_T	START
	dc b'0011111111110000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000000000000000'

;CHAR_U	START
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000111111000000'
	dc b'0000000000000000'

;CHAR_V	START
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000110011000000'
	dc b'0000001100000000'
	dc b'0000000000000000'

;CHAR_W	START
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011001100110000'
	dc b'0011001100110000'
	dc b'0011110011110000'
	dc b'0011000000110000'
	dc b'0000000000000000'           

;CHAR_X	START
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000110011000000'
	dc b'0000001100000000'
	dc b'0000110011000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000000000000000'

;CHAR_Y	START
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000110011000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000000000000000'
;CHAR_Z	START
	dc b'0011111111110000'
	dc b'0000000000110000'
	dc b'0000000011000000'
	dc b'0000001100000000'
	dc b'0000110000000000'
	dc b'0011000000000000'
	dc b'0011111111110000'
	dc b'0000000000000000'

; _[_
	dc b'0011111111110000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011111111110000'
	dc b'0000000000000000'

; \
	dc b'0000000000000000'
	dc b'0011000000000000'
	dc b'0000110000000000'
	dc b'0000001100000000'
	dc b'0000000011000000'
	dc b'0000000000110000'
	dc b'0000000000000000'
	dc b'0000000000000000'

; ]
	dc b'0011111111110000'
	dc b'0000000000110000'
	dc b'0000000000110000'
	dc b'0000000000110000'
	dc b'0000000000110000'
	dc b'0000000000110000'
	dc b'0011111111110000'
	dc b'0000000000000000'

; ^

	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000001100000000'
	dc b'0000110011000000'
	dc b'0011000000110000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'

;CHAR__	START
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'1111111111111111'

; `
	dc b'0000110000000000'
	dc b'0000001100000000'
	dc b'0000000011000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'


; a
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000111111000000'
	dc b'0000000000110000'
	dc b'0000111111110000'
	dc b'0011000000110000'
	dc b'0000111111110000'
	dc b'0000000000000000'
                           
;b
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011111111000000'
	dc b'0000000000000000'

;c
 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000111111110000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0000111111110000'
	dc b'0000000000000000'

;d

	dc b'0000000000110000'
	dc b'0000000000110000'
	dc b'0000111111110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000111111110000'
	dc b'0000000000000000'

;e
 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000111111000000'
	dc b'0011000000110000'
	dc b'0011111111110000'
	dc b'0011000000000000'
	dc b'0000111111110000'
	dc b'0000000000000000'
                      
;f

 	dc b'0000001111000000'
	dc b'0000110000110000'
	dc b'0000110000000000'
	dc b'0011111111000000'
	dc b'0000110000000000'
	dc b'0000110000000000'
	dc b'0000110000000000'
	dc b'0000000000000000'


; g

 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000111111110000'
	dc b'0000000000110000'
	dc b'0000111111000000'

;h
 	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000000000000000'

;i
 	dc b'0000001100000000'
	dc b'0000000000000000'
	dc b'0000111100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000111111000000'
	dc b'0000000000000000'

; j

 	dc b'0000000011000000'
	dc b'0000000000000000'
	dc b'0000001111000000'
	dc b'0000000011000000'
	dc b'0000000011000000'
	dc b'0000000011000000'
	dc b'0000110011000000'
	dc b'0000001100000000'

; k

 	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000011000000'
	dc b'0011001100000000'
	dc b'0011111100000000'
	dc b'0011000011000000'
	dc b'0011000000110000'
	dc b'0000000000000000'

;l
 	dc b'0000111100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000111111000000'
	dc b'0000000000000000'

;m

 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0011110011110000'
	dc b'0011001100110000'
	dc b'0011001100110000'
	dc b'0011001100110000'
	dc b'0011000000110000'
	dc b'0000000000000000'

;n
 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0011111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000000000000000'


;o
 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000111111000000'
	dc b'0000000000000000'

;p

 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0011111111000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011111111000000'
	dc b'0011000000000000'
	dc b'0011000000000000'

;q

 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000111111110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000111111110000'
	dc b'0000000000110000'
	dc b'0000000000110000'

;r
 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0011001111110000'
	dc b'0011110000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0011000000000000'
	dc b'0000000000000000'

;s

 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000111111110000'
	dc b'0011000000000000'
	dc b'0000111111000000'
	dc b'0000000000110000'
	dc b'0011111111000000'
	dc b'0000000000000000'

;t
 	dc b'0000110000000000'
	dc b'0000110000000000'
	dc b'0011111111000000'
	dc b'0000110000000000'
	dc b'0000110000000000'
	dc b'0000110000110000'
	dc b'0000001111000000'
	dc b'0000000000000000'

;u

 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000011110000'
	dc b'0000111100110000'
	dc b'0000000000000000'

;v

 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000110011000000'
	dc b'0000001100000000'
	dc b'0000000000000000'

; w
 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011001100110000'
	dc b'0011001100110000'
	dc b'0011110011110000'
	dc b'0000000000000000'

;x
 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0011000000110000'
	dc b'0000110011000000'
	dc b'0000001100000000'
	dc b'0000110011000000'
	dc b'0011000000110000'
	dc b'0000000000000000'

;y

 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0011000000110000'
	dc b'0000111111110000'
	dc b'0000000000110000'
	dc b'0000111111000000'

;z
 	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0011111111110000'
	dc b'0000000011000000'
	dc b'0000001100000000'
	dc b'0000110000000000'
	dc b'0011111111110000'
	dc b'0000000000000000'

;{

 	dc b'0000001111110000'
	dc b'0000111100000000'
	dc b'0000111100000000'
	dc b'0011110000000000'
	dc b'0000111100000000'
	dc b'0000111100000000'
	dc b'0000001111110000'
	dc b'0000000000000000'


;|                   
 	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
	dc b'0000001100000000'
                        
;}               
 	dc b'0011111100000000'
	dc b'0000001111000000'
	dc b'0000001111000000'
	dc b'0000000011110000'
	dc b'0000001111000000'
	dc b'0000001111000000'
	dc b'0011111100000000'
	dc b'0000000000000000'
                     
;~

 	dc b'0000111100110000'
	dc b'0011001111000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'
	dc b'0000000000000000'

	END


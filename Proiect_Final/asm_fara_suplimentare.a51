org 0000h
	mov R0, #20 ; tensiunea de referinta din incapere
	setb P0.0
	setb P0.1  ; setez butoanele initial ca fiind neapasate

	acall init_timer ; Initializare Timer
	acall LCD
	acall ADC

; Functie pentru initializarea Timer-ului 1
init_timer:
	orl TMOD, #10h ; Setare Timer 1 in modul 1 (16-bit timer)
	ret

LCD:
	; Initializare LCD
	mov a, #38h ; Afisare caractere pe o matrice de 5x7
	acall instructiuni
	acall delay_timer

	mov a, #0Eh ; Pornire LCD
	acall instructiuni
	acall delay_timer

	mov a, #01h ; Stergere LCD
	acall instructiuni
	acall delay_timer

	; Afisare pe prima linie mesajul:"Temp. actuala"
	mov a, #80h ; pozitionare cursor pe prima linie la pozitia 0
	acall instructiuni
	acall delay_timer

	mov a, #'T'
	acall date
	acall delay_timer

	mov a, #'e'
	acall date
	acall delay_timer

	mov a, #'m'
	acall date
	acall delay_timer

	mov a, #'p'
	acall date
	acall delay_timer

	mov a, #'.'
	acall date
	acall delay_timer

	mov a, #'a'
	acall date
	acall delay_timer

	mov a, #'c'
	acall date
	acall delay_timer

	mov a, #'t'
	acall date
	acall delay_timer

	mov a, #'u'
	acall date
	acall delay_timer

	mov a, #'a'
	acall date
	acall delay_timer
	
	mov a, #'l'
	acall date
	acall delay_timer

	mov a, #'a'
	acall date
	acall delay_timer

	mov a, #':'
	acall date
	acall delay_timer

	; Afisare pe a doua linie mesajul:"Temp.setata"
	mov a, #0C0h ; Pozitionare cursor pe linia a doua la pozitia 0
	acall instructiuni
	acall delay_timer

	mov a, #'T'
	acall date
	acall delay_timer

	mov a, #'e'
	acall date
	acall delay_timer

	mov a, #'m'
	acall date
	acall delay_timer

	mov a, #'p'
	acall date
	acall delay_timer

	mov a, #'.'
	acall date
	acall delay_timer

	mov a, #'s'
	acall date
	acall delay_timer

	mov a, #'e'
	acall date
	acall delay_timer

	mov a, #'t'
	acall date
	acall delay_timer

	mov a, #'a'
	acall date
	acall delay_timer

	mov a, #'t'
	acall date
	acall delay_timer

	mov a, #'a'
	acall date
	acall delay_timer

	mov a, #':'
	acall date
	acall delay_timer

	ret

instructiuni: ; Trimite instructiuni la LCD
	mov P2, a
	clr P3.0 ; RS=0 pentru instructiuni
	clr P3.1 ; R/W=0 pentru scriere
	setb P3.2 ; E=1 se activeaza pinul E
	acall delay_timer
	clr P3.2 ; E=0 se dezactiveaza pinul E
	ret

date: ; Scrie date pe LCD
	mov P2, a
	setb P3.0 ; RS=1 pentru date
	clr P3.1 ; R/W=0 pentru scriere
	setb P3.2 ; E=1 se activeaza pinul E

	acall delay_timer
	clr P3.2 ; E=0 se dezactiveaza pinul E
	ret

ADC:
	mov a, #8Dh ; Pozitionare cursor pe prima linie pozitia 13
	acall instructiuni
	acall delay_timer
	mov r1, P1
	; Afisare temperatura setata
	mov a, r1
	mov b, #6h ; 5/2^8
	div ab
	mov b, #0Ah
	div ab
	push b
	add a, #30H
	acall date
	acall delay_timer
	pop b
	mov a, b
	add a, #30h
	acall date
	acall delay_timer

; Afisare temperatura actuala
	mov a, #0DFh ; simbolul de la grade
	acall date
	acall delay_timer
	
	mov a, #0CDh ; Pozitionare cursor pe a doua linie pozitia 13
	acall instructiuni
	acall delay_timer
	
	mov a, R0
	mov b, #0Ah
	div ab
	push b
	add a, #30h
	acall date
	acall delay_timer
	pop b
	mov a, b
	add a, #30h
	acall date
	acall delay_timer
	
	mov a, #0DFh
	acall date
	acall delay_timer
	
; Control releu
	mov a, P1 ;Valoarea actuala
	mov R6, P0 ;Valoarea setata
	subb a, R6
	jc start ; Se verifica daca temperatura setata este mai mare decat cea actuala
	sjmp oprire
start:
	clr P3.7 ; Pornire releu
	sjmp gata

oprire:
	setb P3.7 ;Oprire releu
 
; Tastatura
	jnb P0.0, Buton1 ; Se verifica daca "+" este apasat
	jnb P0.1, Buton2 ; Se verifica daca "-" este apasat
	sjmp ADC
Buton1:
	mov r3, #255
debouncing_1:
	jb P0.0, Buton1
	djnz r3, debouncing_1
	inc R0
b1:
	jnb P0.0, b1
	sjmp ADC

Buton2:
	mov r3, #255
debouncing_2:
	jb P0.1, Buton2
	djnz r3, debouncing_2
	dec R0
b2:
	jnb P0.1, b2
	sjmp ADC

; Functie de intarziere folosind Timer 1
delay_timer:
	mov TH1, #0EEh ; Incarca TH1 pentru un overflow de 5ms (presupunand un cristal de 12MHz)
	mov TL1, #00h ; Incarca TL1 pentru un overflow de 5ms (presupunand un cristal de 12MHz)
	setb TR1 ; Porneste Timer 1
d1:
	jnb TF1, d1 ; Asteapta overflow
	clr TF1 ; Opreste Timer 1
	clr TR1 ; Reseteaza flag-ul Timer 1
	ret
gata:
end

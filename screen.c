
#include <types.h>


extern pascal void SetMasterSCB(Word) inline(0x1604,dispatcher);
extern pascal void SetAllSCBs(Word) inline(0x1404,dispatcher);
extern pascal void SetColorTable(Word, ColorTable) inline(0x0E04,dispatcher);


void ClearScreen(void);

void screen_init(void) {
#define WHITE	0x0fff
#define YELLOW	0x0ff0
#define BLACK	0x0000
#define BLUE	0x000f
#define GREEN	0x0080
#define RED		0x0f00

	static ColorTable ct =
	{
		WHITE, // background
		RED,   // underline / blink
		BLUE,  // bold
		GREEN, // foreground

		WHITE, 
		RED,
		BLUE,
		GREEN,

		WHITE, 
		RED,	
		BLUE, 
		GREEN, 

		WHITE, 
		RED,
		BLUE,
		GREEN,
	};

	unsigned i;


	SetMasterSCB(0xc080);
	SetAllSCBs(0xc080);
	for (i = 0; i < 16; i++)
		SetColorTable(i, ct);

	ClearScreen();
	// linearize memory, enable shadowing.
	asm {
		phb
		pea 0xe0e0
		plb
		plb
		sep #0x20
		lda #0xC1
		tsb 0xc029
		lda #0x08
		trb 0xc035
		rep #0x20
		plb
	}


}

asm void screen_on(void) {
	phb
	pea 0xe0e0
	plb
	plb
	sep #0x20
	lda #0x80
	tsb 0xc029
	rep #0x20
	plb
	rtl
}

asm void screen_off(void) {
	phb
	pea 0xe0e0
	plb
	plb
	sep #0x20
	lda #0x80
	trb 0xc029
	rep #0x20
	plb
	rtl
}
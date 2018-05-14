#ifndef vt100_h
#define vt100_h

enum {
	vtDECCKM = 1 << 1,	/* Cursor key */
	vtDECANM = 1 << 2,	/* ANSI/VT52 */
	vtDECCOLM = 1 << 3,	/* Column */
	vtDECSCNM = 1 << 4,	/* Screen */
	vtDECOM = 1 << 6,	/* Origin */
	vtDECAWM = 1 << 7,	/* Auto wrap */
	vtDECKPAM = 1 << 8, /*  Keypad Application Mode */
	vtLNM = 1 << 9, /* Line Feed/New Line Mode */
	vtDEFAULT = vtDECANM | vtDECAWM,
};

void vt100_init(unsigned flags);
extern void vt100_process(const unsigned char *buffer, unsigned buffer_size);
extern void vt100_event(struct EventRecord *event);

#endif
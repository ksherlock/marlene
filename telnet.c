/*
 * This handles telnet "negotiation", etc.
 * We don't actually negotiate anything but we at least strip
 * the telnet commands.
 */ 

#pragma noroot

#include "telnet.h"

extern unsigned char buffer[];
extern unsigned buffer_size;

extern void send(const unsigned char *, unsigned);


enum {
	st_data = 0,
	st_iac,
	st_do,
	st_dont,
	st_will,
	st_wont,
	st_sb,
	st_sb_iac,
	st_cr
};

static int state = 0;

static unsigned do_bits[3];
static unsigned dont_bits[3];

static unsigned will_bits[3];
static unsigned wont_bits[3];

#define set_bit(type, x) \
	type ## _bits[(x) >> 4] |= 1 << ((x) & 0x0f) 

#define has_bit(type, x) \
	(type ## _bits[(x) >> 4] & 1 << ((x) & 0x0f))


void telnet_init(void) {
	static unsigned char msg[] = {

		IAC, DO, TELOPT_SGA,
		IAC, DO, TELOPT_ECHO,
		IAC, WONT, TELOPT_ECHO,
		IAC, DO, TELOPT_BINARY,
		IAC, DONT, TELOPT_LINEMODE,
		IAC, WILL, TELOPT_TTYPE,
		IAC, WILL, TELOPT_NAWS,
		IAC, WILL, TELOPT_TSPEED,
	};


	do_bits[0] = 0;
	do_bits[1] = 0;
	do_bits[2] = 0;

	dont_bits[0] = 0;
	dont_bits[1] = 0;
	dont_bits[2] = 0;

	will_bits[0] = 0;
	will_bits[1] = 0;
	will_bits[2] = 0;

	wont_bits[0] = 0;
	wont_bits[1] = 0;
	wont_bits[2] = 0;


	state = st_data;

#if 0
	set_bit(do, TELOPT_SGA);
	set_bit(do, TELOPT_ECHO);
	set_bit(do, TELOPT_BINARY);

	set_bit(dont, TELOPT_LINEMODE);

	set_bit(will, TELOPT_TTYPE);
	set_bit(will, TELOPT_NAWS);
	set_bit(will, TELOPT_TSPEED);
#endif

	send(msg, sizeof(msg));
}


void telnet_process(void) {

	unsigned i, j;
	unsigned char cmd[3];


	static unsigned char telopt_naws[] = {
		IAC, SB, TELOPT_NAWS,
		0, 80, 0, 24,
		IAC, SE
	};

	static unsigned char telopt_tspeed[] = {
		IAC, SB, TELOPT_TSPEED,
		'9', '6', '0', '0', ',', '9', '6', '0', '0',
		IAC, SE
	};

	static unsigned char telopt_ttype[] = {
		IAC, SB, TELOPT_TTYPE, TELQUAL_IS,
		'V', 'T', '1', '0', '0',
		IAC, SE
	};



	/* don't need to process if no state, no IACs in buffer */
	if (!state) {
		/* check for an IAC */
		for (i = 0; i < buffer_size; ++i) {
			if (buffer[i] == IAC) break;
		}
		if (i == buffer_size) return;
	}


	for (i = 0, j = 0; i < buffer_size; ++i) {

		unsigned char c = buffer[i];
		switch(state) {
			#if 0
			// ignoring this for now since vt100 ignores 0x00 anyhow.
			// also, would only run if other IAC detected.
			case st_cr: // 0x0d 0x00 < drop the 0x00
				if (c == 0x00) {
					state = st_data;
					break;
				}
				/* drop through */
				#endif
			case st_data:
				if (c == IAC) {
					state = st_iac;
				}
				else buffer[j++] = c;
				//if (c == 0x0d) state = st_cr;
				break;

			case st_iac:
				switch (c) {
					case IAC: // IAC IAC -> IAC
						state = st_data;
						buffer[j++] = c;
						break;
					case AYT:
						send("\r\n[Yes]\r\n", 9);
						state = st_data;
						break;
					case DO: state = st_do; break;
					case DONT: state = st_dont; break;
					case WILL: state = st_will; break;
					case WONT: state = st_wont; break;
					case SB: state = st_sb; break;
					default: state = st_data; break;
				}
				break;

			case st_do:

				cmd[0] = IAC;
				cmd[1] = WILL;
				cmd[2] = c;
				switch(c) {
					case TELOPT_SGA:
					case TELOPT_BINARY:
						// already said will do them, no reply needed.
						break;

					case TELOPT_NAWS:
						set_bit(do, TELOPT_NAWS);
						send(telopt_naws, sizeof(telopt_naws));
						break;
					case TELOPT_TSPEED:
						send(telopt_tspeed, sizeof(telopt_tspeed));
						break;
					case TELOPT_TTYPE:
						send(telopt_ttype, sizeof(telopt_ttype));
						break;

					default:
					case TELOPT_ECHO:
						cmd[1] = WONT;
						send(cmd, 3);
				}
				state = st_data;
				break;

			case st_dont:
				cmd[0] = IAC;
				cmd[1] = WONT;
				cmd[2] = c;
				#if 0
				switch(c) {
					case TELOPT_SGA:
						cmd[1] = WILL;
						break;
					default:
				}
				#endif
				send(cmd, 3);
				state = st_data;
				break;

			case st_will:
				cmd[0] = IAC;
				cmd[1] = DO;
				cmd[2] = c;
				switch(c) {
					case TELOPT_SGA:
					case TELOPT_BINARY:
					case TELOPT_ECHO:
					case TELOPT_NAWS:
						break;
					default:
						cmd[1] = DONT;
						send(cmd, 3);
				}
				state = st_data;
				break;

			case st_wont:
			#if 0
				cmd[0] = IAC;
				cmd[1] = DONT;
				cmd[2] = c;
				switch(c) {
					case TELOPT_SGA:
					case TELOPT_LINEMODE:
						cmp[1] = DO;
						break;
					default:
				}
				send(cmd, 3);
			#endif
				state = st_data;
				break;

			case st_sb:

				if (c == IAC) state = st_sb_iac;
				break;

			case st_sb_iac:
				// IAC | IAC or IAC | SE
				if (c == SE) state = st_data;
				else state = st_sb;
				break;
		}
	}

	buffer_size = j;
}

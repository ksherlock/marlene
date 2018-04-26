Marlene
--------

This is a IIgs telnet client I wrote circa 2004.  I recently (2013) dusted it
off and fixed a couple telnet negotiation bugs. Then I recently (2018) completely
redid the telnet negotiation, vt100, and drawing code. Aside from alternate character sets,
it should be more or less vt100 compatible.

It uses 4-color 640x200 graphics, which means it's slower than text but
has twice as many colors!

Characters are 8 pixels wide (80 x 8 = 640) which is a bit heavy on the
character spacing, but the drawing is simpler than 7 pixel characters.


![](https://github.com/ksherlock/marlene/blob/master/screenshots/telnet.png "telnet man page")


![](https://github.com/ksherlock/marlene/blob/master/screenshots/vim.png "VIM")


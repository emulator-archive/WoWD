***************************************************************************************
*
*   SpellExtra flags created by Captnoord for wowd group (c) 2006 - 2007
*
***************************************************************************************

Readme SpellExtra

This project is the result of the use of 2 different website's,
result is depending on the acurati of those website's.

Allakhazam:
http://wow.allakhazam.com/db/spelllist.html

Thottbot:
http://www.thottbot.com/?sp[SPELLID]

The current files in this map are documents on how to recreate the spellextra flags.
These flags contain manualy checked spellid's. Spellid's that need a form of inface check.

The flags are devided into simple bits.

   Name                Bit

SPELL_EXTRA_INFRONT 	1		// this requests the need of inface check
SPELL_EXTRA_BEHIND  	2		// this requests the need of behind check
SPELL_EXTRA_UNDEF0  	4		// example of a free bit
SPELL_EXTRA_UNDEF1  	8		// example of a free bit


Like the example show's that there is space of extra requirement flags 30 left to be precise.
requirements are easely added by adding/searching the spellid to the spellextra db and change / add the flag
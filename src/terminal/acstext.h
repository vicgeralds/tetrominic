/* code page 437 graphic characters (IBM graphics) */
enum {
	TEXT_MEDIUM_SHADE          = '\xB1',    /* a */
	TEXT_LIGHT_SHADE           = '\xB0',    /* h */
	TEXT_LOWER_RIGHT_CORNER    = '\xD9',    /* j */
	TEXT_UPPER_RIGHT_CORNER    = '\xBF',    /* k */
	TEXT_UPPER_LEFT_CORNER     = '\xDA',    /* l */
	TEXT_LOWER_LEFT_CORNER     = '\xC0',    /* m */
	TEXT_CROSS                 = '\xC5',    /* n */
	TEXT_HORIZONTAL_LINE       = '\xC4',    /* q */
	TEXT_LEFT_T                = '\xC3',    /* t */
	TEXT_RIGHT_T               = '\xB4',    /* u */
	TEXT_BOTTOM_T              = '\xC1',    /* v */
	TEXT_TOP_T                 = '\xC2',    /* w */
	TEXT_VERTICAL_LINE         = '\xB3'     /* x */
};

/* convert to IBM graphics and output using terminal.puttext() */
void putacs_text(const char *s);

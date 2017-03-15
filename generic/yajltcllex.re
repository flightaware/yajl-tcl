#include <stdio.h>

int numberValidator(const char *YYCURSOR)
{
	const char *YYMARKER;
	/*!re2c
		re2c:define:YYCTYPE = char;
		re2c:yyfill:enable = 0;

		end = "\x00";
		num = [-]?([0]|[1-9][0-9]*)([.][0-9]+)?([eE][+-]?[0-9]+)?;

		*		{ return 0; }
		num end { return 1; }
	*/
}

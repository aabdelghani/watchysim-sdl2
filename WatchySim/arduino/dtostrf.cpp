#include <stdio.h>

char* dtostrf(double val, signed char width, unsigned char prec, char* sout) {
	char fmt[20];
	snprintf(fmt, 8, "%%%d.%df", width, prec);
	snprintf(sout, 20, fmt, val);
	return sout;
}

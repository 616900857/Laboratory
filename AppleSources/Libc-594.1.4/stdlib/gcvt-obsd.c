/*	$OpenBSD: gcvt.c,v 1.5 2003/06/17 21:56:24 millert Exp $	*/

/*
 * Copyright (c) 2002, 2003 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Sponsored in part by the Defense Advanced Research Projects
 * Agency (DARPA) and Air Force Research Laboratory, Air Force
 * Materiel Command, USAF, under agreement number F39502-99-1-0512.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char rcsid[] = "$OpenBSD: gcvt.c,v 1.5 2003/06/17 21:56:24 millert Exp $";
#endif /* LIBC_SCCS and not lint */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

extern char *__dtoa(double, int, int, int *, int *, char **);
extern void __freedtoa(char *);

char *
gcvt(double value, int ndigit, char *buf)
{
	char *digits, *dst, *src;
	int i, decpt, sign;
	char *decimal_point = localeconv()->decimal_point;

	if (ndigit == 0) {
		buf[0] = '\0';
		return (buf);
	}

	digits = __dtoa(value, 2, ndigit, &decpt, &sign, NULL);
	if (decpt == 9999) {
		/* Infinity or NaN, assume buffer is long enough. */
		dst = buf;
		if (sign)
			*dst++ = '-';
		strcpy(dst, (*digits == 'N') ? "nan" : "inf");
		__freedtoa(digits);
		return (buf);
	}

	dst = buf;
	if (sign)
		*dst++ = '-';

	if (decpt < 0 || decpt > ndigit) {
		/* exponential format */
		if (--decpt < 0) {
			sign = 1;
			decpt = -decpt;
		} else
			sign = 0;
		src = digits;
		*dst++ = *src++;
		dst = stpcpy(dst, decimal_point);
		while (*src != '\0')
			*dst++ = *src++;
		*dst++ = 'e';
		if (sign)
			*dst++ = '-';
		else
			*dst++ = '+';
		if (decpt < 10) {
			*dst++ = '0';
			*dst++ = '0' + decpt;
			*dst = '\0';
		} else {
			/* XXX - optimize */
			for (sign = decpt, i = 0; (sign /= 10) != 0; i++) {}
			dst[i + 1] = '\0';
			while (decpt != 0) {
				dst[i--] = '0' + decpt % 10;
				decpt /= 10;
			}
		}
	} else {
		/* standard format */
		for (i = 0, src = digits; i < decpt; i++) {
			if (*src != '\0')
				*dst++ = *src++;
			else
				*dst++ = '0';
		}
		if (*src != '\0') {
			if (src == digits) /* need leading zero */
				*dst++ = '0';
			dst = stpcpy(dst, decimal_point);
			for (i = decpt; digits[i] != '\0'; i++) {
				*dst++ = digits[i];
			}
		}
		*dst = '\0';
	}
	__freedtoa(digits);
	return (buf);
}

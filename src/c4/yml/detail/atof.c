/* $Id: atof.c,v 1.4 2002/08/10 13:05:17 pefo Exp $ */

/*
 * Copyright (c) 2000-2002 Opsycon AB  (www.opsycon.se)
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Opsycon AB.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/* XXX should ignore leading white space, and give up on first bad char */
#ifdef FLOATINGPT
double 
atof(char *p)
{
    double          d, t;
    int             len, val, sz, div, isneg;
    char            tmp[18];

    if (*p == '-') {
	isneg = 1;
	p++;
    } else
	isneg = 0;

    sz = strcspn (p, ".");
    if (sz > 0) {
	strncpy (tmp, p, sz);
	tmp[sz] = 0;
	if (!atob (&val, tmp, 10))
	    return (d);
    } else
	val = 0;

    d = (double)val;
    p += sz;
    if (*p)
	p++;
    if (*p) {
	len = strlen (p);
	if (!atob (&val, p, 10))
	    return (0);

	div = 1;
	for (; len > 0; len--)
	    div *= 10;

	t = (double)val;
	t /= div;

	d += t;
    }
    if (isneg)
	d = 0 - d;
    return (d);
}
#endif

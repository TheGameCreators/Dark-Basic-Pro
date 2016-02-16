#ifndef	NXU_ASC2BIN

#define	NXU_ASC2BIN


namespace	NXU
{

// types:
//
//					f		:	4	byte float
//					d		:	4	byte integer
//					c		:	1	byte character
//					b		:	1	byte integer
//					h		:	2	byte integer
//					p		:	4	byte const char	*
//					x1	:	1	byte hex
//					x2	:	2	byte hex
//					x4	:	4	byte hex (etc)
// example usage:
//
//		Asc2Bin("1 2 3 4 5 6",1,"fffff",0);

// use this	one	if you know	eactly 'how	many'	you	expect to	find.
void *Asc2Bin(const	char *source,	const	int	count, const char	*ctype,	void *dest);

// use this	one	if you don't know	how	many.	 It	will tell	you	how	many it	found	and	allocate
// sufficient	memory to	store	it.
void *Asc2Bin(const	char *source,	int	&count,	const	char *ctype);


/* flag values */
#define FL_UNSIGNED   1       /* strtoul called */
#define FL_NEG        2       /* negative sign found */
#define FL_OVERFLOW   4       /* overflow occured */
#define FL_READDIGIT  8       /* we've read at least one correct digit */

unsigned long strtol (
					  const char *nptr,
					  int flags
					  );
}

#endif

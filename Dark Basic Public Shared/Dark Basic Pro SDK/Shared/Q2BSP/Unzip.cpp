//////////////////////////////////////////////////////////////////////
//  Based on PCDEZIP.
//////////////////////////////////////////////////////////////////////
//
//  PCDEZIP.CPP þþ Copyright 1994, Bob Flanders and Michael Holmes      
//  First Published in PC Magazine January 11, 1994                     
//                                                                      
//  PCDEZIP decompresses .ZIP files created with PKZIP through
//  version 2.04g.  PCDEZIP provides syntax and switch compatibilty
//  with both Michael Mefford's PCUNZP and Phil Katz's PKUNZIP.         
//
//////////////////////////////////////////////////////////////////////
#include <stdio.h> 
#include <windows.h>

#pragma warning( disable : 4005 )
#pragma warning( disable : 4018 )
#pragma warning( disable : 4305 )
#pragma warning( disable : 4309 )
#undef	FREE
#undef	MAX_PATH

#pragma  pack(push)
#pragma	 pack(1)
#include "unzip.h"

/* ******************************************************************** *
 *
 *  Global Variables
 *
 * ******************************************************************** */

static
ULONG   crc,                                // running crc 32 value
        bhold;                              // bits hold area

static
long    rsize,                              // remaining size to process
        last_kpal,                          // keep alive message time
        total_size,                         // uncompressed sum
        total_csize;                        // compressed sum

static
int     rc = 1,                             // errorlevel return code
        pos_count,                          // positional parms count
        sf_count,                           // select file parms count
        e_count,                            // end of data counter/flag
        total_files;                        // files processed

static
UINT    sb_size= 0;                         // sliding buffer size

static
BYTE    bsize,                              // bits left to process
       *sb=NULL, *sbp=NULL, *sbe=NULL;      // sliding buffer, ptr, and end


/* ******************************************************************** *
 *
 *  Inflate Constants
 *
 *  These tables are taken from PKZIP's appnote.txt.
 *
 * ******************************************************************** */


static UINT bll[] =                                // bit length code lengths
    {
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5,
    11, 4, 12, 3, 13, 2, 14, 1, 15
    };

static UINT cll[] =                                // copy lengths for literal ..
    {                                       // ..codes 257 thru 285
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13,
    15, 17, 19, 23, 27, 31, 35, 43,
    51, 59, 67, 83, 99, 115, 131, 163,
    195, 227, 258, 0, 0
    };

static UINT cle[] =                                // extra bits for literal
    {                                       // ..codes 257 thru 285
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
    1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4,
    4, 4, 5, 5, 5, 5, 0, 99, 99
    };

static UINT cdo[] =                                // copy distance offsets
    {                                       // ..for codes 0 thru 29
    1, 2, 3, 4, 5, 7, 9, 13, 17, 25,
    33, 49, 65, 97, 129, 193, 257, 385,
    513, 769, 1025, 1537, 2049, 3073,
    4097, 6145, 8193, 12289, 16385, 24577
    };

static UINT cde[] =                                // copy extra bits for distance
    {                                       // ..codes 0 thru 29
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4,
    5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10,
    11, 11, 12, 12, 13, 13
    };

/*--------------------------------------------------------*
**	Declaraciones de datos globales.
**-------------------------------------------------------*/
static char	*ZIPBuffer = NULL;
static char	*ZIPOut	= NULL;


/*--------------------------------------------------------*
**	Declaraciones de funciones
**-------------------------------------------------------*/

static int     extract_open(LF *lfp);
static void    extract_flush(void);
static UINT    lookat_code(int bits);
static UINT    get_code(int bits);
static UINT    get_byte(void);
static void    store_char(char c);
static void    extract_copy(ULONG len);
static int __cdecl    exp_cmp(const void *a,             
                       const void *b);
static UINT    exp_read(SFT *sf);
static void    exp_load_tree(SFT *sf,   // base S-F tree pointer
                             int n);    // number of entries
static int     inf_build_tree(UINT *b,  // code lengths in bits
                              UINT n,   // number of codes
                              UINT s,   // number of simple codes
                              UINT *d,  // base values for non-simple
                              UINT *e,  // list of extra bits
                              HUFF **t, // resulting table
                              UINT *m); // maximum lookup bits
static void    inf_free(HUFF *t);
static int     inf_fixed(void);
static int     inf_dynamic(void);
static void    extract_inflate(LF *lfp);     // local file pointer
static void    extract_explode(LF *lfp);     // local file pointer
static void    extract_expand(LF *lfp);      // local file pointer
static void    extract_shrunk(void);         // local file pointer
static void    extract_stored(void);         // local file pointer
static void    extract_zip(int fnc,          // function
                           LF *lfp);         // local file header pointer


// forward
static void crc_32(unsigned char c, unsigned long *crc);


/* ******************************************************************** *
 *
 *  malloc_chk() -- allocate memory with error processing
 *
 * ******************************************************************** */

static void* 
malloc_chk(int n)                  // size of block
{
	void   *s;                                  // temporary pointers
    if (!(s = malloc(n))) {                  // q. enough memory?
        return (void*) 0xdeadbeef;
    }

	memset((char *) s,0,(long) n);		   // else .. clear to nulls
	return(s);                                // ..and return w/address

}

/* ******************************************************************** *
 *
 *  quit_with( char * )
 *
 * ******************************************************************** */
static void	
quit_with( char *error ) 
{
}

static const char* curptr;
#define next_byte()   ((int)*curptr++&0xff)

/**********************************************************************
*	char *unzip_file()
*
***********************************************************************/

char* unzip_file(const char* p, int& size)
{
    LF* lfp = (LF*) p;
    curptr = p + sizeof(LF) + lfp->lf_fn_len + lfp->lf_ef_len;
	ZIPBuffer = ZIPOut = 0;

    extract_zip(0,lfp);
    extract_zip(1,lfp);
    extract_zip(2,lfp);

    size = lfp->lf_size;
    return ZIPBuffer;
}

/* ******************************************************************** *
 *
 *  extract_open() -- open the destination file for output
 *
 *  Returns: 0 = output file opened
 *           1 = skip file expansion
 *
 * ******************************************************************** */

static
int     extract_open(LF *lfp)               // local file pointer
{
	int		size = lfp->lf_size;
	ZIPBuffer = ZIPOut = (char*) malloc_chk( size+1 );
	return(0);             // ..and process this file
}


/* ******************************************************************** *
 *
 *  store_char() -- store character in output file
 *
 * ******************************************************************** */
static
void    store_char(char c)    // character to store
{
	crc_32(c, &crc);          // determine current crc 32
	*sbp = c;                                   // a. yes .. put char in buffer

	if (++sbp >= sbe)                           // q. past end of buffer?
	{											// a. yes .. write buffer
		memcpy( ZIPOut, sb, sb_size );
		ZIPOut += sb_size;			            
		sbp = sb;                               // ..then back to start again
	}
}

/* ******************************************************************** *
 *
 *  extract_flush() -- write output buffer as needed
 *
 * ******************************************************************** */
static
void    extract_flush(void)
{
	if (sbp != sb)                              // q. local buffer need flushing?
    {											// a. yes .. flush local buffer
		memcpy( ZIPOut, sb, (UINT) (sbp - sb) );
		ZIPOut += (UINT) (sbp - sb);
    }
}



/* ******************************************************************** *
 *
 *  lookat_code() -- look at the next code from input file
 *
 * ******************************************************************** */
static
UINT    lookat_code(int bits)               // nbr of bits
{
static
UINT    bit_mask[] =                        // bit masks
    {
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f,
    0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff,
    0x1fff, 0x3fff, 0x7fff, 0xffff
    };


while (bsize < bits)                        // get enough bits
    {
    if (rsize > 0)                              // q. anything left to process?
        {
        //bhold |= (ULONG)fgetc(ifile) << bsize;  // a. yes .. get a character
        bhold |= (ULONG) next_byte() << bsize;  // a. yes .. get a character
        bsize += 8;                             // ..and increment counter
        rsize--;                                // ..finally, decrement cnt
        }
     else if (NOT e_count)                  // q. done messing around?
        {
        extract_flush();                    // a. yes .. flush output buffer
        quit_with(0/*data_error*/);              // ..then quit with an error msg
        }
     else if (e_count > 0)                  // q. down counting?
        {
        e_count--;                          // a. yes .. decriment counter
        return(-1);                         // ..and return all done
        }
     else
        {
        e_count++;                          // else .. count up
        break;                              // ..and exit loop
        }
    }

return(((UINT) bhold) & bit_mask[bits]);    // return just enough bits

}



/* ******************************************************************** *
 *
 *  get_code() -- get next code from input file
 *
 * ******************************************************************** */
static
UINT    get_code(int bits)                  // nbr of bits in this code
{
UINT    a;                                  // accumulator


a = lookat_code(bits);                      // prepare return value
bhold >>= bits;                             // ..shift out requested bits
bsize -= bits;                              // ..decrement remaining bit count
return(a);                                  // ..and return value

}



/* ******************************************************************** *
 *
 *  get_byte() -- get next byte from input file
 *
 * ******************************************************************** */
static
UINT    get_byte(void)
{

if (rsize > 0)                              // q. anything left?
    {
    rsize--;                                // a. yes .. decrement count
    //return(fgetc(ifile));                   // ..and return w/character
    return(next_byte());                   // ..and return w/character
    }
 else
    return(-1);                             // else .. return error
}




/* ******************************************************************** *
 *
 *  extract_copy() -- copy stored data to output stream
 *
 * ******************************************************************** */
static
void    extract_copy(ULONG len)             // length to copy
{
const char   *b,                                  // work buffer pointer
       *p;                                  // work pointer
UINT    csize = 0;                          // current read size


while (len)                                 // loop copying file to output
    {
    csize = (len <= COPY_BUFFER) ?          // determine next read size
            (int) len : (UINT) COPY_BUFFER;

    //if (fread(b, 1, csize, ifile) != csize) // q. read ok?
    //    quit_with(read_error);              // a. no .. quit w/error msg

    b = curptr; curptr += csize;

    rsize -= csize;                         // decrement remaining length
    len -= csize;                           // ..and loop control length

    for (p = b; csize--;)                   // for the whole block
        store_char(*p++);                   // ..write output data
    }

}



/* ******************************************************************** *
 *
 *  exp_cmp() -- compare the S-F tree entries
 *
 * ******************************************************************** */
static
int __cdecl     exp_cmp(const void *a,              // entries to compare
                const void *b)
{

return(strncmp((char *) a, (char *) b, 2)); // compare blen and value

}



/* ******************************************************************** *
 *
 *  exp_load_tree() -- load a single S-F tree
 *
 * ******************************************************************** */
static
void    exp_load_tree(SFT *sf,              // base S-F tree pointer
                      int n)                // number of entries
{
UINT    c,                                  // current character read
        blen,                               // bit length
        ne,                                 // number of entries
        lb,                                 // last bit increment
        ci,                                 // code increment
        i, j,                               // loop control
        br;                                 // bytes to read
SFT    *p;                                  // work pointer


if ((c = get_byte()) == 0xffff)             // q. read length ok?
    return;                                 // a. no .. just return

br = c + 1;                                 // get number of active elements

for (p = sf, i = 0; br--;)                  // loop thru filling table
    {                                       // ..with lengths
    if ((c = get_byte()) == 0xffff)         // q. get code ok?
        return;                             // a. no .. just return

    blen = (c & 0x0f) + 1;                  // save bit length
    ne = (c >> 4) + 1;                      // ..and nbr of values at blen

    for (j = 0; j < ne; j++, i++, p++)      // loop filling this bit length
        {
        p->blen = blen;                     // save bit length
        p->value = i;                       // ..and the relative entry nbr
        }
    }

qsort((void *) sf, n, sizeof(SFT), exp_cmp);// sort the bit lengths

c = lb = ci = 0;                            // init code, last bit len ..
                                            // ..and code increment
for (p = &sf[n - 1]; p >= sf; p--)          // loop thru tree making codes
    {
    c += ci;                                // build current code
    p->code = c;                            // ..and store in tree

    if (p->blen != lb)                      // q. bit lengths change?
        {
        lb = p->blen;                       // a. yes .. save current value
        ci = (1 << (16 - lb));              // ..and bld new code increment
        }
    }

for (p = sf, j = n; j--; p++)               // loop thru reversing bits
    {
    c = p->code;                            // get code entry

    for (i = 16, ci = 0; i--;)              // loop doing the bit work
        {
        ci = (ci << 1) | (c & 1);           // building the reversed code
        c >>= 1;                            // ..one bit at a time
        }

    p->code = ci;                           // put new code back
    }
}



/* ******************************************************************** *
 *
 *  exp_read() -- read the input stream returning an S-F tree value
 *
 * ******************************************************************** */
static
UINT    exp_read(SFT *sf)                   // base S-F tree pointer
{
UINT    c,                                  // read code
        code,                               // working bit string
        bits;                               // current nbr of bits
SFT    *p = sf;                             // S-F pointer


for (code = bits = 0;;)                     // loop to find the code
    {
    if ((c = get_code(1)) == 0xffff)        // q. get a bit ok?
        return(-1);                         // a. no .. return w/err code

    c <<= bits++;                           // shift left a little
    code |= c;                              // save next bit

    for (;;)                                // loop to find code
        {
        if (p->blen > bits)                 // q. read enough bits yet?
            break;                          // a. no .. get another one

         else if (p->blen == bits)          // q. read just enough?
            {                               // a. yes .. check entries
            if (p->code == code)            // q. find proper entry?
                return(p->value);           // a. yes .. return w/code
            }

        p++;                                // bump index
        }
    }

#if defined(_MSC_VER)
return(-1);                                 // make MSC C/C++ happy
#endif

}



/* ******************************************************************** *
 *
 *  inf_build_tree() -- build a Huffman tree
 *
 *  Returns: TRUE if error building Huffman tree
 *           FALSE if tree built
 *
 * ******************************************************************** */

#define MAX_BITS    16                      // maximum bits in code
#define CODE_MAX    288                     // maximum nbr of codes in set

static
int     inf_build_tree(UINT *b,             // code lengths in bits
                       UINT n,              // number of codes
                       UINT s,              // number of simple codes
                       UINT *d,             // base values for non-simple
                       UINT *e,             // list of extra bits
                       HUFF **t,            // resulting table
                       UINT *m)             // maximum lookup bits
{
UINT    a,                                  // code lengths of k
        c[MAX_BITS + 1],                    // bit length count table
        f,                                  // i repeats every f entries
        i, j,                               // loop control
       *p,                                  // work pointer
        v[CODE_MAX],                        // values in order of bit length
        x[MAX_BITS + 1],                    // bit offsets
       *xp,                                 // pointer
        z;                                  // entries in current table
int     g,                                  // max code length
        h,                                  // table level
        k,                                  // loop control
        l,                                  // max bits length
        w,                                  // bits before this table
        y;                                  // number of dummy entries
HUFF    r,                                  // work entry
       *q,                                  // current table entry
       *u[MAX_BITS];                        // table stack


memset(c, 0, sizeof(c));                    // clear table to nulls


for (p = b, i = n; i--;)                    // loop thru table to generate
    c[*p++]++;                              // ..counts for each bit length

if (c[0] == n)                              // q. all zero lengths?
    {
    *t = 0;                                 // a. yes .. clear result..
    *m = 0;                                 // ..pointer and count
    return(FALSE);                          // ..and return all ok
    }

                                            // find min and max code lengths
for (l = *m, j = 1; j <= MAX_BITS; j++)     // loop to find minimum code len
    if (c[j])                               // q. find the min code length?
        break;                              // a. yes .. exit the loop

k = j;                                      // save minimum code length

if ((UINT) l < j)                           // q. minimum greater than lookup?
    l = j;                                  // a. yes .. set up new lookup

for (i = MAX_BITS; i; i--)                  // loop to find max code length
    if (c[i])                               // q. find a used entry?
        break;                              // a. yes .. exit loop

g = i;                                      // save maximum code length

if ((UINT) l > i)                           // q. lookup len greater than max?
    l = i;                                  // a. yes .. set up new look len

*m = l;                                     // return new lookup to caller


for (y = 1 << j; j < i; j++, y <<= 1)       // loop to adjust last length codes
    if ((y -= c[j]) < 0)                    // q. more codes than bits?
        return(TRUE);                       // a. yes .. return w/error flag

if ((y -= c[i]) < 0)                        // q. more than max entry's count?
    return(TRUE);                           // a. yes .. return w/error flag

c[i] += y;                                  // adjust last length code


x[1] = j = 0;                               // initialize code stack
for (p = c + 1, xp = &x[2], i = g; i--;)    // loop thru generating offsets
    *xp++ = (j += *p++);                    // ..into the entry for each length


for (p = b, i = 0; i < n; i++)              // make table of value in order ..
    if ((j = *p++) != 0)                    // ..by bit lengths
        v[x[j]++] = i;


x[0] = i = 0;                               // first Huffman code is all zero
p = v;                                      // use the values array
h = -1;                                     // no tables yet, level = -1
w = -l;                                     // bits decoded
q = u[0] = (HUFF *) 0;                      // clear stack and pointer
z = 0;                                      // ..and number of entries

for (; k <= g; k++)                         // loop from min to max bit lengths
    {
    for (a = c[k]; a--;)                    // process entries at this bit len
        {
        while (k > w + l)                   // build up tables to k length
            {
            h++;                            // increment table level
            w += l;                         // add current nbr of bits

            z = (z = g - w) > (UINT) l      // determine the number of entries
                    ? l : z;                // ..in the current table

            if ((f = 1 << (j = k - w))      // q. k-w bit table contain enough?
                    > a + 1)
                {
                f -= a + 1;                 // a. too few codes for k-w bit tbl
                xp = c + k;                 // ..deduct codes from patterns left

                while (++j < z)             // loop to build upto z bits
                    {
                    if ((f <<= 1) <= *++xp) // q. large enough?
                        break;              // a. yes .. to use j bits

                    f -= *xp;               // else .. remove codes
                    }
                }

            z = 1 << j;                     // entries for j-bit table


            q = (HUFF *) malloc_chk(        // get memory for new table
                    (z + 1) * sizeof(HUFF));

            *t = q + 1;                     // link to main list
            *(t = &(q->v.table)) = 0;       // clear ptr, save address
            u[h] = ++q;                     // table starts after link


            if (h)                          // q. is there a last table?
                {
                x[h] = i;                   // a. yes .. save pattern
                r.blen = (BYTE) l;          // bits to dump before this table
                r.eb = (BYTE) (16 + j);     // bits in this table
                r.v.table = q;              // pointer to this table
                j = i >> (w - l);           // index to last table
                u[h-1][j] = r;              // connect to last table
                }
            }


        r.blen = (BYTE) (k - w);            // set up table entry

        if (p >= v + n)                     // q. out of values?
            r.eb = 99;                      // a. yes .. set up invalid code
         else if (*p < s)                   // q. need extra bits?
            {
            r.eb = (BYTE) (*p < 256         // a. yes .. set up extra bits
                    ? 16 : 15);             // 256 is end-of-block code
            r.v.code = *p++;                // simple code is just the value
            }
         else
            {
            r.eb = (BYTE) e[*p - s];        // non-simple--look up in lists
            r.v.code = d[*p++ - s];
            }


        f = 1 << (k - w);                   // fill code-like entries with r

        for (j = i >> w; j < z; j += f)
            q[j] = r;


        for (j = 1 << (k - 1); i & j; j >>= 1)  // backwards increment the
            i ^= j;                             // ..k-bit code i

        i ^= j;


        while ((i & ((1 << w) - 1)) != x[h])
            {
            h--;                            // decrement table level
            w -= l;                         // adjust bits before this table
            }
        }
    }

return(FALSE);                              // return FALSE, everything ok

}



/* ******************************************************************** *
 *
 *  inf_free() -- free malloc'd Huffman tables
 *
 * ******************************************************************** */
static
void    inf_free(HUFF *t)                   // base table to free
{
HUFF   *p;                                  // work pointer


while (t)                                   // loop thru freeing memory
    {
    p = (--t)->v.table;                     // get next table address
    free(t);                                // free current table
    t = p;                                  // establish new base pointer
    }
}



/* ******************************************************************** *
 *
 *  inf_codes() -- inflate the codes using the Huffman trees
 *
 * ******************************************************************** */
static
int     inf_codes(HUFF *tl,                 // literal table
                  HUFF *td,                 // distance table
                  int  bl,                  // literal bit length
                  int  bd)                  // distance bit length
{
UINT    c,                                  // current retrieved code
        e,                                  // extra bits
        n, d;                               // length and distance
BYTE   *p;                                  // work pointer
HUFF   *t;                                  // current Huffman tree

for (;;)                                    // loop till end of block
    {
    c = lookat_code(bl);                    // get some bits

    if ((e = (t = tl + c)->eb) > 16)        // q. in this table?
        do  {                               // a. no .. loop reading codes
            if (e == 99)                    // q. invalid entry?
                return(TRUE);               // a. yes .. return an error

            get_code(t->blen);              // read some bits
            e -= 16;                        // nbr of bits to get
            c = lookat_code(e);             // get some bits
            }
            while ((e = (t = t->v.table + c)->eb) > 16);

    get_code(t->blen);                      // read some processed bits

    if (e == 16)                            // q. literal code?
        store_char(t->v.code);              // a. yes .. output code

     else
        {
        if (e == 15)                        // q. end of block?
            return(FALSE);                  // a. yes .. return all ok

        n = get_code(e) + t->v.code;        // get length code
        c = lookat_code(bd);                // get some bits

        if ((e = (t = td + c)->eb) > 16)    // q. in this table?
            do  {                           // a. no .. loop thru
                if (e == 99)                // q. invalid entry?
                    return(TRUE);           // a. yes .. just return

                get_code(t->blen);          // read some bits
                e -= 16;                    // number of bits to get
                c = lookat_code(e);         // get some bits
                }
                while ((e = (t = t->v.table + c)->eb) > 16);

        get_code(t->blen);                  // read some processed bits
        d = t->v.code + get_code(e);        // get distance value

        if ((sbp - sb) >= d)                // q. backward wrap?
            p = sbp - d;                    // a. no .. just back up a bit
         else
            p = sb_size - d + sbp;          // else .. find at end of buffer

        while (n--)                         // copy previously outputed
            {                               // ..strings from sliding buffer
            store_char(*p);                 // put out each character

            if (++p >= sbe)                 // q. hit the end of the buffer?
                p = sb;                     // a. yes .. back to beginning
            }
        }
    }
}



/* ******************************************************************** *
 *
 *  inf_fixed() -- inflate a fixed Huffman code block
 *
 * ******************************************************************** */
static
int     inf_fixed(void)
{
UINT    i,                                  // loop control
        bl = 7,                             // bit length for literal codes
        bd = 5,                             // ..and distance codes
        l[288];                             // length list
HUFF   *tl, *td;                            // literal and distance trees


for (i = 0; i < 144; i++)                   // set up literal table
    l[i] = 8;

for (; i < 256; i++)                        // ..fixing up ..
    l[i] = 9;

for (; i < 280; i++)                        // ..all the
    l[i] = 7;

for (; i < 288; i++)                        // ..entries
    l[i] = 8;

if (inf_build_tree(l, 288, 257, cll,        // q. build literal table ok?
            cle, &tl, &bl))
    return(TRUE);                           // a. no .. return with error

for (i = 0; i < 30; i++)                    // set up the distance list
    l[i] = 5;                               // ..to 5 bits

if (inf_build_tree(l, 30, 0, cdo, cde,      // q. build distance table ok?
            &td, &bd))
    return(TRUE);                           // a. no .. return with error

if (inf_codes(tl, td, bl, bd))              // q. inflate file ok?
    return(TRUE);                           // a. no .. return with error

inf_free(tl);                               // free literal trees
inf_free(td);                               // ..and distance trees
return(FALSE);                              // ..and return all ok

}



/* ******************************************************************** *
 *
 *  inf_dynamic() -- inflate a dynamic Huffman code block
 *
 * ******************************************************************** */
static
int     inf_dynamic(void)
{
UINT    c,                                  // code read from input stream
        i, j,                               // loop control
        l,                                  // last length
        n,                                  // nbr of lengths to get
        bl, bd,                             // literal and distance bit len
        nl, nd,                             // literal and distance codes
        nb,                                 // nbr of bit length codes
        ll[286 + 30];                       // literal length and dist codes
HUFF   *tl, *td;                            // literal and distance trees


nl = get_code(5) + 257;                     // get nbr literal len codes
nd = get_code(5) + 1;                       // ..and the nbr dist len codes
nb = get_code(4) + 4;                       // ..and nbr of of bit lengths

for (j = 0; j < nb; j++)                    // read in bit length code
    ll[bll[j]] = get_code(3);               // set up bit lengths

for (; j < 19; j++)                         // loop thru clearing..
    ll[bll[j]] = 0;                         // ..other lengths

bl = 7;                                     // set literal bit length

if (inf_build_tree(ll, 19, 19,              // q. build decoding table for
            0, 0, &tl, &bl))                // ..trees using 7 bit lookup ok?
    return(TRUE);                           // a. no .. return with error


n = nl + nd;                                // number of lengths to get

for (i = l = 0; i < n;)                     // get literal and dist code lengths
    {
    c = lookat_code(bl);                    // get some bits
    j = (td = tl + c)->blen;                // get length code from table
    get_code(j);                            // use those bits
    j = td->v.code;                         // ..then get code from table

    if (j < 16)                             // q. save length?
        ll[i++] = l = j;                    // a. yes .. also save last length
     else if (j == 16)                      // q. repeat last length 3 to 6x?
        {
        j = get_code(2) + 3;                // get repeat length code

        if (i + j > n)                      // q. past end of array?
            return(TRUE);                   // a. yes .. return with error

        while (j--)                         // else .. loop filling table
            ll[i++] = l;                    // ..with last length
        }
     else if (j == 17)                      // q. 3 to 10 zero length codes?
        {
        j = get_code(3) + 3;                // a. yes .. get repeat code

        if (i + j > n)                      // q. past end of array?
            return(TRUE);                   // a. yes .. return with error

        while (j--)                         // else .. loop filling table
            ll[i++] = 0;                    // ..with zero length

        l = 0;                              // ..and save new last length
        }
     else                                   // else .. j == 18 and
        {                                   // ..generate 11 to 138 zero codes
        j = get_code(7) + 11;               // get repeat code

        if (i + j > n)                      // q. past end of array?
            return(TRUE);                   // a. yes .. return with error

        while (j--)                         // else .. loop filling table
            ll[i++] = 0;                    // ..with zero length

        l = 0;                              // ..and save new last length
        }
    }

inf_free(tl);                               // finally, free literal tree

bl = 9;                                     // length of literal bit codes

if (inf_build_tree(ll, nl, 257,             // q. build literal table ok?
            cll, cle, &tl, &bl))
    return(TRUE);                           // a. no .. return with error

bd = 6;                                     // length of distance bit codes

if (inf_build_tree(ll + nl, nd, 0,          // q. build distance table ok?
            cdo, cde, &td, &bd))
    return(TRUE);                           // a. no .. return with error

if (inf_codes(tl, td, bl, bd))              // q. inflate block ok?
    return(TRUE);                           // a. no .. return with error

inf_free(tl);                               // free literal trees
inf_free(td);                               // ..and distance trees
return(FALSE);                              // then finally, return all ok

}



/* ******************************************************************** *
 *
 *  extract_inflate() -- extract a deflated file
 *
 * ******************************************************************** */
static
void    extract_inflate(LF *lfp)            // local file pointer
{
UINT    c,                                  // current read character
        eoj = 0;                            // end of job flag


if (lfp->lf_flag & LF_FLAG_DDREC)           // q. need data descriptor rec?
    quit_with(0/*not_supported*/);               // a. yes .. quit w/error msg

e_count = -e_count;                         // set end count to negative

while (NOT eoj)                             // loop till end of job
    {
    eoj = get_code(1);                      // get the eoj bit
    c = get_code(2);                        // ..then get block type

    switch (c)                              // depending on block type
        {
        case 0:                             // 0: stored block
            bsize = 0;                      // flush remaining bits
            c = get_code(16);               // get block length ok

            //fgetc(ifile);                   // skip the ..
            //fgetc(ifile);                   // ..ones complement word
            curptr += 2;
            rsize -= 2;                     // ..and its count

            extract_copy(c);                // copy bytes to output stream
            break;                          // ..and get next block

        case 1:                             // 1: fixed Huffman codes
            eoj |= inf_fixed();             // process a fixed block
            break;                          // ..then get another block

        case 2:                             // 2: dynamic Huffman codes
            eoj |= inf_dynamic();           // process the dynamic block
            break;                          // ..then get next block

        case 3:                             // 3: unknown type
            quit_with(0/*data_error*/);          // quit with an error message
        }
    }

}



/* ******************************************************************** *
 *
 *  extract_explode() -- extract an imploded file
 *
 * ******************************************************************** */
static
void    extract_explode(LF *lfp)            // local file pointer
{
UINT    c,                                  // current read character
        ltf,                                // literal S-F tree available
        db,                                 // dictionary read bits
        d,                                  // distance
        len,                                // ..and length to go back
        mml;                                // minimum match length 3
BYTE   *p;                                  // work dictionary pointer
SFT    *sft, *sft2, *sft3;                  // S-F trees pointers


sft = (SFT *) malloc_chk(                   // get memory for S-F trees
            (256 + 64 + 64) * sizeof(SFT));

sft2 = &sft[256];                           // ..and set up ..
sft3 = &sft[320];                           // ..the base pointers

db = (lfp->lf_flag & LF_FLAG_8K) ? 7 : 6;   // ..and dictionary read in bits

mml = ((ltf = lfp->lf_flag & LF_FLAG_3SF)   // set literal S-F tree available
            != 0) ? 3 : 2;                  // ..and minimum match lengths

if (ltf)                                    // q. literal tree available?
    exp_load_tree(sft, 256);                // a. yes .. load literal tree

exp_load_tree(sft2, 64);                    // ..then load length trees
exp_load_tree(sft3, 64);                    // ..and finally, distance trees

for (;;)                                    // loop processing compressed data
    {
    if ((c = get_code(1)) == 0xffff)        // q. get a bit ok?
        break;                              // a. no .. exit loop

    if (c)                                  // q. encoded literal data?
        {                                   // a. yes .. continue processing
        if (ltf)                            // q. literal S-F tree available?
            {                               // a. yes .. get char from tree
            if ((c = exp_read(sft)) == 0xffff)  // q. get char from tree ok?
                break;                          // a. no .. exit loop
            }
         else if ((c = get_code(8)) == 0xffff)  // q. get next character ok?
            break;                              // a. no .. exit loop

        store_char(c);                      // ..and put char to output stream
        }
     else                                   // else .. use sliding dictionary
        {
        if ((d = get_code(db)) == 0xffff)   // q. get distance code ok?
            break;                          // a. no .. exit loop

        if ((c = exp_read(sft3)) == 0xffff) // q. get distance S-F code ok?
            break;                          // a. no .. exit loop

        d = (d | (c << db)) + 1;            // update distance

        if ((len = exp_read(sft2)) == 0xffff)   // q. get length S-F code ok?
            break;                              // a. no .. exit loop

        if (len == 63)                      // q. get max amount?
            {                               // a. yes .. get another byte
            if ((c = get_code(8)) == 0xffff)// q. get additional len ok?
                break;                      // a. no .. exit loop

            len += c;                       // ..then add to overall length
            }

        len += mml;                         // add in minimum match length

        if ((sbp - sb) >= d)                // q. backward wrap?
            p = sbp - d;                    // a. no .. just back up a bit
         else
            p = sb_size - d + sbp;          // else .. find at end of buffer

        while (len--)                       // copy previously outputed
            {                               // ..strings from sliding buffer
            store_char(*p);                 // put out each character

            if (++p >= sbe)                 // q. hit the end of the buffer?
                p = sb;                     // a. yes .. back to beginning
            }
        }
    }

free(sft);                                  // free S-F trees

}



/* ******************************************************************** *
 *
 *  extract_expand() -- extract a reduced file
 *
 * ******************************************************************** */
static
void    extract_expand(LF *lfp)             // local file pointer
{
UINT    i, j, k,                            // loop variables
        c,                                  // current character
        save_reduce,                        // reduction character
        reduce_m,                           // mask
        reduce_s,                           // shift value
        exp_len;                            // expand length
BYTE    last_c = 0,                         // last character
        state = 0,                          // state machine indicator
       *p;                                  // work pointer
FS     *fsp,                                // follower sets pointer
       *fse;                                // ..and entry
static
BYTE    reduce[4][2] =                      // reduction mask and lengths
            {
            { 0x7f, 7 },
            { 0x3f, 6 },
            { 0x1f, 5 },
            { 0x0f, 4 }
            },
        len_codes[33] =                     // bit lengths for numbers
            { 1, 1, 1, 2, 2, 3, 3, 3, 3, 4, // this table maps the minimum
              4, 4, 4, 4, 4, 4, 4, 5, 5, 5, // ..number of bits to represent
              5, 5, 5, 5, 5, 5, 5, 5, 5, 5, // ..a value
              5, 5, 5
            };


fsp = (FS *) malloc_chk(256 * sizeof(FS));  // allocate memory for sets

i = lfp->lf_cm - LF_CM_REDUCED1;            // get index into array
reduce_m = reduce[i][0];                    // ..copy over mask
reduce_s = reduce[i][1];                    // ..and shift amount

for (i = 256, fse = &fsp[255]; i--; fse--)  // build follower sets
    {
    if ((j = get_code(6)) == 0xffff)        // q. get a length code ok?
        break;                              // a. no .. exit loop

    fse->set_len = j;                       // save length of set data

    for (p = fse->set; j--; p++)            // set up length in set
        {
        if ((k = get_code(8)) == 0xffff)    // q. get a data code ok?
            break;                          // a. no .. exit loop

        *p = (char) k;                      // save set data
        }
    }

for (;;)                                    // loop till file processed
    {
    fse = &fsp[last_c];                     // current follower set

    if (NOT fse->set_len)                   // q. empty set?
        {                                   // a. yes .. get more input
        if ((c = get_code(8)) == 0xffff)    // q. get a code ok?
            break;                          // a. no .. exit loop
        }
     else
        {
        if ((c = get_code(1)) == 0xffff)    // q. get a code ok?
            break;                          // a. no .. exit loop

        if (c)                              // q. need to get another byte?
            {                               // a. yes .. get another
            if ((c = get_code(8)) == 0xffff)    // q. get a code ok?
                break;                          // a. no .. exit loop
            }
         else
            {
            i = len_codes[fse->set_len];    // get next read bit length

            if ((c = get_code(i)) == 0xffff)// q. get next code ok?
                break;                      // a. no .. exit loop

            c = fse->set[c];                // get encoded character
            }
        }

    last_c = c;                             // set up new last character

    switch (state)                          // based on current state
        {
        case 0:                             // 0: output character
            if (c == EXPLODE_DLE)           // q. DLE character?
                state = 1;                  // a. yes .. change states
             else
                store_char(c);              // else .. output character
            break;                          // ..then process next character


        case 1:                             // 1: store length
            if (NOT c)                      // q. null character?
                {
                store_char(EXPLODE_DLE);    // a. yes .. output a DLE char
                state = 0;                  // ..and change states
                }
             else
                {
                save_reduce = c;            // save character being reduced
                c &= reduce_m;              // clear unused bits
                exp_len = c;                // save length to expand
                state = (c == reduce_m)     // select next state
                        ? 2 : 3;            // ..if char is special flag
                }
            break;                          // ..then process next character

        case 2:                             // 2: store length
            exp_len += c;                   // save length to expand
            state = 3;                      // select next state
            break;                          // ..then get next character

        case 3:                             // 3: expand string
            c = ((save_reduce >> reduce_s)  // compute offset backwards
                    << 8) + c + 1;
            exp_len += 3;                   // set up expansion length

            if ((sbp - sb) >= c)            // q. backward wrap?
                p = sbp - c;                // a. no .. just back up a bit
             else
                p = sb_size - c + sbp;      // else .. find at end of buffer

            while (exp_len--)               // copy previously outputed
                {                           // ..strings from sliding buffer
                store_char(*p);             // put out each character

                if (++p >= sbe)             // q. hit the end of the buffer?
                    p = sb;                 // a. yes .. back to beginning
                }

            state = 0;                      // change state back
            break;                          // ..and process next character
        }
    }

free(fsp);                                  // free follower sets

}



/* ******************************************************************** *
 *
 *  extract_shrunk() -- extract a LZW shrunk file
 *
 * ******************************************************************** */
static
void    extract_shrunk(void)                // local file pointer
{
int     b_c,                                // base code
        p_c,                                // previous code
        s_c,                                // saved code
        c;                                  // current code
UINT    cnt;                                // decode stack counter
char    cs = 9;                             // code size
SD      *dict,                         // main dictionary
        *d,                            // dictionary entry pointer
        *fd,                           // next free pointer
        *ld;                           // last entry
char    *decode_stack;                 // decode stack


dict = (SD *) malloc_chk(             // allocate dictionary trees
            sizeof(SD) * TABLE_SIZE);
decode_stack = (char *) malloc_chk(   // ..and allocate decode stack
            TABLE_SIZE);

ld = &dict[TABLE_SIZE];                     // get address of last entry

for (fd = d = &dict[257]; d < ld; d++)      // loop thru dictionary
    d->parent_c = FREE;                     // ..and make each one free

store_char(p_c = b_c = get_code(cs));       // get and store a code

for (;;)                                    // inner loop
    {
    cnt = 0;                                // reset decode stack

    if ((s_c = c = get_code(cs)) == -1)     // q. end of data?
        break;                              // a. yes .. exit loop

    if (c == 256)                           // q. special code?
        {                                   // a. yes .. get next code
        if ((c = get_code(cs)) == -1)       // q. get next code ok?
            quit_with(0/*data_error*/);          // a. no .. quit w/error msg

        if (c == 1)                         // q. 256,1 sequence?
            {
            cs++;                           // a. yes .. increase code size
            continue;                       // ..and get next character
            }
         else if (c == 2)                   // q. clear tree (256,2)?
            {                               // a. yes .. partially clear nodes
            for (d = &dict[257];            // loop thru dictionary..
                        d < fd; d++)        // ..starting past literals
                d->parent_c |= 0x8000;      // ..and mark as unused

            for (d = &dict[257];            // loop again thru dictionary..
                        d < fd; d++)        // ..checking each used node
                {
                c = d->parent_c & 0x7fff;       // get node's next pointer

                if (c >= 257)                   // q. uses another node?
                    dict[c].parent_c &= 0x7fff; // a. yes .. clear target
                }

            for (d = &dict[257];            // loop once more, this time
                        d < fd; d++)        // ..release unneeded entries
                if (d->parent_c & 0x8000)   // q. need to be cleared?
                    d->parent_c = FREE;     // a. yes .. set it to free

            for (d = &dict[257];            // loop thru dictionary to..
                        d < ld; d++)        // ..find the first free node
                if (d->parent_c == FREE)    // q. find a free entry?
                    break;                  // a. yes .. exit loop

            fd = d;                         // save next free dict node
            continue;                       // ..continue with inner loop
            }
        }


    if (c < 256)                            // q. literal code?
        store_char(b_c = c);                // a. yes .. put out literal

     else                                   // else .. must be .gt. 256
        {
        if(dict[c].parent_c == FREE)        // q. using new code?
            {
            decode_stack[cnt++] = b_c;      // a. yes .. store old character
            c = p_c;                        // set up search criteria
            }

        while (c > 255)                     // loop finding entries to use
            {
            d = &dict[c];                   // point to current entry
            decode_stack[cnt++] = d->c;     // put character into stack
            c = d->parent_c;                // get parent's code
            }

        store_char(b_c = c);                // put out first character

        while (cnt)                         // loop outputing from ..
            store_char(decode_stack[--cnt]);// ..decode stack
        }

    fd->parent_c = p_c;                     // store parent's code
    fd->c = b_c;                            // ..and its character
    p_c = s_c;                              // set up new parent code

    while (++fd < ld)                       // loop thru dictionary
        if (fd->parent_c == FREE)           // q. entry free?
            break;                          // a. yes .. done looping
    }

	free(decode_stack);                      // free decode stack
	free(dict);                              // ..and dictionary
}

/* ******************************************************************** *
 *
 *  extract_stored() -- extract a stored file
 *
 * ******************************************************************** */
static
void    extract_stored(void)                // local file pointer
{

extract_copy(rsize);                        // copy stored data to file

}



/* ******************************************************************** *
 *
 *  extract_zip() -- extract files from a ZIP file
 *
 *  This routine is a sub-function of the scan_zip() routine.  The
 *  extract_zip() routine is call with the following function calls.
 *
 *      0 = initialization call
 *      1 = current file number
 *      2 = completion call
 *
 * ******************************************************************** */
static
void    extract_zip(int fnc,                // function
                    LF *lfp)                // local file header pointer
{

switch (fnc)                                // based on function number
    {
    case 0:                                 // initialization call
        sb_size = (UINT) BUFFER_SIZE;       // set up the dictionary size
		if(sb==NULL) sb = (BYTE *) malloc_chk(sb_size); // get memory for output buffer
        sbe = &sb[sb_size];                 // set up end of buffer address
        break;                              // ..and return to caller

    case 1:                                 
        if (lfp->lf_flag & LH_FLAG_ENCRYPT) 
        {
			//Error(" No se admiten ficheros encriptados.\n");
			//ASSERT( FALSE );	
            break;                          
        }

        if ( extract_open(lfp) )
            break;                          

        rsize = lfp->lf_csize;              // remaining filesize to process
        bsize = 0;                          // ..bits in byte to process
        e_count = 2;                        // end of data counter/flag
        crc = -1;                           // clear current crc
        sbp = sb;                           // reset next output pointer
        last_kpal = 0;                      // clear keep alive timer
        //spin = 0;                           // ..next kpal character index
        memset(sb, 0, sb_size);             // ..and dictionary/output buffer

        switch (lfp->lf_cm)                 // based on compression method
            {
            case LF_CM_STORED:              // stored file
                extract_stored();           // process stored file
                break;                      // ..then do next file

            case LF_CM_SHRUNK:              // LZW shrunk file
                extract_shrunk();           // process shrunk file
                break;                      // ..then do next file

            case LF_CM_REDUCED1:            // reduced file #1
            case LF_CM_REDUCED2:            // reduced file #2
            case LF_CM_REDUCED3:            // reduced file #3
            case LF_CM_REDUCED4:            // reduced file #4
                extract_expand(lfp);        // process reduced file
                break;                      // ..then do next file

            case LF_CM_IMPLODED:            // imploded file
                extract_explode(lfp);       // process imploded file
                break;                      // ..then do next file

            case LF_CM_DEFLATED:            // deflated file
                extract_inflate(lfp);       // process deflated file
                break;                      // ..then do next file

            case LF_CM_TOKENIZED:           // tokenized file
            default:
                return;                     // ..and return to caller
            }

        extract_flush();                    // else .. flush our output buffer        		

        //if (rsize)                          // q. use up input stream?
        //    Error(data_error);				// a. no .. quit w/err message

        crc ^= 0xffffffffL;                 // finalize crc value

//        if (crc != lfp->lf_crc)             // q. crc match?
  //          Error(crc_error);				// a. no .. quit w/err message

/*      if (!sw_test)                        // q. testing files?
		{
			Error("ZIP Test Failed\n");
		}
*/
  
        break;                              // ..and return to caller

    case 2:                                 // completion call
		if(sb!=NULL)
		{
			free(sb);
			sb = NULL;
		}
        break;                              // then return to caller
    }
}


/* ******************************************************************** *
 *
 *  crc_32() -- build a crc 32 for each outputed character
 *  Copyright (c) 1994, Bob Flanders and Michael Holmes
 *
 * ******************************************************************** */
static
void    crc_32(unsigned char c,             // character to output
               unsigned long *crc)          // crc 32 field
{
static  unsigned
long    crc_32_table[256] =
    {
    0x000000000L, 0x077073096L, 0x0EE0E612CL, 0x0990951BAL, 0x0076DC419L, 0x0706AF48FL, 0x0E963A535L, 0x09E6495A3L,
    0x00EDB8832L, 0x079DCB8A4L, 0x0E0D5E91EL, 0x097D2D988L, 0x009B64C2BL, 0x07EB17CBDL, 0x0E7B82D07L, 0x090BF1D91L,
    0x01DB71064L, 0x06AB020F2L, 0x0F3B97148L, 0x084BE41DEL, 0x01ADAD47DL, 0x06DDDE4EBL, 0x0F4D4B551L, 0x083D385C7L,
    0x0136C9856L, 0x0646BA8C0L, 0x0FD62F97AL, 0x08A65C9ECL, 0x014015C4FL, 0x063066CD9L, 0x0FA0F3D63L, 0x08D080DF5L,
    0x03B6E20C8L, 0x04C69105EL, 0x0D56041E4L, 0x0A2677172L, 0x03C03E4D1L, 0x04B04D447L, 0x0D20D85FDL, 0x0A50AB56BL,
    0x035B5A8FAL, 0x042B2986CL, 0x0DBBBC9D6L, 0x0ACBCF940L, 0x032D86CE3L, 0x045DF5C75L, 0x0DCD60DCFL, 0x0ABD13D59L,
    0x026D930ACL, 0x051DE003AL, 0x0C8D75180L, 0x0BFD06116L, 0x021B4F4B5L, 0x056B3C423L, 0x0CFBA9599L, 0x0B8BDA50FL,
    0x02802B89EL, 0x05F058808L, 0x0C60CD9B2L, 0x0B10BE924L, 0x02F6F7C87L, 0x058684C11L, 0x0C1611DABL, 0x0B6662D3DL,
    0x076DC4190L, 0x001DB7106L, 0x098D220BCL, 0x0EFD5102AL, 0x071B18589L, 0x006B6B51FL, 0x09FBFE4A5L, 0x0E8B8D433L,
    0x07807C9A2L, 0x00F00F934L, 0x09609A88EL, 0x0E10E9818L, 0x07F6A0DBBL, 0x0086D3D2DL, 0x091646C97L, 0x0E6635C01L,
    0x06B6B51F4L, 0x01C6C6162L, 0x0856530D8L, 0x0F262004EL, 0x06C0695EDL, 0x01B01A57BL, 0x08208F4C1L, 0x0F50FC457L,
    0x065B0D9C6L, 0x012B7E950L, 0x08BBEB8EAL, 0x0FCB9887CL, 0x062DD1DDFL, 0x015DA2D49L, 0x08CD37CF3L, 0x0FBD44C65L,
    0x04DB26158L, 0x03AB551CEL, 0x0A3BC0074L, 0x0D4BB30E2L, 0x04ADFA541L, 0x03DD895D7L, 0x0A4D1C46DL, 0x0D3D6F4FBL,
    0x04369E96AL, 0x0346ED9FCL, 0x0AD678846L, 0x0DA60B8D0L, 0x044042D73L, 0x033031DE5L, 0x0AA0A4C5FL, 0x0DD0D7CC9L,
    0x05005713CL, 0x0270241AAL, 0x0BE0B1010L, 0x0C90C2086L, 0x05768B525L, 0x0206F85B3L, 0x0B966D409L, 0x0CE61E49FL,
    0x05EDEF90EL, 0x029D9C998L, 0x0B0D09822L, 0x0C7D7A8B4L, 0x059B33D17L, 0x02EB40D81L, 0x0B7BD5C3BL, 0x0C0BA6CADL,
    0x0EDB88320L, 0x09ABFB3B6L, 0x003B6E20CL, 0x074B1D29AL, 0x0EAD54739L, 0x09DD277AFL, 0x004DB2615L, 0x073DC1683L,
    0x0E3630B12L, 0x094643B84L, 0x00D6D6A3EL, 0x07A6A5AA8L, 0x0E40ECF0BL, 0x09309FF9DL, 0x00A00AE27L, 0x07D079EB1L,
    0x0F00F9344L, 0x08708A3D2L, 0x01E01F268L, 0x06906C2FEL, 0x0F762575DL, 0x0806567CBL, 0x0196C3671L, 0x06E6B06E7L,
    0x0FED41B76L, 0x089D32BE0L, 0x010DA7A5AL, 0x067DD4ACCL, 0x0F9B9DF6FL, 0x08EBEEFF9L, 0x017B7BE43L, 0x060B08ED5L,
    0x0D6D6A3E8L, 0x0A1D1937EL, 0x038D8C2C4L, 0x04FDFF252L, 0x0D1BB67F1L, 0x0A6BC5767L, 0x03FB506DDL, 0x048B2364BL,
    0x0D80D2BDAL, 0x0AF0A1B4CL, 0x036034AF6L, 0x041047A60L, 0x0DF60EFC3L, 0x0A867DF55L, 0x0316E8EEFL, 0x04669BE79L,
    0x0CB61B38CL, 0x0BC66831AL, 0x0256FD2A0L, 0x05268E236L, 0x0CC0C7795L, 0x0BB0B4703L, 0x0220216B9L, 0x05505262FL,
    0x0C5BA3BBEL, 0x0B2BD0B28L, 0x02BB45A92L, 0x05CB36A04L, 0x0C2D7FFA7L, 0x0B5D0CF31L, 0x02CD99E8BL, 0x05BDEAE1DL,
    0x09B64C2B0L, 0x0EC63F226L, 0x0756AA39CL, 0x0026D930AL, 0x09C0906A9L, 0x0EB0E363FL, 0x072076785L, 0x005005713L,
    0x095BF4A82L, 0x0E2B87A14L, 0x07BB12BAEL, 0x00CB61B38L, 0x092D28E9BL, 0x0E5D5BE0DL, 0x07CDCEFB7L, 0x00BDBDF21L,
    0x086D3D2D4L, 0x0F1D4E242L, 0x068DDB3F8L, 0x01FDA836EL, 0x081BE16CDL, 0x0F6B9265BL, 0x06FB077E1L, 0x018B74777L,
    0x088085AE6L, 0x0FF0F6A70L, 0x066063BCAL, 0x011010B5CL, 0x08F659EFFL, 0x0F862AE69L, 0x0616BFFD3L, 0x0166CCF45L,
    0x0A00AE278L, 0x0D70DD2EEL, 0x04E048354L, 0x03903B3C2L, 0x0A7672661L, 0x0D06016F7L, 0x04969474DL, 0x03E6E77DBL,
    0x0AED16A4AL, 0x0D9D65ADCL, 0x040DF0B66L, 0x037D83BF0L, 0x0A9BCAE53L, 0x0DEBB9EC5L, 0x047B2CF7FL, 0x030B5FFE9L,
    0x0BDBDF21CL, 0x0CABAC28AL, 0x053B39330L, 0x024B4A3A6L, 0x0BAD03605L, 0x0CDD70693L, 0x054DE5729L, 0x023D967BFL,
    0x0B3667A2EL, 0x0C4614AB8L, 0x05D681B02L, 0x02A6F2B94L, 0x0B40BBE37L, 0x0C30C8EA1L, 0x05A05DF1BL, 0x02D02EF8DL
    };

c ^= *(unsigned char *) crc;                // apply current crc to current character
*crc >>= 8;                                 // shift old value down
*crc ^= crc_32_table[c];                    // xor in new value

}

#pragma warning( default : 4005 )
#pragma warning( default : 4018 )
#pragma warning( default : 4305 )
#pragma warning( default : 4309 )  

#pragma pack(pop)

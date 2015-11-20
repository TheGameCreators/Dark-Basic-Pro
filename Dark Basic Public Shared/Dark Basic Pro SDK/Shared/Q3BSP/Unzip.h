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
#if !defined (__UNZIP_HH__)
#define __UNZIP_HH__

	#define COUNT(x) (sizeof(x) / sizeof(x[0])) // item count
	#define NOT         !                       // shorthand logical
	#define UINT        unsigned int            // unsigned integer	
	#define BYTE        unsigned char           // ..and unsigned character
	#define ULONG       unsigned long           // ..and unsigned long
	//#define MAX_PATH    79                      // maximum path length
	#define LAST(s)     s[strlen(s) - 1]        // last character in string
	#define TRUE        1                       // true value
	#define FALSE       0                       // false value
	#define BUFFER_SIZE 32768L                  // dictionary/output buffer size
	#define COPY_BUFFER 16384                   // copy buffer size
	#define FILE_ATTR   _A_NORMAL | _A_RDONLY   // file attrib to search on


	/* ******************************************************************** *
	 *
	 *  Header Definitions
	 *
	 * ******************************************************************** */

	struct lf                                   // local file header
		{
		ULONG lf_sig;                               // signature (0x04034b50)
		WORD  lf_extract,                           // vers needed to extract
			  lf_flag,                              // general purpose flag
			  lf_cm,                                // compression method
			  lf_time,                              // file time
			  lf_date;                              // ..and file date
		ULONG lf_crc,                               // CRC-32 for file
			  lf_csize,                             // compressed size
			  lf_size;                              // uncompressed size
		WORD  lf_fn_len,                            // file name length
			  lf_ef_len;                            // extra field length
		};


	struct dd                                   // data descriptor
		{
		ULONG dd_crc,                               // CRC-32 for file
			  dd_csize,                             // compressed size
			  dd_size;                              // uncompressed size
		};


	struct fh                                   // file header
		{
		ULONG  fh_sig;                               // signature (0x02014b50)
		WORD  fh_made,                              // version made by
			  fh_extract,                           // vers needed to extract
			  fh_flag,                              // general purpose flag
			  fh_cm,                                // compression method
			  fh_time,                              // file time
			  fh_date;                              // ..and file date
		ULONG  fh_crc,                               // CRC-32 for file
			  fh_csize,                             // compressed size
			  fh_size;                              // uncompressed size
		WORD  fh_fn_len,                            // file name length
			  fh_ef_len,                            // extra field length
			  fh_fc_len,                            // file comment length
			  fh_disk,                              // disk number
			  fh_attr;                              // internal file attrib
		ULONG  fh_eattr,                             // external file attrib
			  fh_offset;                            // offset of local header
		};


	struct ed                                   // end of central dir record
		{
		ULONG  ed_sig;                               // signature (0x06054b50)
		WORD  ed_disk,                              // this disk number
			  ed_cdisk,                             // disk w/central dir
			  ed_current,                           // current disk's dir entries
			  ed_total;                             // total dir entries
		ULONG  ed_size,                              // size of central dir
			  ed_offset;                            // offset of central dir
		WORD  ed_zc_len;                            // zip file comment length
		};


	typedef struct lf LF;                       // set up
	typedef struct fh FH;                       // ..structure
	typedef struct dd DD;                       // ..shorthands
	typedef struct ed ED;                       //


	#define LF_SIG              0x0403          // local file header signature
	#define FH_SIG              0x0201          // file header signature
	#define ED_SIG              0x0605          // end of central dir signature

												// general purpose flag
	#define LH_FLAG_ENCRYPT     0x01                // file is encrypted

													// for Method 6 - Imploding
	#define LF_FLAG_8K          0x02                // use 8k dictionary vs 4k
	#define LF_FLAG_3SF         0x04                // use 3 S-F trees vs 2

													// for Method 8 - Deflating
	#define LF_FLAG_NORM        0x00                // normal compression
	#define LF_FLAG_MAX         0x02                // maximum compression
	#define LF_FLAG_FAST        0x04                // fast compression
	#define LF_FLAG_SUPER       0x06                // super fast compression
	#define LF_FLAG_DDREC       0x08                // use data descriptor record

												// compression method
	#define LF_CM_STORED        0x00                // stored
	#define LF_CM_SHRUNK        0x01                // shrunk
	#define LF_CM_REDUCED1      0x02                // reduced with factor 1
	#define LF_CM_REDUCED2      0x03                // reduced with factor 2
	#define LF_CM_REDUCED3      0x04                // reduced with factor 3
	#define LF_CM_REDUCED4      0x05                // reduced with factor 4
	#define LF_CM_IMPLODED      0x06                // imploded
	#define LF_CM_TOKENIZED     0x07                // tokenized (not used)
	#define LF_CM_DEFLATED      0x08                // deflated



	/* ******************************************************************** *
	 *
	 *  LZW Support Structures
	 *
	 * ******************************************************************** */

	#define TABLE_SIZE          0x2001          // dictionary table size
	#define FREE                0xffff          // free entry value

	struct  dictionary                          // dictionary entry
		{
		WORD parent_c;                              // parent's code
		char c;                                     // replacement character
		};

	typedef struct dictionary SD;               // structure shorthand



	/* ******************************************************************** *
	 *
	 *  Expanded Structures
	 *
	 * ******************************************************************** */

	#define EXPLODE_DLE         144             // escape character
	#define EXPAND_BUFF         4096            // sliding buffer size

	struct  follower_set                        // follower set
		{
		BYTE set_len,                               // set length
			 set[32];                               // set
		};

	typedef struct follower_set FS;             // structure shorthand



	/* ******************************************************************** *
	 *
	 *  Shannon-Fano Tree Structure
	 *
	 * ******************************************************************** */

	struct  shannon_fano_tree                   // Shannon-Fano trees
		{
		BYTE  blen,                                  // bit length
			  value;                                 // value
		WORD code;                                  // tree bit code
		};

	typedef struct shannon_fano_tree SFT;       // structure shorthand



	/* ******************************************************************** *
	 *
	 *  Huffman Tree Structure
	 *
	 * ******************************************************************** */

	struct  huffman_tree                        // Huffman trees
		{
		BYTE eb,                                    // extra bits
			 blen;                                  // bit length
		union
			{
			int	   code;                            // literal, len or distance
			struct huffman_tree *table;             // chain pointer
			} v;
		};

	typedef struct huffman_tree HUFF;           // structure shorthand

	
#endif
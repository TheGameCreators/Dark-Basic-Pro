/* zlib.h -- interface of the 'zlib' general purpose compression library
  version 1.1.4, March 11th, 2002

  Copyright (C) 1995-2002 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly jloup@gzip.org
  Mark Adler madler@alumni.caltech.edu

*/

#if !defined(AFX_ZIPSINGLEFILE_H__75D6971C_BC1D_4B3E_BA9E_19FBB7471C7E__INCLUDED_)
#define AFX_ZIPSINGLEFILE_H__75D6971C_BC1D_4B3E_BA9E_19FBB7471C7E__INCLUDED_

#include <tchar.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef _ZLIB
	#ifndef _ZLIB_INCLUDED
		#define _ZLIB_INCLUDED
		#pragma message("	_Adding zlib library\n")
		#pragma comment(lib, "zlib.lib")
	#endif
#endif

#ifndef LPCTSTR
	#define LPCTSTR const TCHAR*
#endif
#ifndef LPTSTR
	#define LPTSTR TCHAR*
#endif

/*! \mainpage CGZip, a C++ wrapper for gzip


This documents presents CGZip, C++ class wrapper for the gzip methods, which are included in the zlib libary.
The intention of this class is to have a <b>simple</b> class for zipping-unzipping buffers.

  The main features of the class are:
	- Compress, decompress LPCTSTR to file,
	- Compress, decompress memory buffer to file,
	- Non-MFC,
	- Hides zlib so you don't have to distribute the zlib headers,
	- UNICODE compliant.

\author Jonathan de Halleux, dehalleux@pelikhan.com, 2002
\version 1.1
*/

/*!
	\defgroup ZlibGroup ZLib Classes
	\ingroup PGLGroup
*/


/*! \brief Zlib namespace

  \ingroup ZlibGroup
*/
namespace zlib
{

class CZipper
{
public:
	//! void pointer to void
	typedef void* voidp;

	//! \brief Archive mode
	static enum EArchiveMode
	{
		//! Archive Mode Closed
		ArchiveModeClosed,
		//! Writing to file
		ArchiveModeWrite = 1,
		//! Reading from file
		ArchiveModeRead = 2
	};

	//! \brief Compression mode
	static enum ECompressionMode
	{
		//! no compression
		CompressionModeNoCompression= 0,
		//! Fast compression
		CompressionModeFast = 1,
		//! Best compression
		CompressionModeBest= 9,
		//! Default compression
		CompressionModeDefault = -1
	};

	/*! \brief Strategy

	The strategy parameter is used to tune the compression algorithm. Use the
	value #StrategyDefault for normal data, #StrategyFiltered for data produced by a
   filter (or predictor), or #StrategyHuffmanOnly to force Huffman encoding only (no
   string match).  
   
	 Filtered data consists mostly of small values with a
	somewhat random distribution. In this case, the compression algorithm is
   tuned to compress them better. The effect of #StrategyFiltered is to force more
   Huffman coding and less string matching; it is somewhat intermediate
   between #StrategyDefault and #StrategyHuffmanOnly. 
   
	 The strategy parameter only affects
   the compression ratio but not the correctness of the compressed output even
   if it is not set appropriately.

	*/
	static enum EStrategy
	{
		//! Default
		StrategyDefault,	
		//! filtered data
		StrategyFiltered=1,
		//! Huffman codes only
		StrategyHuffmanOnly=2
	};

	//! Flush modes
	static enum EFlushMode
	{
		FlushModeNoFlush=   0,
		FlushModeSync =		2,
		FlushModeFull=		3,
		FlushModeFinish =	4
	};

	//! Default constructor, no arguments
	CZipper();
	virtual ~CZipper();

	//! returns zlib version
	static LPCTSTR GetVersion()									{	return m_stsVersion;};

	/*! \brief Sets compression level
	
	  \param eCompression compression level

	\sa ECompressionMode
	*/
	void SetCompression( ECompressionMode eCompression)			{	m_eCompressionMode = eCompression; UpdateParams();};
	/*! \brief returns compression

	\sa SetCompression
	*/
	ECompressionMode GetCompression() const						{	return m_eCompressionMode;};

	/*! \brief Sets the strategy

	\sa EStrategy
	*/
	void SetStrategy( EStrategy eStrategy)						{	m_eStrategy = eStrategy; UpdateParams();};
	/*! \brief Returns the compression strategy 
	*/
	EStrategy GetStrategy() const								{	return m_eStrategy;};

	/*! \brief Start zip/unzip process

	\param eArchiveMode archive mode, see #EOpenMode
	\return true if file opened succesfully, false otherwize

	#Open can be used to read a file which is not in gzip format; in this
	#ReadBuffer and #ReadBufferSize will directly read from the file without decompression.
	*/
	virtual bool Open(EArchiveMode eArchiveMode);

	/*! \brief Closes the file

	\return true if succesful, false otherwize

	If writing, flushes the pending writing operator and closes the file. 
	*/
	virtual bool Close()=0;

	//! returns true if file is currently opened
	virtual bool IsOpen() const=0;
	
	//! returns true if zip is in write mode
	bool IsWriting() const				{	return m_eArchiveMode == ArchiveModeWrite;};
	//! returns true if zip is in read mode
	bool IsReading() const				{	return m_eArchiveMode == ArchiveModeRead;};

	/*!  \brief Writes buffer to zip file

	\param pBuffer memory buffer
	\param nBytes size in bytes of pBuffer
	\return true if writing succesful
	*/
	virtual bool WriteBuffer( voidp pBuffer, size_t nBytes)=0;
	/*! \brief Writes const string to zip file

	\param str the string to zip
	\return true if writing succesful
	*/
	virtual bool WriteString( LPCTSTR str)=0;

	/*! \brief Reads buffer

	\param pBuffer pointer to memory buffer of size nBytes
	\param nBytes size of the memory buffer
	\return number of bytes filled in memory buffer
	*/
	virtual int ReadBufferSize( voidp pBuffer, size_t nBytes)=0;

	/*! \brief Reads buffer

	\param ppBuffer pointer to pointer where new memory buffer will be stored
	\param nBytes size of new memory buffer in bytes
	*/
	virtual int ReadBuffer( voidp* ppBuffer, size_t& nBytes)=0;

protected:
	//! Update compression and strategy parameters (only in write mode)
	virtual void UpdateParams()=0;

	//! Archive opening mode
	EArchiveMode m_eArchiveMode;
	//! compression level
	ECompressionMode m_eCompressionMode;
	//! strategy
	EStrategy m_eStrategy;
	//! ZLib version string
	static LPCTSTR m_stsVersion;
};

/*! \brief GZip class (minimal class wrapper for the zlib methods)

\author Jonathan de Halleux, dehalleux@pelikhan.com, 2002

\par Description

This documents present CGZip, C++ class wrapper for the gzip methods, which are included in the zlib libary.
The intention of this class is to have a <b>simple</b> class for zipping-unzipping buffers.

  The main features of the class are:
	- Compress, decompress LPCTSTR to file,
	- Compress, decompress memory buffer to file,
	- Non-MFC,
	- Hides zlib so you don't have to distribute the zlib headers,
	- UNICODE compliant,

\par Examples
	
In the following examples, we shall consider:
  \code
// CGZip is in the zlib namespace
using namespace zlib;
CGZip gzip;
\endcode
	
<em>Zipping a memory buffer or a string</em>
First of all, let's open a file for writing:
\code
if(!gzip.Open(_T("myFile.gz"), CGZip::ArchiveModeWrite)))
{
	// the file could not be opened...
}
\endcode
As you can see, Open returns true if success and false otherwize. All the methods (almost) have this behavior.

Now we can send data and strings to the file:
- Strings:
\code
LPCTSTR szString = _T("This is a test string\n");
// writing string
gzip.WriteString(szString);
\endcode
- Buffers:
\code
void* pBuffer; // a memory buffer
int len;		// size in bytes of the memory buffer
// writing buffer
gzip.WriteBuffer(pBuffer,len);
\endcode

When done, you must close the file:
\code
gzip.Close();
\endcode
Note that if gzip goes out of scope, it is automatically closed.

<em>Unzipping a buffer or a string</em>

The reading behaves pretty much like the writing: First of all, open a file for reading:
\code
if(!gzip.Open(_T("myFile.gz"), CGZip::ArchiveModeRead)))
{
	// the file could not be opened...
}
\endcode

Now, you can read a buffer with fixed length or the hole file:
- a string
LPTSTR szString;
gzip.ReadString(&szString);
- the hole file
  \code 
void* pBuffer=NULL;
int len;
// reads and unzip the hole file, len contains the resulting size of pBuffer
gzip.ReadBuffer(&pBuffer,&len);
\endcode
- a fixed length buffer:

  \code
  int len;
char* pBuffer[len];
// reads and unzip the hole file, len contains the resulting size of pBuffer
gzip.ReadBufferSize(pBuffer,len);
\endcode

\par GZip license

  This piece of code is totally, uterly free for commercial and non-commercial use. Howerver, it uses 
zlib so you should check the zlib license before using it:

\par ZLib license

  This class uses zlib.

    The 'zlib' compression library provides in-memory compression and
  decompression functions, including integrity checks of the uncompressed
  data.  This version of the library supports only one compression method
  (deflation) but other algorithms will be added later and will have the same
  stream interface.

     Compression can be done in a single step if the buffers are large
  enough (for example if an input file is mmap'ed), or can be done by
  repeated calls of the compression function.  In the latter case, the
  application must provide more input and/or consume the output
  (providing more output space) before each call.

     The library also supports reading and writing files in gzip (.gz) format
  with an interface similar to that of stdio.

     The library does not install any signal handler. The decoder checks
  the consistency of the compressed data, so the library should never
  crash even in case of corrupted input.
  
\code
  zlib.h -- interface of the 'zlib' general purpose compression library
  version 1.1.4, March 11th, 2002

  Copyright (C) 1995-2002 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  jloup@gzip.org          madler@alumni.caltech.edu


  The data format used by the zlib library is described by RFCs (Request for
  Comments) 1950 to 1952 in the files ftp://ds.internic.net/rfc/rfc1950.txt
  (zlib format), rfc1951.txt (deflate format) and rfc1952.txt (gzip format).
\endcode


*/
class CGZip : public CZipper
{
public:
	CGZip():m_gzf(NULL),m_bufferSize(0){};
	virtual ~CGZip();

	/*! \brief Open a file

	\param szFileName name of the file to open
	\param eArchiveMode archive mode, see #EOpenMode
	\return true if file opened succesfully, false otherwize

	#Open can be used to read a file which is not in gzip format; in this
	#ReadBuffer and #ReadBufferSize will directly read from the file without decompression.
	*/
	bool Open(LPCTSTR szFileName, EArchiveMode eArchiveMode);

	/*! \brief Closes the file

	\return true if succesful, false otherwize

	If writing, flushes the pending writing operator and closes the file. 
	*/
	virtual bool Close();
	/*! brief Flushes all pending output into the compressed file.

	\param eFlush See EFlushMode
	\return true if succesful, false otherwize
	
     #Flush should be called only when strictly necessary because it can
	degrade compression.
	*/
	bool Flush( EFlushMode eFlush);

	//! returns true if file is currently opened
	virtual bool IsOpen() const	{	return m_gzf != NULL;};
	//! returns true if end of file has been reached
	bool IsEOF() const;

	/*!  \brief Writes buffer to zip file

	\param pBuffer memory buffer
	\param nBytes size in bytes of pBuffer
	\return true if writing succesful
	*/
	bool WriteBuffer( voidp pBuffer, size_t nBytes);
	/*! \brief Writes const string to zip file

	\param str the string to zip
	\return true if writing succesful
	*/
	bool WriteString( LPCTSTR str);

	/*! \brief Reads buffer

	\param pBuffer pointer to memory buffer of size nBytes
	\param nBytes size of the memory buffer
	\return number of bytes filled in memory buffer
	*/
	int ReadBufferSize( voidp pBuffer, size_t nBytes);

	/*! \brief Reads buffer

	\param ppBuffer pointer to pointer where new memory buffer will be stored
	\param nBytes size of new memory buffer in bytes
	*/
	int ReadBuffer( voidp* ppBuffer, size_t& nBytes);

	/*! \brief Reads String

	\param ppString pointer to pointer where new memory buffer will be stored
	*/
	int ReadString( LPTSTR* ppString);

protected:
	//! Update compression and strategy parameters (only in write mode)
	void UpdateParams();

	//! zip file handle
	voidp m_gzf;
	size_t m_bufferSize;
};


};

#endif // !defined(AFX_ZIPSINGLEFILE_H__75D6971C_BC1D_4B3E_BA9E_19FBB7471C7E__INCLUDED_)

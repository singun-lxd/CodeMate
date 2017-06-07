// 此文件是从网上下载的，用于检查字符串是否符合文件名命名规则

// IsValidFileName.cpp :  实现文件
// 

#include "stdafx.h"
#include "IsValidFileName.h"

///////////////////////////////////////////////////////////////////////////
// Strings are only for error display, they are not used by
// the IsValidFileName functions

const TCHAR *pInvalidFileNameErrStr[] =
{
	_T("Error"),
	_T("CLOCK$"),
	_T("AUX"),
	_T("CON"),
	_T("NUL"),
	_T("PRN"),
	_T("COM1"),
	_T("COM2"),
	_T("COM3"),
	_T("COM4"),
	_T("COM5"),
	_T("COM6"),
	_T("COM7"),
	_T("COM8"),
	_T("COM9"),
	_T("LPT1"),
	_T("LPT2"),
	_T("LPT3"),
	_T("LPT4"),
	_T("LPT5"),
	_T("LPT6"),
	_T("LPT7"),
	_T("LPT8"),
	_T("LPT9"),
	NULL
};

///////////////////////////////////////////////////////////////////////////
// const TCHAR *GetIsValidFileNameErrStr(int err)
// 
// 	Return an string associated with the passed error code
// 
// Parameters
// 
// 	err   A negative error number returned by IsValidFileName
// 
// Returns
// 
// 	A pointer to the device string
// 
// 	NULL if err is not in the range of the INVALID_FILENAME_ enumeration.
// 

const TCHAR *GetIsValidFileNameErrStr(int err)
{
	if (err >= 0 || err < INVALID_FILENAME_LPT9)
		return pInvalidFileNameErrStr[0];

	return pInvalidFileNameErrStr[(-err) - 1];
}

///////////////////////////////////////////////////////////////////////////
// int IsValidFileName(const char *pFileName)
// 
// 	Ensure a file name is legal.
// 
// Parameters
// 
// 	pFileName	The file name to check. This must be only the file name.
// 				If a full path is passed, the check will fail.
// 
// Returns
// 
// 	Zero on success.
// 
// 	Non-Zero on failure.
// 
// 		The return code can be used to determine why the call failed:
// 
// 		>0		The illegal character that was used. If the value is a
// 				dot ('.', 46) the file name was nothing but dots.
// 
// 		-1		A NULL or zero length file name was passed, or the file
// 				name exceeded 255 characters.
// 
// 		<-1		A device name was used. The value corresponds to the
// 				INVALID_FILENAME_... series of enumerations. You can pass
// 				this value to GetIsValidFileNameErrStr to obtain a pointer to
// 				the name of this device.
// 
// Remarks
// 
// The NT file naming convention specifies that:
// 
// 	- All characters greater than ASCII 31 to be used except for the following:
// 
// 		"*/:<>?\|
// 
// 	- A file may not be only dots
// 
// 	- The following device names cannot be used for a file name nor may they
// 	  be used for the first segment of a file name (that part which precedes the
// 	  first dot):
// 
// 	  CLOCK$, AUX, CON, NUL, PRN, COM1, COM2, COM3, COM4, COM5, COM6, COM7, COM8,
// 	  COM9, LPT1, LPT2, LPT3, LPT4, LPT5, LPT6, LPT7, LPT8, LPT9
// 
// 	- Device names are case insensitve. aux, AUX, Aux, etc. are identical. 
// 
// 	The ANSI and UNICODE functions are identical except for the declaration of the
// 	argument. A template function could have been used, but I chose not to since
// 	there are only two valid cases.
// 
// 	The algorithm used looks convoluted because it is highly optimized. It is
// 	more than 11.5 times faster than if scanning method using _strnicmp
// 	was used.
// 

int IsValidFileName(const char *pFileName)
{
	if (!pFileName || !*pFileName)
		return ISVALID_FILENAME_ERROR;

	int nonDot = -1;	// position of the first non dot in the file name
	int dot = -1;		// position of the first dot in the file name
	int len = 0;		// length of the file name

	// If a non-dot character has been encountered

	for (; len < 256 && pFileName[len]; len++)
	{
		if (pFileName[len] == '.')
		{
			if (dot < 0)
				dot = len;
			continue;
		}
		else if (nonDot < 0)
			nonDot = len;

		// The upper characters can be passed with a single check and
		// since only the backslash and bar are above the ampersand
		// it saves memory to do the check this way with little performance
		// cost.
		if (pFileName[len] >= '@')
		{
			if (pFileName[len] == '\\' || pFileName[len] == '|')
				return pFileName[len];

			continue;
		}

		static bool isCharValid[32] =
		{
		// ' '   !     "      #     $     %     &     '     (     )     *      +     ,      -     .      / 
			true, true, false, true, true, true, true, true, true, true, false, true, true,  true, true,  false,
		// 0     1     2      3     4     5     6     7     8     9     :      ;     <      =     >      ?
			true, true, true,  true, true, true, true, true, true, true, false, true, false, true, false, false
		// 0     1     2      3     4     5     6     7     8     9     :      ;     <      =     >      ?
		};

		// This is faster, at the expense of memory, than checking each
		// invalid character individually. However, either method is much
		// faster than using strchr().
		if (pFileName[len] >= 32)
		{
			if (isCharValid[pFileName[len] - 32])
				continue;
		}
		return pFileName[len];
	}

	if (len == 256)
		return ISVALID_FILENAME_ERROR;

	// if nonDot is still -1, no non-dots were encountered, return a dot (period)
	if (nonDot < 0)
		return '.';

	// if the first character is a dot, the filename is okay
	if (dot == 0)
		return 0;

	// if the file name has a dot, we only need to check up to the first dot
	if (dot > 0)
		len = dot;

	// Since the device names aren't numerous, this method of checking is the
	// fastest. Note that each character is checked with both cases.
	if (len == 3)
	{
		if (pFileName[0] == 'a' || pFileName[0] == 'A')
		{
			if ((pFileName[1] == 'u' || pFileName[1] == 'U') &&
				(pFileName[2] == 'x' || pFileName[2] == 'X'))
				return INVALID_FILENAME_AUX;
		}
		else if (pFileName[0] == 'c' || pFileName[0] == 'C')
		{
			if ((pFileName[1] == 'o' || pFileName[1] == 'O') &&
				(pFileName[2] == 'n' || pFileName[2] == 'N'))
				return INVALID_FILENAME_CON;
		}
		else if (pFileName[0] == 'n' || pFileName[0] == 'N')
		{
			if ((pFileName[1] == 'u' || pFileName[1] == 'U') &&
				(pFileName[2] == 'l' || pFileName[2] == 'L'))
				return INVALID_FILENAME_NUL;
		}
		else if (pFileName[0] == 'p' || pFileName[0] == 'P')
		{
			if ((pFileName[1] == 'r' || pFileName[1] == 'R') &&
				(pFileName[2] == 'n' || pFileName[2] == 'N'))
				return INVALID_FILENAME_PRN;
		}
	}
	else if (len == 4)
	{
		if (pFileName[0] == 'c' || pFileName[0] == 'C')
		{
			if ((pFileName[1] == 'o' || pFileName[1] == 'O') &&
				(pFileName[2] == 'm' || pFileName[2] == 'M') &&
				(pFileName[3] >= '1' || pFileName[3] <= '9'))
				return INVALID_FILENAME_COM1 - (pFileName[3] - '1');
		}
		else if (pFileName[0] == 'l' || pFileName[0] == 'L')
		{
			if ((pFileName[1] == 'p' || pFileName[1] == 'P') &&
				(pFileName[2] == 't' || pFileName[2] == 'T') &&
				(pFileName[3] >= '1' || pFileName[3] <= '9'))
				return INVALID_FILENAME_LPT1 - (pFileName[3] - '1');
		}
	}
	else if (len == 6)
	{
		if ((pFileName[0] == 'c' || pFileName[0] == 'C') &&
			(pFileName[1] == 'l' || pFileName[1] == 'L') &&
			(pFileName[2] == 'o' || pFileName[2] == 'O') &&
			(pFileName[3] == 'c' || pFileName[3] == 'C') &&
			(pFileName[4] == 'k' || pFileName[4] == 'K') &&
			(pFileName[5] == '$' || pFileName[5] == '$'))
			return INVALID_FILENAME_CLOCK;
	}

	return 0;
}

int IsValidFileName(const wchar_t *pFileName)
{
	if (!pFileName || !*pFileName)
		return ISVALID_FILENAME_ERROR;

	int nonDot = -1;	// position of the first non dot in the file name
	int dot = -1;		// position of the first dot in the file name
	int len = 0;		// length of the file name
	for (; len < 256 && pFileName[len]; len++)
	{
		if (pFileName[len] == '.')
		{
			if (dot < 0)
				dot = len;
			continue;
		}
		else if (nonDot < 0)
			nonDot = len;

		// The upper characters can be passed with a single check and
		// since only the backslash and bar are above the ampersand
		// it saves memory to do the check this way with little performance
		// cost.
		if (pFileName[len] >= '@')
		{
			if (pFileName[len] == '\\' || pFileName[len] == '|')
				return pFileName[len];

			continue;
		}

		static bool isCharValid[32] =
		{
		// ' '   !     "      #     $     %     &     '     (     )     *      +     ,      -     .      / 
			true, true, false, true, true, true, true, true, true, true, false, true, true,  true, true,  false,
		// 0     1     2      3     4     5     6     7     8     9     :      ;     <      =     >      ?
			true, true, true,  true, true, true, true, true, true, true, false, true, false, true, false, false
		// 0     1     2      3     4     5     6     7     8     9     :      ;     <      =     >      ?
		};

		// This is faster, at the expense of memory, than checking each
		// invalid character individually. However, either method is much
		// faster than using strchr().
		if (pFileName[len] >= 32)
		{
			if (isCharValid[pFileName[len] - 32])
				continue;
		}
		return pFileName[len];
	}

	if (len == 256)
		return ISVALID_FILENAME_ERROR;

	// if nonDot is still -1, no non-dots were encountered, return a dot (period)
	if (nonDot < 0)
		return '.';

	// if the first character is a dot, the filename is okay
	if (dot == 0)
		return 0;

	// if the file name has a dot, we only need to check up to the first dot
	if (dot > 0)
		len = dot;

	// Since the device names aren't numerous, this method of checking is the
	// fastest. Note that each character is checked with both cases.
	if (len == 3)
	{
		if (pFileName[0] == 'a' || pFileName[0] == 'A')
		{
			if ((pFileName[1] == 'u' || pFileName[1] == 'U') &&
				(pFileName[2] == 'x' || pFileName[2] == 'X'))
				return INVALID_FILENAME_AUX;
		}
		else if (pFileName[0] == 'c' || pFileName[0] == 'C')
		{
			if ((pFileName[1] == 'o' || pFileName[1] == 'O') &&
				(pFileName[2] == 'n' || pFileName[2] == 'N'))
				return INVALID_FILENAME_CON;
		}
		else if (pFileName[0] == 'n' || pFileName[0] == 'N')
		{
			if ((pFileName[1] == 'u' || pFileName[1] == 'U') &&
				(pFileName[2] == 'l' || pFileName[2] == 'L'))
				return INVALID_FILENAME_NUL;
		}
		else if (pFileName[0] == 'p' || pFileName[0] == 'P')
		{
			if ((pFileName[1] == 'r' || pFileName[1] == 'R') &&
				(pFileName[2] == 'n' || pFileName[2] == 'N'))
				return INVALID_FILENAME_PRN;
		}
	}
	else if (len == 4)
	{
		if (pFileName[0] == 'c' || pFileName[0] == 'C')
		{
			if ((pFileName[1] == 'o' || pFileName[1] == 'O') &&
				(pFileName[2] == 'm' || pFileName[2] == 'M') &&
				(pFileName[3] >= '1' || pFileName[3] <= '9'))
				return INVALID_FILENAME_COM1 - (pFileName[3] - '1');
		}
		else if (pFileName[0] == 'l' || pFileName[0] == 'L')
		{
			if ((pFileName[1] == 'p' || pFileName[1] == 'P') &&
				(pFileName[2] == 't' || pFileName[2] == 'T') &&
				(pFileName[3] >= '1' || pFileName[3] <= '9'))
				return INVALID_FILENAME_LPT1 - (pFileName[3] - '1');
		}
	}
	else if (len == 6)
	{
		if ((pFileName[0] == 'c' || pFileName[0] == 'C') &&
			(pFileName[1] == 'l' || pFileName[1] == 'L') &&
			(pFileName[2] == 'o' || pFileName[2] == 'O') &&
			(pFileName[3] == 'c' || pFileName[3] == 'C') &&
			(pFileName[4] == 'k' || pFileName[4] == 'K') &&
			(pFileName[5] == '$' || pFileName[5] == '$'))
			return INVALID_FILENAME_CLOCK;
	}

	return 0;
}

/**************************************************************************
  Copyright 2002 Joseph Woodbury.

  Use of this file constitutes a full acceptance of the following license
  agreement:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistribution of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

  2. A fee cannot be charged for any redistribution.

  3. Full source must accompany any redistribution in binary form which
     exposes the interfaces of that source whether directly or indirectly.
  
  4. This software cannot be used in such as matter as to cause it,
     or portions of it, in source and/or binary forms, to be covered,
     or required to be disclosed, by the GNU Public License (GPL) and/or
     any similarly structured software license.
  
  5. Any binaries produced using this software must fully indemnify the
     author with a disclaimer at least as effective and comprehensive as
     the following:

  THIS SOFTWARE IS PROVIDED BY JOSEPH WOODBURY "AS IS" AND ANY
  EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL JOSEPH WOODBURY BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**************************************************************************/


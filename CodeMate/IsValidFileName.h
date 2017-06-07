
// IsValidFileName.h : Í·ÎÄ¼þ
// 
#ifndef __ISVALIDFILENAME_H__
#define __ISVALIDFILENAME_H__

#include <tchar.h>

enum
{
	ISVALID_FILENAME_ERROR = -1,
	INVALID_FILENAME_CLOCK = -2,
	INVALID_FILENAME_AUX   = -3,
	INVALID_FILENAME_CON   = -4,
	INVALID_FILENAME_NUL   = -5,
	INVALID_FILENAME_PRN   = -6,
	INVALID_FILENAME_COM1  = -7,
	INVALID_FILENAME_COM2  = -8,
	INVALID_FILENAME_COM3  = -9,
	INVALID_FILENAME_COM4  = -10,
	INVALID_FILENAME_COM5  = -11,
	INVALID_FILENAME_COM6  = -12,
	INVALID_FILENAME_COM7  = -13,
	INVALID_FILENAME_COM8  = -14,
	INVALID_FILENAME_COM9  = -15,
	INVALID_FILENAME_LPT1  = -16,
	INVALID_FILENAME_LPT2  = -17,
	INVALID_FILENAME_LPT3  = -18,
	INVALID_FILENAME_LPT4  = -19,
	INVALID_FILENAME_LPT5  = -20,
	INVALID_FILENAME_LPT6  = -21,
	INVALID_FILENAME_LPT7  = -22,
	INVALID_FILENAME_LPT8  = -23,
	INVALID_FILENAME_LPT9  = -24
};

const TCHAR *pInvalidFileNameErrStr[];
const TCHAR *GetIsValidFileNameErrStr(int err);

int IsValidFileName(const char *pFileName);
int IsValidFileName(const wchar_t *pFileName);

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

#endif

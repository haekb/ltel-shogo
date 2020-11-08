#include "RiotClientShell.h"
#include "client_de.h"
#include "DynArray.h"
#include "BitmapFont.h"
#include "TextHelper.h"
#include "ClientRes.h"
#include "mbstring.h"


HSURFACE CTextHelper::CreateSurfaceFromString (CClientDE* pClientDE, CBitmapFont* pFont, char* str, int nReplacementFont)
{
	if (!pClientDE || !pFont || !str) return DNULL;

	if (nReplacementFont == 0) nReplacementFont = IDS_REPLACEMENTFONT; 

	// If the font is not initialized, call a different function using the system font...

	if (!pFont->IsValid())
	{
		// hack to help localization - we need to know if this is a "selected" font or a "normal" font
		// this assumes font names that are selected end in an "s" identifier and the extension ".pcx"...
		DBOOL bSelected = (pFont->GetImageName()[strlen (pFont->GetImageName()) - 5] == 's');

		// determine the font height
		float nFontHeight = 0.0f;
		if (strcmp (pFont->GetClassName(), "CFont08") == 0)
		{
			nFontHeight = TextHelperGetFloatValFromStringID(pClientDE, IDS_HEIGHTCFONT08, 8.0f);
		}
		else if (strcmp (pFont->GetClassName(), "CFont12") == 0)
		{
			nFontHeight = TextHelperGetFloatValFromStringID(pClientDE, IDS_HEIGHTCFONT12, 12.0f);
		}
		else if (strcmp (pFont->GetClassName(), "CFont18") == 0)
		{
			nFontHeight = TextHelperGetFloatValFromStringID(pClientDE, IDS_HEIGHTCFONT18, 18.0f);
		}
		else if (strcmp (pFont->GetClassName(), "CFont28") == 0)
		{
			nFontHeight = TextHelperGetFloatValFromStringID(pClientDE, IDS_HEIGHTCFONT28, 28.0f);
		}
		
		// determine the preferred font width
		float nRatio = 0.5f;
		HCONSOLEVAR hVar = pClientDE->GetConsoleVar ("FontWidthHeightRatio");
		if (hVar)
		{
			nRatio = pClientDE->GetVarValueFloat (hVar);
		}
		float nFontWidth = nFontHeight * nRatio;
		
		// create the font definition
		HSTRING hstrFont = pClientDE->FormatString (nReplacementFont);
		if (hstrFont == NULL) hstrFont = pClientDE->FormatString (IDS_REPLACEMENTFONT);
		FONT fontdef (pClientDE->GetStringData(hstrFont), (int)nFontWidth, (int)nFontHeight);
		pClientDE->FreeString (hstrFont);

		// create the font color
		HDECOLOR hFontColor = DNULL;
		if (bSelected)
		{
			hFontColor = pClientDE->SetupColor1 (1.0f, 1.0f, 1.0f, DFALSE);
		}
		else
		{
			hFontColor = pClientDE->SetupColor1 (1.0f, 0.4196f, 0.0f, DFALSE);
		}

		// now call the actual function
		return CreateSurfaceFromString (pClientDE, &fontdef, str, hFontColor, DNULL, DTRUE);
	}

	// Determine the size of the surface needed

	int nLength = strlen (str);
	int nWidth = 0;
	for (int i = 0; i < nLength; i++)
	{
		nWidth += pFont->GetCharWidth (str[i]);
	}
	int nHeight = pFont->GetFontHeight();

	// create the surface

	HSURFACE hSurf = pClientDE->CreateSurface (nWidth, nHeight);
	if (!hSurf) return DNULL;

	// go through string and copy characters from font to new surface

	DRect rcSrc;
	rcSrc.top = 0;
	rcSrc.bottom = nHeight;

	int nDstPos = 0;

	for (i = 0; i < nLength; i++)
	{
		rcSrc.left = pFont->GetCharPos (str[i]);
		rcSrc.right = rcSrc.left + pFont->GetCharWidth (str[i]);

		pClientDE->DrawSurfaceToSurface (hSurf, pFont->GetFontSurface(), &rcSrc, nDstPos, 0);
		
		nDstPos += rcSrc.right - rcSrc.left;
	}

	return hSurf;
}

HSURFACE CTextHelper::CreateSurfaceFromString (CClientDE* pClientDE, CBitmapFont* pFont, int strID, int nReplacementFont)
{
	if (!pClientDE) return DNULL;

	HSTRING hStr = pClientDE->FormatString (strID);
	if (!hStr) return DNULL;

	HSURFACE hSurf = CreateSurfaceFromString (pClientDE, pFont, pClientDE->GetStringData (hStr), nReplacementFont);

	pClientDE->FreeString (hStr);

	return hSurf;
}

HSURFACE CTextHelper::CreateWrappedStringSurface (CClientDE* pClientDE, int nWidth, CBitmapFont* pFont, char* str, int nAlignment, DBOOL bCrop)
{
	if (!str || !pClientDE) return DNULL;

	// If the font is not initialized, call a different function using the system font...

	if (!pFont->IsValid())
	{
		// hack to help localization - we need to know if this is a "selected" font or a "normal" font
		// this assumes font names that are selected end in an "s" identifier and the extension ".pcx"...
		DBOOL bSelected = (pFont->GetImageName()[strlen (pFont->GetImageName()) - 5] == 's');

		// determine the font height
		float nFontHeight = 0.0f;
		if (strcmp (pFont->GetClassName(), "CFont08") == 0)
		{
			nFontHeight = TextHelperGetFloatValFromStringID(pClientDE, IDS_HEIGHTCFONT08, 8.0f);
		}
		else if (strcmp (pFont->GetClassName(), "CFont12") == 0)
		{
			nFontHeight = TextHelperGetFloatValFromStringID(pClientDE, IDS_HEIGHTCFONT12, 12.0f);
		}
		else if (strcmp (pFont->GetClassName(), "CFont18") == 0)
		{
			nFontHeight = TextHelperGetFloatValFromStringID(pClientDE, IDS_HEIGHTCFONT18, 18.0f);
		}
		else if (strcmp (pFont->GetClassName(), "CFont28") == 0)
		{
			nFontHeight = TextHelperGetFloatValFromStringID(pClientDE, IDS_HEIGHTCFONT28, 28.0f);
		}
		
		// determine the preferred font width
		float nRatio = 0.5f;
		HCONSOLEVAR hVar = pClientDE->GetConsoleVar ("FontWidthHeightRatio");
		if (hVar)
		{
			nRatio = pClientDE->GetVarValueFloat (hVar);
		}
		float nFontWidth = nFontHeight * nRatio;
		
		// create the font definition
		HSTRING hstrFont = pClientDE->FormatString (IDS_REPLACEMENTFONT);
		FONT fontdef (pClientDE->GetStringData(hstrFont), (int)nFontWidth, (int)nFontHeight);
		pClientDE->FreeString (hstrFont);

		// create the font color
		HDECOLOR hFontColor = DNULL;
		if (bSelected)
		{
			hFontColor = pClientDE->SetupColor1 (1.0f, 1.0f, 1.0f, DFALSE);
		}
		else
		{
			hFontColor = pClientDE->SetupColor1 (1.0f, 0.4196f, 0.0f, DFALSE);
		}

		// now call the actual function
		return CreateWrappedStringSurface (pClientDE, nWidth, &fontdef, str, hFontColor, DNULL, nAlignment, DTRUE);
	}
	
	CDynArray<DDWORD> surfaces (1, 2);		// cannot create a dynarray of HSURFACES - compiler error C2926
	DDWORD nSurfaces = 0;

	char* ptr = str;
	char* ptrStart = ptr;
	char* ptrEnd = DNULL;
	do
	{
		ptrEnd = ptr + strlen (ptr);

		DBOOL bShortened = DFALSE;
		while (ptrEnd > ptrStart)
		{
			int nChars = ptrEnd - ptrStart;
			int nTestWidth = 0;
			for (int i = 0; i < nChars; i++)
			{
				nTestWidth += pFont->GetCharWidth (ptrStart[i]);
			}

			if (nTestWidth <= nWidth)
			{
				bShortened = DTRUE;
				break;
			}
			else
			{
				// find the previous space back from ptrEnd
				ptrEnd--;
				while (ptrEnd > ptrStart && *ptrEnd != ' ') ptrEnd--;
			}
		}
		
		// see if we were successful in coming up with a shortened version
		if (!bShortened)
		{
			// couldn't shorten the remaining string - just create a surface
			// from it and add it to the array
			surfaces[nSurfaces] = (DDWORD) CreateSurfaceFromString (pClientDE, pFont, ptrStart);
		}
		else
		{
			// we shortened it - create a surface and add it to the array
			char temp = *ptrEnd;
			*ptrEnd = '\0';			
			surfaces[nSurfaces] = (DDWORD) CreateSurfaceFromString (pClientDE, pFont, ptrStart);
			*ptrEnd = temp;
		}

		// see if we could create the surface

		if (!surfaces[nSurfaces])
		{
			for (DDWORD i = 0; i < nSurfaces; i++)
			{
				pClientDE->DeleteSurface ((HSURFACE)surfaces[i]);
			}
			return DNULL;
		}
		nSurfaces++;
		
		if (!bShortened)
		{
			break;
		}
		else
		{
			ptrStart = ptrEnd + 1;
		}

	} while (*ptrEnd != '\0');

	// ok, now we should have an array of surfaces that we can combine into one large one...

	if (!nSurfaces) return DNULL;

	// get the final surface height and potential (shortened) width

	DDWORD nTotalHeight = 0;
	DDWORD nCroppedWidth = 0;
	for (DDWORD i = 0; i < nSurfaces; i++)
	{
		DDWORD nSurfWidth = 0;
		DDWORD nSurfHeight = 0;
		pClientDE->GetSurfaceDims ((HSURFACE)surfaces[i], &nSurfWidth, &nSurfHeight);

		nTotalHeight += nSurfHeight;
		if (nSurfWidth > nCroppedWidth) nCroppedWidth = nSurfWidth;
	}

	// create the final surface

	DDWORD nFinalWidth = bCrop ? nCroppedWidth : nWidth;
	HSURFACE hFinalSurface = pClientDE->CreateSurface (nFinalWidth, nTotalHeight);
	if (!hFinalSurface)
	{
		for (DDWORD i = 0; i < nSurfaces; i++)
		{
			pClientDE->DeleteSurface ((HSURFACE)surfaces[i]);
		}
		return DNULL;
	}
	pClientDE->FillRect ((HSURFACE)hFinalSurface, DNULL, DNULL);

	// draw the string surfaces onto final one

	int y = 0;
	for (i = 0; i < nSurfaces; i++)
	{
		DDWORD nSurfWidth = 0;
		DDWORD nSurfHeight = 0;
		pClientDE->GetSurfaceDims ((HSURFACE)surfaces[i], &nSurfWidth, &nSurfHeight);

		int x = 0;
		switch (nAlignment)
		{
			case TH_ALIGN_CENTER:	x = ((int)nFinalWidth - (int)nSurfWidth) / 2;	break;
			case TH_ALIGN_RIGHT:	x = (int)nFinalWidth - (int)nSurfWidth;			break;
		}
		pClientDE->DrawSurfaceToSurface (hFinalSurface, (HSURFACE)surfaces[i], DNULL, x, y);
		y += nSurfHeight;

		// delete this surface since we don't need it anymore

		pClientDE->DeleteSurface ((HSURFACE)surfaces[i]);
	}

	return hFinalSurface;
}

HSURFACE CTextHelper::CreateWrappedStringSurface (CClientDE* pClientDE, int nWidth, CBitmapFont* pFont, int strID, int nAlignment, DBOOL bCrop)
{
	if (!pClientDE) return DNULL;

	HSTRING hStr = pClientDE->FormatString (strID);
	if (!hStr) return DNULL;

	HSURFACE hSurf = CreateWrappedStringSurface (pClientDE, nWidth, pFont, pClientDE->GetStringData (hStr), nAlignment, bCrop);

	pClientDE->FreeString (hStr);

	return hSurf;
}

HSURFACE CTextHelper::CreateSurfaceFromString (CClientDE* pClientDE, FONT* pFontDef, char* str, HDECOLOR foreColor, HDECOLOR backColor, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pClientDE) return DNULL;

	HDEFONT hFont = pClientDE->CreateFont (pFontDef->strFontName, 
										   pFontDef->nWidth,
										   pFontDef->nHeight,
										   pFontDef->bItalic,
										   pFontDef->bUnderline,
										   pFontDef->bBold);
	if (!hFont) return DNULL;

	HSTRING hString = pClientDE->CreateString (str);
	if (!hString)
	{
		pClientDE->DeleteFont (hFont);
		return DNULL;
	}

	HSURFACE hSurface = CreateSurface (pClientDE, hFont, hString, foreColor, backColor, bCropped, nExtraX, nExtraY);

	pClientDE->DeleteFont (hFont);
	pClientDE->FreeString (hString);

	return hSurface;
}

HSURFACE CTextHelper::CreateSurfaceFromString (CClientDE* pClientDE, HDEFONT hFont, char* str, HDECOLOR foreColor, HDECOLOR backColor, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pClientDE) return DNULL;

	HSTRING hString = pClientDE->CreateString (str);
	if (!hString) return DNULL;

	HSURFACE hSurface = CreateSurface (pClientDE, hFont, hString, foreColor, backColor, bCropped, nExtraX, nExtraY);

	pClientDE->FreeString (hString);

	return hSurface;
}

HSURFACE CTextHelper::CreateSurfaceFromString (CClientDE* pClientDE, FONT* pFontDef, int strID, HDECOLOR foreColor, HDECOLOR backColor, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pClientDE) return DNULL;

	HDEFONT hFont = pClientDE->CreateFont (pFontDef->strFontName, 
										   pFontDef->nWidth,
										   pFontDef->nHeight,
										   pFontDef->bItalic,
										   pFontDef->bUnderline,
										   pFontDef->bBold);
	if (!hFont) return DNULL;

	HSTRING hString = pClientDE->FormatString (strID);
	if (!hString)
	{
		pClientDE->DeleteFont (hFont);
		return DNULL;
	}

	HSURFACE hSurface = CreateSurface (pClientDE, hFont, hString, foreColor, backColor, bCropped, nExtraX, nExtraY);

	pClientDE->DeleteFont (hFont);
	pClientDE->FreeString (hString);

	return hSurface;
}

HSURFACE CTextHelper::CreateSurfaceFromString (CClientDE* pClientDE, HDEFONT hFont, int strID, HDECOLOR foreColor, HDECOLOR backColor, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pClientDE) return DNULL;

	HSTRING hString = pClientDE->FormatString (strID);
	if (!hString) return DNULL;

	HSURFACE hSurface = CreateSurface (pClientDE, hFont, hString, foreColor, backColor, bCropped, nExtraX, nExtraY);

	pClientDE->FreeString (hString);

	return hSurface;
}




HSURFACE CTextHelper::CreateWrappedStringSurface (CClientDE* pClientDE, int nWidth, FONT* pFontDef, char* str, HDECOLOR foreColor, HDECOLOR backColor, int nAlignment, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pClientDE) return DNULL;

	HDEFONT hFont = pClientDE->CreateFont (pFontDef->strFontName, 
										   pFontDef->nWidth,
										   pFontDef->nHeight,
										   pFontDef->bItalic,
										   pFontDef->bUnderline,
										   pFontDef->bBold);
	if (!hFont) return DNULL;

	HSURFACE hSurface = CreateWrappedSurface (pClientDE, nWidth, hFont, str, foreColor, backColor, nAlignment, bCropped, nExtraX, nExtraY);

	pClientDE->DeleteFont (hFont);

	return hSurface;
}

HSURFACE CTextHelper::CreateWrappedStringSurface (CClientDE* pClientDE, int nWidth, HDEFONT hFont, char* str, HDECOLOR foreColor, HDECOLOR backColor, int nAlignment, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pClientDE) return DNULL;

	HSURFACE hSurface = CreateWrappedSurface (pClientDE, nWidth, hFont, str, foreColor, backColor, nAlignment, bCropped, nExtraX, nExtraY);

	return hSurface;
}

HSURFACE CTextHelper::CreateWrappedStringSurface (CClientDE* pClientDE, int nWidth, FONT* pFontDef, int strID, HDECOLOR foreColor, HDECOLOR backColor, int nAlignment, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pClientDE) return DNULL;

	HDEFONT hFont = pClientDE->CreateFont (pFontDef->strFontName, 
										   pFontDef->nWidth,
										   pFontDef->nHeight,
										   pFontDef->bItalic,
										   pFontDef->bUnderline,
										   pFontDef->bBold);
	if (!hFont) return DNULL;

	HSTRING hString = pClientDE->FormatString (strID);
	if (!hString)
	{
		pClientDE->DeleteFont (hFont);
		return DNULL;
	}

	HSURFACE hSurface = CreateWrappedSurface (pClientDE, nWidth, hFont, pClientDE->GetStringData (hString), foreColor, backColor, nAlignment, bCropped, nExtraX, nExtraY);

	pClientDE->DeleteFont (hFont);
	pClientDE->FreeString (hString);

	return hSurface;
}

HSURFACE CTextHelper::CreateWrappedStringSurface (CClientDE* pClientDE, int nWidth, HDEFONT hFont, int strID, HDECOLOR foreColor, HDECOLOR backColor, int nAlignment, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pClientDE) return DNULL;

	HSTRING hString = pClientDE->FormatString (strID);
	if (!hString) return DNULL;

	HSURFACE hSurface = CreateWrappedSurface (pClientDE, nWidth, hFont, pClientDE->GetStringData (hString), foreColor, backColor, nAlignment, bCropped, nExtraX, nExtraY);

	pClientDE->FreeString (hString);

	return hSurface;
}





HSURFACE CTextHelper::CreateShortenedStringSurface (CClientDE* pClientDE, int nWidth, FONT* pFontDef, char* str, HDECOLOR foreColor, HDECOLOR backColor, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pClientDE) return DNULL;

	HDEFONT hFont = pClientDE->CreateFont (pFontDef->strFontName, 
										   pFontDef->nWidth,
										   pFontDef->nHeight,
										   pFontDef->bItalic,
										   pFontDef->bUnderline,
										   pFontDef->bBold);
	if (!hFont) return DNULL;

	HSURFACE hSurface = CreateShortenedSurface (pClientDE, nWidth, hFont, str, foreColor, backColor, bCropped, nExtraX, nExtraY);

	pClientDE->DeleteFont (hFont);

	return hSurface;
}

HSURFACE CTextHelper::CreateShortenedStringSurface (CClientDE* pClientDE, int nWidth, HDEFONT hFont, char* str, HDECOLOR foreColor, HDECOLOR backColor, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pClientDE) return DNULL;

	HSURFACE hSurface = CreateShortenedSurface (pClientDE, nWidth, hFont, str, foreColor, backColor, bCropped, nExtraX, nExtraY);

	return hSurface;
}

HSURFACE CTextHelper::CreateShortenedStringSurface (CClientDE* pClientDE, int nWidth, FONT* pFontDef, int strID, HDECOLOR foreColor, HDECOLOR backColor, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pClientDE) return DNULL;

	HDEFONT hFont = pClientDE->CreateFont (pFontDef->strFontName, 
										   pFontDef->nWidth,
										   pFontDef->nHeight,
										   pFontDef->bItalic,
										   pFontDef->bUnderline,
										   pFontDef->bBold);
	if (!hFont) return DNULL;

	HSTRING hString = pClientDE->FormatString (strID);
	if (!hString)
	{
		pClientDE->DeleteFont (hFont);
		return DNULL;
	}

	HSURFACE hSurface = CreateShortenedSurface (pClientDE, nWidth, hFont, pClientDE->GetStringData (hString), foreColor, backColor, bCropped, nExtraX, nExtraY);

	pClientDE->DeleteFont (hFont);
	pClientDE->FreeString (hString);

	return hSurface;
}

HSURFACE CTextHelper::CreateShortenedStringSurface (CClientDE* pClientDE, int nWidth, HDEFONT hFont, int strID, HDECOLOR foreColor, HDECOLOR backColor, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pClientDE) return DNULL;

	HSTRING hString = pClientDE->FormatString (strID);
	if (!hString) return DNULL;

	HSURFACE hSurface = CreateShortenedSurface (pClientDE, nWidth, hFont, pClientDE->GetStringData (hString), foreColor, backColor, bCropped, nExtraX, nExtraY);

	pClientDE->FreeString (hString);

	return hSurface;
}


HSURFACE CTextHelper::CreateSurface (CClientDE* pClientDE, HDEFONT hFont, HSTRING hString, HDECOLOR foreColor, HDECOLOR backColor, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pClientDE) return DNULL;

	HSURFACE hSurf = pClientDE->CreateSurfaceFromString (hFont, hString, foreColor, backColor, nExtraX, nExtraY);
	if (bCropped)
	{
		HSURFACE hCropped = CropSurface (pClientDE, hSurf, backColor);
		if (hCropped)
		{
			pClientDE->DeleteSurface (hSurf);
			return hCropped;
		}
	}
	return hSurf;
}

HSURFACE CTextHelper::CreateWrappedSurface (CClientDE* pClientDE, int nWidth, HDEFONT hFont, char* pString, HDECOLOR foreColor, HDECOLOR backColor, int nAlignment, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pString || !pClientDE) return DNULL;

	// check if we are supposed to do hard wrapping
	bool bHardTextWrap = false;
	{
		HSTRING hStr = pClientDE->FormatString(IDS_ENABLETEXTHARDWRAP);
		if (hStr)
		{
			char* pComp = pClientDE->GetStringData(hStr);
			if (pComp != NULL)
			{
				if (stricmp(pComp,"TRUE") == 0) bHardTextWrap = true;
			}
			pClientDE->FreeString (hStr);
		}
	}

	// get period or other characters that are not supposed to be at start of a line
	char sPeriodChars[256] = ".";
	{
		HSTRING hStr = pClientDE->FormatString(IDS_EXCLUDELINESTARTCHARS);
		if (hStr)
		{
			char* pComp = pClientDE->GetStringData(hStr);
			if (pComp != NULL)
			{
				_mbsncpy((unsigned char*)sPeriodChars, (const unsigned char*)pComp, 255);
				sPeriodChars[255] = '\0';
			}
			pClientDE->FreeString (hStr);
		}
	}

	char* pWorkingString = new char [strlen (pString) + 1];
	if (!pWorkingString) return DNULL;

	CDynArray<DDWORD> surfaces (1, 2);		// cannot create a dynarray of HSURFACES - compiler error C2926
	DDWORD nSurfaces = 0;

	char* ptr = (char*) pString;
	while (*ptr != '\0')
	{
		// copy what's left into the working string

		strcpy (pWorkingString, ptr);

		// create a string that will fit into the desired width

		DBOOL bDone = DFALSE;
		HSURFACE hSurface = DNULL;
		while (!bDone)
		{
			// create a string surface from the working string to test

			HSTRING hString = pClientDE->CreateString (pWorkingString);
			if (!hString) break;

			hSurface = pClientDE->CreateSurfaceFromString (hFont, hString, foreColor, backColor, nExtraX, nExtraY);
			if (!hSurface)
			{
				pClientDE->FreeString (hString);
				break;
			}

			pClientDE->FreeString (hString);
			
			// get the dimensions of the surface

			DDWORD nTestWidth = 0;
			DDWORD nTestHeight = 0;
			pClientDE->GetSurfaceDims (hSurface, &nTestWidth, &nTestHeight);

			if (nTestWidth > (DDWORD)nWidth)
			{
				// string too long, remove some and try again

				char* pSpace;

				// remove by character if we are hard wrapping
				if (bHardTextWrap)
				{
					pSpace = pWorkingString;

					// find last hard in string pPrev
					char* pPrev = NULL;
					while (pSpace != NULL)
					{
						pPrev = pSpace;
						pSpace = (char*)_mbsinc ((const unsigned char*)pSpace);
						if (*pSpace == '\0') pSpace = NULL;
					}
					if (pPrev == NULL) pSpace = pWorkingString;
					else 
					{
						//decrement 1 character
						pSpace = (char*)_mbsdec ((const unsigned char*)pWorkingString, (const unsigned char*)pPrev);

						// check if we are on a period then we need to decrement 3 more
						if ((char*)_mbsspnp((const unsigned char*)pPrev, (const unsigned char*)sPeriodChars) != pPrev)
						{
							if (pSpace != NULL)	pSpace = (char*)_mbsdec ((const unsigned char*)pWorkingString, (const unsigned char*)pSpace);
							if (pSpace != NULL)	pSpace = (char*)_mbsdec ((const unsigned char*)pWorkingString, (const unsigned char*)pSpace);
						}
					}

				}

				// remove by word if hard wrap is not on
				else 
				{
					pSpace = (char*)_mbsrchr ((const unsigned char*)pWorkingString, ' ');
					if (!pSpace) pSpace = pWorkingString;

					while (_mbsnbcmp ((const unsigned char*)pSpace, (const unsigned char*)" ", 1) == 0 && pSpace != pWorkingString)
					{
						pSpace = (char*)_mbsdec ((const unsigned char*)pWorkingString, (const unsigned char*)pSpace);
						if (!pSpace) pSpace = pWorkingString;
					}
				}


				if (pSpace == pWorkingString)
				{
					bDone = DTRUE;
				}
				else
				{
					pSpace = (char*)_mbsinc ((const unsigned char*)pSpace);
					*pSpace = '\0';
				}

				pClientDE->DeleteSurface (hSurface);
				hSurface = DNULL;
			}
			else
			{
				// it fits!

				bDone = DTRUE;
			}
		}
		
		// if we got here without bDone being TRUE, there was an error
		if (!bDone)
		{
			delete [] pWorkingString;
			for (DDWORD i = 0; i < nSurfaces; i++)
			{
				pClientDE->DeleteSurface ((HSURFACE)surfaces[i]);
			}
			return DNULL;
		}

		// if bDone is true but there's no surface, we couldn't make it fit
		// just create a surface from the string and it will get clipped later
		if (bDone && !hSurface)
		{
			HSTRING hString = pClientDE->CreateString (pWorkingString);
			hSurface = pClientDE->CreateSurfaceFromString (hFont, hString, foreColor, backColor, nExtraX, nExtraY);
			if (!hString || !hSurface)
			{
				if (hString) pClientDE->FreeString (hString);
				if (hSurface) pClientDE->DeleteSurface (hSurface);

				delete [] pWorkingString;
				for (DDWORD i = 0; i < nSurfaces; i++)
				{
					pClientDE->DeleteSurface ((HSURFACE)surfaces[i]);
				}
				return DNULL;
			}
			pClientDE->FreeString (hString);
		}

		// add this surface to the array

		surfaces[nSurfaces] = (DDWORD) hSurface;
		nSurfaces++;

		// increment ptr to next character in the string

		ptr += strlen (pWorkingString);
		
		char* pPrev = NULL;
		while (_mbsnbcmp ((const unsigned char*)ptr, (const unsigned char*)" ", 1) == 0 && *ptr)
		{
			pPrev = ptr;
			ptr = (char*)_mbsinc ((const unsigned char*)ptr);
			if (!ptr) 
			{
				ptr = pPrev;
				break;
			}
		}
	}

	delete [] pWorkingString;

	// ok, now we should have an array of surfaces that we can combine into one large one...

	if (!nSurfaces) return DNULL;

	// crop the surfaces if they need to be cropped (leave a one-pixel border)

	if (bCropped)
	{
		for (DDWORD i = 0; i < nSurfaces; i++)
		{
			HSURFACE hCropped = CropSurface (pClientDE, (HSURFACE)surfaces[i], DNULL);
			if (hCropped)
			{
				pClientDE->DeleteSurface ((HSURFACE)surfaces[i]);
				surfaces[i] = (DDWORD) hCropped;
			}
		}
	}

	// get the final surface height

	DDWORD nTotalHeight = 0;
	for (DDWORD i = 0; i < nSurfaces; i++)
	{
		DDWORD nSurfWidth = 0;
		DDWORD nSurfHeight = 0;
		pClientDE->GetSurfaceDims ((HSURFACE)surfaces[i], &nSurfWidth, &nSurfHeight);

		nTotalHeight += nSurfHeight;
	}

	// create the final surface

	HSURFACE hFinalSurface = pClientDE->CreateSurface (nWidth, nTotalHeight);
	if (!hFinalSurface)
	{
		for (DDWORD i = 0; i < nSurfaces; i++)
		{
			pClientDE->DeleteSurface ((HSURFACE)surfaces[i]);
		}
		return DNULL;
	}
	pClientDE->FillRect ((HSURFACE)hFinalSurface, DNULL, DNULL);

	// draw the string surfaces onto final one

	int y = 0;
	for (i = 0; i < nSurfaces; i++)
	{
		DDWORD nSurfWidth = 0;
		DDWORD nSurfHeight = 0;
		pClientDE->GetSurfaceDims ((HSURFACE)surfaces[i], &nSurfWidth, &nSurfHeight);

		int x = 0;
		switch (nAlignment)
		{
			case TH_ALIGN_CENTER:	x = ((int)nWidth - (int)nSurfWidth) / 2;	break;
			case TH_ALIGN_RIGHT:	x = (int)nWidth - (int)nSurfWidth;			break;
		}
		pClientDE->DrawSurfaceToSurface (hFinalSurface, (HSURFACE)surfaces[i], DNULL, x, y);
		y += nSurfHeight;

		// delete this surface since we don't need it anymore

		pClientDE->DeleteSurface ((HSURFACE)surfaces[i]);
	}
	
	if (bCropped)
	{
		HSURFACE hCropped = CropSurface (pClientDE, hFinalSurface, backColor);
		if (hCropped)
		{
			pClientDE->DeleteSurface (hFinalSurface);
			return hCropped;
		}
	}

	return hFinalSurface;
}


HSURFACE CTextHelper::CreateShortenedSurface (CClientDE* pClientDE, int nWidth, HDEFONT hFont, char* pString, HDECOLOR foreColor, HDECOLOR backColor, DBOOL bCropped, int nExtraX, int nExtraY)
{
	if (!pString || !pClientDE) return DNULL;

	// do an initial test to see if the string already fits...

	HSTRING hString = pClientDE->CreateString (pString);
	if (!hString) return DNULL;

	HSURFACE hSurface = pClientDE->CreateSurfaceFromString (hFont, hString, foreColor, backColor, nExtraX, nExtraY);
	if (!hSurface)
	{
		pClientDE->FreeString (hString);
		return DNULL;
	}

	pClientDE->FreeString (hString);

	DDWORD nTestWidth = 0;
	DDWORD nTestHeight = 0;
	pClientDE->GetSurfaceDims (hSurface, &nTestWidth, &nTestHeight);

	if (nTestWidth <= (DDWORD) nWidth)
	{
		// string already fits!

		return hSurface;
	}

	// string doesn't initially fit - remove characters and add ellipses until it does

	int nStrLen = strlen (pString);
	
	// if string length is 3 or less, we cannot shorten it - return NULL

	if (nStrLen <= 3) return DNULL;

	char* pWorkingString = new char [nStrLen + 1];
	if (!pWorkingString) return DNULL;

	hSurface = DNULL;
	int nCurrentLen = nStrLen - 3;
	while (nCurrentLen != 0)
	{
		// copy what's left into the working string

		memset (pWorkingString, 0, nStrLen);
		strncpy (pWorkingString, pString, nCurrentLen);
		strcat (pWorkingString, "...");

		// create a string surface from the working string to test

		HSTRING hString = pClientDE->CreateString (pWorkingString);
		if (!hString) break;

		hSurface = pClientDE->CreateSurfaceFromString (hFont, hString, foreColor, backColor, nExtraX, nExtraY);
		if (!hSurface)
		{
			pClientDE->FreeString (hString);
			break;
		}

		pClientDE->FreeString (hString);
		
		// get the dimensions of the surface

		DDWORD nTestWidth = 0;
		DDWORD nTestHeight = 0;
		pClientDE->GetSurfaceDims (hSurface, &nTestWidth, &nTestHeight);

		if (nTestWidth <= (DDWORD)nWidth)
		{
			// string fits!!!
			break;
		}

		// string too long, remove a character and try again...

		pClientDE->DeleteSurface (hSurface);
		hSurface = DNULL;
		nCurrentLen--;
	}
		
	delete [] pWorkingString;

	if (bCropped)
	{
		HSURFACE hCropped = CropSurface (pClientDE, hSurface, backColor);
		if (hCropped)
		{
			pClientDE->DeleteSurface (hSurface);
			return hCropped;
		}
	}

	return hSurface;
}


HSURFACE CTextHelper::CropSurface (CClientDE* pClientDE, HSURFACE hSurface, HDECOLOR hBackColor)
{
	DDWORD nSurfWidth = 0;
	DDWORD nSurfHeight = 0;
	pClientDE->GetSurfaceDims (hSurface, &nSurfWidth, &nSurfHeight);
	
	HDECOLOR hBlack = pClientDE->SetupColor1(0.0f, 0.0f, 0.0f, DFALSE);
	DRect rcBorders;
	pClientDE->GetBorderSize (hSurface, hBlack, &rcBorders);

	DDWORD nCroppedWidth = nSurfWidth - rcBorders.left - rcBorders.right + 2;
	DDWORD nCroppedHeight = nSurfHeight - rcBorders.top - rcBorders.bottom + 2;
	HSURFACE hNewSurf = pClientDE->CreateSurface (nCroppedWidth, nCroppedHeight);
	
	if (!hNewSurf) return DNULL;
	if (nCroppedWidth > nSurfWidth || nCroppedHeight > nSurfHeight)
	{
		pClientDE->DeleteSurface (hNewSurf);
		return DNULL;
	}

	rcBorders.right = rcBorders.left + nCroppedWidth;
	rcBorders.bottom = rcBorders.top + nCroppedHeight;

	pClientDE->FillRect (hNewSurf, DNULL, DNULL);
	pClientDE->DrawSurfaceToSurface (hNewSurf, hSurface, &rcBorders, 1, 1);

	return hNewSurf;
}

// this function will check a string ID in cres.dll and if its contents are the same as the check string
// then the function returns TRUE if not it returns false
DBOOL TextHelperCheckStringID(CClientDE* pClientDE, int nStringID, const char* sCheck, DBOOL bIgnoreCase, DBOOL bDefaultVal)
{
	DBOOL bRetVal = bDefaultVal;
	{
		HSTRING hStr = pClientDE->FormatString(nStringID);
		if (hStr)
		{
			char* pComp = pClientDE->GetStringData(hStr);
			if (pComp != NULL)
			{
				if (bIgnoreCase)
				{
					if (stricmp(pComp, sCheck) == 0) bRetVal = DTRUE;
					else bRetVal = DFALSE;
				}
				else
				{
					if (strcmp(pComp, sCheck) == 0) bRetVal = DTRUE;
					else bRetVal = DFALSE;
				}
			}
			pClientDE->FreeString (hStr);
		}
	}
	return bRetVal;
}

float TextHelperGetFloatValFromStringID(CClientDE* pClientDE, int nStringID, float nDefaultVal)
{
	float fRetVal = nDefaultVal;
	{
		HSTRING hStr = pClientDE->FormatString(nStringID);
		if (hStr)
		{
			char* pComp = pClientDE->GetStringData(hStr);
			if (pComp != NULL)
			{
				sscanf(pComp, "%f", &fRetVal);
			}
			pClientDE->FreeString (hStr);
		}
	}
	return fRetVal;
}

#pragma warning( disable : 4244 )
int TextHelperGetIntValFromStringID(CClientDE* pClientDE, int nStringID, int nDefaultVal)
{  
	return (int)(TextHelperGetFloatValFromStringID(pClientDE, nStringID, nDefaultVal));
}
#pragma warning( default : 4244 )


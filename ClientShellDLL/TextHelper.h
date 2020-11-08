#ifndef __TEXTHELPER_H
#define __TEXTHELPER_H

class CBitmapFont;
class CClientDE;

// font structure and helper

struct FONT
{
	FONT (char* pStr, int w, int h, DBOOL i = DFALSE, DBOOL u = DFALSE, DBOOL b = DFALSE)
				{ SAFE_STRCPY(strFontName, pStr); nWidth = w; nHeight = h; bItalic = i; bUnderline = u; bBold = b; }

	FONT()		{ memset (strFontName, 0, 64); nWidth = 0; nHeight = 0; bItalic = DFALSE; bUnderline = DFALSE; bBold = DFALSE; }

	char		strFontName[64];
	int			nWidth;
	int			nHeight;
	DBOOL		bItalic;
	DBOOL		bUnderline;
	DBOOL		bBold;
};

#define SETFONT(font, str, w, h, i, u, b) SAFE_STRCPY(font.strFontName, str); font.nWidth = w; font.nHeight = h; font.bItalic = i; font.bUnderline = u; font.bBold = b

#define TH_ALIGN_LEFT		1
#define TH_ALIGN_CENTER		2
#define TH_ALIGN_RIGHT		3

// text helper class definition

class CTextHelper
{
public:

	static HSURFACE CreateSurfaceFromString (CClientDE* pClientDE, CBitmapFont* pFont, char* str, int nReplacementFont = 0);
	static HSURFACE CreateSurfaceFromString (CClientDE* pClientDE, CBitmapFont* pFont, int strID, int nReplacementFont = 0);
	static HSURFACE CreateWrappedStringSurface (CClientDE* pClientDE, int nWidth, CBitmapFont* pFont, char* str, int nAlignment = TH_ALIGN_LEFT, DBOOL bCrop = DTRUE);
	static HSURFACE CreateWrappedStringSurface (CClientDE* pClientDE, int nWidth, CBitmapFont* pFont, int strID, int nAlignment = TH_ALIGN_LEFT, DBOOL bCrop = DTRUE);

	static HSURFACE CreateSurfaceFromString (CClientDE* pClientDE, FONT* pFontDef, char* str, HDECOLOR foreColor, HDECOLOR backColor = DNULL, DBOOL bCropped = DFALSE, int nExtraX = 0, int nExtraY = 0);
	static HSURFACE CreateSurfaceFromString (CClientDE* pClientDE, HDEFONT hFont, char* str, HDECOLOR foreColor, HDECOLOR backColor = DNULL, DBOOL bCropped = DFALSE, int nExtraX = 0, int nExtraY = 0);
	static HSURFACE CreateSurfaceFromString (CClientDE* pClientDE, FONT* pFontDef, int strID, HDECOLOR foreColor, HDECOLOR backColor = DNULL, DBOOL bCropped = DFALSE, int nExtraX = 0, int nExtraY = 0);
	static HSURFACE CreateSurfaceFromString (CClientDE* pClientDE, HDEFONT hFont, int strID, HDECOLOR foreColor, HDECOLOR backColor = DNULL, DBOOL bCropped = DFALSE, int nExtraX = 0, int nExtraY = 0);
	
	static HSURFACE CreateWrappedStringSurface (CClientDE* pClientDE, int nWidth, FONT* pFontDef, char* str, HDECOLOR foreColor, HDECOLOR backColor = DNULL, int nAlignment = TH_ALIGN_LEFT, DBOOL bCropped = DFALSE, int nExtraX = 0, int nExtraY = 0);
	static HSURFACE CreateWrappedStringSurface (CClientDE* pClientDE, int nWidth, HDEFONT hFont, char* str, HDECOLOR foreColor, HDECOLOR backColor = DNULL, int nAlignment = TH_ALIGN_LEFT, DBOOL bCropped = DFALSE, int nExtraX = 0, int nExtraY = 0);
	static HSURFACE CreateWrappedStringSurface (CClientDE* pClientDE, int nWidth, FONT* pFontDef, int strID, HDECOLOR foreColor, HDECOLOR backColor = DNULL, int nAlignment = TH_ALIGN_LEFT, DBOOL bCropped = DFALSE, int nExtraX = 0, int nExtraY = 0);
	static HSURFACE CreateWrappedStringSurface (CClientDE* pClientDE, int nWidth, HDEFONT hFont, int strID, HDECOLOR foreColor, HDECOLOR backColor = DNULL, int nAlignment = TH_ALIGN_LEFT, DBOOL bCropped = DFALSE, int nExtraX = 0, int nExtraY = 0);

	static HSURFACE CreateShortenedStringSurface (CClientDE* pClientDE, int nWidth, FONT* pFontDef, char* str, HDECOLOR foreColor, HDECOLOR backColor = DNULL, DBOOL bCropped = DFALSE, int nExtraX = 0, int nExtraY = 0);
	static HSURFACE CreateShortenedStringSurface (CClientDE* pClientDE, int nWidth, HDEFONT hFont, char* str, HDECOLOR foreColor, HDECOLOR backColor = DNULL, DBOOL bCropped = DFALSE, int nExtraX = 0, int nExtraY = 0);
	static HSURFACE CreateShortenedStringSurface (CClientDE* pClientDE, int nWidth, FONT* pFontDef, int strID, HDECOLOR foreColor, HDECOLOR backColor = DNULL, DBOOL bCropped = DFALSE, int nExtraX = 0, int nExtraY = 0);
	static HSURFACE CreateShortenedStringSurface (CClientDE* pClientDE, int nWidth, HDEFONT hFont, int strID, HDECOLOR foreColor, HDECOLOR backColor = DNULL, DBOOL bCropped = DFALSE, int nExtraX = 0, int nExtraY = 0);

protected:

	static HSURFACE CreateSurface (CClientDE* pClientDE, HDEFONT hFont, HSTRING hString, HDECOLOR foreColor, HDECOLOR backColor, DBOOL bCropped, int nExtraX, int nExtraY);
	static HSURFACE CreateWrappedSurface (CClientDE* pClientDE, int nWidth, HDEFONT hFont, char* pString, HDECOLOR foreColor, HDECOLOR backColor, int nAlignment, DBOOL bCropped, int nExtraX, int nExtraY);
	static HSURFACE CreateShortenedSurface (CClientDE* pClientDE, int nWidth, HDEFONT hFont, char* pString, HDECOLOR foreColor, HDECOLOR backColor, DBOOL bCropped, int nExtraX, int nExtraY);

	static HSURFACE CropSurface (CClientDE* pClientDE, HSURFACE hSurface, HDECOLOR hBackColor);
};

DBOOL TextHelperCheckStringID(CClientDE* pClientDE, int nStringID, const char* sCheck, DBOOL bIgnoreCase = DTRUE, DBOOL bDefaultVal = DFALSE);

float TextHelperGetFloatValFromStringID(CClientDE* pClientDE, int nStringID, float nDefaultVal);

int TextHelperGetIntValFromStringID(CClientDE* pClientDE, int nStringID, int nDefaultVal);

#endif

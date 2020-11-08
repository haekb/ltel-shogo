#ifndef __INFODISPLAY_H
#define __INFODISPLAY_H

#include "ClientUtilities.h"
#include "TextHelper.h"

#define DI_LEFT			0x0001
#define DI_RIGHT		0x0002
#define DI_CENTER		0x0004
#define DI_TOP			0x0008
#define DI_BOTTOM		0x0010

struct DI_INFO
{
	DI_INFO()	{ hSurface = DNULL; szSurface.cx = 0; szSurface.cy = 0; bDeleteSurface = DTRUE; nLocationFlags = 0; nExpireTime = 0.0f; pNext = DNULL; }

	HSURFACE	hSurface;
	CSize		szSurface;
	DBOOL		bDeleteSurface;
	DDWORD		nLocationFlags;
	DFLOAT		nExpireTime;

	DI_INFO*	pNext;
};

class CInfoDisplay
{
public:

	CInfoDisplay()				{ m_pClientDE = DNULL; m_pInfoList = DNULL; }
	~CInfoDisplay()				{ Term(); }

	DBOOL			Init (CClientDE* pClientDE);
	void			Term ();

	// adds the current surface to the draw list without modifying the surface in any way
	DBOOL			AddInfo (HSURFACE hSurface, DFLOAT nSeconds = 3.0f, DDWORD nLocationFlags = DI_CENTER | DI_BOTTOM, DBOOL bDeleteSurface = DTRUE);
	
	// create an optimized surface using a bitmap font and add it to the draw list
	DBOOL			AddInfo (char* str, CBitmapFont* pFont, DFLOAT nSeconds = 3.0f, DDWORD nLocationFlags = DI_CENTER | DI_BOTTOM);
	DBOOL			AddInfo (int nStringID, CBitmapFont* pFont, DFLOAT nSeconds = 3.0f, DDWORD nLocationFlags = DI_CENTER | DI_BOTTOM);

	// create an optimized surface using a truetype font and add it to the draw list
	DBOOL			AddInfo (char* str, FONT* pFontDef, HDECOLOR hForeColor, DFLOAT nSeconds = 3.0f, DDWORD nLocationFlags = DI_CENTER | DI_BOTTOM);
	DBOOL			AddInfo (int nStringID, FONT* pFontDef, HDECOLOR hForeColor, DFLOAT nSeconds = 3.0f, DDWORD nLocationFlags = DI_CENTER | DI_BOTTOM);

	// draw all surfaces in the draw list
	void			Draw();

protected:
	
	DBOOL			AddToList (HSURFACE hSurface, DFLOAT nSeconds, DDWORD nLocationFlags, DBOOL bDeleteSurface);

protected:
	
	CClientDE*		m_pClientDE;

	DI_INFO*		m_pInfoList;
};

#endif

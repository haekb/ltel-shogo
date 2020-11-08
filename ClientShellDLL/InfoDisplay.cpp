#include "client_de.h"
#include "cpp_client_de.h"
#include "InfoDisplay.h"
#include "TextHelper.h"

DBOOL CInfoDisplay::Init (CClientDE* pClientDE)
{
	if (!pClientDE) return DFALSE;

	m_pClientDE = pClientDE;

	return DTRUE;
}

void CInfoDisplay::Term()
{
	if (!m_pClientDE) return;

	DI_INFO* pInfo = m_pInfoList;
	while (pInfo)
	{
		if (pInfo->hSurface && pInfo->bDeleteSurface)
		{
			m_pClientDE->DeleteSurface (pInfo->hSurface);
		}
		pInfo = pInfo->pNext;
	}

	DI_INFO* pPrev = DNULL;
	pInfo = m_pInfoList;
	while (pInfo)
	{
		pPrev = pInfo;
		pInfo = pInfo->pNext;
		delete pPrev;
	}

	m_pInfoList = DNULL;
	m_pClientDE = DNULL;
}

DBOOL CInfoDisplay::AddInfo (HSURFACE hSurface, DFLOAT nSeconds, DDWORD nLocationFlags, DBOOL bDeleteSurface)
{
	if (!m_pClientDE) return DFALSE;
	return AddToList (hSurface, nSeconds, nLocationFlags, bDeleteSurface);
}

DBOOL CInfoDisplay::AddInfo (char* str, CBitmapFont* pFont, DFLOAT nSeconds, DDWORD nLocationFlags)
{
	if (!m_pClientDE) return DFALSE;
	HSURFACE hSurf = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFont, str);
	if (!hSurf) return DFALSE;
	
	HDECOLOR hTrans = m_pClientDE->SetupColor1 (0.0f, 0.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (hSurf, hTrans);

	return AddToList (hSurf, nSeconds, nLocationFlags, DTRUE);
}

DBOOL CInfoDisplay::AddInfo (int nStringID, CBitmapFont* pFont, DFLOAT nSeconds, DDWORD nLocationFlags)
{
	if (!m_pClientDE) return DFALSE;
	HSURFACE hSurf = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFont, nStringID);
	if (!hSurf) return DFALSE;

	HDECOLOR hTrans = m_pClientDE->SetupColor1 (0.0f, 0.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (hSurf, hTrans);

	return AddToList (hSurf, nSeconds, nLocationFlags, DTRUE);
}

DBOOL CInfoDisplay::AddInfo (char* str, FONT* pFontDef, HDECOLOR hForeColor, DFLOAT nSeconds, DDWORD nLocationFlags)
{
	if (!m_pClientDE) return DFALSE;
	HSURFACE hSurf = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontDef, str, hForeColor);
	if (!hSurf) return DFALSE;

	HDECOLOR hTrans = m_pClientDE->SetupColor1 (0.0f, 0.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (hSurf, hTrans);

	return AddToList (hSurf, nSeconds, nLocationFlags, DTRUE);
}

DBOOL CInfoDisplay::AddInfo (int nStringID, FONT* pFontDef, HDECOLOR hForeColor, DFLOAT nSeconds, DDWORD nLocationFlags)
{
	if (!m_pClientDE) return DFALSE;
	HSURFACE hSurf = CTextHelper::CreateSurfaceFromString (m_pClientDE, pFontDef, nStringID, hForeColor);
	if (!hSurf) return DFALSE;

	HDECOLOR hTrans = m_pClientDE->SetupColor1 (0.0f, 0.0f, 0.0f, DTRUE);
	m_pClientDE->OptimizeSurface (hSurf, hTrans);

	return AddToList (hSurf, nSeconds, nLocationFlags, DTRUE);
}

void CInfoDisplay::Draw()
{
	if (!m_pClientDE) return;

	HSURFACE hScreen = m_pClientDE->GetScreenSurface();
	CSize szScreen;
	m_pClientDE->GetSurfaceDims (hScreen, &szScreen.cx, &szScreen.cy);

	DI_INFO* pInfo = m_pInfoList;
	while (pInfo)
	{
		int nY = 0;
		if (pInfo->nLocationFlags & DI_TOP)
		{
			nY = 20;
		}
		else if (pInfo->nLocationFlags & DI_BOTTOM)
		{
			nY = (int)szScreen.cy - 20 - (int)pInfo->szSurface.cy;
		}
		else
		{
			nY = ((int)szScreen.cy - (int)pInfo->szSurface.cy) / 2;
		}

		int nX = 0;
		if (pInfo->nLocationFlags & DI_LEFT)
		{
			nX = 20;
		}
		else if (pInfo->nLocationFlags & DI_RIGHT)
		{
			nX = (int)szScreen.cx - 20 - (int)pInfo->szSurface.cx;
		}
		else
		{
			nX = ((int)szScreen.cx - (int)pInfo->szSurface.cx) / 2;
		}

		m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, pInfo->hSurface, DNULL, nX, nY, DNULL);
		pInfo = pInfo->pNext;
	}

	DFLOAT nTime = m_pClientDE->GetTime();
	DI_INFO* pPrev = DNULL;
	DI_INFO* pNext = DNULL;
	pInfo = m_pInfoList;
	while (pInfo)
	{
		pNext = pInfo->pNext;
		
		if (pInfo->nExpireTime < nTime)
		{
			if (pInfo->hSurface && pInfo->bDeleteSurface)
			{
				m_pClientDE->DeleteSurface (pInfo->hSurface);
			}
	
			if (pPrev)
			{
				pPrev->pNext = pNext;
			}
			else
			{
				m_pInfoList = pNext;
			}

			delete pInfo;
		}
		else
		{
			pPrev = pInfo;
		}

		pInfo = pNext;
	}
}

DBOOL CInfoDisplay::AddToList (HSURFACE hSurface, DFLOAT nSeconds, DDWORD nLocationFlags, DBOOL bDeleteSurface)
{
	if (!m_pClientDE || !hSurface) return DFALSE;

	DFLOAT nTime = m_pClientDE->GetTime();

	// first see if there's already one in the list with the same position - if so, just replace it with this one

	DI_INFO* pInfo = m_pInfoList;
	while (pInfo)
	{
		if (pInfo->nLocationFlags == nLocationFlags)
		{
			if (pInfo->hSurface && pInfo->bDeleteSurface)
			{
				m_pClientDE->DeleteSurface (pInfo->hSurface);
			}

			pInfo->hSurface = hSurface;
			m_pClientDE->GetSurfaceDims (pInfo->hSurface, &pInfo->szSurface.cx, &pInfo->szSurface.cy);
			pInfo->bDeleteSurface = bDeleteSurface;
			pInfo->nLocationFlags = nLocationFlags;
			pInfo->nExpireTime = nTime + nSeconds;

			return DTRUE;
		}

		pInfo = pInfo->pNext;
	}

	// now add this one to the list

	pInfo = new DI_INFO;
	if (!pInfo) return DFALSE;

	pInfo->hSurface = hSurface;
	m_pClientDE->GetSurfaceDims (pInfo->hSurface, &pInfo->szSurface.cx, &pInfo->szSurface.cy);
	pInfo->bDeleteSurface = bDeleteSurface;
	pInfo->nLocationFlags = nLocationFlags;
	pInfo->nExpireTime = nTime + nSeconds;

	if (!m_pInfoList)
	{
		m_pInfoList = pInfo;
	}
	else
	{
		DI_INFO* ptr = m_pInfoList;
		while (ptr->pNext) ptr = ptr->pNext;
		ptr->pNext = pInfo;
	}

	return DTRUE;
}

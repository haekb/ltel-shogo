// ----------------------------------------------------------------------- //
//
// MODULE  : MissionObjectives.cpp
//
// PURPOSE : Riot's Mission Objective system - Implementation
//
// CREATED : 3/22/98
//
// ----------------------------------------------------------------------- //

#include "RiotClientShell.h"
#include "client_de.h"
#include "TextHelper.h"
#include "ClientRes.h"
#include "MissionObjectives.h"
#include "ClientRes.h"

#define TEXT_AREA_WIDTH 186
#define OPEN_ANIM_RATE 1000
#define X_LOCATION 25
#define Y_LOCATION 127
#define SCROLL_SPEED 200

CMissionObjectives::CMissionObjectives()
{
	m_pClientDE = DNULL;
	m_pClientShell = DNULL;
	m_pObjectives = DNULL;
	m_pTopObjective = DNULL;
	m_hDisplay = DNULL;
	m_hSeparator = DNULL;
	m_cxSeparator = 0;
	m_cySeparator = 0;
	m_bScrollable = DFALSE;

	m_bOpenAnimating = DFALSE;
	m_bCloseAnimating = DFALSE;
	m_rcTop.left = m_rcTop.top = m_rcTop.right = m_rcTop.bottom = 0;
	m_rcBottom.left = m_rcBottom.top = m_rcBottom.right = m_rcBottom.bottom = 0;

	m_bScrollingUp = DFALSE;
	m_bScrollingDown = DFALSE;
	m_fScrollOffset = 0.0f;
	m_fScrollOffsetTarget = 0.0f;
}

CMissionObjectives::~CMissionObjectives()
{
	OBJECTIVE* pObjective = m_pObjectives;
	while (pObjective)
	{
		if (pObjective->hSurface && m_pClientDE) m_pClientDE->DeleteSurface (pObjective->hSurface);

		OBJECTIVE* pNext = pObjective->pNext;
		delete pObjective;
		pObjective = pNext;
	}

	if (m_hDisplay) m_pClientDE->DeleteSurface (m_hDisplay);
	if (m_hSeparator) m_pClientDE->DeleteSurface (m_hSeparator);
}

void CMissionObjectives::Init (CClientDE* pClientDE, CRiotClientShell* pClientShell)
{
	m_pClientDE = pClientDE;
	m_pClientShell = pClientShell;

	// create display surface

	m_hDisplay = m_pClientDE->CreateSurfaceFromBitmap ("interface/MissionLog.pcx");
	if (!m_hDisplay) return;

	HDECOLOR hTransColor = m_pClientDE->SetupColor2(0.0f, 0.0f, 0.0f, DTRUE);

	// load separator

	m_hSeparator = m_pClientDE->CreateSurfaceFromBitmap ("interface/MissionLogSeparator.pcx");
	if (m_hSeparator)
	{
		m_pClientDE->GetSurfaceDims (m_hSeparator, &m_cxSeparator, &m_cySeparator);
//		m_pClientDE->OptimizeSurface (m_hSeparator, hTransColor);
	}

	// draw mission log title on surface

	HSTRING hstrFont = m_pClientDE->FormatString (IDS_INGAMEFONT);
	FONT fontdef (m_pClientDE->GetStringData(hstrFont), 
				  TextHelperGetIntValFromStringID(m_pClientDE, IDS_MISSIONOBJECTIVETITLEWIDTH, 10),
				  TextHelperGetIntValFromStringID(m_pClientDE, IDS_MISSIONOBJECTIVETITLEHEIGHT, 26),
				  DFALSE, DFALSE, DTRUE);
	m_pClientDE->FreeString (hstrFont);

	HDECOLOR foreColor = m_pClientDE->SetupColor1 (0.98f, 0.317647f, 0.0f, DFALSE);
	HSURFACE hStringSurface = CTextHelper::CreateSurfaceFromString (m_pClientDE, &fontdef, IDS_MISSIONLOG, foreColor);
	if (hStringSurface)
	{
		hStringSurface = CropSurface (hStringSurface, DNULL);

		DDWORD nWidth = 0;
		DDWORD nHeight = 0;
		m_pClientDE->GetSurfaceDims (hStringSurface, &nWidth, &nHeight);

		int x = 35 + ((145 - (int)nWidth) / 2);
		int y = 11 + ((19 - (int)nHeight) / 2);
		m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hDisplay, hStringSurface, DNULL, x, y, DNULL);

		m_pClientDE->DeleteSurface (hStringSurface);
		hStringSurface = DNULL;
	}

	// draw "primary objectives:" on surface

	fontdef.nWidth = 6;
	fontdef.nHeight = 13;
	fontdef.bBold = DFALSE;
	hStringSurface = CTextHelper::CreateSurfaceFromString (m_pClientDE, &fontdef, IDS_PRIMARY_OBJECTIVES, foreColor);
	if (hStringSurface)
	{
		hStringSurface = CropSurface (hStringSurface, DNULL);

		DDWORD nWidth = 0;
		DDWORD nHeight = 0;
		m_pClientDE->GetSurfaceDims (hStringSurface, &nWidth, &nHeight);

		int x = 13;
		int y = 59 + ((10 - (int)nHeight) / 2);
		m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hDisplay, hStringSurface, DNULL, x, y, DNULL);

		m_pClientDE->DeleteSurface (hStringSurface);
		hStringSurface = DNULL;
	}

//	m_pClientDE->OptimizeSurface (m_hDisplay, hTransColor);
}

void CMissionObjectives::AddObjective (DDWORD nID, DBOOL bCompleted)
{
	if (!m_pClientDE) return;

	// create the surface from the string

	HSTRING hstrFont = m_pClientDE->FormatString (IDS_INGAMEFONT);
	FONT fontdef (m_pClientDE->GetStringData (hstrFont), 
				  TextHelperGetIntValFromStringID(m_pClientDE, IDS_MISSIONOBJECTIVETEXTWIDTH, 6),
				  TextHelperGetIntValFromStringID(m_pClientDE, IDS_MISSIONOBJECTIVETEXTHEIGHT, 13));

	m_pClientDE->FreeString (hstrFont);

	HDECOLOR foreColor = m_pClientDE->SetupColor1 (1.0f, 1.0f, 1.0f, DFALSE);
	HSURFACE hSurf = CTextHelper::CreateWrappedStringSurface (m_pClientDE, TEXT_AREA_WIDTH, &fontdef, nID, foreColor);
	
	if (!hSurf) return;

	hSurf = CropSurface (hSurf, DNULL);

//	HDECOLOR hTransColor = m_pClientDE->SetupColor1(0.0f, 0.0f, 0.0f, DTRUE);

//	m_pClientDE->OptimizeSurface (hSurf, hTransColor);
	
	// create a new objective

	OBJECTIVE* pNew = new OBJECTIVE;
	if (!pNew) return;

	pNew->hSurface	 = hSurf;
	pNew->nID		 = nID;
	pNew->bCompleted = bCompleted;

	// add it to the list

	if (!m_pObjectives)
	{
		m_pObjectives = pNew;
	}
	else
	{
		pNew->pNext = m_pObjectives;
		m_pObjectives->pPrev = pNew;
		m_pObjectives = pNew;
	}

	m_pTopObjective = m_pObjectives;
}

void CMissionObjectives::RemoveObjective (DDWORD nID)
{
	if (!m_pClientDE) return;

	// find the objective

	OBJECTIVE* pObjective = m_pObjectives;
	while (pObjective)
	{
		if (pObjective->nID == nID) break;
		pObjective = pObjective->pNext;
	}
	if (!pObjective) return;

	// now remove it from the list

	if (pObjective->hSurface) m_pClientDE->DeleteSurface (pObjective->hSurface);
	if (pObjective->pPrev) pObjective->pPrev->pNext = pObjective->pNext;
	if (pObjective->pNext) pObjective->pNext->pPrev = pObjective->pPrev;
	if (m_pObjectives == pObjective) m_pObjectives = pObjective->pNext;

	delete pObjective;
	
	m_pTopObjective = m_pObjectives;
}

void CMissionObjectives::CompleteObjective (DDWORD nID)
{
	if (!m_pClientDE) return;

	// find the objective

	OBJECTIVE* pObjective = m_pObjectives;
	while (pObjective)
	{
		if (pObjective->nID == nID) break;
		pObjective = pObjective->pNext;
	}
	if (!pObjective) return;

	// mark it as completed

	pObjective->bCompleted = DTRUE;
}

void CMissionObjectives::ScrollUp()
{
	if (!m_pClientDE || m_bScrollingUp) return;

	if (m_pTopObjective && m_pTopObjective->pPrev) 
	{
		m_bScrollingUp = DTRUE;

		DDWORD nWidth = 0;
		DDWORD nHeight = 0;
		m_pClientDE->GetSurfaceDims (m_pTopObjective->pPrev->hSurface, &nWidth, &nHeight);
		
		m_fScrollOffset = (float) (-((int)nHeight) - (int)m_cySeparator);
		m_fScrollOffsetTarget = 0.0f;
		
		m_pTopObjective = m_pTopObjective->pPrev;
	}

}

void CMissionObjectives::ScrollDown()
{
	if (!m_pClientDE || m_bScrollingDown) return;

	if (m_pTopObjective && m_pTopObjective->pNext)
	{
		m_bScrollingDown = DTRUE;
		
		DDWORD nWidth = 0;
		DDWORD nHeight = 0;
		m_pClientDE->GetSurfaceDims (m_pTopObjective->hSurface, &nWidth, &nHeight);
		
		m_fScrollOffset = 0;
		m_fScrollOffsetTarget = (float) (-((int)nHeight) - (int)m_cySeparator);
	}
}

void CMissionObjectives::Reset()
{
	OBJECTIVE* pObjective = m_pObjectives;
	while (pObjective)
	{
		if (pObjective->hSurface && m_pClientDE) m_pClientDE->DeleteSurface (pObjective->hSurface);

		OBJECTIVE* pNext = pObjective->pNext;
		delete pObjective;
		pObjective = pNext;
	}

	m_pObjectives = DNULL;
	m_pTopObjective = DNULL;
	m_bScrollable = DFALSE;
}

void CMissionObjectives::ResetTop()
{
	m_pTopObjective = m_pObjectives;
}

void CMissionObjectives::SetLevelName (char* strLevelName)
{
}

void CMissionObjectives::StartOpenAnimation()
{
	m_bOpenAnimating = DTRUE;
	
	m_rcTop.left = 0;
	m_rcTop.top = 0;
	m_rcTop.right = 217;
	m_rcTop.bottom = 53;

	m_rcBottom.left = 0;
	m_rcBottom.top = 185;
	m_rcBottom.right = 217;
	m_rcBottom.bottom = 200;
}

void CMissionObjectives::StartCloseAnimation()
{
	m_bCloseAnimating = DTRUE;

	m_rcTop.left = 0;
	m_rcTop.top = 0;
	m_rcTop.right = 217;
	m_rcTop.bottom = 119;

	m_rcBottom.left = 0;
	m_rcBottom.top = 119;
	m_rcBottom.right = 217;
	m_rcBottom.bottom = 200;
}

void CMissionObjectives::Draw()
{
	if (!m_hDisplay || !m_pClientDE || !m_pClientShell) return;

	HSURFACE hScreen = m_pClientDE->GetScreenSurface();
	
	DDWORD nScreenWidth, nScreenHeight;
	m_pClientDE->GetSurfaceDims (hScreen, &nScreenWidth, &nScreenHeight);
	
	DDWORD nMOWidth, nMOHeight;
	m_pClientDE->GetSurfaceDims (m_hDisplay, &nMOWidth, &nMOHeight);

	int nOriginX = (int) ((float)X_LOCATION * (((float)nScreenWidth - (float)nMOWidth) / (640.0f - (float)nMOWidth)));
	int nOriginY = (int) ((float)Y_LOCATION * (((float)nScreenHeight - (float)nMOHeight) / (480.0f - (float)nMOHeight)));;

	// if we're doing the opening animation, draw it and increment the rectangles, then return

	if (m_bOpenAnimating)
	{
		DFLOAT nFrameTime = m_pClientDE->GetFrameTime();

		int y = nOriginY + 119 - (m_rcTop.bottom - m_rcTop.top);
		m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hDisplay, &m_rcTop, nOriginX, y, DNULL);

		y = nOriginY + 119;
		m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hDisplay, &m_rcBottom, nOriginX, y, DNULL);

		m_rcTop.bottom += (int)(nFrameTime * (float)OPEN_ANIM_RATE);
		m_rcBottom.top -= (int)(nFrameTime * (float)OPEN_ANIM_RATE);

		if (m_rcBottom.top <= m_rcTop.bottom) m_bOpenAnimating = DFALSE;
		return;
	}

	// if we're doing the closing animation, draw it and increment the rectangles, then return

	if (m_bCloseAnimating)
	{
		m_pClientShell->AddToClearScreenCount();

		DFLOAT nFrameTime = m_pClientDE->GetFrameTime();

		int y = nOriginY + 119 - (m_rcTop.bottom - m_rcTop.top);
		m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hDisplay, &m_rcTop, nOriginX, y, DNULL);

		y = nOriginY + 119;
		m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hDisplay, &m_rcBottom, nOriginX, y, DNULL);

		m_rcTop.bottom -= (int)(nFrameTime * (float)OPEN_ANIM_RATE);
		m_rcBottom.top += (int)(nFrameTime * (float)OPEN_ANIM_RATE);

		if (m_rcBottom.top >= 185) m_bCloseAnimating = DFALSE;
		return;
	}

	// set the initial coordinates

	int x = 15 + nOriginX;
	int y = 76 + nOriginY;
	
	// adjust the coordinates if we are scrolling up or down

	if (m_bScrollingUp)
	{
		DFLOAT nFrameTime = m_pClientDE->GetFrameTime();

		m_fScrollOffset += nFrameTime * SCROLL_SPEED;
		if (m_fScrollOffset > m_fScrollOffsetTarget)
		{
			m_fScrollOffset = 0.0f;
			m_fScrollOffsetTarget = 0.0f;
			m_bScrollingUp = DFALSE;
		}

		y += (int)m_fScrollOffset;
	}
	else if (m_bScrollingDown)
	{
		DFLOAT nFrameTime = m_pClientDE->GetFrameTime();

		m_fScrollOffset -= nFrameTime * SCROLL_SPEED;
		if (m_fScrollOffset < m_fScrollOffsetTarget)
		{
			m_fScrollOffset = 0.0f;
			m_fScrollOffsetTarget = 0.0f;
			m_bScrollingDown = DFALSE;

			m_pTopObjective = m_pTopObjective->pNext;
		}

		y += (int)m_fScrollOffset;
	}

	// first draw the display to the screen

	m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hDisplay, DNULL, nOriginX, nOriginY, DNULL);

	// now draw the objectives to the screen

	OBJECTIVE* pObjective = m_pTopObjective;
	while (pObjective && y < 188 + nOriginY)
	{
		DDWORD nWidth, nHeight;
		m_pClientDE->GetSurfaceDims (pObjective->hSurface, &nWidth, &nHeight);

		if (y < 74 + nOriginY)
		{
			DRect rcSrc;
			rcSrc.left = 0;
			rcSrc.top = (74 + nOriginY) - y;
			rcSrc.right = nWidth;
			rcSrc.bottom = nHeight;

			if (rcSrc.bottom >= rcSrc.top)
			{
				DrawObjective (hScreen, pObjective, &rcSrc, x, 74 + nOriginY);
			}
		}
		else if (y + (int)nHeight > 188 + nOriginY)
		{
			DRect rcSrc;
			rcSrc.left = 0;
			rcSrc.top = 0;
			rcSrc.right = nWidth;
			rcSrc.bottom = nHeight - ((y + nHeight) - (188 + nOriginY));

			DrawObjective (hScreen, pObjective, &rcSrc, x, y);
		}
		else
		{
			DrawObjective (hScreen, pObjective, DNULL, x, y);
		}

		y += nHeight;

		if (pObjective->pNext && y < 188 + nOriginY)
		{
			if (y < 74 + nOriginY)
			{
				DRect rcSrc;
				rcSrc.left = 0;
				rcSrc.top = (74 + nOriginY) - y;
				rcSrc.right = m_cxSeparator;
				rcSrc.bottom = m_cySeparator;

				if (rcSrc.bottom >= rcSrc.top)
				{
					m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hSeparator, &rcSrc, 51, 74 + nOriginY, DNULL);
				}
			}
			else if (y + (int)m_cySeparator > 188 + nOriginY)
			{
				DRect rcSrc;
				rcSrc.left = 0;
				rcSrc.top = 0;
				rcSrc.right = m_cxSeparator;
				rcSrc.bottom = m_cySeparator - ((y + m_cySeparator) - (188 + nOriginY));

				m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hSeparator, &rcSrc, 51, y, DNULL);
			}
			else
			{
				m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, m_hSeparator, DNULL, 51, y, DNULL);
			}

			y += m_cySeparator;
		}

		pObjective = pObjective->pNext;
	}

	// see if the text should be scrollable

	if (y > 188 + nOriginY) 
	{
		m_bScrollable = DTRUE;
	}
	else
	{
		m_bScrollable = DFALSE;
	}
}

void CMissionObjectives::DrawObjective (HSURFACE hScreen, OBJECTIVE* pObjective, DRect* rcSrc, int x, int y)
{
	if (!m_pClientDE || !hScreen || !pObjective) return;

	if (!pObjective->bCompleted)
	{
		m_pClientDE->DrawSurfaceToSurfaceTransparent (hScreen, pObjective->hSurface, rcSrc, x, y, DNULL);
	}
	else
	{
		HDECOLOR hFillColor = m_pClientDE->SetupColor1 (0.4f, 0.4f, 0.4f, DFALSE);
		m_pClientDE->DrawSurfaceSolidColor (hScreen, pObjective->hSurface, rcSrc, x, y, DNULL, hFillColor);
	}
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CMissionObjectives::Save
//
//	PURPOSE:	Save the mission objectives info
//
// --------------------------------------------------------------------------- //

void CMissionObjectives::Save(HMESSAGEWRITE hWrite)
{
	if (!m_pClientDE) return;

	DBYTE nNumObjectives = 0;

	OBJECTIVE* pObjective = m_pObjectives;
	while (pObjective)
	{
		nNumObjectives++;
		pObjective = pObjective->pNext;
	}	
	
	m_pClientDE->WriteToMessageByte(hWrite, nNumObjectives);

	pObjective = m_pObjectives;
	while (pObjective)
	{
		m_pClientDE->WriteToMessageDWord(hWrite, pObjective->nID);
		m_pClientDE->WriteToMessageByte(hWrite, pObjective->bCompleted);

		pObjective = pObjective->pNext;
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	CMissionObjectives::Load
//
//	PURPOSE:	Load the mission objectives info
//
// --------------------------------------------------------------------------- //

void CMissionObjectives::Load(HMESSAGEREAD hRead)
{
	if (!m_pClientDE) return;

	DBYTE nNumObjectives = m_pClientDE->ReadFromMessageByte(hRead);

	DDWORD dwId;
	DBOOL  bCompleted;

	for (int i=0; i < nNumObjectives; i++)
	{
		dwId		= m_pClientDE->ReadFromMessageDWord(hRead);
		bCompleted	= (DBOOL) m_pClientDE->ReadFromMessageByte(hRead);

		AddObjective(dwId, bCompleted);
	}
}
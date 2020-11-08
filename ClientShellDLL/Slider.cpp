#include "Slider.h"
#include "RiotClientShell.h"
#include "client_de.h"

CSlider::~CSlider()
{
	Term();
}

DBOOL CSlider::Init (CClientDE* pClientDE, int nWidth, int nStops)
{
	if (!pClientDE || nStops > nWidth) return DFALSE;

	m_pClientDE = pClientDE;
	m_bSelected = DFALSE;
	m_bEnabled = DFALSE;
	m_nStops = nStops;

	// load the pieces

	m_hBarNormal = m_pClientDE->CreateSurfaceFromBitmap ("interface/menu/BarSel.pcx");
	m_hBarDisabled = m_pClientDE->CreateSurfaceFromBitmap ("interface/menu/BarDis.pcx");
	m_hThumbNormal = m_pClientDE->CreateSurfaceFromBitmap ("interface/menu/Thumb_n.pcx");
	m_hThumbSelected = m_pClientDE->CreateSurfaceFromBitmap ("interface/menu/Thumb_s.pcx");
	m_hThumbDisabled = m_pClientDE->CreateSurfaceFromBitmap ("interface/menu/Thumb_d.pcx");
	m_hThumbFillNormal = m_pClientDE->CreateSurfaceFromBitmap ("interface/menu/ThumbFill_n.pcx");
	m_hThumbFillSelected = m_pClientDE->CreateSurfaceFromBitmap ("interface/menu/ThumbFill_s.pcx");
	m_hThumbFillDisabled = m_pClientDE->CreateSurfaceFromBitmap ("interface/menu/ThumbFill_d.pcx");
	
	DDWORD width, height;
	m_pClientDE->GetSurfaceDims (m_hBarNormal, &width, &height);
	m_hSlider = m_pClientDE->CreateSurface (width, height);
	
	if (!m_hBarNormal || !m_hBarDisabled || !m_hThumbNormal || !m_hThumbSelected || !m_hThumbDisabled ||
		!m_hThumbFillNormal || !m_hThumbFillSelected || !m_hThumbFillDisabled || !m_hSlider)
	{
		Term();
		return DFALSE;
	}

	m_nWidth = (int) width;
	m_nHeight = (int) height;

	m_pClientDE->GetSurfaceDims (m_hThumbNormal, &width, &height);
	m_szThumb.cx = (int) width;
	m_szThumb.cy = (int) height;

	UpdateSlider();

	return DTRUE;
}

void CSlider::Term()
{
	if (!m_pClientDE) return;

	if (m_hBarNormal) m_pClientDE->DeleteSurface (m_hBarNormal);
	if (m_hBarDisabled) m_pClientDE->DeleteSurface (m_hBarDisabled);
	if (m_hThumbNormal) m_pClientDE->DeleteSurface (m_hThumbNormal);
	if (m_hThumbSelected) m_pClientDE->DeleteSurface (m_hThumbSelected);
	if (m_hThumbDisabled) m_pClientDE->DeleteSurface (m_hThumbDisabled);
	if (m_hThumbFillNormal) m_pClientDE->DeleteSurface (m_hThumbFillNormal);
	if (m_hThumbFillSelected) m_pClientDE->DeleteSurface (m_hThumbFillSelected);
	if (m_hThumbFillDisabled) m_pClientDE->DeleteSurface (m_hThumbFillDisabled);
	if (m_hSlider) m_pClientDE->DeleteSurface (m_hSlider);


	m_pClientDE = DNULL;
	m_bSelected = DFALSE;
	m_bEnabled = DFALSE;
	m_nWidth = 0; 
	m_nHeight = 0; 
	m_nStops = 0; 
	m_nPos = 0; 
	m_hBarNormal = DNULL;
	m_hBarDisabled = DNULL;
	m_hThumbNormal = DNULL;
	m_hThumbSelected = DNULL;
	m_hThumbDisabled = DNULL;
	m_hThumbFillNormal = DNULL;
	m_hThumbFillSelected = DNULL;
	m_hThumbFillDisabled = DNULL;
	m_hSlider = DNULL;
	m_szThumb.cx = m_szThumb.cy = 0;
}

void CSlider::Draw (HSURFACE hDest, int x, int y)
{
	if (!m_pClientDE) return;

	HDECOLOR hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DFALSE);

	m_pClientDE->DrawSurfaceToSurfaceTransparent (hDest, m_hSlider, NULL, x, y, hTransColor);
}

void CSlider::GetThumbPos (int* pX, int* pY)
{
	int nPossiblePositions = m_nWidth - 4 - m_szThumb.cx;
	float nInc = (float)nPossiblePositions / (float)(m_nStops - 1);

	*pY = 0;
	*pX = 2 + (int) ((float)m_nPos * nInc);
}

void CSlider::UpdateSlider()
{
	if (!m_pClientDE) return;

	HDECOLOR hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DFALSE);

	// draw correct bar

	if (m_bEnabled)
	{
		m_pClientDE->DrawSurfaceToSurface (m_hSlider, m_hBarNormal, NULL, 0, 0);
	}
	else
	{
		m_pClientDE->DrawSurfaceToSurface (m_hSlider, m_hBarDisabled, NULL, 0, 0);
	}
	
	// calculate position of thumb

	int xThumb = 0;
	int yThumb = 0;
	GetThumbPos (&xThumb, &yThumb);
	
	// calculate rects for filling behind thumb

	DRect rcDst;
	rcDst.left = 2;
	rcDst.top = 5;
	rcDst.right = rcDst.left + (xThumb - 2);
	rcDst.bottom = 9;

	DBOOL bDrawLine = !!(rcDst.right - rcDst.left);

	// draw correct thumb and fill in line behind thumb


	if (m_bEnabled && m_bSelected)
	{
		m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hSlider, m_hThumbSelected, NULL, xThumb, yThumb, hTransColor);
		if (bDrawLine) m_pClientDE->ScaleSurfaceToSurface (m_hSlider, m_hThumbFillSelected, &rcDst, DNULL);
	}
	else if (m_bEnabled)
	{
		m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hSlider, m_hThumbNormal, NULL, xThumb, yThumb, hTransColor);
		if (bDrawLine) m_pClientDE->ScaleSurfaceToSurface (m_hSlider, m_hThumbFillNormal, &rcDst, DNULL);
	}
	else
	{
		m_pClientDE->DrawSurfaceToSurfaceTransparent (m_hSlider, m_hThumbDisabled, NULL, xThumb, yThumb, hTransColor);
		if (bDrawLine) m_pClientDE->ScaleSurfaceToSurface (m_hSlider, m_hThumbFillDisabled, &rcDst, DNULL);
	}

	//hTransColor = m_pClientDE->SetupColor1 (0.0f, 1.0f, 0.0f, DTRUE);
	//m_pClientDE->OptimizeSurface (m_hSlider, hTransColor);
}
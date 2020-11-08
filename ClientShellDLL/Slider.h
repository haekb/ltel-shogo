#ifndef __SLIDER_H
#define __SLIDER_H

#include "client_de.h"
#include "clientUtilities.h"

class CClientDE;

class CSlider
{
public:
	
	CSlider()						{ m_pClientDE = DNULL; m_bSelected = DFALSE; m_bEnabled = DFALSE; m_nWidth = 0; m_nHeight = 0; m_nStops = 0; 
									  m_nPos = 0; m_hBarNormal = DNULL; m_hBarDisabled = DNULL; m_hThumbNormal = DNULL; m_hThumbSelected = DNULL; 
									  m_hThumbDisabled = DNULL; m_hThumbFillNormal = DNULL; m_hThumbFillSelected = DNULL; m_hThumbFillDisabled = DNULL; 
									  m_hSlider = DNULL; }
	~CSlider();

	DBOOL	Init (CClientDE* pClientDE, int nWidth, int nStops);
	void	Term();
	void	Draw (HSURFACE hDest, int x, int y);

	void	SetSelected (DBOOL bSelected = DTRUE)	{ if (m_bSelected == bSelected) return; m_bSelected = bSelected; UpdateSlider(); }
	void	SetEnabled (DBOOL bEnabled = DTRUE)		{ if (m_bEnabled == bEnabled) return; m_bEnabled = bEnabled; UpdateSlider(); }
	int		GetPos()								{ return m_nPos; }
	void	SetPos (int nPos)						{ if (nPos >= 0 && nPos < m_nStops) m_nPos = nPos; UpdateSlider(); }
	int		GetWidth()								{ return m_nWidth; }
	int		GetHeight()								{ return m_nHeight; }
	int		GetMin()								{ return 0; }
	int		GetMax()								{ return m_nStops - 1; }
	DBOOL	IncPos()								{ if (m_nPos < m_nStops - 1) { SetPos (m_nPos + 1); return DTRUE; } return DFALSE; }
	DBOOL	DecPos()								{ if (m_nPos > 0) { SetPos (m_nPos - 1); return DTRUE; } return DFALSE; }

protected:

	void	GetThumbPos (int* pX, int* pY);
	void	UpdateSlider();

	CClientDE*	m_pClientDE;
	DBOOL		m_bSelected;
	DBOOL		m_bEnabled;
	int			m_nWidth;
	int			m_nHeight;
	int			m_nStops;
	int			m_nPos;
	HSURFACE	m_hBarNormal;
	HSURFACE	m_hBarDisabled;
	HSURFACE	m_hThumbNormal;
	HSURFACE	m_hThumbSelected;
	HSURFACE	m_hThumbDisabled;
	HSURFACE	m_hThumbFillNormal;
	HSURFACE	m_hThumbFillSelected;
	HSURFACE	m_hThumbFillDisabled;
	HSURFACE	m_hSlider;
	CSize		m_szThumb;
};

#endif

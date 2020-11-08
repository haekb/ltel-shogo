#ifndef __BITMAPFONT_H
#define __BITMAPFONT_H

class CClientDE;

class CBitmapFont
{
public:
	
	CBitmapFont();
	~CBitmapFont();

	DBOOL			Init (CClientDE* pClientDE, char* strBitmapName);
	DBOOL			IsValid()						{ return !!m_hFontSurface; }

	char*			GetImageName()					{ return m_strImageName; }
	HSURFACE		GetFontSurface()				{ return m_hFontSurface; }
	DDWORD			GetFontHeight()					{ return m_nFontHeight; }
	int				GetCharPos (int nChar)			{ if (nChar < 0 || nChar > 255) return 0; return m_nCharPos[nChar]; }
	int				GetCharWidth (int nChar)		{ if (nChar < 0 || nChar > 255) return 0; return m_nCharWidth[nChar]; }

	virtual char*	GetClassName() = 0;				// override to return font class name

protected:

	void			InitCharPositions();

	virtual void	InitCharWidths() = 0;

protected:
	
	CClientDE*	m_pClientDE;
	char		m_strImageName[256];
	HSURFACE	m_hFontSurface;
	DDWORD		m_nFontHeight;
	int			m_nCharPos[256];
	int			m_nCharWidth[256];
};

#endif

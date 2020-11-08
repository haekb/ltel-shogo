#include "cpp_client_de.h"
#include "BitmapFont.h"

CBitmapFont::CBitmapFont()
{
	m_pClientDE = DNULL;
	memset (m_strImageName, 0, 256);
	m_hFontSurface = DNULL;
	m_nFontHeight = 0;
	memset (m_nCharPos, 0, 256 * sizeof(int));
	memset (m_nCharWidth, 0, 256 * sizeof(int));
}

CBitmapFont::~CBitmapFont()
{
	if (m_pClientDE && m_hFontSurface) m_pClientDE->DeleteSurface (m_hFontSurface);
}

DBOOL CBitmapFont::Init (CClientDE* pClientDE, char *strBitmapName)
{
	if (!pClientDE || !strBitmapName) return DFALSE;

	m_pClientDE = pClientDE;

	strncpy (m_strImageName, strBitmapName, 255);

	m_hFontSurface = m_pClientDE->CreateSurfaceFromBitmap (strBitmapName);
	if (!m_hFontSurface) return DFALSE;

	DDWORD nWidth = 0;
	m_pClientDE->GetSurfaceDims (m_hFontSurface, &nWidth, &m_nFontHeight);

	InitCharWidths();
	InitCharPositions();

	return DTRUE;
}

void CBitmapFont::InitCharPositions()
{
	int nPos = 0;
	for (int i = 0; i < 256; i++)
	{
		m_nCharPos[i] = nPos;
		nPos += m_nCharWidth[i];
	}
}
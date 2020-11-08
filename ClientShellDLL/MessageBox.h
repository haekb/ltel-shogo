#ifndef __MESSAGEBOX_H
#define __MESSAGEBOX_H

#include "basedefs_de.h"
#include "ClientUtilities.h"
#include "TextHelper.h"

class CClientDE;

class CMessageBox
{
public:

	CMessageBox()		{ m_pClientDE = DNULL; m_hMessageBox = DNULL; m_szMessageBox.cx = m_szMessageBox.cy = 0; m_bYesNo = DFALSE; }
	~CMessageBox();

	DBOOL			Init (CClientDE* pClientDE, int nStringID, DBOOL bYesNo = DFALSE, int nAlignment = TH_ALIGN_LEFT, DBOOL bCrop = DTRUE);

	DBOOL			IsYesNo()		{ return m_bYesNo; }

	void			Draw();

protected:

	CClientDE*		m_pClientDE;
	HSURFACE		m_hMessageBox;
	CSize			m_szMessageBox;
	DBOOL			m_bYesNo;
};

#endif

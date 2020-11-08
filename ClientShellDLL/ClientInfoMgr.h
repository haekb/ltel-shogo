#ifndef __CLIENTINFOMGR_H
#define __CLIENTINFOMGR_H

#include "basedefs_de.h"
#include "ClientUtilities.h"

class CClientDE;

struct CLIENT_INFO
{
	CLIENT_INFO()	{ r = g = b = 0; m_Ping = 0.0f; nID = 0; hstrName = DNULL; nFrags = 0; hName = DNULL; szName.cx = szName.cy = 0; hFragCount = DNULL; szFragCount.cx = szFragCount.cy = 0; pPrev = DNULL; pNext = DNULL;}

	float			m_Ping;
	DDWORD			nID;
	HSTRING			hstrName;
	int				nFrags;

	HSURFACE		hName;
	CSize			szName;
	
	HSURFACE		hFragCount;
	CSize			szFragCount;

	// Player color.
	DBYTE			r, g, b, padding;

	CLIENT_INFO*	pPrev;
	CLIENT_INFO*	pNext;
};

class CClientInfoMgr
{
public:

	CClientInfoMgr();
	~CClientInfoMgr();

	void	Init (CClientDE* pClientDE);

	void	AddClient (HSTRING hstrName, DDWORD nID, int nFragCount, DBYTE r, DBYTE g, DBYTE b);
	void	RemoveClient (DDWORD nID);
	void	RemoveAllClients();
	
	void	AddFrag (DDWORD nLocalID, DDWORD nID);
	void	RemoveFrag (DDWORD nLocalID, DDWORD nID);

	CLIENT_INFO* GetClientByID(DDWORD nID);
	
	DDWORD	GetNumClients();
	char*	GetPlayerName (DDWORD nID);

	void	UpdateFragDisplay (int nFrags);

	void	UpdateAllFragSurfaces();
	void	ClearAllFragSurfaces();
	void	UpdateSingleFragSurface (CLIENT_INFO* pClient);
	void	ClearUpToDate() {m_bFragSurfacesUpToDate = DFALSE;}

	void	Draw (DBOOL bDrawSingleFragCount, DBOOL bDrawAllFragCounts);

protected:

	CClientDE*			m_pClientDE;
	CLIENT_INFO*		m_pClients;

	DDWORD				m_nLastFontSize;
	DBOOL				m_bFragSurfacesUpToDate;

	HSURFACE			m_hFragDisplay;
	DDWORD				m_cxFragDisplay;
	DDWORD				m_cyFragDisplay;
};

#endif
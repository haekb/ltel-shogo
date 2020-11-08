// ----------------------------------------------------------------------- //
//
// MODULE  : MissionObjectives.h
//
// PURPOSE : Riot's Mission Objective system - Definition
//
// CREATED : 3/22/98
//
// ----------------------------------------------------------------------- //

#ifndef __MISSIONOBJECTIVES_H
#define __MISSIONOBJECTIVES_H

class CClientDE;
class CRiotClientShell;

struct OBJECTIVE
{
	OBJECTIVE()		{ nID = 0; hSurface = DNULL; bCompleted = DFALSE; pPrev = DNULL; pNext = DNULL; }

	DDWORD		nID;
	DBOOL		bCompleted;
	HSURFACE	hSurface;

	OBJECTIVE*	pPrev;
	OBJECTIVE*	pNext;
};

class CMissionObjectives
{
public:

	CMissionObjectives();
	~CMissionObjectives();

	void	Init (CClientDE* pClientDE, CRiotClientShell* pClientShell);

	void	AddObjective (DDWORD nID, DBOOL bCompleted=DFALSE);
	void	RemoveObjective (DDWORD nID);
	void	CompleteObjective (DDWORD nID);


	void	ScrollUp();
	void	ScrollDown();

	void	Reset();
	void	ResetTop();
	void	SetLevelName (char* strLevelName);
	void	StartOpenAnimation();
	void	StartCloseAnimation();
	void	Draw();

	DBOOL	IsClosing()		{ return m_bCloseAnimating; }

	void		Save(HMESSAGEWRITE hWrite);
	void		Load(HMESSAGEREAD hRead);

protected:

	void	DrawObjective (HSURFACE hScreen, OBJECTIVE* pObjective, DRect* rcSrc, int x, int y);

protected:
	
	CClientDE*	m_pClientDE;
	CRiotClientShell* m_pClientShell;
	OBJECTIVE*	m_pObjectives;
	OBJECTIVE*	m_pTopObjective;
	HSURFACE	m_hDisplay;
	HSURFACE	m_hSeparator;
	DDWORD		m_cxSeparator;
	DDWORD		m_cySeparator;
	DBOOL		m_bScrollable;
	
	DBOOL		m_bOpenAnimating;
	DBOOL		m_bCloseAnimating;
	DRect		m_rcTop;
	DRect		m_rcBottom;

	DBOOL		m_bScrollingUp;
	DBOOL		m_bScrollingDown;
	DFLOAT		m_fScrollOffset;
	DFLOAT		m_fScrollOffsetTarget;
};

#endif
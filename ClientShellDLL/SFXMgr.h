// ----------------------------------------------------------------------- //
//
// MODULE  : SFXMgr.h
//
// PURPOSE : Special FX Mgr	- Definition
//
// CREATED : 10/24/97
//
// ----------------------------------------------------------------------- //

#ifndef __SFX_MGR_H__
#define __SFX_MGR_H__

#include "cpp_client_de.h"
#include "SpecialFXList.h"
#include "SFXMsgIds.h"


#define DYN_ARRAY_SIZE		(SFX_TOTAL_NUMBER + 1)
#define	CAMERA_LIST_SIZE	20

class CSFXMgr
{
	public :

		CSFXMgr()  { m_pClientDE = DNULL; }
		~CSFXMgr() {}

		DBOOL	Init(CClientDE* pClientDE);

		CSpecialFX* FindSpecialFX(DBYTE nType, HLOCALOBJ hObj);
		CSpecialFX* FindProjectileFX(HLOCALOBJ hObj);

		CSpecialFXList* GetCameraList() { return &m_cameraSFXList; }

		void	RemoveSpecialFX(HLOCALOBJ hObj);
		void	UpdateSpecialFX();
		void	HandleSFXMsg(HLOCALOBJ hObj, HMESSAGEREAD hMessage);

		void	RemoveAll();

		CSpecialFX*	CreateSFX(DBYTE nId, SFXCREATESTRUCT *psfxCreateStruct);
		CSpecialFX* CreateAutoSFX(HOBJECT hServerObj, HMESSAGEREAD hMessage);

		CSpecialFXList* GetBulletHoleList() { return &m_dynSFXLists[1]; }

		void OnTouchNotify(HOBJECT hMain, CollisionInfo *pInfo, float forceMag);

	private :

		DBOOL	AddDynamicSpecialFX(CSpecialFX* pSFX, DBYTE nId);
		void 	UpdateDynamicSpecialFX();
		void	RemoveDynamicSpecialFX(HOBJECT hObj);
		void	RemoveAllDynamicSpecialFX();

		int				GetDynArrayIndex(DBYTE nFXId);
		unsigned int	GetDynArrayMaxNum(DBYTE nArrayIndex);

		CClientDE*		m_pClientDE;

		CSpecialFXList  m_dynSFXLists[DYN_ARRAY_SIZE];  // Lists of dynamic special fx
		CSpecialFXList	m_cameraSFXList;				// List of camera special fx
};

#endif // __SFX_MGR_H__
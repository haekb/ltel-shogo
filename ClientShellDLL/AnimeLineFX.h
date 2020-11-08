
// ----------------------------------------------------------------------- //
//
// MODULE  : AnimeLineFX.h
//
// PURPOSE : Explosion special fx class - Definition
//
// CREATED : 5/27/98
//
// ----------------------------------------------------------------------- //

#ifndef __ANIMELINE_FX_H__
#define __ANIMELINE_FX_H__


	#include "BaseScaleFX.h"


	#define MAX_ANIME_LINES 32


	class LineInfo
	{
	public:
		HOBJECT		m_hObject;
		
		float		m_TimeToLive;
		float		m_Lifetime; // How long it's been alive.
		
		float		m_Angle, m_AngleSpeed;
		float		m_Scale;
	};


	class ALCREATESTRUCT : public SFXCREATESTRUCT
	{
	public:

		DVector		m_Pos;
		DVector		m_DirVec;
	};


	class CAnimeLineFX : public CSpecialFX
	{
		public :

			CAnimeLineFX();
			virtual ~CAnimeLineFX();

			virtual DBOOL Init(SFXCREATESTRUCT* psfxCreateStruct);
			virtual DBOOL CreateObject(CClientDE* pClientDE);
			virtual DBOOL Update();

		private :

			DVector		m_BasePos;
			DVector		m_BaseDir;

			LineInfo	m_Lines[MAX_ANIME_LINES];
			DDWORD		m_nLines;
	};

#endif // __ANIMELINE_FX_H__
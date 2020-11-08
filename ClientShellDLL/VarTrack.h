
// Console variable tracker.. makes it easy to get and set the value of
// console variables.

#ifndef __VarTrack_H__
#define __VarTrack_H__


	#include <stdio.h>
	#include "client_de.h"


	class VarTrack
	{
	public:

					VarTrack()
					{
						m_hVar = DNULL;
						m_pClientDE = DNULL;
						m_pVarName = NULL;
					}

		DBOOL		Init(ClientDE *pClientDE, char *pVarName, char *pStartVal, float fStartVal)
		{
			char tempStr[128], tempStr2[256];

			m_pVarName = pVarName;
			if(!pStartVal)
			{
				sprintf(tempStr, "%5f", fStartVal);
				pStartVal = tempStr;
			}

			m_hVar = pClientDE->GetConsoleVar(pVarName);
			if(!m_hVar)
			{
				sprintf(tempStr2, "\"%s\" \"%s\"", pVarName, pStartVal);
				pClientDE->RunConsoleString(tempStr2);

				m_hVar = pClientDE->GetConsoleVar(pVarName);
				if(!m_hVar)
				{
					return DFALSE;
				}
			}
					
			m_pClientDE = pClientDE;
			return DTRUE;
		}

		DBOOL		IsInitted()
		{
			return !!m_pClientDE;
		}

		float		GetFloat(float defVal=0.0f)
		{
			if(m_pClientDE && m_hVar)
				return m_pClientDE->GetVarValueFloat(m_hVar);
			else
				return defVal;
		}

		char*		GetStr(char *pDefault="")
		{
			char *pRet;

			if(m_pClientDE && m_hVar)
			{
				if(pRet = m_pClientDE->GetVarValueString(m_hVar))
					return pRet;
			}
			return pDefault;
		}

		void		SetFloat(float val)
		{
			char str[256];

			if(!m_pClientDE || !m_pVarName)
				return;

			sprintf(str, "%s %f", m_pVarName, val);
			m_pClientDE->RunConsoleString(str);
		}

	protected:
		
		HCONSOLEVAR	m_hVar;
		ClientDE	*m_pClientDE;
		char		*m_pVarName;
	};


#endif  // __VarTrack_H__






#include "client_de.h"
#include "AnimeLineFX.h"
#include "VarTrack.h"
#include "ClientUtilities.h"
#include "RiotClientShell.h"


#define SCALE_UP_TIME	0.3f
#define ROTATE_TIME		0.1f
#define FADE_OUT_TIME	0.4f
#define TOTAL_TIME		(SCALE_UP_TIME+ROTATE_TIME+FADE_OUT_TIME)


static VarTrack g_AnimeNumLines;
static VarTrack g_AnimeScale;
static VarTrack g_AnimeAngleSpeed;
static VarTrack g_AnimeColor;
static VarTrack g_AnimeVariance;



static inline float Rand01()
{
	return (float)rand() / RAND_MAX;
}


CAnimeLineFX::CAnimeLineFX()
{
	m_nLines = 0;
}


CAnimeLineFX::~CAnimeLineFX()
{
	DDWORD i;

	if(m_pClientDE)
	{
		for(i=0; i < m_nLines; i++)
		{
			if(m_Lines[i].m_hObject)
			{
				m_pClientDE->DeleteObject(m_Lines[i].m_hObject);
				m_Lines[i].m_hObject = DNULL;
			}
		}
	}
}


DBOOL CAnimeLineFX::CreateObject(CClientDE* pClientDE) 
{
	ObjectCreateStruct theStruct;
	DDWORD nLines;
	float theColor[3];
	float theScale;
	float angleSpeed;
	float curAngle, angleAdd;
	DDWORD i;
	LineInfo *pLine;
	PlaySoundInfo playSoundInfo;


	if (!pClientDE) return DFALSE;
	m_pClientDE = pClientDE;

	static float theRadius = 1500.0f;
	PLAYSOUNDINFO_INIT(playSoundInfo);
	playSoundInfo.m_dwFlags = PLAYSOUND_3D | PLAYSOUND_REVERB;
	SAFE_STRCPY(playSoundInfo.m_szSoundName, "sounds\\enemies\\spot\\uca_bad.wav");
	playSoundInfo.m_vPosition = m_BasePos;
	playSoundInfo.m_nPriority	 = SOUNDPRIORITY_PLAYER_HIGH;
	playSoundInfo.m_fOuterRadius = theRadius;
	playSoundInfo.m_fInnerRadius = theRadius * 0.25f;
	playSoundInfo.m_nVolume		 = 100;
	m_pClientDE->PlaySound(&playSoundInfo);



	// Init variables..
	if(!g_AnimeNumLines.IsInitted())
		g_AnimeNumLines.Init(pClientDE, "AnimeNumLines", DNULL, 6.0f);

	if(!g_AnimeScale.IsInitted())
		g_AnimeScale.Init(pClientDE, "AnimeScale", DNULL, 45.0f);

	if(!g_AnimeAngleSpeed.IsInitted())
		g_AnimeAngleSpeed.Init(pClientDE, "AnimeAngleSpeed", DNULL, 15.0f);

	if(!g_AnimeColor.IsInitted())
		g_AnimeColor.Init(pClientDE, "AnimeColor", ".2 .2 .8", 0.0f);

	if(!g_AnimeVariance.IsInitted())
		g_AnimeVariance.Init(pClientDE, "AnimeVariance", DNULL, 1.2f);

	
	nLines = (DDWORD)g_AnimeNumLines.GetFloat();
	theScale = g_AnimeScale.GetFloat();
	angleSpeed = g_AnimeAngleSpeed.GetFloat();
	sscanf(g_AnimeColor.GetStr(), "%f %f %f", &theColor[0], &theColor[1], &theColor[2]);



	// Create the lines.
	INIT_OBJECTCREATESTRUCT(theStruct);
	SAFE_STRCPY(theStruct.m_Filename, "models\\multiplaystar.abc");
	SAFE_STRCPY(theStruct.m_SkinName, "skins\\multiplay.dtx");
	theStruct.m_Flags = FLAG_VISIBLE | FLAG_NOLIGHT;
	theStruct.m_ObjectType = OT_MODEL;
	theStruct.m_Pos = m_BasePos;

	// Make the line models.
	m_nLines = nLines;
	curAngle = Rand01() * MATH_CIRCLE;
	angleAdd = MATH_CIRCLE / m_nLines;
	for(i=0; i < m_nLines; i++)
	{
		pLine = &m_Lines[i];
		
		pLine->m_hObject = m_pClientDE->CreateObject(&theStruct);
		if(pLine->m_hObject)
		{
			m_pClientDE->SetObjectColor(pLine->m_hObject, theColor[0], theColor[1], theColor[2], 1.0f); 
			
			pLine->m_Angle = curAngle;
			curAngle += angleAdd;

			// Vary it..
			pLine->m_Angle += (Rand01() - 0.5f) * g_AnimeVariance.GetFloat();

			pLine->m_AngleSpeed = Rand01();
			pLine->m_AngleSpeed = DMAX(0.3f, pLine->m_AngleSpeed);
			pLine->m_AngleSpeed *= angleSpeed;
			
			pLine->m_TimeToLive = TOTAL_TIME;
			pLine->m_Lifetime = 0.0f;
			pLine->m_Scale = theScale - (Rand01()*theScale*0.4f);
		}
	}

	return DTRUE;
}


DBOOL CAnimeLineFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{ 
	if (!psfxCreateStruct) return DFALSE;

	ALCREATESTRUCT *pAL = (ALCREATESTRUCT*)psfxCreateStruct;
	m_BasePos = pAL->m_Pos;
	m_BaseDir = pAL->m_DirVec;

	return CSpecialFX::Init(psfxCreateStruct);
}


// Return of DFALSE indicates special fx is done and can be removed.
DBOOL CAnimeLineFX::Update()
{
	DDWORD i, nActive;
	DVector vScale;
	DRotation rot;
	float frameTime, r, g, b, a, fMul;
	LineInfo *pLine;


	nActive = 0;
	
	frameTime = m_pClientDE->GetFrameTime();
	for(i=0; i < m_nLines; i++)
	{
		pLine = &m_Lines[i];

		if(pLine->m_hObject)
		{
			pLine->m_Lifetime += frameTime;
			if(pLine->m_Lifetime > pLine->m_TimeToLive)
			{
				m_pClientDE->DeleteObject(pLine->m_hObject);
				pLine->m_hObject = DNULL;
				continue;
			}

			VEC_SET(vScale, pLine->m_Scale, pLine->m_Scale*0.5f, pLine->m_Scale*0.5f);
			if(pLine->m_Lifetime < SCALE_UP_TIME)
			{
				fMul = pLine->m_Lifetime / SCALE_UP_TIME;
				vScale *= fMul;
				m_pClientDE->SetObjectScale(pLine->m_hObject, &vScale);
			}
			else
			{
				m_pClientDE->SetObjectScale(pLine->m_hObject, &vScale);
			}

			// Update its rotation.
			if(pLine->m_Lifetime > SCALE_UP_TIME)
			{
				pLine->m_Angle += pLine->m_AngleSpeed * frameTime;
			}
			
			m_pClientDE->AlignRotation(&rot, &m_BaseDir, DNULL);
			m_pClientDE->RotateAroundAxis(&rot, &m_BaseDir, pLine->m_Angle);
			m_pClientDE->SetObjectRotation(pLine->m_hObject, &rot);
			
			// Set alpha.
			if(pLine->m_Lifetime > (SCALE_UP_TIME+ROTATE_TIME))
			{
				fMul = (pLine->m_Lifetime - (SCALE_UP_TIME+ROTATE_TIME)) / FADE_OUT_TIME;
				m_pClientDE->GetObjectColor(pLine->m_hObject, &r, &g, &b, &a);
				a = 1.0f - fMul;
				m_pClientDE->SetObjectColor(pLine->m_hObject, r, g, b, a);
			}

			++nActive;
		}
	}
	
	return nActive > 0;
}




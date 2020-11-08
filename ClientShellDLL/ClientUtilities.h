// ----------------------------------------------------------------------- //
//
// MODULE  : ClientUtilities.h
//
// PURPOSE : Utility functions
//
// CREATED : 9/25/97
//
// ----------------------------------------------------------------------- //

#ifndef __CLIENT_UTILITIES_H__
#define __CLIENT_UTILITIES_H__

#include "RiotSoundTypes.h"
#include "client_de.h"
#include "RiotCommonUtilities.h"

#define NUM_COMMANDS 28

struct CommandID
{
	int		nStringID;
	int		nCommandID;
};

struct CSize
{
	CSize()		{ cx = 0; cy = 0; }
	
	unsigned long	cx;
	unsigned long	cy;
};

int CommandToArrayPos(int nCommand);
char* CommandName(int nCommand);

HSOUNDDE PlaySoundFromObject( HOBJECT hObject, char *pSoundName, DFLOAT fRadius, DBYTE nSoundPriority, 
							 DBOOL bLoop = DFALSE, DBOOL bHandle = DFALSE, DBOOL bTime = DFALSE, DBYTE nVolume = 100 );
HSOUNDDE PlaySoundFromPos( DVector *vPos, char *pSoundName, DFLOAT fRadius, DBYTE nSoundPriority, 
						  DBOOL bLoop = DFALSE, DBOOL bHandle = DFALSE, DBOOL bTime = DFALSE, DBYTE nVolume = 100 );

HSOUNDDE PlaySoundLocal( char *pSoundName, DBYTE nSoundPriority, DBOOL bLoop = DFALSE, DBOOL bHandle = DFALSE, DBYTE nVolume = 100, DBOOL bReverb = DFALSE );

HSURFACE CropSurface ( HSURFACE hSurf, HDECOLOR hBorderColor );

#endif // __CLIENT_UTILITIES_H__
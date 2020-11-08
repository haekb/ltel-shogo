// ----------------------------------------------------------------------- //
//
// MODULE  : RiotCommonUtilities.h
//
// PURPOSE : Utility functions
//
// CREATED : 5/4/98
//
// ----------------------------------------------------------------------- //

#ifndef __RIOTCOMMONUTILITIES_H__
#define __RIOTCOMMONUTILITIES_H__

#include "basetypes_de.h"
#include "common_de.h"


D_WORD Color255VectorToWord( DVector *pVal );
void Color255WordToVector( D_WORD wVal, DVector *pVal );

int GetRandom();
int GetRandom(int range);
int GetRandom(int lo, int hi);
float GetRandom(float min, float max);


// Compress/decompress a rotation into a single byte.  This only accounts for 
// rotation around the Y axis.
DBYTE CompressRotationByte(CommonLT *pCommon, DRotation *pRotation);
void UncompressRotationByte(CommonLT *pCommon, DBYTE rot, DRotation *pRotation);



#endif // __RIOTCOMMONUTILITIES_H__



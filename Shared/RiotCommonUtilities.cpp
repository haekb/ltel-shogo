// ----------------------------------------------------------------------- //
//
// MODULE  : RiotObjectUtilities.cpp
//
// PURPOSE : Utility functions
//
// CREATED : 9/25/97
//
// ----------------------------------------------------------------------- //

#include "RiotCommonUtilities.h"
#include "basedefs_de.h"

// Temp buffer...

char s_FileBuffer[_MAX_PATH];

// Stolen from gamework.h...

int GetRandom()
{
	return(rand());
}

int GetRandom(int range)
{
	if (range == -1)	// check for divide-by-zero case
	{
		return((rand() % 2) - 1);
	}
		
	return(rand() % (range + 1));
}

int GetRandom(int lo, int hi)
{
	if ((hi - lo + 1) == 0)		// check for divide-by-zero case
	{
		if (rand() & 1) return(lo);
		else return(hi);
	}

	return((rand() % (hi - lo + 1)) + lo);
}

float GetRandom(float min, float max)
{
	float randNum = (float)rand() / RAND_MAX;
	float num = min + (max - min) * randNum;
	return num;
}

//-------------------------------------------------------------------------------------------
// Color255VectorToWord
//
// Converts a color in vector format to a word in 5-6-5 format.  Color ranges are 0-255.
// Arguments:
//		pVal - Color vector
// Return:
//		D_WORD - converted color.
//-------------------------------------------------------------------------------------------
D_WORD Color255VectorToWord( DVector *pVal )
{
	D_WORD wColor;

	// For red, multiply by 5 bits and divide by 8, which is a net division of 3 bits.  Then shift it
	// to the left 11 bits to fit into result, which is a net shift of 8 to left.
	wColor = ( D_WORD )(((( DDWORD )pVal->x & 0xFF ) << 8 ) & 0xF800 );

	// For green, multiply by 6 bits and divide by 8, which is a net division of 2 bits.  Then shift it
	// to the left 5 bits to fit into result, which is a net shift of 3 to left.
	wColor |= ( D_WORD )(((( DDWORD )pVal->y & 0xFF ) << 3 ) & 0x07E0 );

	// For blue, multiply by 5 bits and divide by 8 = divide by 3.
	wColor |= ( D_WORD )((( DDWORD )pVal->z & 0xFF ) >> 3 );

	return wColor;
}

//-------------------------------------------------------------------------------------------
// Color255WordToVector
//
// Converts a color in word format to a vector in 5-6-5 format.  Color ranges are 0-255.
// Arguments:
//		wVal - color word
//		pVal - Color vector
// Return:
//		void
//-------------------------------------------------------------------------------------------
void Color255WordToVector( D_WORD wVal, DVector *pVal )
{
	// For red, divide by 11 bits then multiply by 8 bits and divide by 5 bits = divide by 8 bits...
	pVal->x = ( DFLOAT )(( wVal & 0xF800 ) >> 8 );

	// For green, divide by 5 bits, multiply by 8 bits, divide by 6 bits = divide by 3 bits.
	pVal->y = ( DFLOAT )(( wVal & 0x07E0 ) >> 3 );

	// For blue, divide by 5 bits, multiply by 8 bits = multiply by 3 bits
	pVal->z = ( DFLOAT )(( wVal & 0x001F ) << 3 );
}



DBYTE CompressRotationByte(CommonLT *pCommon, DRotation *pRotation)
{
	DVector up, right, forward;
	float angle;
	char cAngle;

	pCommon->GetRotationVectors(*pRotation, up, right, forward);

	angle = (float)atan2(forward.x, forward.z);
	cAngle = (char)(angle * (127.0f / MATH_PI));
	return (DBYTE)cAngle;
}


void UncompressRotationByte(CommonLT *pCommon, DBYTE rot, DRotation *pRotation)
{
	float angle;

	angle = (float)(char)rot / 127.0f;
	angle *= MATH_PI;
	pCommon->SetupEuler(*pRotation, 0.0f, angle, 0.0f);
}



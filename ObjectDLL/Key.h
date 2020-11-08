// ----------------------------------------------------------------------- //
//
// MODULE  : Key.h
//
// PURPOSE : Key definition for Keyframer class
//
// CREATED : 10/7/97
//
// ----------------------------------------------------------------------- //

#ifndef __KEY_H__
#define __KEY_H__

#include "cpp_engineobjects_de.h"

class Key : public BaseClass
{
	public :

		Key();
		~Key();

		DFLOAT		m_fTimeStamp;			// when this key occurs (relative to previous key)
		DFLOAT		m_fSoundRadius;			// radius of sound
		HSTRING		m_hstrSoundName;		// sound name
		HSTRING		m_hstrMessageTarget;	// message target
		HSTRING		m_hstrMessageName;		// name of message to send
		HSTRING		m_hstrBPrintMessage;	// message to print
	
	protected :

		DDWORD		EngineMessageFn(DDWORD messageID, void *pData, DFLOAT fData);

	private :

		DBOOL ReadProp(ObjectCreateStruct *pData);
		DBOOL InitialUpdate(DVector *pMovement);
		DBOOL Update(DVector* pMovement);
};

#endif // __KEY_H__


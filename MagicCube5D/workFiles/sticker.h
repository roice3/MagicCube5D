#pragma once
#pragma managed(push, off)

#include "vectorND.h"


struct SSticker
{
	SSticker();
	SSticker( int color, CVector5D cubie, CVector5D offset );

	// comparison operators
	bool operator == ( const SSticker & rhs ) const;
	bool operator != ( const SSticker & rhs ) const;

	// The sticker color.
	// NOTE: This also determines the physical face in the solved state.
	int m_color;

	// The cubie this sticker belongs to.
	CVector5D m_cubie;

	// The offset direction of this sticker from the cubie center.
	// NOTE: This also determines the logical face this sticker happens to be on.
	CVector5D m_offset;

	// Our current/future visibilities (when rotating).
	// NOTE: We don't use this for sticker == comparisons.
	//		 It is only here for a cool fade in/out effect 
	//		 when stickers are moving.
	bool m_currentVis;
	bool m_futureVis;

	// Gets the number of colors for the cubie this sticker belongs to.
	int getNumColors() const;
};

#pragma managed(pop)
#pragma once
#pragma managed(push, off)

#include "vectorND.h"

enum EStickerAccent
{
	DIM,
	NORMAL,
	HIGHLIGHT
};

enum EStereoMode
{
	STEREO_NONE,
	STEREO_CROSS_EYED,
	STEREO_ANAGLYPH,
	STEREO_ANAGLYPH_NO_COLOR
};

// We make most of our settings public for easy access.
class CSettings
{
friend class CCube5D;

public:

	CSettings();

	// n in n^5.
	int m_n;

	// Projection parameters.
	double m_projection5Distance;
	double m_projection4Distance;

	// Spacing parameters.
	double m_faceOffset;
	double m_stickerOffset;
	double m_stickerSize;

	// Rotation step (in degrees).
	// This is used to control the rotation rate.
	double m_rotationStep;

	// Whether to show all cubie stickers or only the selected one.
	bool m_showAllCubieStickers;

	// Sticker accent settings.
	EStickerAccent m_accent1Colored;
	EStickerAccent m_accent2Colored;
	EStickerAccent m_accent3Colored;
	EStickerAccent m_accent4Colored;
	EStickerAccent m_accent5Colored;

	// Helper to cycle an accent setting.
	void cycleAccentSetting( EStickerAccent & accent ) const;

	// Stereo settings.
	EStereoMode m_stereoMode;
	double m_stereoSeparation;

	// Convenient access method.
	double getIncrementBetweenStickers() const
	{
		return( 2.0 / ( m_n - 1 ) );
	}

private:

	// Set which faces are visible.
	// We only want the CCube5D class to be able to call these.
	void setAllVisible();
	void setFaceVisible( int i, bool visible );
	bool getFaceVisible( int i ) const;

	// These faces will be invisible.
	// NOTE: We track invisibility vs. visibility since by default 
	// most faces will be shown. This will hopefully speed things up some.
	std::vector<int> m_invisibleFaces;
};

#pragma managed(pop)
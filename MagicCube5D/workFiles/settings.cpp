#include <stdafx.h>
#include "settings.h"

#pragma unmanaged


CSettings::CSettings() 
{
	m_n = 3;
	m_showAllCubieStickers = false;

	m_accent1Colored = NORMAL;
	m_accent2Colored = HIGHLIGHT;
	m_accent3Colored = NORMAL;
	m_accent4Colored = NORMAL;
	m_accent5Colored = NORMAL;

	m_stereoMode = STEREO_NONE;
	m_stereoSeparation = 1.5;
}

void 
CSettings::cycleAccentSetting( EStickerAccent & accent ) const 
{
	switch( accent )
	{
	case DIM:
		accent = NORMAL;
		break;

	case NORMAL:
		accent = HIGHLIGHT;
		break;

	case HIGHLIGHT:
		accent = DIM;
		break;

	default:
		assert( false );
	}
}

void 
CSettings::setAllVisible() 
{
	m_invisibleFaces.clear();
}

void 
CSettings::setFaceVisible( int f, bool visible ) 
{
	if( false == visible )
	{
		for( uint i=0; i<m_invisibleFaces.size(); i++ )
			if( f == m_invisibleFaces[i] )
				return;
		m_invisibleFaces.push_back( f );
	}

	if( true == visible )
	{
		for( uint i=0; i<m_invisibleFaces.size(); i++ )
			if( f == m_invisibleFaces[i] )
			{
				m_invisibleFaces.erase( m_invisibleFaces.begin() + i );
				return;
			}
	}
}

bool 
CSettings::getFaceVisible( int f ) const
{
	for( uint j=0; j<m_invisibleFaces.size(); j++ )
		if( f == m_invisibleFaces[j] )
			return false;

	return true;
}
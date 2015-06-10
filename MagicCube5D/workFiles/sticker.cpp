#include <stdafx.h>
#include "sticker.h"

#pragma unmanaged


SSticker::SSticker() 
{
	m_currentVis = true;
	m_futureVis = true;
}

SSticker::SSticker( int color, CVector5D cubie, CVector5D offset ) 
{
	m_color = color;
	m_cubie = cubie;
	m_offset = offset;
	m_currentVis = true;
	m_futureVis = true;
}

bool  
SSticker::operator == ( const SSticker & rhs ) const
{
	return( 
		m_color == rhs.m_color &&
		m_cubie == rhs.m_cubie &&
		m_offset == rhs.m_offset );
}

bool 
SSticker::operator != ( const SSticker & rhs ) const
{
	return( ! ( operator == ( rhs ) ) );
}

int 
SSticker::getNumColors() const
{
	int numColors = 0;
	for( int i=0; i<5; i++ )
		if(  1 == m_cubie.m_components[i] ||
			-1 == m_cubie.m_components[i] )
			numColors++;
	return numColors;
}
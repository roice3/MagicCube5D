#pragma once
#pragma managed(push, off)

#include <atltypes.h>
#include "cube5D.h"


class CRenderer
{
public:

	CRenderer();

	// This will do the actual rendering.
	void renderScene( int width, int height );
	void renderScene( int width, int height, bool stereo, bool registered );

	// Called to reset things.
	void reset();

	// This needs to be called once with each iteration.
	void iterationMade( bool first );

	// Set the background color.
	void setBackgroundColor( CColor c ) { m_bgColor = c; }

	// Our cube.
	CCube5D m_cube;

private:

	// GL helper methods.
	void setupProjection( int cx, int cy );
	void setupView( );

public:

	CVector3D m_viewLookat;
	CVector3D m_viewLookfrom;

	CColor m_bgColor;
};

#pragma managed(pop)
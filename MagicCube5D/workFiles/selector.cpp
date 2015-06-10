#include <stdafx.h>
#include "selector.h"

#pragma managed(push, off)


CSelector::CSelector()
{
}

void 
CSelector::unProjectMousePoint( int mouseX, int mouseY, CVector3D & linep1, CVector3D & linep2 ) 
{
	//
	// Unproject the window position at 2 z values (0 and 1) to get a line.
	//

	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	GLint x, y;			 // coordinate positions
	GLdouble wx, wy, wz; // returned world x,y,z coordinates.

	glGetIntegerv( GL_VIEWPORT, viewport );
	glGetDoublev( GL_MODELVIEW_MATRIX, mvmatrix );
	glGetDoublev( GL_PROJECTION_MATRIX, projmatrix );

	// NOTE: viewport[3] is the height of the window in pixels.
	x = mouseX;
	y = viewport[3] - (GLint)mouseY - 1;
	//TRACE( "Coordinates at cursor are (%4d,%4d)\n", x, y );

	// Do the unprojects.
	gluUnProject( (GLdouble)x, (GLdouble)y, 0.0, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );
	linep1 = CVector3D( wx, wy, wz );
	//TRACE( "World coordinates at z=0.0 are (%f,%f,%f)\n", wx, wy, wz );
	gluUnProject( (GLdouble)x, (GLdouble)y, 1.0, mvmatrix, projmatrix, viewport, &wx, &wy, &wz );
	linep2 = CVector3D( wx, wy, wz );
	//TRACE( "World coordinates at z=1.0 are (%f,%f,%f)\n", wx, wy, wz );
}
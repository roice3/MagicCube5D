#include <stdafx.h>
#include "renderer.h"

#pragma unmanaged


CRenderer::CRenderer()
{
	m_viewLookfrom = CVector3D( 40, 70, 30 );
	m_viewLookat = CVector3D();
}

void 
CRenderer::renderScene( int width, int height ) 
{
	renderScene( width, height, false, true );
}

void CRenderer::renderScene( int width, int height, bool stereo, bool registered ) 
{
	//
	// GL Drawing setup.
	//

	// Setup the projection.
	setupProjection( width, height );

	// Clear the color.
	{
		glClearColor( (float)m_bgColor.m_r, (float)m_bgColor.m_g, (float)m_bgColor.m_b, 1 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}

	// Enable depth testing.
	glEnable( GL_DEPTH_TEST );

	// Enable alpha blending.
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Smooth shading.
	glShadeModel( GL_SMOOTH );

	// Anitaliasing.
	// Stereo doesn't like this.
	if( ! stereo )
	{
		glEnable( GL_LINE_SMOOTH );
		glEnable( GL_POINT_SMOOTH );
		//glEnable( GL_POLYGON_SMOOTH );	// Mades the polygons look bad (GeForce4 implemented this).
	}

	// Setup the view.
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	setupView( );

	//
	// Render everything here.
	//
	m_cube.render( m_viewLookfrom );

	// Setup for text drawing.
	/*
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0, (GLfloat)width, 0,  (GLfloat)height );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	*/

	//
	// Render all the text here.
	//

	glFinish();
}

void CRenderer::reset( )
{ 
	iterationMade( true );
}

void CRenderer::iterationMade( bool first ) 
{
	m_cube.iterateRotate();
}

void CRenderer::setupProjection( int cx, int cy ) 
{
	// Calculate the clipping plane based on the current view.
	//
	// This may need to be revisited now that there are many different viewing possibilities.
	// Seem ok though.
	double abs = m_viewLookfrom.abs( );
	GLdouble clipNear = 0.01f;		//NEAR_CLIPPING_PLANE;
	GLdouble clipFar  = abs + 1000.0f;	//abs + ENV_OUTER_STAR_LIMITS;
	clipFar *= 1.1;

	if( cy > 0 )
	{
		glViewport( 0, 0, cx, cy );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );
		gluPerspective( 30, (GLdouble)cx/cy, clipNear, clipFar );
		glMatrixMode( GL_MODELVIEW );
	}
}

void CRenderer::setupView( ) 
{
	gluLookAt( 
		m_viewLookfrom.m_components[0], 
		m_viewLookfrom.m_components[1], 
		m_viewLookfrom.m_components[2], 
		0, 0, 0, 0, 0, 1 );
}
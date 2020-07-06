#pragma once
#pragma managed(push, off)

#include <stdafx.h>


class CGlInit
{

public:

	CGlInit();
	HGLRC initializeGL( HWND hwnd );
	HGLRC initializeGL( HDC hContext );

private:

	BOOL setupPixelFormat( HDC hContext );
};
#pragma managed(pop)
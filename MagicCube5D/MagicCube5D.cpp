// MagicCube5D.cpp : main project file.

#include "stdafx.h"
#include "cubeFrame.h"

using namespace MagicCube5D;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
	(gcnew CubeFrame())->Run();
	return 0;
}

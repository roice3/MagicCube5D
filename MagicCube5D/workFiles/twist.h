#pragma once
#pragma managed(push, off)

#include <string>
#include <vector>

// Slicemask defines.
#define SLICEMASK_1 0x01
#define SLICEMASK_2 0x02
#define SLICEMASK_3 0x04
#define SLICEMASK_4 0x08
#define SLICEMASK_5 0x10
#define SLICEMASK_6 0x20
#define SLICEMASK_7 0x40


// Simple struct for a twist (also used for view rotations).
// I went ahead and moved this to a separate file, since it 
// will get more complicated if we start allowing more complex twists.
struct STwist
{
	STwist()
	{
		m_rotationFace = -1;
		m_rotationAxis1 = 2;
		m_rotationAxis2 = 3;
		m_slicemask = 1;
	}

	// NOTE: To do "view" rotations, we mark the 
	// rotationFace as -1 (rotate all faces).
	int m_rotationFace;
	int m_rotationAxis1;
	int m_rotationAxis2;
	int m_slicemask;

	// Helper to reverse this twist.
	void reverseTwist()
	{
		int temp = m_rotationAxis1;
		m_rotationAxis1 = m_rotationAxis2;
		m_rotationAxis2 = temp;
	}
};

// A macro is just an array of twists with a name.
struct SMacro
{
	std::string m_name;
	std::vector<STwist> m_twists;
};

#pragma managed(pop)
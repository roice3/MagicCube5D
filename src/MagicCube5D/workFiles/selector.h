#pragma once
#pragma managed(push, off)

#include "vectorND.h"

class CSelector {
public:
    CSelector();

    // Get a world line from a mouse point.
    static void unProjectMousePoint(int x, int y, CVector3D& linep1, CVector3D& linep2);
};

#pragma managed(pop)

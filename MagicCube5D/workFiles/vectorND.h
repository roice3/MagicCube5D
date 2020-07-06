#pragma once

#pragma managed(push, off)

#include <float.h>
#include <stdafx.h>

#include <vector>

#include "helper.h"

typedef unsigned int uint;

#define DO_OP_VECTOR_NEW(operation)                                          \
    CSameVector ret;                                                         \
    for (int i = 0; i < Size; ++i)                                           \
        ret.m_components[i] = m_components[i] operation rhs.m_components[i]; \
    ret.CheckVector();                                                       \
    return (ret);

#define DO_OP_VECTOR_INPLACE(operation)                                               \
    for (int i = 0; i < Size; ++i) m_components[i] operation## = rhs.m_components[i]; \
    CheckVector();                                                                    \
    return (*this);

#define DO_OP_SCALAR_NEW(operation)                                                   \
    CSameVector ret;                                                                  \
    for (int i = 0; i < Size; ++i) ret.m_components[i] = m_components[i] operation a; \
    ret.CheckVector();                                                                \
    return (ret);

#define DO_OP_SCALAR_INPLACE(operation)                             \
    for (int i = 0; i < Size; ++i) m_components[i] operation## = a; \
    CheckVector();                                                  \
    return (*this);

inline bool isNaN(double value) {
    int floatType = _fpclass(value);
    return floatType == _FPCLASS_SNAN || floatType == _FPCLASS_QNAN ||
           floatType == _FPCLASS_NINF || floatType == _FPCLASS_PINF;
}

template <int nSize>
class CVectorND {
public:
    static const int Size = nSize;

    typedef CVectorND<nSize> CSameVector;

    // Make component values public for easy access.
    double m_components[16];

    // Construction.
    CVectorND() {
        InitDebug();
        reset();
    }

    void reset() {
        for (int i = 0; i < Size; ++i) m_components[i] = 0;
    }

    bool valid() {
        for (int i = 0; i < Size; ++i)
            if (isNaN(m_components[i])) return false;
        return true;
    }

    CSameVector& operator=(const CSameVector& rhs) {
        for (int i = 0; i < Size; ++i) m_components[i] = rhs.m_components[i];
        return *this;
    }

    bool operator==(const CSameVector& rhs) const {
        for (int i = 0; i < Size; ++i)
            if (m_components[i] != rhs.m_components[i]) return false;
        return true;
    }

    bool operator!=(const CSameVector& rhs) const {
        for (int i = 0; i < Size; ++i)
            if (m_components[i] != rhs.m_components[i]) return true;
        return false;
    }

    // Arithmetic operators.
    // NOTE: Operations on vectors of different dimension will just return a 0 dimension
    // vector.
    CSameVector operator+(const CSameVector& rhs) const { DO_OP_VECTOR_NEW(+); }
    CSameVector& operator+=(const CSameVector& rhs) { DO_OP_VECTOR_INPLACE(+); }
    CSameVector operator-(const CSameVector& rhs) const { DO_OP_VECTOR_NEW(-); }
    CSameVector& operator-=(const CSameVector& rhs) { DO_OP_VECTOR_INPLACE(-); }
    CSameVector operator+(double a) const { DO_OP_SCALAR_NEW(+); }
    CSameVector& operator+=(double a) { DO_OP_SCALAR_INPLACE(+); }
    CSameVector operator-(double a) const { DO_OP_SCALAR_NEW(-); }
    CSameVector& operator-=(double a) { DO_OP_SCALAR_INPLACE(-); }
    CSameVector operator*(double a) const { DO_OP_SCALAR_NEW(*); }
    CSameVector& operator*=(double a) { DO_OP_SCALAR_INPLACE(*); }
    CSameVector operator/(double a) const { DO_OP_SCALAR_NEW(/); }
    CSameVector& operator/=(double a) { DO_OP_SCALAR_INPLACE(/); }

    // Unary negation
    CSameVector operator-() const {
        CSameVector ret = *this * -1;
        return ret;
    }

    // Dot Product.
    double dot(const CSameVector& rhs) const {
        CheckVector();
        double ret = 0;
        for (int i = 0; i < Size; ++i) ret += m_components[i] * rhs.m_components[i];
        return ret;
    }

    // The square of the magnitude.  This is here for optimizations.
    // It allows avoiding square roots when doing magnitude comparisons.
    double magSquared() const {
        CheckVector();
        double magSquared = 0;
        for (int i = 0; i < Size; ++i) magSquared += m_components[i] * m_components[i];
        return (magSquared);
    }

    // Absolute Value.
    double abs() const { return (sqrt(magSquared())); }

    // Calculate the distance to another vector.
    double distance(const CSameVector& rhs) const {
        CheckVector();
        double magSquared = 0;
        for (uint i = 0; i < Size; ++i) {
            double componentOffset = rhs.m_components[i] - m_components[i];
            magSquared += componentOffset * componentOffset;
        }
        return sqrt(magSquared);
    }

    // Normalize this vector.  Returns false if degenerate.
    bool normalize() {
        CheckVector();
        double magnitude = abs();
        if (IS_ZERO(magnitude)) return false;
        *this /= magnitude;
        return true;
    }

    // Empty out this vector.
    void empty() {
        for (int i = 0; i < Size; ++i) m_components[i] = 0;
    }

    // Rotates the point described by this vector parallel to a
    // plane defined by 2 coordinate axis (x=0, y=1, z=2, etc.).
    void rotate(int axis1, int axis2, double angle) {

        // Copy of the 2 components we need for calculations
        double component1 = m_components[axis1];
        double component2 = m_components[axis2];

        // Do the rotation.
        m_components[axis1] = cos(angle) * component1 - sin(angle) * component2;
        m_components[axis2] = sin(angle) * component1 + cos(angle) * component2;

        CheckVector();
    }

    // Central projections.
    // dimension (x=1, y=2, etc.) should be less than or equal to Size.
    // distance is the distance to the point we are doing perspective projection from.
    CSameVector& project(int dimension, double distance) {
        assert(dimension <= Size);
        int index = dimension - 1;

        // Required for calulations.
        double denominator = distance - m_components[index];
        if (IS_ZERO(denominator)) denominator = 0;

        // Make points with a negative denominator invalid.
        if (denominator < 0) denominator = 0;

        // The projection.
        for (int i = 0; i < Size; i++) {
            if (i != index)
                m_components[i] *= distance / denominator;
            else
                m_components[i] = 0;
        }

        return (*this);
    }

private:

    // debug helpers
#ifdef _DEBUG
    static const int DebugSize = 16;
    void InitDebug() {
        for (int i = Size; i < DebugSize; ++i) m_components[i] = 42;
    }
    void CheckVector() const {
        for (int i = Size; i < DebugSize; ++i) assert(m_components[i] == 42);
    }
#else
    void InitDebug() {}
    void CheckVector() const {}
#endif
};

// Specialized 3D version.
class CVector3D : public CVectorND<3> {
public:
    CVector3D() {}
    CVector3D(const CSameVector& rhs) { static_cast<CVectorND<3>&>(*this) = rhs; }
    CVector3D(double x, double y, double z) {
        m_components[0] = x;
        m_components[1] = y;
        m_components[2] = z;
    }

    // Cross Product.
    CVector3D cross(const CVector3D& vector3D) const {
        double xVal = m_components[1] * vector3D.m_components[2] -
                      m_components[2] * vector3D.m_components[1];
        double yVal = m_components[2] * vector3D.m_components[0] -
                      m_components[0] * vector3D.m_components[2];
        double zVal = m_components[0] * vector3D.m_components[1] -
                      m_components[1] * vector3D.m_components[0];
        return CVector3D(xVal, yVal, zVal);
    }
    CVector3D& cross(const CVector3D& vector3D) {
        double xVal = m_components[1] * vector3D.m_components[2] -
                      m_components[2] * vector3D.m_components[1];
        double yVal = m_components[2] * vector3D.m_components[0] -
                      m_components[0] * vector3D.m_components[2];
        double zVal = m_components[0] * vector3D.m_components[1] -
                      m_components[1] * vector3D.m_components[0];
        m_components[0] = xVal;
        m_components[1] = yVal;
        m_components[2] = zVal;
        return (*this);
    }
};

// Specialized 4D version.
class CVector4D : public CVectorND<4> {
public:
    CVector4D() {}
    CVector4D(const CSameVector& rhs) { static_cast<CVectorND<4>&>(*this) = rhs; }
    CVector4D(double x, double y, double z, double w) {
        m_components[0] = x;
        m_components[1] = y;
        m_components[2] = z;
        m_components[3] = w;
    }
};

// Specialized 5D version.
class CVector5D : public CVectorND<5> {
public:
    CVector5D() {}
    CVector5D(const CSameVector& rhs) { static_cast<CVectorND<5>&>(*this) = rhs; }
    CVector5D(double x, double y, double z, double w, double v) {
        m_components[0] = x;
        m_components[1] = y;
        m_components[2] = z;
        m_components[3] = w;
        m_components[4] = v;
    }

    // Specific central projections.
    CVector5D& projectTo4D(double distance) {
        project(5, distance);
        return (*this);
    }
    CVector5D& projectTo3D(double distance) {
        project(4, distance);
        return (*this);
    }

    // Get the zero component axes of a CVector5D.
    // NOTE: This methods is intended to be called with a vector that only has 1
    // non-zero component equal to 1 or -1.
    void getZeroAxes(int& axis1, int& axis2, int& axis3, int& axis4) const {

        // Calculate the axes.
        CVector4D axes;
        for (int j = 0, idx = 0; j < 5; j++) {
            double component = m_components[j];
            if (component > -0.5 && component < 0.5) axes.m_components[idx++] = j;
        }

        axis1 = (int)axes.m_components[0];
        axis2 = (int)axes.m_components[1];
        axis3 = (int)axes.m_components[2];
        axis4 = (int)axes.m_components[3];
    }

    // Get the unit valued axis of a CVector5D.
    // NOTE: This method is intended to be called with a vector that only has 1 unit
    // magnitude component equal to 1 or -1.
    //		 (It may have other non-zero components though).
    void getUnitValuedAxis(int& axis) const {
        for (int j = 0; j < 5; j++) {

            // NOTE: We could tighten the tolerance, but it is not necessary.
            double component = m_components[j];
            if (component < -0.9) {
                axis = j;
                return;
            }

            if (component > 0.9) {
                axis = j;
                return;
            }
        }
    }
};

#pragma managed(pop)

#pragma once
#include <string>

#pragma managed(push, off)

// Radian/Degree conversion.
#define CONSTANT_PI	3.14159265358979323846f
template <class T>
T rad2deg(T r) { return (T)(180. * (r / CONSTANT_PI)); }
template <class U>
U deg2rad(U r) { return (U)(CONSTANT_PI * (r / 180.)); }


#define TOLERANCE 0.0000000000001
#define IS_ZERO( value )	\
	( ( ( (value) > -TOLERANCE ) && ( (value) < TOLERANCE ) ) ? TRUE : FALSE )


// Get random values between 0 and n.
inline double getRandomDouble( double n ) 
{
	double random = (double)rand() / RAND_MAX;
	random *= n;
	return( random );
}
inline int getRandomInt( int n ) 
{	
	double randomDouble = getRandomDouble( (double)( n + 1 ) );
	
	// Very small chance we were n+1.
	if( randomDouble == (double)( n + 1 ) )
		randomDouble -= 1;

	return( int(randomDouble) );
}


struct CColor
{
public:

	// Make member variables public for easy access.
	double m_r;
	double m_g;
	double m_b;
	double m_a;

	// Construction.
	CColor( );
	CColor( double r, double g, double b, double a );

	// == operator
	bool operator == ( const CColor & rhs ) const
	{
		return( 
			m_r == rhs.m_r &&
			m_g == rhs.m_g &&
			m_b == rhs.m_b &&
			m_a == rhs.m_a );
	}
};

inline CColor::CColor( )
{
	// White
	m_r = 1;
	m_g = 1;
	m_b = 1;
	m_a = 1;
}

inline CColor::CColor( double r, double g, double b, double a )
{
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}

#pragma managed(pop)

#pragma managed(push, on)

inline std::string systemStringToStdString( System::String ^ input )
{
	// There has got to be a better way to do this!
	std::string returnString;
	for( int i=0; i<input->Length; i++ )
		returnString.push_back( (char)input[i] );
	return returnString;
}

#pragma managed(pop)

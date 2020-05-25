#include <stdafx.h>
#include "stateTransformer.h"

#pragma unmanaged

#include <algorithm>
#include <functional>
#include <map>

CStateTransformer::CStateTransformer( CSettings & settings ) : m_settings( settings )
{
}

void 
CStateTransformer::stickersToMatrix( const std::vector<SSticker> & stickers, StateMatrix & state ) const
{
	int numPerFace = getNumPerFace();
	assert( stickers.size() == 10 * numPerFace );

	// Size our state matrix.
	state.resize( 10 );
	for( int f=0; f<10; f++ )
		state[f].resize( numPerFace );

	// Cycle through the stickers and fill out the state.
	for( uint i=0; i<stickers.size(); i++ )
	{
		int face = getFaceFromOffset( stickers[i].m_offset );
		int sticker = getStickerInt( stickers[i].m_cubie, stickers[i].m_offset );
		state[face][sticker] = stickers[i].m_color;
	}
}

void 
CStateTransformer::matrixToStickers( const StateMatrix & state, std::vector<SSticker> & stickers ) const
{
	stickers.clear();

	int numPerFace = getNumPerFace();

	// Cycle through the state and fill out our stickers.
	for( uint f=0; f<state.size(); f++ )
	{
		assert( state[f].size() == numPerFace );
		for( uint s=0; s<state[f].size(); s++ )
		{
			SSticker sticker;
			sticker.m_color = state[f][s];
			getFaceOffset( f, sticker.m_offset );
			getCubie( s, sticker.m_offset, sticker.m_cubie );
			stickers.push_back( sticker );
		}
	}
}

namespace
{
	void getFaceOffsetInternal( int face, CVector5D & offset ) 
	{
		switch( face )
		{
		case 0:
			offset = CVector5D( 1,0,0,0,0 );
			break;
		case 1:
			offset = CVector5D( -1,0,0,0,0 );
			break;
		case 2:
			offset = CVector5D( 0,1,0,0,0 );
			break;
		case 3:
			offset = CVector5D( 0,-1,0,0,0 );
			break;
		case 4:
			offset = CVector5D( 0,0,1,0,0 );
			break;
		case 5:
			offset = CVector5D( 0,0,-1,0,0 );
			break;
		case 6:
			offset = CVector5D( 0,0,0,1,0 );
			break;
		case 7:
			offset = CVector5D( 0,0,0,-1,0 );
			break;
		case 8:
			offset = CVector5D( 0,0,0,0,1 );
			break;
		case 9:
			offset = CVector5D( 0,0,0,0,-1 );
			break;
		default:
			assert( false );
			offset = CVector5D();
		}
	}

	CVector5D getFaceOffsetInternal( int face )
	{
		CVector5D ret;
		getFaceOffsetInternal( face, ret );
		return ret;
	}

	int 
	getFaceFromOffsetInternal( const CVector5D & offset )
	{
		if( offset == CVector5D( 1,0,0,0,0 ) )
			return 0;
		if( offset == CVector5D( -1,0,0,0,0 ) )
			return 1;
		if( offset == CVector5D( 0,1,0,0,0 ) )
			return 2;
		if( offset == CVector5D( 0,-1,0,0,0 ) )
			return 3;
		if( offset == CVector5D( 0,0,1,0,0 ) )
			return 4;
		if( offset == CVector5D( 0,0,-1,0,0 ) )
			return 5;
		if( offset == CVector5D( 0,0,0,1,0 ) )
			return 6;
		if( offset == CVector5D( 0,0,0,-1,0 ) )
			return 7;
		if( offset == CVector5D( 0,0,0,0,1 ) )
			return 8;
		if( offset == CVector5D( 0,0,0,0,-1 ) )
			return 9;
		assert( false );
		return -1;
	}
}

void 
CStateTransformer::getFaceOffset( int face, CVector5D & offset ) const
{
	getFaceOffsetInternal( face, offset );
}

int 
CStateTransformer::getFaceFromOffset( const CVector5D & offset ) const
{
	return getFaceFromOffsetInternal( offset );
}

void 
CStateTransformer::getCubie( int stickerInt, const CVector5D & face, CVector5D & cubie ) const
{
	cubie.empty();

	int axis1, axis2, axis3, axis4;
	face.getZeroAxes( axis1, axis2, axis3, axis4 );
	std::vector<int> axes;
	axes.push_back( axis1 );
	axes.push_back( axis2 );
	axes.push_back( axis3 );
	axes.push_back( axis4 );

	double increment = m_settings.getIncrementBetweenStickers();

	// Get the 4 components.
	// This is like getting the digits of cubieInt as a base m_n number.
	int num = m_settings.m_n;
	for( int i=0; i<4; i++ )
	{
		int temp = ( stickerInt / (int)pow( (double)num, i ) ) % num;
		double temp2 = increment * temp;
		temp2 -= 1;
		cubie.m_components[axes[i]] = temp2;
	}

	cubie += face;
}

namespace
{
	int roundDoubleToInt( double val )
	{
		// Get the fractional part.
		double fraction = val - (int)val;
		if( fraction > 0.5 )
			return (int)val + 1;
		else
			return (int)val;
	}
}

int 
CStateTransformer::getStickerInt( const CVector5D & cubie, const CVector5D & face ) const
{
	double increment = m_settings.getIncrementBetweenStickers();

	int axis1, axis2, axis3, axis4;
	face.getZeroAxes( axis1, axis2, axis3, axis4 );

	int num = m_settings.m_n;
	int stickerInt = (int)( 
		roundDoubleToInt ( ( cubie.m_components[axis1] + 1 ) / increment ) * pow( (double)num, 0 ) +
		roundDoubleToInt ( ( cubie.m_components[axis2] + 1 ) / increment ) * pow( (double)num, 1 ) +
		roundDoubleToInt ( ( cubie.m_components[axis3] + 1 ) / increment ) * pow( (double)num, 2 ) +
		roundDoubleToInt ( ( cubie.m_components[axis4] + 1 ) / increment ) * pow( (double)num, 3 ) );
	return( stickerInt );
}

bool 
CStateTransformer::isSolved( const StateMatrix & state ) const
{
	int numPerFace = getNumPerFace();
	assert( state.size() );
	assert( state[0].size() );

	for( uint f=0; f<10; f++ )
	{
		int start = state[f][0];
		for( int s=1; s<numPerFace; s++ )
		{
			if( state[f][s] != start )
				return false;
		}
	}

	return true;
}

namespace
{
	// NOTE: This is valid for faces or colors.
	__int8 getMerged( __int8 val )
	{
		switch( val )
		{
		case 0:
		case 1:
			return 0;
		case 2:
		case 3:
			return 1;
		case 4:
		case 5:
			return 2;
		case 6:
		case 7:
			return 3;
		case 8:
		case 9:
			return 4;
		default:
			assert( false );
			return -1;
		}
	}

	int getMergedFaceFromOffset( const CVector5D & offset )
	{
		int face = getFaceFromOffsetInternal( offset );
		return getMerged( face );
	}

	// NOTE: This is valid for faces or colors.
	__int8 unmerge( __int8 val, bool opposite )
	{
		switch( val )
		{
		case 0:
			return opposite ? 1 : 0;
		case 1:
			return opposite ? 3 : 2;
		case 2:
			return opposite ? 5 : 4;
		case 3:
			return opposite ? 7 : 6;
		case 4:
			return opposite ? 9 : 8;
		default:
			assert( false );
			return -1;
		}
	}

	// NOTE: This is valid for faces or colors.
	int getOpposite( int val )
	{
		switch( val )
		{
		case 0:
			return 1;
		case 1:
			return 0;
		case 2:
			return 3;
		case 3:
			return 2;
		case 4:
			return 5;
		case 5:
			return 4;
		case 6:
			return 7;
		case 7:
			return 6;
		case 8:
			return 9;
		case 9:
			return 8;
		default:
			assert( false );
			return -1;
		}
	}

	// Binary Predicate for ordering CVector5Ds (so they can be used as keys to a map below).
	struct compare5D : public std::binary_function<CVector5D,CVector5D,bool> 
	{
		bool operator()( const CVector5D & _Left, const CVector5D & _Right ) const
		{
			for( int i=0; i<5; i++ )
			{
				if( _Left.m_components[i] < _Right.m_components[i] )
					return true;

				if( _Left.m_components[i] > _Right.m_components[i] )
					return false;
			}

			// Making it here means we are equal.
			return false;
		}
	};

	// XXX - These maps could have been simpler! Map could be from int to int
	//		 where getFaceFromOffset() function is called to get face as int.
	//		 It had been so long I forgot about that function, and changing this
	//		 might be worthwhile if speed is improved.  Maybe it is no big deal though.
	typedef std::map<CVector5D,int,compare5D> CubieMap;
	typedef std::map<CVector5D,int,compare5D>::const_iterator CubeMapIterator;

	void searchTrackedColors( __int16 colorTracker[5][5], std::vector<__int8> & foundColors, std::vector<__int8> & foundFaces )
	{
		int max = 0, found_r = -1, found_c = -1;
		for( int r=0; r<5; r++ )
		{
			std::vector<__int8>::const_iterator rIt = find( foundColors.begin(), foundColors.end(), r );
			if( rIt != foundColors.end() )
				continue;

			for( int c=0; c<5; c++ )
			{
				std::vector<__int8>::const_iterator cIt = find( foundFaces.begin(), foundFaces.end(), c );
				if( cIt != foundFaces.end() )
					continue;

				__int16 & val = colorTracker[r][c];
				if( val >= max )
				{
					max = val;
					found_r = r;
					found_c = c;
				}
			}
		}

		foundColors.push_back( found_r );
		foundFaces.push_back( found_c );
	}

	void findCurrentFaceColorsEvenHelper( const std::vector<SSticker> & stickers, const CSettings & settings,
		CubieMap & colorMap, int stickerTypeToUse )
	{
		/*
		This is kind of a bitch!  I want the computer to do what a human could do pretty naturally...

		There are constraints on the ordering of the faces that should be used in this heuristic, 
		e.g. certain colors have to be opposite each other, so selecting the color for one face 
		effectively selects the color for two faces. Also, once 8 are picked, the last two are 
		determined, so all this means really we are picking the best 4 face<->color matchups.

		So after some thought, what I'm going to do is generate a 5x5 matrix of numbers tracking
		the number of colors on the various faces.
		Columns will be "merged" faces and rows will be "merged" color (opposite faces will have 
		entries put into the 5 opposite faces based on opposite color).
		This means entries will range from 0 to 32, twice the amount of 1C pieces on a face.  
		Once the matrix is filled, my selection algorithm will go like this:  
		Find the highest number in the matrix and set the 2 faces according to the color.
		Now search for the next highest number disregarding items from the row and column the first number was in.
		Now search for the next highest number disregarding both rows and columns for the first 2 numbers.
		Repeat this process to find the final needed 4th number.  The 5th number will then be determined by constraints.

		This description leaves out some of the reasoning I've made, but I think this is good.  
		Specifically, it avoids problems of doing matchups that effectively only search for highest numbers
		along a row or a column, not both (which is sort of the natural first approach to take, 
		e.g. search a face for which color is the most common, then do the matchup, but then what happens
		when this process also matches the same color to another face?).
		*/

		__int16 colorTracker[5][5];
		for( int r=0; r<5; r++ )
			for( int c=0; c<5; c++ )
				colorTracker[r][c] = 0;

		// Turns out we need this too.
		// The reason to still keep around the 5x5 matrix above is to weight sets of opposite faces in the choice.
		__int16 colorTrackerFull[10][10];
		for( int r=0; r<10; r++ )
			for( int c=0; c<10; c++ )
				colorTrackerFull[r][c] = 0;

		for( uint i=0; i<stickers.size(); i++ )
		{
			const SSticker & s = stickers[i];
			if( stickerTypeToUse != s.getNumColors() )
				continue;

			__int8 mergedFace = getMergedFaceFromOffset( s.m_offset );
			__int8 color = getMerged( s.m_color );
			colorTracker[color][mergedFace]++;
			colorTrackerFull[s.m_color][getFaceFromOffsetInternal(s.m_offset)]++;
		}

		// Do the matchups.
		std::vector<__int8> foundColors, foundFaces;
		searchTrackedColors( colorTracker, foundColors, foundFaces );
		searchTrackedColors( colorTracker, foundColors, foundFaces );
		searchTrackedColors( colorTracker, foundColors, foundFaces );
		searchTrackedColors( colorTracker, foundColors, foundFaces );
		searchTrackedColors( colorTracker, foundColors, foundFaces ); // We could be more efficient, but this will work.
		assert( 5 == foundColors.size() && 5 == foundFaces.size() );

		// Now setup our color map.
		int numSwitched = 0;
		for( int i=0; i<5; i++ )
		{
			__int8 face1 = unmerge( foundFaces[i], false );
			__int8 face2 = unmerge( foundFaces[i], true );
			__int8 color1 = unmerge( foundColors[i], false );
			__int8 color2 = unmerge( foundColors[i], true );

			// See if we need to switch them.
			bool needToSwitch = false;
			if( 5 == i )
			{
				// There can only be an even number of switches!
				if( 0 == numSwitched % 2 )
					needToSwitch = false;
				else
					needToSwitch = true;
			}
			else
				needToSwitch = colorTrackerFull[color1][face1] < colorTrackerFull[color2][face1];

			if( needToSwitch )
			{
				numSwitched++;
				__int8 temp = face2;
				face2 = face1;
				face1 = temp;
			}

			colorMap[ getFaceOffsetInternal( face1 ) ] = color1;
			colorMap[ getFaceOffsetInternal( face2 ) ] = color2;
		}
	}

	void findCurrentFaceColors( const std::vector<SSticker> & stickers, const CSettings & settings,
		CubieMap & colorMap )
	{
		// NOTE: This is heuristic for even puzzles, which have no static central pieces.
		// XXX - Maybe this function should also go ahead and return the number of 1C pieces solved?

		// Look at the 1C pieces.
		switch( settings.m_n )
		{
		case 2:
		{
			findCurrentFaceColorsEvenHelper( stickers, settings, colorMap, 5 );
			return;
		}
		case 3:
		{
			for( uint i=0; i<stickers.size(); i++ )
			{
				const SSticker & s = stickers[i];
				if( 1 != s.getNumColors() )
					continue;
				
				colorMap[s.m_cubie] = s.m_color;
			}
			return;
		}
		case 4:
		case 6:
		{
			findCurrentFaceColorsEvenHelper( stickers, settings, colorMap, 1 );
			return;
		}
		case 5:
		case 7:
		{
			for( uint i=0; i<stickers.size(); i++ )
			{
				const SSticker & s = stickers[i];
				if( 1 != s.getNumColors() )
					continue;
				
				if( s.m_cubie == s.m_offset )
					colorMap[s.m_cubie] = s.m_color;
			}
			return;
		}
		default:
			assert( false );
		}
	}

	int getNumOfCubieType( int numColors, int m_n )
	{
		if( 2 == m_n )
		{
			if( 5 == numColors )
				return 32;

			return 0;
		}

		int ret = 0;
		switch( numColors )
		{
		case 0:
			ret = 1;
			break;
		case 1:
			ret = 10;
			break;
		case 2:
			ret = 40;
			break;
		case 3:
			ret = 80;
			break;
		case 4:
			ret = 80;
			break;
		case 5:
			ret = 32;
			break;
		default:
			assert( false );
		}

		// Correct for the puzzle type.
		if( m_n > 3 )
			ret *= (int)pow( (float)m_n - 2, 5 - numColors );

		return ret;
	}
}

void 
CStateTransformer::getSolvedStatus( const std::vector<SSticker> & stickers, SSolvedInfo & si ) const
{
	si.reset();

	// Get a map of where the current physical faces are.
	CubieMap colorMap;
	findCurrentFaceColors( stickers, m_settings, colorMap );

	// Now cycle through the stickers and track everything.
	// It is actually easier to track items not solved, so we'll do that.
	// (because 1 bad sticker can mark this vs. tracking solved where every sticker needs to be verified).
	CubieMap map1C, map2C, map3C, map4C, map5C;
	CubeMapIterator it;
	for( uint i=0; i<stickers.size(); i++ )
	{
		const SSticker & s = stickers[i];

		it = colorMap.find( s.m_offset );
		assert( it != colorMap.end() );
		int color = it->second;

		if( s.m_color == color )
		{
			si.ss++;
		}
		else
		{
			switch( s.getNumColors() )
			{
			case 1:
				map1C[s.m_cubie] = 1;
				break;
			case 2:
				map2C[s.m_cubie] = 1;
				break;
			case 3:
				map3C[s.m_cubie] = 1;
				break;
			case 4:
				map4C[s.m_cubie] = 1;
				break;
			case 5:
				map5C[s.m_cubie] = 1;
				break;
			default:
				assert( false );
				break;
			}
		}
	}

	si.n0 = getNumOfCubieType( 0, m_settings.m_n );
	si.n1 = getNumOfCubieType( 1, m_settings.m_n );
	si.n2 = getNumOfCubieType( 2, m_settings.m_n );
	si.n3 = getNumOfCubieType( 3, m_settings.m_n );
	si.n4 = getNumOfCubieType( 4, m_settings.m_n );
	si.n5 = getNumOfCubieType( 5, m_settings.m_n );
	si.s0 = si.n0; 
	si.s1 = si.n1 - map1C.size();
	si.s2 = si.n2 - map2C.size();
	si.s3 = si.n3 - map3C.size();
	si.s4 = si.n4 - map4C.size();
	si.s5 = si.n5 - map5C.size();

	si.ns = getNumPerFace() * 10;
}

void 
CStateTransformer::findCubies( const std::vector<SSticker> & state, const std::vector<int> & colors,
	std::vector<CVector5D> & cubies ) 
{
	int numColors = colors.size();
	cubies.clear();

	// Track candidate cubies.
	// map is from cubie to number of colors matched.
	CubieMap candidates;
	CubeMapIterator it;

	// Get all the potential cubies.
	for( uint i=0; i<state.size(); i++ )
	{
		const SSticker & s = state[i];

		// If it is not a sticker on the right cubie type, we're not interested.
		if( numColors != s.getNumColors() )
			continue;

		// If it is not one of our colors, we are not interested.
		bool keeper = false;
		for( uint j=0; j<colors.size(); j++ )
		{
			if( s.m_color == colors[j] )
			{
				keeper = true;
				break;
			}
		}

		if( !keeper )
			continue;

		// Update the map.
		int numColorsFound = 1;
		it = candidates.find( s.m_cubie );
		if( it != candidates.end() )
			numColorsFound = it->second + 1;

		candidates[s.m_cubie] = numColorsFound;
	}

	// The candidates with the right number of colors are the ones we want.
	// Add them to the ouptut.
	for( it = candidates.begin(); it != candidates.end(); ++it )
	{
		if( it->second != numColors )
			continue;

		cubies.push_back( it->first );
	}
}

int 
CStateTransformer::getNumPerFace() const
{
	return (int)pow( (double)m_settings.m_n, 4 );
}
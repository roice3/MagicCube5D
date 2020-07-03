#include <stdafx.h>
#include "cube5D.h"

#pragma unmanaged

#define LARGE_DISTANCE 10000000


CCube5D::CCube5D() : m_transformer( m_settings )
{
	m_clickState = READY_FOR_FIRST;
	m_rotation = m_percentageDone = 0;
	m_undoMode = m_redoMode = false;
	m_solvedMessageShown = false;
	m_currentlyRecording = false;
	m_useStereoColor = false;
	m_scrambled = false;

	// Setup default settings.
	m_settings.setAllVisible();
	m_settings.m_projection5Distance = 15;
	m_settings.m_projection4Distance = 45;
	m_settings.m_faceOffset = 10;
	m_settings.m_stickerOffset = 3.5;
	m_settings.m_stickerSize = .5;
	m_settings.m_rotationStep = 5.1;

	// Setup the stickers.
	reset();

	// For random scrambling.
	srand( GetCurrentTime() );

	// Fill in the color array.
	for( int i = 0; i < 10; ++i )
		m_colors.push_back( CColor( 1, 1, 1, 1 ) );
}

void 
CCube5D::render( const CVector3D & lookFrom ) 
{
	// Some things we need to calculate for stereo.
	CVector3D axis, offset;
	if( STEREO_NONE != m_settings.m_stereoMode )
	{
		// Calculate a normalized offset between our two pictures.
		double angle = atan2( lookFrom.m_components[1], lookFrom.m_components[0] );
		offset = lookFrom;
		offset.m_components[2] = 0;
		offset.normalize();
		offset.rotate( 0, 1, deg2rad(90.0) );

		// Calculate the axis around which we need to do our rotation.
		axis = lookFrom.cross( offset );
	}

	switch( m_settings.m_stereoMode )
	{
	case STEREO_NONE:
		{
			m_useStereoColor = false;
			renderCube();
			break;
		}
	case STEREO_CROSS_EYED:
		{
			m_useStereoColor = false;

			// Our offset is big.
			offset *= 20;

			// The projection naturally "rotates" the two images some because of the offset.
			// This is a correction angle for that effect.
			double correction = rad2deg( atan( 20.0 / lookFrom.abs() ) );

			// right eye image (on left side)
			glPushMatrix();
				glTranslated( -offset.m_components[0], -offset.m_components[1], 0 );
				glRotated( -m_settings.m_stereoSeparation + correction, 
					axis.m_components[0], axis.m_components[1], axis.m_components[2] );
				renderCube();
			glPopMatrix();	

			// left eye image (on right side)
			glPushMatrix();
				glTranslated( offset.m_components[0], offset.m_components[1], 0 );
				glRotated( m_settings.m_stereoSeparation - correction, 
					axis.m_components[0], axis.m_components[1], axis.m_components[2] );
				renderCube();
			glPopMatrix();

			break;
		}
	case STEREO_ANAGLYPH:
	case STEREO_ANAGLYPH_NO_COLOR:
		{
			// Blend together these overlapping images.
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			glDepthMask( GL_FALSE );

			if( STEREO_ANAGLYPH == m_settings.m_stereoMode )
			{
				m_useStereoColor = false;
				renderCube();
			}

			// Make our offset pretty small and scale with the separation.
			// NOTE: We could take this out or make it a completely separate option with a separate slider.
			// It effectively moves everything in or out of the screen (negative values move everything into the screen).
			// I played with different values, and this seemed pretty good since it pops it out of the screen somewhat.
			//offset.empty();
			offset *= m_settings.m_stereoSeparation / 5;

			m_useStereoColor = true;

			// red image
			//m_stereoColor = CColor( 1,0,0,1 );
			m_stereoColor = CColor( 1,0,0,.5 );
			glPushMatrix();
				glTranslated( -offset.m_components[0], -offset.m_components[1], 0 );
				glRotated( -m_settings.m_stereoSeparation, 
					axis.m_components[0], axis.m_components[1], axis.m_components[2] );
				renderCube();
			glPopMatrix();

			// cyan image
			m_stereoColor = CColor( 0,1,1,.5 );
			glPushMatrix();
				glTranslated( offset.m_components[0], offset.m_components[1], 0 );
				glRotated( m_settings.m_stereoSeparation, 
					axis.m_components[0], axis.m_components[1], axis.m_components[2] );
				renderCube();
			glPopMatrix();

			break;
		}
	}
}

void 
CCube5D::setPuzzleType( int n ) 
{
	m_settings.m_n = n;
	reset();
}

void 
CCube5D::setFaceVisible( int i, bool visible ) 
{
	m_settings.setFaceVisible( i, visible );
	calculateVisibilities();
}

bool 
CCube5D::getFaceVisible( int i ) const
{
	return( m_settings.getFaceVisible( i ) );
}

void 
CCube5D::setFaceColor( int i, CColor c ) 
{
	m_colors[i] = c;

	// This invalidates how we may have calculated these.
	m_cubiesToHighlight.clear();
}

CColor 
CCube5D::getFaceColor( int i ) 
{
	return m_colors[i];
}

void 
CCube5D::startRotate( const STwist & twist ) 
{
	m_rotation = m_percentageDone = 0;
	m_currentTwist = twist;
	calculateVisibilities();
}

void 
CCube5D::iterateRotate() 
{
	m_rotation += deg2rad( m_settings.m_rotationStep );
	m_percentageDone = m_rotation / deg2rad( 90.0 );
}

void 
CCube5D::finishRotate() 
{
	calculateNewStickerPositions();
	m_rotation = m_percentageDone = 0;

	if( m_undoMode )
	{
		// Remove from twist list.
		m_twists.pop_back();
		
		// Save in our redo list.
		STwist temp( m_currentTwist );
		temp.reverseTwist();
		m_redoTwists.push_back( temp );

		if( m_currentlyRecording )
			m_recordingMacro.m_twists.pop_back();

		m_undoMode = false;
		return;
	}

	if( m_redoMode )
	{
		m_redoTwists.pop_back();
		m_redoMode = false;
	}
	else
	{
		m_redoTwists.clear();
	}

	// This block should apply to normal twists and redo twists.
	m_twists.push_back( m_currentTwist );
	if( m_currentlyRecording )
		m_recordingMacro.m_twists.push_back( m_currentTwist );
}

bool 
CCube5D::getUndoParams( STwist & twist ) 
{
	if( ! m_twists.size() )
		return false;

	twist = m_twists[ m_twists.size()-1 ];
	twist.reverseTwist();

	m_undoMode = true;
	return true;
}

bool 
CCube5D::getRedoParams( STwist & twist ) 
{
	if( ! m_redoTwists.size() )
		return false;

	twist = m_redoTwists[ m_redoTwists.size()-1 ];

	m_redoMode = true;
	return true;
}

void 
CCube5D::getMacroNames( std::vector< std::string > & names ) 
{
	names.clear();

	// Load the macros from file.
	m_loader.loadMacrosFromFile( m_macros );
	for( uint i=0; i<m_macros.size(); i++ )
		names.push_back( m_macros[i].m_name );
}

void 
CCube5D::startMacroRecord() 
{
	assert( ! m_currentlyRecording );
	m_recordingMacro = SMacro();
	m_currentlyRecording = true;
}

void 
CCube5D::stopMacroRecord() 
{
	assert( m_currentlyRecording );
	m_currentlyRecording = false;
	m_recordingMacro.m_name = "New Macro";
	m_macros.push_back( m_recordingMacro );

	// We need to save.
	m_loader.saveMacrosToFile( m_macros );
}

void 
CCube5D::renameMacro( int index, std::string name ) 
{
	// Check the input bounds.
	if( index < 0 || index >= (int)m_macros.size() )
	{
		assert( false );
		return;
	}

	// Rename it.
	m_macros[index].m_name = name;

	// We need to save.
	m_loader.saveMacrosToFile( m_macros );
}

void 
CCube5D::deleteMacro( int index ) 
{
	// Check the input bounds.
	if( index < 0 || index >= (int)m_macros.size() )
	{
		assert( false );
		return;
	}

	// Delete it.
	m_macros.erase( m_macros.begin() + index );

	// We need to save.
	m_loader.saveMacrosToFile( m_macros );
}

void 
CCube5D::executeMacro( int index, bool reverse ) 
{
	// Check the input bounds.
	if( index < 0 || index >= (int)m_macros.size() )
	{
		assert( false );
		return;
	}

	// Apply all the macro twists.
	SMacro & macro = m_macros[index];
	if( reverse )
	{
		// i has to be a signed int here, 
		// or the end condition will never happen :)
		for( int i=macro.m_twists.size()-1; i>=0; i-- )
		{
			// We need to reverse the twists too.
			m_currentTwist = macro.m_twists[i];
			m_currentTwist.reverseTwist();
			finishRotate();
		}
	}
	else
	{
		for( uint i=0; i<macro.m_twists.size(); i++ )
		{
			m_currentTwist = macro.m_twists[i];
			finishRotate();
		}
	}

	// Calculate visibilities.
	calculateVisibilities();
}

void 
CCube5D::resetClickState() 
{
	// If we are already reset, clear cubies that we may be highlighting.
	if( READY_FOR_FIRST == m_clickState )
		m_cubiesToHighlight.clear();

	m_clickState = READY_FOR_FIRST;
	m_firstClickedSticker = m_selectedGotoSticker = SSticker();
	m_gotoStickers.clear();
}

bool 
CCube5D::handleFirstClick( bool control ) 
{
	if( m_clickState != READY_FOR_FIRST )
	{
		assert( false );
		return false;
	}

	if( m_selectedSticker == SSticker() )
		return false;

	if( control )
		return( handleControlClick() );

	// Has to be a 2-colored piece.
	if( m_selectedSticker.getNumColors() != 2 )
	{
		// XXX - Checking code should avoid this.
		return false;
	}

	m_firstClickedSticker = m_selectedSticker;
	getGotoStickers();

	m_clickState = READY_FOR_SECOND;
	return false;
}

bool 
CCube5D::handleSecondClick( bool control, int slicemask ) 
{
	if( m_clickState != READY_FOR_SECOND )
	{
		assert( false );
		return false;
	}

	if( control )
	{
		// This invalidates our first state.
		resetClickState();
		return( handleControlClick() );
	}

	// This will hold our twist.
	STwist twist;
	twist.m_rotationFace = m_transformer.getFaceFromOffset( m_firstClickedSticker.m_offset );
	twist.m_slicemask = slicemask;

	// Here are the rotation axes.
	CVector5D temp1 = m_firstClickedSticker.m_cubie - m_firstClickedSticker.m_offset;
	CVector5D temp2 = m_selectedGotoSticker.m_cubie - m_selectedGotoSticker.m_offset;

	temp1.getUnitValuedAxis( twist.m_rotationAxis1 );
	temp2.getUnitValuedAxis( twist.m_rotationAxis2 );

	// Do we need to reverse the twist?
	if( temp1.m_components[twist.m_rotationAxis1] * temp2.m_components[twist.m_rotationAxis2] < 0 )
		twist.reverseTwist();

	startRotate( twist );

	resetClickState();
	return true;
}

bool 
CCube5D::handleControlClick() 
{
	if( m_selectedSticker == SSticker() )
		return false;

	// This will hold our view rotation.
	STwist rotation;
	rotation.m_rotationFace = -1;

	// The first rotation axis is determined by the face of the selected sticker.
	m_selectedSticker.m_offset.getUnitValuedAxis( rotation.m_rotationAxis1 );
	if( 4 == rotation.m_rotationAxis1 )
	{
		// XXX - Checking code should avoid this.
		return false;
	}

	// The second rotation axis is implicit.
	rotation.m_rotationAxis2 = 4;

	// Do we need to reverse the rotation (is the face offset in the positive direction)?
	if( m_selectedSticker.m_offset.m_components[rotation.m_rotationAxis1] > 0 )
		rotation.reverseTwist();

	startRotate( rotation );

	return true;
}

void 
CCube5D::cycleSelectedGotoSticker( bool left ) 
{
	if( m_clickState != READY_FOR_SECOND )
		return;

	if( 6 != m_gotoStickers.size() )
	{
		assert( false );
		return;
	}

	int index = -1;
	for( uint i=0; i<m_gotoStickers.size(); i++ )
		if( m_gotoStickers[i] == m_selectedGotoSticker )
		{
			index = i;
			break;
		}
	assert( -1 != index );

	if( left )
	{
		index++;
		if( index > 5 )
			index = 0;
	}
	else
	{
		index--;
		if( index < 0 )
			index = 5;
	}

	m_selectedGotoSticker = m_gotoStickers[index];
}

void 
CCube5D::getGotoStickers() 
{
	m_gotoStickers.clear();

	if( m_firstClickedSticker == SSticker() )
		return;

	for( uint i=0; i<m_stickers.size(); i++ )
	{
		// Has to be on the same face.
		if( m_stickers[i].m_offset != m_firstClickedSticker.m_offset )
			continue;

		// Has to be a 2-colored piece.
		if( 2 != m_stickers[i].getNumColors() )
			continue;

		// We're not interested in ourself or stickers on the other side.
		CVector5D temp1 = m_firstClickedSticker.m_cubie - m_firstClickedSticker.m_offset;
		CVector5D temp2 = m_stickers[i].m_cubie - m_stickers[i].m_offset;
		if( temp1 == temp2 ||
			temp1 == temp2 * -1 )
			continue;

		// Make sure we can rotate into it.
		// A rotation will be defined by the offset axis of the cubie on the face
		// and the other 3 axes on the face.
		int axis1 = -1;
		temp1.getUnitValuedAxis( axis1 );
		int faceAxis = -1;
		m_firstClickedSticker.m_offset.getUnitValuedAxis( faceAxis );
		bool canRotateInto = false;

		// Cycle through all axes.
		for( int axis=0; axis<5; axis++ )
		{
			if( axis == axis1 || axis == faceAxis )
				continue;

			// Do both rotations and check.
			CVector5D rotated = temp2;
			rotated.rotate( axis1, axis, deg2rad( 90.0 ) );
			roundComponents( rotated );
			if( temp1 == rotated )
			{
				canRotateInto = true;
				break;
			}

			rotated = temp2;
			rotated.rotate( axis, axis1, deg2rad( 90.0 ) );
			roundComponents( rotated );
			if( temp1 == rotated )
			{
				canRotateInto = true;
				break;
			}
		}

		if( ! canRotateInto )
			continue;

		// This is one of them!
		m_gotoStickers.push_back( m_stickers[i] );
	}

	// There should be 6 of these.
	assert( 6 == m_gotoStickers.size() );
}

void 
CCube5D::save( bool saveas ) 
{
	StateMatrix state;
	m_transformer.stickersToMatrix( m_stickers, state );
	m_loader.saveToFile( state, m_twists, saveas );
}

void 
CCube5D::load() 
{
	StateMatrix state;
	if( !m_loader.loadFromFile( state, m_twists ) )
		return;

	// Use the loaded state to determine our puzzle type.
	assert( state.size() );
	if( state.size() )
	{
		bool found = false;
		for( int i=2; i<=7; i++ )
			if( state[0].size() == pow( (double)i, 4 ) )
			{
				m_settings.m_n = i;
				found = true;
				break;
			}
		assert( found );
	}

	m_transformer.matrixToStickers( state, m_stickers );
	calculateVisibilities();
}

void 
CCube5D::selectSticker( const CVector3D & linep1, const CVector3D & linep2, bool shift ) 
{
	if( READY_FOR_SECOND == m_clickState )
	{
		m_selectedSticker = SSticker();
		selectStickerHelper( linep1, linep2, shift, m_gotoStickers, false, m_selectedGotoSticker );
	}
	else
		selectStickerHelper( linep1, linep2, shift, m_stickers, true, m_selectedSticker );
}

void 
CCube5D::selectStickerHelper( const CVector3D & linep1, const CVector3D & linep2, bool shift,
	const std::vector<SSticker> & stickers, bool mustIntersect, SSticker & selectedSticker ) 
{
	// Start off with nothing selected.
	selectedSticker = SSticker();

	// Don't allow sticker selection in stereo mode.
	// XXX - perhaps this should be added in later
	//		 (if there is any chance of people solving with stereo on).
	if( STEREO_NONE != m_settings.m_stereoMode )
		return;

	// Should be called only if we are rotating.
	if( 0 != m_rotation )
	{
		assert( false );
		return;
	}

	// Get the line normal and make sure it is valid.
	CVector3D lineNormal = linep2 - linep1;
	if( IS_ZERO( lineNormal.abs( ) ) )
		return;

	// Find the closest one.
	double minDistance = LARGE_DISTANCE;
	for( uint i=0; i<stickers.size(); i++ )
	{
		if( ! stickers[i].m_currentVis )
			continue;

		// Only allow selecting 2 colored-stickers unless the shift key is down.
		if( ! shift )
			if( 2 != stickers[i].getNumColors() )
				continue;

		CVector3D position;
		getSticker3Position( stickers[i], position );

		// Distance to this sticker.
		double distance = ( ((CVector3D)( position-linep1 )).cross( lineNormal ) ).abs()/lineNormal.abs();
		if( distance < minDistance )
		{
			minDistance = distance;
			selectedSticker = stickers[i];
		}
	}

	// If nothing selected at this point, we are done.
	if( selectedSticker == SSticker() )
		return;

	if( mustIntersect )
	{
		// Now make sure the closest is actually intersected.
		// We'll just do this kind of close with a sphere test for now.
		CVector3D position, low, high;
		getSticker3Position( selectedSticker, position );
		getSticker3Extents( selectedSticker, low, high );
		double dist1 = ( position - low ).abs();
		double dist2 = ( position - high ).abs();
		double distTest = dist1 > dist2 ? dist1 : dist2;
		double distance = ( ((CVector3D)( position-linep1 )).cross( lineNormal ) ).abs()/lineNormal.abs();
		if( distTest < distance )
			selectedSticker = SSticker();
	}
}

void 
CCube5D::scramble( int numTwists ) 
{
	m_scrambled = numTwists >= 100;

	// XXX - a little hackish, since I'm checking a hardcoded value of a full scramble
	if( m_settings.m_n > 5 && 100 == numTwists )
		numTwists *= 2;

	for( int i=0; i<numTwists; i++ )
	{
		m_currentTwist.m_rotationFace = getRandomInt( 9 );

		// Get the axis of the rotation face.
		// (It won't be legal to use this axis for a rotation axis).
		int axis = -1;
		CVector5D rotationFace;
		m_transformer.getFaceOffset( m_currentTwist.m_rotationFace, rotationFace );
		rotationFace.getUnitValuedAxis( axis );

		// Generate random rotation axes.  
		// This is more complicated because we need to make sure they are valid.
		bool valid = false;
		while( ! valid )
		{
			m_currentTwist.m_rotationAxis1 = getRandomInt( 4 );
			if( m_currentTwist.m_rotationAxis1 != axis )
				valid = true;
		}

		valid = false;
		while( ! valid )
		{
			m_currentTwist.m_rotationAxis2 = getRandomInt( 4 );
			if( m_currentTwist.m_rotationAxis2 != axis &&
				m_currentTwist.m_rotationAxis2 != m_currentTwist.m_rotationAxis1 )
				valid = true;
		}

		// Generate a random slicemask (possible slice values being 1 to m_n).
		int slice = 1 + getRandomInt( m_settings.m_n - 1 );
		switch( slice )
		{
		case 1:
			m_currentTwist.m_slicemask = SLICEMASK_1;
			break;
		case 2:
			m_currentTwist.m_slicemask = SLICEMASK_2;
			break;
		case 3:
			m_currentTwist.m_slicemask = SLICEMASK_3;
			break;
		case 4:
			m_currentTwist.m_slicemask = SLICEMASK_4;
			break;
		case 5:
			m_currentTwist.m_slicemask = SLICEMASK_5;
			break;
		case 6:
			m_currentTwist.m_slicemask = SLICEMASK_6;
			break;
		case 7:
			m_currentTwist.m_slicemask = SLICEMASK_7;
			break;
		default:
			assert( false );
		}

		// Do the rotate.
		finishRotate();
	}

	// Calculate visibilities.
	calculateVisibilities();
}

void 
CCube5D::reset() 
{
	createStickers();
	m_twists.clear();
	m_redoTwists.clear();
	m_cubiesToHighlight.clear();
	calculateVisibilities();
}

void 
CCube5D::highlightCubies( std::vector<int> & colors ) 
{
	m_transformer.findCubies( m_stickers, colors, m_cubiesToHighlight );
}

bool 
CCube5D::isSolved() const
{
	StateMatrix state;
	m_transformer.stickersToMatrix( m_stickers, state );
	return m_transformer.isSolved( state );
}

bool 
CCube5D::showSolvedMessage()
{
	// We only want to show the message if an actual scramble occurred.
	if ( !m_scrambled )
		return false;

	// We only want to show a message once.
	if( m_solvedMessageShown )
		return false;

	// We'll heuristically check that this is a genuine solution by making sure 1500 twists have been done.
	// Since there are 243 pieces total, 32 of which are corners (each requiring a sequence on the order of 32 moves to place),
	// it should be safe to assume just about every manual solution will take more moves than this.
	if( m_twists.size() <= 1500 )
		return false;

	// Now check to see if the thing is actually solved.
	if( isSolved() )
	{
		m_solvedMessageShown = true;
		return true;
	}

	return false;
}

SSolvedInfo 
CCube5D::updateSolvedInfo() 
{
	m_transformer.getSolvedStatus( m_stickers, m_solvedInfo );
	return m_solvedInfo;
}

void 
CCube5D::createStickers() 
{
	// Clear them out.
	m_stickers.clear();

	// Create them all.
	int num = m_settings.m_n;
	for( int f=0; f<10; f++ )
	{
		// Get the offset for this face.
		CVector5D faceOffset;
		m_transformer.getFaceOffset( f, faceOffset );

		int axis1, axis2, axis3, axis4;
		faceOffset.getZeroAxes( axis1, axis2, axis3, axis4 );

		double increment = m_settings.getIncrementBetweenStickers();

		for( int i=0; i<num; i++ )
			for( int j=0; j<num; j++ )
				for( int k=0; k<num; k++ )
					for( int l=0; l<num; l++ )
					{
						SSticker sticker;
						sticker.m_color = f;

						// Here is the cubie this sticker belongs to.
						sticker.m_cubie.m_components[axis1] = -1 + i*increment;
						sticker.m_cubie.m_components[axis2] = -1 + j*increment;
						sticker.m_cubie.m_components[axis3] = -1 + k*increment;
						sticker.m_cubie.m_components[axis4] = -1 + l*increment;
						sticker.m_cubie += faceOffset;

						// The starting sticker offset is the face offset.
						sticker.m_offset = faceOffset;

						// Add this one in.
						m_stickers.push_back( sticker );
					}
	}
}

void 
CCube5D::renderCube() 
{
	// Get rotation info.
	int axis = -1;
	std::vector<double> values;
	getRotationInfo( axis, values );

	// Cycle through the stickers and draw them.
	for( uint i=0; i<m_stickers.size(); i++ )
	{
		// On a cubie to highlight?
		bool cubieToHighlight = false;
		if( m_cubiesToHighlight.size() )
		{
			for( uint j=0; j<m_cubiesToHighlight.size(); j++ )
			{
				if( m_stickers[i].m_cubie == m_cubiesToHighlight[j] )
				{
					cubieToHighlight = true;
					break;
				}
			}
		}

		bool forceVisible = false;
		if( cubieToHighlight )
			forceVisible = true;

		// Check sticker visibilities to see if we need to draw this one.
		// If we are not rotating, only the current visibility makes a difference.
		if( 0 == m_rotation )
		{
			// Force these to visible even if they are not on a visible face.
			if( m_settings.m_showAllCubieStickers && m_stickers[i].m_cubie == m_selectedSticker.m_cubie )
				forceVisible = true;

			if( ! forceVisible )
				if( ! m_stickers[i].m_currentVis )
					continue;	
		}
		else
		{
			if( !forceVisible )
			{
				// Invisible now and in the future?
				if( ! m_stickers[i].m_currentVis &&	! m_stickers[i].m_futureVis )
					continue;
			}
		}

		// Should we rotate this one?
		bool rotate = shouldWeRotate( m_stickers[i], axis, values );

		drawSticker( m_stickers[i], rotate, cubieToHighlight );
	}

	// Cycle through the faces and draw them.
	/*
	for( int f=0; f<10; f++ )
	{
		if( !getFaceVisible( f ) )
			continue;
		
		CVector5D faceOffset;
		m_transformer.getFaceOffset( f, faceOffset );

		// Get the zero axes.
		// These are the axes we need to draw the 4-cube on.
		int axis1, axis2, axis3, axis4;
		faceOffset.getZeroAxes( axis1, axis2, axis3, axis4 );

		int rAxis = -1;
		std::vector<double> dummy;
		getRotationInfo( rAxis, dummy );
		bool rotate = -1 == m_currentTwist.m_rotationFace || f == rAxis ;

		glColor4d( 1,1,1,1 );
		glLineWidth( 2.0 );
		draw4Cube( faceOffset*m_settings.m_faceOffset, 
			axis1, axis2, axis3, axis4, 
			3*m_settings.m_stickerOffset + m_settings.m_stickerSize / 2, rotate );
	}
	*/

	// Draw lines for click visualizations.
	if( READY_FOR_SECOND == m_clickState )
	{
		glColor4d( 1,1,1,1 );

		// Get the first clicked sticker 5-center.
		CVector5D tempCenter;
		getSticker5Position( m_firstClickedSticker, tempCenter );

		// Cycle through the goto stickers.
		for( uint i=0; i<m_gotoStickers.size(); i++ )
		{
			bool highlight = m_gotoStickers[i] == m_selectedGotoSticker;
			if( highlight )
				glLineWidth( 4.0 );
			else
				glLineWidth( 1.0 );

			// Our line points.
			CVector5D center1 = tempCenter;
			CVector5D center2;
			getSticker5Position( m_gotoStickers[i], center2 );

			glBegin( GL_LINE_STRIP );
				vertex( center1 );
				vertex( center2 );
			glEnd();
		}
	}
}

void 
CCube5D::vertex( CVector5D & p ) const
{
	p.projectTo4D( m_settings.m_projection5Distance );
	p.projectTo3D( m_settings.m_projection4Distance );
	glVertex3d( 
		(GLdouble)p.m_components[0], 
		(GLdouble)p.m_components[1], 
		(GLdouble)p.m_components[2] );
}

void 
CCube5D::drawSticker( const SSticker & sticker, bool rotate, bool cubieToHighlight ) const
{
	// Setup the color.
	double alpha = 1.0;
	if( rotate && 0 != m_rotation )
	{
		double start = sticker.m_currentVis ? 1.0 : 0.0;
		double end = sticker.m_futureVis ? 1.0 : 0.0;
		alpha = start + ( end - start ) * m_percentageDone;
	}

	// Force these to not fade.
	if( cubieToHighlight )
		alpha = 1.0;

	if( m_useStereoColor )
	{
		glColor4d( m_stereoColor.m_r, m_stereoColor.m_g, 
			m_stereoColor.m_b, alpha * m_stereoColor.m_a );
	}
	else
	{
		CColor color = m_colors[sticker.m_color];
		glColor4d( (GLdouble)color.m_r, (GLdouble)color.m_g, 
			(GLdouble)color.m_b, (GLdouble)alpha );
	}

	// Get the 5-center.
	CVector5D center;
	getSticker5Position( sticker, center );

	// Get the zero axes.
	// These are the axes we need to draw the 4-cube on.
	int axis1, axis2, axis3, axis4;
	sticker.m_offset.getZeroAxes( axis1, axis2, axis3, axis4 );

	// Handle highlighting.
	bool highlightHigh = false;
	EStickerAccent accent = NORMAL;	
	if( READY_FOR_SECOND == m_clickState )
	{
		// High highlight the first clicked sticker and the goto sticker.
		highlightHigh = sticker == m_firstClickedSticker || sticker == m_selectedGotoSticker;
		accent = getStickerAccent( sticker );
	}
	else
	{
		// High highlight the selected sticker.
		highlightHigh = m_settings.m_showAllCubieStickers ? 
			sticker.m_cubie == m_selectedSticker.m_cubie : 
			sticker == m_selectedSticker;
		accent = getStickerAccent( sticker );
	}

	// Handle found cubies.
	if( m_cubiesToHighlight.size() )
	{
		if( cubieToHighlight )
			highlightHigh = true;
		else
		{
			// Gray everything else!
			glColor4d( 0.5, 0.5, 0.5, alpha * 0.5 );
		}
	}

	if( highlightHigh )
		glLineWidth( 4.5 );
	else
	{
		switch( accent )
		{
		case DIM:
			glLineWidth( 0.5 );
			glColor4d( 0.5, 0.5, 0.5, alpha * 0.5 );
			break;

		case NORMAL:
			glLineWidth( 1.0 );
			break;

		case HIGHLIGHT:
			glLineWidth( 2.5 );
			break;

		default:
			assert( false );
		}
	}

	draw4Cube( center, axis1, axis2, axis3, axis4, m_settings.m_stickerSize, rotate );
}

void 
CCube5D::getSticker5Position( const SSticker & sticker, CVector5D & center ) const
{
	// Cubie center relative to face origin.
	center = sticker.m_cubie - sticker.m_offset;
	center *= m_settings.m_stickerOffset;

	// Offset to the face.
	center += sticker.m_offset * m_settings.m_faceOffset;
}

void 
CCube5D::getSticker3Position( const SSticker & sticker, CVector3D & position ) const
{
	// Get the 5-center.
	CVector5D center;
	getSticker5Position( sticker, center );

	// Now project these down to 3D.
	center.projectTo4D( m_settings.m_projection5Distance );
	center.projectTo3D( m_settings.m_projection4Distance );
	for( int i=0; i<3; i++ )
		position.m_components[i] = center.m_components[i];
}

void 
CCube5D::getSticker3Extents( const SSticker & sticker, CVector3D & low, CVector3D & high ) const
{
	// Get the 5-center.
	CVector5D center;
	getSticker5Position( sticker, center );

	// Get the zero axes.
	int axis1, axis2, axis3, axis4;
	sticker.m_offset.getZeroAxes( axis1, axis2, axis3, axis4 );

	// Get all the points for this 4-cube.
	CVector5D vertices[16];
	get4CubeVertices( center, axis1, axis2, axis3, axis4, m_settings.m_stickerSize, vertices );

	// Now project these all down to 3D.
	low = CVector3D( LARGE_DISTANCE, LARGE_DISTANCE, LARGE_DISTANCE );
	high = CVector3D( -LARGE_DISTANCE, -LARGE_DISTANCE, -LARGE_DISTANCE );
	for( int i=0; i<16; i++ )
	{
		vertices[i].projectTo4D( m_settings.m_projection5Distance );
		vertices[i].projectTo3D( m_settings.m_projection4Distance );

		for( int j=0; j<3; j++ )
		{
			if( low.m_components[j] > vertices[i].m_components[j] )
				low.m_components[j] = vertices[i].m_components[j];
			if( high.m_components[j] < vertices[i].m_components[j] )
				high.m_components[j] = vertices[i].m_components[j];
		}
	}
}

namespace
{
	void renderPoints( const CVector5D & p1, const CVector5D & p2, const CVector5D & p3, const CVector5D & p4 )
	{
		glBegin( GL_LINE_LOOP );

			glVertex3d( 
				(GLdouble)p1.m_components[0], 
				(GLdouble)p1.m_components[1], 
				(GLdouble)p1.m_components[2] );
			glVertex3d( 
				(GLdouble)p2.m_components[0], 
				(GLdouble)p2.m_components[1], 
				(GLdouble)p2.m_components[2] );
			glVertex3d( 
				(GLdouble)p3.m_components[0], 
				(GLdouble)p3.m_components[1], 
				(GLdouble)p3.m_components[2] );
			glVertex3d( 
				(GLdouble)p4.m_components[0], 
				(GLdouble)p4.m_components[1], 
				(GLdouble)p4.m_components[2] );

		glEnd();
	}
}

void 
CCube5D::draw4Cube( const CVector5D & offset, int axis1, int axis2, int axis3, int axis4, double scale, bool rotate ) const
{
	// For smooth rotation start/ends.
	const double rotation = deg2rad( 45*(sin(2*m_rotation - CONSTANT_PI/2) + 1) );

	// XXX - combine code with get4CubeVertices()??
	const double r = scale / 2;
	CVector5D vertices[4][4];
	double angle1 = deg2rad( 45.0 );
	for( int i=0; i<4; i++ )
	{
		double angle2 = deg2rad( 45.0 );
		for( int j=0; j<4; j++ )
		{
			CVector5D & point = vertices[i][j];
			point.m_components[axis1] = r * cos( angle1 );
			point.m_components[axis2] = r * sin( angle1 );
			point.m_components[axis3] = r * cos( angle2 );
			point.m_components[axis4] = r * sin( angle2 );
			point += offset;

			if( rotate )
				point.rotate( m_currentTwist.m_rotationAxis1, m_currentTwist.m_rotationAxis2, rotation );

			// Go ahead and project it down.
			point.projectTo4D( m_settings.m_projection5Distance );
			point.projectTo3D( m_settings.m_projection4Distance );

			angle2 += deg2rad( 90.0 );
		}
		angle1 += deg2rad( 90.0 );
	}

	// Now render them.
	for( int i=0; i<4; i++ )
	{
		renderPoints( vertices[i][0], vertices[i][1], vertices[i][2], vertices[i][3] );
		renderPoints( vertices[0][i], vertices[1][i], vertices[2][i], vertices[3][i] );
	}
}

void 
CCube5D::get4CubeVertices( const CVector5D & offset, int axis1, int axis2, int axis3, int axis4, double scale, CVector5D vertices[16] ) const
{
	// Do as a torus in 2 directions.
	const int numPoints = 4;
	const double r = scale / 2;

	// Draw one direction.
	double angle1 = deg2rad(45.0);
	for( int i=0; i<numPoints; i++ )
	{
		double angle2 = deg2rad(45.0);
		for( int j=0; j<numPoints; j++ )
		{
			CVector5D point;
			point.m_components[axis1] = r * cos( angle2 );
			point.m_components[axis2] = r * sin( angle2 );
			point.m_components[axis3] = r * cos( angle1 );
			point.m_components[axis4] = r * sin( angle1 );
			point += offset;
			vertices[i*4+j] = point;

			angle2 += deg2rad( 360.0 / numPoints );
		}
		angle1 += deg2rad( 360.0 / numPoints );
	}
}

void 
CCube5D::calculateVisibilities() 
{
	// Get rotation info.
	int axis = -1;
	std::vector<double> values;
	getRotationInfo( axis, values );

	// Cycle through the stickers.
	for( uint i=0; i<m_stickers.size(); i++ )
	{
		// The current visibility.
		CVector5D face = m_stickers[i].m_offset;
		int f1 = m_transformer.getFaceFromOffset( face );
		m_stickers[i].m_currentVis = getFaceVisible( f1 );
		
		// Should we rotate this one?
		bool rotate = shouldWeRotate( m_stickers[i], axis, values );
		if( rotate )
		{
			face.rotate( m_currentTwist.m_rotationAxis1, m_currentTwist.m_rotationAxis2, deg2rad( 90.0 ) );

			// Avoid rounding issues.
			roundComponents( face );

			int f2 = m_transformer.getFaceFromOffset( face );
			m_stickers[i].m_futureVis = getFaceVisible( f2 );
		}
		else
			m_stickers[i].m_futureVis = m_stickers[i].m_currentVis;
	}
}

void 
CCube5D::calculateNewStickerPositions() 
{
	// Get rotation info.
	int axis = -1;
	std::vector<double> values;
	getRotationInfo( axis, values );

	// Cycle through the stickers.
	for( uint i=0; i<m_stickers.size(); i++ )
	{
		// Should we rotate this one?
		bool rotate = shouldWeRotate( m_stickers[i], axis, values );
		if( ! rotate )
			continue;

		m_stickers[i].m_cubie.rotate( m_currentTwist.m_rotationAxis1, m_currentTwist.m_rotationAxis2, deg2rad( 90.0 ) );
		m_stickers[i].m_offset.rotate( m_currentTwist.m_rotationAxis1, m_currentTwist.m_rotationAxis2, deg2rad( 90.0 ) );

		// Avoid rounding issues.
		roundComponents( m_stickers[i].m_cubie );
		roundComponents( m_stickers[i].m_offset );

		// Our future visibility is now our current visibility.
		m_stickers[i].m_currentVis = m_stickers[i].m_futureVis;
	}

	// Cycle through the cubies to highlight.
	for( uint i=0; i<m_cubiesToHighlight.size(); i++ )
	{
		// Should we rotate this one?
		bool rotate = shouldWeRotate( m_cubiesToHighlight[i], axis, values );
		if( ! rotate )
			continue;

		m_cubiesToHighlight[i].rotate( m_currentTwist.m_rotationAxis1, m_currentTwist.m_rotationAxis2, deg2rad( 90.0 ) );

		// Avoid rounding issues.
		roundComponents( m_cubiesToHighlight[i] );
	}
}

void 
CCube5D::roundComponents( CVector5D & vector ) const
{
	// Our max component values are 1 or -1 no matter what the puzzle size, 
	// so component values are quantized to certain decimals depending on the puzzle size.
	double increment = m_settings.getIncrementBetweenStickers();

	// Cycle through the components.
	for( int j=0; j<5; j++ )
	{
		double & component = vector.m_components[j];

		// Zero components are ok.
		// We could tighten the tolerance, but that wouldn't be 
		// necessary unless m_n > about 20, which is unrealistic anyway.
		static double tolerance = .1;
		if( component > -.1 && component < .1 )
		{
			component = 0;
			continue;
		}

		// Otherwise, components should be quantized to allowed values.
		double start = -1 - increment/2;
		double end = start + increment;
		for( int i=0; i<m_settings.m_n; i++ )
		{
			if( component > start &&
				component <= end )
			{
				component = -1 + i*increment;
				break;
			}

			start = end;
			end = start + increment;
		}
	}
}

void 
CCube5D::getRotationInfo( int & axis, std::vector<double> & values ) const
{
	// Are we just going to rotate everything?
	if( -1 == m_currentTwist.m_rotationFace )
		return;

	// Get the unit valued axis of the rotation face.
	CVector5D rotationFace;
	m_transformer.getFaceOffset( m_currentTwist.m_rotationFace, rotationFace );
	rotationFace.getUnitValuedAxis( axis );

	// Get the values on this component axis a sticker can have to be rotated.
	// This code special-cases the common case where the slicemask is 1.
	double rotationFaceComponent = rotationFace.m_components[axis];
	if( 1 != m_currentTwist.m_slicemask )
	{
		bool add = -1 == rotationFaceComponent;

		// This is the increment between stickers.
		double increment = m_settings.getIncrementBetweenStickers();

		// Check the slicemasks.
#define CHECK_SLICEMASK( SLICEMASK, i )											\
		if( SLICEMASK & m_currentTwist.m_slicemask )							\
		{																		\
			if( add )															\
				values.push_back( rotationFaceComponent + (i-1)*increment );	\
			else																\
				values.push_back( rotationFaceComponent - (i-1)*increment );	\
		}

		CHECK_SLICEMASK( SLICEMASK_1, 1 )
		CHECK_SLICEMASK( SLICEMASK_2, 2 )
		CHECK_SLICEMASK( SLICEMASK_3, 3 )
		CHECK_SLICEMASK( SLICEMASK_4, 4 )
		CHECK_SLICEMASK( SLICEMASK_5, 5 )
		CHECK_SLICEMASK( SLICEMASK_6, 6 )
		CHECK_SLICEMASK( SLICEMASK_7, 7 )
#undef CHECK_SLICEMASK

	}
	else
	{
		values.push_back( rotationFaceComponent );
	}
}

bool 
CCube5D::shouldWeRotate( const SSticker & sticker, int axis, const std::vector<double> & values ) const
{
	return shouldWeRotate( sticker.m_cubie, axis, values );
}

bool 
CCube5D::shouldWeRotate( const CVector5D & cubie, int axis, const std::vector<double> & values ) const
{
	// View rotation?
	if( -1 == m_currentTwist.m_rotationFace )
		return true;

	// Sticker part of the twist?
	for( uint i=0; i<values.size(); i++ )
	{
		// This equality check requires a tolerance.
		if( IS_ZERO( values[i] - cubie.m_components[axis] ) )
			return true;
	}

	return false;
}

EStickerAccent 
CCube5D::getStickerAccent( const SSticker & sticker ) const
{
	int numColors = (int)sticker.getNumColors();
	switch( numColors )
	{
	case 1:
		return m_settings.m_accent1Colored;
	case 2:
		return m_settings.m_accent2Colored;
	case 3:
		return m_settings.m_accent3Colored;
	case 4:
		return m_settings.m_accent4Colored;
	case 5:
		return m_settings.m_accent5Colored;
	default:
		assert( false );
	}

	return NORMAL;
}

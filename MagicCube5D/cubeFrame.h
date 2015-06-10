
#pragma once

#include "workFiles/renderer.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;

namespace MagicCube5D
{

	public ref class CubeFrame : ICube
	{
	public:

		CubeFrame();
		~CubeFrame();
		!CubeFrame();

		// run the application
		void Run();

	protected:

		// event handers
		void FrameLoad( Object^ sender, EventArgs^ args );
		void Paint( Object^ sender, System::Windows::Forms::PaintEventArgs^ args );
		void TimerTick( System::Object^ sender, System::EventArgs^ e );
		void MouseDown( Object^ sender, System::Windows::Forms::MouseEventArgs^ args );
		void MouseMove( Object^ sender, System::Windows::Forms::MouseEventArgs^ args );
		void MouseUp( Object^ sender, System::Windows::Forms::MouseEventArgs^ args );
		void KeyDown( Object^ sender, System::Windows::Forms::KeyEventArgs^ args );
		void KeyUp( Object^ sender, System::Windows::Forms::KeyEventArgs^ args );
		void SliceDown( Object^ sender, System::Windows::Forms::KeyEventArgs^ args );
		void SliceUp( Object^ sender, System::Windows::Forms::KeyEventArgs^ args );

		// drag helpers
		void StartDrag();
		void PerformDrag( int x, int y, MouseButtons btn );
		void FinishDrag();

		// paint helpers
		void Redraw( bool force );
		void DoRender( System::Drawing::Graphics^ g );
		void DoRender();

		// keyboard helpers
		static bool ShiftDown();
		static bool CtrlDown();

		// Helper to select a sticker.
		void internalSelectSticker();

	public:

		// ICube methods
		virtual void SetParameter( CubeParameter param, int value );
		virtual void EnableFace( int face, bool enable );
		virtual void ColorFace( int face, System::Drawing::Color c );
		virtual void Rotate( int a1, int a2, int face, int slicemask );
		virtual property bool EnableRedraw
		{
			bool get();
			void set( bool );
		}
		virtual void CycleStickerAccent( int colors );
		virtual void Save( bool saveas );
		virtual void Load();
		virtual void Scramble( int number );
		virtual void Solve();
		virtual void Undo();
		virtual void Redo();
		virtual void Reset();
		virtual void HighlightCubies( List<int> ^ colors );
		virtual array<String ^> ^ GetMacroNames(); 
		virtual void StartMacroRecord();
		virtual void StopMacroRecord();
		virtual void RenameMacro( int index, String ^ name );
		virtual void DeleteMacro( int index );
		virtual void ExecuteMacro( int index, bool reverse );

	private:

		// Internal rotation method
		// NOTE: This will always rotate, even if the puzzle is in the animating state.
		//		 You'll want to consider using the ICube rotate method instead.
		void internalRotate( const STwist & twist );

		// Show a congratulatory solved message.
		void showSolvedMessage();

	private:

		// The main frame of our UI
		MainFrame^ m_frame;

		// Additonal managed component bits.
		System::ComponentModel::IContainer^ m_components;
		Timer^	m_timer;

		// The cube renderer.
		CRenderer* m_renderer;
		bool	m_enableRedraw;

		// Whether or not we are solving.
		bool	m_solving;

		// Rotation stuff.
		double	m_currentAngle;

		// Animating?
		bool	m_animating;

		// Mouse moving.
		int		m_mouseDownX;
		int		m_mouseDownY;
		bool	m_dragging;
		int		m_mouseLastX;
		int		m_mouseLastY;

		// Our current slicemask;
		int		m_slicemask;
	};

}

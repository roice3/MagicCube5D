// {{{ClassName=CubeFrame

#include "cubeFrame.h"

#include <stdafx.h>

#include "workFiles/glInit.h"
#include "workFiles/helper.h"
#include "workFiles/selector.h"

using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace MagicCube5D {
CubeFrame::CubeFrame() {
    m_solving = false;
    m_enableRedraw = true;
    m_animating = false;

    m_dragging = false;
    m_mouseDownX = m_mouseDownY = -1;
    m_mouseLastX = m_mouseLastY = 0;

    m_slicemask = 0;
}

CubeFrame::~CubeFrame() {}

CubeFrame::!CubeFrame() {
    if (m_components) delete m_components;

    delete (m_renderer);
}

namespace {
void updateSolvedInfo(CCube5D& cube, MainFrame ^ frame) {
    SSolvedInfo si = cube.updateSolvedInfo();
    frame->updateProgressPage(si.s0, si.n0, si.s1, si.n1, si.s2, si.n2, si.s3, si.n3,
                              si.s4, si.n4, si.s5, si.n5, si.ss, si.ns);
}
}  // namespace

void CubeFrame::Run() {
    m_frame = gcnew MainFrame(this);

    m_components = gcnew System::ComponentModel::Container();
    m_timer = gcnew Timer(m_components);
    m_timer->Interval = 20;
    m_timer->Tick += gcnew EventHandler(this, &CubeFrame::TimerTick);

    m_frame->Load += gcnew EventHandler(this, &CubeFrame::FrameLoad);

    m_frame->DrawSurface->Paint += gcnew PaintEventHandler(this, &CubeFrame::Paint);
    m_frame->DrawSurface->KeyDown += gcnew KeyEventHandler(this, &CubeFrame::KeyDown);
    m_frame->DrawSurface->KeyUp += gcnew KeyEventHandler(this, &CubeFrame::KeyUp);
    m_frame->KeyDown += gcnew KeyEventHandler(this, &CubeFrame::SliceDown);
    m_frame->KeyUp += gcnew KeyEventHandler(this, &CubeFrame::SliceUp);
    m_frame->DrawSurface->MouseDown +=
            gcnew MouseEventHandler(this, &CubeFrame::MouseDown);
    m_frame->DrawSurface->MouseMove +=
            gcnew MouseEventHandler(this, &CubeFrame::MouseMove);
    m_frame->DrawSurface->MouseUp += gcnew MouseEventHandler(this, &CubeFrame::MouseUp);

    m_renderer = new CRenderer();

    updateSolvedInfo(m_renderer->m_cube, m_frame);

    Application::Run(m_frame);
}

void CubeFrame::FrameLoad(Object ^ sender, EventArgs ^ args) {
    CGlInit glInit;
    glInit.initializeGL((HWND)m_frame->DrawSurface->Handle.ToInt32());
}

void CubeFrame::Paint(Object ^ sender, System::Windows::Forms::PaintEventArgs ^ args) {
    DoRender(args->Graphics);
}

void CubeFrame::TimerTick(System::Object ^ sender, System::EventArgs ^ e) {
    if (m_currentAngle + m_renderer->m_cube.m_settings.m_rotationStep >= 90) {
        m_renderer->m_cube.finishRotate();
        updateSolvedInfo(m_renderer->m_cube, m_frame);
        bool showMessage = false;

        // Special handling if we are solving.
        if (m_solving) {

            // Start the next undo.
            STwist undo;
            if (m_renderer->m_cube.getUndoParams(undo)) {
                internalRotate(undo);
                return;
            } else {
                m_solving = false;
            }
        } else {
            showMessage = m_renderer->m_cube.showSolvedMessage();
        }

        m_timer->Enabled = false;
        m_animating = false;

        // We need to select a sticker.
        internalSelectSticker();

        Redraw(false);

        // Show a congratulatory message if they've solved it.
        if (showMessage) showSolvedMessage();

        return;
    }

    // Do the drawing.
    m_renderer->iterationMade(false);
    DoRender();

    m_currentAngle += m_renderer->m_cube.m_settings.m_rotationStep;
}

void CubeFrame::MouseMove(System::Object ^ sender,
                          System::Windows::Forms::MouseEventArgs ^ e) {

    // Make sure we have the focus.
    m_frame->DrawSurface->Select();

    // Are we starting a drag?
    // NOTE: The mousedown checks makes sure we had a mouse down call and fixes a
    // problem I was seeing
    //		 where the view would reset when you loaded in a log file.
    if (!m_dragging && e->Button != MouseButtons::None && -1 != m_mouseDownX &&
        -1 != m_mouseDownY &&
        ((Math::Abs(e->X - m_mouseDownX) > SystemInformation::DragSize.Width / 2) ||
         (Math::Abs(e->Y - m_mouseDownY) > SystemInformation::DragSize.Height / 2))) {
        StartDrag();

        // Fake the original mouse position so we will get some drag motion immediately.
        m_mouseLastX = m_mouseDownX;
        m_mouseLastY = m_mouseDownY;
    }

    // Are we dragging?
    if (m_dragging) PerformDrag(e->X, e->Y, e->Button);

    m_mouseLastX = e->X;
    m_mouseLastY = e->Y;

    // We don't need to continue past here if we are animating.
    if (m_animating) return;

    // Select a sticker based on the new mouse position.
    internalSelectSticker();

    Redraw(true);
}

void CubeFrame::StartDrag() {
    m_dragging = true;
    m_frame->DrawSurface->Capture = true;
}

void CubeFrame::PerformDrag(int x, int y, MouseButtons btn) {

    // This is increment we moved, scaled to the window size.
    float xIncrement = (float)(x - m_mouseLastX) /
                       (float)m_frame->DrawSurface->ClientRectangle.Width;
    float yIncrement = (float)(y - m_mouseLastY) /
                       (float)m_frame->DrawSurface->ClientRectangle.Height;

    // A measure of the magnitude of the change.
    float magnitude =
            (float)(sqrt(xIncrement * xIncrement + yIncrement * yIncrement) * 100);

    if (btn == System::Windows::Forms::MouseButtons::Left) {

        // The spherical coordinate radius.
        CVector3D viewVector = m_renderer->m_viewLookfrom - m_renderer->m_viewLookat;
        double radius = viewVector.abs();
        if (!IS_ZERO(radius)) {

            // The spherical coordinate angles.
            double theta =
                    atan2(viewVector.m_components[1], viewVector.m_components[0]);
            double phi = acos(viewVector.m_components[2] / radius);

            // Increment the angles.
            theta -= magnitude * xIncrement;
            phi -= magnitude * yIncrement;

            // Check the bounds.
            // So we don't have to worry about the upVector, don't allow these to be
            // exactly 0 or Pi.
            if (phi <= 0) phi = 0.0000001;
            if (phi >= CONSTANT_PI) phi = CONSTANT_PI - 0.0000001;

            // Calculate the new position.
            m_renderer->m_viewLookfrom.m_components[0] =
                    m_renderer->m_viewLookat.m_components[0] +
                    radius * sin(phi) * cos(theta);
            m_renderer->m_viewLookfrom.m_components[1] =
                    m_renderer->m_viewLookat.m_components[1] +
                    radius * sin(phi) * sin(theta);
            m_renderer->m_viewLookfrom.m_components[2] =
                    m_renderer->m_viewLookat.m_components[2] + radius * cos(phi);
        }
    }

    if (btn == System::Windows::Forms::MouseButtons::Right) {

        // The view vector magnitude.
        CVector3D viewVector = m_renderer->m_viewLookfrom - m_renderer->m_viewLookat;
        double abs = viewVector.abs();

        // Increment it.
        abs += 5 * abs * yIncrement;
        viewVector.normalize();
        viewVector *= abs;

        double smallestRadius = .02;
        if (viewVector.abs() < smallestRadius) {
            viewVector.normalize();
            viewVector *= smallestRadius;
        }

        // Set the new position.
        m_renderer->m_viewLookfrom = viewVector;
    }
}

void CubeFrame::FinishDrag() {
    m_frame->DrawSurface->Capture = false;
    m_dragging = false;
}

void CubeFrame::MouseDown(System::Object ^ sender,
                          System::Windows::Forms::MouseEventArgs ^ e) {
    m_mouseDownX = e->X;
    m_mouseDownY = e->Y;
}

void CubeFrame::MouseUp(System::Object ^ sender,
                        System::Windows::Forms::MouseEventArgs ^ e) {
    m_mouseDownX = m_mouseDownY = -1;

    // Figure out if we were dragging, and if the drag is done.
    if (m_dragging) {
        if (Form::MouseButtons == MouseButtons::None) FinishDrag();
        return;
    }

    if (m_animating) return;

    // Past here, the mouse-up represents a click.

    // Left button click.
    if (e->Button == System::Windows::Forms::MouseButtons::Left) {
        bool rotateStarted = false;
        if (CCube5D::READY_FOR_FIRST == m_renderer->m_cube.m_clickState)
            rotateStarted = m_renderer->m_cube.handleFirstClick(CtrlDown());
        else
            rotateStarted = m_renderer->m_cube.handleSecondClick(
                    CtrlDown(), 0 == m_slicemask ? 1 : m_slicemask);
        if (rotateStarted) {
            m_currentAngle = 0;
            m_animating = true;
            m_timer->Enabled = true;
        }
    }
}

void CubeFrame::KeyDown(System::Object ^ sender,
                        System::Windows::Forms::KeyEventArgs ^ e) {
    if (m_animating) return;

    // Reset click handling.
    if (e->KeyCode == Keys::Escape) m_renderer->m_cube.resetClickState();

    // Cycle through click stickers.
    if (e->KeyCode == Keys::Left) m_renderer->m_cube.cycleSelectedGotoSticker(true);
    if (e->KeyCode == Keys::Right) m_renderer->m_cube.cycleSelectedGotoSticker(false);

    // Shift
    if (e->KeyCode == Keys::ShiftKey) {

        // We need to select a sticker.
        internalSelectSticker();
    }

    Redraw(true);
}

void CubeFrame::KeyUp(System::Object ^ sender,
                      System::Windows::Forms::KeyEventArgs ^ e) {
    if (m_animating) return;

    // Shift
    if (e->KeyCode == Keys::ShiftKey) {

        // We need to select a sticker.
        internalSelectSticker();
    }

    Redraw(true);
}

void CubeFrame::SliceDown(System::Object ^ sender,
                          System::Windows::Forms::KeyEventArgs ^ e) {

    // Update our slicemask;
    if (e->KeyCode == Keys::NumPad1 || e->KeyCode == Keys::D1)
        m_slicemask |= SLICEMASK_1;
    if (e->KeyCode == Keys::NumPad2 || e->KeyCode == Keys::D2)
        m_slicemask |= SLICEMASK_2;
    if (e->KeyCode == Keys::NumPad3 || e->KeyCode == Keys::D3)
        m_slicemask |= SLICEMASK_3;
    if (e->KeyCode == Keys::NumPad4 || e->KeyCode == Keys::D4)
        m_slicemask |= SLICEMASK_4;
    if (e->KeyCode == Keys::NumPad5 || e->KeyCode == Keys::D5)
        m_slicemask |= SLICEMASK_5;
    if (e->KeyCode == Keys::NumPad6 || e->KeyCode == Keys::D6)
        m_slicemask |= SLICEMASK_6;
    if (e->KeyCode == Keys::NumPad7 || e->KeyCode == Keys::D7)
        m_slicemask |= SLICEMASK_7;
}

void CubeFrame::SliceUp(System::Object ^ sender,
                        System::Windows::Forms::KeyEventArgs ^ e) {

    // Update our slicemask;
    if (e->KeyCode == Keys::NumPad1 || e->KeyCode == Keys::D1)
        m_slicemask &= ~SLICEMASK_1;
    if (e->KeyCode == Keys::NumPad2 || e->KeyCode == Keys::D2)
        m_slicemask &= ~SLICEMASK_2;
    if (e->KeyCode == Keys::NumPad3 || e->KeyCode == Keys::D3)
        m_slicemask &= ~SLICEMASK_3;
    if (e->KeyCode == Keys::NumPad4 || e->KeyCode == Keys::D4)
        m_slicemask &= ~SLICEMASK_4;
    if (e->KeyCode == Keys::NumPad5 || e->KeyCode == Keys::D5)
        m_slicemask &= ~SLICEMASK_5;
    if (e->KeyCode == Keys::NumPad6 || e->KeyCode == Keys::D6)
        m_slicemask &= ~SLICEMASK_6;
    if (e->KeyCode == Keys::NumPad7 || e->KeyCode == Keys::D7)
        m_slicemask &= ~SLICEMASK_7;
}

void CubeFrame::SetParameter(CubeParameter param, int value) {
    switch (param) {
        case CubeParameter::Puzzle:
            m_renderer->m_cube.setPuzzleType((int)value);
            updateSolvedInfo(m_renderer->m_cube, m_frame);
            break;
        case CubeParameter::Perspective5d:
            m_renderer->m_cube.m_settings.m_projection5Distance = (double)value;
            break;
        case CubeParameter::Perspective4d:
            m_renderer->m_cube.m_settings.m_projection4Distance = (double)value;
            break;
        case CubeParameter::FaceSeparation:
            m_renderer->m_cube.m_settings.m_faceOffset = (double)value / 5;
            break;
        case CubeParameter::StickerSeparation:
            m_renderer->m_cube.m_settings.m_stickerOffset = (double)value / 10;
            break;
        case CubeParameter::StickerSize:
            m_renderer->m_cube.m_settings.m_stickerSize = (double)value / 25;
            break;
        case CubeParameter::RotationRate:
            m_renderer->m_cube.m_settings.m_rotationStep = 0.1 + (double)value / 10;
            break;
        case CubeParameter::StereoMode: {
            m_renderer->m_cube.m_settings.m_stereoMode = (EStereoMode)value;

            // Alter the viewing distance depending on the mode.
            CVector3D viewVector = m_renderer->m_viewLookfrom;
            viewVector.normalize();
            if (STEREO_CROSS_EYED == m_renderer->m_cube.m_settings.m_stereoMode)
                viewVector *= 170;
            else
                viewVector *= 85;
            m_renderer->m_viewLookfrom = viewVector;

            break;
        }
        case CubeParameter::StereoSeparation:
            m_renderer->m_cube.m_settings.m_stereoSeparation = (double)value / 20;
            break;
    }

    Redraw(true);
}

bool CubeFrame::EnableRedraw::get() { return m_enableRedraw; }

void CubeFrame::EnableRedraw::set(bool val) {
    m_enableRedraw = val;
    if (val) Redraw(false);
}

void CubeFrame::CycleStickerAccent(int colors) {
    switch (colors) {
        case 1:
            m_renderer->m_cube.m_settings.cycleAccentSetting(
                    m_renderer->m_cube.m_settings.m_accent1Colored);
            break;
        case 2:
            m_renderer->m_cube.m_settings.cycleAccentSetting(
                    m_renderer->m_cube.m_settings.m_accent2Colored);
            break;
        case 3:
            m_renderer->m_cube.m_settings.cycleAccentSetting(
                    m_renderer->m_cube.m_settings.m_accent3Colored);
            break;
        case 4:
            m_renderer->m_cube.m_settings.cycleAccentSetting(
                    m_renderer->m_cube.m_settings.m_accent4Colored);
            break;
        case 5:
            m_renderer->m_cube.m_settings.cycleAccentSetting(
                    m_renderer->m_cube.m_settings.m_accent5Colored);
            break;
    }

    Redraw(true);
}

void CubeFrame::EnableFace(int face, bool enable) {
    m_renderer->m_cube.setFaceVisible(face, enable);
    Redraw(true);
}

void CubeFrame::ColorFace(int face, System::Drawing::Color c) {

    // Setting background color?
    CColor color((float)c.R / 255, (float)c.G / 255, (float)c.B / 255,
                 (float)c.A / 255);
    if (10 == face)
        m_renderer->setBackgroundColor(color);
    else
        m_renderer->m_cube.setFaceColor(face, color);
}

void CubeFrame::Rotate(int a1, int a2, int face, int dummy) {
    if (m_animating) return;

    STwist twist;
    twist.m_rotationAxis1 = a1;
    twist.m_rotationAxis2 = a2;
    twist.m_rotationFace = face;
    twist.m_slicemask = 0 == m_slicemask ? 1 : m_slicemask;
    internalRotate(twist);
}

void CubeFrame::internalRotate(const STwist& twist) {
    m_currentAngle = 0;
    m_renderer->m_cube.startRotate(twist);
    m_animating = true;
    m_timer->Enabled = true;
}

void CubeFrame::showSolvedMessage() {
    String ^ text =
            "Congratulations!  You have achieved ultimate super-nerd status.\n\n"
            "Take a few moments and space-out on the ridiculousness of what you have "
            "just accomplished, then "
            "send your logfile to roice@gravitation3d.com to get your name listed in "
            "the MagicCube5D Hall of Insanity.";
    String ^ caption = "MagicCube5D Conquered!";
    MessageBox::Show(text, caption, MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
}

void CubeFrame::Save(bool saveas) {
    if (m_animating) return;

    m_renderer->m_cube.save(saveas);
}

void CubeFrame::Load() {
    if (m_animating) return;

    m_renderer->m_cube.load();
    updateSolvedInfo(m_renderer->m_cube, m_frame);
    Redraw(true);
}

void CubeFrame::Scramble(int number) {
    if (m_animating) return;

    m_renderer->m_cube.scramble(number);
    updateSolvedInfo(m_renderer->m_cube, m_frame);
    Redraw(true);
}

void CubeFrame::Solve() {
    if (m_animating) return;

    // Start the first undo.
    STwist undo;
    if (m_renderer->m_cube.getUndoParams(undo)) {
        m_solving = true;
        internalRotate(undo);
    }
}

void CubeFrame::Undo() {
    if (m_animating) return;

    STwist undo;
    if (m_renderer->m_cube.getUndoParams(undo)) internalRotate(undo);
}

void CubeFrame::Redo() {
    if (m_animating) return;

    STwist redo;
    if (m_renderer->m_cube.getRedoParams(redo)) internalRotate(redo);
}

void CubeFrame::Reset() {
    if (m_animating) return;

    m_renderer->m_cube.reset();
    updateSolvedInfo(m_renderer->m_cube, m_frame);
}

void CubeFrame::HighlightCubies(List<int> ^ colors) {
    std::vector<int> tempColors;
    for (int i = 0; i < colors->Count; i++) tempColors.push_back(colors[i]);

    m_renderer->m_cube.highlightCubies(tempColors);
}

array<String ^> ^ CubeFrame::GetMacroNames() {
    std::vector<std::string> macroNames;
    m_renderer->m_cube.getMacroNames(macroNames);
    array<String ^> ^ names = gcnew array<String ^>(macroNames.size());
    for (uint i = 0; i < macroNames.size(); i++)
        names[i] = gcnew String(macroNames[i].c_str());
    return names;
}

// XXX - make these return a bool so we are sure to stay synced with macro file.
//		 e.g. changing a name could update the UI but not the macro file as it is
//now.
void CubeFrame::StartMacroRecord() {
    if (m_animating) return;

    m_renderer->m_cube.startMacroRecord();
}

void CubeFrame::StopMacroRecord() {
    if (m_animating) return;

    m_renderer->m_cube.stopMacroRecord();
}

void CubeFrame::RenameMacro(int index, String ^ name) {
    if (m_animating) return;

    std::string newName = systemStringToStdString(name);
    m_renderer->m_cube.renameMacro(index, newName);
}

void CubeFrame::DeleteMacro(int index) {
    if (m_animating) return;

    m_renderer->m_cube.deleteMacro(index);
}

void CubeFrame::ExecuteMacro(int index, bool reverse) {
    if (m_animating) return;

    m_renderer->m_cube.executeMacro(index, reverse);
    Redraw(true);

    updateSolvedInfo(m_renderer->m_cube, m_frame);

    // Show a congratulatory message if they've solved it.
    if (m_renderer->m_cube.showSolvedMessage()) showSolvedMessage();
}

void CubeFrame::Redraw(bool force) {
    if (force || m_enableRedraw) m_frame->DrawSurface->Invalidate();
}

void CubeFrame::DoRender() {
    Graphics ^ g = Graphics::FromHwnd(m_frame->DrawSurface->Handle);
    DoRender(g);
    g->~Graphics();
}

void CubeFrame::DoRender(Graphics ^ g) {
    static int foo = 0;

    m_renderer->m_cube.m_settings.m_showAllCubieStickers = ShiftDown();
    m_renderer->renderScene(m_frame->DrawSurface->ClientRectangle.Width,
                            m_frame->DrawSurface->ClientRectangle.Height);

    IntPtr hdc = g->GetHdc();
    SwapBuffers((HDC)hdc.ToPointer());
    g->ReleaseHdc(hdc);
}

bool CubeFrame::ShiftDown() {
    return (Form::ModifierKeys & Keys::Shift) == Keys::Shift;
}

bool CubeFrame::CtrlDown() {
    return (Form::ModifierKeys & Keys::Control) == Keys::Control;
}

void CubeFrame::internalSelectSticker() {
    CVector3D linep1, linep2;
    CSelector::unProjectMousePoint(m_mouseLastX, m_mouseLastY, linep1, linep2);
    m_renderer->m_cube.selectSticker(linep1, linep2, ShiftDown());
}
}  // namespace MagicCube5D

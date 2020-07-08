#pragma once
#pragma managed(push, off)

#include "helper.h"
#include "loader.h"

class CCube5D {
public:
    CCube5D();

    // Main rendering control.
    void render(const CVector3D& lookFrom);

    //
    // Settings and related methods.
    //

    // All of our editable settings (public for easy access).
    CSettings m_settings;

    // Set the puzzle type.
    void setPuzzleType(int n);

    // Set which faces are visible.
    void setFaceVisible(int i, bool visible);
    bool getFaceVisible(int i) const;

    // Set the face colors.
    void setFaceColor(int i, CColor c);
    CColor getFaceColor(int i);

    //
    // Methods to control twists/rotations.
    //

    // Twist control.
    void startRotate(const STwist& twist);
    void iterateRotate();
    void finishRotate();

    // Get undo rotation parameters.
    // Calling this will set us in the "undo" state for the next rotation.
    // Returns false if there are no more twists to undo.
    bool getUndoParams(STwist& twist);

    // Get redo rotation parameters.
    // Calling this will set us in the "redo" state for the next rotation.
    // Returns false if there are no more twists to redo.
    bool getRedoParams(STwist& twist);

    //
    // Macro related methods.
    //

    // NOTE: These methods will load/save to the macro file as needed.
    void getMacroNames(std::vector<std::string>& names);
    void startMacroRecord();
    void stopMacroRecord();
    void renameMacro(int index, std::string name);
    void deleteMacro(int index);
    void executeMacro(int index, bool reverse);

    //
    // Click handling methods.
    //

    enum EClickState { READY_FOR_FIRST, READY_FOR_SECOND };
    EClickState m_clickState;

    // Reset our click state.
    void resetClickState();

    // The next 3 methods will return true if a twist needs to be initiated.
    // (They will also already call startRotate appropriately so we don't
    //  have to pass rotation information back out).
    bool handleFirstClick(bool control);
    bool handleSecondClick(bool control, int slicemask);

    // This will do a 5D view rotation.
    bool handleControlClick();

    // This will cycle the selected goto sticker.
    void cycleSelectedGotoSticker(bool left);

private:

    // Our first clicked sticker.
    SSticker m_firstClickedSticker;

    // The possible second click stickers.
    std::vector<SSticker> m_gotoStickers;

    // The selected goto sticker.
    SSticker m_selectedGotoSticker;

    // Fill possible second click sticker array.
    void getGotoStickers();

public:

    //
    // Other methods.
    //

    // Save/load.
    void save(bool saveas);
    void load();

    // Select a sticker based on a line.
    void selectSticker(const CVector3D& linep1, const CVector3D& linep2, bool shift);

    // Scramble the puzzle.
    void scramble(int numTwists);

    // Reset the puzzle.
    void reset();

    // Highlights cubies with a given set of colors.
    void highlightCubies(std::vector<int>& colors);

    // Are we solved?
    bool isSolved() const;

    // Show a solved message?  This is a heuristic method.
    bool showSolvedMessage();

    // Update our solved info.
    SSolvedInfo updateSolvedInfo();

private:

    // Create our stickers.
    void createStickers();

    // Main rendering method.
    void renderCube();

    // Rendering helpers.
    void vertex(CVector5D& p) const;

    // Methods to draw a 4-cube and a 4-face.
    void drawSticker(const SSticker& sticker, bool rotate, bool cubieToHighlight) const;
    void draw4Cube(const CVector5D& offset, int axis1, int axis2, int axis3, int axis4,
                   double scale = 1.0, bool rotate = true) const;

    // Get the vertices of a 4-cube.
    void get4CubeVertices(const CVector5D& offset, int axis1, int axis2, int axis3,
                          int axis4, double scale, CVector5D vertices[16]) const;

    // Get a sticker's 5D projected center.
    // This is not the normalized center (it takes into account all the projection
    // settings).
    void getSticker5Position(const SSticker& sticker, CVector5D& position) const;

    // Get a sticker's 3D projected information.
    void getSticker3Position(const SSticker& sticker, CVector3D& position) const;
    void getSticker3Extents(const SSticker& sticker, CVector3D& low,
                            CVector3D& high) const;

    // Calculate sticker visibilities.
    void calculateVisibilities();

    // Calculate new sticker positions.
    void calculateNewStickerPositions();

    // Helper to avoid rounding issues.
    void roundComponents(CVector5D& vector) const;

    // Twisting helpers.
    // getRotationInfo finds the axis and the values on that axis a sticker can have to
    // be rotated.
    void getRotationInfo(int& axis, std::vector<double>& values) const;
    bool shouldWeRotate(const SSticker& sticker, int axis,
                        const std::vector<double>& values) const;
    bool shouldWeRotate(const CVector5D& cubie, int axis,
                        const std::vector<double>& values) const;

    // Internal sticker selection helper.
    void selectStickerHelper(const CVector3D& linep1, const CVector3D& linep2,
                             bool shift, const std::vector<SSticker>& stickers,
                             bool mustIntersect, SSticker& selectedSticker);

    // Helper to see if a sticker should be accented.
    EStickerAccent getStickerAccent(const SSticker& sticker) const;

private:

    // Rotation variables.
    double m_rotation;
    double m_percentageDone;

    // Our stickers.
    std::vector<SSticker> m_stickers;

    // The colors for each sticker.
    std::vector<CColor> m_colors;

    // Our twist history.
    std::vector<STwist> m_twists;
    STwist m_currentTwist;
    std::vector<STwist> m_redoTwists;

    // Selected sticker.
    SSticker m_selectedSticker;

    // A loader.
    CLoader m_loader;

    // A state transformer.
    CStateTransformer m_transformer;

    // Whether we are in undo/redo modes.
    bool m_undoMode, m_redoMode;

    // Whether or not we have shown the solved message already.
    bool m_solvedMessageShown;

    // Our macros.
    std::vector<SMacro> m_macros;

    // Variables for the currently recording macro.
    SMacro m_recordingMacro;
    bool m_currentlyRecording;

    // Stereo properties.
    bool m_useStereoColor;
    CColor m_stereoColor;

    // Cubies to highlight.
    std::vector<CVector5D> m_cubiesToHighlight;

    // Solved info.
    SSolvedInfo m_solvedInfo;

    // Whether or not a full scramble occurred.
    bool m_scrambled;
};

#pragma managed(pop)

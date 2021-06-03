#pragma once
#pragma managed(push, off)

#include "settings.h"
#include "sticker.h"

//
// Our puzzle state is really just the set of stickers in the cube class.
// However, this can be represented much more compactly as a matrix of integers,
// which is nice for more compact log files for instance.
// This class will do the conversion between these 2 representations of the state.
//
// It also has useful helpers for converting specific items between integer/verbose
// representations.
//

// This matrix will hold a representation of the puzzle state.
// The outer vector cycles through faces.
// The inner vector cycles through stickers on those faces.
// The matrix integers represent the sticker colors.
typedef std::vector<std::vector<int>> StateMatrix;

struct SSolvedInfo {
    void reset() {
        n0 = s0 = n1 = s1 = n2 = s2 = n3 = s3 = n4 = s4 = n5 = s5 = ns = ss = 0;
    }

    // Number and number solved of the various types.
    int n0;
    int s0;
    int n1;
    int s1;
    int n2;
    int s2;
    int n3;
    int s3;
    int n4;
    int s4;
    int n5;
    int s5;
    int ns;
    int ss;
};

class CStateTransformer {
public:
    CStateTransformer(CSettings& settings);

    void stickersToMatrix(const std::vector<SSticker>& stickers,
                          StateMatrix& state) const;
    void matrixToStickers(const StateMatrix& state,
                          std::vector<SSticker>& stickers) const;

    // Table lookups.
    void getFaceOffset(int face, CVector5D& offset) const;
    int getFaceFromOffset(const CVector5D& offset) const;
    void getCubie(int stickerInt, const CVector5D& face, CVector5D& cubie) const;
    int getStickerInt(const CVector5D& cubie, const CVector5D& face) const;

    // Check to see if a state is solved.
    bool isSolved(const StateMatrix& state) const;

    // Get more detailed info on amount solved.
    // NOTE: 0-colors are always solved!
    void getSolvedStatus(const std::vector<SSticker>& stickers,
                         SSolvedInfo& solvedInfo) const;

    // Find cubies based on sticker colors.
    // NOTE: This only can return more than one answer in the 4^5 and 5^5 cases.
    void findCubies(const std::vector<SSticker>& state, const std::vector<int>& colors,
                    std::vector<CVector5D>& cubies);

private:
    int getNumPerFace() const;

private:
    CSettings& m_settings;
};
#pragma managed(pop)

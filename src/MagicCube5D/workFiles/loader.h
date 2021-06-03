#pragma once

#include "stateTransformer.h"
#include "twist.h"

class CLoader {
public:
    CLoader();

    // Save/Load the entire puzzle.
    void saveToFile(const StateMatrix& state, const std::vector<STwist>& twists,
                    bool saveas, bool scrambled = false);
    bool loadFromFile(StateMatrix& state, std::vector<STwist>& twists,
                      bool* scrambled = NULL);

    // Save/Load macros.
    void saveMacrosToFile(const std::vector<SMacro>& macros);
    bool loadMacrosFromFile(std::vector<SMacro>& macros);

private:

    // Save/Load twists.
    void saveTwists(System::IO::StreamWriter ^ sw, const std::vector<STwist>& twists);
    bool loadTwists(System::IO::StreamReader ^ sr, std::vector<STwist>& twists,
                    int numLines = -1);

    // Prompt for filenames.
    System::String ^ getSaveFileName(bool forcePrompt);
    System::String ^ getLoadFileName();

    // The last filename we saved.
    std::string m_filename;
};

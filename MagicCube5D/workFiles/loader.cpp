#include "loader.h"

#include <stdafx.h>

#include "vectorND.h"

using namespace System::Collections;
using System::Char;
using System::String;
using System::IO::File;

#define VERSION_STRING "Version1"

CLoader::CLoader() {}

void CLoader::saveToFile(const StateMatrix& state, const std::vector<STwist>& twists,
                         bool saveas, bool scrambled) {

    // Get the filename to save to.
    String ^ fileName = getSaveFileName(saveas);
    if ("" == fileName) return;

    System::IO::StreamWriter ^ sw = File::CreateText(fileName);

    // Write the header.
    String ^ header = "MagicCube5D\t" + VERSION_STRING + "\t" +
                      System::Convert::ToString(twists.size());

    // Append a simple "s" if the cube has been scrambled.
    if (scrambled) header += "\ts";

    sw->WriteLine(header);

    // Now write out the state.
    String ^ line;
    for (uint f = 0; f < state.size(); f++) {
        line = "";
        for (uint s = 0; s < state[f].size(); s++)
            line += System::Convert::ToString(state[f][s]);
        sw->WriteLine(line);
    }

    // Now write out all the twists.
    saveTwists(sw, twists);

    // Close the file.
    sw->Close();
}

bool CLoader::loadFromFile(StateMatrix& state, std::vector<STwist>& twists,
                           bool* scrambled) {
    String ^ fileName = getLoadFileName();
    if (!File::Exists(fileName)) return false;

    System::IO::StreamReader ^ sr = gcnew System::IO::StreamReader(fileName);

    // Delimiters (just tabs).
    array<Char> ^ delimiter = {'\t'};

    // Read the header.
    // Fail if unexpected format.
    String ^ line = sr->ReadLine();
    array<String ^> ^ split = line->Split(delimiter);
    IEnumerator ^ myEnum = split->GetEnumerator();

    myEnum->MoveNext();
    String ^ tempString = safe_cast<String ^>(myEnum->Current);
    if (!tempString->Equals("MagicCube5D")) return false;

    myEnum->MoveNext();
    tempString = safe_cast<String ^>(myEnum->Current);
    if (!tempString->Equals(VERSION_STRING)) return false;

    myEnum->MoveNext();
    tempString = safe_cast<String ^>(myEnum->Current);
    int numTwists = System::Convert::ToInt32(tempString);

    if (scrambled != NULL) *scrambled = split->Length >= 4 && split[3]->Equals("s");

    // Now read the state.
    state.resize(10);
    for (uint f = 0; f < state.size(); f++) {
        line = sr->ReadLine();
        state[f].resize(line->Length);
        for (int s = 0; s < line->Length; s++)
            state[f][s] = System::Convert::ToInt32(line->Substring(s, 1));
    }

    // Now read all the twists.
    loadTwists(sr, twists);

    // Close the file.
    sr->Close();

    return true;
}

void CLoader::saveMacrosToFile(const std::vector<SMacro>& macros) {
    String ^ fileName = "MagicCube5D.macros";
    System::IO::StreamWriter ^ sw = File::CreateText(fileName);

    for (uint m = 0; m < macros.size(); m++) {
        String ^ line = "Macro\t" + gcnew String(macros[m].m_name.c_str()) + "\t" +
                        System::Convert::ToString(macros[m].m_twists.size());
        sw->WriteLine(line);
        saveTwists(sw, macros[m].m_twists);
    }

    sw->Close();
}

bool CLoader::loadMacrosFromFile(std::vector<SMacro>& macros) {
    macros.clear();

    String ^ fileName = "MagicCube5D.macros";
    if (!File::Exists(fileName)) return false;

    System::IO::StreamReader ^ sr = gcnew System::IO::StreamReader(fileName);

    // Delimiters (just tabs).
    array<Char> ^ delimiter = {'\t'};

    String ^ line = "";
    while (sr->Peek() >= 0) {
        SMacro macro;

        // Read the macro name and number of twists.
        line = sr->ReadLine();
        array<String ^> ^ split = line->Split(delimiter);
        IEnumerator ^ myEnum = split->GetEnumerator();

        myEnum->MoveNext();
        String ^ tempString = safe_cast<String ^>(myEnum->Current);
        if (!tempString->Equals("Macro")) return false;

        myEnum->MoveNext();
        tempString = safe_cast<String ^>(myEnum->Current);
        for (int i = 0; i < tempString->Length; i++)
            macro.m_name.push_back((char)tempString[i]);

        myEnum->MoveNext();
        tempString = safe_cast<String ^>(myEnum->Current);
        int numTwists = System::Convert::ToInt32(tempString);

        // Read the twists.
        int numLines = (int)ceil((double)numTwists / 10);
        loadTwists(sr, macro.m_twists, numLines);

        macros.push_back(macro);
    }

    sr->Close();

    return true;
}

void CLoader::saveTwists(System::IO::StreamWriter ^ sw,
                         const std::vector<STwist>& twists) {
    String ^ line = "";
    int count = 0;
    for (uint t = 0; t < twists.size(); t++) {
        String ^ twistString =
                -1 == twists[t].m_rotationFace
                        ? "v"
                        : System::Convert::ToString(twists[t].m_rotationFace);
        twistString += System::Convert::ToString(twists[t].m_rotationAxis1) +
                       System::Convert::ToString(twists[t].m_rotationAxis2);
        if (1 != twists[t].m_slicemask)
            twistString += ":" + System::Convert::ToString(twists[t].m_slicemask);
        line += twistString;
        count++;

        if (count >= 10 || t == twists.size() - 1) {
            sw->WriteLine(line);
            line = "";
            count = 0;
        } else {
            line += "\t";
        }
    }
}

bool CLoader::loadTwists(System::IO::StreamReader ^ sr, std::vector<STwist>& twists,
                         int numLines) {
    twists.clear();

    // Delimiters (just tabs).
    array<Char> ^ delimiter = {'\t'};

    String ^ line = "";
    int linesRead = 0;
    while (sr->Peek() >= 0) {

        // Were there a specific number of lines specified to read?
        if (-1 != numLines)
            if (linesRead == numLines) return true;

        line = sr->ReadLine();
        linesRead++;
        array<String ^> ^ split = line->Split(delimiter);
        IEnumerator ^ myEnum = split->GetEnumerator();

        while (myEnum->MoveNext()) {
            String ^ tempString = safe_cast<String ^>(myEnum->Current);
            if (tempString->Length < 3 || tempString->Length > 6) return false;

            STwist twist;
            twist.m_rotationFace =
                    "v" == tempString->Substring(0, 1)
                            ? -1
                            : System::Convert::ToInt32(tempString->Substring(0, 1));
            twist.m_rotationAxis1 =
                    System::Convert::ToInt32(tempString->Substring(1, 1));
            twist.m_rotationAxis2 =
                    System::Convert::ToInt32(tempString->Substring(2, 1));

            // Handle loading a slicemask.
            if (tempString->Length > 3)
                twist.m_slicemask = System::Convert::ToInt32(
                        tempString->Substring(4, tempString->Length - 4));

            twists.push_back(twist);
        }
    }

    return true;
}

#define FILTER "MagicCube5D log files (*.log)|*.log|All files (*.*)|*.*"

System::String ^ CLoader::getSaveFileName(bool forcePrompt) {
    if (0 == m_filename.length() || forcePrompt) {
        System::Windows::Forms::SaveFileDialog ^ dlg =
                gcnew System::Windows::Forms::SaveFileDialog();
        dlg->AddExtension = true;
        dlg->OverwritePrompt = true;
        dlg->Filter = FILTER;
        if (System::Windows::Forms::DialogResult::OK != dlg->ShowDialog()) return "";
        m_filename = systemStringToStdString(dlg->FileName);
    }

    return gcnew String(m_filename.c_str());
}

System::String ^ CLoader::getLoadFileName() {
    System::Windows::Forms::OpenFileDialog ^ dlg =
            gcnew System::Windows::Forms::OpenFileDialog();
    dlg->CheckFileExists = true;
    dlg->Filter = FILTER;
    if (System::Windows::Forms::DialogResult::OK != dlg->ShowDialog()) return "";
    m_filename = systemStringToStdString(dlg->FileName);
    return dlg->FileName;
}

#undef FILTER

#include <iostream>
#include "ControlMapper.h"
#include "ini.h"

ControlMapper::ControlMapper(std::string& iniFilePath):
        iniFilePath(iniFilePath),
        m_ForwardVKCode(-1),
        m_BackwardVKCode(-1),
        m_RightVKCode(-1),
        m_LeftVKCode(-1),
        m_UpVKCode(-1),
        m_DownVKCode(-1)
{
    VKMap = {
        {"A", 'A'}, {"B", 'B'}, {"C", 'C'}, {"D", 'D'}, {"E", 'E'},
        {"F", 'F'}, {"G", 'G'}, {"H", 'H'}, {"I", 'I'}, {"J", 'J'},
        {"K", 'K'}, {"L", 'L'}, {"M", 'M'}, {"N", 'N'}, {"O", 'O'},
        {"P", 'P'}, {"Q", 'Q'}, {"R", 'R'}, {"S", 'S'}, {"T", 'T'},
        {"U", 'U'}, {"V", 'V'}, {"W", 'W'}, {"X", 'X'}, {"Y", 'Y'},
        {"Z", 'Z'}, {"Space", VK_SPACE}, {"Shift", VK_SHIFT}
    };
}

int ControlMapper::GetForwardVKCode()
{
    return m_ForwardVKCode;
}

int ControlMapper::GetBackwardVKCode()
{
    return m_BackwardVKCode;
}

int ControlMapper::GetLeftVKCode()
{
    return m_LeftVKCode;
}

int ControlMapper::GetRightVKCode()
{
    return m_RightVKCode;
}

int ControlMapper::GetUpVKCode()
{
    return m_UpVKCode;
}

int ControlMapper::GetDownVKCode()
{
    return m_DownVKCode;
}

void ControlMapper::parseControls() {
    std::map<std::string, std::string> controls;

    auto handler = [&](void* user, const char* section, const char* name, const char* val) {
        if (std::string(section) == "Controls") controls[name] = val;
        return 1;
    };

    if (ini_parse(iniFilePath.c_str(), handler, nullptr) < 0) {
        std::cerr << "Error while loading controls from '" << iniFilePath << "'\n";
        return;
    }

    m_ForwardVKCode  = VKMap[controls["MoveForward"]];
    m_BackwardVKCode = VKMap[controls["MoveBackward"]];
    m_RightVKCode    = VKMap[controls["MoveRight"]];
    m_LeftVKCode     = VKMap[controls["MoveLeft"]];
    m_UpVKCode       = VKMap[controls["MoveUp"]];
    m_DownVKCode     = VKMap[controls["MoveDown"]];
}
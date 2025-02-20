#include <iostream>
#include "ControlMapper.h"
#include "ini.h"
#include "ini.c"

ControlMapper* ControlMapper::Instance() {

    if (!Instance_) Instance_ = new ControlMapper(CONTROLS_INI_PATH);
    return Instance_;
}

ControlMapper::ControlMapper(std::string& iniFilePath):
        iniFilePath(iniFilePath),
        m_ForwardVKCode(-1),
        m_BackwardVKCode(-1),
        m_RightVKCode(-1),
        m_LeftVKCode(-1),
        m_UpVKCode(-1),
        m_DownVKCode(-1)
{
    m_VKMap = {
        {"a", 'A'}, {"b", 'B'}, {"c", 'C'}, {"d", 'D'}, {"e", 'E'},
        {"f", 'F'}, {"g", 'G'}, {"h", 'H'}, {"i", 'I'}, {"j", 'J'},
        {"k", 'K'}, {"l", 'L'}, {"m", 'M'}, {"n", 'N'}, {"o", 'O'},
        {"p", 'P'}, {"q", 'Q'}, {"r", 'R'}, {"s", 'S'}, {"t", 'T'},
        {"u", 'U'}, {"v", 'V'}, {"w", 'W'}, {"x", 'X'}, {"y", 'Y'},
        {"z", 'Z'}, {"space", VK_SPACE}, {"shift", VK_SHIFT}
    };

    parseControls();
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

static int InihHandler(void* map, const char* section, const char* name, const char* value)
{
    auto controls = static_cast<std::map<std::string, std::string>*>(map);

    if (std::string(section) == "Controls") (*controls)[name] = value;

    return 1;
}
#include <filesystem>
void ControlMapper::parseControls()
{
    std::map<std::string, std::string> controls;

    auto a = std::filesystem::current_path().string();
    if (ini_parse(iniFilePath.c_str(), InihHandler, &controls) < 0) {
        std::cerr << "Error while loading controls from '" << iniFilePath << "'\n";
        return;
    }

    // convert to lowercase
    for (auto& pair : controls)
    {
        std::transform(pair.second.begin(), pair.second.end(), pair.second.begin(),
                       [](unsigned char c){ return std::tolower(c); });
    }

    m_ForwardVKCode  = m_VKMap[controls["MoveForward"]];
    m_BackwardVKCode = m_VKMap[controls["MoveBackward"]];
    m_RightVKCode    = m_VKMap[controls["MoveRight"]];
    m_LeftVKCode     = m_VKMap[controls["MoveLeft"]];
    m_UpVKCode       = m_VKMap[controls["MoveUp"]];
    m_DownVKCode     = m_VKMap[controls["MoveDown"]];
}
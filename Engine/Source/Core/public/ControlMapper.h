#pragma once

#include <string>
#include <map>
#include <windows.h>
#include <Core/export.h>

class CORE_API ControlMapper final
{
public:
    static ControlMapper* Instance();

private:
    ControlMapper(std::string& iniFilePath);

public:
    int GetForwardVKCode();
    int GetBackwardVKCode();
    int GetLeftVKCode();
    int GetRightVKCode();
    int GetUpVKCode();
    int GetDownVKCode();

private:
    std::string iniFilePath;
    int m_ForwardVKCode;
    int m_BackwardVKCode;
    int m_RightVKCode;
    int m_LeftVKCode;
    int m_UpVKCode;
    int m_DownVKCode;
    std::map<std::string, int> m_VKMap;

    void parseControls();

private:
    static inline ControlMapper* Instance_ = nullptr;
    static inline std::string CONTROLS_INI_PATH = "../../../../../controls.ini";
};
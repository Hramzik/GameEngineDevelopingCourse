#pragma once

#include <string>
#include <map>
#include <windows.h>

class ControlMapper
{
public:
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

    int parseVKCode(std::string& key);
};
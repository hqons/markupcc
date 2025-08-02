#pragma once
#include <string>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <cstdio>
#endif

std::string getSystemFontPath(const std::string& fontName) {
#ifdef _WIN32
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return "";

    char value[512];
    DWORD value_length = sizeof(value);
    DWORD type = REG_SZ;

    // 支持 TrueType 字体名规则
    std::string regFontName = fontName + " (TrueType)";
    if (RegQueryValueExA(hKey, regFontName.c_str(), 0, &type, (LPBYTE)value, &value_length) == ERROR_SUCCESS) {
        char fontPath[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_FONTS, NULL, 0, fontPath); // 获取系统字体目录
        RegCloseKey(hKey);
        return std::string(fontPath) + "\\" + std::string(value);
    }
    RegCloseKey(hKey);
    return "";

#elif defined(__linux__) || defined(__APPLE__)
    std::string cmd = "fc-match -f \"%{file}\\n\" \"" + fontName + "\"";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";

    char buffer[512];
    std::string result;
    if (fgets(buffer, sizeof(buffer), pipe)) {
        result = buffer;
        result.erase(result.find_last_not_of(" \n\r\t") + 1);  // 去掉换行符
    }
    pclose(pipe);
    return result;
#else
    return ""; // 不支持的平台
#endif
}

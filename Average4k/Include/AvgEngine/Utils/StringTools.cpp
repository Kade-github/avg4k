/*
    Copyright 2021-2023 AvgEngine - Kade

    Use of this source code without explict permission from owner is strictly prohibited.
*/

#include <AvgEngine/Utils/StringTools.h>
#include <GLFW/glfw3.h>

static const std::unordered_map<int, std::string> stringTools_keyCodeToName = {
        {GLFW_KEY_A, "A"},
        {GLFW_KEY_B, "B"},
        {GLFW_KEY_C, "C"},
        {GLFW_KEY_D, "D"},
        {GLFW_KEY_E, "E"},
        {GLFW_KEY_F, "F"},
        {GLFW_KEY_G, "G"},
        {GLFW_KEY_H, "H"},
        {GLFW_KEY_I, "I"},
        {GLFW_KEY_J, "J"},
        {GLFW_KEY_K, "K"},
        {GLFW_KEY_L, "L"},
        {GLFW_KEY_M, "M"},
        {GLFW_KEY_N, "N"},
        {GLFW_KEY_O, "O"},
        {GLFW_KEY_P, "P"},
        {GLFW_KEY_Q, "Q"},
        {GLFW_KEY_R, "R"},
        {GLFW_KEY_S, "S"},
        {GLFW_KEY_T, "T"},
        {GLFW_KEY_U, "U"},
        {GLFW_KEY_V, "V"},
        {GLFW_KEY_W, "W"},
        {GLFW_KEY_X, "X"},
        {GLFW_KEY_Y, "Y"},
        {GLFW_KEY_Z, "Z"},
        {GLFW_KEY_0, "0"},
        {GLFW_KEY_1, "1"},
        {GLFW_KEY_2, "2"},
        {GLFW_KEY_3, "3"},
        {GLFW_KEY_4, "4"},
        {GLFW_KEY_5, "5"},
        {GLFW_KEY_6, "6"},
        {GLFW_KEY_7, "7"},
        {GLFW_KEY_8, "8"},
        {GLFW_KEY_9, "9"},
        {GLFW_KEY_SPACE, "SPACE"},
        {GLFW_KEY_APOSTROPHE, "APOSTROPHE"},
        {GLFW_KEY_COMMA, "COMMA"},
        {GLFW_KEY_MINUS, "MINUS"},
        {GLFW_KEY_PERIOD, "PERIOD"},
        {GLFW_KEY_SLASH, "SLASH"},
        {GLFW_KEY_SEMICOLON, "SEMICOLON"},
        {GLFW_KEY_EQUAL, "EQUAL"},
        {GLFW_KEY_LEFT_BRACKET, "LEFT_BRACKET"},
        {GLFW_KEY_BACKSLASH, "BACKSLASH"},
        {GLFW_KEY_RIGHT_BRACKET, "RIGHT_BRACKET"},
        {GLFW_KEY_GRAVE_ACCENT, "GRAVE_ACCENT"},
        {GLFW_KEY_WORLD_1, "WORLD_1"},
        {GLFW_KEY_WORLD_2, "WORLD_2"},
        {GLFW_KEY_ESCAPE, "ESCAPE"},
        {GLFW_KEY_ENTER, "ENTER"},
        {GLFW_KEY_TAB, "TAB"},
        {GLFW_KEY_BACKSPACE, "BACKSPACE"},
        {GLFW_KEY_INSERT, "INSERT"},
        {GLFW_KEY_DELETE, "DELETE"},
        {GLFW_KEY_RIGHT, "RIGHT"},
        {GLFW_KEY_LEFT, "LEFT"},
        {GLFW_KEY_DOWN, "DOWN"},
        {GLFW_KEY_UP, "UP"},
        {GLFW_KEY_PAGE_UP, "PAGE_UP"},
        {GLFW_KEY_PAGE_DOWN, "PAGE_DOWN"},
        {GLFW_KEY_HOME, "HOME"},
        {GLFW_KEY_END, "END"},
        {GLFW_KEY_CAPS_LOCK, "CAPS_LOCK"},
        {GLFW_KEY_SCROLL_LOCK, "SCROLL_LOCK"},
        {GLFW_KEY_NUM_LOCK, "NUM_LOCK"},
        {GLFW_KEY_PRINT_SCREEN, "PRINT_SCREEN"},
        {GLFW_KEY_PAUSE, "PAUSE"},
        {GLFW_KEY_F1, "F1"},
        {GLFW_KEY_F2, "F2"},
        {GLFW_KEY_F3, "F3"},
        {GLFW_KEY_F4, "F4"},
        {GLFW_KEY_F5, "F5"},
        {GLFW_KEY_F6, "F6"},
        {GLFW_KEY_F7, "F7"},
        {GLFW_KEY_F8, "F8"},
        {GLFW_KEY_F9, "F9"},
        {GLFW_KEY_F10, "F10"},
        {GLFW_KEY_F11, "F11"},
        {GLFW_KEY_F12, "F12"},
        {GLFW_KEY_F13, "F13"},
        {GLFW_KEY_F14, "F14"},
        {GLFW_KEY_F15, "F15"},
        {GLFW_KEY_F16, "F16"},
        {GLFW_KEY_F17, "F17"},
        {GLFW_KEY_F18, "F18"},
        {GLFW_KEY_F19, "F19"},
        {GLFW_KEY_F20, "F20"},
        {GLFW_KEY_F21, "F21"},
        {GLFW_KEY_F22, "F22"},
        {GLFW_KEY_F23, "F23"},
        {GLFW_KEY_F24, "F24"},
        {GLFW_KEY_F25, "F25"},
        {GLFW_KEY_KP_0, "KP_0"},
        {GLFW_KEY_KP_1, "KP_1"},
        {GLFW_KEY_KP_2, "KP_2"},
        {GLFW_KEY_KP_3, "KP_3"},
        {GLFW_KEY_KP_4, "KP_4"},
        {GLFW_KEY_KP_5, "KP_5"},
        {GLFW_KEY_KP_6, "KP_6"},
        {GLFW_KEY_KP_7, "KP_7"},
        {GLFW_KEY_KP_8, "KP_8"},
        {GLFW_KEY_KP_9, "KP_9"},
        {GLFW_KEY_KP_DECIMAL, "KP_DECIMAL"},
        {GLFW_KEY_KP_DIVIDE, "KP_DIVIDE"},
        {GLFW_KEY_KP_MULTIPLY, "KP_MULTIPLY"},
        {GLFW_KEY_KP_SUBTRACT, "KP_SUBTRACT"},
        {GLFW_KEY_KP_ADD, "KP_ADD"},
        {GLFW_KEY_KP_ENTER, "KP_ENTER"},
        {GLFW_KEY_KP_EQUAL, "KP_EQUAL"},
        {GLFW_KEY_LEFT_SHIFT, "LEFT_SHIFT"},
        {GLFW_KEY_LEFT_CONTROL, "LEFT_CONTROL"},
        {GLFW_KEY_LEFT_ALT, "LEFT_ALT"},
        {GLFW_KEY_LEFT_SUPER, "LEFT_SUPER"},
        {GLFW_KEY_RIGHT_SHIFT, "RIGHT_SHIFT"},
        {GLFW_KEY_RIGHT_CONTROL, "RIGHT_CONTROL"},
        {GLFW_KEY_RIGHT_ALT, "RIGHT_ALT"},
        {GLFW_KEY_RIGHT_SUPER, "RIGHT_SUPER"},
        {GLFW_KEY_MENU, "MENU"}
};


int AvgEngine::Utils::StringTools::convertKeyNameToKeyCode(const std::string& keyName)
{
    auto iter = std::find_if(std::begin(stringTools_keyCodeToName), std::end(stringTools_keyCodeToName),
        [&keyName](auto&& pair) {
            return pair.second == keyName;
        });
    if (iter == std::end(stringTools_keyCodeToName))
        return -1;
    return iter->first;
}

std::string AvgEngine::Utils::StringTools::convertKeyCodeToKeyName(const int& keyCode)
{

    auto it = stringTools_keyCodeToName.find(keyCode);
    if (it != stringTools_keyCodeToName.end())
    {
        return it->second;
    }

    return "UNKNOWN";
    
}

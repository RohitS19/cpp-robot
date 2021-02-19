// Only include this file 1 time (extra includes are ignored)
#pragma once

// Includes
#include <Windows.h>

// These methods are good for single input sends:
// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendinput
// MS DOCS on how to use SendInput and also how to do Batch Inputs

UINT InputEvent(DWORD type, DWORD flag)
{
    INPUT Input = {0};
    Input.type = type;
    Input.mi.dwFlags = flag;
    return ::SendInput(1, &Input, sizeof(INPUT));
}

UINT InputEvent(DWORD type, WORD VK, DWORD flag)
{
    INPUT Input = {0};
    Input.type = type;
    Input.ki.wVk = VK;
    Input.ki.dwFlags = flag;
    return ::SendInput(1, &Input, sizeof(INPUT));
}

UINT InputEvent(DWORD type, WORD VK)
{
    INPUT Input = {0};
    Input.type = type;
    Input.ki.wVk = VK;
    return ::SendInput(1, &Input, sizeof(INPUT));
}

/**********************************************
 ***              MOUSE Events              ***
 **********************************************/

namespace mouse
{
    // Private Members
    namespace helpers {
        UINT MouseEvent(DWORD flag) { return InputEvent(INPUT_MOUSE, flag); }

        LONG roundl(DOUBLE value) {
            return (LONG) value + ((LONG) value <= (LONG)(value - 0.5));
        }

        BOOL MoveCursorLinear(LONG x, LONG y, DWORD steps, DWORD milliseconds) {
            BOOL success = true;
            POINT cursor_pos;
            GetCursorPos(&cursor_pos);
            DOUBLE delta_x = x - cursor_pos.x, delta_y = y - cursor_pos.y;
            for(DWORD i = 0; i < steps; ++i) {
                if(milliseconds) { Sleep(milliseconds); }
                success = SetCursorPos(cursor_pos.x + roundl(i * delta_x / steps), cursor_pos.y + roundl(i * delta_y / steps)) && success;
            }
            return success;
        }
    }

    enum class MoveType
    {
        LINEAR
    };

    UINT LeftDown() { return helpers::MouseEvent(MOUSEEVENTF_LEFTDOWN); }

    UINT LeftUp() { return helpers::MouseEvent(MOUSEEVENTF_LEFTUP); }

    UINT RightDown() { return helpers::MouseEvent(MOUSEEVENTF_RIGHTDOWN); }

    UINT RightUp() { return helpers::MouseEvent(MOUSEEVENTF_RIGHTUP); }

    UINT LeftClick(DWORD milliseconds = 0)
    {
        UINT down = LeftDown();
        if (milliseconds != 0)
        {
            Sleep(milliseconds);
        }
        return down & LeftUp();
    }

    UINT RightClick(DWORD milliseconds = 0)
    {
        UINT down = RightDown();
        if (milliseconds != 0)
        {
            Sleep(milliseconds);
        }
        return down & RightUp();
    }

    BOOL MoveCursor(LONG x, LONG y, DWORD steps, MoveType type, DWORD milliseconds = 0)
    {
        if(steps == 0) {
            return SetCursorPos(x, y);
        }
        switch(type) {
            case MoveType::LINEAR: return helpers::MoveCursorLinear(x, y, steps, milliseconds);
            default: return false;
        }
    }
}

/***********************************************
 ***             KEYBOARD Events             ***
 ***********************************************/

namespace keyboard
{
    // Private Members
    namespace helpers {
        UINT KeyboardEvent(WORD VK, DWORD flag) { return InputEvent(INPUT_KEYBOARD, VK, flag); }

        UINT KeyboardEvent(WORD VK) { return InputEvent(INPUT_KEYBOARD, VK); }

        LONG roundl(DOUBLE value) {
            return (LONG) value + ((LONG) value <= (LONG)(value - 0.5));
        }
    }

    UINT KeyDown(WORD VK) {
        return helpers::KeyboardEvent(VK);
        
    }

    UINT KeyUp(WORD VK) {
        return helpers::KeyboardEvent(VK, KEYEVENTF_KEYUP);
    }

    UINT KeyDown(char key) {
        key = (char)toupper(key);
        return KeyDown((WORD)key);
    }

    UINT KeyUp(char key) {
        key = (char)toupper(key);
        return KeyUp((WORD)key);
    }

    UINT KeyDown(int key, DWORD milliseconds = 0) {
        UINT down = KeyDown((char)(key % 10 + '0'));
        key /= 10;
        while(key > 0) {
            if(milliseconds) { Sleep(milliseconds); }
            down &= KeyDown((char)(key % 10 + '0'));
            key /= 10;
        }
        return down;
    }

    UINT KeyUp(int key, DWORD milliseconds = 0) {
        UINT up = KeyUp((char)(key % 10 + '0'));
        key /= 10;
        while(key > 0) {
            if(milliseconds) { Sleep(milliseconds); }
            up &= KeyUp((char)(key % 10 + '0'));
            key /= 10;
        }
        return up;
    }

    UINT KeyDown(const char* keys, DWORD milliseconds = 0) {
        UINT down = KeyDown(keys[0]);
        for(UINT i = 1; keys[i]; ++i) {
            if(milliseconds) { Sleep(milliseconds); }
            down &= KeyDown(keys[i]);
        }
        return down;
    }

    UINT KeyUp(const char* keys, DWORD milliseconds = 0) {
        UINT up = KeyUp(keys[0]);
        for(UINT i = 1; keys[i]; ++i) {
            if(milliseconds) { Sleep(milliseconds); }
            up &= KeyUp(keys[i]);
        }
        return up;
    }

    UINT KeyType(char key, DWORD milliseconds = 0) {
        UINT down = KeyDown(key);
        if(milliseconds) { Sleep(milliseconds); }
        return down & KeyUp(key);
    }

    UINT KeyType(int key, DWORD milliseconds = 0) {
        int k = (char)(key % 10) + '0';
        UINT down = KeyDown(k);
        if(milliseconds) { Sleep(milliseconds); }
        down &= KeyUp(k);
        key /= 10;
        while(key > 0) {
            if(milliseconds) { Sleep(milliseconds); }
            k = (char)(key % 10) + '0';
            down &= KeyDown(k);
            if(milliseconds) { Sleep(milliseconds); }
            down &= KeyUp(k);
            key /= 10;
        }
        return down;
    }

    UINT KeyType(const char* word, DWORD milliseconds = 0) {
        UINT down = KeyDown(word[0]);
        for(UINT i = 1; word[i]; ++i) {
            if(milliseconds) { Sleep(milliseconds); }
            down &= KeyUp(word[i-1]);
            if(milliseconds) { Sleep(milliseconds); }
            down &= KeyDown(word[i]);
        }
        return down;
    }
}
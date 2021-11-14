#include "LGLCommon.hpp"

const int ALPHABET_SIZE      = 26;
const int DIGITS_COUNT       = 10;
const int SPECIAL_SYMS_COUNT = 11;

const char *DIGITS_SHIFTED       = "!@#$%^&*()";
const char *SPECIAL_SYMS         = "[];,.'/\\`=-";
const char *SPECIAL_SYMS_SHIFTED = "{}:<>\"?|~+_";

namespace LGL
{
    char toCharacter(LGL::KeyboardKey key, LGL::InputModifier modifier)
    {
        char c = '\0';

        if (key == LGL::KeyboardKey::Space)
        {
            c = ' ';
            return c;
        }

        int alphabetOffset = (int)key - (int)LGL::KeyboardKey::A;
        if (0 <= alphabetOffset && alphabetOffset < ALPHABET_SIZE)
        {
            if (isShiftPressed(modifier))
                c = 'A' + alphabetOffset;
            else
                c = 'a' + alphabetOffset;

            return c;
        }

        int digitsOffset = (int)key - (int)LGL::KeyboardKey::Num0;
        if (0 <= digitsOffset && digitsOffset < DIGITS_COUNT)
        {
            if (isShiftPressed(modifier))
                c = DIGITS_SHIFTED[digitsOffset];
            else
                c = '0' + digitsOffset;

            return c;
        }

        int specialSymsOffset = (int)key - (int)LGL::KeyboardKey::LBracket;
        if (0 <= specialSymsOffset && specialSymsOffset < SPECIAL_SYMS_COUNT)
        {
            if (isShiftPressed(modifier))
                c = SPECIAL_SYMS_SHIFTED[specialSymsOffset];
            else
                c = SPECIAL_SYMS[specialSymsOffset];

            return c;
        }

        // not a character
        return '\0';
    }
}
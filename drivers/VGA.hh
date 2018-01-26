#ifndef INCLUDED_VGA_HH
#define INCLUDED_VGA_HH

#include <stddef.h>
#include <stdint.h>
#include <kernel/utils/String.hh>

namespace Drivers
{
    //Static Class
    class VGA final
    {
        private:
        static bool initialized;
        static size_t terminal_row;
        static size_t terminal_column;
        static uint8_t terminal_color;
        static uint16_t* terminal_buffer;

        public:
        enum struct Color : int
        {
            BLACK = 0,
            BLUE = 1,
            GREEN = 2,
            CYAN = 3,
            RED = 4,
            MAGENTA = 5,
            BROWN = 6,
            LIGHT_GREY = 7,
            DARK_GREY = 8,
            LIGHT_BLUE = 9,
            LIGHT_GREEN = 10,
            LIGHT_CYAN = 11,
            LIGHT_RED = 12,
            LIGHT_MAGENTA = 13,
            LIGHT_BROWN = 14,
            WHITE = 15,
        };

        static constexpr size_t Width = 80;
        static constexpr size_t Height = 25;

        static constexpr uint8_t AttributeValue(const Color fg, const Color bg)
        {
            return (static_cast<int>(fg) | (static_cast<int>(bg) << 4));
        }

        static constexpr uint16_t CellValue(const unsigned char c, const uint8_t attr)
        {
            return (uint16_t)c | (uint16_t) attr << 8;
        }

        static void Init();
        static void SetColor(const uint8_t color);
        static void PutAt(const char c, const uint8_t color, const size_t x, const size_t y);
        static void PutChar(const char c);
        static void Write(const char* cstr, const size_t len);
        static void Write(const Utils::String str);

    };






}





#endif
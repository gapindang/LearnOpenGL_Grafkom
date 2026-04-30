#include <windows.h>
#include <iostream>

int main() {
    HDC hdc = CreateCompatibleDC(NULL);
    HFONT hFont = CreateFontA(64, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, 
                             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
                             DEFAULT_PITCH | FF_SWISS, "Arial");
    SelectObject(hdc, hFont);
    
    RECT rect = {0, 0, 0, 0};
    DrawTextA(hdc, "Test", -1, &rect, DT_CALCRECT);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    std::cout << "Text size: " << width << "x" << height << std::endl;
    return 0;
}

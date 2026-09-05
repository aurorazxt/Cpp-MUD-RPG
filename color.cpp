#include "mud_common.h"

ColorCtrl::ColorCtrl() {
    textColor = 7;
    optionColor = 10;
    infoColor = 14;
    resetColor();
}

int ColorCtrl::getTextColor() const { return textColor; }
int ColorCtrl::getOptionColor() const { return optionColor; }
int ColorCtrl::getInfoColor() const { return infoColor; }

void ColorCtrl::setTheme(int themeId) {
    switch (themeId) {
        case 1:
            textColor = 7;
            optionColor = 10;
            infoColor = 14;
            break;
        case 2:
            textColor = 15;
            optionColor = 11;
            infoColor = 13;
            break;
        case 3:
            textColor = 6;
            optionColor = 9;
            infoColor = 12;
            break;
        default:
            textColor = 7;
            optionColor = 10;
            infoColor = 14;
            break;
    }
}

void ColorCtrl::setTextColor(int color) { textColor = color; }
void ColorCtrl::setOptionColor(int color) { optionColor = color; }
void ColorCtrl::setInfoColor(int color) { infoColor = color; }

void ColorCtrl::applyText() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, textColor);
}

void ColorCtrl::applyOption() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, optionColor);
}

void ColorCtrl::applyInfo() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, infoColor);
}

void ColorCtrl::resetColor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7);
}

#include "MUD_GAME.h"

ColorCtrl::ColorCtrl()//默认构造函数
{
	textColor = 7;
	optionColor = 14;
	infoColor = 10;
}

void ColorCtrl::setTheme(int themeId)
{
	if (themeId == 1)
	{
		textColor = 7; optionColor = 14; infoColor = 10;
	}
	else if (themeId == 2)
	{
		textColor = 15; optionColor = 12; infoColor = 11;
	}
	else
	{
		textColor = 7; optionColor = 9; infoColor = 13;
	}
}
//修改颜色
void ColorCtrl::applyText()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), textColor);
}
void ColorCtrl::applyOption()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), optionColor);
}
void ColorCtrl::applyInfo()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), infoColor);
}
void ColorCtrl::resetColor()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}
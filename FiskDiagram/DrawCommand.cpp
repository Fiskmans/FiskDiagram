#include "DrawCommand.h"

DrawCommand::DrawCommand()
	: myDepth(0)
{
}

 LineCommand::LineCommand(Canvas::Point aMin, Canvas::Point aMax, V4F aColor, Canvas::Pattern aPattern, float aDepth)
	 : myMin(aMin)
	 , myMax(aMax)
	 , myColor(aColor)
	 , myPattern(aPattern)
{
	 myDepth = aDepth;
}

void
LineCommand::Draw(Canvas* aCanvas)
{
	aCanvas->DrawLine(myMin,myMax,myColor,myPattern);
}

BoxCommand::BoxCommand(Canvas::Point aMin, Canvas::Point aMax, V4F aColor, bool aFilled, Canvas::Pattern aPattern, float aDepth)
	: myMin(aMin)
	, myMax(aMax)
	, myColor(aColor)
	, myPattern(aPattern)
	, myFilled(aFilled)
{
	myDepth = aDepth;
}

void
BoxCommand::Draw(Canvas* aCanvas)
{
	aCanvas->DrawBox(myMin,myMax,myColor,myFilled,myPattern);
}

TextCommand::TextCommand(const std::string& aText, Canvas::Point aPosition, V4F aColor, float aDepth)
	: myText(aText)
	, myPosition(aPosition)
	, myColor(aColor)
{
	myDepth = aDepth;
}

void
TextCommand::Draw(Canvas* aCanvas)
{
	aCanvas->DrawText(myText,myPosition,myColor);
}

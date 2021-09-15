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

Canvas::Size LineCommand::Max()
{
	return {std::max(myMin.x, myMax.x), std::max(myMin.y, myMax.y)};
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

Canvas::Size BoxCommand::Max()
{
	return myMax;
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

Canvas::Size TextCommand::Max()
{
	return {myPosition.x + Canvas::MeasureString(myText).x,myPosition.y};
}

BezierCommand::BezierCommand(Canvas::Point aStart, Canvas::Point aC1, Canvas::Point aC2, Canvas::Point aEnd, V4F aColor, Canvas::Pattern aPattern, float aDepth)
	: myStart(aStart)
	, myC1(aC1)
	, myC2(aC2)
	, myEnd(aEnd)
	, myColor(aColor)
	, myPattern(aPattern)
{
	myDepth = aDepth;
}

void BezierCommand::Draw(Canvas* aCanvas)
{
	aCanvas->DrawBezier(myStart,myC1,myC2,myEnd,myColor,myPattern);
}

Canvas::Size BezierCommand::Max()
{
	return 
	{
		std::max(std::max(myStart.x,myC1.x),std::max(myC2.x,myEnd.x)),
		std::max(std::max(myStart.y,myC1.y),std::max(myC2.y,myEnd.y))
	};
}

TriangleCommand::TriangleCommand(Canvas::Point aA, Canvas::Point aB, Canvas::Point aC, V4F aColor, float aDepth)
	: myA(aA)
	, myB(aB)
	, myC(aC)
	, myColor(aColor)
{
	myDepth = aDepth;
}

void TriangleCommand::Draw(Canvas* aCanvas)
{
	aCanvas->DrawTriangle(myA,myB,myC,myColor);
}

Canvas::Size TriangleCommand::Max()
{
	return {
		std::max(std::max(myA.x, myB.x),myC.x),
		std::max(std::max(myA.y, myB.y),myC.y)};
}


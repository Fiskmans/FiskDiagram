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

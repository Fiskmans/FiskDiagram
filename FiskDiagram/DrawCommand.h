#pragma once
#include "Canvas.h"

class DrawCommand
{
public:
	DrawCommand();
	virtual ~DrawCommand() = default;
	virtual void Draw(Canvas* aCanvas) = 0;
	
	float myDepth;
};

class LineCommand : public DrawCommand
{
public:
	LineCommand(Canvas::Point aMin, Canvas::Point aMax,V4F aColor, Canvas::Pattern aPattern, float aDepth);

	void Draw(Canvas* aCanvas) override;

private:
	Canvas::Point	myMin;
	Canvas::Point	myMax;
	V4F myColor;
	Canvas::Pattern myPattern;
};
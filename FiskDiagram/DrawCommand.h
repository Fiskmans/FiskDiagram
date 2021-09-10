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

class BoxCommand : public DrawCommand
{
public:
	BoxCommand(Canvas::Point aMin, Canvas::Point aMax,V4F aColor, bool aFilled, Canvas::Pattern aPattern, float aDepth);

	void Draw(Canvas* aCanvas) override;

private:
	Canvas::Point	myMin;
	Canvas::Point	myMax;
	V4F myColor;
	bool myFilled;
	Canvas::Pattern myPattern;
};

class TextCommand : public DrawCommand
{
public:
	TextCommand(const std::string& aText,Canvas::Point aPosition,V4F aColor,float aDepth);

	void Draw(Canvas* aCanvas) override;

private:
	std::string myText;
	Canvas::Point	myPosition;
	V4F myColor;
};

class BezierCommand : public DrawCommand
{
public:
	BezierCommand(Canvas::Point aStart,Canvas::Point aC1,Canvas::Point aC2,Canvas::Point aEnd,V4F aColor, Canvas::Pattern aPattern,float aDepth);

	void Draw(Canvas* aCanvas) override;

private:
	Canvas::Point myStart;
	Canvas::Point myC1;
	Canvas::Point myC2;
	Canvas::Point myEnd;
	Canvas::Pattern myPattern;
	V4F myColor;
};
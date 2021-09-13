#pragma once
#include "Vector.hpp"

struct ID3D11ShaderResourceView;
struct ID3D11Device;

#ifdef DrawText
#undef DrawText
#endif

class Canvas
{
public:
	enum class BlendMode
	{
		Add,
		Replace
	};

	typedef CommonUtilities::Vector2<int> Point;
	typedef CommonUtilities::Vector2<int> Size;
	typedef std::vector<bool> Pattern;
	
	struct Patterns
	{
		const static Canvas::Pattern Solid;
		const static Canvas::Pattern Dotted;
		const static Canvas::Pattern Dashed;
		const static Canvas::Pattern Striped;
		const static Canvas::Pattern CutLine;
	};

	Canvas();
	~Canvas();

	void Setup(Size aSize, V4F aClearColor);

	ID3D11ShaderResourceView* Export(ID3D11Device* aDevice);

	size_t GetWidth();
	size_t GetHeight();

	void DrawPixel(Point aPoint, V4F aColor);
	void DrawBox(Point aMin, Point aMax, V4F aColor, bool aFilled, Pattern aPattern = Patterns::Solid);
	void DrawText(const std::string& aText, Point aBottomLeft, V4F	aColor);
	void DrawLine(Point aStart, Point aEnd, V4F aColor, Pattern aPattern = Patterns::Solid);
	void DrawBezier(Point aStart,Point aC1,Point aC2,Point aEnd, V4F aColor, Pattern aPattern = Patterns::Solid);

	static Size MeasureString(const std::string& aString);

private:

	Point BezierInterpolate(Point aStart,Point aC1,Point aC2,Point aEnd, float aValue);
	
	BlendMode myBlendMode;

	size_t myWidth;
	size_t myHeight;
	V4F* myTexture;
};


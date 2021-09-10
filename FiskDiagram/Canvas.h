#pragma once
#include "Vector.hpp"

struct ID3D11ShaderResourceView;
struct ID3D11Device;

class Canvas
{
public:
	enum class BlendMode
	{
		Add,
		Replace
	};

	typedef CommonUtilities::Vector2<int> Point;
	typedef std::vector<bool> Pattern;

	Canvas();
	~Canvas();

	void Setup(size_t aWidth, size_t aHeight, V4F aClearColor);

	ID3D11ShaderResourceView* Export(ID3D11Device* aDevice);

	size_t GetWidth();
	size_t GetHeight();

	void DrawPixel(Point aPoint, V4F aColor);
	void DrawBox(Point aMin, Point aMax, V4F aColor, bool aFilled);
	void DrawText(const std::string& aText, Point aBottomLeft, V4F	aColor);
	void DrawLine(Point aStart, Point aEnd, V4F aColor, Pattern aPattern = { true });
	void DrawBezier(Point aStart,Point aC1,Point aC2,Point aEnd, V4F aColor, Pattern aPattern = { true });

private:
	
	BlendMode myBlendMode;

	size_t myWidth;
	size_t myHeight;
	V4F* myTexture;
};


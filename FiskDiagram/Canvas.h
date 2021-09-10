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

	Canvas();
	~Canvas();

	void Setup(size_t aWidth, size_t aHeight, V4F aClearColor);

	ID3D11ShaderResourceView* Export(ID3D11Device* aDevice);

	size_t GetWidth();
	size_t GetHeight();

	void DrawPixel(int aX, int aY, V4F aColor);
	void DrawBox(CommonUtilities::Vector2<int> aMin, CommonUtilities::Vector2<int> aMax, V4F aColor, bool aFilled);
	void DrawText(const std::string& aText, CommonUtilities::Vector2<int> aBottomLeft, V4F	aColor);
	void DrawLine(CommonUtilities::Vector2<int> aStart, CommonUtilities::Vector2<int> aEnd, V4F aColor, std::vector<bool> aPattern = { true });

private:
	
	BlendMode myBlendMode;

	size_t myWidth;
	size_t myHeight;
	V4F* myTexture;
};


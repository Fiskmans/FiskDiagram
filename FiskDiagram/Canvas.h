#pragma once
#include "Vector.hpp"

struct ID3D11ShaderResourceView;
struct ID3D11Device;

class Canvas
{
public:
	Canvas();
	~Canvas();

	void Setup(size_t aWidth, size_t aHeight, V3F aClearColor);

	ID3D11ShaderResourceView* Export(ID3D11Device* aDevice);

	size_t GetWidth();
	size_t GetHeight();
private:
	
	size_t myWidth;
	size_t myHeight;
	V4F* myTexture;
};


#include "stdafx.h"
#include "Canvas.h"
#include <d3d11.h>

Canvas::Canvas()
	: myHeight(0)
	, myWidth(0)
	, myTexture(nullptr)
	, myBlendMode(BlendMode::Add)
{
}

Canvas::~Canvas()
{
	delete[] myTexture;
}

void Canvas::Setup(size_t aWidth, size_t aHeight, V4F aClearColor)
{
	if (myTexture)
		delete[] myTexture;

	myWidth = aWidth;
	myHeight = aHeight;

	myTexture = new V4F[myWidth * myHeight];
	for (size_t i = 0; i < myWidth * myHeight; i++)
		myTexture[i] = V4F(aClearColor, 1);
}

ID3D11ShaderResourceView* Canvas::Export(ID3D11Device* aDevice)
{
	ID3D11ShaderResourceView* view;

	{
		D3D11_TEXTURE2D_DESC desc;
		WIPE(desc);
		desc.Width = myWidth;
		desc.Height = myHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data;
		WIPE(data);
		data.pSysMem = myTexture;
		data.SysMemPitch = myWidth * sizeof(V3F);
		data.SysMemSlicePitch = myWidth * myHeight * sizeof(V3F);

		
		ID3D11Texture2D* tex;
		HRESULT res = aDevice->CreateTexture2D(&desc, &data, &tex);

		if (FAILED(res))
		{
			LOGERROR("Failed to create texture");
			return nullptr;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		WIPE(viewDesc);

		viewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		viewDesc.Texture2D.MipLevels = 1;
		viewDesc.Texture2D.MostDetailedMip = 0;
		viewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
		res = aDevice->CreateShaderResourceView(tex, &viewDesc, &view);

		tex->Release();
		if (FAILED(res))
		{
			LOGERROR("Failed to create view");
			return nullptr;
		}
	}

	return view;
}

size_t Canvas::GetWidth()
{
	return myWidth;
}

size_t Canvas::GetHeight()
{
	return myHeight;
}

void Canvas::DrawPixel(size_t aX, size_t aY, V4F aColor)
{
	if (aX < 0 || aY < 0 || aX >= myWidth || aY >= myHeight)
	{
		return;
	}
	size_t index = aY * myWidth + aX;
	switch (myBlendMode)
	{
	case BlendMode::Add:
		myTexture[index] = aColor + LERP(myTexture[index], V4F(0, 0, 0, 0), aColor.w);
		myTexture[index].x = CLAMP(0, 1, myTexture[index].x);
		myTexture[index].y = CLAMP(0, 1, myTexture[index].y);
		myTexture[index].z = CLAMP(0, 1, myTexture[index].z);
		break;
	case BlendMode::Replace:
		myTexture[index] = aColor;
		break;
	default:
		break;
	}
}

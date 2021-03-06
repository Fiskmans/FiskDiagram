#pragma once
#include "DirectX11Framework.h"
#include <d3d11.h>
#include "ShaderCompiler.h"
#include "Canvas.h"
#include "DrawCommand.h"


class DiagramDrawer
{
public:
	DiagramDrawer(DirectX11Framework* aFrameWork, const std::string& aFile);
	~DiagramDrawer();


	void Update();

	void Render();
private:
	void Reload(const std::string aFilePath);
	void Draw(std::vector<DrawCommand*> aDrawList);

	std::vector<DrawCommand*> Parse(std::istream& aContent);

	std::string myFilePath;

	Canvas myCanvas;
	Tools::FileWatcher myWatcher;
	Tools::FileWatcher::UniqueID myWatchHandle;

	DirectX11Framework* myFramework;
	ID3D11ShaderResourceView* myGraphicsTexture;
	ID3D11Buffer* myBuffer;
	ID3D11Buffer* myVertexBuffer;
	ID3D11SamplerState* mySampler;
	ID3D11RenderTargetView* myTarget;
	D3D11_VIEWPORT* myViewPort;

	PixelShader* myPixelShader;
	VertexShader* myVertexShader;
	bool myIsDragging;
	V4F myViewWindow;
};


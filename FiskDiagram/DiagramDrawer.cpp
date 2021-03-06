#include "stdafx.h"
#include "DiagramDrawer.h"
#include "Diagram.h"
#include <sstream>
#include <fstream>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

struct VertexBuffer
{
	V4F myViewPort;
	float myWidth;
	float myHeight;
	V2F padding;
};

DiagramDrawer::DiagramDrawer(DirectX11Framework* aFrameWork, const std::string& aFile) : myFramework(aFrameWork), myFilePath(aFile) , myGraphicsTexture(nullptr)
{
	myIsDragging = false;
	myViewWindow = V4F(0, 0, 1920, 1080);
	ID3D11Device* device = aFrameWork->GetDevice();
	Reload(myFilePath);
	myWatchHandle = myWatcher.RegisterCallback(myFilePath, std::bind(&DiagramDrawer::Reload, this,std::placeholders::_1));

	std::vector<char> blob;
	myVertexShader = GetVertexShader(device, "Data/Shaders/Fullscreen/VertexShader.hlsl", blob);
	if (!myVertexShader)
	{
		return;
	}

	myPixelShader = GetPixelShader(device, "Data/Shaders/Fullscreen/Copy.hlsl");
	if (!myPixelShader)
	{
		return;
	}

	{
		D3D11_BUFFER_DESC desc;
		WIPE(desc);
		desc.ByteWidth = sizeof(VertexBuffer);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		HRESULT res = device->CreateBuffer(&desc,nullptr,&myVertexBuffer);
		if (FAILED(res))
		{
			LOGERROR("Failed to create buffer");
			return;
		}
	}
	{
		D3D11_SAMPLER_DESC desc;
		WIPE(desc);
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;

		HRESULT res = device->CreateSamplerState(&desc,&mySampler);
		if (FAILED(res))
		{
			LOGERROR("Failed to create sampler state");
			return;
		}
	}

	{
		ID3D11Texture2D* tex = aFrameWork->GetBackbufferTexture();

		HRESULT result = myFramework->GetDevice()->CreateRenderTargetView(tex, nullptr, &myTarget);
		if (FAILED(result))
		{
			return;
		}

		if (tex)
		{
			D3D11_TEXTURE2D_DESC desc;
			tex->GetDesc(&desc);
			myViewPort = new D3D11_VIEWPORT({ 0.f, 0.f, static_cast<float>(desc.Width), static_cast<float>(desc.Height), 0.f, 1.f });
		}
	}
}

DiagramDrawer::~DiagramDrawer()
{
	myWatcher.UnRegister(myWatchHandle);
}

void DiagramDrawer::Update()
{
	myIsDragging = ImGui::GetIO().MouseDown[0];

	if (myIsDragging)
	{
		myViewWindow.x -= ImGui::GetIO().MouseDelta.x;
		myViewWindow.z -= ImGui::GetIO().MouseDelta.x;
		myViewWindow.y -= ImGui::GetIO().MouseDelta.y;
		myViewWindow.w -= ImGui::GetIO().MouseDelta.y;
	}

	myWatcher.FlushChanges();

}

void DiagramDrawer::Render()
{
	ID3D11DeviceContext* context = myFramework->GetContext();

	{
		D3D11_MAPPED_SUBRESOURCE resource;
		HRESULT res = context->Map(myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

		if (SUCCEEDED(res))
		{
			VertexBuffer data;
			data.myViewPort = myViewWindow;
			data.myHeight = myCanvas.GetHeight();
			data.myWidth = myCanvas.GetWidth();

			memcpy(resource.pData, &data, sizeof(data));

			context->Unmap(myVertexBuffer, 0);
		}
		else
		{
			return;
		}
	}
	context->VSSetConstantBuffers(0, 1, &myVertexBuffer);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(nullptr);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
	context->GSSetShader(nullptr, nullptr, 0);

	context->PSSetSamplers(0, 1, &mySampler);
	
	context->PSSetShaderResources(0, 1, &myGraphicsTexture);

	context->OMSetRenderTargets(1, &myTarget, nullptr);
	context->RSSetViewports(1, myViewPort);

	context->PSSetShader(*myPixelShader,nullptr,0);
	context->VSSetShader(*myVertexShader,nullptr,0);

	context->Draw(6, 0);
}

void DiagramDrawer::Reload(const std::string aFilePath)
{
	std::ifstream				ifs(aFilePath);
	Draw(Parse(ifs));
	if (myGraphicsTexture)
		myGraphicsTexture->Release();
	myGraphicsTexture = nullptr;
	myGraphicsTexture = myCanvas.Export(myFramework->GetDevice());
}

void DiagramDrawer::Draw(std::vector<DrawCommand*> aDrawList)
{
	Canvas::Size size{10, 10};
	for (DrawCommand*	command : aDrawList)
	{
		Canvas::Size max = command->Max();
		size.x = std::max(size.x, max.x);
		size.y = std::max(size.y, max.y);
	}

	size.x += 30;
	size.y += 30;

	myCanvas.Setup(size, V4F(1, 1, 1, 1));


	std::sort(aDrawList.begin(),aDrawList.end(),[](DrawCommand* aLHS,DrawCommand* aRHS) { return aLHS->myDepth < aRHS->myDepth; });

	for (DrawCommand* command : aDrawList)
	{
		command->Draw(&myCanvas);
		delete command;
	}
	aDrawList.clear();
}

std::vector<DrawCommand*> DiagramDrawer::Parse(std::istream& aContent)
{
	Diagram diagram;

	std::string line;
	while (std::getline(aContent, line))
	{
		diagram.AddLine(line);
	}

	return diagram.Finalize();
}

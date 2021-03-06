#pragma once
#include "ShaderFlags.h"
#include "Shaders.h"

struct ID3D11Device;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11GeometryShader;

void ReleaseAllShaders();

PixelShader* GetPixelShader(ID3D11Device* aDevice, const std::string& aPath);
VertexShader* GetVertexShader(ID3D11Device* aDevice, const std::string& aPath, std::vector<char>& aVsBlob);

void ReloadPixelShader(ID3D11Device* aDevice, const std::string& aPath);
void ReloadVertexShader(ID3D11Device* aDevice, const std::string& aPath);

bool LoadGeometryShader(ID3D11Device* aDevice, std::string aFilePath, ID3D11GeometryShader*& aShaderOutput);

bool CompilePixelShader(ID3D11Device* aDevice, std::string aData, ID3D11PixelShader*& aShaderOutput);
bool CompileGeometryShader(ID3D11Device* aDevice, std::string aData, ID3D11GeometryShader*& aShaderOutput);
bool CompileVertexShader(ID3D11Device* aDevice, std::string aData, ID3D11VertexShader*& aShaderOutput, void* aCompiledOutput);

void FlushShaderChanges();
#include "stdafx.h"
#include "ImGuiHelpers.h"
#include <math.h>
#include "TimeHelper.h"

namespace Tools
{
	namespace ImguiHelperGlobals
	{
		ID3D11Device* globalDevice;
		ID3D11DeviceContext* globalContext;
		size_t globalTextureUsage;
		FullscreenRenderer* globalRenderer;
		FullscreenTextureFactory* globalTextureFactory;
		
		void ResetCounter()
		{
			globalTextureUsage = 0;
		}

		void Setup(ID3D11Device* aDevice, ID3D11DeviceContext* aContext, FullscreenRenderer* aRenderer, FullscreenTextureFactory* aFactory)
		{
			ResetCounter();
			globalDevice = aDevice;
			globalContext = aContext;
			globalRenderer = aRenderer;
			globalTextureFactory = aFactory;
		}
	}

	void ZoomableImGuiImage(void* aTexture, ImVec2 aSize)
	{
		ImVec2 drawPos = ImGui::GetCursorScreenPos();
		ImGui::Image(aTexture, aSize);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		{
			ImVec2 iopos = ImVec2(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
			ImGui::SetNextWindowPos(iopos);
			ImGui::BeginTooltip();
			static float region_sz = 32.0f;
			static float zoom = 8.0f;
			float delta = ImGui::GetIO().MouseWheel;
			if (ImGui::GetIO().KeyShift)
			{
				zoom *= pow(1.1, delta);
			}
			else
			{
				region_sz *= pow(1.1, -delta);
				zoom *= pow(1.1, delta);
			}
			if (region_sz > 200.f)
			{
				region_sz *= pow(1.1, delta);
				zoom *= pow(1.1, -delta);
			}

			float region_x = iopos.x - drawPos.x - region_sz * 0.5f; if (region_x < 0.0f) region_x = 0.0f; else if (region_x > aSize.x - region_sz) region_x = aSize.x - region_sz;
			float region_y = iopos.y - drawPos.y - region_sz * 0.5f; if (region_y < 0.0f) region_y = 0.0f; else if (region_y > aSize.y - region_sz) region_y = aSize.y - region_sz;
			ImVec2 uv0 = ImVec2((region_x) / aSize.x, (region_y) / aSize.y);
			ImVec2 uv1 = ImVec2((region_x + region_sz) / aSize.x, (region_y + region_sz) / aSize.y);
			ImGui::Image(aTexture, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
			ImGui::EndTooltip();
		}

	}

	ImVec4 GetColor(int aDepth)
	{
		static std::vector<ImVec4> colors = {
			ImVec4(0.0f,1.0f,0.0f,1.0f),
			ImVec4(1.0f,0.0f,0.0f,1.0f),
			ImVec4(0.5f,0.7f,0.5f,1.0f),
			ImVec4(0.5f,0.5f,0.8f,1.0f),
			ImVec4(0.9f,0.9f,0.5f,1.0f),
			ImVec4(0.8f,0.2f,0.5f,1.0f),
			ImVec4(0.6f,0.9f,0.1f,1.0f)
		};
		return colors[aDepth % colors.size()];
	}
	ImVec4 DefaultColor(int aDepth)
	{
		return ImGui::GetStyleColorVec4(ImGuiCol_Text);
	}

	const char* GetCovarage(float aProcent)
	{
		if (aProcent > 0.95)
		{
			return "All";
		}
		if (aProcent > 0.85)
		{
			return "Good";
		}
		if (aProcent > 0.70)
		{
			return "Lacking";
		}
		if (aProcent > 0.40)
		{
			return "Half";
		}
		if (aProcent > 0.3)
		{
			return "Insignificant";
		}
		if (aProcent > 0.1)
		{
			return "Miniscule";
		}
		return "None";
	}
	std::string PadOrTrimTo(const std::string& aString, int aLength)
	{
		if (aString.length() > aLength)
		{
			return aString.substr(0, aLength);
		}
		return aString + std::string(aLength - aString.length(), ' ');
	}

	Tools::TimeTree*& HoveredTimeTree()
	{
		static Tools::TimeTree* val = nullptr;
		return val;
	}

	void DrawTimeTree_internal(Tools::TimeTree* aTree, int aDepth, std::function<ImVec4(int)> aColorGetter, const char* aFormat, std::function<float(TimeTree*)> aArgumentGetter)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, aColorGetter(aDepth));
		if (ImGui::TreeNode(aTree->myName, aFormat, PadOrTrimTo(aTree->myName, 20).c_str(),(int)aTree->myCallCount,  aArgumentGetter(aTree), GetCovarage(aTree->myCovarage)))
		{
			if (ImGui::IsItemHovered())
			{
				HoveredTimeTree() = aTree;
			}
			for (auto& i : aTree->myChildren)
			{
				DrawTimeTree_internal(i, aDepth + 1, aColorGetter, aFormat, aArgumentGetter);
			}
			if (aTree->myChildren.empty())
			{
				ImGui::BulletText("No futher breakdown available");
			}
			ImGui::TreePop();
		}
		ImGui::PopStyleColor(1);
	}

	void DrawTimeTree(Tools::TimeTree* aTree)
	{

		ImGui::PushID(aTree->myName);
		static bool colorize = true;
		ImGui::Checkbox("Colorize", &colorize);
		static int mode = 0;

		const char* names[4] = { "time","Percent of parent","percent of performance","percent of hovered" };
		ImGui::SameLine();
		if (ImGui::BeginCombo("Value Mode", names[mode]))
		{
			for (size_t i = 0; i < 4; i++)
			{
				bool selected = mode == i;
				if (ImGui::Selectable(names[i], &selected))
				{
					mode = i;
				}
			}
			ImGui::EndCombo();
		}


		const char* format = nullptr;
		std::function<float(TimeTree*)> valueGetter;
		switch (mode)
		{
		case 0:
			format = "%s: [%3d] %5.2fms covarage: %s";
			valueGetter = [](TimeTree* aNode) -> float { return aNode->myTime * 1000.f; };
			break;
		case 1:
			format = "%s: [%3d] %5.1f%% covarage: %s";
			valueGetter = [](TimeTree* aNode) -> float { return (aNode->myParent ? aNode->myTime / aNode->myParent->myTime : 1.f) * 100.f; };
			break;
		case 2:
			format = "%s: [%3d] %5.1f%% covarage: %s";
			valueGetter = [](TimeTree* aNode) -> float
			{
				std::function< float(TimeTree*)> GetRootTime = [&GetRootTime](TimeTree* aNode) -> float
				{
					if (aNode->myParent)
					{
						return GetRootTime(aNode->myParent);
					}
					return aNode->myTime;
				};
				return aNode->myTime / GetRootTime(aNode) * 100.f;
			};
			break;
		case 3:
			format = "%s: [%3d] %5.1f%% covarage: %s";
			valueGetter = [](TimeTree* aNode) -> float { return (HoveredTimeTree() ? aNode->myTime / HoveredTimeTree()->myTime : 1.f) * 100.f; };
			break;
		default:
			break;
		}

		ImGui::BeginChild(aTree->myName);
		if (colorize)
		{
			DrawTimeTree_internal(aTree, 0, &GetColor, format, valueGetter);
		}
		else
		{
			DrawTimeTree_internal(aTree, 0, &DefaultColor, format, valueGetter);
		}
		ImGui::EndChild();
		ImGui::PopID();
	}

}
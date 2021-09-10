#include "stdafx.h"
#include "WindowData.h"

#include "GraphicEngine.h"
#include "DirectX11Framework.h"
#include "WindowHandler.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "DiagramDrawer.h"
#include "Run.h"

void SaveImGuiStyle()
{
	static_assert(sizeof(char) == 1, "Double check");
	std::ofstream stream;
	stream.open("ImGuiStyle.ini", std::ios::binary | std::ios::out);
	stream.write(reinterpret_cast<char*>(&ImGui::GetStyle()), sizeof(*(&ImGui::GetStyle())));
}

void LoadOrDefaultImGuiStyle()
{
	static_assert(sizeof(char) == 1, "Double check");
	std::ifstream stream;
	stream.open("ImGuiStyle.ini", std::ios::binary | std::ios::in);
	if (stream)
	{
		stream.read(reinterpret_cast<char*>(&ImGui::GetStyle()), sizeof(*(&ImGui::GetStyle())));
	}
	else
	{
		ImGui::StyleColorsDark();
	}
}

void AttachConsole()
{
	AllocConsole();

	FILE* fp;
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stderr);
}

int Run(const char* aFile)
{
	const wchar_t* commLine = GetCommandLineW();
	int argc;
	LPWSTR* argv = CommandLineToArgvW(commLine, &argc);
	for (size_t i = 0; i < argc; i++)
	{
		DebugTools::CommandLineFlags.emplace(argv[i]);
	}

	long long startTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();

	AttachConsole();

	Logger::SetFilter(Logger::Type::AnyGame | Logger::Type::AnyWarning | Logger::Type::AllSystem & ~Logger::Type::AnyVerbose);
	Logger::SetHalting(Logger::Type::SystemCrash);


	Logger::Map(Logger::Type::AnyGame, "info");
	Logger::Map(Logger::Type::AnySystem, "system");
	Logger::SetColor(Logger::Type::AnyError, FOREGROUND_RED | FOREGROUND_INTENSITY);
	Logger::SetColor(Logger::Type::AnyWarning, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	Logger::SetColor(Logger::Type::AnyInfo, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	Logger::SetColor(Logger::Type::SystemNetwork, FOREGROUND_GREEN | FOREGROUND_BLUE);


	Window::WindowData windowData;
	windowData.myX = 0;
	windowData.myY = 0;
	windowData.myWidth = 1920;
	windowData.myHeight = 1080;

	CGraphicsEngine engine;

	if (!engine.Init(windowData))
	{
		SYSCRASH("Could not start engine");
		return -1;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	LoadOrDefaultImGuiStyle();
	ImGui_ImplDX11_Init(engine.GetFrameWork()->GetDevice(), engine.GetFrameWork()->GetContext());
	ImGui_ImplWin32_Init(engine.GetWindowHandler()->GetWindowHandle());


	DiagramDrawer diagram(engine.GetFrameWork(), aFile);

	MSG windowMessage;
	WIPE(windowMessage);
	float clearColor[4] = { 0.8f,0.36f,0.7f,1.f };
	bool shutdown = false;
	while (!shutdown)
	{
		{
			PERFORMANCETAG("Winmessage Parsing");
			while (PeekMessage(&windowMessage, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&windowMessage);
				DispatchMessage(&windowMessage);

				if (windowMessage.message == WM_QUIT || windowMessage.message == WM_DESTROY)
				{
					shutdown = true;
				}
			}
		}
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		WindowControl::DrawWindowControl();
		WindowControl::Window("Console log", []()
			{
				Logger::ImGuiLog();
			});
		DirectX11Framework::Imgui();
		WindowControl::Window("Diagnostic", []()
			{
				Tools::TimeTree* at = Tools::GetTimeTreeRoot();
				static bool accumulative = true;
				ImGui::Checkbox("Accumulative", &accumulative);
				Tools::DrawTimeTree(at);
				if (!accumulative)
				{
					Tools::FlushTimeTree();
				}
			});

		{
			PERFORMANCETAG("Engine run");
			engine.BeginFrame(clearColor);
		}


		{
			PERFORMANCETAG("Main loop");
			diagram.Update();
			diagram.Render();

			{
				PERFORMANCETAG("Imgui Drawing [old]");
				ImGui::Render();
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			}
			{
				PERFORMANCETAG("End frame [old]");
				engine.EndFrame();
			}
		}
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();


	return EXIT_SUCCESS;
}

int Error()
{
	AttachConsole();
	std::cout << "Invalid arguments" << std::endl << std::endl;
	std::cout << _EXE_NAME " <Filepath>" << std::endl << std::endl;
	system("pause");

	return EXIT_FAILURE;
}

#include <PAVPvar.h>
#include <PAVPcallback.h>
#include <thread>
#include <chrono>

void InitUISystem()
{
	ui = new pavp::UISystem();
	int wndW, wndH;
	wnd->GetWindowSize(&wndW, &wndH);
	pavp::TexParam tp = {
		GL_CLAMP_TO_BORDER,
		GL_CLAMP_TO_BORDER,
		GL_LINEAR,
		GL_LINEAR
	};
	pavp::Color themeColor = { 1.0f, 1.0f, 1.0f, 0.5f };
	pavp::Color blockColor = { 0.0, 0.0f, 0.0f, 0.5f };
	pavp::Color barColor = { 0.f, 0.505882f, 1.f, 0.5f };

	pavp::BitMap* playIcon = new pavp::BitMap(PLAY_IMAGE);
	pavp::BitMap* pauseIcon = new pavp::BitMap(PAUSE_IMAGE);
	playtex = new pavp::Texture(playIcon, tp);
	pausetex = new pavp::Texture(pauseIcon, tp);
	delete playIcon;
	delete pauseIcon;
	playBtn = ui->set(new pavp::IconButton(wndW, wndH, wndW / 2 - 25, wndH - 75, 50, 50, -0.5f, playtex->get(), themeColor));
	float* pBbuffer = ui->get(playBtn)->Buffer();
	pBbuffer[7] = pBbuffer[16] = -0.05f; //left
	pBbuffer[25] = pBbuffer[34] = 1.05f; //right
	pBbuffer[8] = pBbuffer[35] = 1.05f; //top
	pBbuffer[17] = pBbuffer[26] = -0.05f; //bottom
	ui->get(playBtn)->UpdataBuffer();
	ui->get(playBtn)->BindCallback(playBtnMBCallback);
	ui->get(playBtn)->BindCallback(playBtnCPCallback);
	ui->get(playBtn)->BindCallback(midSZCallback);

	pavp::BitMap* videoIcon = new pavp::BitMap(VIDEO_IMAGE);
	videotex = new pavp::Texture(videoIcon, tp);
	delete videoIcon;
	videoBtn = ui->set(new pavp::IconButton(wndW, wndH, 25, wndH - 75, 50, 50, -0.5f, videotex->get(), themeColor));
	float* vBbuffer = ui->get(videoBtn)->Buffer();
	vBbuffer[7] = vBbuffer[16] = -0.05f; //left
	vBbuffer[25] = vBbuffer[34] = 1.05f; //right
	vBbuffer[8] = vBbuffer[35] = 1.05f; //top
	vBbuffer[17] = vBbuffer[26] = -0.05f; //bottom
	ui->get(videoBtn)->UpdataBuffer();
	ui->get(videoBtn)->BindCallback(videoBtnMBCallback);
	ui->get(videoBtn)->BindCallback(videoBtnCPCallback);
	ui->get(videoBtn)->BindCallback(leftSZCallback);

	pavp::BitMap* shaderIcon = new pavp::BitMap(SHADER_IMAGE);
	shadertex = new pavp::Texture(shaderIcon, tp);
	delete shaderIcon;
	shaderBtn = ui->set(new pavp::IconButton(wndW, wndH, 100, wndH - 75, 50, 50, -0.5f, shadertex->get(), themeColor));
	float* sBbuffer = ui->get(shaderBtn)->Buffer();
	sBbuffer[7] = sBbuffer[16] = -0.05f; //left
	sBbuffer[25] = sBbuffer[34] = 1.05f; //right
	sBbuffer[8] = sBbuffer[35] = -0.05f; //top
	sBbuffer[17] = sBbuffer[26] = 1.05f; //bottom
	ui->get(shaderBtn)->UpdataBuffer();
	ui->get(shaderBtn)->BindCallback(shaderBtnMBCallback);
	ui->get(shaderBtn)->BindCallback(shaderBtnCPCallback);
	ui->get(shaderBtn)->BindCallback(leftSZCallback);

	pavp::BitMap* fullIcon = new pavp::BitMap(FULL_IMAGE);
	pavp::BitMap* windowIcon = new pavp::BitMap(WINDOW_IMAGE);
	fulltex = new pavp::Texture(fullIcon, tp);
	windowtex = new pavp::Texture(windowIcon, tp);
	delete fullIcon;
	delete windowIcon;
	fullBtn = ui->set(new pavp::IconButton(wndW, wndH, wndW - 75, wndH - 75, 50, 50, -0.5f, fulltex->get(), themeColor));
	float* fBbuffer = ui->get(fullBtn)->Buffer();
	fBbuffer[7] = fBbuffer[16] = -0.05f; //left
	fBbuffer[25] = fBbuffer[34] = 1.05f; //right
	fBbuffer[8] = fBbuffer[35] = 1.05f; //top
	fBbuffer[17] = fBbuffer[26] = -0.05f; //bottom
	ui->get(fullBtn)->UpdataBuffer();
	ui->get(fullBtn)->BindCallback(fullBtnMBCallback);
	ui->get(fullBtn)->BindCallback(fullBtnCPCallback);
	ui->get(fullBtn)->BindCallback(rightSZCallback);

	pavp::BitMap* barbmp = new pavp::BitMap(BAR_IMAGE);
	bartex = new pavp::Texture(barbmp, tp);
	delete barbmp;
	bar = ui->set(new pavp::Bar(wndW, wndH, 50, wndH - 100, wndW - 100, 10, 10, -0.5f, 0.0f, bartex->get(), barColor));
	ui->get(bar)->BindCallback(barMBCallback);
	ui->get(bar)->BindCallback(barCPCallback);
	ui->get(bar)->BindCallback(barSZCallback);

	pavp::BitMap* blackbmp = new pavp::BitMap(BLACK_IMAGE);
	blacktex = new pavp::Texture(blackbmp, tp);
	delete blackbmp;
	block = ui->set(new pavp::IconButton(wndW, wndH, 0, wndH * 3 / 4, wndW, wndH / 4, 0.0f, blacktex->get(), blockColor));
	float* bBbuffer = ui->get(block)->Buffer();
	bBbuffer[6] = bBbuffer[33] = 0.0f;
	ui->get(block)->UpdataBuffer();
	ui->get(block)->BindCallback(blockSZCallback);

	ui->make();
}

void PlayLoop()
{
	av = new pavp::AudioVideo(avName.c_str());
	av->Ready();
	play = false;
	while (!av->avSig.exit && !av->avSig.eof && !wnd->ShouldClose())
	{
		curtTime = glfwGetTime();
		deltaTime = curtTime - lastTime;
		lastTime = curtTime;
		if (play)
			break;
		wnd->PollEvents();
		wnd->Clear(GL_COLOR_BUFFER_BIT);

		DWORD delay = av->Render();
		if (uiHide > 0. || cursorInUI)
		{
			uiHide -= deltaTime;
			ui->render();
		}
		else if (!wnd->CursorIsHide())
		{
			wnd->HideCursor();
		}
		wnd->Refresh();
		DWORD tmp = DWORD((glfwGetTime() - curtTime) * 1000.0);
		delay -= delay > tmp ? tmp : delay;
		if (delay > 100)
			delay = 41;
		if(delay)
			std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		if (!ui->get(bar)->IsPress())
		{
			if (!av->avSig.seek)
			{
				((pavp::Bar*)ui->get(bar))->SetPercent(av->GetPercent());
				ui->get(bar)->UpdataBuffer();
			}
		}
	}
	ui->get(playBtn)->SetIcon(playtex->get());
	glBindTexture(GL_TEXTURE_2D, playtex->get());
	av->Exit();
	delete av;
	av = nullptr;
}

int main(void)
{
	wnd = new pavp::Window(1024, 576, "PAVP2");

	InitUISystem();
	nativeShader = new pavp::Shader(nullptr, avCanvasShaderV, nullptr, avCanvasShaderF);
	loadedShader = nativeShader;

	wnd->SetClearColor(0.0, 0.0, 0.0);
	wnd->SetWindowSizeCallback(WindowSizeCallback);
	wnd->SetWindowPosCallback(WindowPosCallback);
	wnd->SetKeyCallback(KeyCallback);
	wnd->SetCursorPosCallback(CursorPosCallback);
	wnd->SetMouseButtonCallback(MouseButtonCallback);
	wnd->SetSizelimit(800, 600, GLFW_DONT_CARE, GLFW_DONT_CARE);
	while (!wnd->ShouldClose())
	{
		curtTime = glfwGetTime();
		deltaTime = curtTime - lastTime;
		lastTime = curtTime;

		wnd->PollEvents();
		wnd->Clear(GL_COLOR_BUFFER_BIT);

		if (play)
			PlayLoop();

		if (uiHide > 0. || cursorInUI)
		{
			uiHide -= deltaTime;
			ui->render();
		}
		else if (!wnd->CursorIsHide())
		{
			wnd->HideCursor();
		}
		wnd->Refresh();
	}
}
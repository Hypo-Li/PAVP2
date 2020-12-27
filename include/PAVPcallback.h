#pragma once
#include <PAVPui.h>
#include <PAVPbase.h>
#include <PAVPwindow.h>
/**
* IconButton Buffer
*     pos            color       coords
* [00][01][02] [03][04][05][06] [07][08] lefttop
* [09][10][11] [12][13][14][15] [16][17] leftbottom
* [18][19][20] [21][22][23][24] [25][26] rightbottom
* [27][28][29] [30][31][32][33] [34][35] righttop
*/
//====================playBtn====================
void playBtnMBCallback(pavp::Widget* w, int button, int action)
{
	//std::cout << "button\n";
	float* buffer = w->Buffer();
	if (action == PAVP_UI_PRESS)
	{
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 1.0f;
		w->SetPress(true);
	}
	else
	{
		if (!w->IsPress())
			return;
		if (w->GetIcon() == playtex->get())
		{
			//暂停->播放
			w->SetIcon(pausetex->get());
			glBindTexture(GL_TEXTURE_2D, pausetex->get());
			av->Pause(false);
		}
		else
		{
			//播放->暂停
			w->SetIcon(playtex->get());
			glBindTexture(GL_TEXTURE_2D, playtex->get());
			av->Pause(true);
		}
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border1);
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 0.7f;
		w->SetPress(false);
	}
	w->UpdataBuffer();
}

void playBtnCPCallback(pavp::Widget* w, float x, float y, int action)
{
	GLuint icon = w->GetIcon();
	glBindTexture(GL_TEXTURE_2D, icon);
	float* buffer = w->Buffer();
	if (action == PAVP_UI_MOVE_IN)
	{
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border1);
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 0.7f;
	}
	else
	{
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border0);
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 0.5f;
	}
	w->UpdataBuffer();
}

void midSZCallback(pavp::Widget* w, float wmul, float hmul)
{
	float* buffer = w->Buffer();
	float uiw = buffer[18] - buffer[0];
	float uih = buffer[1] - buffer[10];
	uiw *= wmul / 2.0f;
	uih *= hmul;
	buffer[0] = buffer[9] = -uiw; //left
	buffer[18] = buffer[27] = uiw; //right
	buffer[1] = buffer[28] = (buffer[1] + 1.0f) * hmul -1.0f; //top
	buffer[10] = buffer[19] = buffer[1] - uih; //bottom
	w->UpdataBuffer();
}
//====================!playBtn====================

//====================videoBtn====================
static pavp::Filter videoType[] = {
	{L"视频文件", L"*.mp4;*.mkv;*."}
};
void videoBtnMBCallback(pavp::Widget* w, int button, int action)
{
	float* buffer = w->Buffer();
	if (action == PAVP_UI_PRESS)
	{
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 1.0f;
		w->SetPress(true);
	}
	else
	{
		if (!w->IsPress())
			return;
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 0.7f;
		w->SetPress(false);
		if (wnd->GetOpenFile(&avName, videoType, 1))
		{
			play = true;
			ui->get(playBtn)->SetIcon(playtex->get());
			glBindTexture(GL_TEXTURE_2D, playtex->get());
			((pavp::Bar*)ui->get(bar))->SetPercent(0.0f);
			ui->get(bar)->UpdataBuffer();
		}
	}
	w->UpdataBuffer();
}

void videoBtnCPCallback(pavp::Widget* w, float x, float y, int action)
{
	glBindTexture(GL_TEXTURE_2D, videotex->get());
	float* buffer = w->Buffer();
	if (action == PAVP_UI_MOVE_IN)
	{
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border1);
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 0.7f;
	}
	else
	{
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border0);
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 0.5f;
	}
	w->UpdataBuffer();
}

void leftSZCallback(pavp::Widget* w, float wmul, float hmul)
{
	float* buffer = w->Buffer();
	float uiw = buffer[18] - buffer[0];
	float uih = buffer[1] - buffer[10];
	uiw *= wmul;
	uih *= hmul;
	buffer[0] = buffer[9] = (buffer[0] + 1.0f) * wmul - 1.0f; //left
	buffer[18] = buffer[27] = buffer[0] + uiw;
	buffer[1] = buffer[28] = (buffer[1] + 1.0f) * hmul - 1.0f; //top
	buffer[10] = buffer[19] = buffer[1] - uih; //bottom
	w->UpdataBuffer();
}
//====================!videoBtn====================

//====================shaderBtn====================
static pavp::Filter shaderType[] = {
	{L"glsl文件", L"*.glsl;"}
};
void shaderBtnMBCallback(pavp::Widget* w, int button, int action)
{
	float* buffer = w->Buffer();
	if (action == PAVP_UI_PRESS)
	{
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 1.0f;
		w->SetPress(true);
	}
	else
	{
		if (!w->IsPress())
			return;
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 0.7f;
		w->SetPress(false);
		if (wnd->GetOpenFile(&shaderName, shaderType, 1))
		{
			pavp::Shader* tmpShader = new pavp::Shader(nullptr, avCanvasShaderV, shaderName.c_str(), nullptr);
			if (tmpShader->CheckShader())
			{
				if (loadedShader != nativeShader)
					delete loadedShader;
				loadedShader = tmpShader;
				iFrameLoc = loadedShader->GetUniformLoc("iFrame");
				iSizeLoc = loadedShader->GetUniformLoc("iSize");
				iTimeLoc = loadedShader->GetUniformLoc("iTime");
				iTextureLoc[0] = loadedShader->GetUniformLoc("iTexture0");
				iTextureLoc[1] = loadedShader->GetUniformLoc("iTexture1");
				iTextureLoc[2] = loadedShader->GetUniformLoc("iTexture2");
				iTextureLoc[3] = loadedShader->GetUniformLoc("iTexture3");
				loadedShader->Use();
				if (av)
				{
					loadedShader->SetVec2(iSizeLoc, av->avCtx.viewWidth, av->avCtx.viewHeight);
				}
				for (BYTE i = 0; i < 4; ++i)
				{
					if (iTexture[i])
						delete iTexture[i];
					if (tmpTexture[i])
					{
						iTexture[i] = tmpTexture[i];
						loadedShader->SetTexture2D(iTextureLoc[i], i + 1, iTexture[i]->get());
					}
				}
			}
			else
			{
				delete tmpShader;
			}
		}
	}
	w->UpdataBuffer();
}

void shaderBtnCPCallback(pavp::Widget* w, float x, float y, int action)
{
	glBindTexture(GL_TEXTURE_2D, shadertex->get());
	float* buffer = w->Buffer();
	if (action == PAVP_UI_MOVE_IN)
	{
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border1);
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 0.7f;
	}
	else
	{
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border0);
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 0.5f;
	}
	w->UpdataBuffer();
}
//====================!shaderBtn====================

//====================bar====================
void barMBCallback(pavp::Widget* w, int button, int action)
{
	float xpos, ypos;
	wnd->GetCursorPos(&xpos, &ypos);
	float* buffer = w->Buffer();
	float percent = (xpos - buffer[0]) / (buffer[36] - buffer[0]);
	if (action == PAVP_UI_PRESS)
	{
		if(xpos < buffer[0])
			((pavp::Bar*)w)->SetPercent(0.0f);
		else if(xpos > buffer[36])
			((pavp::Bar*)w)->SetPercent(1.0f);
		else
			((pavp::Bar*)w)->SetPercent(percent);
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = buffer[42] = buffer[51] = buffer[60] = buffer[69] = 1.0f;
		w->SetPress(true);
	}
	else {
		if(w->In(xpos, ypos))
			buffer[6] = buffer[15] = buffer[24] = buffer[33] = buffer[42] = buffer[51] = buffer[60] = buffer[69] = 0.7f;
		else
			buffer[6] = buffer[15] = buffer[24] = buffer[33] = buffer[42] = buffer[51] = buffer[60] = buffer[69] = 0.5f;
		w->SetPress(false);
		av->Seek(((pavp::Bar*)w)->GetPercent());
	}
	w->UpdataBuffer();
}

void barCPCallback(pavp::Widget* w, float x, float y, int action)
{
	glBindTexture(GL_TEXTURE_2D, bartex->get());
	float* buffer = w->Buffer();
	if (action == PAVP_UI_MOVE_IN)
	{
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = buffer[42] = buffer[51] = buffer[60] = buffer[69] = 0.7f;
	}
	else
	{
		if(w->IsPress())
			buffer[6] = buffer[15] = buffer[24] = buffer[33] = buffer[42] = buffer[51] = buffer[60] = buffer[69] = 1.0f;
		else
			buffer[6] = buffer[15] = buffer[24] = buffer[33] = buffer[42] = buffer[51] = buffer[60] = buffer[69] = 0.5f;
	}
	w->UpdataBuffer();
}

void barSZCallback(pavp::Widget* w, float wmul, float hmul)
{
	float* buffer = w->Buffer();
	float uiw = buffer[36] - buffer[0];
	float uih = buffer[1] - buffer[10];
	float bw = buffer[27] - buffer[18];
	float percent = (buffer[18] + bw / 2.0f - buffer[0]) / uiw;
	uih *= hmul;
	buffer[1] = buffer[46] = buffer[55] = buffer[64] = (buffer[1] + 1.0f) * hmul - 1.0f; //top
	buffer[10] = buffer[19] = buffer[28] = buffer[37] = buffer[1] - uih;
	buffer[0] = buffer[9] = (buffer[0] + 1.0f) * wmul - 1.0f; //left
	buffer[36] = buffer[45] = 1.0f - (1.0f - buffer[36]) * wmul; //right
	((pavp::Bar*)w)->SetPercent(percent);
	w->UpdataBuffer();
}
//====================!bar====================

//====================fullBtn====================
void fullBtnMBCallback(pavp::Widget* w, int button, int action)
{
	float* buffer = w->Buffer();
	if (action == PAVP_UI_PRESS)
	{
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 1.0f;
		w->SetPress(true);
	}
	else
	{
		if (!w->IsPress())
			return;
		if (w->GetIcon() == fulltex->get())
		{
			w->SetIcon(windowtex->get());
			glBindTexture(GL_TEXTURE_2D, windowtex->get());
			wnd->SetFullScreen();
		}
		else
		{
			w->SetIcon(fulltex->get());
			glBindTexture(GL_TEXTURE_2D, fulltex->get());
			wnd->SetWindowing();
		}
		float xpos, ypos;
		wnd->GetCursorPos(&xpos, &ypos);
		if (w->In(xpos, ypos))
		{
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border1);
			buffer[6] = buffer[15] = buffer[24] = buffer[33] = 0.7f;
		}
		else {
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border0);
			buffer[6] = buffer[15] = buffer[24] = buffer[33] = 0.5f;
		}
		
		w->SetPress(false);
	}
	w->UpdataBuffer();
}

void fullBtnCPCallback(pavp::Widget* w, float x, float y, int action)
{
	GLuint icon = w->GetIcon();
	glBindTexture(GL_TEXTURE_2D, icon);
	float* buffer = w->Buffer();
	if (action == PAVP_UI_MOVE_IN)
	{
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border1);
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 0.7f;
	}
	else
	{
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border0);
		buffer[6] = buffer[15] = buffer[24] = buffer[33] = 0.5f;
	}
	w->UpdataBuffer();
}

void rightSZCallback(pavp::Widget* w, float wmul, float hmul)
{
	float* buffer = w->Buffer();
	float uiw = buffer[18] - buffer[0];
	float uih = buffer[1] - buffer[10];
	uiw *= wmul;
	uih *= hmul;
	buffer[18] = buffer[27] = 1.0f - (1.0f - buffer[18]) * wmul; //right
	buffer[0] = buffer[9] = buffer[18] - uiw;
	buffer[1] = buffer[28] = (buffer[1] + 1.0f) * hmul - 1.0f; //top
	buffer[10] = buffer[19] = buffer[1] - uih; //bottom
	w->UpdataBuffer();
}
//====================!fullBtn====================

//====================block====================
void blockSZCallback(pavp::Widget* w, float wmul, float hmul)
{
	float* buffer = w->Buffer();
	float uih = buffer[1] - buffer[10];
	buffer[1] = buffer[28] = (buffer[1] + 1.0f) * hmul - 1.0f; //top
	w->UpdataBuffer();
}
//====================!block====================

void KeyCallback(GLFWwindow* w, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			fullBtnMBCallback(ui->get(fullBtn), PAVP_UI_MB_LEFT, PAVP_UI_PRESS);
			fullBtnMBCallback(ui->get(fullBtn), PAVP_UI_MB_LEFT, PAVP_UI_RELEASE);
			break;
		case GLFW_KEY_SPACE:
			playBtnMBCallback(ui->get(playBtn), PAVP_UI_MB_LEFT, PAVP_UI_PRESS);
			playBtnMBCallback(ui->get(playBtn), PAVP_UI_MB_LEFT, PAVP_UI_RELEASE);
			break;
		default:
			break;
		}
	}
}

void WindowSizeCallback(GLFWwindow* w, int width, int height)
{
	if (width == 0 || height == 0)
		return;
	float ww, wh;
	wnd->GetWindowSize(&ww, &wh);
	float nww = float(width), nwh = float(height);
	ui->callsize(ww / nww, wh / nwh);
	wnd->SetViewport(0, 0, width, height);
	wnd->UpdataSize(width, height);
	if (av)
		av->ViewportCanvas();
}

void WindowPosCallback(GLFWwindow* w, int xpos, int ypos)
{
	wnd->UpdataPos(xpos, ypos);
}

static DWORD lastInUI = PAVP_UI_NULL;

void CursorPosCallback(GLFWwindow* w, double xpos, double ypos)
{
	//如果bar的按钮正被按下，则调用bar的按钮回调函数
	if (ui->get(bar)->IsPress())
		barMBCallback(ui->get(bar), PAVP_UI_MB_LEFT, PAVP_UI_PRESS);

	uiHide = UI_SHOW_TIME;
	if (wnd->CursorIsHide())
		wnd->ShowCursor();
	float hww, hwh;
	wnd->GetWindowSize(&hww, &hwh);
	hww /= 2.0f;
	hwh /= 2.0f;
	float newx = xpos / hww - 1.0f;
	float newy = 1.0f - ypos / hwh;
	DWORD inui = ui->which(newx, newy);
	if (inui != lastInUI)
	{
		if (inui != PAVP_UI_NULL)
		{
			cursorInUI = true;
			ui->get(inui)->cpCallback(ui->get(inui), newx, newy, PAVP_UI_MOVE_IN);
		}
		else
			cursorInUI = false;
		if(lastInUI != PAVP_UI_NULL)
		{
			ui->get(lastInUI)->cpCallback(ui->get(lastInUI), newx, newy, PAVP_UI_MOVE_OUT);
		}
		lastInUI = inui;
	}
}

void MouseButtonCallback(GLFWwindow* w, int button, int action, int mods)
{
	//避免按钮释放时光标不在bar内导致无法释放的问题
	if (action == GLFW_RELEASE && ui->get(bar)->IsPress())
		barMBCallback(ui->get(bar), PAVP_UI_MB_LEFT, PAVP_UI_RELEASE);

	float xpos, ypos;
	wnd->GetCursorPos(&xpos, &ypos);
	DWORD inui = ui->which(xpos, ypos);
	if (inui != PAVP_UI_NULL)
	{
		ui->get(inui)->mbCallback(ui->get(inui), button, action);
	}
}
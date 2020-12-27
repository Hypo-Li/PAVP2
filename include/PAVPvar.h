#pragma once
#include <PAVPwindow.h>
#include <PAVPbuffer.h>
#include <PAVPui.h>
#include <PAVPshader.h>
#include <PAVPav.h>
#ifdef _DEBUG
#define PLAY_IMAGE "./res/ui/play.png"
#define PAUSE_IMAGE "./res/ui/pause.png"
#define BLACK_IMAGE "./res/ui/black.png"
#define VIDEO_IMAGE "./res/ui/video.png"
#define SHADER_IMAGE "./res/ui/shader.png"
#define FULL_IMAGE "./res/ui/full.png"
#define WINDOW_IMAGE "./res/ui/window.png"
#define BAR_IMAGE "./res/ui/bar.png"
#else
#define PLAY_IMAGE "../res/ui/play.png"
#define PAUSE_IMAGE "../res/ui/pause.png"
#define BLACK_IMAGE "../res/ui/black.png"
#define VIDEO_IMAGE "../res/ui/video.png"
#define SHADER_IMAGE "../res/ui/shader.png"
#define FULL_IMAGE "../res/ui/full.png"
#define WINDOW_IMAGE "../res/ui/window.png"
#define BAR_IMAGE "../res/ui/bar.png"
#endif

extern pavp::Window* wnd;
extern pavp::UISystem* ui;
extern pavp::AudioVideo* av;

extern DWORD playBtn;
extern DWORD videoBtn;
extern DWORD shaderBtn;
extern DWORD fullBtn;
extern DWORD bar;
extern DWORD block;

extern pavp::Texture* playtex;
extern pavp::Texture* pausetex;
extern pavp::Texture* videotex;
extern pavp::Texture* shadertex;
extern pavp::Texture* fulltex;
extern pavp::Texture* windowtex;
extern pavp::Texture* blacktex;
extern pavp::Texture* bartex;

extern GLfloat border0[4];
extern GLfloat border1[4];

extern double lastTime;
extern double curtTime;
extern double deltaTime;

extern bool cursorInUI;
extern double uiHide;
#define UI_SHOW_TIME 3.0

extern bool play;
extern std::string avName;
extern std::string shaderName;
extern pavp::Shader* nativeShader;
extern pavp::Shader* loadedShader;

extern GLint iFrameLoc;
extern GLint iSizeLoc;
extern GLint iTimeLoc;
extern GLint iTextureLoc[];
extern pavp::Texture* iTexture[];
extern pavp::Texture* tmpTexture[];

extern char avCanvasShaderV[];
extern char avCanvasShaderF[];
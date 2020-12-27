#include <PAVPvar.h>

pavp::Window* wnd = nullptr;
pavp::UISystem* ui = nullptr;
pavp::AudioVideo* av = nullptr;

DWORD playBtn;
DWORD videoBtn;
DWORD shaderBtn;
DWORD fullBtn;
DWORD bar;
DWORD block;

pavp::Texture* playtex = nullptr;
pavp::Texture* pausetex = nullptr;
pavp::Texture* videotex = nullptr;
pavp::Texture* shadertex = nullptr;
pavp::Texture* fulltex = nullptr;
pavp::Texture* windowtex = nullptr;
pavp::Texture* blacktex = nullptr;
pavp::Texture* bartex = nullptr;

GLfloat border0[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat border1[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

double lastTime = 0.0;
double curtTime = 0.0;
double deltaTime = 0.0;
double lastapts = 0.0;

bool cursorInUI = false;
double uiHide = UI_SHOW_TIME;

bool play = false;
std::string avName;
std::string shaderName;
pavp::Shader* nativeShader;
pavp::Shader* loadedShader;

GLint iFrameLoc;
GLint iSizeLoc;
GLint iTimeLoc;
GLint iTextureLoc[4] = { -1 };
pavp::Texture* iTexture[4] = { nullptr };
pavp::Texture* tmpTexture[4] = { nullptr };

char avCanvasShaderV[] =
"#version 330 core\n"
"layout (location = 0) in vec2 ipos;\n"
"layout (location = 1) in vec2 icoords;\n"
"out vec2 iUV;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(ipos.x, ipos.y, 0.5f, 1.0);\n"
"    iUV = icoords;\n"
"}\0";

char avCanvasShaderF[] =
"#version 330 core\n"
"in vec2 iUV;\n"
"uniform sampler2D iFrame;\n"
"uniform int iWidth;\n"
"uniform int iHeight;\n"
"uniform float iTime;\n"
"uniform sampler2D iTexture0;\n"
"uniform sampler2D iTexture1;\n"
"uniform sampler2D iTexture2;\n"
"uniform sampler2D iTexture3;\n"
"out vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = texture(iFrame, iUV);\n"
"}\0";
#include <PAVPvar.h>
#define VERTEX_ERROR 0
#define FRAGMENT_ERROR 1
#define LINK_ERROR 2

void pavp::Shader::ProcessTex(const char* shaderCode)
{
	std::string code(shaderCode);
	std::string texKeyWord[] = {
		"//iTexture0",
		"//iTexture1",
		"//iTexture2",
		"//iTexture3"
	};
	size_t pos;
	char texName[255] = { 0 };
	pavp::TexParam tp = {
		GL_REPEAT,
		GL_REPEAT,
		GL_LINEAR,
		GL_LINEAR
	};
	for (BYTE i = 0; i < 4; ++i)
	{
		WORD j = 0;
		pos = code.find(texKeyWord[i]);
		if (pos != code.npos)
		{
			pos += 12;
			while (code[pos] != ';')
				texName[j++] = code[pos++];
			pavp::BitMap* bmp = new pavp::BitMap(texName);
			tmpTexture[i] = nullptr;
			if (bmp->CheckBmp())
			{
				tmpTexture[i] = new pavp::Texture(bmp, tp);
			}
			delete bmp;
			memset(texName, 0, 255);
		}
	}
}

pavp::Shader::Shader(const char* vertexPath, const char* vertexStr, const char* fragmentPath, const char* fragmentStr):state(false)
{
	GLuint vertexShader, fragmentShader;
	pavp::AsciiText* vertexCode = nullptr;
	pavp::AsciiText* fragmentCode = nullptr;
	const char* vc;
	const char* fc;
	if (vertexPath) //path参数不为空，从文件读取源码
	{
		vertexCode = new pavp::AsciiText(vertexPath);
		vc = vertexCode->cstr();
	}
	else
	{
		vc = vertexStr;
	}

	if (fragmentPath) //path参数不为空，从文件读取源码
	{
		fragmentCode = new pavp::AsciiText(fragmentPath);
		if (!fragmentCode->CheckText())
			return;
		fc = fragmentCode->cstr();
	}
	else
	{
		fc = fragmentStr;
	}

	ProcessTex(fc);

	state = true;

	//创建着色器
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vertexShader, 1, &vc, NULL);
	glShaderSource(fragmentShader, 1, &fc, NULL);

	//编译
	glCompileShader(vertexShader);
	CheckError(vertexShader, true);
	glCompileShader(fragmentShader);
	CheckError(fragmentShader, true);

	//链接
	this->id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);

	glLinkProgram(id);
	CheckError(id, false);

	//删除shader
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	if (vertexPath)
		delete vertexCode;
	if (fragmentPath)
		delete fragmentCode;
}

pavp::Shader::~Shader()
{
	glDeleteProgram(id);
}

void pavp::Shader::CheckError(GLuint checkID, int type)
{
	int success;
	char infolog[512];
	if (type != LINK_ERROR) {
		glGetShaderiv(checkID, GL_COMPILE_STATUS, &success);
		if (!success) {
			state = false;
			glGetShaderInfoLog(checkID, 512, NULL, infolog);
			if(type == VERTEX_ERROR)
				std::cerr << "Shader Vertex Error: " << infolog << std::endl;
			else
				std::cerr << "Shader Fragment Error: " << infolog << std::endl;
		}
	}
	else {
		glGetProgramiv(checkID, GL_LINK_STATUS, &success);
		if (!success) {
			state = false;
			glGetProgramInfoLog(checkID, 512, NULL, infolog);
			std::cerr <<"Shader Link Error: "<< infolog << std::endl;
		}
	}
}
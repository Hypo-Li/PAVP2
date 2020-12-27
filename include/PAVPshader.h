#pragma once
#include <glad/glad.h>
#include <PAVPbuffer.h>

namespace pavp
{
	class Shader;
}

class pavp::Shader
{
public:
	Shader():id(0){}

	Shader(const char* vertexPath, const char* vertexStr, const char* fragmentPath, const char* fragmentStr);

	~Shader();

	GLint GetUniformLoc(const char* uniform)
	{
		return glGetUniformLocation(id, uniform);
	}

	void Use()
	{
		glUseProgram(id);
	}

	void Delete()
	{
		glDeleteProgram(id);
	}

	void SetBool(GLint loc, bool value)
	{
		glUniform1i(loc, value);
	}

	void SetInt(GLint loc, int value)
	{
		glUniform1i(loc, value);
	}

	void SetFloat(GLint loc, float value)
	{
		glUniform1f(loc, value);
	}

	void SetVec2(GLint loc, float x, float y)
	{
		glUniform2f(loc, x, y);
	}

	void SetVec3(GLint loc, float x, float y, float z)
	{
		glUniform3f(loc, x, y, z);
	}

	void SetTexture2D(GLint loc, GLuint num, GLuint texID)
	{
		glActiveTexture(GL_TEXTURE0 + num);
		glBindTexture(GL_TEXTURE_2D, texID);
		glUniform1i(loc, num);
	}

	bool CheckShader()
	{
		return state;
	}

private:
	GLuint id;
	
	bool state;

	void CheckError(GLuint checkID, int type);

	void ProcessTex(const char* shaderCode);
};
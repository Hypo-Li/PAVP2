#pragma once
#include <PAVPbase.h>
#include <iostream>
#include <array>
#include <glad/glad.h>

namespace pavp
{
	class AsciiText;
	class BitMap;
	struct TexParam;
	class Texture;
}

class pavp::AsciiText
{
public:
	AsciiText();

	AsciiText(const char* path);

	~AsciiText();

	const char* cstr()
	{
		return text;
	}

	char* str()
	{
		return text;
	}

	bool CheckText()
	{
		return len;
	}

private:
	char* text;
	size_t len;
};

class pavp::BitMap
{
public:
	BitMap(){}

	BitMap(const char* path);

	~BitMap();

	BYTE* bit()
	{
		return this->data;
	}

	DWORD w()
	{
		return this->width;
	}

	DWORD h()
	{
		return this->height;
	}

	DWORD c()
	{
		return this->channels;
	}

	bool CheckBmp()
	{
		return this->data;
	}

private:
	BYTE* data;
	DWORD width, height, channels;
};

struct pavp::TexParam
{
	GLint wrap_s; //�����Ʒ�ʽ
	GLint wrap_t; //�����Ʒ�ʽ
	GLint min_filter; //��С���˷�ʽ
	GLint mag_filter; //�Ŵ���˷�ʽ
};

class pavp::Texture
{
public:
	Texture(BitMap* bm, TexParam& tp);

	~Texture();

	GLuint get()
	{
		return id;
	}

private:
	GLuint id;
};
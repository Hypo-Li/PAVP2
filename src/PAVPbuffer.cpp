#include <PAVPbuffer.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>

pavp::AsciiText::AsciiText(const char* path):len(0), text(nullptr)
{
	FILE* fp = fopen(path, "rb");
	if (!fp)
		return;
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	text = (char*)malloc(len + 1);
	fread(text, len, 1, fp);
	text[len] = 0;
	fclose(fp);
}

pavp::AsciiText::~AsciiText()
{
	if (!text)
		return;
	free(text);
	text = nullptr;
}

pavp::BitMap::BitMap(const char* path)
{
	int w, h, c;
	this->data = stbi_load(path, &w, &h, &c, 0);
	if (!data)
	{
		std::cerr << "image load error: " << path << std::endl;
		this->data = nullptr;
		return;
	}
	this->width = w;
	this->height = h;
	this->channels = c;
}

pavp::BitMap::~BitMap()
{
	stbi_image_free(this->data);
}

pavp::Texture::Texture(BitMap* bm, TexParam& tp)
{
	GLenum format;
	switch (bm->c())
	{
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		format = GL_RED;
		break;
	}
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tp.wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tp.wrap_t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tp.min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tp.mag_filter);
	glTexImage2D(GL_TEXTURE_2D, 0, format, bm->w(), bm->h(), 0, format, GL_UNSIGNED_BYTE, bm->bit());
	glBindTexture(GL_TEXTURE_2D, 0);
}

pavp::Texture::~Texture()
{
	glDeleteTextures(1, &id);
}
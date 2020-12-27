#include <PAVPui.h>

//==========class UISystem==========
static const char* UIVshader =
	"#version 330 core\n"
	"layout (location = 0) in vec3 ipos;\n"
	"layout (location = 1) in vec4 icolor;\n"
	"layout (location = 2) in vec2 icoords;\n"
	"out vec4 color;\n"
	"out vec2 uv;\n"
	"void main()\n"
	"{\n"
	"    gl_Position = vec4(ipos.x, ipos.y, ipos.z, 1.0);\n"
	"    color = icolor;\n"
	"    uv = icoords;\n"
	"}\0";

static const char* UIFshader =
	"#version 330 core\n"
	"in vec4 color;\n"
	"in vec2 uv;\n"
	"out vec4 fragColor;\n"
	"uniform sampler2D icon;\n"
	"void main()\n"
	"{\n"
	"    vec4 pixel = texture(icon, uv);\n"
	"    fragColor = vec4(color.rgb, pixel.a * color.a);\n"
	"}\0";

pavp::UISystem::UISystem()
{
	//创建shader
	this->shader = new Shader(nullptr, UIVshader, nullptr, UIFshader);
	this->iconLoc = this->shader->GetUniformLoc("icon");
	sorted = nullptr;
}

pavp::UISystem::~UISystem()
{
	delete this->shader;
	if (sorted)
		delete[] sorted;
}

DWORD pavp::UISystem::set(pavp::Widget* w)
{
	this->widgets.push_back(w);
	DWORD id = this->widgets.size() - 1;
	this->widgets[id]->id = id;
	return id;
}

void pavp::UISystem::render()
{
	DWORD size = widgets.size();
	shader->Use();
	for (DWORD i = 0; i < size; ++i)
	{
		if(!sorted[size - i - 1]->hide)
			sorted[size - i - 1]->Render(shader, iconLoc);
	}
}

bool pavp::UISystem::layercomp(pavp::Widget* a, pavp::Widget* b)
{
	return a->layer < b->layer;
}

void pavp::UISystem::make()
{
	std::vector<pavp::Widget*> temp = widgets;
	std::sort(temp.begin(), temp.end(), layercomp);
	DWORD n = widgets.size();
	sorted = new pavp::Widget * [n];
	for (DWORD i = 0; i < n; ++i)
	{
		sorted[i] = temp[i];
	}
}

DWORD pavp::UISystem::which(float xpos, float ypos)
{
	DWORD size = widgets.size();
	for (DWORD i = 0; i < size; ++i)
	{
		if (sorted[i]->In(xpos, ypos))
			return sorted[i]->id;
	}
	return PAVP_UI_NULL;
}

void pavp::UISystem::callsize(float wmul, float hmul)
{
	DWORD size = widgets.size();
	for (DWORD i = 0; i < size; ++i)
	{
		sorted[i]->szCallback(sorted[i], wmul, hmul);
	}
}
//==========!class UISystem==========

//==========class Widget==========
pavp::Widget::Widget()
{
	//类成员初始化
	press = hide = false;
	mbCallback = defMBCallback;
	cpCallback = defCPCallback;
	szCallback = defSZCallback;
}

pavp::Widget::~Widget()
{
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}
//==========!class Widget==========

//==========class IconButton==========
/**
* IconButton Buffer
*     pos            color       coords
* [00][01][02] [03][04][05][06] [07][08] lefttop
* [09][10][11] [12][13][14][15] [16][17] leftbottom
* [18][19][20] [21][22][23][24] [25][26] rightbottom
* [27][28][29] [30][31][32][33] [34][35] righttop
*/
static unsigned int IconButtonIndex[6] = { 0, 1, 2, 0, 2, 3 };
pavp::IconButton::IconButton(float left, float top, float width, float height, float layer, GLuint icon, Color& color)
{
	//创建缓冲
	CreateBufferObj();
	this->layer = layer;
	this->icon = icon;
	buffer[0] = buffer[9] = left;
	buffer[18] = buffer[27] = left + width;
	buffer[1] = buffer[28] = top;
	buffer[10] = buffer[19] = top - height;
	buffer[2] = buffer[11] = buffer[20] = buffer[29] = layer;
	buffer[3] = buffer[12] = buffer[21] = buffer[30] = color.r;
	buffer[4] = buffer[13] = buffer[22] = buffer[31] = color.g;
	buffer[5] = buffer[14] = buffer[23] = buffer[32] = color.b;
	buffer[6] = buffer[15] = buffer[24] = buffer[33] = color.a;
	buffer[7] = buffer[16] = buffer[17] = buffer[26] = 0.0f;
	buffer[8] = buffer[25] = buffer[34] = buffer[35] = 1.0f;
	UpdataBuffer();
}

pavp::IconButton::IconButton(int windowW, int windowH, int left, int top, int width, int height, float layer, GLuint icon, Color& color)
{
	//创建缓冲
	CreateBufferObj();
	this->layer = layer;
	this->icon = icon;
	float hww = float(windowW) / 2.0f;
	float hwh = float(windowH) / 2.0f;
	float l = float(left);
	float t = float(top);
	float w = float(width);
	float h = float(height);
	l = (l - hww) / hww;
	t = (hwh - t) / hwh;
	w = w / hww;
	h = h / hwh;
	buffer[0] = buffer[9] = l;
	buffer[18] = buffer[27] = l + w;
	buffer[1] = buffer[28] = t;
	buffer[10] = buffer[19] = t - h;
	buffer[2] = buffer[11] = buffer[20] = buffer[29] = layer;
	buffer[3] = buffer[12] = buffer[21] = buffer[30] = color.r;
	buffer[4] = buffer[13] = buffer[22] = buffer[31] = color.g;
	buffer[5] = buffer[14] = buffer[23] = buffer[32] = color.b;
	buffer[6] = buffer[15] = buffer[24] = buffer[33] = color.a;
	buffer[7] = buffer[16] = buffer[17] = buffer[26] = 0.0f;
	buffer[8] = buffer[25] = buffer[34] = buffer[35] = 1.0f;
	UpdataBuffer();
}

pavp::IconButton::~IconButton()
{

}

void pavp::IconButton::CreateBufferObj()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); //pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float))); //color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float))); //coords
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(IconButtonIndex), IconButtonIndex, GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void pavp::IconButton::UpdataBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(buffer), buffer);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

float* pavp::IconButton::Buffer()
{
	return this->buffer;
}

void pavp::IconButton::Render(pavp::Shader* shader, GLint iconLoc)
{
	glBindVertexArray(VAO);
	shader->SetTexture2D(iconLoc, 0, icon);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool pavp::IconButton::In(float xpos, float ypos)
{
	if (xpos >= buffer[0] && xpos <= buffer[18] && ypos >= buffer[10] && ypos <= buffer[1])
	{
		return true;
	}
	return false;
}
//==========!class IconButton==========

//==========class Bar==========
/**
* Bar Buffer
*     pos            color       coords
* [00][01][02] [03][04][05][06] [07][08] LeftTop
* [09][10][11] [12][13][14][15] [16][17] LeftBottom
* [18][19][20] [21][22][23][24] [25][26] BtnLeftBottom
* [27][28][29] [30][31][32][33] [34][35] BtnRightBottom
* [36][37][38] [39][40][41][42] [43][44] RightBottom
* [45][46][47] [48][49][50][51] [52][53] RightTop
* [54][55][56] [57][58][59][60] [61][62] BtnRightTop
* [63][64][65] [66][67][68][69] [70][71] BtnLeftTop
*/
static unsigned int BarIndex[18] = { 0, 1 , 2, 0, 2, 7, 7, 2, 3, 7, 3, 6, 6, 3, 4, 6, 4, 5 };
pavp::Bar::Bar(float left, float top, float width, float height, float btnw, float layer, float percent, GLuint icon, Color& color)
{
	//创建缓冲
	CreateBufferObj();
	this->layer = layer;
	this->icon = icon;
	buffer[0] = buffer[9] = left;
	buffer[36] = buffer[45] = left + width;
	buffer[1] = buffer[46] = buffer[55] = buffer[64] = top;
	buffer[10] = buffer[19] = buffer[28] = buffer[37] = top - height;
	buffer[18] = buffer[63] = left + width * percent - btnw / 2.0f;
	buffer[27] = buffer[54] = buffer[18] + btnw;
	buffer[2] = buffer[11] = buffer[20] = buffer[29] = buffer[38] = buffer[47] = buffer[56] = buffer[65] = layer;
	buffer[3] = buffer[12] = buffer[21] = buffer[30] = buffer[39] = buffer[48] = buffer[57] = buffer[66] = color.r;
	buffer[4] = buffer[13] = buffer[22] = buffer[31] = buffer[40] = buffer[49] = buffer[58] = buffer[67] = color.g;
	buffer[5] = buffer[14] = buffer[23] = buffer[32] = buffer[41] = buffer[50] = buffer[59] = buffer[68] = color.b;
	buffer[6] = buffer[15] = buffer[24] = buffer[33] = buffer[42] = buffer[51] = buffer[60] = buffer[69] = color.a;
	buffer[7] = buffer[16] = buffer[17] = buffer[26] = buffer[35] = buffer[44] = value = 0.0f;
	value = 0.0f;
	buffer[8] = buffer[43] = buffer[52] = buffer[53] = buffer[62] = buffer[71] = 1.0f;
	buffer[25] = buffer[70] = 0.33333333f;
	buffer[34] = buffer[61] = 0.66666666f;
	UpdataBuffer();
}

pavp::Bar::Bar(int windowW, int windowH, int left, int top, int width, int height, int btnw, float layer, float percent, GLuint icon, Color& color)
{
	//创建缓冲
	CreateBufferObj();
	this->layer = layer;
	this->icon = icon;
	float hww = float(windowW) / 2.0f;
	float hwh = float(windowH) / 2.0f;
	float l = float(left);
	float t = float(top);
	float w = float(width);
	float h = float(height);
	float bw = float(btnw) / hww;
	l = (l - hww) / hww;
	t = (hwh - t) / hwh;
	w = w / hww;
	h = h / hwh;
	buffer[0] = buffer[9] = l;
	buffer[36] = buffer[45] = l + w;
	buffer[1] = buffer[46] = buffer[55] = buffer[64] = t;
	buffer[10] = buffer[19] = buffer[28] = buffer[37] = t - h;
	buffer[18] = buffer[63] = l + w * percent - bw / 2.0f;
	buffer[27] = buffer[54] = buffer[18] + bw;
	buffer[2] = buffer[11] = buffer[20] = buffer[29] = buffer[38] = buffer[47] = buffer[56] = buffer[65] = layer;
	buffer[3] = buffer[12] = buffer[21] = buffer[30] = buffer[39] = buffer[48] = buffer[57] = buffer[66] = color.r;
	buffer[4] = buffer[13] = buffer[22] = buffer[31] = buffer[40] = buffer[49] = buffer[58] = buffer[67] = color.g;
	buffer[5] = buffer[14] = buffer[23] = buffer[32] = buffer[41] = buffer[50] = buffer[59] = buffer[68] = color.b;
	buffer[6] = buffer[15] = buffer[24] = buffer[33] = buffer[42] = buffer[51] = buffer[60] = buffer[69] = color.a;
	buffer[7] = buffer[16] = buffer[17] = buffer[26] = buffer[35] = buffer[44] = value = 0.0f;
	buffer[8] = buffer[43] = buffer[52] = buffer[53] = buffer[62] = buffer[71] = 1.0f;
	buffer[25] = buffer[70] = 0.33333333f;
	buffer[34] = buffer[61] = 0.66666666f;
	UpdataBuffer();
}

pavp::Bar::~Bar()
{

}

void pavp::Bar::CreateBufferObj()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0); //pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float))); //color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float))); //coords
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(BarIndex), BarIndex, GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void pavp::Bar::UpdataBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(buffer), buffer);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

float* pavp::Bar::Buffer()
{
	return this->buffer;
}

void pavp::Bar::Render(pavp::Shader* shader, GLint iconLoc)
{
	glBindVertexArray(VAO);
	shader->SetTexture2D(iconLoc, 0, icon);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
}

bool pavp::Bar::In(float xpos, float ypos)
{
	if (xpos >= buffer[0] && xpos <= buffer[36] && ypos >= buffer[10] && ypos <= buffer[1])
	{
		return true;
	}
	return false;
}

void pavp::Bar::SetPercent(float percent)
{
	value = percent;
	float width = buffer[36] - buffer[0];
	float bw = buffer[27] - buffer[18];
	buffer[18] = buffer[63] = buffer[0] + width * percent - bw / 2.0f; //BtnLeft
	buffer[27] = buffer[54] = buffer[18] + bw; //BtnRight
}

float pavp::Bar::GetPercent()
{
	//float width = buffer[36] - buffer[0];
	//float bw = buffer[27] - buffer[18];
	//return (buffer[18] - buffer[0] + bw / 2.0f) / width;
	return value;
}
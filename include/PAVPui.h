#pragma once
#include <PAVPshader.h>
#include <vector>
#include <algorithm>

#define PAVP_UI_NULL 0xFFFFFFFF
namespace pavp
{
	class UISystem;

	class Widget;

	//action
#define PAVP_UI_MOVE_IN 0
#define PAVP_UI_MOVE_OUT 1
	//Cursor Pos Callback
	typedef void (*CPCallback)(Widget* w, float x, float y, int action);

	//button
#define PAVP_UI_MB_LEFT GLFW_MOUSE_BUTTON_LEFT
#define PAVP_UI_MB_RIGHT GLFW_MOUSE_BUTTON_RIGHT
#define PAVP_UI_MB_MIDDLE GLFW_MOUSE_BUTTON_MIDDLE
	//action
#define PAVP_UI_PRESS GLFW_PRESS
#define PAVP_UI_RELEASE GLFW_RELEASE

	//Mouse Button Callback
	typedef void(*MBCallback)(Widget* w, int button, int action);

	typedef void(*SZCallback)(Widget* w, float wmul, float hmul);

	struct Color
	{
		float r, g, b, a;
	};

	class IconButton;

	class Bar;
}

class pavp::Widget
{
	friend pavp::UISystem;

public:
	Widget();

	virtual ~Widget();

	//绑定回调函数
	void BindCallback(MBCallback cb)
	{
		mbCallback = cb;
	}

	void BindCallback(CPCallback cb)
	{
		cpCallback = cb;
	}

	void BindCallback(SZCallback cb)
	{
		szCallback = cb;
	}

	//创建缓冲对象
	virtual void CreateBufferObj() = 0;

	//向显存内更新缓冲数据
	virtual void UpdataBuffer() = 0;

	//获取数据缓存地址
	virtual float* Buffer() = 0;

	//渲染
	virtual void Render(pavp::Shader* shader, GLint iconLoc) = 0;

	//获得组件当前图标
	inline GLuint GetIcon()
	{
		return this->icon;
	}

	//设置组件当前图标
	inline void SetIcon(GLuint icon)
	{
		this->icon = icon;
	}

	//获取组件按下状态
	inline bool IsPress()
	{
		return this->press;
	}

	//设置组件按下状态
	inline void SetPress(bool p)
	{
		this->press = p;
	}

	//判断给定坐标是否在控件内
	virtual bool In(float xpos, float ypos) = 0;

protected:
	GLuint VBO, VAO, EBO;
	float layer;
	bool hide;
	bool press;
	GLuint icon;
	DWORD id;

public:
	MBCallback mbCallback;
	CPCallback cpCallback;
	SZCallback szCallback;

private:
	static void defMBCallback(Widget* w, int button, int action){}

	static void defCPCallback(Widget* w, float xpos, float ypos, int action){}

	static void defSZCallback(Widget* w, float wmul, float hmul){}
};

class pavp::UISystem
{
public:
	UISystem();

	~UISystem();

	//添加一个UI组件，返回其id
	DWORD set(pavp::Widget* w);

	//通过id获取一个UI组件的指针
	inline pavp::Widget* get(DWORD wid)
	{
		return this->widgets[wid];
	}

	//隐藏某个UI组件
	inline void hide(DWORD wid)
	{
		this->widgets[wid]->hide = true;
	}

	//显示某个UI组件
	inline void show(DWORD wid)
	{
		this->widgets[wid]->hide = false;
	}

	//渲染UI
	void render();

	//完成添加后整理UI组件
	void make();

	//获取与该坐标相关的UI
	DWORD which(float xpos, float ypos);

	//调用所有widget的大小回调
	void callsize(float wmul, float hmul);

private:
	std::vector<pavp::Widget*> widgets;
	pavp::Widget** sorted;

	pavp::Shader* shader;

	GLint iconLoc;

	static bool layercomp(pavp::Widget* a, pavp::Widget* b);
};

class pavp::IconButton :public pavp::Widget
{
public:
	IconButton(float left, float top, float width, float height, float layer, GLuint icon, Color& color);

	IconButton(int windowW, int windowH, int left, int top, int width, int height, float layer, GLuint icon, Color& color);

	~IconButton();

	virtual void CreateBufferObj();

	virtual void UpdataBuffer();

	virtual float* Buffer();

	virtual void Render(pavp::Shader* shader, GLint iconLoc);

	virtual bool In(float xpos, float ypos);

private:
	float buffer[36];
};

class pavp::Bar :public pavp::Widget
{
public:
	Bar(float left, float top, float width, float height, float btnw, float layer, float percent, GLuint icon, Color& color);

	Bar(int windowW, int windowH, int left, int top, int width, int height, int btnw, float layer, float percent, GLuint icon, Color& color);

	~Bar();

	virtual void CreateBufferObj();

	virtual void UpdataBuffer();

	virtual float* Buffer();

	virtual void Render(pavp::Shader* shader, GLint iconLoc);

	virtual bool In(float xpos, float ypos);

	float GetPercent();

	void SetPercent(float percent);

private:
	float buffer[72];

	float value;
};
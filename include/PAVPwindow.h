#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

namespace pavp
{
	class Window;

	struct Filter
	{
		const wchar_t* name;
		const wchar_t* spec;
	};
}

class pavp::Window
{
public:
	//构造
	Window(int width, int height, const char* name);

	//析构
	~Window();

	//判断窗口是否应被关闭
	inline bool ShouldClose()
	{
		return glfwWindowShouldClose(this->window);
	}

	//关闭窗口
	inline void Close()
	{
		glfwSetWindowShouldClose(this->window, GL_TRUE);
	}

	//刷新缓冲(双缓冲交换)
	inline void Refresh()
	{
		glfwSwapBuffers(this->window);
	}

	//处理事件
	inline void PollEvents()
	{
		glfwPollEvents();
	}

	//清除缓冲
	inline void Clear(GLbitfield value)
	{
		glClear(value);
	}

	//设置键盘回调函数
	inline void SetKeyCallback(GLFWkeyfun callback)
	{
		glfwSetKeyCallback(this->window, callback);
	}

	//设置鼠标位置回调函数
	inline void SetCursorPosCallback(GLFWcursorposfun callback)
	{
		glfwSetCursorPosCallback(this->window, callback);
	}

	//设置鼠标按键回调函数
	inline void SetMouseButtonCallback(GLFWmousebuttonfun callback)
	{
		glfwSetMouseButtonCallback(this->window, callback);
	}

	//设置鼠标滚轮回调函数
	inline void SetScrollCallback(GLFWscrollfun callback)
	{
		glfwSetScrollCallback(this->window, callback);
	}

	//设置窗口大小回调函数
	inline void SetWindowSizeCallback(GLFWwindowsizefun callback)
	{
		glfwSetWindowSizeCallback(this->window, callback);
	}

	//设置窗口位置回调函数
	inline void SetWindowPosCallback(GLFWwindowposfun callback)
	{
		glfwSetWindowPosCallback(this->window, callback);
	}

	//获取窗口位置
	inline void GetWindowPos(int* xpos, int* ypos)
	{
		*xpos = this->xpos;
		*ypos = this->ypos;
	}

	//设置窗口位置
	inline void SetWindowPos(int xpos, int ypos)
	{
		if (this->windowed)
		{
			glfwSetWindowPos(this->window, xpos, ypos);
			this->xpos = xpos;
			this->ypos = ypos;
		}
	}

	//更新窗口位置
	inline void UpdataPos(int xpos, int ypos)
	{
		this->xpos = xpos;
		this->ypos = ypos;
	}

	//获取窗口大小
	inline void GetWindowSize(int* width, int* height)
	{
		*width = this->width;
		*height = this->height;
	}

	inline void GetWindowSize(float* width, float* height)
	{
		*width = fwidth;
		*height = fheight;
	}

	//设置窗口大小
	inline void SetWindowSize(int width, int height)
	{
		if (this->windowed)
		{
			glfwSetWindowSize(this->window, width, height);
			this->width = width;
			this->height = height;
		}
	}

	//更新窗口大小
	inline void UpdataSize(int width, int height)
	{
		this->width = width;
		this->height = height;
		this->fwidth = float(width);
		this->fheight = float(height);
	}

	//设置大小极限
	inline void SetSizelimit(int minW, int minH, int maxW, int maxH)
	{
		glfwSetWindowSizeLimits(this->window, minW, minH, maxW, maxH);
	}

	//设置窗口全屏
	inline void SetFullScreen()
	{
		if (this->windowed)
		{
			this->windowed = false;
			//存储窗口化时的状态
			this->befFullx = this->xpos;
			this->befFully = this->ypos;
			this->befFullw = this->width;
			this->befFullh = this->height;
			//改变窗口大小
			glfwSetWindowMonitor(
				this->window,
				this->monitor,
				0, 0,
				this->screenW, this->screenH,
				GLFW_DONT_CARE
			);
		}
	}

	//设置窗口窗口化
	inline void SetWindowing()
	{
		if (!this->windowed)
		{
			this->windowed = true;
			//改变窗口大小
			glfwSetWindowMonitor(
				this->window,
				NULL,
				this->befFullx, this->befFully,
				this->befFullw, this->befFullh,
				GLFW_DONT_CARE
			);
		}
	}

	//判断是否窗口化
	inline bool IsWindowed()
	{
		return this->windowed;
	}

	//设置视口范围
	inline void SetViewport(int left, int top, int width, int height)
	{
		glViewport(left, top, width, height);
	}

	//设置清除颜色
	inline void SetClearColor(float r, float g, float b)
	{
		glClearColor(r, g, b, 1.0f);
	}

	//隐藏光标
	inline void HideCursor()
	{
		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		cursorHide = true;
	}

	//显示光标
	inline void ShowCursor()
	{
		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		cursorHide = false;
	}

	//判断光标是否隐藏
	inline bool CursorIsHide()
	{
		return cursorHide;
	}

	//获取光标位置
	inline void GetCursorPos(float* xpos, float* ypos)
	{
		double x, y;
		glfwGetCursorPos(this->window, &x, &y);
		*xpos = x / (fwidth / 2.0f) - 1.0f;
		*ypos = 1.0f - y / (fheight / 2.0f);
	}

	//绑定上下文到当前线程
	inline void BindContext()
	{
		glfwMakeContextCurrent(this->window);
	}

	//解绑当前线程上下文
	inline void UnBindContext()
	{
		glfwMakeContextCurrent(NULL);
	}

	bool GetOpenFile(std::string* name, pavp::Filter* filter, int filterNum);

private:
	GLFWwindow* window; //窗口指针
	GLFWmonitor* monitor; //显示器指针
	const GLFWvidmode* vidmode; //视频模式指针
	bool windowed; //是否窗口化
	bool cursorHide; //鼠标是否隐藏
	int xpos, ypos; //窗口位置
	int width, height; //窗口大小
	float fwidth, fheight;
	int befFullx, befFully; //全屏化之前的窗口位置
	int befFullw, befFullh; //全屏化之前的窗口大小
	int screenW, screenH;

	void InitCOM();
};
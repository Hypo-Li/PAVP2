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
	//����
	Window(int width, int height, const char* name);

	//����
	~Window();

	//�жϴ����Ƿ�Ӧ���ر�
	inline bool ShouldClose()
	{
		return glfwWindowShouldClose(this->window);
	}

	//�رմ���
	inline void Close()
	{
		glfwSetWindowShouldClose(this->window, GL_TRUE);
	}

	//ˢ�»���(˫���彻��)
	inline void Refresh()
	{
		glfwSwapBuffers(this->window);
	}

	//�����¼�
	inline void PollEvents()
	{
		glfwPollEvents();
	}

	//�������
	inline void Clear(GLbitfield value)
	{
		glClear(value);
	}

	//���ü��̻ص�����
	inline void SetKeyCallback(GLFWkeyfun callback)
	{
		glfwSetKeyCallback(this->window, callback);
	}

	//�������λ�ûص�����
	inline void SetCursorPosCallback(GLFWcursorposfun callback)
	{
		glfwSetCursorPosCallback(this->window, callback);
	}

	//������갴���ص�����
	inline void SetMouseButtonCallback(GLFWmousebuttonfun callback)
	{
		glfwSetMouseButtonCallback(this->window, callback);
	}

	//���������ֻص�����
	inline void SetScrollCallback(GLFWscrollfun callback)
	{
		glfwSetScrollCallback(this->window, callback);
	}

	//���ô��ڴ�С�ص�����
	inline void SetWindowSizeCallback(GLFWwindowsizefun callback)
	{
		glfwSetWindowSizeCallback(this->window, callback);
	}

	//���ô���λ�ûص�����
	inline void SetWindowPosCallback(GLFWwindowposfun callback)
	{
		glfwSetWindowPosCallback(this->window, callback);
	}

	//��ȡ����λ��
	inline void GetWindowPos(int* xpos, int* ypos)
	{
		*xpos = this->xpos;
		*ypos = this->ypos;
	}

	//���ô���λ��
	inline void SetWindowPos(int xpos, int ypos)
	{
		if (this->windowed)
		{
			glfwSetWindowPos(this->window, xpos, ypos);
			this->xpos = xpos;
			this->ypos = ypos;
		}
	}

	//���´���λ��
	inline void UpdataPos(int xpos, int ypos)
	{
		this->xpos = xpos;
		this->ypos = ypos;
	}

	//��ȡ���ڴ�С
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

	//���ô��ڴ�С
	inline void SetWindowSize(int width, int height)
	{
		if (this->windowed)
		{
			glfwSetWindowSize(this->window, width, height);
			this->width = width;
			this->height = height;
		}
	}

	//���´��ڴ�С
	inline void UpdataSize(int width, int height)
	{
		this->width = width;
		this->height = height;
		this->fwidth = float(width);
		this->fheight = float(height);
	}

	//���ô�С����
	inline void SetSizelimit(int minW, int minH, int maxW, int maxH)
	{
		glfwSetWindowSizeLimits(this->window, minW, minH, maxW, maxH);
	}

	//���ô���ȫ��
	inline void SetFullScreen()
	{
		if (this->windowed)
		{
			this->windowed = false;
			//�洢���ڻ�ʱ��״̬
			this->befFullx = this->xpos;
			this->befFully = this->ypos;
			this->befFullw = this->width;
			this->befFullh = this->height;
			//�ı䴰�ڴ�С
			glfwSetWindowMonitor(
				this->window,
				this->monitor,
				0, 0,
				this->screenW, this->screenH,
				GLFW_DONT_CARE
			);
		}
	}

	//���ô��ڴ��ڻ�
	inline void SetWindowing()
	{
		if (!this->windowed)
		{
			this->windowed = true;
			//�ı䴰�ڴ�С
			glfwSetWindowMonitor(
				this->window,
				NULL,
				this->befFullx, this->befFully,
				this->befFullw, this->befFullh,
				GLFW_DONT_CARE
			);
		}
	}

	//�ж��Ƿ񴰿ڻ�
	inline bool IsWindowed()
	{
		return this->windowed;
	}

	//�����ӿڷ�Χ
	inline void SetViewport(int left, int top, int width, int height)
	{
		glViewport(left, top, width, height);
	}

	//���������ɫ
	inline void SetClearColor(float r, float g, float b)
	{
		glClearColor(r, g, b, 1.0f);
	}

	//���ع��
	inline void HideCursor()
	{
		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		cursorHide = true;
	}

	//��ʾ���
	inline void ShowCursor()
	{
		glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		cursorHide = false;
	}

	//�жϹ���Ƿ�����
	inline bool CursorIsHide()
	{
		return cursorHide;
	}

	//��ȡ���λ��
	inline void GetCursorPos(float* xpos, float* ypos)
	{
		double x, y;
		glfwGetCursorPos(this->window, &x, &y);
		*xpos = x / (fwidth / 2.0f) - 1.0f;
		*ypos = 1.0f - y / (fheight / 2.0f);
	}

	//�������ĵ���ǰ�߳�
	inline void BindContext()
	{
		glfwMakeContextCurrent(this->window);
	}

	//���ǰ�߳�������
	inline void UnBindContext()
	{
		glfwMakeContextCurrent(NULL);
	}

	bool GetOpenFile(std::string* name, pavp::Filter* filter, int filterNum);

private:
	GLFWwindow* window; //����ָ��
	GLFWmonitor* monitor; //��ʾ��ָ��
	const GLFWvidmode* vidmode; //��Ƶģʽָ��
	bool windowed; //�Ƿ񴰿ڻ�
	bool cursorHide; //����Ƿ�����
	int xpos, ypos; //����λ��
	int width, height; //���ڴ�С
	float fwidth, fheight;
	int befFullx, befFully; //ȫ����֮ǰ�Ĵ���λ��
	int befFullw, befFullh; //ȫ����֮ǰ�Ĵ��ڴ�С
	int screenW, screenH;

	void InitCOM();
};
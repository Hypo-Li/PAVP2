#include <PAVPwindow.h>
#include <Shlobj.h>
#define OPENGL_VERSION_MAJOR 3
#define OPENGL_VERSION_MINOR 3
static void ErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW error: " << description << std::endl;
	exit(-1);
}

pavp::Window::Window(int width, int height, const char* name)
{
	//glfw初始化
	glfwInit();
	//设置错误回调函数
	glfwSetErrorCallback(ErrorCallback);
	//设置OpenGL版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
	//声明使用OpenGL Core
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//类成员赋值
	this->monitor = glfwGetPrimaryMonitor();
	this->vidmode = glfwGetVideoMode(monitor);
	this->width = width;
	this->height = height;
	this->windowed = true;
	this->cursorHide = false;
	this->befFullx = vidmode->width / 2 - 512;
	this->befFully = vidmode->height / 2 - 288;
	this->befFullw = 1024;
	this->befFullh = 288;
	this->screenW = GetSystemMetrics(SM_CXSCREEN);
	this->screenH = GetSystemMetrics(SM_CYSCREEN);
	GLFWmonitor* tempMonitor = NULL;
	//当width||height为0时，设置为全屏
	if (!(width && height)) {
		this->width = vidmode->width;
		this->height = vidmode->height;
		tempMonitor = this->monitor;
		this->windowed = false;
	}
	this->fwidth = float(this->width);
	this->fheight = float(this->height);
	//创建窗口
	this->window = glfwCreateWindow(
		this->width,
		this->height,
		name,
		tempMonitor,
		NULL
	);
	//获取窗口位置
	glfwGetWindowPos(this->window, &this->xpos, &this->ypos);
	
	//glad初始化
	glfwMakeContextCurrent(this->window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "GLAD error: Failed to initialize GLAD" << std::endl;
		exit(-1);
	}

	//设置纹理一字节对齐
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//启用混合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//反面剔除
	glEnable(GL_CULL_FACE);
	//深度测试
	//glEnable(GL_DEPTH_TEST);

	//COM组件初始化
	InitCOM();
}

pavp::Window::~Window()
{
	glfwDestroyWindow(this->window);
	glfwTerminate();
}

void pavp::Window::InitCOM()
{
	CoInitialize(nullptr);
}

bool pavp::Window::GetOpenFile(std::string* name, pavp::Filter* filter, int filterNum)
{
	IFileDialog* pfd = nullptr;
	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
	if (SUCCEEDED(hr)) {
		DWORD dwFlags;
		hr = pfd->GetOptions(&dwFlags);
		hr = pfd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM | FOS_ALLOWMULTISELECT
		);
		hr = pfd->SetFileTypes(filterNum, (const COMDLG_FILTERSPEC*)filter);
		hr = pfd->SetFileTypeIndex(1);
		hr = pfd->Show(NULL);
		if (SUCCEEDED(hr)) {
			IShellItem* pSelItem;
			hr = pfd->GetResult(&pSelItem);
			if (SUCCEEDED(hr)) {
				LPWSTR pszFilePath = NULL;
				hr = pSelItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszFilePath);
				size_t len = WideCharToMultiByte(CP_ACP, 0, pszFilePath, wcslen(pszFilePath), NULL, 0, NULL, NULL);
				char* file = new char[len + 1];
				WideCharToMultiByte(CP_ACP, 0, pszFilePath, wcslen(pszFilePath), file, len, NULL, NULL);
				file[len] = '\0';
				*name = file;
				delete file;
				CoTaskMemFree(pszFilePath);
				pSelItem->Release();
			}
			pfd->Release();
			return true;
		}
		pfd->Release();
	}
	return false;
}
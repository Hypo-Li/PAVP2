#pragma once
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_types.h>
#undef main
}
#include <thread>
#include <mutex>
#include <condition_variable>
#include <PAVPqueue.h>
#include <PAVPshader.h>
#include <libyuv/libyuv.h>

namespace pavp
{
	//RGB24֡
	struct RGB24Frame
	{
		BYTE* data;
		float pts;
	};

	//����Ƶ��Ϣ�йصı���
	struct AVContext
	{
		AVFormatContext* pFormatCtx;
		//ʱ������λ����
		DWORD duration;
		//����
		double percent;

		//Audio
		int audio_index = -1;
		AVStream* pStreamA;
		AVCodecContext* pCodecCtxA;
		AVCodec* pCodecA;
		pavp::Queue<AVPacket> aPktQue;
		AVSampleFormat aFormat;
		int aChannels;
		int aSample;
		int aSampleRate;
		uint8_t* apos;
		DWORD alen;
		double apts;

		//Video
		int video_index = -1;
		AVStream* pStreamV;
		AVCodecContext* pCodecCtxV;
		AVCodec* pCodecV;
		pavp::Queue<AVPacket> vPktQue;
		AVPixelFormat vFormat;
		int vWidth, vHeight;
		float viewWidth, viewHeight;

		//��libyuvʵ��yuvת��������Դ
		pavp::Queue<RGB24Frame> vRGB24FrmQue;

	};

	//��������Ƶ���ŵ��źű���
	struct AVSignal
	{
		bool pause;
		bool eof;
		bool exit;
		bool seek;
		int64_t seek_timestamp;
	};

	//���ڱ�֤�̰߳�ȫ��ͬ���ı���
	struct AVThread
	{
		std::condition_variable demuxCV;
		std::condition_variable audioCV;
		std::condition_variable videoCV;
		std::condition_variable renderCV;

		std::mutex aPktCVMtx;
		std::mutex vPktCVMtx;
		std::mutex demuxCVMtx;
		std::mutex audioMtx;
		std::mutex videoMtx;
		std::mutex frmCVMtx;

		std::mutex aPktMtx;
		std::mutex vPktMtx;
		std::mutex frmMtx;
	};

	typedef DWORD (*RGBTexMaker)(pavp::AVContext* ctx, pavp::AVSignal* sig, pavp::AVThread* thd, GLuint tex);

	class AudioVideo;
}

class pavp::AudioVideo
{
public:
	//AudioVideo:
	AVContext avCtx;
	AVSignal avSig;
	AVThread avThd;

	AudioVideo(const char* filePath);

	~AudioVideo();

	void Ready();

	DWORD Render();

	void Pause(bool v);

	void Exit();

	void Seek(double percent);

	double GetPercent();

	void ViewportCanvas();

private:
	void InitAudioVideo(const char* filePath);
	void CreateBufferObj();

	RGBTexMaker MakeRGBTexture;

	//Task thread
	std::thread* thd_Demux;
	std::thread* thd_DecodeAudio;
	std::thread* thd_DecodeVideo;

	//Shader:
	Shader* RGBShader;

	//Buffer Object
	GLuint VAO, VBO, EBO, Tex, FBO, RBO;
};

#define MAX_AUDIO_PACKET_SIZE 8
#define MAX_VIDEO_PACKET_SIZE 6
#define MAX_VIDEO_FRAME_SIZE 4
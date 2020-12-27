#include <PAVPav.h>
#include <PAVPdemux.h>
#include <PAVPaudio.h>
#include <PAVPvideo.h>
#include <PAVPvar.h>

static void PutError(const char* obj, const char* log)
{
	std::cerr << obj << ": " << log << std::endl;
}

void pavp::AudioVideo::InitAudioVideo(const char* filePath)
{
	//打开文件
	if (avformat_open_input(&avCtx.pFormatCtx, filePath, NULL, NULL) != 0)
	{
		PutError(filePath, "Can't open;");
		return;
	}

	//解封装
	if (avformat_find_stream_info(avCtx.pFormatCtx, NULL) < 0)
	{
		PutError(filePath, "Can't find stream info;");
		return;
	}

	avCtx.duration = avCtx.pFormatCtx->duration / 1000000;

	//寻找流
	avCtx.audio_index = av_find_best_stream(avCtx.pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
	avCtx.video_index = av_find_best_stream(avCtx.pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);

	//如果音频流存在，拷贝音频解码器
	if (avCtx.audio_index != AVERROR_STREAM_NOT_FOUND)
	{
		//流
		avCtx.pStreamA = avCtx.pFormatCtx->streams[avCtx.audio_index];

		//创建解码器
		avCtx.pCodecA = avcodec_find_decoder(avCtx.pStreamA->codecpar->codec_id);
		if (!avCtx.pCodecA)
		{
			PutError(filePath, "Can't find audio decoder;");
			return;
		}

		//创建解码器上下文
		avCtx.pCodecCtxA = avcodec_alloc_context3(avCtx.pCodecA);
		if (avcodec_parameters_to_context(avCtx.pCodecCtxA, avCtx.pStreamA->codecpar) < 0)
		{
			PutError(filePath, "Can't copy audio codec parameters to decoder context;");
			return;
		}

		//没有此句会出现：Could not update timestamps for skipped samples
		avCtx.pCodecCtxA->pkt_timebase = avCtx.pStreamA->time_base;

		//打开音频解码器
		if (avcodec_open2(avCtx.pCodecCtxA, avCtx.pCodecA, NULL) < 0)
		{
			PutError(filePath, "Can't open audio decoder;");
			return;
		}

		//记录音频参数数据
		avCtx.aFormat = avCtx.pCodecCtxA->sample_fmt;
		avCtx.aChannels = avCtx.pCodecCtxA->channels;
		avCtx.aSample = avCtx.pCodecCtxA->frame_size;
		if (!avCtx.aSample)
		{
			switch (avCtx.pCodecA->id)
			{
			case AV_CODEC_ID_AC3:
				avCtx.aSample = 1536;
				break;
			case AV_CODEC_ID_FLAC:
				avCtx.aSample = 4096;
				break;
			default:
				std::cerr << "Can't make sure the audio sample size";
				avCtx.aSample = 4096;
				break;
			}
		}
		avCtx.aSampleRate = avCtx.pCodecCtxA->sample_rate;
	}

	if (avCtx.video_index != AVERROR_STREAM_NOT_FOUND)
	{
		//流
		avCtx.pStreamV = avCtx.pFormatCtx->streams[avCtx.video_index];

		//创建解码器
		avCtx.pCodecV = avcodec_find_decoder(avCtx.pStreamV->codecpar->codec_id);
		if (!avCtx.pCodecV)
		{
			PutError(filePath, "Can't find video decoder;");
			return;
		}

		//创建解码器上下文
		avCtx.pCodecCtxV = avcodec_alloc_context3(avCtx.pCodecV);
		if (avcodec_parameters_to_context(avCtx.pCodecCtxV, avCtx.pStreamV->codecpar) < 0)
		{
			PutError(filePath, "Can't copy video codec parameters to decoder context;");
			return;
		}

		//打开视频解码器
		if (avcodec_open2(avCtx.pCodecCtxV, avCtx.pCodecV, NULL) < 0)
		{
			PutError(filePath, "Can't open video decoder;");
			return;
		}

		//记录视频参数数据
		avCtx.vFormat = avCtx.pCodecCtxV->pix_fmt;
		avCtx.vWidth = avCtx.pCodecCtxV->width;
		avCtx.vHeight = avCtx.pCodecCtxV->height;
	}
}

static float avCanvasVertex[] = {
	-1.0f, 1.0f,  0.0f, 0.0f,//left top
	-1.0f, -1.0f, 0.0f, 1.0f, //left bottom
	1.0f, -1.0f, 1.0f, 1.0f, //right bottom
	1.0f, 1.0f, 1.0f, 0.0f //right top
};

static unsigned int avCanvasIndex[] = {
	0, 1, 2,
	0, 2, 3
};

void pavp::AudioVideo::CreateBufferObj()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(avCanvasVertex), avCanvasVertex, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); //pos
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(avCanvasIndex), avCanvasIndex, GL_STATIC_DRAW);
	glBindVertexArray(0);

	glGenTextures(1, &Tex);
	glBindTexture(GL_TEXTURE_2D, Tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, avCtx.vWidth, avCtx.vHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, avCtx.vWidth, avCtx.vHeight);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Tex, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

pavp::AudioVideo::AudioVideo(const char* filePath)
{
	avSig = { true, false, false, false };
	InitAudioVideo(filePath);
	CreateBufferObj();
	ViewportCanvas();
	avCtx.percent = 0.0;
}

void FreeFrame(pavp::RGB24Frame* f);
pavp::AudioVideo::~AudioVideo()
{
	avCtx.aPktQue.clear(av_packet_unref);
	avCtx.vPktQue.clear(av_packet_unref);
	avCtx.vRGB24FrmQue.clear(FreeFrame);
	if (avCtx.audio_index != AVERROR_STREAM_NOT_FOUND)
	{
		avcodec_close(avCtx.pCodecCtxA);
		avcodec_free_context(&avCtx.pCodecCtxA);
	}
	if (avCtx.video_index != AVERROR_STREAM_NOT_FOUND)
	{
		avcodec_close(avCtx.pCodecCtxV);
		avcodec_free_context(&avCtx.pCodecCtxV);
	}
	avformat_close_input(&avCtx.pFormatCtx);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

void pavp::AudioVideo::Ready()
{
	if (avCtx.audio_index != AVERROR_STREAM_NOT_FOUND && avCtx.video_index == AVERROR_STREAM_NOT_FOUND)
	{
		thd_Demux = new std::thread(Demux_Audio, &avCtx, &avSig, &avThd);
		thd_DecodeAudio = new std::thread(DecodeAudio_SDL2, &avCtx, &avSig, &avThd);
	}
	else if (avCtx.audio_index != AVERROR_STREAM_NOT_FOUND && avCtx.video_index != AVERROR_STREAM_NOT_FOUND)
	{
		thd_Demux = new std::thread(DemuxAudioVideo, &avCtx, &avSig, &avThd);
		thd_DecodeAudio = new std::thread(DecodeAudio_SDL2, &avCtx, &avSig, &avThd);
		thd_DecodeVideo = new std::thread(DecodeVideo_libyuv, &avCtx, &avSig, &avThd);
		MakeRGBTexture = MakeTexture_libyuv;
	}
}

DWORD pavp::AudioVideo::Render()
{
	DWORD delay = MakeRGBTexture(&avCtx, &avSig, &avThd, Tex);
	loadedShader->Use();
	glBindVertexArray(VAO);
	for (BYTE i = 0; i < 4; ++i)
	{
		if (iTexture[i])
			loadedShader->SetTexture2D(iTextureLoc[i], i + 1, iTexture[i]->get());
	}
	loadedShader->SetTexture2D(iFrameLoc, 0, Tex);
	loadedShader->SetFloat(iTimeLoc, avCtx.apts);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	return delay;
}

void pavp::AudioVideo::Pause(bool v)
{
	if (!av)
		return;
	avSig.pause = v;
	avThd.audioCV.notify_all();
	avThd.videoCV.notify_all();
}

void pavp::AudioVideo::Exit()
{
	if (!av)
		return;
	//先令暂停状态取消再设置退出信号
	if (avSig.pause)
		avSig.pause = false;
	avSig.exit = true;
	avThd.audioCV.notify_all();
	avThd.videoCV.notify_all();
	avThd.demuxCV.notify_all();
	if (avCtx.audio_index != AVERROR_STREAM_NOT_FOUND && avCtx.video_index == AVERROR_STREAM_NOT_FOUND)
	{
		thd_Demux->join();
		thd_DecodeAudio->join();
	}
	else if (avCtx.audio_index != AVERROR_STREAM_NOT_FOUND && avCtx.video_index != AVERROR_STREAM_NOT_FOUND)
	{
		thd_Demux->join();
		thd_DecodeAudio->join();
		thd_DecodeVideo->join();
	}
}

void pavp::AudioVideo::Seek(double percent)
{
	if (!av)
		return;
	avSig.seek_timestamp = (percent * avCtx.duration) / av_q2d(avCtx.pStreamV->time_base);
	avSig.seek = true;
}

double pavp::AudioVideo::GetPercent()
{
	return avCtx.percent;
}

void pavp::AudioVideo::ViewportCanvas()
{
	float x, y, w, h;
	float vwhRate = (float)avCtx.vWidth / (float)avCtx.vHeight;
	float ww, wh;
	wnd->GetWindowSize(&ww, &wh);
	float wwhRate = ww / wh;
	if (vwhRate > wwhRate)
	{
		x = 0.0f;
		w = ww;
		h = w / vwhRate;
		y = (wh - h) / 2;
	}
	else {
		y = 0.0f;
		h = wh;
		w = h * vwhRate;
		x = (ww - w) / 2;
	}
	x = (x / ww - 0.5f) * 2.0f;
	y = (0.5f - y / wh) * 2.0f;
	w = (w / ww) * 2.0f;
	h = (h / wh) * 2.0f;
	float buffer[] = {
		x, y, 0.0f, 0.0f,
		x, y - h, 0.0f, 1.0f,
		x + w, y - h, 1.0f, 1.0f,
		x + w, y, 1.0f, 0.0f
	};
	avCtx.viewWidth = w * ww;
	avCtx.viewHeight = h * wh;
	loadedShader->Use();
	loadedShader->SetVec2(iSizeLoc, avCtx.viewWidth, avCtx.viewHeight);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(buffer), buffer);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
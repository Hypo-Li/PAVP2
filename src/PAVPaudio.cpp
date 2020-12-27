#include <PAVPaudio.h>

static void ReadAudioData_SDL2(void* udata, uint8_t* stream, int len)
{
	pavp::AVContext* avCtx = (pavp::AVContext*)udata;
	memset(stream, 0, len);
	if (!avCtx->alen)
		return;
	len = len > avCtx->alen ? avCtx->alen : len;

	SDL_MixAudio(stream, avCtx->apos, len, SDL_MIX_MAXVOLUME);
	avCtx->apos += len;
	avCtx->alen -= len;
}

void DecodeAudio_SDL2(pavp::AVContext* ctx, pavp::AVSignal* sig, pavp::AVThread* thd)
{
	AVPacket* pPacket = NULL;
	AVFrame* pFrame = av_frame_alloc();
	DWORD aPktSize;

	//设置重采样器
	SwrContext* pSwrCtx = swr_alloc();
	pSwrCtx = swr_alloc_set_opts(pSwrCtx, av_get_default_channel_layout(ctx->aChannels), AV_SAMPLE_FMT_S16, ctx->aSampleRate, av_get_default_channel_layout(ctx->aChannels), ctx->aFormat, ctx->aSampleRate, 0, NULL);
	swr_init(pSwrCtx);

	//分配重采样输出缓冲区
	size_t frameSize = av_samples_get_buffer_size(NULL, ctx->aChannels, ctx->aSample, AV_SAMPLE_FMT_S16, 1);
#define MAX_FRAME_IN_ONE_PACKET 5
	size_t bufferSize = frameSize * MAX_FRAME_IN_ONE_PACKET;
	uint8_t* buffer = (uint8_t*)malloc(bufferSize);
	memset(buffer, 0, bufferSize);

	//配置SDL
	if (SDL_Init(SDL_INIT_AUDIO))
		std::cerr << "Can't init SDL;" << std::endl;
	SDL_AudioSpec spec;
	spec.freq = ctx->aSampleRate;	//采样率
	spec.format = AUDIO_S16SYS;	//采样格式
	spec.channels = ctx->aChannels; //声道数
	spec.silence = 0;			//静音的值
	spec.samples = ctx->aSample;	//每帧每个通道的采样数
	spec.callback = ReadAudioData_SDL2;
	spec.userdata = ctx;
	if (SDL_OpenAudio(&spec, NULL) < 0)
		std::cerr << "Can't open audio;" << std::endl;

	std::unique_lock <std::mutex> lock(thd->audioMtx);
	while (!ctx->aPktQue.size())
		thd->audioCV.wait(lock);
	SDL_PauseAudio(0);
	std::cout << "Decode Audio is start;\n";
	while (ctx->aPktQue.size() && !sig->exit)
	{
		if (ctx->aPktQue.size() < MAX_AUDIO_PACKET_SIZE / 2)
			thd->demuxCV.notify_all();
		while(!(pPacket = ctx->aPktQue.frontp()))
			thd->audioCV.wait(lock);
		if (!avcodec_send_packet(ctx->pCodecCtxA, pPacket))
		{
			int i = 0;
			ctx->apts = pPacket->pts * av_q2d(ctx->pStreamA->time_base);
			//解引用包
			av_packet_unref(pPacket);
			thd->aPktMtx.lock();
			ctx->aPktQue.pop();
			thd->aPktMtx.unlock();
			while (!avcodec_receive_frame(ctx->pCodecCtxA, pFrame))
			{
				//重采样
				uint8_t* currentBuffer = buffer + i * frameSize;
				swr_convert(pSwrCtx, &currentBuffer, ctx->aSample, (const uint8_t**)pFrame->data, pFrame->nb_samples);

				//解引用帧 如果不调用unref，在receive_frame中也会自动调用
				av_frame_unref(pFrame);
				++i;
			}
			//播放(SDL)
			ctx->alen = i * frameSize;
			ctx->apos = buffer;
			//等待声音设备播放完成
			while (ctx->alen > 0)
				SDL_Delay(1);

			while (aPktSize = ctx->aPktQue.size(), (!aPktSize && !sig->exit && !sig->eof) || (aPktSize && sig->pause && !sig->exit))
				thd->audioCV.wait(lock);
		}
	}
	av_frame_free(&pFrame);
	SDL_CloseAudio();
	swr_free(&pSwrCtx);
	SDL_Quit();
	std::cout << "Decode Audio is over;\n";
	return;
}
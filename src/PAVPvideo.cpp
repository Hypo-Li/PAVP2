#include <PAVPvideo.h>
#include <PAVPvar.h>
//同步阈值
#define AV_SYNC_THRESHOLD 0.01
DWORD GetDelay(float vpts, float apts)
{
	float diff = vpts - apts;
	if (diff > AV_SYNC_THRESHOLD && diff < 1.0) //视频渲染超前，适当延迟
		return (diff - AV_SYNC_THRESHOLD) * 1000;
	if (diff <= AV_SYNC_THRESHOLD && diff >= -AV_SYNC_THRESHOLD) //同步中，正常播放
		return 0;
	return 0; //视频落后，加速播放
}

void DecodeVideo_libyuv(pavp::AVContext* ctx, pavp::AVSignal* sig, pavp::AVThread* thd)
{
	std::cout << "Decode Video is start;\n";
	AVPacket* pPacket = NULL;
	AVFrame* pFrame = av_frame_alloc();
	DWORD vPktSize, vFrmSize;
	bool a, b;
	int ret;
	pavp::RGB24Frame vFrame;
	DWORD bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGB24, ctx->vWidth, ctx->vHeight, 1);
	std::unique_lock <std::mutex> lock(thd->videoMtx, std::try_to_lock);

	while (!ctx->vPktQue.size())
		thd->videoCV.wait(lock);
	while (ctx->vPktQue.size() && !sig->exit)
	{
		if (ctx->vPktQue.size() < MAX_VIDEO_PACKET_SIZE / 2)
			thd->demuxCV.notify_all();
		thd->vPktMtx.lock();
		while (!(pPacket = ctx->vPktQue.frontp()))
			thd->videoCV.wait(lock);
		do
		{
			if (avcodec_send_packet(ctx->pCodecCtxV, pPacket) < 0)
				std::cerr << "Can't send video packet;" << std::endl;
			ret = avcodec_receive_frame(ctx->pCodecCtxV, pFrame);
		} while (ret == AVERROR(EAGAIN));

		av_packet_unref(pPacket);
		ctx->vPktQue.pop();
		thd->vPktMtx.unlock();

		BYTE* frmBuf = (BYTE*)malloc(bufferSize);
		libyuv::I420ToRGB24(pFrame->data[0], pFrame->linesize[0], pFrame->data[2], pFrame->linesize[2], pFrame->data[1], pFrame->linesize[1], frmBuf, ctx->vWidth * 3, ctx->vWidth, ctx->vHeight);
		vFrame.data = frmBuf;
		vFrame.pts = pFrame->pts * av_q2d(ctx->pStreamV->time_base);
		av_frame_unref(pFrame);
		vFrmSize = ctx->vRGB24FrmQue.size();
		thd->frmMtx.lock();
		ctx->vRGB24FrmQue.push(vFrame);
		thd->frmMtx.unlock();
		if (!vFrmSize)
			thd->renderCV.notify_all();
		while (vPktSize = ctx->vPktQue.size(), ((!vPktSize && !sig->exit && !sig->eof) || (vPktSize && sig->pause && !sig->exit)) || ((ctx->vRGB24FrmQue.size() >= MAX_VIDEO_FRAME_SIZE) && !sig->exit))
			thd->videoCV.wait(lock);
	}
	av_frame_free(&pFrame);
	std::cout << "Decode Video is over;\n";
}

DWORD MakeTexture_libyuv(pavp::AVContext* ctx, pavp::AVSignal* sig, pavp::AVThread* thd, GLuint tex)
{
	std::unique_lock<std::mutex> lock(thd->frmCVMtx, std::try_to_lock);
	if(ctx->vRGB24FrmQue.size() < 3)
		thd->videoCV.notify_all();
	if (sig->pause || sig->exit || sig->eof)
		return 0;
	while (!ctx->vRGB24FrmQue.size())
		thd->renderCV.wait(lock);
	thd->frmMtx.lock();
	pavp::RGB24Frame frame = ctx->vRGB24FrmQue.front();
	ctx->vRGB24FrmQue.pop();
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ctx->vWidth, ctx->vHeight, GL_RGB, GL_UNSIGNED_BYTE, frame.data);
	free(frame.data);
	DWORD delay = GetDelay(frame.pts, ctx->apts);
	thd->frmMtx.unlock();
	return delay;
}
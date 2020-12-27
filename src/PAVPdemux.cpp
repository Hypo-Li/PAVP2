#include <PAVPdemux.h>
#include <PAVPvar.h>

void Demux_Audio(pavp::AVContext* ctx, pavp::AVSignal* sig, pavp::AVThread* thd)
{
	std::cout << "Demux is start;\n";
	AVPacket* pPacket = av_packet_alloc();
	DWORD aPktSize;
	std::unique_lock<std::mutex> lock(thd->aPktCVMtx, std::try_to_lock);

	while (!(av_read_frame(ctx->pFormatCtx, pPacket) < 0 || sig->exit))
	{
		if (pPacket->stream_index == ctx->audio_index)
		{
			aPktSize = ctx->aPktQue.size();
			av_packet_ref(ctx->aPktQue.push(), pPacket);
			if (!aPktSize) //在包数目为0时加入包并唤醒
				thd->audioCV.notify_all();
			while (ctx->aPktQue.size() >= 20 && !sig->exit)
				thd->demuxCV.wait(lock);
		}
		av_packet_unref(pPacket);
	}
	sig->eof = true;
	thd->audioCV.notify_all();
	av_packet_free(&pPacket);
	std::cout << "Demux is over;\n";
}

void FreeFrame(pavp::RGB24Frame* f)
{
	free(f->data);
}

//解复用 分派包
void DemuxAudioVideo(pavp::AVContext* ctx, pavp::AVSignal* sig, pavp::AVThread* thd)
{
	std::cout << "Demux is start;\n";
	AVPacket* pPacket = av_packet_alloc();
	DWORD aPktSize, vPktSize;
	std::unique_lock<std::mutex> lock(thd->demuxCVMtx);

	while (!(av_read_frame(ctx->pFormatCtx, pPacket) < 0 || sig->exit))
	{
		//std::cout << "Demux is working;\n";
		if (pPacket->stream_index == ctx->audio_index)
		{
			aPktSize = ctx->aPktQue.size();
			thd->aPktMtx.lock();
			av_packet_ref(ctx->aPktQue.push(), pPacket);
			thd->aPktMtx.unlock();
			if (!aPktSize) //在包数目为0时加入包并唤醒
				thd->audioCV.notify_all();
			while ((ctx->aPktQue.size() >= MAX_AUDIO_PACKET_SIZE && ctx->vPktQue.size() >= MAX_VIDEO_PACKET_SIZE) && !sig->exit)
				thd->demuxCV.wait(lock);
		}
		else if (pPacket->stream_index == ctx->video_index)
		{
			vPktSize = ctx->vPktQue.size();
			thd->vPktMtx.lock();
			av_packet_ref(ctx->vPktQue.push(), pPacket);
			thd->vPktMtx.unlock();
			if (!vPktSize) //在包数目为0时加入包并唤醒
				thd->videoCV.notify_all();
			while ((ctx->aPktQue.size() >= MAX_AUDIO_PACKET_SIZE && ctx->vPktQue.size() >= MAX_VIDEO_PACKET_SIZE) && !sig->exit)
				thd->demuxCV.wait(lock);
		}
		av_packet_unref(pPacket);

		if (sig->seek)
		{
			av_seek_frame(ctx->pFormatCtx, ctx->audio_index, sig->seek_timestamp, AVSEEK_FLAG_BACKWARD);
			av_seek_frame(ctx->pFormatCtx, ctx->video_index, sig->seek_timestamp, AVSEEK_FLAG_BACKWARD);
			sig->seek = false;
			thd->audioCV.notify_all();
			thd->videoCV.notify_all();
			thd->renderCV.notify_all();

			thd->aPktMtx.lock();
			ctx->aPktQue.clear(av_packet_unref);
			thd->aPktMtx.unlock();
			//清空vPkt队列会引发一系列问题，这里不清空
			/*thd->frmMtx.lock();
			ctx->vRGB24FrmQue.clear(FreeFrame);
			thd->frmMtx.unlock();*/
			ctx->apts = sig->seek_timestamp * av_q2d(ctx->pStreamA->time_base);
		}
		ctx->percent = ctx->apts / ctx->duration;
	}
	if (!sig->exit)
		sig->eof = true;
	sig->exit = true;
	thd->audioCV.notify_all();
	thd->videoCV.notify_all();
	av_packet_free(&pPacket);
	std::cout << "Demux is over;\n";
}
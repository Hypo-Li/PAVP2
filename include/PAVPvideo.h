#pragma once
#include <PAVPav.h>

void DecodeVideo_libyuv(pavp::AVContext* ctx, pavp::AVSignal* sig, pavp::AVThread* thd);

DWORD MakeTexture_libyuv(pavp::AVContext* ctx, pavp::AVSignal* sig, pavp::AVThread* thd, GLuint tex);

void DemuxAudioVideo(pavp::AVContext* ctx, pavp::AVSignal* sig, pavp::AVThread* thd);
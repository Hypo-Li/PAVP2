#pragma once
#include <PAVPav.h>

void DemuxAudioVideo(pavp::AVContext* ctx, pavp::AVSignal* sig, pavp::AVThread* thd);

void Demux_Audio(pavp::AVContext* ctx, pavp::AVSignal* sig, pavp::AVThread* thd);
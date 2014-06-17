/*
  SDL_audiolib - An audio decoding, resampling and mixing library
  Copyright (C) 2014  Nikos Chantziaras

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/
#include "audiodecoder.h"
#include <SDL_audio.h>
#include <SDL_rwops.h>

#include "aulib_global.h"
#include "aulib.h"
#include "aulib_config.h"
#include "audiostream.h"
#include "dec_vorbis.h"
#include "dec_mpg123.h"
#include "dec_modplug.h"
#include "dec_fluidsynth.h"
#include "dec_sndfile.h"


Aulib::AudioDecoder::AudioDecoder()
{ }


Aulib::AudioDecoder::~AudioDecoder()
{ }


Aulib::AudioDecoder*
Aulib::AudioDecoder::decoderFor(const char* filename)
{
    SDL_RWops* rwops = SDL_RWFromFile(filename, "rb");
    return AudioDecoder::decoderFor(rwops);
}


Aulib::AudioDecoder*
Aulib::AudioDecoder::decoderFor(SDL_RWops* rwops)
{
    Aulib::AudioDecoder* decoder;

#ifdef USE_DEC_LIBVORBIS
    decoder = new Aulib::AudioDecoderVorbis;
    if (decoder->open(rwops)) {
        delete decoder;
        return new Aulib::AudioDecoderVorbis;
    }
#endif

#ifdef USE_DEC_FLUIDSYNTH
    decoder = new Aulib::AudioDecoderFluidSynth;
    if (decoder->open(rwops)) {
        delete decoder;
        return new Aulib::AudioDecoderFluidSynth;
    }
#endif

#ifdef USE_DEC_SNDFILE
    decoder = new Aulib::AudioDecoderSndfile;
    if (decoder->open(rwops)) {
        delete decoder;
        return new Aulib::AudioDecoderSndfile;
    }
#endif

#ifdef USE_DEC_MODPLUG
    decoder = new Aulib::AudioDecoderModPlug;
    if (decoder->open(rwops)) {
        delete decoder;
        return new Aulib::AudioDecoderModPlug;
    }
#endif

#ifdef USE_DEC_MPG123
    decoder = new Aulib::AudioDecoderMpg123;
    if (decoder->open(rwops)) {
        delete decoder;
        return new Aulib::AudioDecoderMpg123;
    }
#endif

    return nullptr;
}


// Conversion happens in-place.
static void
monoToStereo(float buf[], int len)
{
    for (int i = len / 2 - 1, j = len - 1; i > 0; --i) {
        buf[j--] = buf[i];
        buf[j--] = buf[i];
    }
}


static void
stereoToMono(float dst[], float src[], int srcLen)
{
    for (int i = 0, j = 0; i < srcLen; i += 2, ++j) {
        dst[j] = src[i] * 0.5f;
        dst[j] += src[i + 1] * 0.5f;
    }
}


int
Aulib::AudioDecoder::decode(float buf[], int len, bool& callAgain)
{
    const SDL_AudioSpec& spec = Aulib::spec();

    if (this->getChannels() == 1 and spec.channels == 2) {
        int srcLen = this->doDecoding(buf, len / 2, callAgain);
        monoToStereo(buf, srcLen * 2);
        return srcLen * 2;
    }

    if (this->getChannels() == 2 and spec.channels == 1) {
        static float* stereoBuf = nullptr;
        static int stereoBufLen = 0;
        if (stereoBufLen != len * 2) {
            stereoBufLen = len * 2;
            delete[] stereoBuf;
            stereoBuf = new float[stereoBufLen];
        }
        int srcLen = this->doDecoding(stereoBuf, stereoBufLen, callAgain);
        stereoToMono(buf, stereoBuf, srcLen);
        return srcLen / 2;
    }
    return this->doDecoding(buf, len, callAgain);
}
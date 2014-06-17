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
#include "stream.h"
#include "aulib_debug.h"


namespace Aulib {

/// \private
class Stream_priv {
    friend class Stream;

    Stream_priv();
    ~Stream_priv();

    Stream::Callback fFinishCallback;
    Stream::Callback fLoopCallback;
};

} // namespace Aulib


Aulib::Stream_priv::Stream_priv()
{
}


Aulib::Stream_priv::~Stream_priv()
{
}


Aulib::Stream::Stream()
    : d(new Stream_priv)
{
}


Aulib::Stream::~Stream()
{
    delete d;
}


void
Aulib::Stream::setFinishCallback(Callback func)
{
    d->fFinishCallback = func;
}


void
Aulib::Stream::unsetFinishCallback()
{
    d->fFinishCallback.clear();
}


void
Aulib::Stream::setLoopCallback(Callback func)
{
    d->fLoopCallback = func;
}


void
Aulib::Stream::unsetLoopCallback()
{
    d->fLoopCallback.clear();
}


void
Aulib::Stream::invokeFinishCallback()
{
    if (d->fFinishCallback) {
        d->fFinishCallback(*this);
    }
}


void
Aulib::Stream::invokeLoopCallback()
{
    if (d->fLoopCallback) {
        d->fLoopCallback(*this);
    }
}
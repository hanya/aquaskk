/* -*- C++ -*-

   MacOS X implementation of the SKK input method.

   Copyright (C) 2007 Tomotaka SUWA <t.suwa@mac.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef HaikuAnnotator_h
#define HaikuAnnotator_h

#include "SKKAnnotator.h"

class BMessage;

class HaikuLooper;


class HaikuAnnotator : public SKKAnnotator
{
public:
    HaikuAnnotator(HaikuLooper* looper);

    virtual void Update(const SKKCandidate& candidate, int cursorOffset);
    void ShowAt(BMessage* msg);
private:
    virtual void SKKWidgetShow();
    virtual void SKKWidgetHide();

    HaikuLooper* fLooper;
    bool fShowRequested;
};


#endif

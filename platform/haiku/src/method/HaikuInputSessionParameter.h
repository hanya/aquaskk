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

#ifndef HaikuInputSessionParameter_h
#define HaikuInputSessionParameter_h

#include "SKKInputSessionParameter.h"

#include <memory>

class HaikuLooper;
class HaikuMessenger;
class HaikuCandidateWindow;
class HaikuAnnotator;
class HaikuDynamicCompletor;
class SKKMessenger;
class SKKCandidateWindow;
class SKKAnnotator;
class SKKDynamicCompletor;


class HaikuInputSessionParameter : public SKKInputSessionParameter
{
    std::auto_ptr<SKKConfig> config_;
    std::auto_ptr<SKKFrontEnd> frontend_;
    SKKMessenger* messenger_;
    std::auto_ptr<SKKClipboard> clipboard_;
    SKKCandidateWindow* candidateWindow_;
    SKKAnnotator* annotator_;
    SKKDynamicCompletor* completor_;
public:
    HaikuInputSessionParameter(HaikuLooper* looper,
        HaikuMessenger* messenger,
        HaikuCandidateWindow* candidateWindow,
        HaikuAnnotator* annotation,
        HaikuDynamicCompletor* completor);

    virtual SKKConfig* Config();
    virtual SKKFrontEnd* FrontEnd();
    virtual SKKMessenger* Messenger();
    virtual SKKClipboard* Clipboard();
    virtual SKKCandidateWindow* CandidateWindow();
    virtual SKKAnnotator* Annotator();
    virtual SKKDynamicCompletor* DynamicCompletor();
};

#endif

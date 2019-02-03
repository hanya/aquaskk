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


#include "HaikuInputSessionParameter.h"

#include "HaikuConfig.h"
#include "HaikuFrontEnd.h"
#include "HaikuClipboard.h"
#include "HaikuLooper.h"
#include "HaikuMessenger.h"
#include "HaikuCandidateWindow.h"
#include "HaikuAnnotator.h"
#include "HaikuDynamicCompletor.h"


HaikuInputSessionParameter::HaikuInputSessionParameter(HaikuLooper* looper,
                                HaikuMessenger* messenger,
                                HaikuCandidateWindow* candidateWindow,
                                HaikuAnnotator* annotator,
                                HaikuDynamicCompletor* completor)
    : config_(new HaikuConfig())
    , frontend_(new HaikuFrontEnd(looper))
    , messenger_(messenger)
    , clipboard_(new HaikuClipboard())
    , candidateWindow_(candidateWindow)
    , annotator_(annotator)
    , completor_(completor)
{
}

SKKConfig* HaikuInputSessionParameter::Config() {
    return config_.get();
}

SKKFrontEnd* HaikuInputSessionParameter::FrontEnd() {
    return frontend_.get();
}

SKKMessenger* HaikuInputSessionParameter::Messenger() {
    return messenger_;
}

SKKClipboard* HaikuInputSessionParameter::Clipboard() {
    return clipboard_.get();
}

SKKCandidateWindow* HaikuInputSessionParameter::CandidateWindow() {
    return candidateWindow_;
}

SKKAnnotator* HaikuInputSessionParameter::Annotator() {
    return annotator_;
}

SKKDynamicCompletor* HaikuInputSessionParameter::DynamicCompletor() {
    return completor_;
}


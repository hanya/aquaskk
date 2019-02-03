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

#include "HaikuDynamicCompletor.h"

#include <Message.h>

#include "CompletionWindow.h"
#include "HaikuLooper.h"


HaikuDynamicCompletor::HaikuDynamicCompletor(HaikuLooper* looper, const char* selectLabel)
    : fLooper(looper)
    , fShowRequested(false)
{
    fCompletionWindow = new CompletionWindow(selectLabel);
}


HaikuDynamicCompletor::~HaikuDynamicCompletor()
{
    fCompletionWindow->Quit();
}


void HaikuDynamicCompletor::Update(const std::string& completion,
                        int commonPrefixLength, int cursorOffset)
{
    fCompletion = completion;
    fCommonPrefixLength = commonPrefixLength;
    fCursorOffset = cursorOffset;
}


void HaikuDynamicCompletor::ShowAt(BMessage* msg)
{
    if (fShowRequested) {
        fShowRequested = false;
        fCompletionWindow->MessageReceived(msg);
        fCompletionWindow->SetWorkspaces(B_CURRENT_WORKSPACE);
        if (fCompletionWindow->IsHidden()) {
            fCompletionWindow->Show();
        }
    }
}


void HaikuDynamicCompletor::SKKWidgetShow()
{
    fCandidates.clear();
    if (!fCompletion.empty()) {
        std::string::size_type offset = 0;
        while (1) {
            std::string::size_type pos = fCompletion.find("\n", offset);
            if (pos == std::string::npos) {
                fCandidates.push_back(fCompletion.substr(offset));
                break;
            }
            fCandidates.push_back(fCompletion.substr(offset, pos - offset));
            offset = pos + 1;
        }
        fCompletionWindow->SetCandidates(fCandidates, 0, fCursorOffset);
        fShowRequested = true;
        fLooper->AskForLocation();
    } else {
        // force to hide if empty
        if (!fCompletionWindow->IsHidden()) {
            fShowRequested = false;
            fCompletionWindow->Hide();
        }
    }
#if DEBUG
    fLooper->_SendLog("DynamicCompletor.Show");
#endif
}


void HaikuDynamicCompletor::SKKWidgetHide()
{
    fShowRequested = false;
    if (!fCompletionWindow->IsHidden()) {
        fCompletionWindow->Hide();
    }
#if DEBUG
    fLooper->_SendLog("DynamicCompletor.Hide");
#endif
}

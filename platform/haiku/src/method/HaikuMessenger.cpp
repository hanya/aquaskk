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

#include "HaikuMessenger.h"

#include "HaikuLooper.h"
#include "MessageWindow.h"
#include "SKKConstVars.h"

#include <Beep.h>


HaikuMessenger::HaikuMessenger(HaikuLooper* looper)
    : fLooper(looper)
    , fShowRequested(false)
{
    fMessageWindow = new MessageWindow();
}


void HaikuMessenger::SendMessage(const std::string& msg)
{
    fMessage = msg;
    _Show();
}


void HaikuMessenger::Beep()
{
    if (SKKUserDefaultKeys::beep_on_registration) {
        beep();
    }
}


void HaikuMessenger::ShowAtCursor(BMessage* msg)
{
    if (fShowRequested) {
        fShowRequested = false;
        fMessageWindow->MessageReceived(msg);
        fMessageWindow->SetWorkspaces(B_CURRENT_WORKSPACE);
        if (fMessageWindow->IsHidden()) {
            fMessageWindow->Show();
        }
    }
}


void HaikuMessenger::_Show()
{
    fShowRequested = true;
    fMessageWindow->SetMessage(fMessage);
    fLooper->AskForCursorLocation();
}


void HaikuMessenger::Hide()
{
    fShowRequested = false;
    if (!fMessageWindow->IsHidden()) {
        fMessageWindow->Hide();
    }
}

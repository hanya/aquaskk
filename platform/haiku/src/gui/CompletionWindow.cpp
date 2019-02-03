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

#include "CompletionWindow.h"

#include "CompletionView.h"

#include <Input.h>


CompletionWindow::CompletionWindow(const char* label)
    : BWindow(
        BRect(100, 100, 200, 250),
        "CompletionWindow",
        B_NO_BORDER_WINDOW_LOOK,
        B_FLOATING_ALL_WINDOW_FEEL,
        B_NOT_RESIZABLE | B_NOT_CLOSABLE | B_NOT_ZOOMABLE |
        B_NOT_MINIMIZABLE | B_NOT_MOVABLE |
        B_AVOID_FOCUS |
        B_NOT_ANCHORED_ON_ACTIVATE)
{
    fCompletionView = new CompletionView(Bounds(), label);
    AddChild(fCompletionView);

    Run();
    fCompletionView->Init();
}


void CompletionWindow::MessageReceived(BMessage* msg)
{
    switch(msg->what)
    {
        case B_INPUT_METHOD_EVENT:
        {
            if (msg->GetInt32("be:opcode", 0) == B_INPUT_METHOD_LOCATION_REQUEST) {
                BPoint offset = fCompletionView->GetOffset();
                BPoint point;
                float height = 0;
                if (msg->FindPoint("be:location_reply", 1, &point) == B_OK &&
                    msg->FindFloat("be:height_reply", 1, &height) == B_OK) {
                } else if (msg->FindPoint("be:location_reply", 1, &point) == B_OK &&
                           msg->FindFloat("be:height_reply", 1, &height) == B_OK) {
                } else {
                    break;
                }
                MoveTo(point.x - offset.x, point.y + height + 1);
            }
            break;
        }
        default:
        {
            BWindow::MessageReceived(msg);
            break;
        }
    }
}


void CompletionWindow::SetCandidates(std::vector<std::string>& candidates,
        int prefixLength, int offset) const
{
    fCompletionView->SetCandidates(candidates, prefixLength, offset);
}


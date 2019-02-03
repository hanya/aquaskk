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

#include "CandidateWindow.h"

#include "CandidateView.h"

#include <Input.h>


CandidateWindow::CandidateWindow(bool upwards, bool horizontal)
    : BWindow(
        BRect(100, 100, 200, 250),
        "CandidateWindow",
        B_NO_BORDER_WINDOW_LOOK,
        B_FLOATING_ALL_WINDOW_FEEL,
        B_NOT_RESIZABLE | B_NOT_CLOSABLE | B_NOT_ZOOMABLE |
        B_NOT_MINIMIZABLE | B_NOT_MOVABLE |
        B_AVOID_FOCUS |
        B_NOT_ANCHORED_ON_ACTIVATE)
    , fUpwards(upwards)
    , fHorizontal(horizontal)
{
    if (fHorizontal) {
        fCandidateView = new HorizontalCandidateView(Bounds());
    } else {
        fCandidateView = new VerticalCandidateView(Bounds());
    }
    AddChild(fCandidateView);

    Run();
    fCandidateView->Init();
}


void CandidateWindow::MessageReceived(BMessage* msg)
{
    switch(msg->what)
    {
        case B_INPUT_METHOD_EVENT:
        {
            if (msg->GetInt32("be:opcode", 0) == B_INPUT_METHOD_LOCATION_REQUEST) {
                BPoint offset = fCandidateView->GetOffset();
                BPoint point;
                float height = 0;
                if (msg->FindPoint("be:location_reply", 1, &point) == B_OK &&
                    msg->FindFloat("be:height_reply", 1, &height) == B_OK) {
                } else if (msg->FindPoint("be:location_reply", 0, &point) == B_OK &&
                           msg->FindFloat("be:height_reply", 0, &height) == B_OK) {
                } else {
                    break;
                }
                if (!fUpwards || !fHorizontal) {
                    MoveTo(point.x - offset.x, point.y + height + 1);
                } else {
                    MoveTo(point.x - offset.x, point.y - offset.y - 1);
                }
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


void CandidateWindow::SetFontAndSize(const char* name, int32 size) const
{
    fCandidateView->SetFontAndSize(name, size);
}


void CandidateWindow::SetLabels(const std::string& labels) const
{
    fCandidateView->SetLabels(labels);
}


void CandidateWindow::SetCandidates(
        std::vector<std::string>& candidates, int cursor) const
{
    fCandidateView->SetCandidates(candidates, cursor);
}


void CandidateWindow::SetPage(int page, int total) const
{
    fCandidateView->SetPage(page, total);
}


float CandidateWindow::GetTextLeft() const
{
    return fCandidateView->GetTextLeft();
}


float CandidateWindow::GetDefaultCellWidth() const
{
    return fCandidateView->GetDefaultCellWidth();
}


float CandidateWindow::GetCellWidth(const char* str) const
{
    return fCandidateView->GetCellWidth(str);
}

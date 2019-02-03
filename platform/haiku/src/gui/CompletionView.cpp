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

#include "CompletionView.h"

#include <Window.h>


const float border = 1.;


CompletionView::CompletionView(BRect frame, const char* label)
    : VerticalCandidateView(frame)
{
    fSelectByTabLabel = label;
    Reset();
}


BPoint CompletionView::GetOffset()
{
    return BPoint(border + fHalfwidthSpaceWidth, 0);
}


void CompletionView::Reset()
{
    fLabelWidth = StringWidth(fSelectByTabLabel);
}


void CompletionView::_CalculateSize()
{
    float maxWidth = fLabelWidth;
    for (int i = 0; i < fCandidates.size(); i++) {
        maxWidth = std::max(StringWidth(fCandidates[i].c_str()), maxWidth);
    }

    const float totalWidth = border * 2 +
                                maxWidth + fHalfwidthSpaceWidth * 2;
    fHeight = (fCandidates.size() + 1) * fLineHeight + border * 3;
    Window()->ResizeTo(totalWidth, fHeight);
}


void CompletionView::SetCandidates(const std::vector<std::string>& candidates,
        int prefixLength, int offset)
{
    std::lock_guard<std::mutex> lock(fMutex);

    fCandidates = candidates;
    fCursor = offset;

    _CalculateSize();
    /*
    if (!Window()->IsHidden()) {
        DelayedInvalidate(50 * 1000);
    }
    */
}


void CompletionView::Draw(BRect rect)
{
    std::lock_guard<std::mutex> lock(fMutex);

    // A
    // B
    // Select by Tab
    _DrawClear();

    // draw candidates
    float y = fFirstLineY;
    float x = border + fHalfwidthSpaceWidth;

    for (int i = 0; i < fCandidates.size(); i++) {
        DrawString(fCandidates[i].c_str(), BPoint(x, y));
        y += fLineHeight;
    }
    // draw information
    SetHighColor(fPaginationColor);
    DrawString(fSelectByTabLabel,
        BPoint(ceilf((Frame().Width() - fLabelWidth)/2), y + border));

    // separator between information
    SetHighColor(fBorderColor);
    StrokeLine(BPoint(border, fCandidates.size() * fLineHeight + border),
            BPoint(Frame().Width() - border, fCandidates.size() * fLineHeight + border),
            B_SOLID_HIGH);

    // selected background
    if (0 <= fCursor && fCursor < fCandidates.size()) {
        BRect rect(border, border + fCursor * fLineHeight,
                Frame().Width() - border, 0);
        rect.bottom = rect.top + fLineHeight;
        _DrawSelection(rect);
    }
}

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

#include "CandidateView.h"

#include <Window.h>

#include <sstream>

const float border = 1.;


CandidateView::CandidateView(BRect frame)
    : BView(
        frame,
        "candidates",
        B_FOLLOW_ALL_SIDES,
        B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW)
    , fCursor(0)
    , fPage(0)
    , fTotal(0)
{
    fTextColor = HighColor();
    fBackgroundColor = LowColor();
    fShortcutColor = HighColor();
    fShortcutBackColor = {0xF0, 0xF0, 0xF0, 1};
    fBorderColor = {0xB0, 0xB0, 0xB0, 1};
    fPaginationColor = {0x50, 0x50, 0x50, 1};

    fCandidates.resize(10);
    fCandidateWidth.resize(10);
    fLabelOffset.resize(10);
}


CandidateView::~CandidateView()
{
}


BPoint CandidateView::GetOffset()
{
    return BPoint(GetTextLeft(), 0);
}


float CandidateView::GetTextLeft() const
{
    return border + fShortcutWidth + fHalfwidthSpaceWidth;
}


float CandidateView::GetDefaultCellWidth() const
{
    return fShortcutWidth + fDefaultCandidateWidth;
}


float CandidateView::GetCellWidth(const char* str) const
{
    float width = StringWidth(str);
    return fShortcutWidth + width + fHalfwidthSpaceWidth * 2;
}


void CandidateView::SetFontAndSize(const char* name, int32 size)
{
    BFont font;
    font.SetFamilyAndFace(name, 0);
    font.SetSize(size);
    SetFont(&font);
}


void CandidateView::Init()
{
    std::lock_guard<std::mutex> lock(fMutex);

    BFont font;
    GetFont(&font);
    font_height fontHeight;
    font.GetHeight(&fontHeight);
    const float ascent = ceilf(fontHeight.ascent);
    const float lineHeight = ceilf(fontHeight.ascent +
                    fontHeight.descent + fontHeight.leading);

    fLineHeight = lineHeight;
    fFirstLineY = border + ascent;

    fHeight = fLineHeight + border + border;

    const char *fullwidthSpace = "\xe3\x80\x80";
    fFullwidthSpaceWidth = ceilf(StringWidth(fullwidthSpace, 3));
    fHalfwidthSpaceWidth = ceilf(StringWidth(" ", 1));
    fShortcutCharWidth = ceilf(StringWidth("W", 1));

    fShortcutWidth = fHalfwidthSpaceWidth * 2 + fShortcutCharWidth;
    fDefaultCandidateWidth = fHalfwidthSpaceWidth * 2 + fFullwidthSpaceWidth * 2;
}


void CandidateView::SetLabels(const std::string& labels)
{
    std::lock_guard<std::mutex> lock(fMutex);

    fLabels = labels;

    _CalculateLabelWidth();
}


void CandidateView::SetCandidates(const std::vector<std::string>& candidates, int cursor)
{
    std::lock_guard<std::mutex> lock(fMutex);

    fCursor = cursor;
    fCandidates = candidates;

    _CalculateSize();
}


void CandidateView::SetPage(int page, int total)
{
    std::lock_guard<std::mutex> lock(fMutex);

    fPage = page;
    fTotal = total;

    std::ostringstream s;
    s << fPage << '/' << fTotal;
    fPagination = s.str();
}


void CandidateView::_CalculateLabelWidth()
{
    fLabelOffset.clear();

    const float halfShortcutWidth = fShortcutWidth / 2;
    const char* label = fLabels.c_str();

    for (int i = 0; i < fLabels.size(); i++) {
        fLabelOffset.push_back(halfShortcutWidth -
                                ceilf(StringWidth(label, 1) / 2));
        label++;
    }
}


void CandidateView::_CalculateSize()
{
    //' A ' XXXX ' S '
    float width = 0;

    const float paginationWidth = StringWidth(fPagination.c_str());
    width = border * 2 +
        paginationWidth + fHalfwidthSpaceWidth * 2;

    Window()->ResizeTo(100, 100);
}


void CandidateView::_DrawClear()
{
    SetHighColor(fTextColor);
    SetLowColor(fBackgroundColor);

    float width = Frame().Width();
    float height = Frame().Height();

    // clear
    BRect areaRect(border, border, width - border * 2, height - border * 2);
    FillRect(areaRect, B_SOLID_LOW);

    // draw border
    SetPenSize(1.0);
    SetHighColor(fBorderColor);
    StrokeRect(Frame());

    SetHighColor(fTextColor);
}


void CandidateView::_DrawSelection(BRect rect)
{
    //source_alpha srcAlpha;
    //alpha_function alphaFunc;
    //GetBlendingMode(&srcAlpha, &alphaFunc);
    //drawing_mode drawingMode = DrawingMode();
    //SetDrawingMode(B_OP_ALPHA);
    //SetBlendingMode(B_CONSTANT_ALPHA, B_ALPHA_OVERLAY);
    //SetHighColor(0x90, 0x90, 0xFF, 15);
    //FillRect(rect);
    SetHighColor(0x90, 0x90, 0xFF, 100);
    StrokeRect(rect);
    //SetBlendingMode(srcAlpha, alphaFunc);
    //SetDrawingMode(drawingMode);
}


VerticalCandidateView::VerticalCandidateView(BRect frame)
    : CandidateView(frame)
{
}


BPoint VerticalCandidateView::GetOffset()
{
    const float x = border + fShortcutWidth + fHalfwidthSpaceWidth;
    return BPoint(x, 0);
}


void VerticalCandidateView::_CalculateSize()
{
    float maxWidth = 0;

    for (int i = 0; i < fCandidates.size(); i++) {
        float width = StringWidth(fCandidates[i].c_str());
        maxWidth = std::max(width, maxWidth);
    }

    const float totalWidth = border * 2 + fShortcutWidth +
                maxWidth + fHalfwidthSpaceWidth * 2;
    fHeight = (fCandidates.size() + 1) * fLineHeight + border * 3;
    Window()->ResizeTo(totalWidth, fHeight);
}


void VerticalCandidateView::Draw(BRect rect)
{
    std::lock_guard<std::mutex> lock(fMutex);

    _DrawVertical();
}


void VerticalCandidateView::_DrawVertical()
{
    // A foo
    // S bar
    //   1/2
    _DrawClear();

    // draw shortcut background
    SetHighColor(fShortcutBackColor);
    FillRect(BRect(border, border,
            border + fShortcutWidth,
            fLineHeight * fCandidates.size()));
    SetHighColor(fTextColor);

    // draw candidates
    float y = fFirstLineY;
    float x = border + fShortcutWidth + fHalfwidthSpaceWidth;
    const char* labels = fLabels.c_str();

    for (int i = 0; i < fCandidates.size(); i++) {
        // draw shortcut label
        DrawString(labels, 1, BPoint(border + fLabelOffset[i], y));

        DrawString(fCandidates[i].c_str(), BPoint(x, y));

        y += fLineHeight;

        labels++;
    }
    // draw pagination
    SetHighColor(fPaginationColor);
    DrawString(fPagination.c_str(),
        BPoint(Frame().Width() - border -
            StringWidth(fPagination.c_str()) - fHalfwidthSpaceWidth, y + border));

    // separator between pagination
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


HorizontalCandidateView::HorizontalCandidateView(BRect frame)
    : CandidateView(frame)
{
}


BPoint HorizontalCandidateView::GetOffset()
{
    return BPoint(border + fShortcutWidth + fHalfwidthSpaceWidth,
                  fHeight);
}


void HorizontalCandidateView::_CalculateSize()
{
    //' A ' XXXX ' S ' YYYY 1/2
    float width = 0;
    const float defaultCandidateWidth = fFullwidthSpaceWidth * 2;

    fCandidateWidth.clear();
    for (int i = 0; i < fCandidates.size(); i++) {
        float candidateWidth = StringWidth(fCandidates[i].c_str());
        if (candidateWidth < defaultCandidateWidth) {
            candidateWidth = defaultCandidateWidth;
        }
        width += candidateWidth + fHalfwidthSpaceWidth * 2 + fShortcutWidth;
        fCandidateWidth.push_back(candidateWidth);
    }

    const float paginationWidth = StringWidth(fPagination.c_str());
    width += border * 2 + paginationWidth + fHalfwidthSpaceWidth * 2;

    Window()->ResizeTo(width, fHeight);
}


void HorizontalCandidateView::Draw(BRect rect)
{
    std::lock_guard<std::mutex> lock(fMutex);

    _DrawHorizontal();
}


void HorizontalCandidateView::_DrawHorizontal()
{
    // A foo S bar D hog 1/2
    _DrawClear();

    // draw candidates
    float y = fFirstLineY;
    float x = border;
    const char* labels = fLabels.c_str();

    for (int i = 0; i < fCandidates.size(); i++) {
        // draw shortcut background
        SetHighColor(fShortcutBackColor);
        FillRect(BRect(x, border, x + fShortcutWidth, fHeight - border));
        SetHighColor(fTextColor);

        // draw shortcut label
        DrawString(labels, 1, BPoint(x + fLabelOffset[i], y));

        x += fShortcutWidth + fHalfwidthSpaceWidth;

        // draw candidate
        DrawString(fCandidates[i].c_str(), BPoint(x, y));

        const float width = fCandidateWidth[i];
        if (width + fHalfwidthSpaceWidth * 2 < fDefaultCandidateWidth) {
            x += fDefaultCandidateWidth;
        } else {
            x += width + fHalfwidthSpaceWidth;
        }

        labels++;
    }

    // draw pagination
    SetHighColor(fPaginationColor);
    DrawString(fPagination.c_str(), BPoint(x + fHalfwidthSpaceWidth, y));

    // selected background
    if (0 <= fCursor && fCursor < fCandidates.size()) {
        float x = border;
        float width = 0;
        for (int i = 0; i < fCandidateWidth.size(); i++) {
            width = fShortcutWidth + fHalfwidthSpaceWidth * 2 + fCandidateWidth[i];
            if (i == fCursor) {
                break;
            }
            x += width;
        }
        BRect rect(x, border, x + width, fHeight - border);
        _DrawSelection(rect);
    }
}


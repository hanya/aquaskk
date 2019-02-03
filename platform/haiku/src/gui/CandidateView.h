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

#ifndef CandidateView_h
#define CandidateView_h

#include <View.h>

#include <mutex>
#include <string>
#include <vector>


class CandidateView : public BView
{
public:
             CandidateView(BRect frame);
    virtual ~CandidateView();

        void  Init();
        void  SetLabels(const std::string& labels);
        void  SetCandidates(const std::vector<std::string>& candidates, int cursor);
        void  SetPage(int page, int total);
        float GetTextLeft() const;
        float GetDefaultCellWidth() const;
        float GetCellWidth(const char* str) const;
    virtual BPoint GetOffset();
        void  SetFontAndSize(const char* name, int32 size);
protected:
    std::vector<std::string> fCandidates;
    std::vector<float> fCandidateWidth;
    std::string fLabels;
    std::string fPagination;
    std::mutex  fMutex;
    int fCursor;
    int fPage;
    int fTotal;

    void _DrawClear();
    void _DrawSelection(BRect rect);
    virtual void _CalculateSize();
    virtual void _CalculateLabelWidth();

    float fLineHeight;
    float fHeight;
    float fHalfwidthSpaceWidth;
    float fFullwidthSpaceWidth;
    float fShortcutCharWidth;
    float fShortcutWidth;
    float fDefaultCandidateWidth;
    float fFirstLineY;

    rgb_color fTextColor;
    rgb_color fBackgroundColor;
    rgb_color fShortcutColor;
    rgb_color fShortcutBackColor;
    rgb_color fBorderColor;
    rgb_color fPaginationColor;

    std::vector<float> fLabelOffset;
};


class VerticalCandidateView : public CandidateView
{
public:
    VerticalCandidateView(BRect frame);
    virtual ~VerticalCandidateView() {};

    virtual void Draw(BRect rect);

    virtual BPoint GetOffset();
protected:
    void _DrawVertical();
    virtual void _CalculateSize();
};


class HorizontalCandidateView : public CandidateView
{
public:
    HorizontalCandidateView(BRect frame);
    virtual ~HorizontalCandidateView() {};

    virtual void Draw(BRect rect);

    virtual BPoint GetOffset();
private:
    void _DrawHorizontal();
    virtual void _CalculateSize();
};


#endif

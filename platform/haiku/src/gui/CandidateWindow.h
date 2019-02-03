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

#ifndef CandidateWindow_h
#define CandidateWindow_h

#include <Window.h>

#include <string>
#include <vector>

class CandidateView;


class CandidateWindow : public BWindow
{
public:
    CandidateWindow(bool upwards, bool horizontal);

    virtual void MessageReceived(BMessage* msg);
    void SetLabels(const std::string& labels) const;
    void SetCandidates(std::vector<std::string>& candidates, int cursor) const;
    void SetPage(int page, int total) const;

    void SetFontAndSize(const char* name, int32 size) const;
    float GetTextLeft() const;
    float GetDefaultCellWidth() const;
    float GetCellWidth(const char* str) const;
private:
    CandidateView*  fCandidateView;
    bool    fUpwards;
    bool    fHorizontal;
};

#endif

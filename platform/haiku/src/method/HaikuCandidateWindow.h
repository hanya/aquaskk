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

#ifndef HaikuCandidateWindow_h
#define HaikuCandidateWindow_h

#include "SKKCandidateWindow.h"

#include <string>
#include <vector>

class BMessage;
class HaikuLooper;

class CandidateWindow;


class HaikuCandidateWindow : public SKKCandidateWindow
{
public:
             HaikuCandidateWindow(HaikuLooper* looper);
    virtual ~HaikuCandidateWindow();

    virtual void Setup(SKKCandidateIterator begin, SKKCandidateIterator end,
                       std::vector<int>& pages);
    virtual void Update(SKKCandidateIterator begin, SKKCandidateIterator end,
                        int cursor, int page_pos, int page_max);
    virtual int LabelIndex(char label);
    void ShowAt(BMessage* msg);
    void Reload();
private:
    virtual void SKKWidgetShow();
    virtual void SKKWidgetHide();

    void _ReloadUserDefaults();

    HaikuLooper*  fLooper;
    CandidateWindow* fCandidateWindow;
    std::vector<std::string> fCandidates;
    std::string fLabels;
    int fCursor;
    int fPage;
    int fTotal;
    bool fHorizontal;
    bool fTextHorizontal;
    bool fShowRequested;
};


#endif

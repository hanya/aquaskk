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

#include "HaikuCandidateWindow.h"

#include "CandidateWindow.h"
#include "HaikuLooper.h"
#include "SKKConstVars.h"
#include "utf8util.h"


HaikuCandidateWindow::HaikuCandidateWindow(HaikuLooper* looper)
    : fLooper(looper)
    , fCandidateWindow(NULL)
    , fCursor(0)
    , fPage(0)
    , fTotal(0)
    , fHorizontal(true)
    , fTextHorizontal(true)
    , fShowRequested(false)
{
    Reload();
}


HaikuCandidateWindow::~HaikuCandidateWindow()
{
    fCandidateWindow->Quit();
}


void HaikuCandidateWindow::Setup(
        SKKCandidateIterator begin,
        SKKCandidateIterator end, std::vector<int>& pages)
{
    pages.clear();
    if (begin == end) {
        return;
    }
    if (fHorizontal) {
        std::vector<int> cellWidth;
        const int defaultCellWidth = fCandidateWindow->GetDefaultCellWidth();

        while(begin != end)
        {
            std::string candidate(begin->Variant());
            int width = 0;
            if (utf8::length(candidate) < 3) {
                width = defaultCellWidth;
            } else {
                width = fCandidateWindow->GetCellWidth(candidate.c_str());
            }
            cellWidth.push_back(width);
            begin++;
        }

        int limit = defaultCellWidth * fLabels.size();
#if DEBUG
        {
            char s[64];
            sprintf(s, "CandidateWindow.Setup, count: %d, limit: %d, size: %d, de: %d",
                    (int)cellWidth.size(), limit, fLabels.size(), defaultCellWidth);
            fLooper->_SendLog((const char*)s);
        }
#endif
        int offset = 0;
        do {
            int size = 0;
            int count = 0;
            while (offset < cellWidth.size()) {
                if (limit < size + cellWidth[offset]) {
                    if (size == 0) {
                        offset++;
                        count = 1;
                    }
                    break;
                }
                size += cellWidth[offset];
                offset++;
                count++;
            }
            pages.push_back(count);
#if DEBUG
        {
            char s[64];
            sprintf(s, "count: %d", count);
            fLooper->_SendLog((const char*)s);
        }
#endif
        } while (offset < cellWidth.size());
    } else {
        // vertical candidate window
        const int max = fLabels.size() -1;
        int count = 0;
        while (begin != end) {
            count++;
            if (count == max) {
                pages.push_back(count);
                count = 0;
            }
            begin++;
        }
        if (0 < count) {
            pages.push_back(count);
        }
    }
}


void HaikuCandidateWindow::Update(
            SKKCandidateIterator begin, SKKCandidateIterator end,
            int cursor, int page_pos, int page_max)
{
    fCandidates.clear();

    for (SKKCandidateIterator curr = begin; curr != end; curr++) {
        std::string candidate(curr->Variant());
        fCandidates.push_back(candidate);
    }

    fCursor = cursor;
    fPage = page_pos;
    fTotal = page_max;
#if DEBUG
    fLooper->_SendLog("CandidateWindow.Update");
#endif
}


void HaikuCandidateWindow::SKKWidgetShow()
{
    fShowRequested = true;
    fCandidateWindow->SetPage(fPage, fTotal);
    fCandidateWindow->SetCandidates(fCandidates, fCursor);
    fLooper->AskForLocation();
#if DEBUG
    fLooper->_SendLog("CandidateWindow.Show");
#endif
}


void HaikuCandidateWindow::SKKWidgetHide()
{
    fShowRequested = false;
    if (!fCandidateWindow->IsHidden()) {
        fCandidateWindow->Hide();
    }
#if DEBUG
    fLooper->_SendLog("CandidateWindow.Hide");
#endif
}


int HaikuCandidateWindow::LabelIndex(char label)
{
    for (int i = 0; i < fLabels.size(); i++) {
        if (fLabels[i] == label) {
            return i;
        }
    }
    return -1;
}


void HaikuCandidateWindow::ShowAt(BMessage* msg)
{
    if (fShowRequested) {
        fShowRequested = false;
        fCandidateWindow->MessageReceived(msg);
        fCandidateWindow->SetWorkspaces(B_CURRENT_WORKSPACE);
        if (fCandidateWindow->IsHidden()) {
            fCandidateWindow->Show();
        }
    }
}


void HaikuCandidateWindow::Reload()
{
    _ReloadUserDefaults();
}


void HaikuCandidateWindow::_ReloadUserDefaults()
{
    if (fCandidateWindow) {
        fCandidateWindow->Quit();
        //delete fCandidateWindow;
        fCandidateWindow = NULL;
    }
    fCandidateWindow = new CandidateWindow(
            SKKUserDefaultKeys::put_candidate_window_upward,
            !SKKUserDefaultKeys::candidate_window_vertical);
    /*
    if (SKKUserDefaultKeys::use_custom_font) {
        fCandidateWindow->SetFontAndSize(
            SKKUserDefaultKeys::candidate_window_font_name.c_str(),
            SKKUserDefaultKeys::candidate_window_font_size);
    }
    */
    fLabels = SKKUserDefaultKeys::candidate_window_labels;
    fCandidateWindow->SetLabels(fLabels);
}

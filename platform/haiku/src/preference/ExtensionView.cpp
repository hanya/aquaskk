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

#include "ExtensionView.h"

#include <Catalog.h>
#include <CheckBox.h>
#include <LayoutBuilder.h>
#include <ScrollView.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Preference"


ExtensionView::ExtensionView(const char* name)
    : BView(name, B_WILL_DRAW)
{
    mpGridView = new ExtensionGridView<ExtensionRow>(NULL);
    mpGridView->SetDrawingMode(B_OP_OVER);
    BScrollView* pExtList = new BScrollView("extgrid", mpGridView,
            B_FOLLOW_LEFT_TOP, B_FRAME_EVENTS | B_SUPPORTS_LAYOUT,
            false, true, B_FANCY_BORDER);

    BLayoutBuilder::Group<>(this, B_VERTICAL)
        .SetInsets(5, 5, 5, 0)
        .Add(pExtList);

    Layout(true);
}


void ExtensionView::AddRule(const char* id, bool active, const char* description, int32 location)
{
    mpGridView->AddRow(new ExtensionRow(id, active, description, location));
}


void ExtensionView::SetRuleState(const char* id, bool active)
{
    bool invalidate = false;
    const int32 count = mpGridView->CountRows();
    for (int32 i = 0; i < count; i++) {
        ExtensionRow* row = mpGridView->ItemAt(i);
        if (row) {
            if (row->Id().compare(id) == 0) {
                row->SetActive(true);
                invalidate = true;
                break;
            }
        }
    }
    if (invalidate) {
        mpGridView->DelayedInvalidate(100000);
    }
}

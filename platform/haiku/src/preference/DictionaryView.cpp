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

#include "DictionaryView.h"

#include <Button.h>
#include <Catalog.h>
#include <CheckBox.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>
#include <StringView.h>
#include <TextControl.h>

#include "MessageId.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Preference"


DictionaryView::DictionaryView(const char* name, BLooper* looper)
    : BView(name, B_WILL_DRAW)
{
    BStringView* label1 = new BStringView("userdil", B_TRANSLATE("User dictionary"));
    mpUserLocation = new BTextControl("", "", new BMessage());
    mpUserLocationChange = new BButton("userb", B_TRANSLATE("Change..."),
                new BMessage(IMP_DICT_USER_LOCATION));

    mpDictGridView = new DictGridView<DictRow>(looper);
    mpDictGridView->SetDrawingMode(B_OP_OVER);
    BScrollView* pExtList = new BScrollView("extgrid", mpDictGridView,
            B_FOLLOW_LEFT_TOP, B_FRAME_EVENTS | B_SUPPORTS_LAYOUT,
            false, true, B_FANCY_BORDER);

    mpAdd = new BButton("add", B_TRANSLATE("Add"), new BMessage(IMP_DICT_ADD));
    mpRemove = new BButton("remove", B_TRANSLATE("Remove"), new BMessage(IMP_DICT_REMOVE));
    mpUp = new BButton("up", B_TRANSLATE("Up"), new BMessage(IMP_DICT_UP));
    mpDown = new BButton("down", B_TRANSLATE("Down"), new BMessage(IMP_DICT_DOWN));
    mpSelectLocation = new BButton("loc", B_TRANSLATE("Select location..."),
            new BMessage(IMP_DICT_LOCATION));
    mpRemove->SetEnabled(false);
    mpUp->SetEnabled(false);
    mpDown->SetEnabled(false);
    mpSelectLocation->SetEnabled(false);

    mpDownload = new BButton("update", B_TRANSLATE("Update"), new BMessage(IMP_DICT_DOWNLOAD));

    BLayoutBuilder::Group<>(this, B_VERTICAL)
        .SetInsets(5, 5, 5, 0)
        .AddGroup(B_HORIZONTAL)
            .Add(label1)
            .Add(mpUserLocation)
            .Add(mpUserLocationChange)
        .End()
        .Add(pExtList)
        .AddGroup(B_HORIZONTAL)
            .Add(mpAdd)
            .Add(mpRemove)
            .Add(mpUp)
            .Add(mpDown)
            .Add(mpSelectLocation)
            .AddGlue()
            .Add(mpDownload)
        .End()
        ;

    Layout(true);
}


void DictionaryView::MessageReceived(BMessage* msg)
{
    switch (msg->what)
    {
        case IMP_DICT_ADD:
        {
            _AddRow();
            break;
        }
        case IMP_DICT_REMOVE:
        {
            _RemoveRow();
            break;
        }
        case IMP_DICT_UP:
        {
            _MoveRow(true);
            break;
        }
        case IMP_DICT_DOWN:
        {
            _MoveRow(false);
            break;
        }
        /*
        case IMP_DICT_DOWNLOAD:
        {
            Window()->MessageReceived(msg);
            break;
        }
        */
        case GRID_SELECTION_CHANGED:
        {
            _SelectionChanged();
            break;
        }
        default:
            BView::MessageReceived(msg);
            break;
    }
}


void DictionaryView::SetLocation(const char* location)
{
    const int32 selected = mpDictGridView->CurrentSelection();
    if (0 <= selected && selected < mpDictGridView->CountRows()) {
        DictRow* row = mpDictGridView->ItemAt(selected);
        if (row) {
            row->SetLocation(location);
            mpDictGridView->Invalidate();
        }
    }
}


void DictionaryView::_AddRow()
{
    const int32 selected = mpDictGridView->CurrentSelection();
    const int32 index = selected == -1 ? mpDictGridView->CountRows() : selected + 1;
    int32 row, column;
    mpDictGridView->CursorIndex(&row, &column);
    DictRow* pDictRow = new DictRow(false, 0, "", "");
    mpDictGridView->InsertRow(index, pDictRow);
    mpDictGridView->SelectRow(index);
    mpDictGridView->SetCursorIndex(index, column);
}


void DictionaryView::_RemoveRow()
{
    const int32 selected = mpDictGridView->CurrentSelection();
    if (0 <= selected && selected < mpDictGridView->CountRows()) {
        mpDictGridView->RemoveRow(selected);
    }
}


void DictionaryView::_MoveRow(bool up)
{
    const int32 selected = mpDictGridView->CurrentSelection();
    int32 row, column;
    mpDictGridView->CursorIndex(&row, &column);
    mpDictGridView->MoveRow(selected, up);
    mpDictGridView->SelectRow(up ? selected - 1 : selected + 1);
    mpDictGridView->SetCursorIndex(up ? row - 1 : row + 1, column);
    mpDictGridView->Invalidate();
}


void DictionaryView::_SelectionChanged()
{
    const int32 selected = mpDictGridView->CurrentSelection();
    const int32 lastIndex = mpDictGridView->CountRows() - 1;
    if (0 <= selected && selected <= lastIndex) {
        mpRemove->SetEnabled(true);
        mpSelectLocation->SetEnabled(true);
        mpUp->SetEnabled(selected != 0);
        mpDown->SetEnabled(selected != lastIndex);
    } else {
        mpRemove->SetEnabled(false);
        mpUp->SetEnabled(false);
        mpDown->SetEnabled(false);
        mpSelectLocation->SetEnabled(false);
    }
}


void DictionaryView::AddDictionary(bool active, int32 type, const char* location, const char* etag)
{
    mpDictGridView->AddRow(new DictRow(active, type, location, etag));
}

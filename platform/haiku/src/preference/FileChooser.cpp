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

#include "FileChooser.h"

#include <Catalog.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Path.h>
#include <Window.h>

#include "FileFilter.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Preference"


FileChooser::FileChooser(BLooper* looper,
        file_panel_mode mode, FileFilter* filter, int32 kind)
    : BLooper("chooser"),
      fTarget(looper),
      fFileFilter(filter),
      fKind(kind)
{
    BMessenger messenger(NULL, this);
    fFilePanel = std::unique_ptr<BFilePanel>(
                    new BFilePanel(mode, &messenger, NULL,
                        0, false, NULL, filter, false, true));

    BMenuBar* bar = dynamic_cast<BMenuBar*>(
                    fFilePanel->Window()->FindView("MenuBar"));
    if (bar != NULL) {
        // add filter
        if (fFileFilter != NULL) {
            fFilterMenu = new BMenu(B_TRANSLATE("Filter"));
            bar->AddItem(fFilterMenu);
            for (int i = 0; i < fFileFilter->FilterCount(); ++i) {
                fFilterMenu->AddItem(new BMenuItem(
                        fFileFilter->FilterName(i),
                        _CreateFilterMessage(i)));
            }
            if (fFileFilter->FilterIndex() >= 0) {
                if (fFilterMenu->CountItems() > fFileFilter->FilterIndex()) {
                    fFilterMenu->ItemAt(fFileFilter->FilterIndex())->SetMarked(true);
                }
            } else if (fFilterMenu->CountItems() > 0) {
                fFileFilter->SetFilterIndex(0);
                fFilterMenu->ItemAt(0)->SetMarked(true);
            }

            fFilterMenu->SetTargetForItems(messenger);
        }
    }
    Run();
}

FileChooser::~FileChooser()
{
}

bool FileChooser::QuitRequested()
{
    return true;
}

BMessage* FileChooser::_CreateFilterMessage(int32 index)
{
    BMessage* msg = new BMessage(CHANGE_FILTER);
    msg->AddInt32("index", index);
    return msg;
}

void FileChooser::MessageReceived(BMessage *msg)
{
    switch (msg->what)
    {
        case B_REFS_RECEIVED:
        {
            entry_ref ref;
            if (msg->FindRef("refs", &ref) == B_OK) {
                BMessage mess(B_REFS_RECEIVED);
                mess.AddRef("refs", &ref);
                mess.AddInt32("kind", fKind);
                fTarget->PostMessage(&mess);
            }
            break;
        }
        case B_SAVE_REQUESTED:
        {
            entry_ref ref;
            const char* name = NULL;
            if (msg->FindRef("directory", &ref) == B_OK &&
                msg->FindString("name", &name) == B_OK) {
                BMessage mess(B_SAVE_REQUESTED);
                mess.AddRef("directory", &ref);
                mess.AddString("name", name);
                mess.AddInt32("kind", fKind);
                fTarget->PostMessage(&mess);
            }
            break;
        }
        case CHANGE_FILTER:
        {
            int32 index;
            if (msg->FindInt32("index", &index) == B_OK) {
                if (fFileFilter->SetFilterIndex(index)) {
                    _SwitchMenuItem(fFilterMenu, "index", index);
                    fFilePanel->Refresh();
                }
            }
            break;
        }
        default:
        {
            BHandler::MessageReceived(msg);
            break;
        }
    }
}

void FileChooser::_SwitchMenuItem(BMenu* menu, const char* name, int32 v)
{
    BMenuItem* item = menu->FindMarked();
    if (item != NULL) {
        item->SetMarked(false);
    }
    for (int i = 0; i < menu->CountItems(); i++) {
        BMenuItem* item = menu->ItemAt(i);
        if (item != NULL) {
            BMessage* mess = item->Message();
            if (mess != NULL) {
                int32 t = -1;
                if (mess->FindInt32(name, &t) == B_OK && t == v) {
                    item->SetMarked(true);
                }
            }
        }
    }
}

void FileChooser::Show()
{
    fFilePanel->Show();
}


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

#ifndef FileChooser_H
#define FileChooser_H

#include <FilePanel.h>
#include <Looper.h>

#include <memory>

class BMenu;

class FileFilter;


class FileChooser : public BLooper
{
public:
    FileChooser(BLooper* looper,
        file_panel_mode mode=B_OPEN_PANEL, FileFilter* filter=NULL, int32 kind=0);
    virtual ~FileChooser();

    virtual void MessageReceived(BMessage* msg);
    virtual bool QuitRequested();

    void Show();
private:
    BMessage * _CreateFilterMessage(int32 index);
    void _SwitchMenuItem(BMenu* menu, const char* name, int32 v);

    FileFilter* fFileFilter;
    std::unique_ptr<BFilePanel> fFilePanel;
    int32 fKind;
    BMenu* fFilterMenu;
    BLooper* fTarget;

    static const int32 CHANGE_FILTER = 'chfl';
};


#endif // FileChooser_H

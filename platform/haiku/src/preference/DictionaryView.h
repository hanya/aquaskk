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

#ifndef DictionaryView_h
#define DictionaryView_h

#include <View.h>

#include "DictionaryGridView.h"

class BButton;
class BLooper;
class BTextControl;


class DictionaryView : public BView
{
public:
    DictionaryView(const char* name, BLooper* looper);
    virtual ~DictionaryView() {}

    virtual void MessageReceived(BMessage* msg);

    void AddDictionary(bool active, int32 type, const char* location, const char* etag);
    void SetLocation(const char* location);

    BTextControl* mpUserLocation;
    BButton* mpUserLocationChange;
    DictGridView<DictRow>* mpDictGridView;
    BButton* mpAdd;
    BButton* mpRemove;
    BButton* mpSelectLocation;
    BButton* mpUp;
    BButton* mpDown;
    BButton* mpDownload;
private:
    void _SelectionChanged();
    void _AddRow();
    void _RemoveRow();
    void _MoveRow(bool up);
};

#endif

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

#ifndef MessageWindow_h
#define MessageWindow_h

#include <Window.h>

#include <string>

class BMessageRunner;
class BPath;

class MessageView;


class MessageWindow : public BWindow
{
public:
             MessageWindow(bigtime_t delay=1000*2000);
    virtual ~MessageWindow();

    virtual void MessageReceived(BMessage* msg);

    void SetMessage(const std::string& msg);
private:
    BMessageRunner*  fCloseRunner;
    bigtime_t        fDelay;
    MessageView*   fMessageView;

    void _ShowAndSetCloseDelay(BPoint point, bigtime_t delay);
    void _Init();
    BBitmap* _LoadBitmap(BPath* path, const char* name);
};


#endif

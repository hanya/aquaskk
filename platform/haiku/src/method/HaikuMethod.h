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

#ifndef HaikuMethod_h
#define HaikuMethod_h

#include <Messenger.h>
#include <SupportDefs.h>
#include <add-ons/input_server/InputServerMethod.h>

class BMessenger;


class HaikuMethod : public BInputServerMethod
{
public:
             HaikuMethod();
    virtual ~HaikuMethod();

    virtual status_t      InitCheck();
    virtual filter_result Filter(BMessage* msg, BList* _list);
    virtual status_t      MethodActivated(bool active);

#ifdef X86_GCC2
    void MessageFromHandler(BMessage* msg);
#endif
private:
    BMessenger fHaikuLooper;
#ifdef X86_GCC2
    BHandler*  fHandler;
#endif
};

#endif

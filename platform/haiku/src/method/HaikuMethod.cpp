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

#include "HaikuMethod.h"

#include "HaikuMethodIcon.h"
#include "MethodMessageId.h"
#ifndef X86_GCC2
#include "HaikuLooper.h"
#else // X86_GCC2
#include <Application.h>
#include <Entry.h>
#include <FindDirectory.h>
#include <Handler.h>
#include <Input.h>
#include <Menu.h>
#include <Path.h>
#include <Roster.h>
#include "HaikuConstants.h"
#endif // X86_GCC2


extern "C" _EXPORT
BInputServerMethod *instantiate_input_method()
{
    return new HaikuMethod();
}


#ifdef X86_GCC2
class MethodHandler : public BHandler
{
public:
    MethodHandler(HaikuMethod* method)
        : BHandler()
        , fMethod(method) {}
    virtual ~MethodHandler() {}

    virtual void MessageReceived(BMessage* msg) {
        if (fMethod) {
            fMethod->MessageFromHandler(msg);
        }
    }
private:
    HaikuMethod* fMethod;
};


bool FindTask(directory_which d, BEntry* entry)
{
    BPath path;
    if (find_directory(d, &path) == B_OK) {
        path.Append(APP_DIR_NAME);
        path.Append(AQUASKK_TASK);
        entry->SetTo(path.Path());
        return entry->Exists();
    }
    return false;
}


bool FindTask(BEntry* entry)
{
    if (FindTask(B_USER_NONPACKAGED_DATA_DIRECTORY, entry)) {
        return true;
    }
    if (FindTask(B_SYSTEM_DATA_DIRECTORY, entry)) {
        return true;
    }
    return false;
}


status_t StartTask()
{
    status_t e = B_ERROR;
    BRoster roster;
    if (!roster.IsRunning(TASK_SIG)) {
        BEntry entry;
        if (FindTask(&entry)) {
            entry_ref ref;
            if (entry.GetRef(&ref) == B_OK) {
                e = roster.Launch(&ref);
            }
        }
    }
    return e;
}
#endif // X86_GCC2


HaikuMethod::HaikuMethod()
    : BInputServerMethod("AquaSKK", kAquaSkkMethod)
{
#ifdef X86_GCC2
    fHandler = new MethodHandler(this);
    if (be_app->Lock()) {
        be_app->AddHandler(fHandler);
        be_app->Unlock();
    }
#endif // X86_GCC2
}


HaikuMethod::~HaikuMethod()
{
    SetMenu(NULL, BMessenger());
#ifndef X86_GCC2
    BLooper* looper = NULL;
    fHaikuLooper.Target(&looper);
    fHaikuLooper.SetTo((const BHandler*)NULL, NULL);
    if (looper != NULL) {
        if (looper->Lock()) {
            looper->Quit();
        }
    }
#else // X86_GCC2
    fHaikuLooper.SendMessage(B_QUIT_REQUESTED);
    delete fHandler;
#endif // X86_GCC2
}


status_t HaikuMethod::InitCheck()
{
    status_t e = B_OK;
#ifndef X86_GCC2
    HaikuLooper* looper = new HaikuLooper(this);
    if (looper->Lock()) {
        e = looper->InitCheck();
        looper->Unlock();
    }
    fHaikuLooper = BMessenger(NULL, looper);
#else // X86_GCC2
    e = StartTask();
    if (e == B_OK) {
        fHaikuLooper = BMessenger(TASK_SIG);
        BMessage message('init');
        message.AddMessenger("method", BMessenger(fHandler, be_app));
        fHaikuLooper.SendMessage(&message);
    }
#endif
    return e;
}


filter_result HaikuMethod::Filter(BMessage *msg, BList *_list)
{
    if (msg->what == B_KEY_DOWN) {
        fHaikuLooper.SendMessage(msg);
        return B_SKIP_MESSAGE;
    }
    return B_DISPATCH_MESSAGE;
}


status_t HaikuMethod::MethodActivated(bool active)
{
    BMessage msg(active ? IM_METHOD_ACTIVATED : IM_METHOD_DEACTIVATED);
    fHaikuLooper.SendMessage(&msg);
    return BInputServerMethod::MethodActivated(active);
}

#ifdef X86_GCC2
void HaikuMethod::MessageFromHandler(BMessage* msg)
{
    switch (msg->what)
    {
        case B_KEY_DOWN:
        {
            BMessage* message = new BMessage(*msg);
            EnqueueMessage(message);
            break;
        }
        case B_INPUT_METHOD_EVENT:
        {
            BMessage* message = new BMessage(*msg);
            EnqueueMessage(message);
            break;
        }
        case B_GET_PROPERTY:
        {
            EnqueueMessage(msg);
            break;
        }
        case IM_UPDATE_MENU:
        {
            BMessage menuMsg;
            if (msg->FindMessage("menu", &menuMsg) != B_OK) {
                return;
            }
            BMessenger messenger;
            if (msg->FindMessenger("target", &messenger) != B_OK) {
                return;
            }
            BMenu* menu = (BMenu*)BMenu::Instantiate(&menuMsg);
            if (menu == NULL) {
                return;
            }
            SetMenu(menu, messenger);
            break;
        }
        default:
            break;
    }
}
#endif // X86_GCC2

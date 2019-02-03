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

#include "Task.h"

#include "HaikuLooper.h"
#include "HaikuConstants.h"
#include "MethodMessageId.h"


Task::Task()
    : BApplication(TASK_SIG)
{
    HaikuLooper* looper = new HaikuLooper();
    if (looper->Lock()) {
        looper->InitCheck();
        looper->Unlock();
    }
    fLooper = BMessenger(NULL, looper);
}


Task::~Task()
{
    BLooper* looper = NULL;
    fLooper.Target(&looper);
    fLooper.SetTo((const BHandler*)NULL, NULL);
    if (looper) {
        if (looper->Lock()) {
            looper->Quit();
        }
    }
}


bool Task::QuitRequested()
{
    BLooper* looper = NULL;
    fLooper.Target(&looper);
    fLooper.SetTo((const BHandler*)NULL, NULL);
    if (looper) {
        if (looper->Lock()) {
            looper->Quit();
        }
    }
}


void Task::MessageReceived(BMessage* msg)
{
    switch (msg->what) {
        case B_KEY_DOWN:
            fLooper.SendMessage(msg);
            break;
        case IM_METHOD_ACTIVATED:
            fLooper.SendMessage(msg);
            break;
        case IM_METHOD_DEACTIVATED:
            fLooper.SendMessage(msg);
            break;
        case 'init':
            fLooper.SendMessage(msg);
            break;
        default:
            BApplication::MessageReceived(msg);
            break;
    }
}


int main(int argc, char* argv[]) {
    Task* app = new Task();
    app->Run();
    delete app;
    return 0;
}

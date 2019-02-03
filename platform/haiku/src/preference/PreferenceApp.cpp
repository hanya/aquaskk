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

#include "PreferenceDialog.h"

#include <Application.h>
#include <Roster.h>
#include <Window.h>

#include "HaikuConstants.h"

#define WINDOW_ACTIVATE 'Wact'


class SKKPreferenceApp : public BApplication
{
private:
    BWindow* mpWindow;
public:
    SKKPreferenceApp();
    virtual ~SKKPreferenceApp() { mpWindow = NULL; };

    virtual void MessageReceived(BMessage* msg);
};


SKKPreferenceApp::SKKPreferenceApp()
    : BApplication(PREFERENCE_SIGNATURE)
    , mpWindow(NULL)
{
    mpWindow = new PreferenceDialog();
    mpWindow->CenterOnScreen();
    mpWindow->Show();
}


void SKKPreferenceApp::MessageReceived(BMessage* msg)
{
    switch (msg->what)
    {
        case WINDOW_ACTIVATE:
        {
            if (mpWindow != NULL) {
                mpWindow->Activate(true);

                BMessage reply(B_REPLY);
                reply.AddBool("status", true);
                msg->SendReply(&reply);
            }
            break;
        }
        default:
        {
            BApplication::MessageReceived(msg);
            break;
        }
    }
}



int main(int argc, char* argv[])
{
    bool status = false;
    BMessage message(WINDOW_ACTIVATE);
    BMessenger* messenger = new BMessenger();
    BList list(5);
    be_roster->GetAppList(PREFERENCE_SIGNATURE, &list);
    for (int i = 0; i < list.CountItems(); i++) {
        intptr_t id = (intptr_t)list.ItemAt(i);
        BMessage reply;
        if (messenger->SetTo(PREFERENCE_SIGNATURE, id) == B_OK) {
            if (messenger->SendMessage(&message, &reply) == B_OK) {
                status = reply.GetBool("status", false);
            }
        }
    }
    delete messenger;

    if (!status) {
        SKKPreferenceApp* app = new SKKPreferenceApp();
        app->Run();
        delete app;
    }
    return 0;
}

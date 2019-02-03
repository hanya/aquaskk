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

#ifndef HaikuLooper_h
#define HaikuLooper_h

#include <Looper.h>
#include <Messenger.h>

#include <memory>
#include <string>

class BMenu;
class BPath;

class HaikuMethod;
class HaikuMessenger;
class HaikuAnnotator;
class HaikuCandidateWindow;
class HaikuDynamicCompletor;
class HaikuModeIndicator;
class SKKInputSession;
class skkserv;


class HaikuLooper : public BLooper
{
public:
#ifndef X86_GCC2
             HaikuLooper(HaikuMethod *method);
#else // X86_GCC2
             HaikuLooper();
#endif // X86_GCC2
    virtual ~HaikuLooper();

    virtual void Quit();
    virtual void MessageReceived(BMessage *msg);
    virtual status_t EnqueueMessage(BMessage *msg);
            status_t InitCheck();

    void InsertString(const std::string& str);
    void ComposeString(const std::string& str, int cursorOffset);
    void ComposeString(const std::string& str,
                       int candidateStart, int candidateLength);
    void SendMessage(const std::string& str);
    std::string SelectedString();
    void AskForLocation();
    void AskForCursorLocation();
    void SelectInputMode(int mode);
#if DEBUG
    void _SendLog(const char* s);
#endif
private:
    void _ReloadUserDefaults();
    void _ReloadDictionarySet();
    void _ReloadComponents();
    void _ReloadBackEnd();

    void _SendMethodLocationRequest();
    void _SendMethodStarted();
    void _SendMethodStopped();

    void _HandleMethodActivated(bool active);
    void _HandleKeyDown(BMessage* msg);
    void _HandleLocationRequest(BMessage* msg);
    void _HandleInputMethodStopped();

    void _UpdateMenu();
    void _PrepareConfigDirectory();
    void _PrepareDictionary();
    void _ShowPreference();

    void _HideAllWindow();
    status_t _LoadJson(const char* path, BMessage* msg);
    bool _GetSelection(int32* start, int32* end);
    void _DeleteSelection();

    enum Category {
        USER_ONLY,
        SYSTEM_ONLY,
        USER_AND_SYSTEM,
    };
    std::string _FindFile(const char* name, Category category, bool exists=true);
    bool _FileExists(const BPath &base, const char* name);

#ifndef X86_GCC2
    HaikuMethod*     fOwner;
#else // X86_GCC2
    BMessenger       fMethod;
#endif // X86_GCC2
    SKKInputSession* fSession;
    bool             fMethodStarted;
    BMenu*           fDeskbarMenu;
    HaikuMessenger*         fMessenger;
    HaikuCandidateWindow*   fCandidateWindow;
    HaikuAnnotator*         fAnnotator;
    HaikuDynamicCompletor*  fDynamicCompletor;
    HaikuModeIndicator*     fModeIndicator;
    skkserv*                fSkkserv;
#if DEBUG
    std::unique_ptr<BMessenger> fLogger;
#endif
};

#endif

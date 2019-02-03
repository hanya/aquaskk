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

#ifndef PreferenceDialog_h
#define PreferenceDialog_h

#include <Window.h>

#include <string>

class BPath;

class CompletionView;
class ConversionView;
//class DisplayView;
class DictionaryView;
class ExtensionView;
class FileChooser;
class InputView;
class MiscView;
class VersionView;


class PreferenceDialog : public BWindow
{
public:
    PreferenceDialog();
    virtual ~PreferenceDialog();

    virtual void MessageReceived(BMessage* msg);

    void _Load();
    void _Save();

    enum Category {
        USER_ONLY,
        SYSTEM_ONLY,
        USER_AND_SYSTEM,
    };
    std::string _FindFile(const char* name, Category category, bool exists=true);
    status_t _LoadJson(const char* path, BMessage* msg);
private:
    void _LoadUserDefaults();
    void _LoadRules();
    void _LoadRule(Category category, int32 division);
    void _LoadDictionarySet();
    bool _FileExists(const BPath &base, const char* name);
    void _SaveUserDefaults();
    void _SaveActiveRules();
    void _SaveDictionarySet();
    void _DownloadDictionary();

    InputView*      mpInputView;
    ExtensionView*  mpExtensionView;
    CompletionView* mpCompletionView;
    ConversionView* mpConversionView;
    //DisplayView*    mpDisplayView;
    DictionaryView* mpDictionaryView;
    MiscView*       mpMiscView;
    VersionView*    mpVersionView;
    FileChooser*    mpUserLocationChooser;
    FileChooser*    mpLocationChooser;
    BButton*        mpOkButton;
    BButton*        mpCancelButton;
    std::string     fDictHost;
    std::string     fDictPath;
};

#endif

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

#include "HaikuLooper.h"

#include "SKKPreProcessor.h"
#include "SKKAutoUpdateDictionary.h"
#include "SKKCommonDictionary.h"
#include "SKKGadgetDictionary.h"
#include "SKKProxyDictionary.h"
#include "SKKRomanKanaConverter.h"
#include "SKKDictionaryFactory.h"
#include "SKKInputSession.h"
#include "HaikuMethod.h"
#include "HaikuInputSessionParameter.h"
#include "HaikuCandidateWindow.h"
#include "HaikuAnnotator.h"
#include "HaikuDynamicCompletor.h"
#include "HaikuMessenger.h"
#include "HaikuModeIndicator.h"
#include "MethodMessageId.h"
#include "SKKConstVars.h"
#include "SKKBackEnd.h"
#include "DictionaryTypes.h"
#include "HaikuConstants.h"
#include "skkserv.h"

#include <Application.h>
#include <Catalog.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Input.h>
#include <private/shared/Json.h>
#include <private/shared/JsonMessageWriter.h>
#include <LayoutBuilder.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Path.h>
#include <Roster.h>
#include <String.h>

#include <kernel/image.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "InputMethod"


#if DEBUG
#include <stdio.h>
#define LOGGER "application/x-vnd.Logger"
#define LOG_COMMAND 'Logg'
#define LOG_NAME "log"
#endif

#ifndef X86_GCC2
HaikuLooper::HaikuLooper(HaikuMethod* method)
    : BLooper("AquaSKKLooper")
    , fOwner(method)
#else
HaikuLooper::HaikuLooper()
    : BLooper("AquaSKKLooper")
#endif
    , fMethodStarted(false)
    , fMessenger(NULL)
    , fCandidateWindow(NULL)
    , fAnnotator(NULL)
    , fDynamicCompletor(NULL)
    , fModeIndicator(NULL)
    , fSkkserv(NULL)
{
#ifndef X86_GCC2
    if (be_app) {
        if (be_app->Lock()) {
            be_app->AddHandler(this);
            be_app->Unlock();
        }
    }
#endif
#if DEBUG
    fLogger = std::unique_ptr<BMessenger>(new BMessenger(LOGGER));
    _SendLog("HaikuLooper.ctor");
#endif
    fDeskbarMenu = new BMenu("AquaSKKMenu");
    BLayoutBuilder::Menu<>(fDeskbarMenu)
        .AddItem(B_TRANSLATE("Setting..."),
            new BMessage(IM_SETTINGS))
        //.AddItem(B_TRANSLATE("Direct input mode"),
        //    new BMessage(IM_DIRECT_MODE))
        .AddItem(B_TRANSLATE("Private mode"),
            new BMessage(IM_PRIVATE_MODE))
        .AddItem(B_TRANSLATE("Reload setting"),
            new BMessage(IM_RELOAD_SETTINGS));

    Run();
}


HaikuLooper::~HaikuLooper()
{
#if DEBUG
    _SendLog("HaikuLooper.dector");
#endif
#ifndef X86_GCC2
    if (be_app) {
        if (be_app->Lock()) {
            be_app->RemoveHandler(this);
            be_app->Unlock();
        }
    }
#endif // X86_GCC2
    delete fSession;
    delete fMessenger;
    delete fCandidateWindow;
    delete fAnnotator;
    delete fDynamicCompletor;
    // fModeIndicator is deleted while fSession destruction
    delete fSkkserv;
}


void HaikuLooper::Quit()
{
#ifndef X86_GCC2
    fOwner->SetMenu(NULL, BMessenger());
#endif // X86_GCC2
    delete fDeskbarMenu;
    BLooper::Quit();
}


status_t HaikuLooper::InitCheck()
{
#if DEBUG
    _SendLog("HaikuLooper.InitCheck");
#endif
    if (fMessenger) {
        delete fMessenger;
    }
    if (fCandidateWindow) {
        delete fCandidateWindow;
    }
    if (fAnnotator) {
        delete fAnnotator;
    }
    if (fDynamicCompletor) {
        delete fDynamicCompletor;
    }

    _PrepareConfigDirectory();
    _ReloadUserDefaults();
    _PrepareDictionary();
    _ReloadDictionarySet();
    _ReloadBackEnd();
    _ReloadComponents();

    fMessenger = new HaikuMessenger(this);
    fCandidateWindow = new HaikuCandidateWindow(this);
    fAnnotator = new HaikuAnnotator(this);
    fDynamicCompletor = new HaikuDynamicCompletor(this, B_TRANSLATE("Choose by Tab"));
#if DEBUG
    _SendLog("HaikuLooper.InitCheck init session");
#endif
    fModeIndicator = new HaikuModeIndicator(this);
    fSession = new SKKInputSession(new HaikuInputSessionParameter(
            this, fMessenger, fCandidateWindow, fAnnotator, fDynamicCompletor));
    fSession->AddInputModeListener(fModeIndicator);

    _UpdateMenu();
#if DEBUG
    _SendLog("HaikuLooper.InitCheck done");
#endif
    return B_OK;
}


void HaikuLooper::MessageReceived(BMessage *msg)
{
    switch (msg->what)
    {
        case B_KEY_DOWN:
        {
            _HandleKeyDown(msg);
            break;
        }
        case B_INPUT_METHOD_EVENT:
        {
            const uint32 opcode = msg->GetInt32("be:opcode", 0);
            switch (opcode)
            {
                case B_INPUT_METHOD_LOCATION_REQUEST:
                    if (fMethodStarted) {
                        _HandleLocationRequest(msg);
                    }
                    break;
                case B_INPUT_METHOD_STOPPED:
                    _HandleInputMethodStopped();
                    break;
                default:
                    break;
            }
            break;
        }
        case IM_METHOD_ACTIVATED:
        {
            _HandleMethodActivated(true);
            _UpdateMenu(); // todo
            break;
        }
        case IM_METHOD_DEACTIVATED:
        {
            _HandleMethodActivated(false);
            break;
        }
        case IM_SETTINGS:
        {
            _ShowPreference();
#if DEBUG
    _SendLog("HaikuLooper.IM_SETTINGS");
#endif
            _UpdateMenu(); // todo
            break;
        }
        /*
        case IM_DIRECT_MODE:
        {
            SKKUserDefaultKeys::direct_clients =
                    !SKKUserDefaultKeys::direct_clients;
            _UpdateMenu(); // todo
            break;
        }
        */
        case IM_PRIVATE_MODE:
        {
            SKKUserDefaultKeys::enable_private_mode =
                    !SKKUserDefaultKeys::enable_private_mode;
            SKKBackEnd::theInstance().EnablePrivateMode(
                        SKKUserDefaultKeys::enable_private_mode);
            _UpdateMenu(); // todo
            break;
        }
        case IM_RELOAD_SETTINGS:
        {
            InitCheck();
            fCandidateWindow->Reload();
            break;
        }
#ifdef X86_GCC2
        case 'init':
        {
            msg->FindMessenger("method", &fMethod);
            break;
        }
#endif // X86_GCC2
        default:
        {
            BLooper::MessageReceived(msg);
            break;
        }
    }
}


status_t HaikuLooper::EnqueueMessage(BMessage *msg)
{
#ifndef X86_GCC2
    if (fOwner != NULL) {
        return fOwner->EnqueueMessage(msg);
    }
    return -1;
#else // X86_GCC2
    if (fMethod.IsValid()) {
        fMethod.SendMessage(msg);
        return B_OK;
    }
#endif // X86_GCC2
    return -1;
}


void HaikuLooper::SelectInputMode(int mode)
{
#if DEBUG
    char s[64];
    sprintf(s, "mode: %d", (int)mode);
    _SendLog((const char*)s);
#endif
}


void HaikuLooper::InsertString(const std::string& str)
{
    // insert string into the target
    if (str.empty()) {
        return;
    }

    // start input method if not started yet to helps
    // to insert string properly.
    if (!fMethodStarted) {
        _SendMethodStarted();
    }
    BMessage* msg = new BMessage(B_INPUT_METHOD_EVENT);
    msg->AddInt32("be:opcode", B_INPUT_METHOD_CHANGED);
    msg->AddBool("be:confirmed", true);
    msg->AddString("be:string", str.c_str());
    EnqueueMessage(msg);
#if DEBUG
    char s[64];
    sprintf(s, "insert str: %s", str.c_str());
    _SendLog((const char*)s);
#endif
    // stop method
    _HandleInputMethodStopped();
}


void HaikuLooper::ComposeString(const std::string& str, int cursorOffset)
{
    // show string into the preedit
    if (str.empty()) {
        return;
    }
    // todo, how can we use offset?
    if (!fMethodStarted) {
        _SendMethodStarted();
        // todo, flag for preedit active?
    }
    BMessage* msg = new BMessage(B_INPUT_METHOD_EVENT);
    msg->AddInt32("be:opcode", B_INPUT_METHOD_CHANGED);
    msg->AddInt32("be:clause_start", 0);
    msg->AddInt32("be:clause_end", str.length() - 1);
    //msg->AddInt32("be:selection", cursorOffset);
    //msg->AddInt32("be:selection", str.length() - 1);
    msg->AddString("be:string", str.c_str());
    EnqueueMessage(msg);
#if DEBUG
    char s[64];
    sprintf(s, "compose1 str: \"%s\", offset: %d",
            str.c_str(), cursorOffset);
    _SendLog((const char*)s);
#endif
    // todo, have to send highlight index? always second char?
}


void HaikuLooper::ComposeString(const std::string& str,
                   int candidateStart, int candidateLength)
{
    // another way to show string into the preedit
    if (str.empty()) {
        return;
    }
    // todo, what should we do for start and length
    if (!fMethodStarted) {
        _SendMethodStarted();
        // todo, flag for preedit active?
    }
    BMessage* msg = new BMessage(B_INPUT_METHOD_EVENT);
    msg->AddInt32("be:opcode", B_INPUT_METHOD_CHANGED);
    msg->AddInt32("be:clause_start", 0);
    msg->AddInt32("be:clause_end", str.length() - 1);
    // todo, convert character index into byte index
    //msg->AddInt32("be:selection", candidateStart);
    //msg->AddInt32("be:selection", candidateLength);
    msg->AddString("be:string", str.c_str());
    EnqueueMessage(msg);
#if DEBUG
    char s[64];
    sprintf(s, "compose2 str: \"%s\", start: %d, length: %d",
            str.c_str(), candidateStart, candidateLength);
    _SendLog((const char*)s);
#endif
}


void HaikuLooper::SendMessage(const std::string& str)
{
    // todo, show information message
#if DEBUG
    _SendLog("HaikuSKK.SendMessage");
    _SendLog(str.c_str());
#endif
}


std::string HaikuLooper::SelectedString()
{
    /*
    // not workin, crash
    int32 start, end;
    if (_GetSelection(&start, &end)) {
        BMessage message(B_GET_PROPERTY);
        message.AddString("property", "Text");
        message.AddInt32("index", start);
        message.AddInt32("range", end - start);

        if (EnqueueMessage(&message) == B_OK) {
            const char* text = NULL;
            if (message.FindString("str", &text) == B_OK) {
                if (text != NULL) {
                    std::string s(text);
                    return s;
                }
            }
        }
    }
    */
    return "";
}


void HaikuLooper::_DeleteSelection()
{
    int32 start, end;
    if (_GetSelection(&start, &end)) {
        BMessage msg(B_SET_PROPERTY);
        msg.AddString("property", "Text");
        msg.AddInt32("index", start);
        msg.AddInt32("range", end - start);
        EnqueueMessage(&msg);
    }
}


bool HaikuLooper::_GetSelection(int32* start, int32* end)
{
    BMessage msg(B_GET_PROPERTY);
    msg.AddString("property", "selection");

    if (EnqueueMessage(&msg) == B_OK) {
        int32 r1, r2;
        if (msg.FindInt32("result", 0, &r1) == B_OK &&
            msg.FindInt32("result", 1, &r2) == B_OK &&
            r1 != r2 && 0 <= r1 && 0 <= r2) {
            if (r1 > r2) {
                int32 tmp = r1;
                r1 = r2;
                r2 = tmp;
            }
            *start = r1;
            *end = r2;
        }
    }
    return false;
}


void HaikuLooper::AskForLocation()
{
#if DEBUG
    _SendLog("HaikuLooper.AskForLocation");
#endif
    _SendMethodLocationRequest();
}


void HaikuLooper::AskForCursorLocation()
{
    // todo, for mode indicator and messenger
}


void HaikuLooper::_ReloadUserDefaults()
{
    std::string path = _FindFile(USER_DEFAULTS, USER_AND_SYSTEM, true);
    BMessage msg;
    if (_LoadJson(path.c_str(), &msg) != B_OK) {
        path = _FindFile(USER_DEFAULTS, SYSTEM_ONLY, true);
        if (_LoadJson(path.c_str(), &msg) != B_OK) {
            return;
        }
    }
#if DEBUG
    _SendLog("HaikuLooper.InitCheck load from UserDefaults.json");
#endif
    SKKUserDefaultKeys::suppress_newline_on_commit =
            msg.GetBool("suppress_newline_on_commit", true);
    SKKUserDefaultKeys::use_numeric_conversion =
            msg.GetBool("use_numeric_conversion", true);
    SKKUserDefaultKeys::show_input_mode_icon =
            msg.GetBool("show_input_mode_icon", true);
    SKKUserDefaultKeys::show_input_mode_icon = false; // todo
    SKKUserDefaultKeys::beep_on_registration =
            msg.GetBool("beep_on_registration", false);
    SKKUserDefaultKeys::inline_backspace_implies_commit =
            msg.GetBool("inline_backspace_implies_commit", true);
    SKKUserDefaultKeys::delete_okuri_when_quit =
            msg.GetBool("delete_okuri_when_quit", true);
    SKKUserDefaultKeys::enable_extended_completion =
            msg.GetBool("enable_extended_completion", true);
    SKKUserDefaultKeys::enable_dynamic_completion =
            msg.GetBool("enable_dynamic_completion", true);
    SKKUserDefaultKeys::dynamic_completion_range =
            int32(msg.GetDouble("dynamic_completion_range", 1.));
    SKKUserDefaultKeys::minimum_completion_length =
            int32(msg.GetDouble("minimum_completion_length", 0.));
    SKKUserDefaultKeys::max_count_of_inline_candidates =
            int32(msg.GetDouble("max_count_of_inline_candidates", 4.));
    SKKUserDefaultKeys::candidate_window_labels =
            std::string(msg.GetString("candidate_window_labels", "ASDFGHJKL"));
    SKKUserDefaultKeys::candidate_window_font_name =
            msg.GetBool("candidate_window_font_name", "Noto Sans CJK JP");
    SKKUserDefaultKeys::candidate_window_font_size =
            int32(msg.GetDouble("candidate_window_font_size", 12.));
    SKKUserDefaultKeys::put_candidate_window_upward =
            msg.GetBool("put_candidate_window_upward", false);
    SKKUserDefaultKeys::enable_annotation =
            msg.GetBool("enable_annotation", false);
    SKKUserDefaultKeys::user_dictionary_path =
            msg.GetString("user_dictionary_path", "/boot/home/config/settings/aquaskk/skk-jisyo.utf8");
    SKKUserDefaultKeys::skkserv_enable =
            msg.GetBool("skkserv_enable", false);
    SKKUserDefaultKeys::skkserv_localonly =
            msg.GetBool("skkserv_localonly", true);
    SKKUserDefaultKeys::skkserv_port =
            int32(msg.GetDouble("skkserv_port", 1178.));
    SKKUserDefaultKeys::enable_private_mode =
            msg.GetBool("enable_private_mode", false);
    SKKUserDefaultKeys::fix_intermediate_conversion =
            msg.GetBool("fix_intermediate_conversion", true);
    SKKUserDefaultKeys::display_shortest_match_of_kana_conversions =
            msg.GetBool("display_shortest_match_of_kana_conversions", true);
    SKKUserDefaultKeys::handle_recursive_entry_as_okuri =
            msg.GetBool("handle_recursive_entry_as_okuri", true);
    SKKUserDefaultKeys::dictionary_host =
            msg.GetString("dictionary_host", "github.com");
    SKKUserDefaultKeys::dictionary_path =
            msg.GetString("dictionary_path", "/skk-dev/dict/raw/gh-pages");
    //SKKUserDefaultKeys::direct_clients =
    //        msg.GetBool("direct_clients", false);
    SKKUserDefaultKeys::use_custom_font =
            msg.GetBool("use_custom_font", false);
    SKKUserDefaultKeys::candidate_window_vertical =
            msg.GetBool("candidate_window_vertical", false);
}


void HaikuLooper::_ReloadBackEnd()
{
    SKKBackEnd::theInstance().UseNumericConversion(
            SKKUserDefaultKeys::use_numeric_conversion);

    SKKBackEnd::theInstance().EnableExtendedCompletion(
            SKKUserDefaultKeys::enable_extended_completion);

    SKKBackEnd::theInstance().EnablePrivateMode(
            SKKUserDefaultKeys::enable_private_mode);

    SKKBackEnd::theInstance().SetMinimumCompletionLength(
            SKKUserDefaultKeys::minimum_completion_length);
}


void HaikuLooper::_ReloadDictionarySet()
{
    std::string path = _FindFile(DICTIONARY_SET, USER_AND_SYSTEM, true);
    BMessage msg;
    _LoadJson(path.c_str(), &msg);
    SKKDictionaryKeyContainer keys;
    std::string dictPath = _FindFile(DICT_DIR, USER_ONLY, false);

    BString name;
    const int32 count = msg.CountNames(B_MESSAGE_TYPE);
    for (int32 i = 0; i < count; i++) {
        BMessage d;
        if (msg.FindMessage(name.SetToFormat("%" B_PRIu32, i).String(), &d) == B_OK) {
            const bool active = d.GetBool("active", false);
            if (!active) {
                continue;
            }
            const char* loc = NULL;
            double dtype = 0;
            if (d.FindString("location", &loc) == B_OK &&
                d.FindDouble("type", &dtype) == B_OK) {
                const int type = int32(dtype);
                if (type == DictionaryTypes::Gadget ||
                    type == DictionaryTypes::Proxy) {
                    keys.push_back(SKKDictionaryKey(type, std::string(loc)));
                } else {
                    BString s(loc);
                    BPath path;
                    if (s.StartsWith("/")) {
                        path.SetTo(loc);
                    } else if (s.StartsWith("~/")) {
                        BPath homePath;
                        find_directory(B_USER_DIRECTORY, &homePath);
                        path.SetTo(homePath.Path());
                        path.Append(s.String()+2);
                    } else if (s.Length() > 0) {
                        path.SetTo(dictPath.c_str());
                        path.Append(loc);
                    }
                    std::string location(path.Path());
                    keys.push_back(SKKDictionaryKey(type, location));
                }
            }
        }
    }
    SKKBackEnd::theInstance().Initialize(
            SKKUserDefaultKeys::user_dictionary_path, keys);
}


void HaikuLooper::_ReloadComponents()
{
    std::string keymapConfPath = _FindFile(KEY_MAP, USER_AND_SYSTEM, true);
    SKKPreProcessor::theInstance().Initialize(keymapConfPath.c_str());

    std::string kanaRuleConfPath = _FindFile(KANA_RULE, USER_AND_SYSTEM, true);
    SKKRomanKanaConverter::theInstance().Initialize(kanaRuleConfPath.c_str());

    std::vector<std::string> activeRules;
    std::string activeRulesPath = _FindFile(ACTIVE_RULES, USER_ONLY, true);
    if (!activeRulesPath.empty()) {
        BMessage msg;
        if (_LoadJson(activeRulesPath.c_str(), &msg) == B_OK) {
            BString s;
            const int32 strCount = msg.CountNames(B_STRING_TYPE);
            for (int j = 0; j < strCount; j++) {
                const char* id = NULL;
                s.SetToFormat("%" B_PRIu32, j);
                if (msg.FindString(s.String(), &id) == B_OK) {
                    activeRules.push_back(std::string(id));
                }
            }
        }
    }

    const int32 activeCount = activeRules.size();
    std::string userRulesPath = _FindFile(SUB_RULE, USER_ONLY, false);
    std::string systemRulesPath = _FindFile(SUB_RULE, SYSTEM_ONLY, false);
    std::string userPath = _FindFile("", USER_ONLY, false);
    std::string systemPath = _FindFile("", SYSTEM_ONLY, false);
    const struct {
        const char* rulesPath;
        const char* dataPath;
    } rules[] = {
        { userRulesPath.c_str(), userPath.c_str() },
        { systemRulesPath.c_str(), systemPath.c_str() },
        { NULL, NULL },
    };
    for (int n = 0; rules[n].rulesPath != NULL; n++) {
        BMessage msg;
        if (_LoadJson(rules[n].rulesPath, &msg) == B_OK) {
            BPath basePath(rules[n].dataPath);
            BString name;
            const int32 count = msg.CountNames(B_MESSAGE_TYPE);
            for (int i = 0; i < count; i++) {
                BMessage d;
                if (msg.FindMessage(name.SetToFormat("%" B_PRIu32, i).String(), &d) == B_OK) {
                    const char* id = NULL;
                    const char* rule = NULL;
                    const char* keymap = NULL;
                    if (d.FindString("id", &id) != B_OK) {
                        continue;
                    }
                    // check the rule active
                    std::string sid(id);
                    bool active = false;
                    for (int c = 0; c < activeCount; c++) {
                        if (activeRules[c] == sid) {
                            active = true;
                            break;
                        }
                    }
                    if (!active) {
                        continue;
                    }
                    d.FindString("rule", &rule);
                    d.FindString("keymap", &keymap);
                    if (rule == NULL && keymap == NULL) {
                        continue;
                    }
                    if (rule != NULL && _FileExists(basePath, rule)) {
                        BPath p(basePath);
                        p.Append(rule);
                        SKKRomanKanaConverter::theInstance().Patch(p.Path());
                    }
                    if (keymap != NULL && _FileExists(basePath, keymap)) {
                        BPath p(basePath);
                        p.Append(keymap);
                        SKKPreProcessor::theInstance().Patch(p.Path());
                    }
                }
            }
        }
    }
#if DEBUG
    _SendLog("HaikuLooper.InitCheck load components done");
#endif
}


bool HaikuLooper::_FileExists(const BPath &base, const char* name)
{
    BPath path(base);
    path.Append(name);
    BFile f(path.Path(), B_READ_ONLY);
    return f.IsReadable();
}


std::string HaikuLooper::_FindFile(const char* name, Category category, bool exists)
{
    const directory_which DIRS[] = {
        B_USER_SETTINGS_DIRECTORY,
        B_USER_NONPACKAGED_DATA_DIRECTORY,
        B_SYSTEM_DATA_DIRECTORY,
    };

    BPath path;
    int i = category == SYSTEM_ONLY ? 1 : 0;
    int lastIndex = category == USER_ONLY ? 0 : 2;
    for (; i <= lastIndex; i++) {
        if (find_directory(DIRS[i], &path) == B_OK) {
            path.Append(APP_DIR_NAME);
            path.Append(name);
            if (exists) {
                BFile f(path.Path(), B_READ_ONLY);
                if (f.IsReadable()) {
                    return std::string(path.Path());
                }
            } else {
                return std::string(path.Path());
            }
        }
    }
    return "";
}


status_t HaikuLooper::_LoadJson(const char* path, BMessage* msg)
{
    BJsonMessageWriter* writer = new BJsonMessageWriter(*msg);
    BFile f(path, B_READ_ONLY);
    BPrivate::BJson::Parse(&f, writer);
    status_t result = writer->ErrorStatus();
    delete writer;
    return result;
}


void HaikuLooper::_SendMethodLocationRequest()
{
    BMessage* msg = new BMessage(B_INPUT_METHOD_EVENT);
    msg->AddInt32("be:opcode", B_INPUT_METHOD_LOCATION_REQUEST);
    EnqueueMessage(msg);
}


void HaikuLooper::_SendMethodStarted()
{
#if DEBUG
    _SendLog("HaikuLooper._SendMethodStarted");
#endif
    BMessage* msg = new BMessage(B_INPUT_METHOD_EVENT);
    msg->AddInt32("be:opcode", B_INPUT_METHOD_STARTED);
    msg->AddMessenger("be:reply_to", BMessenger(NULL, this));
    EnqueueMessage(msg);

    fMethodStarted = true;
}


void HaikuLooper::_SendMethodStopped()
{
#if DEBUG
    _SendLog("HaikuLooper._SendMethodStarted");
#endif
    BMessage* msg = new BMessage(B_INPUT_METHOD_EVENT);
    msg->AddInt32("be:opcode", B_INPUT_METHOD_STOPPED);
    EnqueueMessage(msg);

    fMethodStarted = false;
}


void HaikuLooper::_HandleMethodActivated(bool active)
{
    if (active) {
#if DEBUG
        _SendLog("HaikuSKK.activated");
#endif
        fSession->Activate();
        // Do not make method active here.
        //_SendMethodStarted();

        if (SKKUserDefaultKeys::show_input_mode_icon) {
            // todo,
        }
    } else {
#if DEBUG
        _SendLog("HaikuSKK.deactivated");
#endif
        fSession->Deactivate();

        _SendMethodStopped();
        _HideAllWindow();
        // todo, hide indicator if shown
    }
}


void HaikuLooper::_HandleKeyDown(BMessage *msg)
{
#if DEBUG
    {
        int32 key = msg->GetInt32("key", 0); // raw keycode
        uint8 byte = (uint8)msg->GetInt8("byte", 0); // ascii code
        uint32 modifiers = (uint32)msg->GetInt32("modifiers", 0);

        char a[2];
        a[0] = (char)byte;
        a[1] = '\0';
        char s[64];
        sprintf(s, "keydown, key: 0x%x, mod: 0x%x, byte: 0x%x(%s)",
            key, modifiers, (char)byte, (const char*)&a);
        _SendLog((const char*)s);
    }
#endif
    SKKEvent event = SKKPreProcessor::theInstance().Execute(msg);
#if DEBUG
    {
        char s[64];
        sprintf(s, "skkevent code: 0x%x, attr: 0x%x, op: 0x%x",
            event.code, event.attribute, event.option);
        _SendLog((const char*)s);
    }
#endif
    bool state = fSession->HandleEvent(event);
#if DEBUG
    {
        char s[64];
        sprintf(s, "handled: %d", (int)state);
        _SendLog((const char*)s);
    }
#endif
    if (!state) {
        EnqueueMessage(DetachCurrentMessage());
    }
}


void HaikuLooper::_HandleLocationRequest(BMessage* msg)
{
    fCandidateWindow->ShowAt(msg);
    //fAnnotator->ShowAt(msg);
    fDynamicCompletor->ShowAt(msg);

    // todo, send location to mode indicator

#if DEBUG
    _SendLog("HaikuSKK._HandleLocationRequest");
#endif
}


void HaikuLooper::_HandleInputMethodStopped()
{
    // method stopped by somewhere such as tray icon
    _SendMethodStopped();
    _HideAllWindow();

    fSession->Clear();
#if DEBUG
    _SendLog("HaikuSKK._HandleInputMethodStopped");
#endif
}


void HaikuLooper::_HideAllWindow()
{
    //fMessenger->Hide(); // todo
    fCandidateWindow->Hide();
    //fAnnotator->Hide();
    fDynamicCompletor->Hide();
}


void HaikuLooper::_UpdateMenu()
{
    BMenuItem *item = NULL;
    for (int i = 0; i < fDeskbarMenu->CountItems(); ++i) {
        item = fDeskbarMenu->ItemAt(i);
        if (item != NULL) {
            BMessage *m = item->Message();
            if (m) {
                if (m->what == IM_PRIVATE_MODE) {
                    item->SetMarked(SKKUserDefaultKeys::enable_private_mode);
                }
                // else if (m->what == IM_DIRECT_MODE) {
                //    item->SetMarked(SKKUserDefaultKeys::direct_clients);
                //}
            }
        }
    }
#ifndef X86_GCC2
    fOwner->SetMenu(fDeskbarMenu, BMessenger(NULL, this));
#else
    BMessage msg(IM_UPDATE_MENU);
    BMessage menuMsg;
    fDeskbarMenu->Archive(&menuMsg);
    msg.AddMessage("menu", &menuMsg);
    msg.AddMessenger("target", BMessenger(NULL, this));
    EnqueueMessage(&msg);
#endif
}


void HaikuLooper::_PrepareDictionary()
{
    delete fSkkserv;
    if (SKKUserDefaultKeys::skkserv_enable) {
        fSkkserv = new skkserv(SKKUserDefaultKeys::skkserv_port,
                               SKKUserDefaultKeys::skkserv_localonly);
    }
    // maps between dictionary class and dictionary type
    SKKRegisterFactoryMethod<SKKCommonDictionary>(DictionaryTypes::Common);
    SKKRegisterFactoryMethod<SKKCommonDictionaryUTF8>(DictionaryTypes::CommonUTF8);
    SKKRegisterFactoryMethod<SKKCommonDictionary>(DictionaryTypes::Download);
    SKKRegisterFactoryMethod<SKKCommonDictionaryUTF8>(DictionaryTypes::DownloadUTF8);
    SKKRegisterFactoryMethod<SKKProxyDictionary>(DictionaryTypes::Proxy);
    SKKRegisterFactoryMethod<SKKGadgetDictionary>(DictionaryTypes::Gadget);
}


void HaikuLooper::_PrepareConfigDirectory()
{
    BPath path;
    if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
        path.Append(APP_DIR_NAME);
        BDirectory dir;
        dir.CreateDirectory(path.Path(), NULL);
    }
}


void HaikuLooper::_ShowPreference()
{
    std::string path = _FindFile(PREFERENCE_BIN, SYSTEM_ONLY, true);

    BEntry entry;
    entry.SetTo(path.c_str());
    entry_ref ref;
    if (entry.GetRef(&ref) == B_OK) {
        be_roster->Launch(&ref);
    }
/*
    const char* argv[] = {
        path.c_str(),
        NULL,
    };

    thread_id tid = load_image(1, argv, (const char**)environ);
    if (tid != B_ERROR) {
        resume_thread(tid);
    }
*/
}


#if DEBUG
void HaikuLooper::_SendLog(const char *s)
{
    if (!fLogger->IsValid()) {
        if (fLogger->SetTo(LOGGER) != B_OK) {
            return;
        }
    }
    BMessage msg(LOG_COMMAND);
    msg.AddString(LOG_NAME, s);
    fLogger->SendMessage(&msg);
}
#endif

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

#include <Button.h>
#include <Catalog.h>
#include <CheckBox.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <LayoutBuilder.h>
#include <Path.h>
#include <Roster.h>
#include <SeparatorView.h>
#include <Slider.h>
#include <Spinner.h>
#include <StringView.h>
#include <TabView.h>

#include <private/shared/Json.h>
#include <private/shared/JsonMessageWriter.h>
#include <private/shared/JsonTextWriter.h>

#include "CompletionView.h"
#include "ConversionView.h"
#include "DictionaryTypes.h"
#include "DictionaryView.h"
#include "DisplayView.h"
#include "ExtensionView.h"
#include "FileChooser.h"
#include "FileFilter.h"
#include "InputView.h"
#include "MessageId.h"
#include "MethodMessageId.h"
#include "MiscView.h"
#include "VersionView.h"
#include "HaikuConstants.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Preference"

#define OK 'btok'
#define CANCEL 'btcl'


const struct {
    int32 type;
    const char* label;
} DictionaryTypeLabels[] = {
    { DictionaryTypes::Common, B_TRANSLATE_MARK("SKK dictionary (EUC-JP)") },
    { DictionaryTypes::CommonUTF8, B_TRANSLATE_MARK("SKK dictionary (UTF-8)") },
    { DictionaryTypes::Download, B_TRANSLATE_MARK("SKK download dictionary (EUC-JP)") },
    { DictionaryTypes::DownloadUTF8, B_TRANSLATE_MARK("SKK download dictionary (UTF-8)") },
    { DictionaryTypes::Proxy, B_TRANSLATE_MARK("skkserv dictionary") },
    { DictionaryTypes::Gadget, B_TRANSLATE_MARK("Program dictionary") },
    { 0, NULL },
};


PreferenceDialog::PreferenceDialog()
    : BWindow(
        BRect(50, 50, 500, 500),
        B_TRANSLATE("AquaSKK Settings"),
        B_TITLED_WINDOW_LOOK,
        B_NORMAL_WINDOW_FEEL,
        B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS |
        B_CLOSE_ON_ESCAPE | B_QUIT_ON_WINDOW_CLOSE)
    , mpUserLocationChooser(NULL)
    , mpLocationChooser(NULL)
{
    mpInputView = new InputView(B_TRANSLATE("Input"));
    mpExtensionView = new ExtensionView(B_TRANSLATE("Extension"));
    mpCompletionView = new CompletionView(B_TRANSLATE("Completion"));
    mpConversionView = new ConversionView(B_TRANSLATE("Conversion"));
    mpDictionaryView = new DictionaryView(B_TRANSLATE("Dictionary"), this);
    //mpDisplayView = new DisplayView(B_TRANSLATE("Display"));
    mpMiscView = new MiscView(B_TRANSLATE("Misc"));
    mpVersionView = new VersionView(B_TRANSLATE("Version"));

    BTabView* tabView = new BTabView("tabview", B_WIDTH_FROM_LABEL);
    tabView->AddTab(mpInputView);
    tabView->AddTab(mpExtensionView);
    tabView->AddTab(mpCompletionView);
    tabView->AddTab(mpConversionView);
    //tabView->AddTab(mpDisplayView);
    tabView->AddTab(mpDictionaryView);
    tabView->AddTab(mpMiscView);
    tabView->AddTab(mpVersionView);
    tabView->SetBorder(B_NO_BORDER);

    mpOkButton = new BButton("ok", "OK", new BMessage(OK));
    mpCancelButton = new BButton("cl", B_TRANSLATE("Cancel"), new BMessage(CANCEL));

    BLayoutBuilder::Group<>(this, B_VERTICAL)
        .SetInsets(0, 5, 0, 5)
        .Add(tabView)
        .Add(new BSeparatorView(B_HORIZONTAL))
        .AddGroup(B_HORIZONTAL)
            .SetInsets(5, 0)
            .AddGlue()
            .Add(mpOkButton)
            .Add(mpCancelButton)
        .End();

    StateColumn<ExtensionRow>* ruleStateColumn =
        new StateColumn<ExtensionRow>(
            mpExtensionView->mpGridView, 0);
    DescriptionColumn<ExtensionRow>* ruleDescColumn =
        new DescriptionColumn<ExtensionRow>(
            mpExtensionView->mpGridView, 1);
    LocationColumn<ExtensionRow>* ruleLocationColumn =
        new LocationColumn<ExtensionRow>(
            mpExtensionView->mpGridView, 2);
    mpExtensionView->mpGridView->AddColumn(ruleStateColumn);
    mpExtensionView->mpGridView->AddColumn(ruleDescColumn);
    mpExtensionView->mpGridView->AddColumn(ruleLocationColumn);
    mpExtensionView->mpGridView->SetTarget(this);

    StateColumn<DictRow>* dictStateColumn =
        new StateColumn<DictRow>(
            mpDictionaryView->mpDictGridView, 0);
    DictKindColumn<DictRow>* dictKindColumn =
        new DictKindColumn<DictRow>(
            mpDictionaryView->mpDictGridView, 1, this);
    DictLocationColumn<DictRow>* dictLocationColumn =
        new DictLocationColumn<DictRow>(
            mpDictionaryView->mpDictGridView, 2, this,
            mpDictionaryView->mpDictGridView->InputView());
    mpDictionaryView->mpDictGridView->AddColumn(dictStateColumn);
    mpDictionaryView->mpDictGridView->AddColumn(dictKindColumn);
    mpDictionaryView->mpDictGridView->AddColumn(dictLocationColumn);

    // todo, update column width
    float labelWidth = 0.f;
    for (int i = 0; DictionaryTypeLabels[i].label != NULL; i++) {
        const char* label = B_TRANSLATE(DictionaryTypeLabels[i].label);
        dictKindColumn->AddKind(label);
        labelWidth = std::max(mpDictionaryView->StringWidth(label), labelWidth);
    }
    const float sep = mpDictionaryView->StringWidth("  ");
    mpDictionaryView->mpDictGridView->TitleView()->SetColumnWidth(1, labelWidth + sep * 3);

    Layout(true);

    // make tabview wider to show all tabs
    BSize size = tabView->Bounds().Size();
    size.width += 50;
    tabView->SetExplicitMinSize(size);

    _Load();
}


PreferenceDialog::~PreferenceDialog()
{
    if (mpUserLocationChooser && mpUserLocationChooser->Lock()) {
        mpUserLocationChooser->Quit();
    }
    if (mpLocationChooser && mpLocationChooser->Lock()) {
        mpLocationChooser->Quit();
    }
}


void PreferenceDialog::_Load()
{
    _LoadUserDefaults();
    _LoadRules();
    _LoadDictionarySet();
}


void PreferenceDialog::_LoadUserDefaults()
{
    std::string path = _FindFile(USER_DEFAULTS, USER_AND_SYSTEM, true);
    BMessage msg;
    if (_LoadJson(path.c_str(), &msg) == B_OK) {
        // input
        mpInputView->mpSuppressNewLineOnCommit->SetValue(
            msg.GetBool("suppress_newline_on_commit", true) ? B_CONTROL_ON : B_CONTROL_OFF);
        mpInputView->mpShowInputModeIcon->SetValue(
            msg.GetBool("show_input_mode_icon", false) ? B_CONTROL_ON : B_CONTROL_OFF);

        // completion
        mpCompletionView->mpEnableExtendedCompletion->SetValue(
            msg.GetBool("enable_dynamic_completion", true) ? B_CONTROL_ON : B_CONTROL_OFF);
        mpCompletionView->SetMinCompletionLength(
            int32(msg.GetDouble("minimum_completion_length", 1.)));
        mpCompletionView->mpEnableDynamicCompletion->SetValue(
            msg.GetBool("enable_dynamic_completion") ? B_CONTROL_ON : B_CONTROL_OFF);
        mpCompletionView->mpDynamicCompletionRange->SetValue(
            int32(msg.GetDouble("dynamic_completion_range", 1.)));

        // conversion
        mpConversionView->mpCandidateWindowLabels->SetText(
            msg.GetString("candidate_window_labels", "ASDFJKL"));
        mpConversionView->SetCandidateWindowPosition(
            msg.GetBool("put_candidate_window_upward", false));
        mpConversionView->SetCandidateWindowDirection(
            msg.GetBool("candidate_window_vertical", false));
        mpConversionView->mpDisplayShortestMatchOfKanaConversion->SetValue(
            msg.GetBool("display_shortest_match_of_kana_conversions", true) ? B_CONTROL_ON : B_CONTROL_OFF);
        mpConversionView->mpUseNumericConversion->SetValue(
            msg.GetBool("use_numeric_conversion", true) ? B_CONTROL_ON : B_CONTROL_OFF);
        mpConversionView->mpMaxCountOfInlineCandidates->SetValue(
            int32(msg.GetDouble("max_count_of_inline_candidates", 4.)));
        mpConversionView->mpInlineBackspaceImpliesCommit->SetValue(
            msg.GetBool("inline_backspace_implies_commit", true) ? B_CONTROL_ON : B_CONTROL_OFF);

        // display
        /*
        mpDisplayView->UseCustomFont(
            msg.GetBool("use_custom_font", false) ? B_CONTROL_ON : B_CONTROL_OFF);
        mpDisplayView->SetFontName(
            msg.GetString("candidate_window_font_name", "Noto Sans CJK JP"));
        mpDisplayView->mpCandidateWindowFontSize->SetValue(
            int32(msg.GetDouble("candidate_window_font_size", 12.)));
        */

        // dictionary
        mpDictionaryView->mpUserLocation->SetText(
            msg.GetString("user_dictionary_path",
                    "~/config/settings/aquaskk/skk-jisyo.utf8"));

        // misc
        mpMiscView->mpHandleRecursiveEntryAsOkuri->SetValue(
            msg.GetBool("handle_recursive_entry_as_okuri", true) ? B_CONTROL_ON : B_CONTROL_OFF);
        mpMiscView->mpDeleteOkuriWhenQuit->SetValue(
            msg.GetBool("delete_okuri_when_quit", true) ? B_CONTROL_ON : B_CONTROL_OFF);
        mpMiscView->mpBeepOnRegistration->SetValue(
            msg.GetBool("beep_on_registration", true) ? B_CONTROL_ON : B_CONTROL_OFF);
        mpMiscView->mpEnableAnnotation->SetValue(
            msg.GetBool("enable_annotation", true) ? B_CONTROL_ON : B_CONTROL_OFF);
        mpMiscView->mpEnableSKKServ->SetValue(
            msg.GetBool("skkserv_enable", false) ? B_CONTROL_ON : B_CONTROL_OFF);
        mpMiscView->SetSKKServPort(
            int32(msg.GetDouble("skkserv_port", 1178.)));
        mpMiscView->mpSKKServLocalOnly->SetValue(
            msg.GetBool("skkserv_localonly", true) ? B_CONTROL_ON : B_CONTROL_OFF);

        // keep these values for dictionary download
        fDictHost = msg.GetString("dictionary_host", "github.com");
        fDictPath = msg.GetString("dictionary_path", "/skk-dev/dict/raw/gh-pages");
    }
}


void PreferenceDialog::_LoadRules()
{
    _LoadRule(SYSTEM_ONLY, 0);
    _LoadRule(USER_ONLY, 1);
    // set active state
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
                    mpExtensionView->SetRuleState(id, true);
                }
            }
        }
    }
}


void PreferenceDialog::_LoadRule(Category category, int32 division)
{
    std::string rulePath = _FindFile(SUB_RULE, category, true);
    if (rulePath.empty()) {
        return;
    }
    BPath rule(rulePath.c_str());
    BPath basePath;
    rule.GetParent(&basePath);

    BMessage msg;
    if (_LoadJson(rulePath.c_str(), &msg) == B_OK) {
        BString name;
        const int32 count = msg.CountNames(B_MESSAGE_TYPE);
        for (int i = 0; i < count; i++) {
            BMessage d;
            if (msg.FindMessage(name.SetToFormat("%" B_PRIu32, i).String(), &d) == B_OK) {
                const char* id = NULL;
                const char* rule = NULL;
                const char* keymap = NULL;
                const char* description = NULL;
                if (d.FindString("id", &id) != B_OK) {
                    continue;
                }
                d.FindString("rule", &rule);
                d.FindString("keymap", &keymap);
                if (rule == NULL && keymap == NULL) {
                    continue;
                }
                if (rule != NULL && !_FileExists(basePath, rule)) {
                    continue;
                }
                if (keymap != NULL && !_FileExists(basePath, keymap)) {
                    continue;
                }
                if (d.FindString("description", &description) == B_OK) {
                    mpExtensionView->AddRule(id, false, description, division);
                }
            }
        }
    }
}


bool PreferenceDialog::_FileExists(const BPath &base, const char* name)
{
    BPath path(base);
    path.Append(name);
    BFile f(path.Path(), B_READ_ONLY);
    return f.IsReadable();
}


void PreferenceDialog::_LoadDictionarySet()
{
    std::string dicPath = _FindFile(DICTIONARY_SET, USER_ONLY, true);
    if (dicPath.empty()) {
        dicPath = _FindFile(DICTIONARY_SET, SYSTEM_ONLY, true);
        if (dicPath.empty()) {
            return;
        }
    }
    BMessage msg;
    if (_LoadJson(dicPath.c_str(), &msg) == B_OK) {
        BString name;
        const int32 count = msg.CountNames(B_MESSAGE_TYPE);
        for (int i = 0; i < count; i++) {
            BMessage d;
            if (msg.FindMessage(name.SetToFormat("%" B_PRIu32, i).String(), &d) == B_OK) {
                bool active = false;
                const char* loc = NULL;
                double dtype = 0;
                const char* etag = d.GetString("etag", "");
                if (d.FindBool("active", &active) == B_OK &&
                    d.FindString("location", &loc) == B_OK &&
                    d.FindDouble("type", &dtype) == B_OK) {
                    mpDictionaryView->AddDictionary(active, int32(dtype), loc, etag);
                }
            }
        }
    }
}


std::string PreferenceDialog::_FindFile(const char* name, Category category, bool exists)
{
    const directory_which DIRS[] = {
        B_USER_SETTINGS_DIRECTORY,
        B_USER_NONPACKAGED_DATA_DIRECTORY,
        B_SYSTEM_DATA_DIRECTORY,
    };

    BPath path;
    int i = category == SYSTEM_ONLY ? 1 : 0;
    const int lastIndex = category == USER_ONLY ? 0 : 2;
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


status_t PreferenceDialog::_LoadJson(const char* path, BMessage* msg)
{
    BJsonMessageWriter* writer = new BJsonMessageWriter(*msg);
    BFile f(path, B_READ_ONLY);
    BPrivate::BJson::Parse(&f, writer);
    status_t result = writer->ErrorStatus();
    delete writer;
    return result;
}


void PreferenceDialog::_Save()
{
    std::string appDir = _FindFile("", USER_ONLY, false);
    BDirectory dir;
    dir.CreateDirectory(appDir.c_str(), NULL);

    _SaveUserDefaults();
    _SaveActiveRules();
    _SaveDictionarySet();
}


void PreferenceDialog::_SaveActiveRules()
{
    std::vector<std::string> activeRules;
    const int32 count = mpExtensionView->mpGridView->CountRows();
    for (int32 i = 0; i < count; i++) {
        ExtensionRow* row = mpExtensionView->mpGridView->ItemAt(i);
        if (row) {
            if (row->IsActive()) {
                activeRules.push_back(std::string(row->Id()));
            }
        }
    }

    // active-rules.json format
    // ["id1", "id2"]

    std::string path = _FindFile(ACTIVE_RULES, USER_ONLY, false);
    BFile f(path.c_str(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
    if (!f.IsWritable()) {
        return;
    }
    BJsonTextWriter* writer = new BJsonTextWriter(&f);
    writer->WriteArrayStart();
    for (int i = 0; i < activeRules.size(); i++) {
        writer->WriteString(activeRules[i].c_str());
    }
    writer->WriteArrayEnd();
    writer->Complete();
    delete writer;
}


void PreferenceDialog::_SaveDictionarySet()
{
    // DictionarySet.json format
    // [{"active": bool, "location": loc, "type": type}, {...}]

    std::string path = _FindFile(DICTIONARY_SET, USER_ONLY, false);
    BFile f(path.c_str(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
    if (!f.IsWritable()) {
        return;
    }
    BJsonTextWriter* writer = new BJsonTextWriter(&f);
    writer->WriteArrayStart();

    const int32 count = mpDictionaryView->mpDictGridView->CountRows();
    for (int32 i = 0; i < count; i++) {
        writer->WriteObjectStart();

        DictRow* row = mpDictionaryView->mpDictGridView->ItemAt(i);
        if (row) {
            writer->WriteObjectName("active");
            writer->WriteBoolean(row->IsActive());
            writer->WriteObjectName("location");
            writer->WriteString(row->Location().c_str());
            writer->WriteObjectName("type");
            writer->WriteInteger(row->Type());
            if (!row->ETag().empty()) {
                writer->WriteObjectName("etag");
                writer->WriteString(row->ETag().c_str());
            }
        }

        writer->WriteObjectEnd();
    }

    writer->WriteArrayEnd();
    writer->Complete();
    delete writer;
}


void PreferenceDialog::_SaveUserDefaults()
{
    // UserDefaults.json format
    // { "property1": value, "property2": value }

    std::string path = _FindFile(USER_DEFAULTS, USER_ONLY, false);
    BFile f(path.c_str(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
    if (!f.IsWritable()) {
        return;
    }

    BJsonTextWriter* writer = new BJsonTextWriter(&f);
    writer->WriteObjectStart();
    // input
    writer->WriteObjectName("suppress_newline_on_commit");
    writer->WriteBoolean(
        mpInputView->mpSuppressNewLineOnCommit->Value() == B_CONTROL_ON);
    writer->WriteObjectName("show_input_mode_icon");
    writer->WriteBoolean(
        mpInputView->mpShowInputModeIcon->Value() == B_CONTROL_ON);

    // completion
    writer->WriteObjectName("enable_dynamic_completion");
    writer->WriteBoolean(
        mpCompletionView->mpEnableExtendedCompletion->Value() == B_CONTROL_ON);
    writer->WriteObjectName("minimum_completion_length");
    writer->WriteInteger(
        mpCompletionView->mpMinimumCompletionLength->Value());
    writer->WriteObjectName("enable_dynamic_completion");
    writer->WriteBoolean(
        mpCompletionView->mpEnableDynamicCompletion->Value() == B_CONTROL_ON);
    writer->WriteObjectName("dynamic_completion_range");
    writer->WriteInteger(
        mpCompletionView->mpDynamicCompletionRange->Value());

    // conversion
    writer->WriteObjectName("candidate_window_labels");
    writer->WriteString(
        mpConversionView->mpCandidateWindowLabels->Text());
    writer->WriteObjectName("put_candidate_window_upward");
    writer->WriteBoolean(
        mpConversionView->GetCandidateWindowPosition());
    writer->WriteObjectName("candidate_window_vertical");
    writer->WriteBoolean(
        mpConversionView->GetCandidateWindowDirection());
    writer->WriteObjectName("display_shortest_match_of_kana_conversions");
    writer->WriteBoolean(
        mpConversionView->mpDisplayShortestMatchOfKanaConversion->Value() == B_CONTROL_ON);
    writer->WriteObjectName("use_numeric_conversion");
    writer->WriteBoolean(
        mpConversionView->mpUseNumericConversion->Value() == B_CONTROL_ON);
    writer->WriteObjectName("max_count_of_inline_candidates");
    writer->WriteInteger(
        mpConversionView->mpMaxCountOfInlineCandidates->Value());
    writer->WriteObjectName("inline_backspace_implies_commit");
    writer->WriteBoolean(
        mpConversionView->mpInlineBackspaceImpliesCommit->Value() == B_CONTROL_ON);
/*
    // display
    writer->WriteObjectName("use_custom_font");
    writer->WriteBoolean(
        mpDisplayView->mpUseCustomFont->Value() == B_CONTROL_ON);
    writer->WriteObjectName("candidate_window_font_name");
    writer->WriteString(
        mpDisplayView->GetFontName());
    writer->WriteObjectName("candidate_window_font_size");
    writer->WriteInteger(
        mpDisplayView->mpCandidateWindowFontSize->Value());
*/
    // dictionary
    writer->WriteObjectName("user_dictionary_path");
    writer->WriteString(
        mpDictionaryView->mpUserLocation->Text());

    // misc
    writer->WriteObjectName("handle_recursive_entry_as_okuri");
    writer->WriteBoolean(
        mpMiscView->mpHandleRecursiveEntryAsOkuri->Value() == B_CONTROL_ON);
    writer->WriteObjectName("delete_okuri_when_quit");
    writer->WriteBoolean(
        mpMiscView->mpDeleteOkuriWhenQuit->Value() == B_CONTROL_ON);
    writer->WriteObjectName("beep_on_registration");
    writer->WriteBoolean(
        mpMiscView->mpBeepOnRegistration->Value() == B_CONTROL_ON);
    writer->WriteObjectName("enable_annotation");
    writer->WriteBoolean(
        mpMiscView->mpEnableAnnotation->Value() == B_CONTROL_ON);
    writer->WriteObjectName("skkserv_enable");
    writer->WriteBoolean(
        mpMiscView->mpEnableSKKServ->Value() == B_CONTROL_ON);
    writer->WriteObjectName("skkserv_port");
    writer->WriteInteger(
        mpMiscView->GetSKKServPort());
    writer->WriteObjectName("skkserv_localonly");
    writer->WriteBoolean(
        mpMiscView->mpSKKServLocalOnly->Value() == B_CONTROL_ON);

    //
    writer->WriteObjectName("dictionary_host");
    writer->WriteString(fDictHost.c_str());
    writer->WriteObjectName("dictionary_path");
    writer->WriteString(fDictPath.c_str());

    writer->WriteObjectEnd();

    writer->Complete();
    delete writer;
}


void PreferenceDialog::_DownloadDictionary()
{
    std::string downloaderPath = _FindFile(DICT_DOWNLOADER, SYSTEM_ONLY, true);
    std::string dictDir = _FindFile(DICT_DIR, USER_ONLY, false);
    std::string dictSet = _FindFile(DICTIONARY_SET, USER_ONLY, false);
    BString types;
    types.SetToFormat("%d,%d",
            DictionaryTypes::Download, DictionaryTypes::DownloadUTF8);

    const char* argv[] = {
        "/bin/python", downloaderPath.c_str(),
        "--dictdir", dictDir.c_str(),
        "--dictset", dictSet.c_str(),
        "--host", fDictHost.c_str(),
        "--path", fDictPath.c_str(),
        "--types", types.String(),
        NULL,
    };
/*
    BEntry entry;
    entry.SetTo("/bin/env");
    entry_ref ref;
    if (entry.GetRef(&ref) == B_OK) {
        be_roster->Launch(&ref, 12, argv);
    }
    */

    thread_id tid = load_image(12, argv, (const char**)environ);
    if (tid != B_ERROR) {
        //resume_thread(tid);
        mpDictionaryView->mpDownload->SetEnabled(false);
        mpOkButton->SetEnabled(false);
        mpCancelButton->SetEnabled(false);

        status_t ret = -1;
        wait_for_thread(tid, &ret);

        mpDictionaryView->mpDownload->SetEnabled(true);
        mpOkButton->SetEnabled(true);
        mpCancelButton->SetEnabled(true);
    }
}


void PreferenceDialog::MessageReceived(BMessage* msg)
{
    switch (msg->what)
    {
        case OK:
        {
            _Save();
            Quit();
            break;
        }
        case CANCEL:
        {
            QuitRequested();
            Quit();
            break;
        }
        case IMP_DICT_USER_LOCATION:
        {
            if (!mpUserLocationChooser) {
                FileFilter* filter = new FileFilter();
                filter->AddFileFilter(B_TRANSLATE("All files (*.*)"), "");
                mpUserLocationChooser = new FileChooser(
                        this, B_OPEN_PANEL, filter, IMP_DICT_USER_LOCATION);
            }
            mpUserLocationChooser->Show();
            break;
        }
        case IMP_DICT_LOCATION:
        {
            if (!mpLocationChooser) {
                FileFilter* filter = new FileFilter();
                filter->AddFileFilter(B_TRANSLATE("All files (*.*)"), "");
                mpLocationChooser = new FileChooser(
                        this, B_OPEN_PANEL, filter, IMP_DICT_LOCATION);
            }
            mpLocationChooser->Show();
            break;
        }
        case B_REFS_RECEIVED:
        {
            entry_ref ref;
            if (msg->FindRef("refs", &ref) == B_OK) {
                const int32 kind = msg->GetInt32("kind", 0);
                BPath path(&ref);
                if (kind == IMP_DICT_USER_LOCATION) {
                    mpDictionaryView->mpUserLocation->SetText(path.Path());
                } else if (kind == IMP_DICT_LOCATION) {
                    std::string filePath(path.Path());
                    BPath settingsPath;
                    if (find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath) == B_OK) {
                        settingsPath.Append(APP_DIR_NAME);
                        settingsPath.Append(DICT_DIR);
                        if (filePath.find(settingsPath.Path() == 0)) {
                            filePath.replace(0, strlen(settingsPath.Path()), "");
                            mpDictionaryView->SetLocation(filePath.c_str());
                            break;
                        }
                    }
                    BPath userPath;
                    if (find_directory(B_USER_DIRECTORY , &userPath) == B_OK) {
                        if (filePath.find(userPath.Path()) == 0) {
                            filePath.replace(0, strlen(userPath.Path()), "~");
                            mpDictionaryView->SetLocation(filePath.c_str());
                            break;
                        }
                    }
                }
            }
            break;
        }
        case IMP_DICT_ADD:
        {
            mpDictionaryView->MessageReceived(msg);
            break;
        }
        case IMP_DICT_REMOVE:
        {
            mpDictionaryView->MessageReceived(msg);
            break;
        }
        case IMP_DICT_UP:
        {
            mpDictionaryView->MessageReceived(msg);
            break;
        }
        case IMP_DICT_DOWN:
        {
            mpDictionaryView->MessageReceived(msg);
            break;
        }
        case IMP_DICT_DOWNLOAD:
        {
            _SaveDictionarySet();
            _DownloadDictionary();
            break;
        }
        /*
        case IMP_DISPLAY_CUSTOM_FONT:
        {
            mpDisplayView->UseCustomFont(
                mpDisplayView->mpUseCustomFont->Value() == B_CONTROL_ON);
            break;
        }
        */
        case GRID_SELECTION_CHANGED:
        {
            mpDictionaryView->MessageReceived(msg);
            break;
        }
        default:
        {
            BWindow::MessageReceived(msg);
            break;
        }
    }
}

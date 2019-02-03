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

#include "SKKConstVars.h"

#include <FindDirectory.h>
#include <Path.h>

#include "HaikuConstants.h"


namespace SKKUserDefaultKeys
{
    bool suppress_newline_on_commit;
    bool use_numeric_conversion;
    bool show_input_mode_icon;
    bool beep_on_registration;
    bool inline_backspace_implies_commit;
    bool delete_okuri_when_quit;

    bool  enable_extended_completion;
    bool  enable_dynamic_completion;
    int32 dynamic_completion_range;
    int32 minimum_completion_length;

    int32 max_count_of_inline_candidates;
    std::string candidate_window_labels;
    bool use_custom_font;
    std::string candidate_window_font_name;
    int32 candidate_window_font_size;
    bool  put_candidate_window_upward;
    bool  enable_annotation;

    std::string user_dictionary_path;

    bool  skkserv_enable;
    bool  skkserv_localonly;
    int32 skkserv_port;

    bool enable_private_mode;

    bool fix_intermediate_conversion;
    bool display_shortest_match_of_kana_conversions;
    bool handle_recursive_entry_as_okuri;

    std::string dictionary_host;
    std::string dictionary_path;

    //bool direct_clients;
    
    bool candidate_window_vertical;
};


namespace SKKFilePaths
{
    static std::string pathForSystemResourceFolder()
    {
        BPath path;
        if (find_directory(B_SYSTEM_DATA_DIRECTORY, &path) == B_OK) {
            static std::string spath(path.Path());
            return spath;
        }
        return "";
    }

    static std::string pathForApplicationSupportFolder()
    {
        BPath path;
        if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) == B_OK) {
            static std::string spath(path.Path());
            return spath;
        }
        return "";
    }

    static std::string pathForDictionarySet()
    {
        BPath path(pathForApplicationSupportFolder().c_str());
        path.Append(APP_DIR_NAME);
        path.Append(DICTIONARY_SET);
        static std::string spath(path.Path());
        return spath;
    }

    static std::string pathForUserDefaults()
    {
        BPath path(pathForApplicationSupportFolder().c_str());
        path.Append(APP_DIR_NAME);
        path.Append(USER_DEFAULTS);
        static std::string spath(path.Path());
        return spath;
    }

    std::string SystemResourceFolder = pathForSystemResourceFolder();
    std::string ApplicationSupportFolder = pathForApplicationSupportFolder();
    std::string DictionarySet = pathForDictionarySet();
    std::string UserDefaults = pathForUserDefaults();
};


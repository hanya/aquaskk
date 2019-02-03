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

#ifndef SKKConstVars_h
#define SKKConstVars_h

#include <SupportDefs.h>

#include <string>


namespace SKKUserDefaultKeys
{
    extern bool suppress_newline_on_commit;
    extern bool use_numeric_conversion;
    extern bool show_input_mode_icon;
    extern bool beep_on_registration;
    extern bool inline_backspace_implies_commit;
    extern bool delete_okuri_when_quit;

    extern bool  enable_extended_completion;
    extern bool  enable_dynamic_completion;
    extern int32 dynamic_completion_range;
    extern int32 minimum_completion_length;

    extern int32       max_count_of_inline_candidates;
    extern std::string candidate_window_labels;
    extern bool use_custom_font;
    extern std::string candidate_window_font_name;
    extern int32       candidate_window_font_size;
    extern bool        put_candidate_window_upward;
    extern bool        enable_annotation;

    extern std::string user_dictionary_path;

    extern bool  skkserv_enable;
    extern bool  skkserv_localonly;
    extern int32 skkserv_port;

    extern bool enable_private_mode;

    extern bool fix_intermediate_conversion;
    extern bool display_shortest_match_of_kana_conversions;
    extern bool handle_recursive_entry_as_okuri;

    extern std::string dictionary_host;
    extern std::string dictionary_path;

    //extern bool direct_clients;

    extern bool candidate_window_vertical;
};


namespace SKKFilePaths {
    extern std::string SystemResourceFolder;
    extern std::string ApplicationSupportFolder;
    extern std::string DictionarySet;
    extern std::string UserDefaults;
};


#endif

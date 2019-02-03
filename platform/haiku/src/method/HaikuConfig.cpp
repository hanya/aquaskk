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

#include "HaikuConfig.h"

#include "SKKConstVars.h"


HaikuConfig::HaikuConfig()
{
}

bool HaikuConfig::FixIntermediateConversion()
{
    return SKKUserDefaultKeys::fix_intermediate_conversion;
}

bool HaikuConfig::EnableDynamicCompletion()
{
    return SKKUserDefaultKeys::enable_dynamic_completion;
}

int HaikuConfig::DynamicCompletionRange()
{
    return SKKUserDefaultKeys::dynamic_completion_range;
}

bool HaikuConfig::EnableAnnotation()
{
    return SKKUserDefaultKeys::enable_annotation;
}

bool HaikuConfig::DisplayShortestMatchOfKanaConversions()
{
    return SKKUserDefaultKeys::display_shortest_match_of_kana_conversions;
}

bool HaikuConfig::SuppressNewlineOnCommit()
{
    return SKKUserDefaultKeys::suppress_newline_on_commit;
}

int HaikuConfig::MaxCountOfInlineCandidates()
{
    return SKKUserDefaultKeys::max_count_of_inline_candidates;
}

bool HaikuConfig::HandleRecursiveEntryAsOkuri()
{
    return SKKUserDefaultKeys::handle_recursive_entry_as_okuri;
}

bool HaikuConfig::InlineBackSpaceImpliesCommit()
{
    return SKKUserDefaultKeys::inline_backspace_implies_commit;
}

bool HaikuConfig::DeleteOkuriWhenQuit()
{
    return SKKUserDefaultKeys::delete_okuri_when_quit;
}

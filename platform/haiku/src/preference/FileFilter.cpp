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

#include "FileFilter.h"

#include <FilePanel.h>

#include <compat/sys/stat.h>
#include <string.h>


FileFilter::FileFilter()
    : BRefFilter(),
      fFilterIndex(-1)
{
}

FileFilter::~FileFilter()
{
}

void FileFilter::AddFileFilter(const char* name, const char* ext)
{
    FilterItem f;
    f.name = name;
    // split with ;
    std::string filter(ext);
    int n;
    int prev = 0;
    while (true) {
        n = filter.find(';', prev);
        if (n != std::string::npos) {
            f.filters.push_back(filter.substr(prev, n-1));
            prev = n+1;
        } else {
            f.filters.push_back(filter.substr(prev, filter.size()));
            break;
        }
    }

    fFilters.push_back(f);
}

int32 FileFilter::FilterIndex()
{
    return fFilterIndex;
}

bool FileFilter::SetFilterIndex(int32 index)
{
    bool changed = fFilterIndex != index;
    fFilterIndex = index;
    return changed;
}

const char * FileFilter::FilterName(int32 index)
{
    if (0 <= index && index < fFilters.size()) {
        return fFilters[index].name.c_str();
    }
    return NULL;
}

int32 FileFilter::FilterCount()
{
    return fFilters.size();
}

bool FileFilter::Filter(const entry_ref* ref, BNode* node,
                        struct stat_beos* stat, const char* mimeType)
{
    if (ref == NULL || ref->name == NULL) {
        return false;
    }
    if (S_ISDIR(stat->st_mode)) {
        return true;
    }
    if (!(0 <= fFilterIndex && fFilterIndex < fFilters.size())) {
        return true;
    }
    if (fFilters.size() == 0) {
        return true;
    }

    auto i = fFilters[fFilterIndex].filters.begin();
    auto e = fFilters[fFilterIndex].filters.end();
    for (; i != e; ++i) {
        const char* ext = (*i).c_str();
        int ext_length = (*i).size();
        if (ext_length == 0) {
            return true; // All *.*
        }

        int length = strlen(ref->name);

        if (length < ext_length) {
            continue;
        }
        char* found = NULL;
        char* prev = NULL;
        found = strstr(ref->name, ext);
        prev = found;
        while (found != NULL) {
            found = strstr(prev+1, ext);
            if (found != NULL) {
                prev = found;
            }
        }
        if (prev != NULL && (length - ext_length) == (prev - ref->name)) {
            return true;
        }
    }

    return false;
}


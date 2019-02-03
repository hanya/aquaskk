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

#ifndef FileFilter_H
#define FileFilter_H

#include <FilePanel.h>

#include <string>
#include <vector>


class FileFilter : public BRefFilter
{
public:
    FileFilter();
    virtual ~FileFilter();
    virtual bool Filter(const entry_ref* ref, BNode* node,
                        struct stat_beos* stat, const char* mimeType);

    void AddFileFilter(const char* name, const char* ext);
    int32 FilterIndex();
    bool SetFilterIndex(int32 index);
    const char* FilterName(int32 index);
    int32 FilterCount();
private:
    typedef struct {
        std::string name;
        std::vector<std::string> filters;
    } FilterItem;
    std::vector<FilterItem> fFilters;
    int32 fFilterIndex;
};


#endif // FileFilter_H

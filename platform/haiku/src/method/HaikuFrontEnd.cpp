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

#include "HaikuFrontEnd.h"

#include "HaikuLooper.h"


HaikuFrontEnd::HaikuFrontEnd(HaikuLooper* looper)
    : fLooper(looper)
{
}


void HaikuFrontEnd::InsertString(const std::string& str) {
    fLooper->InsertString(str);
}

void HaikuFrontEnd::ComposeString(const std::string& str,
                                  int cursorOffset) {
    fLooper->ComposeString(str, cursorOffset);
}

void HaikuFrontEnd::ComposeString(const std::string& str,
                    int candidateStart, int candidateLength) {
    fLooper->ComposeString(str, candidateStart, candidateLength);
}

std::string HaikuFrontEnd::SelectedString() {
    return fLooper->SelectedString();
}

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

#include "SKKPreProcessor.h"

#include "SKKKeyState.h"

#include <InterfaceDefs.h>


SKKPreProcessor::SKKPreProcessor()
{
}


SKKPreProcessor& SKKPreProcessor::theInstance() {
    static SKKPreProcessor obj;
    return obj;
}


void SKKPreProcessor::Initialize(const std::string& path) {
    fKeymap.Initialize(path);
}


void SKKPreProcessor::Patch(const std::string& path) {
    fKeymap.Patch(path);
}


SKKEvent SKKPreProcessor::Execute(const BMessage* msg) {
    // todo
    int32 key = msg->GetInt32("key", 0); // raw keycode
    uint8 byte = (uint8)msg->GetInt8("byte", 0); // ascii code
    uint32 modifiers = (uint32)msg->GetInt32("modifiers", 0);

    int mods = 0;

    // todo, shift
    if (modifiers &
        (B_LEFT_SHIFT_KEY | B_RIGHT_SHIFT_KEY | B_SHIFT_KEY)) {
        // check set of shift keys is enough
        //if (std::isgraph(byte)) {
            // what should we do here? do we need this?
        //}

        mods += SKKKeyState::SHIFT;
    }

    if (modifiers &
        (B_COMMAND_KEY | B_LEFT_COMMAND_KEY | B_RIGHT_COMMAND_KEY)) {
        mods += SKKKeyState::CTRL;
    }

    if (modifiers &
        (B_CONTROL_KEY | B_LEFT_CONTROL_KEY | B_RIGHT_CONTROL_KEY)) {
        mods += SKKKeyState::ALT;
    }

    // todo, meta?

    // check other key

    SKKEvent result = fKeymap.Fetch(byte, key, mods);

    if (modifiers & B_CAPS_LOCK) {
        result.option |= CapsLock;
    }

    return result;
}


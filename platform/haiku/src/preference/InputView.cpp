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

#include "InputView.h"

#include <Catalog.h>
#include <CheckBox.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>
#include <StringView.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Preference"


InputView::InputView(const char* name)
    : BView(name, B_WILL_DRAW)
{
    BStringView* label1 = new BStringView("label", B_TRANSLATE("Input"));
    label1->SetAlignment(B_ALIGN_RIGHT);

    mpSuppressNewLineOnCommit = new BCheckBox("suppress",
            B_TRANSLATE("Suppress new line on commit"), new BMessage());
    mpShowInputModeIcon = new BCheckBox("showinputmode",
            B_TRANSLATE("Show input mode icon"), new BMessage());
    mpShowInputModeIcon->SetEnabled(false);

    BLayoutBuilder::Group<>(this, B_VERTICAL)
        .SetInsets(5, 5, 5, 5)
        .AddGroup(B_HORIZONTAL)
            .Add(label1)
            .Add(new BSeparatorView(B_HORIZONTAL))
        .End()
        .AddGroup(B_VERTICAL)
            .SetInsets(5, 0, 0, 0)
            .Add(mpSuppressNewLineOnCommit)
            .Add(mpShowInputModeIcon)
            .AddGlue()
        .End();

    Layout(true);
}

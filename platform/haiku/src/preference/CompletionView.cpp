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

#include "CompletionView.h"


#include <Catalog.h>
#include <CheckBox.h>
#include <LayoutBuilder.h>
#include <MessageId.h>
#include <SeparatorView.h>
#include <Slider.h>
#include <Spinner.h>
#include <StringView.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Preference"


const int32 MIN_COMPLETION_LENGTH = 0;
const int32 MAX_COMPLETION_LENGTH = 15;


CompletionView::CompletionView(const char* name)
    : BView(name, B_WILL_DRAW)
{
    BStringView* label1 = new BStringView("compop", B_TRANSLATE("Completion"));
    label1->SetAlignment(B_ALIGN_RIGHT);
    mpEnableExtendedCompletion = new BCheckBox("compen", B_TRANSLATE("Enable"),
                new BMessage());
    BStringView* label2 = new BStringView("compfil", B_TRANSLATE("Completion filter"));
    //label2->SetAlignment(B_ALIGN_RIGHT);
    mpMinimumCompletionLength = new BSlider("dynsl", "",
                new BMessage(IMP_COMP_MINIMUM_LENGTH),
                MIN_COMPLETION_LENGTH, MAX_COMPLETION_LENGTH, B_HORIZONTAL);
    mpCompletionLength = new BStringView("count", "");
    BStringView* label3 = new BStringView("dyn", B_TRANSLATE("ignore characters below"));
    label3->SetAlignment(B_ALIGN_RIGHT);

    BStringView* label4 = new BStringView("dyn", B_TRANSLATE("Dynamic completion"));
    mpEnableDynamicCompletion = new BCheckBox("dynen", B_TRANSLATE("Enable"),
                new BMessage());
    BStringView* label5 = new BStringView("dyn", B_TRANSLATE("Show up to"));
    mpDynamicCompletionRange = new BSpinner("spinn", "",
                new BMessage(IMP_COMP_DYNAMIC_RANGE));
    mpDynamicCompletionRange->SetRange(1, 100);
    mpDynamicCompletionRange->SetExplicitMaxSize(
            mpDynamicCompletionRange->MinSize());

    BLayoutBuilder::Group<>(this, B_VERTICAL)
        .SetInsets(5, 5, 5, 5)
        .AddGroup(B_HORIZONTAL)
            .Add(label1)
            .Add(new BSeparatorView(B_HORIZONTAL))
        .End()
        .AddGroup(B_VERTICAL)
            .SetInsets(5, 0, 0, 0)
            .Add(mpEnableExtendedCompletion)
            .AddGroup(B_HORIZONTAL)
                .AddGroup(B_VERTICAL)
                    .Add(label2)
                    .AddGlue()
                .End()
                .AddGroup(B_VERTICAL)
                    .AddGroup(B_HORIZONTAL)
                        .Add(mpMinimumCompletionLength)
                        .AddGlue()
                    .End()
                    .AddGroup(B_HORIZONTAL)
                        .Add(mpCompletionLength)
                        .Add(label3)
                        .AddGlue()
                    .End()
                .End()
            .End()
            .AddGlue()
        .End()
        .AddGroup(B_HORIZONTAL)
            .Add(label4)
            .Add(new BSeparatorView(B_HORIZONTAL))
        .End()
        .AddGroup(B_VERTICAL)
            .SetInsets(5, 0, 0, 0)
            .Add(mpEnableDynamicCompletion)
            .AddGroup(B_HORIZONTAL)
                .Add(label5)
                .Add(mpDynamicCompletionRange)
                .AddGlue()
            .End()
            .AddGlue()
        .End()
        .AddGlue();

    Layout(true);
}


void CompletionView::MessageReceived(BMessage* msg)
{
    switch (msg->what)
    {
        case IMP_COMP_MINIMUM_LENGTH:
        {
            _SetMinCompletionLengthText(mpMinimumCompletionLength->Value());
            break;
        }
        default:
        {
            BView::MessageReceived(msg);
            break;
        }
    }
}


void CompletionView::SetMinCompletionLength(int32 value)
{
    mpMinimumCompletionLength->SetValue(value);
    _SetMinCompletionLengthText(value);
}


void CompletionView::_SetMinCompletionLengthText(int32 value)
{
    BString s;
    s.SetToFormat("%d", value);
    mpCompletionLength->SetText(s.String());
}

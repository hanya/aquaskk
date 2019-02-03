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

#include "ConversionView.h"

#include <Catalog.h>
#include <CheckBox.h>
#include <LayoutBuilder.h>
#include <MessageId.h>
#include <PopUpMenu.h>
#include <SeparatorView.h>
#include <Spinner.h>
#include <StringView.h>
#include <TextControl.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Preference"


enum {
    IM_CONV_CANDIDATE_ABOVE = 'IMca',
    IM_CONV_CANDIDATE_BELOW = 'IMcb',
    IM_CONV_CANDIDATE_HORIZONTAL = 'IMch',
    IM_CONV_CANDIDATE_VERTICAL = 'IMcv',
};


ConversionView::ConversionView(const char* name)
    : BView(name, B_WILL_DRAW)
{
    BStringView* label0 = new BStringView("conv", B_TRANSLATE("Conversion"));
    BStringView* label1 = new BStringView("anno", B_TRANSLATE("Annotation"));
    BStringView* label2 = new BStringView("showpos", B_TRANSLATE("Candidates"));
    BStringView* label4 = new BStringView("candw", B_TRANSLATE("Candidate labels"));
    BStringView* label5 = new BStringView("inline", B_TRANSLATE("Max inline candidates"));
    BStringView* label7 = new BStringView("nlabel", B_TRANSLATE("Special conversion"));
    BStringView* label8 = new BStringView("inlin", B_TRANSLATE("Inline conversion"));
    BStringView* label9 = new BStringView("label1", B_TRANSLATE("Roman to kana conversion"));
    BStringView* label6 = new BStringView("label6", B_TRANSLATE("Fixing"));
    BStringView* label3 = new BStringView("label10", B_TRANSLATE("Candidate direction"));

    mpCandidatePositionPM = new BPopUpMenu("position");
    mpCandidatePositionPM->SetRadioMode(true);
    BLayoutBuilder::Menu<>(mpCandidatePositionPM)
        .AddItem(B_TRANSLATE("Above"), new BMessage(IM_CONV_CANDIDATE_ABOVE))
        .AddItem(B_TRANSLATE("Below"), new BMessage(IM_CONV_CANDIDATE_BELOW));
    mpPutCandidateWindowUpward = new BMenuField("posMF", "", mpCandidatePositionPM);

    mpCandidateDirectionPM = new BPopUpMenu("direction");
    mpCandidateDirectionPM->SetRadioMode(true);
    BLayoutBuilder::Menu<>(mpCandidateDirectionPM)
        .AddItem(B_TRANSLATE("Horizontal"), new BMessage(IM_CONV_CANDIDATE_HORIZONTAL))
        .AddItem(B_TRANSLATE("Vertical"), new BMessage(IM_CONV_CANDIDATE_VERTICAL));
    mpCandidateDirection = new BMenuField("dirMF", "", mpCandidateDirectionPM);

    mpCandidateWindowLabels = new BTextControl("", "", new BMessage(100));
    BSize minSize = mpCandidateWindowLabels->MinSize();
    mpCandidateWindowLabels->SetExplicitMaxSize(
            BSize(StringWidth("00000000000") * 2.5, minSize.height));
    mpMaxCountOfInlineCandidates = new BSpinner("", "",
            new BMessage(IMP_CONV_MAX_INLINE_CANDIDATE));
    mpMaxCountOfInlineCandidates->SetRange(0, 100);
    mpMaxCountOfInlineCandidates->SetExplicitMaxSize(
            mpMaxCountOfInlineCandidates->MinSize());

    mpUseNumericConversion = new BCheckBox("useNumeric",
            B_TRANSLATE("Numeric conversion"), new BMessage());
    mpInlineBackspaceImpliesCommit = new BCheckBox("inline",
            B_TRANSLATE("Inline backspace implies commit"), new BMessage());
    mpDisplayShortestMatchOfKanaConversion = new BCheckBox("display",
            B_TRANSLATE("Display shortest match of kana conversion"), new BMessage());

    BLayoutBuilder::Group<>(this, B_VERTICAL)
        .SetInsets(5, 5, 5, 5)
        .AddGroup(B_HORIZONTAL)
            .Add(label0)
            .Add(new BSeparatorView(B_HORIZONTAL))
        .End()
        .AddGrid()
            .SetInsets(5, 0, 0, 0)
            .Add(label4, 0, 0)
            .Add(mpCandidateWindowLabels, 1, 0)
            .Add(label2, 0, 1)
            .Add(mpPutCandidateWindowUpward, 1, 1)
            .Add(label3, 0, 2)
            .Add(mpCandidateDirection, 1, 2)
            .Add(label9, 0, 3)
            .Add(mpDisplayShortestMatchOfKanaConversion, 1, 3)
        .End()
        .AddGroup(B_HORIZONTAL)
            .Add(label7)
            .Add(new BSeparatorView(B_HORIZONTAL))
        .End()
        .AddGrid()
            .SetInsets(5, 0, 0, 0)
            .Add(mpUseNumericConversion, 0, 0)
        .End()
        .AddGroup(B_HORIZONTAL)
            .Add(label8)
            .Add(new BSeparatorView(B_HORIZONTAL))
        .End()
        .AddGrid()
            .SetInsets(5, 5, 0, 0)
            .Add(label5, 0, 0)
            .Add(mpMaxCountOfInlineCandidates, 1, 0)
            .Add(label6, 0, 1)
            .Add(mpInlineBackspaceImpliesCommit, 1, 1)
        .End()
        .AddGlue();

    Layout(true);
}


bool ConversionView::GetCandidateWindowPosition()
{
    BMenuItem* item = mpCandidatePositionPM->FindMarked();
    if (item) {
        BMessage* msg = item->Message();
        return msg->what == IM_CONV_CANDIDATE_ABOVE;
    }
    return false;
}


void ConversionView::SetCandidateWindowPosition(bool upward)
{
    const int32 id = upward ? IM_CONV_CANDIDATE_ABOVE : IM_CONV_CANDIDATE_BELOW;
    BMenuItem* item = mpCandidatePositionPM->FindItem(id);
    if (item) {
        item->SetMarked(true);
    }
}


bool ConversionView::GetCandidateWindowDirection()
{
    BMenuItem* item = mpCandidateDirectionPM->FindMarked();
    if (item) {
        BMessage* msg = item->Message();
        return msg->what == IM_CONV_CANDIDATE_VERTICAL;
    }
    return false;
}


void ConversionView::SetCandidateWindowDirection(bool vertical)
{
    const int32 id = !vertical ? IM_CONV_CANDIDATE_HORIZONTAL : IM_CONV_CANDIDATE_VERTICAL;
    BMenuItem* item = mpCandidateDirectionPM->FindItem(id);
    if (item) {
        item->SetMarked(true);
    }
}

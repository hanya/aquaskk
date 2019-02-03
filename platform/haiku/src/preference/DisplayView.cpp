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

#include "DisplayView.h"

#include <Catalog.h>
#include <CheckBox.h>
#include <LayoutBuilder.h>
#include <PopUpMenu.h>
#include <SeparatorView.h>
#include <Spinner.h>
#include <StringView.h>

#include <MessageId.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Preference"


#define FONT_NAME_CHOOSE 'fdnc'
#define FONT_STYLE_CHOOSE 'fdsc'


void AddFonts(BPopUpMenu* fontPM)
{
    int32 families = count_font_families();
    font_family name;
    font_style style;
    for (int32 i = 0; i < families; i++) {
        if (get_font_family(i, &name) == B_OK) {
            int32 styles = count_font_styles(name);
            if (styles == 0) {
                continue;
            }
            fontPM->AddItem(new BMenuItem(name, new BMessage(FONT_NAME_CHOOSE)));
            /*
            BMenu* stylesPM = new BPopUpMenu(name);
            stylesPM->SetRadioMode(true);
            stylesPM->SetLabelFromMarked(false);
            fontPM->AddItem(stylesPM);
            for (int32 j = 0; j < styles; j++) {
                if (get_font_style(name, j, &style) == B_OK) {
                    BMessage* mess = new BMessage(FONT_STYLE_CHOOSE);
                    mess->AddInt32("family", i);
                    BMenuItem* item = new BMenuItem(style, mess);
                    stylesPM->AddItem(item);
                    if (j == 0) {
                        item->SetMarked(true);
                    }
                }
            }
            */
        }
    }
}


DisplayView::DisplayView(const char* name)
    : BView(name, B_WILL_DRAW)
{
    BStringView* label1 = new BStringView("label1", B_TRANSLATE("Display"));
    BStringView* label2 = new BStringView("font", B_TRANSLATE("Candidate font"));
    BStringView* label3 = new BStringView("fsize", B_TRANSLATE("Size"));

    mpUseCustomFont = new BCheckBox("ussyf",
            B_TRANSLATE("Use custom font"), new BMessage(IMP_DISPLAY_CUSTOM_FONT));
    mpFontPM = new BPopUpMenu("fontname");
    mpFontPM->SetRadioMode(true);
    AddFonts(mpFontPM);

    mpCandidateWindowFontName = new BMenuField("fontMF", "", mpFontPM);
    mpCandidateWindowFontSize = new BSpinner("", "", new BMessage());
    mpCandidateWindowFontSize->SetRange(8, 72);
    BSize size = mpCandidateWindowFontSize->PreferredSize();
    BSize minSize = mpCandidateWindowFontSize->MinSize();
    mpCandidateWindowFontSize->SetExplicitMaxSize(BSize(minSize.width, size.height));

    BLayoutBuilder::Group<>(this, B_VERTICAL)
        .SetInsets(5, 5, 5, 5)
        .AddGroup(B_HORIZONTAL)
            .Add(label1)
            .Add(new BSeparatorView(B_HORIZONTAL))
        .End()
        .AddGroup(B_VERTICAL)
            .SetInsets(5, 0, 0, 0)
            .Add(mpUseCustomFont)
        .End()
        .AddGrid(B_VERTICAL)
            .SetInsets(10, 5, 0, 0)
            .Add(label2, 0, 0)
            .AddGroup(B_HORIZONTAL, 5, 1, 0)
                .Add(mpCandidateWindowFontName)
                .AddGlue()
                .Add(label3)
                .Add(mpCandidateWindowFontSize)
            .End()
        .End()
        .AddGlue();

    Layout(true);
}


void DisplayView::UseCustomFont(bool state)
{
    mpCandidateWindowFontName->SetEnabled(state);
    mpCandidateWindowFontSize->SetEnabled(state);
}


const char* DisplayView::GetFontName() const
{
    BMenuItem* item = mpFontPM->FindMarked();
    if (item) {
        return item->Label();
    }
    return "";
}


void DisplayView::SetFontName(const char* name)
{
    for (int i = 0; i < mpFontPM->CountItems(); i++) {
        BMenuItem* item = mpFontPM->ItemAt(i);
        if (item) {
            if (strcmp(name, item->Label()) == 0) {
                item->SetMarked(true);
                break;
            }
        }
    }
}

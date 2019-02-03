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

#include "MiscView.h"

#include <Catalog.h>
#include <CheckBox.h>
#include <LayoutBuilder.h>
#include <SeparatorView.h>
#include <StringView.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Preference"


MiscView::MiscView(const char* name)
    : BView(name, B_WILL_DRAW)
{
    BStringView* labels1 = new BStringView("servlabel", B_TRANSLATE("skkserv"));
    labels1->SetAlignment(B_ALIGN_RIGHT);
    mpEnableSKKServ = new BCheckBox("enable", B_TRANSLATE("Enable"), new BMessage());
    BStringView* labels2 = new BStringView("port", B_TRANSLATE("port"));
    mpSKKServPort = new BTextControl("porttc", "", "", new BMessage());
    mpSKKServPort->TextView()->SetMaxBytes(5);
    BSize minSize = mpSKKServPort->MinSize();
    mpSKKServPort->SetExplicitMaxSize(
            BSize(StringWidth("000000000000"), minSize.height));
    mpSKKServLocalOnly = new BCheckBox("local",
        B_TRANSLATE("Reject connection out of localhost"), new BMessage());
    mpEnableAnnotation = new BCheckBox("anoch",
        B_TRANSLATE("Show annotation"), new BMessage());
    mpEnableAnnotation->SetEnabled(false);

    BStringView* label2 = new BStringView("label2", B_TRANSLATE("Start of okuri"));
    BStringView* label3 = new BStringView("label3", B_TRANSLATE("Convert with okuri"));
    BStringView* label5 = new BStringView("label5", B_TRANSLATE("Okuri"));
    BStringView* label6 = new BStringView("label6", B_TRANSLATE("Other"));
    
    mpHandleRecursiveEntryAsOkuri = new BCheckBox("handle",
            B_TRANSLATE("Handle recursive entry as okuri"), new BMessage());
    mpDeleteOkuriWhenQuit = new BCheckBox("delete",
            B_TRANSLATE("Delete okuri when quit"), new BMessage());
    mpBeepOnRegistration = new BCheckBox("beepon",
            B_TRANSLATE("Beep on registration"), new BMessage(500));

    BLayoutBuilder::Group<>(this, B_VERTICAL)
        .SetInsets(5, 5, 5, 5)
        .AddGroup(B_HORIZONTAL)
            .Add(label5)
            .Add(new BSeparatorView(B_HORIZONTAL))
        .End()
        .AddGroup(B_VERTICAL)
            .SetInsets(5, 0, 0, 0)
            .Add(mpHandleRecursiveEntryAsOkuri)
            .Add(mpDeleteOkuriWhenQuit)
        .End()
        .AddGroup(B_HORIZONTAL)
            .Add(label6)
            .Add(new BSeparatorView(B_HORIZONTAL))
        .End()
        .AddGroup(B_VERTICAL)
            .SetInsets(5, 0, 0, 0)
            .Add(mpBeepOnRegistration)
            .Add(mpEnableAnnotation)
            .AddGlue()
        .End()
        .AddGroup(B_HORIZONTAL)
            .Add(labels1)
            .Add(new BSeparatorView(B_HORIZONTAL))
        .End()
        .AddGroup(B_VERTICAL)
            .SetInsets(5, 0, 0, 0)
            .Add(mpEnableSKKServ)
            .AddGroup(B_HORIZONTAL)
                .Add(labels2)
                .Add(mpSKKServPort)
                .AddGlue()
            .End()
            .Add(mpSKKServLocalOnly)
            .AddGlue()
        .End()
        .AddGlue();

    Layout(true);
}


int32 MiscView::GetSKKServPort() const
{
    const char* s = mpSKKServPort->Text();
    return atoi(s);
}


void MiscView::SetSKKServPort(int32 port)
{
    BString s;
    s.SetToFormat("%d", port);
    mpSKKServPort->SetText(s.String());
}

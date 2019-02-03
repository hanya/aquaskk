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

#include "VersionView.h"

#include <LayoutBuilder.h>
#include <SeparatorView.h>
#include <StringView.h>
#include "version.h"


VersionView::VersionView(const char* name)
    : BView(name, B_WILL_DRAW)
{
    mpLabel = new BStringView("version", "AquaSKK " AQUASKK_VERSION " (" AQUASKK_DATE ")");
    mpLabel->SetFontSize(36);
    mpLabel->SetAlignment(B_ALIGN_CENTER);
    mpCopyright1 = new BStringView("copyright1",
                        "Copyright \xc2\xa9 2002-2005, phonohawk");
    mpCopyright2 = new BStringView("copyright2",
                        "Copyright \xc2\xa9 2005-2013, AquaSKK Project");
    mpCopyright3 = new BStringView("copyright3",
                        "Copyright \xc2\xa9 2014-2018, codefirst");
    mpCopyright1->SetFontSize(20);
    mpCopyright2->SetFontSize(20);
    mpCopyright3->SetFontSize(20);

    BLayoutBuilder::Group<>(this, B_VERTICAL)
        .SetInsets(10, 10, 5, 5)
        .Add(mpLabel)
        .Add(new BSeparatorView(B_HORIZONTAL))
        .AddGroup(B_VERTICAL)
            .SetInsets(5, 5, 0, 0)
            .Add(mpCopyright1)
            .Add(mpCopyright2)
            .Add(mpCopyright3)
            .AddGlue()
        .End();

    Layout(true);
}

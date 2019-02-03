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

#ifndef ConversionView_h
#define ConversionView_h

#include <View.h>

class BCheckBox;
class BMenuField;
class BPopUpMenu;
class BSpinner;
class BTextControl;


class ConversionView : public BView
{
public:
    ConversionView(const char* name);
    virtual ~ConversionView() {}

    bool GetCandidateWindowPosition();
    void SetCandidateWindowPosition(bool upward);
    bool GetCandidateWindowDirection();
    void SetCandidateWindowDirection(bool vertical);
    void SetMinComprationLengthText(int32 value);

    BTextControl* mpCandidateWindowLabels;
    BMenuField*   mpPutCandidateWindowUpward;
    BPopUpMenu*   mpCandidatePositionPM;
    BMenuField*   mpCandidateDirection;
    BPopUpMenu*   mpCandidateDirectionPM;
    BCheckBox*    mpDisplayShortestMatchOfKanaConversion;
    BCheckBox*    mpUseNumericConversion;
    BSpinner*     mpMaxCountOfInlineCandidates;
    BCheckBox*    mpInlineBackspaceImpliesCommit;
};


#endif

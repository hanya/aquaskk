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

#include "HaikuModeIndicator.h"

#include "HaikuLooper.h"
#include "InputModeWindow.h"
#include "SKKConstVars.h"


HaikuModeIndicator::HaikuModeIndicator(HaikuLooper* looper)
    : SKKInputModeListener()
    , mpInputModeWindow(NULL)
    , fShowRequested(false)
    , fIndicatorEnabled(false)
    , fMode(HirakanaInputMode)
{
    mpLooper = looper;
    mpInputModeWindow = new InputModeWindow(3000 * 1000);

    Reload();
}


HaikuModeIndicator::~HaikuModeIndicator()
{
    mpInputModeWindow->Quit();
    mpLooper = NULL;
}


void HaikuModeIndicator::SKKWidgetShow()
{
    if (fIndicatorEnabled) {
        fShowRequested = true;
        mpLooper->AskForCursorLocation();
    }
}


void HaikuModeIndicator::SKKWidgetHide()
{
    fShowRequested = false;
    if (!mpInputModeWindow->IsHidden()) {
        mpInputModeWindow->Hide();
    }
}


void HaikuModeIndicator::SelectInputMode(SKKInputMode mode)
{
    fMode = mode;
}


void HaikuModeIndicator::ShowAtCursor(BMessage* msg)
{
    if (fShowRequested) {
        fShowRequested = false;
        mpInputModeWindow->SetMode(fMode);
        if (mpInputModeWindow->IsHidden()) {
            mpInputModeWindow->Show();
        }
    }
}


void HaikuModeIndicator::Reload()
{
    fIndicatorEnabled = SKKUserDefaultKeys::show_input_mode_icon;
}

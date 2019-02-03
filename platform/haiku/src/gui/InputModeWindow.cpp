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

#include "InputModeWindow.h"

#include <Bitmap.h>
#include <FindDirectory.h>
#include <MessageRunner.h>
#include <Path.h>
#include <TranslationUtils.h>
#include <View.h>

#include "MethodMessageId.h"
#include "HaikuConstants.h"

#include <vector>


struct ModeIcon {
    int mode;
    const char* name;
};


const struct ModeIcon INPUT_MODE_ICONS[] = {
    {HirakanaInputMode, "AquaSKK-Hirakana.png"},
    {KatakanaInputMode, "AquaSKK-Katakana.png"},
    {Jisx0201KanaInputMode, "AquaSKK-Jisx0201Kana.png"},
    {AsciiInputMode, "AquaSKK-Ascii.png"},
    {Jisx0208LatinInputMode, "AquaSKK-Jisx0208Latin.png"},
    {0, 0},
};


typedef struct {
    int mode;
    BBitmap* bitmap;
} ModIconBitmap;


class InputModeView : public BView
{
public:
    InputModeView();
    virtual ~InputModeView();

    virtual void Draw(BRect rect);

    void AppendBitmap(int mode, BBitmap* bitmap);
    void SetIcon(int mode);
private:
    BBitmap* fModeBitmap;
    std::vector<ModIconBitmap> fModeBitmaps;
    int      fMode;
    bool     fBitmapChanged;
};


InputModeView::InputModeView()
    : BView(
        BRect(0, 0, 10, 10),
        "view",
        B_FOLLOW_ALL_SIDES,
        B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW)
    , fModeBitmap(NULL)
    , fMode(-1)
    , fBitmapChanged(true)
{
}


void InputModeView::AppendBitmap(int mode, BBitmap* bitmap)
{
    fModeBitmaps.push_back({mode, bitmap});
}


void InputModeView::SetIcon(int mode)
{
    if (fMode != mode) {
        BSize oldSize = fModeBitmap ? fModeBitmap->Bounds().Size() : BSize(0, 0);
        for (size_t i = 0; i < fModeBitmaps.size(); i++) {
            if (fModeBitmaps[i].mode == mode) {
                fMode = fModeBitmaps[i].mode;
                fModeBitmap = fModeBitmaps[i].bitmap;
                fBitmapChanged = true;
            }
        }
        if (fBitmapChanged) {
            BSize newSize = fModeBitmap ? fModeBitmap->Bounds().Size() : BSize(0, 0);
            if (oldSize != newSize) {
                Window()->ResizeTo(newSize.width, newSize.height);
            }
        }
    }
}


InputModeView::~InputModeView()
{
    fModeBitmap = NULL;
    for (int i = 0; i < fModeBitmaps.size(); i++) {
        if (fModeBitmaps[i].bitmap) {
            delete fModeBitmaps[i].bitmap;
        }
    }
}


void InputModeView::Draw(BRect rect)
{
    if (fBitmapChanged) {
        FillRect(Frame(), B_SOLID_LOW);
        fBitmapChanged = false;
    }
    if (fModeBitmap) {
        DrawBitmap(fModeBitmap, BPoint());
    }
}


InputModeWindow::InputModeWindow(bigtime_t delay)
    : BWindow(
        BRect(50, 50, 50, 50),
        "InputModeWindow",
        B_NO_BORDER_WINDOW_LOOK,
        B_FLOATING_ALL_WINDOW_FEEL,
        B_NOT_RESIZABLE | B_NOT_CLOSABLE | B_NOT_ZOOMABLE |
        B_NOT_MINIMIZABLE | B_NOT_MOVABLE |
        B_AVOID_FOCUS |
        B_NOT_ANCHORED_ON_ACTIVATE)
    , fCloseRunner(NULL)
    , fDelay(delay)
{
    fInputModeView = new InputModeView();
    AddChild(fInputModeView);

    Run();
    _Init();
    fInputModeView->SetIcon(HirakanaInputMode);
}


InputModeWindow::~InputModeWindow()
{
}


void InputModeWindow::_Init()
{
    BPath npIconPath, systemIconPath;

    if (find_directory(B_USER_NONPACKAGED_DATA_DIRECTORY, &npIconPath) == B_OK) {
        npIconPath.Append(APP_DIR_NAME);
        npIconPath.Append(ICON_DIR);
    }
    if (find_directory(B_SYSTEM_DATA_DIRECTORY, &systemIconPath) == B_OK) {
        systemIconPath.Append(APP_DIR_NAME);
        systemIconPath.Append(ICON_DIR);
    }

    const ModeIcon* icon = INPUT_MODE_ICONS;
    while (icon->name) {
        BBitmap* bitmap1 = _LoadBitmap(&npIconPath, icon->name);
        if (bitmap1) {
            fInputModeView->AppendBitmap(icon->mode, bitmap1);
        } else {
            BBitmap* bitmap2 = _LoadBitmap(&systemIconPath, icon->name);
            if (bitmap2) {
                fInputModeView->AppendBitmap(icon->mode, bitmap2);
            }
        }
        icon++;
    }
}


BBitmap* InputModeWindow::_LoadBitmap(BPath* dirPath, const char* name)
{
    BPath path(*dirPath);
    path.Append(name);
    return BTranslationUtils::GetBitmap(path.Path());
}


void InputModeWindow::MessageReceived(BMessage* msg)
{
    switch(msg->what)
    {
        case IM_WINDOW_SHOW:
        {
            BPoint point;
            if (msg->FindPoint("location", &point) != B_OK) {
                point = BPoint(100, 100);
            }
            if (IsHidden()) {
                bigtime_t delay;
                if (msg->FindInt64("delay", &delay) != B_OK) {
                    delay = fDelay;
                }
                _ShowAndSetCloseDelay(point, delay);
            }
            break;
        }
        case IM_WINDOW_HIDE:
        {
            if (!IsHidden()) {
                Hide();
                delete fCloseRunner;
                fCloseRunner = NULL;
            }
            break;
        }
        default:
        {
            BWindow::MessageReceived(msg);
            break;
        }
    }
}


void InputModeWindow::SetMode(SKKInputMode mode)
{
    fInputModeView->SetIcon(mode);
}


void InputModeWindow::_ShowAndSetCloseDelay(BPoint point, bigtime_t delay)
{
    MoveTo(point);
    SetWorkspaces(B_CURRENT_WORKSPACE);
    Show();

    BMessage msg(IM_WINDOW_HIDE);
    fCloseRunner = new BMessageRunner(BMessenger(this), &msg, delay, 1);
}

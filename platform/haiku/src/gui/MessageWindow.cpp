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

#include "MessageWindow.h"

#include <Bitmap.h>
#include <File.h>
#include <FindDirectory.h>
#include <IconUtils.h>
#include <MessageRunner.h>
#include <Path.h>
#include <Resources.h>
#include <TranslationUtils.h>
#include <View.h>
#include <Window.h>

#include <string>
#include <vector>

#include "MethodMessageId.h"


const float border = 1.;


class MessageView : public BView
{
public:
    MessageView();
    virtual ~MessageView();

    virtual void Draw(BRect rect);

    void SetMessage(const std::string& msg);
    void SetIcon(BBitmap* icon);
private:
    void _CalculateSize();
    void _DrawClear();

    std::string fMessage;
    BBitmap* fBitmap;
    rgb_color fTextColor;
    rgb_color fBackgroundColor;
    rgb_color fBorderColor;
    float fHalfwidthSpaceWidth;
    float fLineOffset;
    float fLineHeight;
};


MessageView::MessageView()
    : BView(
        BRect(0, 0, 10, 10),
        "view",
        B_FOLLOW_ALL_SIDES,
        B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW)
{
    fTextColor = HighColor();
    fBackgroundColor = LowColor();
    fBorderColor = {0xB0, 0xB0, 0xB0, 1};
}

MessageView::~MessageView()
{
    delete fBitmap;
}


void MessageView::_CalculateSize()
{
    BFont font;
    GetFont(&font);
    font_height fontHeight;
    font.GetHeight(&fontHeight);
    const float ascent = ceilf(fontHeight.ascent);
    const float lineHeight = ceilf(fontHeight.ascent +
                    fontHeight.descent + fontHeight.leading);
    fLineHeight = lineHeight;
    fLineOffset = fontHeight.descent + fontHeight.leading;
    fHalfwidthSpaceWidth = StringWidth(" ");
    float height = ceilf(lineHeight * 1.5);
    float width = 0;

    if (fBitmap && height > 24) {
        height = height + border * 2;
        width = border * 2 + 24 + StringWidth(fMessage.c_str()) +
                    fHalfwidthSpaceWidth * 2;
    } else {
        height = 32 + border * 2;
        width = border * 2 + 24 + StringWidth(fMessage.c_str()) +
                    fHalfwidthSpaceWidth * 2;
    }
    Window()->ResizeTo(width, height);
}


void MessageView::Draw(BRect rect)
{
    _DrawClear();

    const float height = Window()->Bounds().Height();
    BPoint point(border + 24 + fHalfwidthSpaceWidth,
                height - (height - fLineHeight) / 2 - fLineOffset);
    DrawString(fMessage.c_str(), point);

    if (fBitmap) {
        SetDrawingMode(B_OP_ALPHA);
        SetBlendingMode(B_PIXEL_ALPHA, B_ALPHA_OVERLAY);
        DrawBitmap(fBitmap, BPoint(border, border));
    }
}


void MessageView::_DrawClear()
{
    SetHighColor(fTextColor);
    SetLowColor(fBackgroundColor);

    float width = Frame().Width();
    float height = Frame().Height();

    // clear
    BRect areaRect(border, border, width - border * 2, height - border * 2);
    FillRect(areaRect, B_SOLID_LOW);

    // draw border
    SetPenSize(1.0);
    SetHighColor(fBorderColor);
    StrokeRect(Window()->Bounds());

    SetHighColor(fTextColor);
}


void MessageView::SetMessage(const std::string& msg)
{
    fMessage = msg;

    _CalculateSize();
}


void MessageView::SetIcon(BBitmap* icon)
{
    fBitmap = icon;
}


MessageWindow::MessageWindow(bigtime_t delay)
    : BWindow(
        BRect(100, 100, 50, 50),
        "MessageWindow",
        B_NO_BORDER_WINDOW_LOOK,
        B_FLOATING_ALL_WINDOW_FEEL,
        B_NOT_RESIZABLE | B_NOT_CLOSABLE | B_NOT_ZOOMABLE |
        B_NOT_MINIMIZABLE | B_NOT_MOVABLE |
        B_AVOID_FOCUS |
        B_NOT_ANCHORED_ON_ACTIVATE)
    , fCloseRunner(NULL)
    , fDelay(delay)
{
    fMessageView = new MessageView();
    AddChild(fMessageView);

    Run();
    _Init();
}


MessageWindow::~MessageWindow()
{
}


void MessageWindow::_Init()
{
    BBitmap* icon = NULL;
    BPath path;
    if (find_directory(B_BEOS_SERVERS_DIRECTORY, &path) != B_OK) {

    }
    path.Append("app_server");
    BFile file;
    file.SetTo(path.Path(), B_READ_ONLY);

    BResources resources;
    resources.SetTo(&file);

    icon = new BBitmap(BRect(0, 0, 24-1, 24-1), 0, B_RGBA32);

    size_t size = 0;
    const uint8* rawIcon = (const uint8*)resources.LoadResource(
            B_VECTOR_ICON_TYPE, "info", &size);
    if (rawIcon != NULL &&
        BIconUtils::GetVectorIcon(rawIcon, size, icon) == B_OK) {
        fMessageView->SetIcon(icon);
        return;
    }

    rawIcon = (const uint8*)resources.LoadResource(
            B_LARGE_ICON_TYPE, "info", &size);


    if (icon->ColorSpace() != B_CMAP8) {
        BIconUtils::ConvertFromCMAP8(rawIcon, 24, 24, 24, icon);
    }

    fMessageView->SetIcon(icon);

}


void MessageWindow::SetMessage(const std::string& msg)
{
    fMessageView->SetMessage(msg);
}


void MessageWindow::MessageReceived(BMessage* msg)
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


void MessageWindow::_ShowAndSetCloseDelay(BPoint point, bigtime_t delay)
{
    MoveTo(point);
    SetWorkspaces(B_CURRENT_WORKSPACE);
    Show();

    BMessage msg(IM_WINDOW_HIDE);
    fCloseRunner = new BMessageRunner(BMessenger(this), &msg, delay, 1);
}


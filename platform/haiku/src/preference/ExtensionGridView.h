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

#ifndef ExtensionGridView_h
#define ExtensionGridView_h

#include "LocGridView.h"

#include <Catalog.h>
#include <Looper.h>
#include <View.h>


#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Preference"


template<typename T>
class StateColumn : public CheckBoxColumn<T>
{
public:
    StateColumn(BView* view, int32 column);
    virtual ~StateColumn() {};
protected:
    virtual void _FlipState(T* pRow);
};


template<typename T>
StateColumn<T>::StateColumn(BView* view, int32 column)
    : CheckBoxColumn<T>(view, column)
{
}


template<typename T>
void StateColumn<T>::_FlipState(T* pRow)
{
    if (pRow) {
        pRow->FlipState();
    }
}


template<typename T>
class DescriptionColumn : public GridStringColumn<T>
{
public:
    DescriptionColumn(BView* view, int32 column);
    virtual ~DescriptionColumn() {}
protected:
    virtual const char* _GetLabel(T* pRow);
};


template<typename T>
DescriptionColumn<T>::DescriptionColumn(BView* view, int32 column)
    : GridStringColumn<T>(view, column)
{
}


template<typename T>
const char* DescriptionColumn<T>::_GetLabel(T* pRow)
{
    if (pRow && this->mnColumn == 1) {
        return pRow->Description().c_str();
    }
    return "";
}


struct RuleDivision {
    enum {
        SYSTEM = 0,
        LOCAL = 1,
        INVALID_DIVISION,
    };
};


template<typename T>
class LocationColumn : public GridStringColumn<T>
{
public:
    LocationColumn (BView* view, int32 column);
    virtual ~LocationColumn() {}
protected:
    virtual const char* _GetLabel(T* pRow);
};


template<typename T>
LocationColumn <T>::LocationColumn(BView* view, int32 column)
    : GridStringColumn<T>(view, column)
{
}


template<typename T>
const char* LocationColumn<T>::_GetLabel(T* pRow)
{
    if (pRow && this->mnColumn == 2) {
        int32 location = pRow->Location();
        switch (location)
        {
            case 0:
                return B_TRANSLATE("System");
                break;
            case 1:
                return B_TRANSLATE("Local");
                break;
            default:
                break;
        }
    }
    return "";
}


class ExtensionRow : public GridRow
{
public:
    ExtensionRow(const char* id, bool active, const char* desc, int32 location)
        : GridRow()
        , maId(id)
        , mbActive(active)
        , maDescription(desc)
        , mnLocation(location)
    {
    }
    virtual ~ExtensionRow() {}

    const std::string& Id() { return maId; }
    bool IsActive() const { return mbActive; }
    void SetActive(bool state) { mbActive = state; }
    void FlipState() { mbActive = !mbActive; }
    const std::string& Description() const { return maDescription; }
    int32 Location() const { return mnLocation; }
private:
    std::string maId;
    bool mbActive;
    std::string maDescription;
    int32 mnLocation;
};


template<typename T>
class ExtensionGridView : public GridView<T>
{
public:
    ExtensionGridView(BLooper* looper);
    virtual ~ExtensionGridView() {}

    virtual void Draw(BRect rect);
};


template<typename T>
ExtensionGridView<T>::ExtensionGridView(BLooper* looper)
    : GridView<T>(SINGLE, looper, NULL)
{
    GridTitleView* pTitleView = new GridTitleView(false, this);
    pTitleView->AddColumn(B_TRANSLATE("Active"), 0);
    pTitleView->AddColumn(B_TRANSLATE("Description"));
    pTitleView->AddColumn(B_TRANSLATE("Division"), 0);
    this->SetTitleView(pTitleView);
    const float margin = this->StringWidth("  ");
    pTitleView->SetColumnWidth(0,
        this->StringWidth(pTitleView->GetColumn(0)) + margin * 4);
    pTitleView->SetColumnWidth(2,
        this->StringWidth(B_TRANSLATE("System")) + margin * 4);
}


template<typename T>
void ExtensionGridView<T>::Draw(BRect rect)
{
    const float nRowHeight = this->mnRowHeight;
    const float nTitleHeight = this->mpTitleView->Height();

    int32 nLastRow = std::min(this->mnFirstRow + this->mnRows + 1,
                            static_cast<int32>(this->mpRows.size()));

    this->SetHighUIColor(B_LIST_ITEM_TEXT_COLOR);

    BPoint pos(0, nTitleHeight + ceil(nRowHeight * 0.75));
    BRect r0(0, nTitleHeight, this->mpTitleView->ColumnWidth(0), nTitleHeight + nRowHeight);
    BRect r1(r0.right, nTitleHeight, r0.right + this->mpTitleView->ColumnWidth(1), r0.bottom);
    BRect r2(r1.right, nTitleHeight, r1.right + this->mpTitleView->ColumnWidth(2), r0.bottom);
    T* pCursorRow = this->_IsCursorValid() ? this->mpRows[this->mnCursorRow] : NULL;
    for (size_t i = this->mnFirstRow; i < nLastRow; ++i) {
        T* pRow = this->mpRows[i];
        //if (pRow->IsSelected()) {

        //}
        const bool bSelectedRow = pRow == pCursorRow;

        this->mpColumns[0]->DrawAt(this, r0, bSelectedRow && this->mnCursorColumn == 0, pRow);
        r0.top = r0.bottom;
        r0.bottom += nRowHeight;
        pos.x += this->mpTitleView->ColumnWidth(0);

        this->mpColumns[1]->DrawAt(this, r1, bSelectedRow && this->mnCursorColumn == 1, pRow);
        r1.top = r0.top;
        r1.bottom = r0.bottom;
        pos.x += this->mpTitleView->ColumnWidth(1);

        this->mpColumns[2]->DrawAt(this, r2, bSelectedRow && this->mnCursorColumn == 2, pRow);
        r2.top = r0.top;
        r2.bottom = r0.bottom;
        pos.x += this->mpTitleView->ColumnWidth(2);

        pos.x = 0;
        pos.y += nRowHeight;
        //if (pRow->IsSelected()) {

        //}
    }
    this->_DrawGrid();
    this->_DrawCursor();
}


#endif

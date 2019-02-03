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

#ifndef DictionaryGridView_h
#define DictionaryGridView_h

#include "LocGridView.h"

#include <Catalog.h>
#include <Looper.h>
#include <View.h>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Preference"


class DictRow : public GridRow
{
public:
    DictRow(bool active, int32 type, const char* location, const char* etag)
        : GridRow()
        , mbActive(active)
        , mnType(type)
        , maLocation(location)
        , maETag(etag)
    {
    }
    virtual ~DictRow() {}

    bool IsActive() const { return mbActive; }
    void SetActive(bool state) { mbActive = state; }
    void FlipState() { mbActive = !mbActive; }
    int32 Type() const { return mnType; }
    void SetType(int32 type) { mnType = type; }
    const std::string& Location() const { return maLocation; }
    void SetLocation(const char* location) { maLocation = location; }
    const std::string& ETag() const { return maETag; }
private:
    bool mbActive;
    int32 mnType;
    std::string maLocation;
    std::string maETag;
};


template<typename T>
class DictKindColumn : public GridMenuFieldColumn<T>
{
public:
    DictKindColumn(BView* view, int32 column, BHandler* handler);
    virtual ~DictKindColumn() {}

    void AddKind(const char* label);
protected:
    virtual void _UpdateRow(T* pRow, BMessage* msg);
    virtual int32 _LabelIndex(T* pRow);
    virtual int32 _CurrentIndex(T* pRow);
};


template<typename T>
DictKindColumn<T>::DictKindColumn(BView* view, int32 column, BHandler* handler)
    : GridMenuFieldColumn<T>(view, column, handler)
{
}


template<typename T>
void DictKindColumn<T>::AddKind(const char* label)
{
    this->maLabels.push_back(label);
    this->mpPopUpMenu->AddItem(
        new BMenuItem(label, new BMessage(this->CountLabels() -1)));
}


template<typename T>
void DictKindColumn<T>::_UpdateRow(T* pRow, BMessage* msg)
{
    if (pRow) {
        pRow->SetType(msg->what);
    }
}

template<typename T>
int32 DictKindColumn<T>::_LabelIndex(T* pRow)
{
    if (pRow) {
        return pRow->Type();
    }
    return -1;
}

template<typename T>
int32 DictKindColumn<T>::_CurrentIndex(T* pRow)
{
    if (pRow) {
        return pRow->Type();
    }
    return -1;
}


template<typename T>
class DictLocationColumn : public GridTextColumn<T>
{
public:
    DictLocationColumn(BView* view, int32 column, BHandler* handler, ListInputView* p);
    virtual ~DictLocationColumn() {};
protected:
    virtual const char* _GetLabel(T* pRow);
    virtual void _SetText(const char* s);
};


template<typename T>
DictLocationColumn<T>::DictLocationColumn(
        BView* view, int32 column, BHandler* handler, ListInputView* p)
    : GridTextColumn<T>(view, column, handler, p)
{
}


template<typename T>
const char* DictLocationColumn<T>::_GetLabel(T* pRow)
{
    if (pRow) {
        return pRow->Location().c_str();
    }
    return NULL;
}


template<typename T>
void DictLocationColumn<T>::_SetText(const char* s)
{
    if (this->mpRow) {
        this->mpRow->SetLocation(s);
    }
}

#define GRID_SELECTION_CHANGED 'Gdsc'

template<typename T>
class DictGridView : public GridView<T>
{
public:
    DictGridView(BLooper* looper);
    virtual ~DictGridView() {}

    virtual void Draw(BRect rect);
protected:
    virtual void _SelectionChanged();
};

template<typename T>
DictGridView<T>::DictGridView(BLooper* looper)
    : GridView<T>(SINGLE, looper, NULL)
{
    GridTitleView* pTitleView = new GridTitleView(false, this);
    pTitleView->AddColumn(B_TRANSLATE("Active"), 0);
    pTitleView->AddColumn(B_TRANSLATE("Type"), 0);
    pTitleView->AddColumn(B_TRANSLATE("Location"));
    this->SetTitleView(pTitleView);
    const float margin = this->StringWidth("  ");
    pTitleView->SetColumnWidth(0,
        this->StringWidth(pTitleView->GetColumn(0)) + margin * 4);
}

template<typename T>
void DictGridView<T>::Draw(BRect rect)
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


template<typename T>
void DictGridView<T>::_SelectionChanged()
{
    if (this->mpLooper) {
        BMessage msg(GRID_SELECTION_CHANGED);
        this->mpLooper->PostMessage(&msg);
    }
}


#endif

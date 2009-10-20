/***************************************************************************
        postionviewwidget - inherited class for doc position views.
                             -------------------
    begin                : 2006-02-20
    copyright            : (C) 2006 by Klaas Freitag
    email                : freitag@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef POSITIONVIEWWIDGET_H
#define POSITIONVIEWWIDGET_H

#include <q3ptrlist.h>
#include <qmap.h>
#include <Q3PopupMenu>
#include <QPaintEvent>

#include "ui_positionwidget.h"

/**
	@author Klaas Freitag <freitag@kde.org>
*/
class DocPosition;
class KMenu;
class Geld;
class Q3PopupMenu;
class KLocale;

class PositionViewWidget : public QWidget, public Ui_positionWidget
{
    Q_OBJECT
public:
    enum State { Active, New, Deleted, Locked };
    enum Kind  { Normal, Demand, Alternative, Invalid };

    PositionViewWidget( );
    PositionViewWidget( int );

    void setDocPosition( DocPositionBase*, KLocale* );
    virtual ~PositionViewWidget();
    bool modified() { return mModified; }
    int ordNumber() { return mOrdNumber; }
    void setOrdNumber( int  );

    bool deleted() { return mToDelete; }
    DocPositionGuardedPtr position(){ return mPositionPtr; }
    State state() { return mState; }
    Kind  kind()  { return mKind; }
    QString kindString( Kind = Invalid ) const;
    QString stateString( const State& state ) const;
    QString kindLabel( Kind ) const;
    void cleanKindString();
    Geld currentPrice();
    bool priceValid();
    void setCurrentPrice( Geld );
    Geld unitPrice();
    void setLocale( KLocale* );
    QStringList tagList() { return mTags; }
    QString extraDiscountTagRestriction();

public slots:
    void slotSetOverallPrice( Geld );
    void slotRefreshPrice();
    void slotModified();
    void slotExecButtonPressed();
    void slotTaggingButtonPressed();
    void slotMenuAboutToHide();
    void slotMenuAboutToShow();
    void slotSetState( State );
    void slotSetEnabled( bool );
    void slotEnableKindMenu( bool );

protected slots:
    void slotLockPosition();
    void slotUnlockPosition();
    void slotSetPositionNormal();
    void slotSetPositionAlternative();
    void slotSetPositionDemand();
    void slotUpdateTagToolTip();
    void paintEvent ( QPaintEvent* );
signals:
    void positionModified();
    void deletePosition();
    void moveUp();
    void moveDown();
    void lockPosition();
    void unlockPosition();
    void priceChanged( const Geld& );
    void positionStateNormal();
    void positionStateAlternative();
    void positionStateDemand();

private:
    bool mModified;
    bool m_skipModifiedSignal;
    bool mToDelete;
    int  mOrdNumber;

    DocPositionGuardedPtr mPositionPtr;
    KMenu *mExecPopup;
    Q3PopupMenu *mStateSubmenu;
    QStringList mTags;
    int  mDeleteId;
    int  mLockId;
    int  mUnlockId;
    Geld mPositionPrice;  // only used for Discount items to store the result
    State mState;
    Kind  mKind;
    bool mPositionPriceValid;
    KLocale *mLocale;
};

class PositionViewWidgetList : public Q3PtrList<PositionViewWidget>
{
  public:
    PositionViewWidgetList();
    PositionViewWidget* widgetFromPosition( DocPositionGuardedPtr );

    Geld nettoPrice();
};


typedef Q3PtrListIterator<PositionViewWidget> PositionViewWidgetListIterator;

#endif

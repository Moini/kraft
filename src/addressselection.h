/***************************************************************************
                 addressselection  - widget to select Addresses
                             -------------------
    begin                : 2006-09-03
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

#ifndef ADDRESSSELECTION_H
#define ADDRESSSELECTION_H

#include <q3vbox.h>
#include <qmap.h>
#include <qstring.h>
#include <QTreeWidget>

#include <kabc/addressee.h>
#include <kabc/addressbook.h>

class QComboBox;

using namespace KABC;

class AddressSelection : public QTreeWidget
{
  Q_OBJECT

public:
  AddressSelection( QWidget *parent = 0 );

  ~AddressSelection() { };
  void setupAddressList( );
  Addressee currentAddressee( QTreeWidgetItem* item = 0 );

signals:
  void addressSelected( const Addressee& );

protected slots:
  void slotAddressBookChanged( AddressBook* );
  void slotSelectionChanged();

private:
  QMap<QTreeWidgetItem*, QString> mAddressIds;
  AddressBook *mStdAddressbook;
};

#endif

/***************************************************************************
                          templkatalogview.h
                             -------------------
    begin                : 2005-07-09
    copyright            : (C) 2005 by Klaas Freitag
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

#ifndef TEMPLKATALOGVIEW_H
#define TEMPLKATALOGVIEW_H

// include files for Qt

// include files for KDE
#include <kapplication.h>
#include <kmainwindow.h>
#include <kaction.h>
#include <kurl.h>

#include "katalogview.h"
#include "templkataloglistview.h"

class TemplKatalog;
class KatalogListView;
class FloskelTemplate;
class FlosTemplDialog;
class FilterHeader;
class QBoxLayout;
/**
  * The base class for Kraft katalog view.
  * @see KMainWindow
  * @see KApplication
  * @see KConfig
  *
  * @author Klaas Freitag <freitag@kde.org>
  * @version $Id$
  */

class TemplKatalogView: public KatalogView
{
  Q_OBJECT

public:
  /** construtor of KraftApp, calls all init functions to create the application.
   */
  TemplKatalogView(QWidget* parent=0, const char* name=0);
  TemplKatalogView(const QString& katToShow, QWidget* parent=0, const char* name=0);
  ~TemplKatalogView();

  // virtual KatalogListView *createListView(QWidget*);

  /** opens a file specified by commandline option
   */
  void createCentralWidget(QBoxLayout*, QWidget*);
  KatalogListView* getListView(){return m_listview;}

protected:
  Katalog* getKatalog( const QString& );
  bool currentItemToDocPosition( DocPosition& );
  CalcPartList currentItemsCalcParts();

public slots:
  /* Editiing of templates -> open edit dialog */
  void slEditTemplate();
  void slNewTemplate();

  /* selected Ok in the template editor */
  void slEditOk(FloskelTemplate*);
  void slEditRejected();

private:
  // opens the edit dialog.
  void openDialog( QTreeWidgetItem*, FloskelTemplate *, bool );
  // editing dialog for templates
  FlosTemplDialog  *m_flosDialog;
  TemplKatalogListView *m_listview;
};

#endif // TEMPLKATALOGVIEW_H

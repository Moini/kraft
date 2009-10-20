/***************************************************************************
             katalogman  -
                             -------------------
    begin                : 2004-12-09
    copyright            : (C) 2004 by Klaas Freitag
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

// include files for Qt
#include <QStringList>
#include <QSqlQuery>

// include files for KDE
#include <klocale.h>
#include <kdebug.h>
#include <k3staticdeleter.h>

#include "kraftdb.h"
#include "katalogman.h"
#include "katalog.h"
#include "templkatalog.h"

static K3StaticDeleter<KatalogMan> selfDeleter;

KatalogMan* KatalogMan::mSelf = 0;

KatalogMan *KatalogMan::self()
{
  if ( ! mSelf ) {
    selfDeleter.setObject( mSelf, new KatalogMan() );
  }
  return mSelf;
}

KatalogMan::KatalogMan( )
{

}

KatalogMan::~KatalogMan( )
{
}

QStringList KatalogMan::allKatalogNames()
{

  QStringList list;

  QSqlQuery q( "SELECT name FROM CatalogSet ORDER BY sortKey, name" );

  while( q.next() ) {
    list << q.value( 0 ).toString();
  }

  return list;
}

QString KatalogMan::catalogTypeString( const QString& catName )
{
  QString res;
  if ( !catName.isEmpty() ) {
    QSqlQuery q;
    q.prepare( "SELECT catalogType FROM CatalogSet where name=:name" );
    q.bindValue( ":name",  catName );

    if ( q.exec() && q.next() ) {
      res = q.value( 0 ).toString();
    }
  }
  return res;
}

void KatalogMan::registerKatalog( Katalog *k )
{
    Katalog* kat = m_katalogDict[k->getName()];

    if( kat ) {
        kWarning() << "Katalog with same name already here -> deleting!" << endl;
        delete kat;
    } else {
        // not found, try to open it
        kDebug() << "Katalog " << k->getName() << " registered and loading..." << endl;
        m_katalogDict.insert( k->getName(), k );
        k->load ();
    }
}

Katalog *KatalogMan::getKatalog(const QString& name)
{
    Katalog* kat = m_katalogDict[name];

    if( !kat ) {
        kDebug() << "No katalog " << name << " found" << endl;
    } else {
        kDebug() << "Returning existing katalog " << name << endl;
    }
    return kat;
}

// this is called after an template has been changed in the database.
void KatalogMan::notifyKatalogChange( Katalog* k, dbID )
{
  // FIXME: More efficient catalog reloading.
  if ( k ) {
    const QString name = k->getName();
    k->reload( dbID() );

    QList<KatalogListView*> views = mKatalogListViews[name];

    KatalogListView *view;
    QListIterator<KatalogListView*> i( views );
    while ( i.hasNext() ) {
      view = i.next();
      view->slotRedraw();
    }
  }
}

void KatalogMan::registerKatalogListView( const QString& name, KatalogListView *view )
{
  QList<KatalogListView*> views = mKatalogListViews[name];

  if ( ! views.contains( view ) ) {
    views.append( view );
    mKatalogListViews[name] = views;
  }
}

/*
 * currently, there is only one catalog of type Template by design, see
 * for example in templatesaverdb.cpp or the database design where only
 * one template catalog is in use.
 */

Katalog* KatalogMan::defaultTemplateCatalog()
{
  QHashIterator<QString, Katalog*> it( m_katalogDict ); // See QDictIterator
  while ( it.hasNext() ) {
    it.next();
    Katalog *k = it.value();
    if ( k->type() == TemplateCatalog ) {
      kDebug() << "Found default template catalog: " << k->getName() << endl;
      return k;
    }
  }
  return 0;
}

/* END */


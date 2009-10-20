/***************************************************************************
                          kraftdb.h  -
                             -------------------
    begin                : Die Feb 3 2004
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

#ifndef KRAFTDB_H
#define KRAFTDB_H

#include <QSqlError>
#include <QSqlDatabase>

#include <QMap>
#include <QObject>

#include "kraftcat_export.h"

class dbID;
class QSqlError;

/**
  *@author Klaas Freitag
  */

class KRAFTCAT_EXPORT KraftDB : public QObject
{

  Q_OBJECT

public:
  ~KraftDB();
  /** Read property of QSqlDatabase* m_db. */
  static KraftDB *self();

  dbID getLastInsertID();

  void checkInit();
  bool checkSchemaVersion( QWidget* );
  QSqlDatabase *getDB(){ return &m_db; }
  QString qtDriver();

  typedef QMap<QString, QString> StringMap;
  QStringList wordList( const QString&, StringMap replaceMap = StringMap() );
  void writeWordList( const QString&, const QStringList& );

  QString databaseName() const;
  QString defaultDatabaseName() const;

  QSqlError lastError();

  int checkConnect( const QString&, const QString&,
                    const QString&, const QString& );
  bool isOk() {
    return mSuccess;
  }

  QString mysqlEuroEncode( const QString& ) const;
  QString mysqlEuroDecode( const QString& ) const;

  int currentSchemaVersion();
  QString replaceTagsInWord( const QString& w, StringMap replaceMap ) const;
signals:
  void statusMessage( const QString& );

private: // Private attributes
  KraftDB();
  int processSqlFile( const QString&, int& );

  bool createDatabase( QWidget* );

  /** The default database */
  QSqlDatabase m_db;

  static KraftDB *mSelf;
  bool mSuccess;
  const QString EuroTag;
};

#endif

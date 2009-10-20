/***************************************************************************
             templatesaverbase  -
                             -------------------
    begin                : 2005-20-01
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

// include files for Qt


#include <q3sqlcursor.h>
#include <qsqlrecord.h>
#include <qsqlindex.h>
//Added by qt3to4:
#include <QSqlQuery>
#include <QSqlError>

// include files for KDE
#include <klocale.h>
#include <kdebug.h>

#include "documentsaverdb.h"
#include "kraftdoc.h"
#include "kraftdb.h"
#include "unitmanager.h"
#include "dbids.h"
#include "kraftsettings.h"
#include "doctype.h"

/* Table document:
 * +----------------+--------------+------+-----+-------------------+----------------+
 * | Field          | Type         | Null | Key | Default           | Extra          |
 * +----------------+--------------+------+-----+-------------------+----------------+
 * | docID          | int(11)      | NO   | PRI | NULL              | auto_increment |
 * | ident          | varchar(32)  | YES  | MUL | NULL              |                |
 * | docType        | varchar(255) | YES  |     | NULL              |                |
 * | docDescription | text         | YES  |     | NULL              |                |
 * | clientID       | varchar(32)  | YES  | MUL | NULL              |                |
 * | clientAddress  | text         | YES  |     | NULL              |                |
 * | salut          | varchar(255) | YES  |     | NULL              |                |
 * | goodbye        | varchar(128) | YES  |     | NULL              |                |
 * | lastModified   | timestamp    | NO   |     | CURRENT_TIMESTAMP |                |
 * | date           | date         | YES  |     | NULL              |                |
 * | pretext        | text         | YES  |     | NULL              |                |
 * | posttext       | text         | YES  |     | NULL              |                |
 * | country        | varchar(32)  | YES  |     | NULL              |                |
 * | language       | varchar(32)  | YES  |     | NULL              |                |
 * +----------------+--------------+------+-----+-------------------+----------------+
 * 14 rows in set (0.00 sec)
 *
 */

DocumentSaverDB::DocumentSaverDB( ) : DocumentSaverBase(),
                                      PosTypePosition( QString::fromLatin1( "Position" ) ),
                                      PosTypeExtraDiscount( QString::fromLatin1( "ExtraDiscount" ) ),
                                      PosTypeHeader( QString::fromLatin1( "Header" ) )
{

}

bool DocumentSaverDB::saveDocument(KraftDoc *doc )
{
    bool result = false;
    if( ! doc ) return result;

    Q3SqlCursor cur("document");
    cur.setMode( Q3SqlCursor::Writable );
    QSqlRecord *record = 0;

    kDebug() << "############### Document Save ################" << endl;

    if( doc->isNew() ) {
        record = cur.primeInsert();
    } else {
      cur.select( "docID=" + doc->docID().toString() );
      if ( cur.next() ) {
        record = cur.primeUpdate();
      } else {
        kError() << "Could not select document record" << endl;
        return result;
      }
       // The document was already saved.
    }

    fillDocumentBuffer( record, doc );

    if( doc->isNew() ) {
      kDebug() << "Doc is new, inserting" << endl;
      cur.insert();

      dbID id = KraftDB::self()->getLastInsertID();
      doc->setDocID( id );

      // get the uniq id and write it into the db
      DocType dt( doc->docType() );
      QString ident = dt.generateDocumentIdent( doc );
      doc->setIdent( ident );
      Q3SqlCursor cur2( "document" );
      cur2.select( QString( "docID=" + id.toString() ) );
      if ( cur2.next() ) {
        QSqlRecord *uprecord = cur2.primeUpdate();
        uprecord->setValue( "ident", ident );
        cur2.update();
      }

    } else {
      kDebug() << "Doc is not new, updating #" << doc->docID().intID() << endl;

      record->setValue( "docID", doc->docID().toString() );
      cur.update();
    }

    saveDocumentPositions( doc );

    kDebug() << "Saved document no " << doc->docID().toString() << endl;

    return result;
}

#if 0
/*
 * this method requires a database id because that is the only garanteed
 * unique part.
 */
QString DocumentSaverDB::generateDocumentIdent( dbID id, KraftDoc *doc ) const
{
  /*
   * The pattern may contain the following tags:
   * %y - the year of the documents date.
   * %w - the week number of the documents date
   * %d - the day number of the documents date
   * %m - the month number of the documents date
   * %c - the customer id from kaddressbook
   * %i - the uniq identifier from db.
   * %type - the localised doc type (offer, invoice etc.)
   */
  QString pattern = KraftSettings::self()->self()->docIdent();

  QDate d = doc->date();
  KraftDB::StringMap m;
  int dummy;

  m[ "%y" ] = QString::number( d.year() );
  m[ "%w" ] = QString::number( d.weekNumber( &dummy ) );
  m[ "%d" ] = QString::number( d.day()  );
  m[ "%m" ] = QString::number( d.month() );
  m[ "%i" ] = id.toString();
  m[ "%c" ] = doc->addressUid();
  m[ "%type" ] = doc->docType();

  QString re = KraftDB::self()->replaceTagsInWord( pattern, m );
  kDebug() << "Generated document ident: " << re << endl;

  return re;
}
#endif
void DocumentSaverDB::saveDocumentPositions( KraftDoc *doc )
{
  DocPositionList posList = doc->positions();

  // invert all pos numbers to avoid a unique violation
  // FIXME: We need non-numeric ids
  QSqlQuery upq;
  QString queryStr = "UPDATE docposition SET ordNumber = -1 * ordNumber WHERE docID=";
  queryStr +=  doc->docID().toString();
  queryStr += " AND ordNumber > 0";
  upq.prepare( queryStr );
  upq.exec();

  int ordNumber = 1;

  DocPositionListIterator it( posList );
  while( it.hasNext() ) {
    DocPositionBase *dpb = it.next();

    if( dpb->type() == DocPositionBase::Position ||
        dpb->type() == DocPositionBase::ExtraDiscount ) {
      DocPosition *dp = static_cast<DocPosition*>(dpb);
      QSqlRecord *record = 0;
      Q3SqlCursor cur( "docposition" );
      bool doInsert = true;

      int posDbID = dp->dbId().toInt();
      kDebug() << "Saving Position DB-Id: " << posDbID << endl;
      if( posDbID > -1 ) {
        const QString selStr = QString("docID=%1 AND positionID=%2").arg( doc->docID().toInt() ).arg( posDbID );
        // kDebug() << "Selecting with " << selStr << endl;
        cur.select( selStr );
        if ( cur.next() ) {
          if( ! dp->toDelete() )
            record = cur.primeUpdate();
          doInsert = false;
        } else {
          kError() << "ERR: Could not select document position record" << endl;
          return;
        }
      } else {
        // The record is new
        record = cur.primeInsert();
      }

      if( dp->toDelete() ) {
        kDebug() << "This one is to delete, do it!" << endl;
        // FIXME: Delete attributes for this position
        if( doInsert ) {
          kWarning() << "Attempt to delete a toInsert-Item, obscure" << endl;
        }
        // delete all existing attributes
        dp->attributes().dbDeleteAll( dp->dbId() );

        // the position is already existing, delete it
        cur.primeDelete();
        cur.del();

        continue;
      }

      if( record ) {
        // kDebug() << "Updating position " << dp->position() << " is " << dp->text() << endl;
        QString typeStr = PosTypePosition;
        double price = 0;
        if ( dp->type() == DocPositionBase::Header ) {
          typeStr = PosTypeHeader;
          price = 0;
        } else if ( dp->type() == DocPositionBase::ExtraDiscount ) {
          typeStr = PosTypeExtraDiscount;
          price = dp->unitPrice().toDouble();
        } else if ( dp->type() == DocPositionBase::Position ) {
          price= dp->unitPrice().toDouble();
        }

        record->setValue( "docID",     doc->docID().toInt() );
        record->setValue( "ordNumber", ordNumber );
        record->setValue( "text",      dp->text() );
        record->setValue( "postype",   typeStr );
        record->setValue( "amount",    dp->amount() );
        record->setValue( "unit",      dp->unit().id() );
        record->setValue( "price",     price );

        ordNumber++; // FIXME

        if( doInsert ) {
          kDebug() << "Inserting!" << endl;
          cur.insert();
          dp->setDbId( KraftDB::self()->getLastInsertID().toInt() );
        } else {
          kDebug() << "Updating!" << endl;
          cur.update();
        }
      } else {
        kDebug() << "ERR: No record object found!" << endl;
      }

      dp->attributes().save( dp->dbId() );

      QSqlError err = cur.lastError();
      if( err.type() != QSqlError::NoError ) {
        kDebug() << "SQL-ERR: " << err.text() << " in " << cur.name() << endl;
      }

      if ( err.type() == QSqlError::NoError ) {
      }
    }
  }
}

void DocumentSaverDB::fillDocumentBuffer( QSqlRecord *buf, KraftDoc *doc )
{
    if( buf && doc ) {
      kDebug() << "Adressstring: " << doc->address() << endl;
      buf->setValue( "ident",    doc->ident() );
      buf->setValue( "docType",  doc->docType() );
      buf->setValue( "docDescription", KraftDB::self()->mysqlEuroEncode( doc->whiteboard() ) );
      buf->setValue( "clientID", doc->addressUid() );
      buf->setValue( "clientAddress", doc->address() );
      buf->setValue( "salut",    doc->salut() );
      buf->setValue( "goodbye",  doc->goodbye() );
      buf->setValue( "date",     doc->date() );
      buf->setValue( "lastModified", "NOW()" );
      buf->setValue( "pretext",  KraftDB::self()->mysqlEuroEncode( doc->preText() ) );
      buf->setValue( "posttext", KraftDB::self()->mysqlEuroEncode( doc->postText() ) );
      buf->setValue( "country",  doc->country() );
      buf->setValue( "language", doc->language() );
    }
}

void DocumentSaverDB::load( const QString& id, KraftDoc *doc )
{
    Q3SqlCursor cur("document");
    kDebug() << "Loading document id " << id << endl;

    cur.select( "docID=" + id );

    if( cur.next())
    {
        kDebug() << "loading document with id " << id << endl;
        dbID dbid;
        dbid = id;
        doc->setDocID( dbid );

        doc->setIdent(      cur.value( "ident"    ).toString() );
        doc->setDocType(    cur.value( "docType"  ).toString() );
        doc->setAddressUid( cur.value( "clientID" ).toString() );
        doc->setAddress(    cur.value( "clientAddress" ).toString() );
        doc->setSalut(      cur.value( "salut"    ).toString() );
        doc->setGoodbye(    cur.value( "goodbye"  ).toString() );
        doc->setDate (      cur.value( "date"     ).toDate() );
        doc->setLastModified( cur.value( "lastModified" ).toDate() );
        doc->setCountryLanguage( cur.value( "language" ).toString(),
                                 cur.value( "country" ).toString());

        doc->setPreText(    KraftDB::self()->mysqlEuroDecode( cur.value( "pretext"  ).toString() ) );
        doc->setPostText(   KraftDB::self()->mysqlEuroDecode( cur.value( "posttext" ).toString() ) );
        doc->setWhiteboard( KraftDB::self()->mysqlEuroDecode( cur.value( "docDescription" ).toString() ) );
    }

    loadPositions( id, doc );
}
/* docposition:
  +------------+--------------+------+-----+---------+----------------+
  | Field      | Type         | Null | Key | Default | Extra          |
  +------------+--------------+------+-----+---------+----------------+
  | positionID | int(11)      |      | PRI | NULL    | auto_increment |
  | docID      | int(11)      |      | MUL | 0       |                |
  | ordNumber  | int(11)      |      |     | 0       |                |
  | text       | mediumtext   | YES  |     | NULL    |                |
  | amount     | decimal(6,2) | YES  |     | NULL    |                |
  | unit       | varchar(64)  | YES  |     | NULL    |                |
  | price      | decimal(6,2) | YES  |     | NULL    |                |
  +------------+--------------+------+-----+---------+----------------+
*/
void DocumentSaverDB::loadPositions( const QString& id, KraftDoc *doc )
{
    Q3SqlCursor cur("docposition");
    QSqlIndex posIndex = cur.index( "ordNumber" );
    cur.select( "docID=" + id, posIndex );

    kDebug() << "* loading document positions for document id " << id << endl;
    while( cur.next() ) {
        kDebug() << " loading position id " << cur.value( "positionID" ).toInt() << endl;

        DocPositionBase::PositionType type = DocPositionBase::Position;
        QString typeStr = cur.value( "postype" ).toString();
        if ( typeStr == PosTypeExtraDiscount ) {
          type = DocPositionBase::ExtraDiscount;
        } else if ( typeStr == PosTypePosition ) {
          // nice, default position type.
          type = DocPositionBase::Position;
        } else if ( typeStr == PosTypeHeader ) {
          type = DocPositionBase::Header;}
        else if ( ! typeStr.isEmpty() ) {
          kDebug() << "ERROR: Strange type string loaded from db: " << typeStr << endl;
        }

        DocPosition *dp = doc->createPosition( type );
        dp->setDbId( cur.value("positionID").toInt() );
        dp->setText( cur.value("text").toString() );

        // Note: empty fields are treated as Positions which is intended because
        // the type col was added later and thus might be empty for older entries

        dp->setAmount( cur.value("amount").toDouble() );

        dp->setUnit( UnitManager::self()->getUnit( cur.value("unit").toInt() ) );
        dp->setUnitPrice( cur.value("price").toDouble() );
        dp->loadAttributes();
    }
}

DocumentSaverDB::~DocumentSaverDB( )
{

}

/* END */


#include "documentsaverdb.moc"

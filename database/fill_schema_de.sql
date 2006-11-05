DELETE FROM preisArten;
INSERT INTO preisArten VALUES (0, 'offen');
INSERT INTO preisArten VALUES (1, 'selbsterstellt');
INSERT INTO preisArten VALUES (2, 'kalkuliert');

DELETE FROM CatalogSet;
INSERT INTO CatalogSet (name, description, catalogType, sortKey) VALUES 
 ( "Mustertexte GALA-Bau", "Kalkulierte Musterposten f�r den Garten- und Landschaftsbau", "TemplCatalog", 1 );
SET @newCat := LAST_INSERT_ID();

DELETE FROM CatalogChapters;
INSERT INTO CatalogChapters (chapter, sortKey) VALUES ('Arbeit', 1 );
INSERT INTO CatalogChapters (chapter, sortKey) VALUES ('Maschine', 2 );
INSERT INTO CatalogChapters (chapter, sortKey) VALUES ('Materialeinsatz', 3 );
INSERT INTO CatalogChapters (chapter, sortKey) VALUES ('Service', 4 );
INSERT INTO CatalogChapters (chapter, sortKey) VALUES ('Sonstige', 5 );
INSERT INTO CatalogChapters (chapter, sortKey) VALUES ('Transport', 6 );
UPDATE CatalogChapters SET catalogSetID=@newCat;

INSERT INTO CatalogSet( name, description, catalogType, sortKey) VALUES ("Material",
  "Materialkatalog", "MaterialCatalog", 2 );
SET @newCat := LAST_INSERT_ID();

INSERT INTO CatalogChapters (chapter, sortKey, catalogSetID) VALUES 
  ('Sch�ttg�ter', 3, @newCat);
INSERT INTO CatalogChapters (chapter, sortKey, catalogSetID) VALUES 
  ('Naturstein', 2, @newCat);
INSERT INTO CatalogChapters (chapter, sortKey, catalogSetID) VALUES 
  ('Beton', 1, @newCat);
INSERT INTO CatalogChapters (chapter, sortKey, catalogSetID) VALUES 
  ('Rohre', 4, @newCat);

DELETE FROM units;
INSERT INTO units VALUES (0, 'm', 'Meter', 'm', 'Meter' );
INSERT INTO units VALUES (1, 'qm', 'Quadratmeter', 'qm', 'Quadratmeter' );
INSERT INTO units VALUES (2, 'cbm', 'Kubikmeter', 'cbm', 'Kubikmeter' );
INSERT INTO units VALUES (3, 'Sck.', 'Sack', 'Sck.', 'Saecke' );
INSERT INTO units VALUES (4, 'l', 'Liter', 'l', 'Liter' );
INSERT INTO units VALUES (5, 'kg', 'Kilogramm', 'kg', 'Kilogramm' );
INSERT INTO units VALUES (6, 'Stck.', 'Stueck', 'Stck.', 'Stueck' );
INSERT INTO units VALUES (7, 't', 'Tonne', 't', 'Tonnen' );
INSERT INTO units VALUES (8, 'pausch.', 'pauschal', 'pausch.', 'pauschal' );
INSERT INTO units VALUES (9, 'Std.', 'Stunde', 'Std.', 'Stunden' );

DELETE FROM stdSaetze;
INSERT INTO stdSaetze (name, price, sortKey) VALUES ('Geselle', 34.00, 1 );
INSERT INTO stdSaetze (name, price, sortKey) VALUES ('Meister', 39.00, 2 );
INSERT INTO stdSaetze (name, price, sortKey) VALUES ('Helfer', 30.00, 4 );
INSERT INTO stdSaetze (name, price, sortKey) VALUES ('Auszubildender', 21.00, 3 );
INSERT INTO stdSaetze (name, price, sortKey) VALUES ('Maschinenfuehrer', 33.00, 5 );

DELETE FROM wordLists;
INSERT INTO wordLists VALUES ('greeting', 'mit den besten Gr�ssen,' );
INSERT INTO wordLists VALUES ('greeting', 'liebe Gr�sse,' );
INSERT INTO wordLists VALUES ('greeting', 'Hochachtungsvoll,' );
INSERT INTO wordLists VALUES ('greeting', 'mit freundlichem Gru�,' );

INSERT INTO wordLists VALUES ('salut', 'Sehr geehrter Herr %NAME' );
INSERT INTO wordLists VALUES ('salut', 'Sehr geehrte Frau %NAME' );
INSERT INTO wordLists VALUES ('salut', 'Sehr geehrte Frau %NAME, sehr geehrter Herr %NAME' );
INSERT INTO wordLists VALUES ('salut', 'Lieber %GIVEN_NAME' );
INSERT INTO wordLists VALUES ('salut', 'Liebe %GIVEN_NAME' );

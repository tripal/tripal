--
-- Note by Lacey-anne Sanderson:
-- File contains the genetic_code schema which is part of the sequence module.
--
-- ==========================================
-- Chado sequence module
--
-- =================================================================
-- Dependencies:
--
-- :import cvterm from cv
-- :import pub from pub
-- :import organism from organism
-- :import dbxref from db
-- :import contact from contact
-- =================================================================

CREATE SCHEMA genetic_code;
SET search_path = genetic_code,public,pg_catalog;

CREATE TABLE gencode (
        gencode_id      INTEGER PRIMARY KEY NOT NULL,
        organismstr     VARCHAR(512) NOT NULL
);

CREATE TABLE gencode_codon_aa (
        gencode_id      INTEGER NOT NULL REFERENCES gencode(gencode_id),
        codon           CHAR(3) NOT NULL,
        aa              CHAR(1) NOT NULL,
        CONSTRAINT gencode_codon_unique UNIQUE( gencode_id, codon )
);
CREATE INDEX gencode_codon_aa_i1 ON gencode_codon_aa(gencode_id,codon,aa);

CREATE TABLE gencode_startcodon (
        gencode_id      INTEGER NOT NULL REFERENCES gencode(gencode_id),
        codon           CHAR(3),
        CONSTRAINT gencode_startcodon_unique UNIQUE( gencode_id, codon )
);
SET search_path = public,pg_catalog;

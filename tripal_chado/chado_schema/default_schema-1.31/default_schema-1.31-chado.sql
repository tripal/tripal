
-- ==========================================
-- Chado general module
--
-- ================================================
-- TABLE: tableinfo
-- ================================================

create table tableinfo (
    tableinfo_id bigserial not null,
    primary key (tableinfo_id),
    name varchar(30) not null,
    primary_key_column varchar(30) null,
    is_view int not null default 0,
    view_on_table_id bigint null,
    superclass_table_id bigint null,
    is_updateable int not null default 1,
    modification_date date not null default now(),
    constraint tableinfo_c1 unique (name)
);

COMMENT ON TABLE tableinfo IS NULL;

-- ================================================
-- TABLE: db
-- ================================================

create table db (
    db_id bigserial not null,
    primary key (db_id),
    name varchar(255) not null,
--    contact_id bigint,
--    foreign key (contact_id) references contact (contact_id) on delete cascade INITIALLY DEFERRED,
    description varchar(255) null,
    urlprefix varchar(255) null,
    url varchar(255) null,
    constraint db_c1 unique (name)
);

COMMENT ON TABLE db IS 'A database authority. Typical databases in
bioinformatics are FlyBase, GO, UniProt, NCBI, MGI, etc. The authority
is generally known by this shortened form, which is unique within the
bioinformatics and biomedical realm.  To Do - add support for URIs,
URNs (e.g. LSIDs). We can do this by treating the URL as a URI -
however, some applications may expect this to be resolvable - to be
decided.';

-- ================================================
-- TABLE: dbxref
-- ================================================

create table dbxref (
    dbxref_id bigserial not null,
    primary key (dbxref_id),
    db_id bigint not null,
    foreign key (db_id) references db (db_id) on delete cascade INITIALLY DEFERRED,
    accession varchar(1024) not null,
    version varchar(255) not null default '',
    description text,
    constraint dbxref_c1 unique (db_id,accession,version)
);
create index dbxref_idx1 on dbxref (db_id);
create index dbxref_idx2 on dbxref (accession);
create index dbxref_idx3 on dbxref (version);

COMMENT ON TABLE dbxref IS 'A unique, global, public, stable identifier. Not necessarily an external reference - can reference data items inside the particular chado instance being used. Typically a row in a table can be uniquely identified with a primary identifier (called dbxref_id); a table may also have secondary identifiers (in a linking table <T>_dbxref). A dbxref is generally written as <DB>:<ACCESSION> or as <DB>:<ACCESSION>:<VERSION>.';

COMMENT ON COLUMN dbxref.accession IS 'The local part of the identifier. Guaranteed by the db authority to be unique for that db.';

-- $Id: cv.sql,v 1.37 2007-02-28 15:08:48 briano Exp $
-- ==========================================
-- Chado cv module
--
-- =================================================================
-- Dependencies:
--
-- :import dbxref from db
-- =================================================================

-- ================================================
-- TABLE: cv
-- ================================================
create table cv (
    cv_id bigserial not null,
    primary key (cv_id),
    name varchar(255) not null,
   definition text,
   constraint cv_c1 unique (name)
);

COMMENT ON TABLE cv IS 'A controlled vocabulary or ontology. A cv is
composed of cvterms (AKA terms, classes, types, universals - relations
and properties are also stored in cvterm) and the relationships
between them.';

COMMENT ON COLUMN cv.name IS 'The name of the ontology. This
corresponds to the obo-format -namespace-. cv names uniquely identify
the cv. In OBO file format, the cv.name is known as the namespace.';

COMMENT ON COLUMN cv.definition IS 'A text description of the criteria for
membership of this ontology.';

-- ================================================
-- TABLE: cvterm
-- ================================================
create table cvterm (
    cvterm_id bigserial not null,
    primary key (cvterm_id),
    cv_id bigint not null,
    foreign key (cv_id) references cv (cv_id) on delete cascade INITIALLY DEFERRED,
    name varchar(1024) not null,
    definition text,
    dbxref_id bigint not null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
    is_obsolete int not null default 0,
    is_relationshiptype int not null default 0,
    constraint cvterm_c1 unique (name,cv_id,is_obsolete),
    constraint cvterm_c2 unique (dbxref_id)
);
create index cvterm_idx1 on cvterm (cv_id);
create index cvterm_idx2 on cvterm (name);
create index cvterm_idx3 on cvterm (dbxref_id);

COMMENT ON TABLE cvterm IS 'A term, class, universal or type within an
ontology or controlled vocabulary.  This table is also used for
relations and properties. cvterms constitute nodes in the graph
defined by the collection of cvterms and cvterm_relationships.';

COMMENT ON COLUMN cvterm.cv_id IS 'The cv or ontology or namespace to which
this cvterm belongs.';

COMMENT ON COLUMN cvterm.name IS 'A concise human-readable name or
label for the cvterm. Uniquely identifies a cvterm within a cv.';

COMMENT ON COLUMN cvterm.definition IS 'A human-readable text
definition.';

COMMENT ON COLUMN cvterm.dbxref_id IS 'Primary identifier dbxref - The
unique global OBO identifier for this cvterm.  Note that a cvterm may
have multiple secondary dbxrefs - see also table: cvterm_dbxref.';

COMMENT ON COLUMN cvterm.is_obsolete IS 'Boolean 0=false,1=true; see
GO documentation for details of obsoletion. Note that two terms with
different primary dbxrefs may exist if one is obsolete.';

COMMENT ON COLUMN cvterm.is_relationshiptype IS 'Boolean
0=false,1=true relations or relationship types (also known as Typedefs
in OBO format, or as properties or slots) form a cv/ontology in
themselves. We use this flag to indicate whether this cvterm is an
actual term/class/universal or a relation. Relations may be drawn from
the OBO Relations ontology, but are not exclusively drawn from there.';

COMMENT ON INDEX cvterm_c1 IS 'A name can mean different things in
different contexts; for example "chromosome" in SO and GO. A name
should be unique within an ontology or cv. A name may exist twice in a
cv, in both obsolete and non-obsolete forms - these will be for
different cvterms with different OBO identifiers; so GO documentation
for more details on obsoletion. Note that occasionally multiple
obsolete terms with the same name will exist in the same cv. If this
is a possibility for the ontology under consideration (e.g. GO) then the
ID should be appended to the name to ensure uniqueness.';

COMMENT ON INDEX cvterm_c2 IS 'The OBO identifier is globally unique.';

-- ================================================
-- TABLE: cvterm_relationship
-- ================================================
create table cvterm_relationship (
    cvterm_relationship_id bigserial not null,
    primary key (cvterm_relationship_id),
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    subject_id bigint not null,
    foreign key (subject_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    constraint cvterm_relationship_c1 unique (subject_id,object_id,type_id)
);
create index cvterm_relationship_idx1 on cvterm_relationship (type_id);
create index cvterm_relationship_idx2 on cvterm_relationship (subject_id);
create index cvterm_relationship_idx3 on cvterm_relationship (object_id);

COMMENT ON TABLE cvterm_relationship IS 'A relationship linking two
cvterms. Each cvterm_relationship constitutes an edge in the graph
defined by the collection of cvterms and cvterm_relationships. The
meaning of the cvterm_relationship depends on the definition of the
cvterm R refered to by type_id. However, in general the definitions
are such that the statement "all SUBJs REL some OBJ" is true. The
cvterm_relationship statement is about the subject, not the
object. For example "insect wing part_of thorax".';

COMMENT ON COLUMN cvterm_relationship.subject_id IS 'The subject of
the subj-predicate-obj sentence. The cvterm_relationship is about the
subject. In a graph, this typically corresponds to the child node.';

COMMENT ON COLUMN cvterm_relationship.object_id IS 'The object of the
subj-predicate-obj sentence. The cvterm_relationship refers to the
object. In a graph, this typically corresponds to the parent node.';

COMMENT ON COLUMN cvterm_relationship.type_id IS 'The nature of the
relationship between subject and object. Note that relations are also
housed in the cvterm table, typically from the OBO relationship
ontology, although other relationship types are allowed.';

-- ================================================
-- TABLE: cvtermpath
-- ================================================
create table cvtermpath (
    cvtermpath_id bigserial not null,
    primary key (cvtermpath_id),
    type_id bigint,
    foreign key (type_id) references cvterm (cvterm_id) on delete set null INITIALLY DEFERRED,
    subject_id bigint not null,
    foreign key (subject_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    cv_id bigint not null,
    foreign key (cv_id) references cv (cv_id) on delete cascade INITIALLY DEFERRED,
    pathdistance int,
    constraint cvtermpath_c1 unique (subject_id,object_id,type_id,pathdistance)
);
create index cvtermpath_idx1 on cvtermpath (type_id);
create index cvtermpath_idx2 on cvtermpath (subject_id);
create index cvtermpath_idx3 on cvtermpath (object_id);
create index cvtermpath_idx4 on cvtermpath (cv_id);

COMMENT ON TABLE cvtermpath IS 'The reflexive transitive closure of
the cvterm_relationship relation.';

COMMENT ON COLUMN cvtermpath.type_id IS 'The relationship type that
this is a closure over. If null, then this is a closure over ALL
relationship types. If non-null, then this references a relationship
cvterm - note that the closure will apply to both this relationship
AND the OBO_REL:is_a (subclass) relationship.';

COMMENT ON COLUMN cvtermpath.cv_id IS 'Closures will mostly be within
one cv. If the closure of a relationship traverses a cv, then this
refers to the cv of the object_id cvterm.';

COMMENT ON COLUMN cvtermpath.pathdistance IS 'The number of steps
required to get from the subject cvterm to the object cvterm, counting
from zero (reflexive relationship).';

-- ================================================
-- TABLE: cvtermsynonym
-- ================================================
create table cvtermsynonym (
    cvtermsynonym_id bigserial not null,
    primary key (cvtermsynonym_id),
    cvterm_id bigint not null,
    foreign key (cvterm_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    synonym varchar(1024) not null,
    type_id bigint,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade  INITIALLY DEFERRED,
    constraint cvtermsynonym_c1 unique (cvterm_id,synonym)
);
create index cvtermsynonym_idx1 on cvtermsynonym (cvterm_id);

COMMENT ON TABLE cvtermsynonym IS 'A cvterm actually represents a
distinct class or concept. A concept can be refered to by different
phrases or names. In addition to the primary name (cvterm.name) there
can be a number of alternative aliases or synonyms. For example, "T
cell" as a synonym for "T lymphocyte".';

COMMENT ON COLUMN cvtermsynonym.type_id IS 'A synonym can be exact,
narrower, or broader than.';

-- ================================================
-- TABLE: cvterm_dbxref
-- ================================================
create table cvterm_dbxref (
    cvterm_dbxref_id bigserial not null,
    primary key (cvterm_dbxref_id),
    cvterm_id bigint not null,
    foreign key (cvterm_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id bigint not null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED,
    is_for_definition int not null default 0,
    constraint cvterm_dbxref_c1 unique (cvterm_id,dbxref_id)
);
create index cvterm_dbxref_idx1 on cvterm_dbxref (cvterm_id);
create index cvterm_dbxref_idx2 on cvterm_dbxref (dbxref_id);

COMMENT ON TABLE cvterm_dbxref IS 'In addition to the primary
identifier (cvterm.dbxref_id) a cvterm can have zero or more secondary
identifiers/dbxrefs, which may refer to records in external
databases. The exact semantics of cvterm_dbxref are not fixed. For
example: the dbxref could be a pubmed ID that is pertinent to the
cvterm, or it could be an equivalent or similar term in another
ontology. For example, GO cvterms are typically linked to InterPro
IDs, even though the nature of the relationship between them is
largely one of statistical association. The dbxref may be have data
records attached in the same database instance, or it could be a
"hanging" dbxref pointing to some external database. NOTE: If the
desired objective is to link two cvterms together, and the nature of
the relation is known and holds for all instances of the subject
cvterm then consider instead using cvterm_relationship together with a
well-defined relation.';

COMMENT ON COLUMN cvterm_dbxref.is_for_definition IS 'A
cvterm.definition should be supported by one or more references. If
this column is true, the dbxref is not for a term in an external database -
it is a dbxref for provenance information for the definition.';

-- ================================================
-- TABLE: cvtermprop
-- ================================================
create table cvtermprop (
    cvtermprop_id bigserial not null,
    primary key (cvtermprop_id),
    cvterm_id bigint not null,
    foreign key (cvterm_id) references cvterm (cvterm_id) on delete cascade,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade,
    value text not null default '',
    rank int not null default 0,
    unique(cvterm_id, type_id, value, rank)
);
create index cvtermprop_idx1 on cvtermprop (cvterm_id);
create index cvtermprop_idx2 on cvtermprop (type_id);

COMMENT ON TABLE cvtermprop IS 'Additional extensible properties can be attached to a cvterm using this table. Corresponds to -AnnotationProperty- in W3C OWL format.';

COMMENT ON COLUMN cvtermprop.type_id IS 'The name of the property or slot is a cvterm. The meaning of the property is defined in that cvterm.';

COMMENT ON COLUMN cvtermprop.value IS 'The value of the property, represented as text. Numeric values are converted to their text representation.';

COMMENT ON COLUMN cvtermprop.rank IS 'Property-Value ordering. Any
cvterm can have multiple values for any particular property type -
these are ordered in a list using rank, counting from zero. For
properties that are single-valued rather than multi-valued, the
default 0 value should be used.';

-- ================================================
-- TABLE: dbxrefprop
-- ================================================
create table dbxrefprop (
    dbxrefprop_id bigserial not null,
    primary key (dbxrefprop_id),
    dbxref_id bigint not null,
    foreign key (dbxref_id) references dbxref (dbxref_id) INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) INITIALLY DEFERRED,
    value text not null default '',
    rank int not null default 0,
    constraint dbxrefprop_c1 unique (dbxref_id,type_id,rank)
);
create index dbxrefprop_idx1 on dbxrefprop (dbxref_id);
create index dbxrefprop_idx2 on dbxrefprop (type_id);

COMMENT ON TABLE dbxrefprop IS 'Metadata about a dbxref. Note that this is not defined in the dbxref module, as it depends on the cvterm table. This table has a structure analagous to cvtermprop.';

-- ================================================
-- TABLE: cvprop
-- ================================================
create table cvprop (
    cvprop_id bigserial not null,
    primary key (cvprop_id),
    cv_id bigint not null,
    foreign key (cv_id) references cv (cv_id) INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) INITIALLY DEFERRED,
    value text,
    rank int not null default 0,
    constraint cvprop_c1 unique (cv_id,type_id,rank)
);

COMMENT ON TABLE cvprop IS 'Additional extensible properties can be attached to a cv using this table.  A notable example would be the cv version';

COMMENT ON COLUMN cvprop.type_id IS 'The name of the property or slot is a cvterm. The meaning of the property is defined in that cvterm.';
COMMENT ON COLUMN cvprop.value IS 'The value of the property, represented as text. Numeric values are converted to their text representation.';

COMMENT ON COLUMN cvprop.rank IS 'Property-Value ordering. Any
cv can have multiple values for any particular property type -
these are ordered in a list using rank, counting from zero. For
properties that are single-valued rather than multi-valued, the
default 0 value should be used.';

-- ================================================
-- TABLE: chadoprop
-- ================================================
create table chadoprop (
    chadoprop_id bigserial not null,
    primary key (chadoprop_id),
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) INITIALLY DEFERRED,
    value text,
    rank int not null default 0,
    constraint chadoprop_c1 unique (type_id,rank)
);

COMMENT ON TABLE chadoprop IS 'This table is different from other prop tables in the database, as it is for storing information about the database itself, like schema version';

COMMENT ON COLUMN chadoprop.type_id IS 'The name of the property or slot is a cvterm. The meaning of the property is defined in that cvterm.';
COMMENT ON COLUMN chadoprop.value IS 'The value of the property, represented as text. Numeric values are converted to their text representation.';

COMMENT ON COLUMN chadoprop.rank IS 'Property-Value ordering. Any
cv can have multiple values for any particular property type -
these are ordered in a list using rank, counting from zero. For
properties that are single-valued rather than multi-valued, the
default 0 value should be used.';

-- ================================================
-- TABLE: dbprop
-- ================================================

create table dbprop (
  dbprop_id bigserial not null,
  primary key (dbprop_id),
  db_id bigint not null,
  type_id bigint not null,
  value text null,
  rank int not null default 0,
  foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
  foreign key (db_id) references db (db_id) on delete cascade INITIALLY DEFERRED,
  constraint dbprop_c1 unique (db_id,type_id,rank)
);
create index dbprop_idx1 on dbprop (db_id);
create index dbprop_idx2 on dbprop (type_id);

COMMENT ON TABLE dbprop IS 'An external database can have any number of
slot-value property tags attached to it. This is an alternative to
hardcoding a list of columns in the relational schema, and is
completely extensible. There is a unique constraint, dbprop_c1, for
the combination of db_id, rank, and type_id. Multivalued property-value pairs must be differentiated by rank.';

-- $Id: contact.sql,v 1.5 2007-02-25 17:00:17 briano Exp $
-- ==========================================
-- Chado contact module
--
-- =================================================================
-- Dependencies:
--
-- :import cvterm from cv
-- =================================================================

-- ================================================
-- TABLE: contact
-- ================================================

CREATE TABLE contact (
    contact_id bigserial not null,
    primary key (contact_id),
    type_id bigint null,
    foreign key (type_id) references cvterm (cvterm_id),
    name varchar(255) not null,
    description varchar(255) null,
    constraint contact_c1 unique (name)
);

COMMENT ON TABLE contact IS 'Model persons, institutes, groups, organizations, etc.';
COMMENT ON COLUMN contact.type_id IS 'What type of contact is this?  E.g. "person", "lab".';

-- ================================================
-- TABLE: contactprop
-- ================================================
CREATE TABLE contactprop (
    contactprop_id bigserial primary key not null,
    contact_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL,
    CONSTRAINT contactprop_c1 UNIQUE (contact_id, type_id, rank),
    FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE,
    FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE
);

CREATE INDEX contactprop_idx1 ON contactprop USING btree (contact_id);
CREATE INDEX contactprop_idx2 ON contactprop USING btree (type_id);

COMMENT ON TABLE contactprop IS 'A contact can have any number of slot-value property
tags attached to it. This is an alternative to hardcoding a list of columns in the
relational schema, and is completely extensible.';

-- ================================================
-- TABLE: contact_relationship
-- ================================================

create table contact_relationship (
    contact_relationship_id bigserial not null,
    primary key (contact_relationship_id),
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    subject_id bigint not null,
    foreign key (subject_id) references contact (contact_id) on delete cascade INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references contact (contact_id) on delete cascade INITIALLY DEFERRED,
    constraint contact_relationship_c1 unique (subject_id,object_id,type_id)
);
create index contact_relationship_idx1 on contact_relationship (type_id);
create index contact_relationship_idx2 on contact_relationship (subject_id);
create index contact_relationship_idx3 on contact_relationship (object_id);

COMMENT ON TABLE contact_relationship IS 'Model relationships between contacts';
COMMENT ON COLUMN contact_relationship.subject_id IS 'The subject of the subj-predicate-obj sentence. In a DAG, this corresponds to the child node.';
COMMENT ON COLUMN contact_relationship.object_id IS 'The object of the subj-predicate-obj sentence. In a DAG, this corresponds to the parent node.';
COMMENT ON COLUMN contact_relationship.type_id IS 'Relationship type between subject and object. This is a cvterm, typically from the OBO relationship ontology, although other relationship types are allowed.';

-- $Id: pub.sql,v 1.27 2007-02-19 20:50:44 briano Exp $
-- ==========================================
-- Chado pub module
--
-- =================================================================
-- Dependencies:
--
-- :import cvterm from cv
-- :import dbxref from db
-- :import analysis from companalysis
-- :import contact from contact
-- =================================================================

-- ================================================
-- TABLE: pub
-- ================================================

create table pub (
    pub_id bigserial not null,
    primary key (pub_id),
    title text,
    volumetitle text,
    volume varchar(255),
    series_name varchar(255),
    issue varchar(255),
    pyear varchar(255),
    pages varchar(255),
    miniref varchar(255),
    uniquename text not null,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    is_obsolete boolean default 'false',
    publisher varchar(255),
    pubplace varchar(255),
    constraint pub_c1 unique (uniquename)
);
CREATE INDEX pub_idx1 ON pub (type_id);

COMMENT ON TABLE pub IS 'A documented provenance artefact - publications,
documents, personal communication.';
COMMENT ON COLUMN pub.title IS 'Descriptive general heading.';
COMMENT ON COLUMN pub.volumetitle IS 'Title of part if one of a series.';
COMMENT ON COLUMN pub.series_name IS 'Full name of (journal) series.';
COMMENT ON COLUMN pub.pages IS 'Page number range[s], e.g. 457--459, viii + 664pp, lv--lvii.';
COMMENT ON COLUMN pub.type_id IS  'The type of the publication (book, journal, poem, graffiti, etc). Uses pub cv.';

-- ================================================
-- TABLE: pub_relationship
-- ================================================

create table pub_relationship (
    pub_relationship_id bigserial not null,
    primary key (pub_relationship_id),
    subject_id bigint not null,
    foreign key (subject_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,

    constraint pub_relationship_c1 unique (subject_id,object_id,type_id)
);
create index pub_relationship_idx1 on pub_relationship (subject_id);
create index pub_relationship_idx2 on pub_relationship (object_id);
create index pub_relationship_idx3 on pub_relationship (type_id);

COMMENT ON TABLE pub_relationship IS 'Handle relationships between
publications, e.g. when one publication makes others obsolete, when one
publication contains errata with respect to other publication(s), or
when one publication also appears in another pub.';

-- ================================================
-- TABLE: pub_dbxref
-- ================================================

create table pub_dbxref (
    pub_dbxref_id bigserial not null,
    primary key (pub_dbxref_id),
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id bigint not null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED,
    is_current boolean not null default 'true',
    constraint pub_dbxref_c1 unique (pub_id,dbxref_id)
);
create index pub_dbxref_idx1 on pub_dbxref (pub_id);
create index pub_dbxref_idx2 on pub_dbxref (dbxref_id);

COMMENT ON TABLE pub_dbxref IS 'Handle links to repositories,
e.g. Pubmed, Biosis, zoorec, OCLC, Medline, ISSN, coden...';

-- ================================================
-- TABLE: pubauthor
-- ================================================

create table pubauthor (
    pubauthor_id bigserial not null,
    primary key (pubauthor_id),
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    rank int not null,
    editor boolean default 'false',
    surname varchar(100) not null,
    givennames varchar(100),
    suffix varchar(100),

    constraint pubauthor_c1 unique (pub_id, rank)
);
create index pubauthor_idx2 on pubauthor (pub_id);

COMMENT ON TABLE pubauthor IS 'An author for a publication. Note the denormalisation (hence lack of _ in table name) - this is deliberate as it is in general too hard to assign IDs to authors.';
COMMENT ON COLUMN pubauthor.givennames IS 'First name, initials';
COMMENT ON COLUMN pubauthor.suffix IS 'Jr., Sr., etc';
COMMENT ON COLUMN pubauthor.rank IS 'Order of author in author list for this pub - order is important.';
COMMENT ON COLUMN pubauthor.editor IS 'Indicates whether the author is an editor for linked publication. Note: this is a boolean field but does not follow the normal chado convention for naming booleans.';

-- ================================================
-- TABLE: pubprop
-- ================================================

create table pubprop (
    pubprop_id bigserial not null,
    primary key (pubprop_id),
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text not null,
    rank integer,
    constraint pubprop_c1 unique (pub_id,type_id,rank)
);
create index pubprop_idx1 on pubprop (pub_id);
create index pubprop_idx2 on pubprop (type_id);

COMMENT ON TABLE pubprop IS 'Property-value pairs for a pub. Follows standard chado pattern.';

-- ================================================
-- TABLE: pubauthor_contact
-- ================================================

CREATE TABLE pubauthor_contact (
    pubauthor_contact_id bigserial primary key NOT NULL,
    contact_id bigint NOT NULL,
    pubauthor_id bigint NOT NULL,
    CONSTRAINT pubauthor_contact_c1 UNIQUE (contact_id, pubauthor_id),
    FOREIGN KEY (pubauthor_id) REFERENCES pubauthor(pubauthor_id) ON DELETE CASCADE,
    FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE
);

CREATE INDEX pubauthor_contact_idx1 ON pubauthor USING btree (pubauthor_id);
CREATE INDEX pubauthor_contact_idx2 ON contact USING btree (contact_id);

COMMENT ON TABLE pubauthor_contact IS 'An author on a publication may have a corresponding entry in the contact table and this table can link the two.';
-- $Id: organism.sql,v 1.19 2007/04/01 18:45:41 briano Exp $
-- ==========================================
-- Chado organism module
--
-- ============
-- DEPENDENCIES
-- ============
-- :import cvterm from cv
-- :import dbxref from db
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-- ================================================
-- TABLE: organism
-- ================================================

create table organism (
	organism_id bigserial not null,
	primary key (organism_id),
	abbreviation varchar(255) null,
	genus varchar(255) not null,
	species varchar(255) not null,
	common_name varchar(255) null,
  infraspecific_name varchar(1024) null,
  type_id bigint default null,
  FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE CASCADE,
	comment text null,
	constraint organism_c1 unique (genus,species,type_id,infraspecific_name)
);

COMMENT ON TABLE organism IS 'The organismal taxonomic
classification. Note that phylogenies are represented using the
phylogeny module, and taxonomies can be represented using the cvterm
module or the phylogeny module.';

COMMENT ON COLUMN organism.species IS 'A type of organism is always
uniquely identified by genus and species. When mapping from the NCBI
taxonomy names.dmp file, this column must be used where it
is present, as the common_name column is not always unique (e.g. environmental
samples). If a particular strain or subspecies is to be represented,
this is appended onto the species name. Follows standard NCBI taxonomy
pattern.';

COMMENT ON COLUMN organism.type_id IS 'A controlled vocabulary term that
specifies the organism rank below species. It is used when an infraspecific
name is provided.  Ideally, the rank should be a valid ICN name such as
subspecies, varietas, subvarietas, forma and subforma';

COMMENT ON COLUMN organism.infraspecific_name IS 'The scientific name for any taxon
below the rank of species.  The rank should be specified using the type_id field
and the name is provided here.';

-- ================================================
-- TABLE: organism_dbxref
-- ================================================

create table organism_dbxref (
    organism_dbxref_id bigserial not null,
    primary key (organism_dbxref_id),
    organism_id bigint not null,
    foreign key (organism_id) references organism (organism_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id bigint not null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED,
    constraint organism_dbxref_c1 unique (organism_id,dbxref_id)
);
create index organism_dbxref_idx1 on organism_dbxref (organism_id);
create index organism_dbxref_idx2 on organism_dbxref (dbxref_id);

COMMENT ON TABLE organism_dbxref IS 'Links an organism to a dbxref.';

-- ================================================
-- TABLE: organismprop
-- ================================================

create table organismprop (
    organismprop_id bigserial not null,
    primary key (organismprop_id),
    organism_id bigint not null,
    foreign key (organism_id) references organism (organism_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint organismprop_c1 unique (organism_id,type_id,rank)
);
create index organismprop_idx1 on organismprop (organism_id);
create index organismprop_idx2 on organismprop (type_id);

COMMENT ON TABLE organismprop IS 'Tag-value properties - follows standard chado model.';

-- ================================================
-- TABLE: organismprop_pub
-- ================================================

create table organismprop_pub (
    organismprop_pub_id bigserial not null,
    primary key (organismprop_pub_id),
    organismprop_id bigint not null,
    foreign key (organismprop_id) references organismprop (organismprop_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint organismprop_pub_c1 unique (organismprop_id,pub_id)
);
create index organismprop_pub_idx1 on organismprop_pub (organismprop_id);
create index organismprop_pub_idx2 on organismprop_pub (pub_id);

COMMENT ON TABLE organismprop_pub IS 'Attribution for organismprop.';


-- ================================================
-- TABLE: organism_pub
-- ================================================

create table organism_pub (
       organism_pub_id bigserial not null,
       primary key (organism_pub_id),
       organism_id bigint not null,
       foreign key (organism_id) references organism (organism_id) on delete cascade INITIALLY DEFERRED,
       pub_id bigint not null,
       foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
       constraint organism_pub_c1 unique (organism_id,pub_id)
);
create index organism_pub_idx1 on organism_pub (organism_id);
create index organism_pub_idx2 on organism_pub (pub_id);

COMMENT ON TABLE organism_pub IS 'Attribution for organism.';


-- ================================================
-- TABLE: organism_cvterm
-- ================================================

create table organism_cvterm (
       organism_cvterm_id bigserial not null,
       primary key (organism_cvterm_id),
       organism_id bigint not null,
       foreign key (organism_id) references organism (organism_id) on delete cascade INITIALLY
DEFERRED,
       cvterm_id bigint not null,
       foreign key (cvterm_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
       rank int not null default 0,
       pub_id bigint not null,
       foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
       constraint organism_cvterm_c1 unique(organism_id,cvterm_id,pub_id)
);
create index organism_cvterm_idx1 on organism_cvterm (organism_id);
create index organism_cvterm_idx2 on organism_cvterm (cvterm_id);

COMMENT ON TABLE organism_cvterm IS 'organism to cvterm associations. Examples: taxonomic name';

COMMENT ON COLUMN organism_cvterm.rank IS 'Property-Value
ordering. Any organism_cvterm can have multiple values for any particular
property type - these are ordered in a list using rank, counting from
zero. For properties that are single-valued rather than multi-valued,
the default 0 value should be used';


-- ================================================
-- TABLE: organism_cvtermprop
-- ================================================

create table organism_cvtermprop (
    organism_cvtermprop_id bigserial not null,
    primary key (organism_cvtermprop_id),
    organism_cvterm_id bigint not null,
    foreign key (organism_cvterm_id) references organism_cvterm (organism_cvterm_id) on delete cascade,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint organism_cvtermprop_c1 unique (organism_cvterm_id,type_id,rank)
);
create index organism_cvtermprop_idx1 on organism_cvtermprop (organism_cvterm_id);
create index organism_cvtermprop_idx2 on organism_cvtermprop (type_id);

COMMENT ON TABLE organism_cvtermprop IS 'Extensible properties for
organism to cvterm associations. Examples: qualifiers';

COMMENT ON COLUMN organism_cvtermprop.type_id IS 'The name of the
property/slot is a cvterm. The meaning of the property is defined in
that cvterm. ';

COMMENT ON COLUMN organism_cvtermprop.value IS 'The value of the
property, represented as text. Numeric values are converted to their
text representation. This is less efficient than using native database
types, but is easier to query.';

COMMENT ON COLUMN organism_cvtermprop.rank IS 'Property-Value
ordering. Any organism_cvterm can have multiple values for any particular
property type - these are ordered in a list using rank, counting from
zero. For properties that are single-valued rather than multi-valued,
the default 0 value should be used';

-- ================================================
-- TABLE: organism_relationship
-- ================================================

CREATE TABLE organism_relationship (
    organism_relationship_id bigserial primary key NOT NULL,
    subject_id bigint NOT NULL,
    object_id bigint NOT NULL,
    type_id bigint NOT NULL,
    rank integer DEFAULT 0 NOT NULL,
    CONSTRAINT organism_relationship_c1 UNIQUE (subject_id, object_id, type_id, rank),
    FOREIGN KEY (object_id) REFERENCES organism(organism_id) ON DELETE CASCADE,
    FOREIGN KEY (subject_id) REFERENCES organism(organism_id) ON DELETE CASCADE,
    FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE
);

CREATE INDEX organism_relationship_idx1 ON organism_relationship USING btree (subject_id);
CREATE INDEX organism_relationship_idx2 ON organism_relationship USING btree (object_id);
CREATE INDEX organism_relationship_idx3 ON organism_relationship USING btree (type_id);

COMMENT ON TABLE organism_relationship IS 'Specifies relationships between organisms
that are not taxonomic. For example, in breeding, relationships such as
"sterile_with", "incompatible_with", or "fertile_with" would be appropriate. Taxonomic
relatinoships should be housed in the phylogeny tables.';

-- $Id: sequence.sql,v 1.69 2009-05-14 02:44:23 scottcain Exp $
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

-- ================================================
-- TABLE: feature
-- ================================================

create table feature (
    feature_id bigserial not null,
    primary key (feature_id),
    dbxref_id bigint,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
    organism_id bigint not null,
    foreign key (organism_id) references organism (organism_id) on delete cascade INITIALLY DEFERRED,
    name varchar(255),
    uniquename text not null,
    residues text,
    seqlen bigint,
    md5checksum char(32),
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    is_analysis boolean not null default 'false',
    is_obsolete boolean not null default 'false',
    timeaccessioned timestamp not null default current_timestamp,
    timelastmodified timestamp not null default current_timestamp,
    constraint feature_c1 unique (organism_id,uniquename,type_id)
);
create sequence feature_uniquename_seq;
create index feature_name_ind1 on feature(name);
create index feature_idx1 on feature (dbxref_id);
create index feature_idx2 on feature (organism_id);
create index feature_idx3 on feature (type_id);
create index feature_idx4 on feature (uniquename);
create index feature_idx5 on feature (lower(name));

ALTER TABLE feature ALTER residues SET STORAGE EXTERNAL;

COMMENT ON TABLE feature IS 'A feature is a biological sequence or a
section of a biological sequence, or a collection of such
sections. Examples include genes, exons, transcripts, regulatory
regions, polypeptides, protein domains, chromosome sequences, sequence
variations, cross-genome match regions such as hits and HSPs and so
on; see the Sequence Ontology for more. The combination of
organism_id, uniquename and type_id should be unique.';

COMMENT ON COLUMN feature.dbxref_id IS 'An optional primary public stable
identifier for this feature. Secondary identifiers and external
dbxrefs go in the table feature_dbxref.';

COMMENT ON COLUMN feature.organism_id IS 'The organism to which this feature
belongs. This column is mandatory.';

COMMENT ON COLUMN feature.name IS 'The optional human-readable common name for
a feature, for display purposes.';

COMMENT ON COLUMN feature.uniquename IS 'The unique name for a feature; may
not be necessarily be particularly human-readable, although this is
preferred. This name must be unique for this type of feature within
this organism.';

COMMENT ON COLUMN feature.residues IS 'A sequence of alphabetic characters
representing biological residues (nucleic acids, amino acids). This
column does not need to be manifested for all features; it is optional
for features such as exons where the residues can be derived from the
featureloc. It is recommended that the value for this column be
manifested for features which may may non-contiguous sublocations (e.g.
transcripts), since derivation at query time is non-trivial. For
expressed sequence, the DNA sequence should be used rather than the
RNA sequence. The default storage method for the residues column is
EXTERNAL, which will store it uncompressed to make substring operations
faster.';

COMMENT ON COLUMN feature.seqlen IS 'The length of the residue feature. See
column:residues. This column is partially redundant with the residues
column, and also with featureloc. This column is required because the
location may be unknown and the residue sequence may not be
manifested, yet it may be desirable to store and query the length of
the feature. The seqlen should always be manifested where the length
of the sequence is known.';

COMMENT ON COLUMN feature.md5checksum IS 'The 32-character checksum of the sequence,
calculated using the MD5 algorithm. This is practically guaranteed to
be unique for any feature. This column thus acts as a unique
identifier on the mathematical sequence.';

COMMENT ON COLUMN feature.type_id IS 'A required reference to a table:cvterm
giving the feature type. This will typically be a Sequence Ontology
identifier. This column is thus used to subclass the feature table.';

COMMENT ON COLUMN feature.is_analysis IS 'Boolean indicating whether this
feature is annotated or the result of an automated analysis. Analysis
results also use the companalysis module. Note that the dividing line
between analysis and annotation may be fuzzy, this should be determined on
a per-project basis in a consistent manner. One requirement is that
there should only be one non-analysis version of each wild-type gene
feature in a genome, whereas the same gene feature can be predicted
multiple times in different analyses.';

COMMENT ON COLUMN feature.is_obsolete IS 'Boolean indicating whether this
feature has been obsoleted. Some chado instances may choose to simply
remove the feature altogether, others may choose to keep an obsolete
row in the table.';

COMMENT ON COLUMN feature.timeaccessioned IS 'For handling object
accession or modification timestamps (as opposed to database auditing data,
handled elsewhere). The expectation is that these fields would be
available to software interacting with chado.';

COMMENT ON COLUMN feature.timelastmodified IS 'For handling object
accession or modification timestamps (as opposed to database auditing data,
handled elsewhere). The expectation is that these fields would be
available to software interacting with chado.';

--- COMMENT ON INDEX feature_c1 IS 'Any feature can be globally identified
--- by the combination of organism, uniquename and feature type';

-- ================================================
-- TABLE: featureloc
-- ================================================

create table featureloc (
    featureloc_id bigserial not null,
    primary key (featureloc_id),
    feature_id bigint not null,
    foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    srcfeature_id bigint,
    foreign key (srcfeature_id) references feature (feature_id) on delete set null INITIALLY DEFERRED,
    fmin bigint,
    is_fmin_partial boolean not null default 'false',
    fmax bigint,
    is_fmax_partial boolean not null default 'false',
    strand smallint,
    phase int,
    residue_info text,
    locgroup int not null default 0,
    rank int not null default 0,
    constraint featureloc_c1 unique (feature_id,locgroup,rank),
    constraint featureloc_c2 check (fmin <= fmax)
);
create index featureloc_idx1 on featureloc (feature_id);
create index featureloc_idx2 on featureloc (srcfeature_id);
create index featureloc_idx3 on featureloc (srcfeature_id,fmin,fmax);

COMMENT ON TABLE featureloc IS 'The location of a feature relative to
another feature. Important: interbase coordinates are used. This is
vital as it allows us to represent zero-length features e.g. splice
sites, insertion points without an awkward fuzzy system. Features
typically have exactly ONE location, but this need not be the
case. Some features may not be localized (e.g. a gene that has been
characterized genetically but no sequence or molecular information is
available). Note on multiple locations: Each feature can have 0 or
more locations. Multiple locations do NOT indicate non-contiguous
locations (if a feature such as a transcript has a non-contiguous
location, then the subfeatures such as exons should always be
manifested). Instead, multiple featurelocs for a feature designate
alternate locations or grouped locations; for instance, a feature
designating a blast hit or hsp will have two locations, one on the
query feature, one on the subject feature. Features representing
sequence variation could have alternate locations instantiated on a
feature on the mutant strain. The column:rank is used to
differentiate these different locations. Reflexive locations should
never be stored - this is for -proper- (i.e. non-self) locations only; nothing should be located relative to itself.';

COMMENT ON COLUMN featureloc.feature_id IS 'The feature that is being located. Any feature can have zero or more featurelocs.';

COMMENT ON COLUMN featureloc.srcfeature_id IS 'The source feature which this location is relative to. Every location is relative to another feature (however, this column is nullable, because the srcfeature may not be known). All locations are -proper- that is, nothing should be located relative to itself. No cycles are allowed in the featureloc graph.';

COMMENT ON COLUMN featureloc.fmin IS 'The leftmost/minimal boundary in the linear range represented by the featureloc. Sometimes (e.g. in Bioperl) this is called -start- although this is confusing because it does not necessarily represent the 5-prime coordinate. Important: This is space-based (interbase) coordinates, counting from zero. To convert this to the leftmost position in a base-oriented system (eg GFF, Bioperl), add 1 to fmin.';

COMMENT ON COLUMN featureloc.fmax IS 'The rightmost/maximal boundary in the linear range represented by the featureloc. Sometimes (e.g. in bioperl) this is called -end- although this is confusing because it does not necessarily represent the 3-prime coordinate. Important: This is space-based (interbase) coordinates, counting from zero. No conversion is required to go from fmax to the rightmost coordinate in a base-oriented system that counts from 1 (e.g. GFF, Bioperl).';

COMMENT ON COLUMN featureloc.strand IS 'The orientation/directionality of the
location. Should be 0, -1 or +1.';

COMMENT ON COLUMN featureloc.rank IS 'Used when a feature has >1
location, otherwise the default rank 0 is used. Some features (e.g.
blast hits and HSPs) have two locations - one on the query and one on
the subject. Rank is used to differentiate these. Rank=0 is always
used for the query, Rank=1 for the subject. For multiple alignments,
assignment of rank is arbitrary. Rank is also used for
sequence_variant features, such as SNPs. Rank=0 indicates the wildtype
(or baseline) feature, Rank=1 indicates the mutant (or compared) feature.';

COMMENT ON COLUMN featureloc.locgroup IS 'This is used to manifest redundant,
derivable extra locations for a feature. The default locgroup=0 is
used for the DIRECT location of a feature. Important: most Chado users may
never use featurelocs WITH logroup > 0. Transitively derived locations
are indicated with locgroup > 0. For example, the position of an exon on
a BAC and in global chromosome coordinates. This column is used to
differentiate these groupings of locations. The default locgroup 0
is used for the main or primary location, from which the others can be
derived via coordinate transformations. Another example of redundant
locations is storing ORF coordinates relative to both transcript and
genome. Redundant locations open the possibility of the database
getting into inconsistent states; this schema gives us the flexibility
of both warehouse instantiations with redundant locations (easier for
querying) and management instantiations with no redundant
locations. An example of using both locgroup and rank: imagine a
feature indicating a conserved region between the chromosomes of two
different species. We may want to keep redundant locations on both
contigs and chromosomes. We would thus have 4 locations for the single
conserved region feature - two distinct locgroups (contig level and
chromosome level) and two distinct ranks (for the two species).';

COMMENT ON COLUMN featureloc.residue_info IS 'Alternative residues,
when these differ from feature.residues. For instance, a SNP feature
located on a wild and mutant protein would have different alternative residues.
for alignment/similarity features, the alternative residues is used to
represent the alignment string (CIGAR format). Note on variation
features; even if we do not want to instantiate a mutant
chromosome/contig feature, we can still represent a SNP etc with 2
locations, one (rank 0) on the genome, the other (rank 1) would have
most fields null, except for alternative residues.';

COMMENT ON COLUMN featureloc.phase IS 'Phase of translation with
respect to srcfeature_id.
Values are 0, 1, 2. It may not be possible to manifest this column for
some features such as exons, because the phase is dependant on the
spliceform (the same exon can appear in multiple spliceforms). This column is mostly useful for predicted exons and CDSs.';

COMMENT ON COLUMN featureloc.is_fmin_partial IS 'This is typically
false, but may be true if the value for column:fmin is inaccurate or
the leftmost part of the range is unknown/unbounded.';

COMMENT ON COLUMN featureloc.is_fmax_partial IS 'This is typically
false, but may be true if the value for column:fmax is inaccurate or
the rightmost part of the range is unknown/unbounded.';

--- COMMENT ON INDEX featureloc_c1 IS 'locgroup and rank serve to uniquely
--- partition locations for any one feature';


-- ================================================
-- TABLE: featureloc_pub
-- ================================================

create table featureloc_pub (
    featureloc_pub_id bigserial not null,
    primary key (featureloc_pub_id),
    featureloc_id bigint not null,
    foreign key (featureloc_id) references featureloc (featureloc_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    constraint featureloc_pub_c1 unique (featureloc_id,pub_id)
);
create index featureloc_pub_idx1 on featureloc_pub (featureloc_id);
create index featureloc_pub_idx2 on featureloc_pub (pub_id);

COMMENT ON TABLE featureloc_pub IS 'Provenance of featureloc. Linking table between featurelocs and publications that mention them.';


-- ================================================
-- TABLE: feature_pub
-- ================================================

create table feature_pub (
    feature_pub_id bigserial not null,
    primary key (feature_pub_id),
    feature_id bigint not null,
    foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    constraint feature_pub_c1 unique (feature_id,pub_id)
);
create index feature_pub_idx1 on feature_pub (feature_id);
create index feature_pub_idx2 on feature_pub (pub_id);

COMMENT ON TABLE feature_pub IS 'Provenance. Linking table between features and publications that mention them.';


-- ================================================
-- TABLE: feature_pubprop
-- ================================================

create table feature_pubprop (
    feature_pubprop_id bigserial not null,
    primary key (feature_pubprop_id),
    feature_pub_id bigint not null,
    foreign key (feature_pub_id) references feature_pub (feature_pub_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint feature_pubprop_c1 unique (feature_pub_id,type_id,rank)
);
create index feature_pubprop_idx1 on feature_pubprop (feature_pub_id);

COMMENT ON TABLE feature_pubprop IS 'Property or attribute of a feature_pub link.';


-- ================================================
-- TABLE: featureprop
-- ================================================

create table featureprop (
    featureprop_id bigserial not null,
    primary key (featureprop_id),
    feature_id bigint not null,
    foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint featureprop_c1 unique (feature_id,type_id,rank)
);
create index featureprop_idx1 on featureprop (feature_id);
create index featureprop_idx2 on featureprop (type_id);

COMMENT ON TABLE featureprop IS 'A feature can have any number of slot-value property tags attached to it. This is an alternative to hardcoding a list of columns in the relational schema, and is completely extensible.';

COMMENT ON COLUMN featureprop.type_id IS 'The name of the
property/slot is a cvterm. The meaning of the property is defined in
that cvterm. Certain property types will only apply to certain feature
types (e.g. the anticodon property will only apply to tRNA features) ;
the types here come from the sequence feature property ontology.';

COMMENT ON COLUMN featureprop.value IS 'The value of the property, represented as text. Numeric values are converted to their text representation. This is less efficient than using native database types, but is easier to query.';

COMMENT ON COLUMN featureprop.rank IS 'Property-Value ordering. Any
feature can have multiple values for any particular property type -
these are ordered in a list using rank, counting from zero. For
properties that are single-valued rather than multi-valued, the
default 0 value should be used';

COMMENT ON INDEX featureprop_c1 IS 'For any one feature, multivalued
property-value pairs must be differentiated by rank.';


-- ================================================
-- TABLE: featureprop_pub
-- ================================================

create table featureprop_pub (
    featureprop_pub_id bigserial not null,
    primary key (featureprop_pub_id),
    featureprop_id bigint not null,
    foreign key (featureprop_id) references featureprop (featureprop_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    constraint featureprop_pub_c1 unique (featureprop_id,pub_id)
);
create index featureprop_pub_idx1 on featureprop_pub (featureprop_id);
create index featureprop_pub_idx2 on featureprop_pub (pub_id);

COMMENT ON TABLE featureprop_pub IS 'Provenance. Any featureprop assignment can optionally be supported by a publication.';


-- ================================================
-- TABLE: feature_dbxref
-- ================================================

create table feature_dbxref (
    feature_dbxref_id bigserial not null,
    primary key (feature_dbxref_id),
    feature_id bigint not null,
    foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id bigint not null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED,
    is_current boolean not null default 'true',
    constraint feature_dbxref_c1 unique (feature_id,dbxref_id)
);
create index feature_dbxref_idx1 on feature_dbxref (feature_id);
create index feature_dbxref_idx2 on feature_dbxref (dbxref_id);

COMMENT ON TABLE feature_dbxref IS 'Links a feature to dbxrefs. This is for secondary identifiers; primary identifiers should use feature.dbxref_id.';

COMMENT ON COLUMN feature_dbxref.is_current IS 'True if this secondary dbxref is the most up to date accession in the corresponding db. Retired accessions should set this field to false';


-- ================================================
-- TABLE: feature_relationship
-- ================================================

create table feature_relationship (
    feature_relationship_id bigserial not null,
    primary key (feature_relationship_id),
    subject_id bigint not null,
    foreign key (subject_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint feature_relationship_c1 unique (subject_id,object_id,type_id,rank)
);
create index feature_relationship_idx1 on feature_relationship (subject_id);
create index feature_relationship_idx2 on feature_relationship (object_id);
create index feature_relationship_idx3 on feature_relationship (type_id);

COMMENT ON TABLE feature_relationship IS 'Features can be arranged in
graphs, e.g. "exon part_of transcript part_of gene"; If type is
thought of as a verb, the each arc or edge makes a statement
[Subject Verb Object]. The object can also be thought of as parent
(containing feature), and subject as child (contained feature or
subfeature). We include the relationship rank/order, because even
though most of the time we can order things implicitly by sequence
coordinates, we can not always do this - e.g. transpliced genes. It is also
useful for quickly getting implicit introns.';

COMMENT ON COLUMN feature_relationship.subject_id IS 'The subject of the subj-predicate-obj sentence. This is typically the subfeature.';

COMMENT ON COLUMN feature_relationship.object_id IS 'The object of the subj-predicate-obj sentence. This is typically the container feature.';

COMMENT ON COLUMN feature_relationship.type_id IS 'Relationship type between subject and object. This is a cvterm, typically from the OBO relationship ontology, although other relationship types are allowed. The most common relationship type is OBO_REL:part_of. Valid relationship types are constrained by the Sequence Ontology.';

COMMENT ON COLUMN feature_relationship.rank IS 'The ordering of subject features with respect to the object feature may be important (for example, exon ordering on a transcript - not always derivable if you take trans spliced genes into consideration). Rank is used to order these; starts from zero.';

COMMENT ON COLUMN feature_relationship.value IS 'Additional notes or comments.';


-- ================================================
-- TABLE: feature_relationship_pub
-- ================================================

create table feature_relationship_pub (
	feature_relationship_pub_id bigserial not null,
	primary key (feature_relationship_pub_id),
	feature_relationship_id bigint not null,
	foreign key (feature_relationship_id) references feature_relationship (feature_relationship_id) on delete cascade INITIALLY DEFERRED,
	pub_id bigint not null,
	foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    constraint feature_relationship_pub_c1 unique (feature_relationship_id,pub_id)
);
create index feature_relationship_pub_idx1 on feature_relationship_pub (feature_relationship_id);
create index feature_relationship_pub_idx2 on feature_relationship_pub (pub_id);

COMMENT ON TABLE feature_relationship_pub IS 'Provenance. Attach optional evidence to a feature_relationship in the form of a publication.';


-- ================================================
-- TABLE: feature_relationshipprop
-- ================================================

create table feature_relationshipprop (
    feature_relationshipprop_id bigserial not null,
    primary key (feature_relationshipprop_id),
    feature_relationship_id bigint not null,
    foreign key (feature_relationship_id) references feature_relationship (feature_relationship_id) on delete cascade,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint feature_relationshipprop_c1 unique (feature_relationship_id,type_id,rank)
);
create index feature_relationshipprop_idx1 on feature_relationshipprop (feature_relationship_id);
create index feature_relationshipprop_idx2 on feature_relationshipprop (type_id);

COMMENT ON TABLE feature_relationshipprop IS 'Extensible properties
for feature_relationships. Analagous structure to featureprop. This
table is largely optional and not used with a high frequency. Typical
scenarios may be if one wishes to attach additional data to a
feature_relationship - for example to say that the
feature_relationship is only true in certain contexts.';

COMMENT ON COLUMN feature_relationshipprop.type_id IS 'The name of the
property/slot is a cvterm. The meaning of the property is defined in
that cvterm. Currently there is no standard ontology for
feature_relationship property types.';

COMMENT ON COLUMN feature_relationshipprop.value IS 'The value of the
property, represented as text. Numeric values are converted to their
text representation. This is less efficient than using native database
types, but is easier to query.';

COMMENT ON COLUMN feature_relationshipprop.rank IS 'Property-Value
ordering. Any feature_relationship can have multiple values for any particular
property type - these are ordered in a list using rank, counting from
zero. For properties that are single-valued rather than multi-valued,
the default 0 value should be used.';

-- ================================================
-- TABLE: feature_relationshipprop_pub
-- ================================================

create table feature_relationshipprop_pub (
    feature_relationshipprop_pub_id bigserial not null,
    primary key (feature_relationshipprop_pub_id),
    feature_relationshipprop_id bigint not null,
    foreign key (feature_relationshipprop_id) references feature_relationshipprop (feature_relationshipprop_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    constraint feature_relationshipprop_pub_c1 unique (feature_relationshipprop_id,pub_id)
);
create index feature_relationshipprop_pub_idx1 on feature_relationshipprop_pub (feature_relationshipprop_id);
create index feature_relationshipprop_pub_idx2 on feature_relationshipprop_pub (pub_id);

COMMENT ON TABLE feature_relationshipprop_pub IS 'Provenance for feature_relationshipprop.';

-- ================================================
-- TABLE: feature_cvterm
-- ================================================

create table feature_cvterm (
    feature_cvterm_id bigserial not null,
    primary key (feature_cvterm_id),
    feature_id bigint not null,
    foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    cvterm_id bigint not null,
    foreign key (cvterm_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    is_not boolean not null default false,
    rank int not null default 0,
    constraint feature_cvterm_c1 unique (feature_id,cvterm_id,pub_id,rank)
);
create index feature_cvterm_idx1 on feature_cvterm (feature_id);
create index feature_cvterm_idx2 on feature_cvterm (cvterm_id);
create index feature_cvterm_idx3 on feature_cvterm (pub_id);

COMMENT ON TABLE feature_cvterm IS 'Associate a term from a cv with a feature, for example, GO annotation.';

COMMENT ON COLUMN feature_cvterm.pub_id IS 'Provenance for the annotation. Each annotation should have a single primary publication (which may be of the appropriate type for computational analyses) where more details can be found. Additional provenance dbxrefs can be attached using feature_cvterm_dbxref.';

COMMENT ON COLUMN feature_cvterm.is_not IS 'If this is set to true, then this annotation is interpreted as a NEGATIVE annotation - i.e. the feature does NOT have the specified function, process, component, part, etc. See GO docs for more details.';


-- ================================================
-- TABLE: feature_cvtermprop
-- ================================================

create table feature_cvtermprop (
    feature_cvtermprop_id bigserial not null,
    primary key (feature_cvtermprop_id),
    feature_cvterm_id bigint not null,
    foreign key (feature_cvterm_id) references feature_cvterm (feature_cvterm_id) on delete cascade,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint feature_cvtermprop_c1 unique (feature_cvterm_id,type_id,rank)
);
create index feature_cvtermprop_idx1 on feature_cvtermprop (feature_cvterm_id);
create index feature_cvtermprop_idx2 on feature_cvtermprop (type_id);

COMMENT ON TABLE feature_cvtermprop IS 'Extensible properties for
feature to cvterm associations. Examples: GO evidence codes;
qualifiers; metadata such as the date on which the entry was curated
and the source of the association. See the featureprop table for
meanings of type_id, value and rank.';

COMMENT ON COLUMN feature_cvtermprop.type_id IS 'The name of the
property/slot is a cvterm. The meaning of the property is defined in
that cvterm. cvterms may come from the OBO evidence code cv.';

COMMENT ON COLUMN feature_cvtermprop.value IS 'The value of the
property, represented as text. Numeric values are converted to their
text representation. This is less efficient than using native database
types, but is easier to query.';

COMMENT ON COLUMN feature_cvtermprop.rank IS 'Property-Value
ordering. Any feature_cvterm can have multiple values for any particular
property type - these are ordered in a list using rank, counting from
zero. For properties that are single-valued rather than multi-valued,
the default 0 value should be used.';


-- ================================================
-- TABLE: feature_cvterm_dbxref
-- ================================================

create table feature_cvterm_dbxref (
    feature_cvterm_dbxref_id bigserial not null,
    primary key (feature_cvterm_dbxref_id),
    feature_cvterm_id bigint not null,
    foreign key (feature_cvterm_id) references feature_cvterm (feature_cvterm_id) on delete cascade,
    dbxref_id bigint not null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED,
    constraint feature_cvterm_dbxref_c1 unique (feature_cvterm_id,dbxref_id)
);
create index feature_cvterm_dbxref_idx1 on feature_cvterm_dbxref (feature_cvterm_id);
create index feature_cvterm_dbxref_idx2 on feature_cvterm_dbxref (dbxref_id);

COMMENT ON TABLE feature_cvterm_dbxref IS 'Additional dbxrefs for an association. Rows in the feature_cvterm table may be backed up by dbxrefs. For example, a feature_cvterm association that was inferred via a protein-protein interaction may be backed by by refering to the dbxref for the alternate protein. Corresponds to the WITH column in a GO gene association file (but can also be used for other analagous associations). See http://www.geneontology.org/doc/GO.annotation.shtml#file for more details.';

-- ================================================
-- TABLE: feature_cvterm_pub
-- ================================================

create table feature_cvterm_pub (
    feature_cvterm_pub_id bigserial not null,
    primary key (feature_cvterm_pub_id),
    feature_cvterm_id bigint not null,
    foreign key (feature_cvterm_id) references feature_cvterm (feature_cvterm_id) on delete cascade,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    constraint feature_cvterm_pub_c1 unique (feature_cvterm_id,pub_id)
);
create index feature_cvterm_pub_idx1 on feature_cvterm_pub (feature_cvterm_id);
create index feature_cvterm_pub_idx2 on feature_cvterm_pub (pub_id);

COMMENT ON TABLE feature_cvterm_pub IS 'Secondary pubs for an
association. Each feature_cvterm association is supported by a single
primary publication. Additional secondary pubs can be added using this
linking table (in a GO gene association file, these corresponding to
any IDs after the pipe symbol in the publications column.';

-- ================================================
-- TABLE: synonym
-- ================================================

create table synonym (
    synonym_id bigserial not null,
    primary key (synonym_id),
    name varchar(255) not null,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    synonym_sgml varchar(255) not null,
    constraint synonym_c1 unique (name,type_id)
);
create index synonym_idx1 on synonym (type_id);
create index synonym_idx2 on synonym ((lower(synonym_sgml)));

COMMENT ON TABLE synonym IS 'A synonym for a feature. One feature can have multiple synonyms, and the same synonym can apply to multiple features.';

COMMENT ON COLUMN synonym.name IS 'The synonym itself. Should be human-readable machine-searchable ascii text.';

COMMENT ON COLUMN synonym.synonym_sgml IS 'The fully specified synonym, with any non-ascii characters encoded in SGML.';

COMMENT ON COLUMN synonym.type_id IS 'Types would be symbol and fullname for now.';


-- ================================================
-- TABLE: feature_synonym
-- ================================================

create table feature_synonym (
    feature_synonym_id bigserial not null,
    primary key (feature_synonym_id),
    synonym_id bigint not null,
    foreign key (synonym_id) references synonym (synonym_id) on delete cascade INITIALLY DEFERRED,
    feature_id bigint not null,
    foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    is_current boolean not null default 'false',
    is_internal boolean not null default 'false',
    constraint feature_synonym_c1 unique (synonym_id,feature_id,pub_id)
);
create index feature_synonym_idx1 on feature_synonym (synonym_id);
create index feature_synonym_idx2 on feature_synonym (feature_id);
create index feature_synonym_idx3 on feature_synonym (pub_id);

COMMENT ON TABLE feature_synonym IS 'Linking table between feature and synonym.';

COMMENT ON COLUMN feature_synonym.pub_id IS 'The pub_id link is for relating the usage of a given synonym to the publication in which it was used.';

COMMENT ON COLUMN feature_synonym.is_current IS 'The is_current boolean indicates whether the linked synonym is the  current -official- symbol for the linked feature.';

COMMENT ON COLUMN feature_synonym.is_internal IS 'Typically a synonym exists so that somebody querying the db with an obsolete name can find the object theyre looking for (under its current name.  If the synonym has been used publicly and deliberately (e.g. in a paper), it may also be listed in reports as a synonym. If the synonym was not used deliberately (e.g. there was a typo which went public), then the is_internal boolean may be set to -true- so that it is known that the synonym is -internal- and should be queryable but should not be listed in reports as a valid synonym.';

-- ================================================
-- TABLE: feature_contact
-- ================================================

CREATE TABLE feature_contact (
    feature_contact_id bigserial primary key NOT NULL,
    feature_id bigint NOT NULL,
    contact_id bigint NOT NULL,
    CONSTRAINT feature_contact_c1 UNIQUE (feature_id, contact_id),
    FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE,
    FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE
);

CREATE INDEX feature_contact_idx1 ON feature_contact USING btree (feature_id);
CREATE INDEX feature_contact_idx2 ON feature_contact USING btree (contact_id);

COMMENT ON TABLE feature_contact IS 'Links contact(s) with a feature.  Used to indicate a particular
person or organization responsible for discovery or that can provide more information on a particular feature.';

-- $Id: companalysis.sql,v 1.37 2007-03-23 15:18:02 scottcain Exp $
-- ==========================================
-- Chado companalysis module
--
-- =================================================================
-- Dependencies:
--
-- :import feature from sequence
-- :import cvterm from cv
-- :import dbxref from db
-- :import pub from pub
-- =================================================================

-- ================================================
-- TABLE: analysis
-- ================================================

create table analysis (
    analysis_id bigserial not null,
    primary key (analysis_id),
    name varchar(255),
    description text,
    program varchar(255) not null,
    programversion varchar(255) not null,
    algorithm varchar(255),
    sourcename varchar(255),
    sourceversion varchar(255),
    sourceuri text,
    timeexecuted timestamp not null default current_timestamp,
    constraint analysis_c1 unique (program,programversion,sourcename)
);
COMMENT ON TABLE analysis IS 'An analysis is a particular type of a
    computational analysis; it may be a blast of one sequence against
    another, or an all by all blast, or a different kind of analysis
    altogether. It is a single unit of computation.';
COMMENT ON COLUMN analysis.name IS 'A way of grouping analyses. This
    should be a handy short identifier that can help people find an
    analysis they want. For instance "tRNAscan", "cDNA", "FlyPep",
    "SwissProt", and it should not be assumed to be unique. For instance, there may be lots of separate analyses done against a cDNA database.';
COMMENT ON COLUMN analysis.program IS 'Program name, e.g. blastx, blastp, sim4, genscan.';
COMMENT ON COLUMN analysis.programversion IS 'Version description, e.g. TBLASTX 2.0MP-WashU [09-Nov-2000].';
COMMENT ON COLUMN analysis.algorithm IS 'Algorithm name, e.g. blast.';
COMMENT ON COLUMN analysis.sourcename IS 'Source name, e.g. cDNA, SwissProt.';
COMMENT ON COLUMN analysis.sourceuri IS 'This is an optional, permanent URL or URI for the source of the  analysis. The idea is that someone could recreate the analysis directly by going to this URI and fetching the source data (e.g. the blast database, or the training model).';

-- ================================================
-- TABLE: analysisprop
-- ================================================

create table analysisprop (
    analysisprop_id bigserial not null,
    primary key (analysisprop_id),
    analysis_id bigint not null,
    foreign key (analysis_id) references analysis (analysis_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text,
    rank int not null default 0,
    constraint analysisprop_c1 unique (analysis_id,type_id,rank)
);
create index analysisprop_idx1 on analysisprop (analysis_id);
create index analysisprop_idx2 on analysisprop (type_id);

-- ================================================
-- TABLE: analysisfeature
-- ================================================

create table analysisfeature (
    analysisfeature_id bigserial not null,
    primary key (analysisfeature_id),
    feature_id bigint not null,
    foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    analysis_id bigint not null,
    foreign key (analysis_id) references analysis (analysis_id) on delete cascade INITIALLY DEFERRED,
    rawscore double precision,
    normscore double precision,
    significance double precision,
    identity double precision,
    constraint analysisfeature_c1 unique (feature_id,analysis_id)
);
create index analysisfeature_idx1 on analysisfeature (feature_id);
create index analysisfeature_idx2 on analysisfeature (analysis_id);

COMMENT ON TABLE analysisfeature IS 'Computational analyses generate features (e.g. Genscan generates transcripts and exons; sim4 alignments generate similarity/match features). analysisfeatures are stored using the feature table from the sequence module. The analysisfeature table is used to decorate these features, with analysis specific attributes. A feature is an analysisfeature if and only if there is a corresponding entry in the analysisfeature table. analysisfeatures will have two or more featureloc entries,
 with rank indicating query/subject';
COMMENT ON COLUMN analysisfeature.identity IS 'Percent identity between the locations compared.  Note that these 4 metrics do not cover the full range of scores possible; it would be undesirable to list every score possible, as this should be kept extensible. instead, for non-standard scores, use the analysisprop table.';
COMMENT ON COLUMN analysisfeature.significance IS 'This is some kind of expectation or probability metric, representing the probability that the analysis would appear randomly given the model. As such, any program or person querying this table can assume the following semantics:
   * 0 <= significance <= n, where n is a positive number, theoretically unbounded but unlikely to be more than 10
  * low numbers are better than high numbers.';
COMMENT ON COLUMN analysisfeature.normscore IS 'This is the rawscore but
    semi-normalized. Complete normalization to allow comparison of
    features generated by different programs would be nice but too
    difficult. Instead the normalization should strive to enforce the
    following semantics: * normscores are floating point numbers >= 0,
    * high normscores are better than low one. For most programs, it would be sufficient to make the normscore the same as this rawscore, providing these semantics are satisfied.';
COMMENT ON COLUMN analysisfeature.rawscore IS 'This is the native score generated by the program; for example, the bitscore generated by blast, sim4 or genscan scores. One should not assume that high is necessarily better than low.';

-- ================================================
-- TABLE: analysisfeatureprop
-- ================================================

CREATE TABLE analysisfeatureprop (
    analysisfeatureprop_id bigserial PRIMARY KEY,
    analysisfeature_id bigint NOT NULL REFERENCES analysisfeature(analysisfeature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED,
    type_id bigint NOT NULL REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED,
    value TEXT,
    rank int NOT NULL,
    CONSTRAINT analysisfeature_id_type_id_rank UNIQUE(analysisfeature_id, type_id, rank)
);
create index analysisfeatureprop_idx1 on analysisfeatureprop (analysisfeature_id);
create index analysisfeatureprop_idx2 on analysisfeatureprop (type_id);

-- ================================================
-- TABLE: analysis_dbxref
-- ================================================

create table analysis_dbxref (
  analysis_dbxref_id bigserial not null,
  analysis_id bigint not null,
  dbxref_id bigint not null,
  primary key (analysis_dbxref_id),
  is_current boolean not null default 'true',
  foreign key (analysis_id) references analysis (analysis_id) on delete cascade INITIALLY DEFERRED,
  foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED,
  constraint analysis_dbxref_c1 unique (analysis_id,dbxref_id)
);
create index analysis_dbxref_idx1 on analysis_dbxref (analysis_id);
create index analysis_dbxref_idx2 on analysis_dbxref (dbxref_id);

COMMENT ON TABLE analysis_dbxref IS 'Links an analysis to dbxrefs.';

COMMENT ON COLUMN analysis_dbxref.is_current IS 'True if this dbxref
is the most up to date accession in the corresponding db. Retired
accessions should set this field to false';


-- ================================================
-- TABLE: analysis_cvterm
-- ================================================

create table analysis_cvterm (
  analysis_cvterm_id bigserial not null,
  analysis_id bigint not null,
  cvterm_id bigint not null,
  is_not boolean not null default false,
  rank integer not null default 0,
  primary key (analysis_cvterm_id),
  foreign key (analysis_id) references analysis (analysis_id) on delete cascade INITIALLY DEFERRED,
  foreign key (cvterm_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
  constraint analysis_cvterm_c1 unique (analysis_id,cvterm_id,rank)
);
create index analysis_cvterm_idx1 on analysis_cvterm (analysis_id);
create index analysis_cvterm_idx2 on analysis_cvterm (cvterm_id);

COMMENT ON TABLE analysis_cvterm IS 'Associate a term from a cv with an analysis.';

COMMENT ON COLUMN analysis_cvterm.is_not IS 'If this is set to true, then this
annotation is interpreted as a NEGATIVE annotation - i.e. the analysis does
NOT have the specified term.';

-- ================================================
-- TABLE: analysis_relationship
-- ================================================

create table analysis_relationship (
  analysis_relationship_id bigserial not null,
  subject_id bigint not null,
  object_id bigint not null,
  type_id bigint not null,
  value text null,
  rank int not null default 0,
  primary key (analysis_relationship_id),
  foreign key (subject_id) references analysis (analysis_id) on delete cascade INITIALLY DEFERRED,
  foreign key (object_id) references analysis (analysis_id) on delete cascade INITIALLY DEFERRED,
  foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
  constraint analysis_relationship_c1 unique (subject_id,object_id,type_id,rank)
);
create index analysis_relationship_idx1 on analysis_relationship (subject_id);
create index analysis_relationship_idx2 on analysis_relationship (object_id);
create index analysis_relationship_idx3 on analysis_relationship (type_id);

COMMENT ON COLUMN analysis_relationship.subject_id IS 'analysis_relationship.subject_id i
s the subject of the subj-predicate-obj sentence.';

COMMENT ON COLUMN analysis_relationship.object_id IS 'analysis_relationship.object_id
is the object of the subj-predicate-obj sentence.';

COMMENT ON COLUMN analysis_relationship.type_id IS 'analysis_relationship.type_id
is relationship type between subject and object. This is a cvterm, typically
from the OBO relationship ontology, although other relationship types are allowed.';

COMMENT ON COLUMN analysis_relationship.rank IS 'analysis_relationship.rank is
the ordering of subject analysiss with respect to the object analysis may be
important where rank is used to order these; starts from zero.';

COMMENT ON COLUMN analysis_relationship.value IS 'analysis_relationship.value
is for additional notes or comments.';

-- ================================================
-- TABLE: analysis_pub
-- ================================================

create table analysis_pub (
    analysis_pub_id bigserial not null,
    primary key (analysis_pub_id),
    analysis_id bigint not null,
    foreign key (analysis_id) references analysis (analysis_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    constraint analysis_pub_c1 unique (analysis_id, pub_id)
);
create index analysis_pub_idx1 on analysis_pub (analysis_id);
create index analysis_pub_idx2 on analysis_pub (pub_id);

COMMENT ON TABLE analysis_pub IS 'Provenance. Linking table between analyses and the publications that mention them.';

-- $Id: phenotype.sql,v 1.6 2007-04-27 16:09:46 emmert Exp $
-- ==========================================
-- Chado phenotype module
--
-- 05-31-2011
-- added 'name' column to phenotype. non-unique human readable field.
--
-- =================================================================
-- Dependencies:
--
-- :import cvterm from cv
-- :import feature from sequence
-- =================================================================

-- ================================================
-- TABLE: phenotype
-- ================================================

CREATE TABLE phenotype (
    phenotype_id bigserial NOT NULL,
    primary key (phenotype_id),
    uniquename TEXT NOT NULL,
    name TEXT default null,
    observable_id bigint,
    FOREIGN KEY (observable_id) REFERENCES cvterm (cvterm_id) ON DELETE CASCADE,
    attr_id bigint,
    FOREIGN KEY (attr_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL,
    value TEXT,
    cvalue_id bigint,
    FOREIGN KEY (cvalue_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL,
    assay_id bigint,
    FOREIGN KEY (assay_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL,
    CONSTRAINT phenotype_c1 UNIQUE (uniquename)
);
CREATE INDEX phenotype_idx1 ON phenotype (cvalue_id);
CREATE INDEX phenotype_idx2 ON phenotype (observable_id);
CREATE INDEX phenotype_idx3 ON phenotype (attr_id);

COMMENT ON TABLE phenotype IS 'A phenotypic statement, or a single
atomic phenotypic observation, is a controlled sentence describing
observable effects of non-wild type function. E.g. Obs=eye, attribute=color, cvalue=red.';
COMMENT ON COLUMN phenotype.observable_id IS 'The entity: e.g. anatomy_part, biological_process.';
COMMENT ON COLUMN phenotype.attr_id IS 'Phenotypic attribute (quality, property, attribute, character) - drawn from PATO.';
COMMENT ON COLUMN phenotype.value IS 'Value of attribute - unconstrained free text. Used only if cvalue_id is not appropriate.';
COMMENT ON COLUMN phenotype.cvalue_id IS 'Phenotype attribute value (state).';
COMMENT ON COLUMN phenotype.assay_id IS 'Evidence type.';


-- ================================================
-- TABLE: phenotype_cvterm
-- ================================================

CREATE TABLE phenotype_cvterm (
    phenotype_cvterm_id bigserial NOT NULL,
    primary key (phenotype_cvterm_id),
    phenotype_id bigint NOT NULL,
    FOREIGN KEY (phenotype_id) REFERENCES phenotype (phenotype_id) ON DELETE CASCADE,
    cvterm_id bigint NOT NULL,
    FOREIGN KEY (cvterm_id) REFERENCES cvterm (cvterm_id) ON DELETE CASCADE,
    rank int not null default 0,
    CONSTRAINT phenotype_cvterm_c1 UNIQUE (phenotype_id, cvterm_id, rank)
);
CREATE INDEX phenotype_cvterm_idx1 ON phenotype_cvterm (phenotype_id);
CREATE INDEX phenotype_cvterm_idx2 ON phenotype_cvterm (cvterm_id);

COMMENT ON TABLE phenotype_cvterm IS 'phenotype to cvterm associations.';


-- ================================================
-- TABLE: feature_phenotype
-- ================================================

CREATE TABLE feature_phenotype (
    feature_phenotype_id bigserial NOT NULL,
    primary key (feature_phenotype_id),
    feature_id bigint NOT NULL,
    FOREIGN KEY (feature_id) REFERENCES feature (feature_id) ON DELETE CASCADE,
    phenotype_id bigint NOT NULL,
    FOREIGN KEY (phenotype_id) REFERENCES phenotype (phenotype_id) ON DELETE CASCADE,
    CONSTRAINT feature_phenotype_c1 UNIQUE (feature_id,phenotype_id)
);
CREATE INDEX feature_phenotype_idx1 ON feature_phenotype (feature_id);
CREATE INDEX feature_phenotype_idx2 ON feature_phenotype (phenotype_id);

COMMENT ON TABLE feature_phenotype IS 'Linking table between features and phenotypes.';


-- ================================================
-- TABLE: phenotypeprop
-- ================================================

create table phenotypeprop (
       phenotypeprop_id bigserial not null,
       primary key (phenotypeprop_id),
       phenotype_id bigint not null,
       foreign key (phenotype_id) references phenotype (phenotype_id) on delete cascade INITIALLY DEFERRED,
       type_id bigint not null,
       foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
       value text null,
       rank int not null default 0,
       constraint phenotypeprop_c1 unique (phenotype_id,type_id,rank)
);
create index phenotypeprop_idx1 on phenotypeprop (phenotype_id);
create index phenotypeprop_idx2 on phenotypeprop (type_id);

COMMENT ON TABLE phenotypeprop IS 'A phenotype can have any number of slot-value property tags attached to it. This is an alternative to hardcoding a list of columns in the relational schema, and is completely extensible. There is a unique constraint, phenotypeprop_c1, for the combination of phenotype_id, rank, and type_id. Multivalued property-value pairs must be differentiated by rank.';
-- $Id: genetic.sql,v 1.31 2008-08-25 19:53:14 scottcain Exp $
-- ==========================================
-- Chado genetics module
--
-- changes 2011-05-31
--   added type_id to genotype (can be null for backward compatibility)
--   added genotypeprop table
-- 2006-04-11
--   split out phenotype tables into phenotype module
--
-- redesigned 2003-10-28
--
-- changes 2003-11-10:
--   incorporating suggestions to make everything a gcontext; use
--   gcontext_relationship to make some gcontexts derivable from others. we
--   would incorporate environment this way - just add the environment
--   descriptors as properties of the child gcontext
--
-- changes 2004-06 (Documented by DE: 10-MAR-2005):
--   Many, including rename of gcontext to genotype,  split
--   phenstatement into phenstatement & phenotype, created environment
--
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
-- ============
-- DEPENDENCIES
-- ============
-- :import feature from sequence
-- :import phenotype from phenotype
-- :import cvterm from cv
-- :import pub from pub
-- :import dbxref from db
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-- ================================================
-- TABLE: genotype
-- ================================================
create table genotype (
    genotype_id bigserial not null,
    primary key (genotype_id),
    name text,
    uniquename text not null,
    description text,
    type_id bigint NOT NULL,
    FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE CASCADE,
    constraint genotype_c1 unique (uniquename)
);
create index genotype_idx1 on genotype(uniquename);
create index genotype_idx2 on genotype(name);

COMMENT ON TABLE genotype IS 'Genetic context. A genotype is defined by a collection of features, mutations, balancers, deficiencies, haplotype blocks, or engineered constructs.';

COMMENT ON COLUMN genotype.uniquename IS 'The unique name for a genotype;
typically derived from the features making up the genotype.';

COMMENT ON COLUMN genotype.name IS 'Optional alternative name for a genotype,
for display purposes.';

-- ===============================================
-- TABLE: feature_genotype
-- ================================================
create table feature_genotype (
    feature_genotype_id bigserial not null,
    primary key (feature_genotype_id),
    feature_id bigint not null,
    foreign key (feature_id) references feature (feature_id) on delete cascade,
    genotype_id bigint not null,
    foreign key (genotype_id) references genotype (genotype_id) on delete cascade,
    chromosome_id bigint,
    foreign key (chromosome_id) references feature (feature_id) on delete set null,
    rank int not null,
    cgroup    int not null,
    cvterm_id bigint not null,
    foreign key (cvterm_id) references cvterm (cvterm_id) on delete cascade,
    constraint feature_genotype_c1 unique (feature_id, genotype_id, cvterm_id, chromosome_id, rank, cgroup)
);
create index feature_genotype_idx1 on feature_genotype (feature_id);
create index feature_genotype_idx2 on feature_genotype (genotype_id);

COMMENT ON TABLE feature_genotype IS NULL;
COMMENT ON COLUMN feature_genotype.rank IS 'rank can be used for
n-ploid organisms or to preserve order.';
COMMENT ON COLUMN feature_genotype.cgroup IS 'Spatially distinguishable
group. group can be used for distinguishing the chromosomal groups,
for example (RNAi products and so on can be treated as different
groups, as they do not fall on a particular chromosome).';
COMMENT ON COLUMN feature_genotype.chromosome_id IS 'A feature of SO type "chromosome".';

-- ================================================
-- TABLE: environment
-- ================================================
create table environment (
    environment_id bigserial not NULL,
    primary key  (environment_id),
    uniquename text not null,
    description text,
    constraint environment_c1 unique (uniquename)
);
create index environment_idx1 on environment(uniquename);

COMMENT ON TABLE environment IS 'The environmental component of a phenotype description.';


-- ================================================
-- TABLE: environment_cvterm
-- ================================================
create table environment_cvterm (
    environment_cvterm_id bigserial not null,
    primary key  (environment_cvterm_id),
    environment_id bigint not null,
    foreign key (environment_id) references environment (environment_id) on delete cascade,
    cvterm_id bigint not null,
    foreign key (cvterm_id) references cvterm (cvterm_id) on delete cascade,
    constraint environment_cvterm_c1 unique (environment_id, cvterm_id)
);
create index environment_cvterm_idx1 on environment_cvterm (environment_id);
create index environment_cvterm_idx2 on environment_cvterm (cvterm_id);

COMMENT ON TABLE environment_cvterm IS NULL;

-- ================================================
-- TABLE: phenstatement
-- ================================================
CREATE TABLE phenstatement (
    phenstatement_id bigserial NOT NULL,
    primary key (phenstatement_id),
    genotype_id bigint NOT NULL,
    FOREIGN KEY (genotype_id) REFERENCES genotype (genotype_id) ON DELETE CASCADE,
    environment_id bigint NOT NULL,
    FOREIGN KEY (environment_id) REFERENCES environment (environment_id) ON DELETE CASCADE,
    phenotype_id bigint NOT NULL,
    FOREIGN KEY (phenotype_id) REFERENCES phenotype (phenotype_id) ON DELETE CASCADE,
    type_id bigint NOT NULL,
    FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE CASCADE,
    pub_id bigint NOT NULL,
    FOREIGN KEY (pub_id) REFERENCES pub (pub_id) ON DELETE CASCADE,
    CONSTRAINT phenstatement_c1 UNIQUE (genotype_id,phenotype_id,environment_id,type_id,pub_id)
);
CREATE INDEX phenstatement_idx1 ON phenstatement (genotype_id);
CREATE INDEX phenstatement_idx2 ON phenstatement (phenotype_id);

COMMENT ON TABLE phenstatement IS 'Phenotypes are things like "larval lethal".  Phenstatements are things like "dpp-1 is recessive larval lethal". So essentially phenstatement is a linking table expressing the relationship between genotype, environment, and phenotype.';

-- ================================================
-- TABLE: phendesc
-- ================================================
CREATE TABLE phendesc (
    phendesc_id bigserial NOT NULL,
    primary key (phendesc_id),
    genotype_id bigint NOT NULL,
    FOREIGN KEY (genotype_id) REFERENCES genotype (genotype_id) ON DELETE CASCADE,
    environment_id bigint NOT NULL,
    FOREIGN KEY (environment_id) REFERENCES environment ( environment_id) ON DELETE CASCADE,
    description TEXT NOT NULL,
    type_id bigint NOT NULL,
        FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE CASCADE,
    pub_id bigint NOT NULL,
    FOREIGN KEY (pub_id) REFERENCES pub (pub_id) ON DELETE CASCADE,
    CONSTRAINT phendesc_c1 UNIQUE (genotype_id,environment_id,type_id,pub_id)
);
CREATE INDEX phendesc_idx1 ON phendesc (genotype_id);
CREATE INDEX phendesc_idx2 ON phendesc (environment_id);
CREATE INDEX phendesc_idx3 ON phendesc (pub_id);

COMMENT ON TABLE phendesc IS 'A summary of a _set_ of phenotypic statements for any one gcontext made in any one publication.';

-- ================================================
-- TABLE: phenotype_comparison
-- ================================================
CREATE TABLE phenotype_comparison (
    phenotype_comparison_id bigserial NOT NULL,
    primary key (phenotype_comparison_id),
    genotype1_id bigint NOT NULL,
        FOREIGN KEY (genotype1_id) REFERENCES genotype (genotype_id) ON DELETE CASCADE,
    environment1_id bigint NOT NULL,
        FOREIGN KEY (environment1_id) REFERENCES environment (environment_id) ON DELETE CASCADE,
    genotype2_id bigint NOT NULL,
        FOREIGN KEY (genotype2_id) REFERENCES genotype (genotype_id) ON DELETE CASCADE,
    environment2_id bigint NOT NULL,
        FOREIGN KEY (environment2_id) REFERENCES environment (environment_id) ON DELETE CASCADE,
    phenotype1_id bigint NOT NULL,
        FOREIGN KEY (phenotype1_id) REFERENCES phenotype (phenotype_id) ON DELETE CASCADE,
    phenotype2_id bigint,
        FOREIGN KEY (phenotype2_id) REFERENCES phenotype (phenotype_id) ON DELETE CASCADE,
    pub_id bigint NOT NULL,
    FOREIGN KEY (pub_id) REFERENCES pub (pub_id) ON DELETE CASCADE,
    organism_id bigint NOT NULL,
    FOREIGN KEY (organism_id) REFERENCES organism (organism_id) ON DELETE CASCADE,
    CONSTRAINT phenotype_comparison_c1 UNIQUE (genotype1_id,environment1_id,genotype2_id,environment2_id,phenotype1_id,pub_id)
);
CREATE INDEX phenotype_comparison_idx1 on phenotype_comparison (genotype1_id);
CREATE INDEX phenotype_comparison_idx2 on phenotype_comparison (genotype2_id);
CREATE INDEX phenotype_comparison_idx4 on phenotype_comparison (pub_id);

COMMENT ON TABLE phenotype_comparison IS 'Comparison of phenotypes e.g., genotype1/environment1/phenotype1 "non-suppressible" with respect to genotype2/environment2/phenotype2.';

-- ================================================
-- TABLE: phenotype_comparison_cvterm
-- ================================================
CREATE TABLE phenotype_comparison_cvterm (
    phenotype_comparison_cvterm_id bigserial not null,
    primary key (phenotype_comparison_cvterm_id),
    phenotype_comparison_id bigint not null,
    FOREIGN KEY (phenotype_comparison_id) references phenotype_comparison (phenotype_comparison_id) on delete cascade,
    cvterm_id bigint not null,
    FOREIGN KEY (cvterm_id) references cvterm (cvterm_id) on delete cascade,
    pub_id bigint not null,
    FOREIGN KEY (pub_id) references pub (pub_id) on delete cascade,
    rank int not null default 0,
    CONSTRAINT phenotype_comparison_cvterm_c1 unique (phenotype_comparison_id, cvterm_id)
);
CREATE INDEX phenotype_comparison_cvterm_idx1 on phenotype_comparison_cvterm (phenotype_comparison_id);
CREATE INDEX  phenotype_comparison_cvterm_idx2 on phenotype_comparison_cvterm (cvterm_id);

-- ================================================
-- TABLE: genotypeprop
-- ================================================
create table genotypeprop (
    genotypeprop_id bigserial not null,
    primary key (genotypeprop_id),
    genotype_id bigint not null,
    foreign key (genotype_id) references genotype (genotype_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint genotypeprop_c1 unique (genotype_id,type_id,rank)
);
create index genotypeprop_idx1 on genotypeprop (genotype_id);
create index genotypeprop_idx2 on genotypeprop (type_id);
-- $Id: map.sql,v 1.14 2007-03-23 15:18:02 scottcain Exp $
-- ==========================================
-- Chado map module
--
-- =================================================================
-- Dependencies:
--
-- :import feature from sequence
-- :import cvterm from cv
-- :import pub from pub
-- :import contact from contact
-- :import dbxref from db
-- :import organism from organism
-- =================================================================

-- ================================================
-- TABLE: featuremap
-- ================================================

create table featuremap (
    featuremap_id bigserial not null,
    primary key (featuremap_id),
    name varchar(255),
    description text,
    unittype_id bigint null,
    foreign key (unittype_id) references cvterm (cvterm_id) on delete set null INITIALLY DEFERRED,
    constraint featuremap_c1 unique (name)
);

-- ================================================
-- TABLE: featurerange
-- ================================================

create table featurerange (
    featurerange_id bigserial not null,
    primary key (featurerange_id),
    featuremap_id bigint not null,
    foreign key (featuremap_id) references featuremap (featuremap_id) on delete cascade INITIALLY DEFERRED,
    feature_id bigint not null,
    foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    leftstartf_id bigint not null,
    foreign key (leftstartf_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    leftendf_id bigint,
    foreign key (leftendf_id) references feature (feature_id) on delete set null INITIALLY DEFERRED,
    rightstartf_id bigint,
    foreign key (rightstartf_id) references feature (feature_id) on delete set null INITIALLY DEFERRED,
    rightendf_id bigint not null,
    foreign key (rightendf_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    rangestr varchar(255)
);
create index featurerange_idx1 on featurerange (featuremap_id);
create index featurerange_idx2 on featurerange (feature_id);
create index featurerange_idx3 on featurerange (leftstartf_id);
create index featurerange_idx4 on featurerange (leftendf_id);
create index featurerange_idx5 on featurerange (rightstartf_id);
create index featurerange_idx6 on featurerange (rightendf_id);

COMMENT ON TABLE featurerange IS 'In cases where the start and end of a mapped feature is a range, leftendf and rightstartf are populated. leftstartf_id, leftendf_id, rightstartf_id, rightendf_id are the ids of features with respect to which the feature is being mapped. These may be cytological bands.';
COMMENT ON COLUMN featurerange.featuremap_id IS 'featuremap_id is the id of the feature being mapped.';


-- ================================================
-- TABLE: featurepos
-- ================================================

create table featurepos (
    featurepos_id bigserial not null,
    primary key (featurepos_id),
    featuremap_id bigint not null,
    foreign key (featuremap_id) references featuremap (featuremap_id) on delete cascade INITIALLY DEFERRED,
    feature_id bigint not null,
    foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    map_feature_id bigint not null,
    foreign key (map_feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    mappos float not null
);
create index featurepos_idx1 on featurepos (featuremap_id);
create index featurepos_idx2 on featurepos (feature_id);
create index featurepos_idx3 on featurepos (map_feature_id);

COMMENT ON COLUMN featurepos.map_feature_id IS 'map_feature_id
links to the feature (map) upon which the feature is being localized.';

-- ================================================
-- TABLE: featureposprop
-- ================================================

CREATE TABLE featureposprop (
    featureposprop_id bigserial primary key NOT NULL,
    featurepos_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL,
    CONSTRAINT featureposprop_c1 UNIQUE (featurepos_id, type_id, rank),
    FOREIGN KEY (featurepos_id) REFERENCES featurepos(featurepos_id) ON DELETE CASCADE,
    FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE
);

CREATE INDEX featureposprop_idx1 ON featureposprop USING btree (featurepos_id);
CREATE INDEX featureposprop_idx2 ON featureposprop USING btree (type_id);

COMMENT ON TABLE featureposprop IS 'Property or attribute of a featurepos record.';

-- ================================================
-- TABLE: featuremap_pub
-- ================================================

create table featuremap_pub (
    featuremap_pub_id bigserial not null,
    primary key (featuremap_pub_id),
    featuremap_id bigint not null,
    foreign key (featuremap_id) references featuremap (featuremap_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED
);
create index featuremap_pub_idx1 on featuremap_pub (featuremap_id);
create index featuremap_pub_idx2 on featuremap_pub (pub_id);

-- ================================================
-- TABLE: featuremapprop
-- ================================================

CREATE TABLE featuremapprop (
    featuremapprop_id bigserial primary key NOT NULL,
    featuremap_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL,
    FOREIGN KEY (featuremap_id) REFERENCES featuremap(featuremap_id) ON DELETE CASCADE,
    FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE,
    CONSTRAINT featuremapprop_c1 UNIQUE (featuremap_id, type_id, rank)
);
create index featuremapprop_idx1 on featuremapprop(featuremap_id);
create index featuremapprop_idx2 on featuremapprop(type_id);

COMMENT ON TABLE featuremapprop IS 'A featuremap can have any number of slot-value property
tags attached to it. This is an alternative to hardcoding a list of columns in the
relational schema, and is completely extensible.';

-- ================================================
-- TABLE: featuremap_contact
-- ================================================
CREATE TABLE featuremap_contact (
    featuremap_contact_id bigserial primary key NOT NULL,
    featuremap_id bigint NOT NULL,
    contact_id bigint NOT NULL,
    CONSTRAINT featuremap_contact_c1 UNIQUE (featuremap_id, contact_id),
    FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE,
    FOREIGN KEY (featuremap_id) REFERENCES featuremap(featuremap_id) ON DELETE CASCADE
);

CREATE INDEX featuremap_contact_idx1 ON featuremap_contact USING btree (featuremap_id);
CREATE INDEX featuremap_contact_idx2 ON featuremap_contact USING btree (contact_id);

COMMENT ON TABLE featuremap_contact IS 'Links contact(s) with a featuremap.  Used to
indicate a particular person or organization responsible for constrution of or
that can provide more information on a particular featuremap.';


-- ================================================
-- TABLE: featuremap_dbxref
-- ================================================

CREATE TABLE featuremap_dbxref (
    featuremap_dbxref_id bigserial primary key NOT NULL,
    featuremap_id bigint NOT NULL,
    dbxref_id bigint NOT NULL,
    is_current boolean DEFAULT true NOT NULL,
    FOREIGN KEY (featuremap_id) REFERENCES featuremap(featuremap_id) ON DELETE CASCADE,
    FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE
);

CREATE INDEX featuremap_dbxref_idx1 ON featuremap_dbxref USING btree (featuremap_id);
CREATE INDEX featuremap_dbxref_idx2 ON featuremap_dbxref USING btree (dbxref_id);

COMMENT ON TABLE feature_dbxref IS 'Links a feature to dbxrefs.';

COMMENT ON COLUMN feature_dbxref.is_current IS 'True if this secondary dbxref is
the most up to date accession in the corresponding db. Retired accessions
should set this field to false';


-- ================================================
-- TABLE: featuremap_organism
-- ================================================

CREATE TABLE featuremap_organism (
    featuremap_organism_id bigserial primary key NOT NULL,
    featuremap_id bigint NOT NULL,
    organism_id bigint NOT NULL,
    CONSTRAINT featuremap_organism_c1 UNIQUE (featuremap_id, organism_id),
    FOREIGN KEY (featuremap_id) REFERENCES featuremap(featuremap_id) ON DELETE CASCADE,
    FOREIGN KEY (organism_id) REFERENCES organism(organism_id) ON DELETE CASCADE
);

CREATE INDEX featuremap_organism_idx1 ON featuremap_organism USING btree (featuremap_id);
CREATE INDEX featuremap_organism_idx2 ON featuremap_organism USING btree (organism_id);

COMMENT ON TABLE featuremap_organism IS 'Links a featuremap to the organism(s) with which it is associated.';
-- $Id: phylogeny.sql,v 1.11 2007-04-12 17:00:30 briano Exp $
-- ==========================================
-- Chado phylogenetics module
--
-- Richard Bruskiewich
-- Chris Mungall
--
-- Initial design: 2004-05-27
--
-- ============
-- DEPENDENCIES
-- ============
-- :import feature from sequence
-- :import cvterm from cv
-- :import pub from pub
-- :import organism from organism
-- :import dbxref from db
-- :import analysis from companalysis
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-- ================================================
-- TABLE: phylotree
-- ================================================

create table phylotree (
	phylotree_id bigserial not null,
	primary key (phylotree_id),
   dbxref_id bigint not null,
   foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade,
	name varchar(255) null,
	type_id bigint,
	foreign key (type_id) references cvterm (cvterm_id) on delete cascade,
	analysis_id bigint null,
   foreign key (analysis_id) references analysis (analysis_id) on delete cascade,
	comment text null,
	unique(phylotree_id)
);
create index phylotree_idx1 on phylotree (phylotree_id);

COMMENT ON TABLE phylotree IS 'Global anchor for phylogenetic tree.';
COMMENT ON COLUMN phylotree.type_id IS 'Type: protein, nucleotide, taxonomy, for example. The type should be any SO type, or "taxonomy".';


-- ================================================
-- TABLE: phylotree_pub
-- ================================================

create table phylotree_pub (
       phylotree_pub_id bigserial not null,
       primary key (phylotree_pub_id),
       phylotree_id bigint not null,
       foreign key (phylotree_id) references phylotree (phylotree_id) on delete cascade,
       pub_id bigint not null,
       foreign key (pub_id) references pub (pub_id) on delete cascade,
       unique(phylotree_id, pub_id)
);
create index phylotree_pub_idx1 on phylotree_pub (phylotree_id);
create index phylotree_pub_idx2 on phylotree_pub (pub_id);

COMMENT ON TABLE phylotree_pub IS 'Tracks citations global to the tree e.g. multiple sequence alignment supporting tree construction.';

-- ================================================
-- TABLE: phylotreeprop
-- ================================================

create table phylotreeprop (
  phylotreeprop_id bigserial not null,
  phylotree_id bigint not null,
  type_id bigint not null,
  value text null,
  rank int not null default 0,
  primary key (phylotreeprop_id),
  foreign key (phylotree_id) references phylotree (phylotree_id) on delete cascade INITIALLY DEFERRED,
  foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
  constraint phylotreeprop_c1 unique (phylotree_id,type_id,rank)
);
create index phylotreeprop_idx1 on phylotreeprop (phylotree_id);
create index phylotreeprop_idx2 on phylotreeprop (type_id);

COMMENT ON TABLE phylotreeprop IS 'A phylotree can have any number of slot-value property
tags attached to it. This is an alternative to hardcoding a list of columns in the
relational schema, and is completely extensible.';

COMMENT ON COLUMN phylotreeprop.type_id IS 'The name of the property/slot is a cvterm.
The meaning of the property is defined in that cvterm.';

COMMENT ON COLUMN phylotreeprop.value IS 'The value of the property, represented as text.
Numeric values are converted to their text representation. This is less efficient than
using native database types, but is easier to query.';

COMMENT ON COLUMN phylotreeprop.rank IS 'Property-Value ordering. Any
phylotree can have multiple values for any particular property type
these are ordered in a list using rank, counting from zero. For
properties that are single-valued rather than multi-valued, the
default 0 value should be used';

COMMENT ON INDEX phylotreeprop_c1 IS 'For any one phylotree, multivalued
property-value pairs must be differentiated by rank.';

-- ================================================
-- TABLE: phylonode
-- ================================================

create table phylonode (
       phylonode_id bigserial not null,
       primary key (phylonode_id),
       phylotree_id bigint not null,
       foreign key (phylotree_id) references phylotree (phylotree_id) on delete cascade,
       parent_phylonode_id bigint null,
       foreign key (parent_phylonode_id) references phylonode (phylonode_id) on delete cascade,
       left_idx int not null,
       right_idx int not null,
       type_id bigint,
       foreign key(type_id) references cvterm (cvterm_id) on delete cascade,
       feature_id bigint,
       foreign key (feature_id) references feature (feature_id) on delete cascade,
       label varchar(255) null,
       distance float  null,
--       Bootstrap float null.
       unique(phylotree_id, left_idx),
       unique(phylotree_id, right_idx)
);

CREATE INDEX phylonode_parent_phylonode_id_idx ON phylonode (parent_phylonode_id);

COMMENT ON TABLE phylonode IS 'This is the most pervasive
       element in the phylogeny module, cataloging the "phylonodes" of
       tree graphs. Edges are implied by the parent_phylonode_id
       reflexive closure. For all nodes in a nested set implementation the left and right index will be *between* the parents left and right indexes.';
COMMENT ON COLUMN phylonode.feature_id IS 'Phylonodes can have optional features attached to them e.g. a protein or nucleotide sequence usually attached to a leaf of the phylotree for non-leaf nodes, the feature may be a feature that is an instance of SO:match; this feature is the alignment of all leaf features beneath it.';
COMMENT ON COLUMN phylonode.type_id IS 'Type: e.g. root, interior, leaf.';
COMMENT ON COLUMN phylonode.parent_phylonode_id IS 'Root phylonode can have null parent_phylonode_id value.';


-- ================================================
-- TABLE: phylonode_dbxref
-- ================================================

create table phylonode_dbxref (
       phylonode_dbxref_id bigserial not null,
       primary key (phylonode_dbxref_id),

       phylonode_id bigint not null,
       foreign key (phylonode_id) references phylonode (phylonode_id) on delete cascade,
       dbxref_id bigint not null,
       foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade,

       unique(phylonode_id,dbxref_id)
);
create index phylonode_dbxref_idx1 on phylonode_dbxref (phylonode_id);
create index phylonode_dbxref_idx2 on phylonode_dbxref (dbxref_id);

COMMENT ON TABLE phylonode_dbxref IS 'For example, for orthology, paralogy group identifiers; could also be used for NCBI taxonomy; for sequences, refer to phylonode_feature, feature associated dbxrefs.';


-- ================================================
-- TABLE: phylonode_pub
-- ================================================

create table phylonode_pub (
       phylonode_pub_id bigserial not null,
       primary key (phylonode_pub_id),

       phylonode_id bigint not null,
       foreign key (phylonode_id) references phylonode (phylonode_id) on delete cascade,
       pub_id bigint not null,
       foreign key (pub_id) references pub (pub_id) on delete cascade,

       unique(phylonode_id, pub_id)
);
create index phylonode_pub_idx1 on phylonode_pub (phylonode_id);
create index phylonode_pub_idx2 on phylonode_pub (pub_id);

-- ================================================
-- TABLE: phylonode_organism
-- ================================================

create table phylonode_organism (
       phylonode_organism_id bigserial not null,
       primary key (phylonode_organism_id),

       phylonode_id bigint not null,
       foreign key (phylonode_id) references phylonode (phylonode_id) on delete cascade,
       organism_id bigint not null,
       foreign key (organism_id) references organism (organism_id) on delete cascade,

       unique(phylonode_id)
);
create index phylonode_organism_idx1 on phylonode_organism (phylonode_id);
create index phylonode_organism_idx2 on phylonode_organism (organism_id);

COMMENT ON TABLE phylonode_organism IS 'This linking table should only be used for nodes in taxonomy trees; it provides a mapping between the node and an organism. One node can have zero or one organisms, one organism can have zero or more nodes (although typically it should only have one in the standard NCBI taxonomy tree).';
COMMENT ON COLUMN phylonode_organism.phylonode_id IS 'One phylonode cannot refer to >1 organism.';


-- ================================================
-- TABLE: phylonodeprop
-- ================================================

create table phylonodeprop (
       phylonodeprop_id bigserial not null,
       primary key (phylonodeprop_id),

       phylonode_id bigint not null,
       foreign key (phylonode_id) references phylonode (phylonode_id) on delete cascade,
       type_id bigint not null,
       foreign key (type_id) references cvterm (cvterm_id) on delete cascade,

       value text not null default '',
-- It is not clear how useful the rank concept is here, leave it in for now.
       rank int not null default 0,

       unique(phylonode_id, type_id, value, rank)
);
create index phylonodeprop_idx1 on phylonodeprop (phylonode_id);
create index phylonodeprop_idx2 on phylonodeprop (type_id);

COMMENT ON COLUMN phylonodeprop.type_id IS 'type_id could designate phylonode hierarchy relationships, for example: species taxonomy (kingdom, order, family, genus, species), "ortholog/paralog", "fold/superfold", etc.';

-- ================================================
-- TABLE: phylonode_relationship
-- ================================================

create table phylonode_relationship (
       phylonode_relationship_id bigserial not null,
       primary key (phylonode_relationship_id),
       subject_id bigint not null,
       foreign key (subject_id) references phylonode (phylonode_id) on delete cascade,
       object_id bigint not null,
       foreign key (object_id) references phylonode (phylonode_id) on delete cascade,
       type_id bigint not null,
       foreign key (type_id) references cvterm (cvterm_id) on delete cascade,
       rank int,
       phylotree_id bigint not null,
       foreign key (phylotree_id) references phylotree (phylotree_id) on delete cascade,
       unique(subject_id, object_id, type_id)
);
create index phylonode_relationship_idx1 on phylonode_relationship (subject_id);
create index phylonode_relationship_idx2 on phylonode_relationship (object_id);
create index phylonode_relationship_idx3 on phylonode_relationship (type_id);

COMMENT ON TABLE phylonode_relationship IS 'This is for
relationships that are not strictly hierarchical; for example,
horizontal gene transfer. Most phylogenetic trees are strictly
hierarchical, nevertheless it is here for completeness.';

-- $Id: expression.sql,v 1.14 2007-03-23 15:18:02 scottcain Exp $
-- ==========================================
-- Chado expression module
--
-- =================================================================
-- Dependencies:
--
-- :import feature from sequence
-- :import cvterm from cv
-- :import pub from pub
-- =================================================================


-- ================================================
-- TABLE: expression
-- ================================================

create table expression (
       expression_id bigserial not null,
       primary key (expression_id),
       uniquename text not null,
       md5checksum character(32),
       description text,
       constraint expression_c1 unique (uniquename)
);

COMMENT ON TABLE expression IS 'The expression table is essentially a bridge table.';


-- ================================================
-- TABLE: expression_cvterm
-- ================================================

create table expression_cvterm (
       expression_cvterm_id bigserial not null,
       primary key (expression_cvterm_id),
       expression_id bigint not null,
       foreign key (expression_id) references expression (expression_id) on delete cascade INITIALLY DEFERRED,
       cvterm_id bigint not null,
       foreign key (cvterm_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
       rank int not null default 0,
       cvterm_type_id bigint not null,
       foreign key (cvterm_type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
       constraint expression_cvterm_c1 unique (expression_id,cvterm_id,rank,cvterm_type_id)
);
create index expression_cvterm_idx1 on expression_cvterm (expression_id);
create index expression_cvterm_idx2 on expression_cvterm (cvterm_id);
create index expression_cvterm_idx3 on expression_cvterm (cvterm_type_id);


--================================================
-- TABLE: expression_cvtermprop
-- ================================================

create table expression_cvtermprop (
    expression_cvtermprop_id bigserial not null,
    primary key (expression_cvtermprop_id),
    expression_cvterm_id bigint not null,
    foreign key (expression_cvterm_id) references expression_cvterm (expression_cvterm_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint expression_cvtermprop_c1 unique (expression_cvterm_id,type_id,rank)
);
create index expression_cvtermprop_idx1 on expression_cvtermprop (expression_cvterm_id);
create index expression_cvtermprop_idx2 on expression_cvtermprop (type_id);

COMMENT ON TABLE expression_cvtermprop IS 'Extensible properties for
expression to cvterm associations. Examples: qualifiers.';

COMMENT ON COLUMN expression_cvtermprop.type_id IS 'The name of the
property/slot is a cvterm. The meaning of the property is defined in
that cvterm. For example, cvterms may come from the FlyBase miscellaneous cv.';

COMMENT ON COLUMN expression_cvtermprop.value IS 'The value of the
property, represented as text. Numeric values are converted to their
text representation. This is less efficient than using native database
types, but is easier to query.';

COMMENT ON COLUMN expression_cvtermprop.rank IS 'Property-Value
ordering. Any expression_cvterm can have multiple values for any particular
property type - these are ordered in a list using rank, counting from
zero. For properties that are single-valued rather than multi-valued,
the default 0 value should be used.';

-- ================================================
-- TABLE: expressionprop
-- ================================================

create table expressionprop (
    expressionprop_id bigserial not null,
    primary key (expressionprop_id),
    expression_id bigint not null,
    foreign key (expression_id) references expression (expression_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint expressionprop_c1 unique (expression_id,type_id,rank)
);
create index expressionprop_idx1 on expressionprop (expression_id);
create index expressionprop_idx2 on expressionprop (type_id);


-- ================================================
-- TABLE: expression_pub
-- ================================================

create table expression_pub (
       expression_pub_id bigserial not null,
       primary key (expression_pub_id),
       expression_id bigint not null,
       foreign key (expression_id) references expression (expression_id) on delete cascade INITIALLY DEFERRED,
       pub_id bigint not null,
       foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
       constraint expression_pub_c1 unique (expression_id,pub_id)
);
create index expression_pub_idx1 on expression_pub (expression_id);
create index expression_pub_idx2 on expression_pub (pub_id);


-- ================================================
-- TABLE: feature_expression
-- ================================================

create table feature_expression (
       feature_expression_id bigserial not null,
       primary key (feature_expression_id),
       expression_id bigint not null,
       foreign key (expression_id) references expression (expression_id) on delete cascade INITIALLY DEFERRED,
       feature_id bigint not null,
       foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
       pub_id bigint not null,
       foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
       constraint feature_expression_c1 unique (expression_id,feature_id,pub_id)
);
create index feature_expression_idx1 on feature_expression (expression_id);
create index feature_expression_idx2 on feature_expression (feature_id);
create index feature_expression_idx3 on feature_expression (pub_id);


-- ================================================
-- TABLE: feature_expressionprop
-- ================================================

create table feature_expressionprop (
       feature_expressionprop_id bigserial not null,
       primary key (feature_expressionprop_id),
       feature_expression_id bigint not null,
       foreign key (feature_expression_id) references feature_expression (feature_expression_id) on delete cascade INITIALLY DEFERRED,
       type_id bigint not null,
       foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
       value text null,
       rank int not null default 0,
       constraint feature_expressionprop_c1 unique (feature_expression_id,type_id,rank)
);
create index feature_expressionprop_idx1 on feature_expressionprop (feature_expression_id);
create index feature_expressionprop_idx2 on feature_expressionprop (type_id);

COMMENT ON TABLE feature_expressionprop IS 'Extensible properties for
feature_expression (comments, for example). Modeled on feature_cvtermprop.';


-- ================================================
-- TABLE: eimage
-- ================================================

create table eimage (
		eimage_id bigserial not null,
      primary key (eimage_id),
      eimage_data text,
      eimage_type varchar(255) not null,
      image_uri varchar(255)
);

COMMENT ON COLUMN eimage.eimage_data IS 'We expect images in eimage_data (e.g. JPEGs) to be uuencoded.';
COMMENT ON COLUMN eimage.eimage_type IS 'Describes the type of data in eimage_data.';


-- ================================================
-- TABLE: expression_image
-- ================================================

create table expression_image (
       expression_image_id bigserial not null,
       primary key (expression_image_id),
       expression_id bigint not null,
       foreign key (expression_id) references expression (expression_id) on delete cascade INITIALLY DEFERRED,
       eimage_id bigint not null,
       foreign key (eimage_id) references eimage (eimage_id) on delete cascade INITIALLY DEFERRED,
       constraint expression_image_c1 unique(expression_id,eimage_id)
);
create index expression_image_idx1 on expression_image (expression_id);
create index expression_image_idx2 on expression_image (eimage_id);
-- $Id: library.sql,v 1.10 2008-03-25 16:00:43 emmert Exp $
-- =================================================================
-- Dependencies:
--
-- :import feature from sequence
-- :import synonym from sequence
-- :import cvterm from cv
-- :import pub from pub
-- :import organism from organism
-- :import expression from expression
-- :import dbxref from db
-- :import contact from contact
-- =================================================================

-- ================================================
-- TABLE: library
-- ================================================

create table library (
    library_id bigserial not null,
    primary key (library_id),
    organism_id bigint not null,
    foreign key (organism_id) references organism (organism_id),
    name varchar(255),
    uniquename text not null,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id),
    is_obsolete int not null default 0,
    timeaccessioned timestamp not null default current_timestamp,
    timelastmodified timestamp not null default current_timestamp,
    constraint library_c1 unique (organism_id,uniquename,type_id)
);
create index library_name_ind1 on library(name);
create index library_idx1 on library (organism_id);
create index library_idx2 on library (type_id);
create index library_idx3 on library (uniquename);

COMMENT ON COLUMN library.type_id IS 'The type_id foreign key links to a controlled vocabulary of library types. Examples of this would be: "cDNA_library" or "genomic_library"';


-- ================================================
-- TABLE: library_synonym
-- ================================================

create table library_synonym (
    library_synonym_id bigserial not null,
    primary key (library_synonym_id),
    synonym_id bigint not null,
    foreign key (synonym_id) references synonym (synonym_id) on delete cascade INITIALLY DEFERRED,
    library_id bigint not null,
    foreign key (library_id) references library (library_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    is_current boolean not null default 'true',
    is_internal boolean not null default 'false',
    constraint library_synonym_c1 unique (synonym_id,library_id,pub_id)
);
create index library_synonym_idx1 on library_synonym (synonym_id);
create index library_synonym_idx2 on library_synonym (library_id);
create index library_synonym_idx3 on library_synonym (pub_id);

COMMENT ON TABLE library_synonym IS 'Linking table between library and synonym.';

COMMENT ON COLUMN library_synonym.is_current IS 'The is_current bit indicates whether the linked synonym is the current -official- symbol for the linked library.';

COMMENT ON COLUMN library_synonym.pub_id IS 'The pub_id link is for
relating the usage of a given synonym to the publication in which it was used.';

COMMENT ON COLUMN library_synonym.is_internal IS 'Typically a synonym
exists so that somebody querying the database with an obsolete name
can find the object they are looking for under its current name.  If
the synonym has been used publicly and deliberately (e.g. in a paper), it my also be listed in reports as a synonym.   If the synonym was not used deliberately (e.g., there was a typo which went public), then the is_internal bit may be set to "true" so that it is known that the synonym is "internal" and should be queryable but should not be listed in reports as a valid synonym.';


-- ================================================
-- TABLE: library_pub
-- ================================================

create table library_pub (
    library_pub_id bigserial not null,
    primary key (library_pub_id),
    library_id bigint not null,
    foreign key (library_id) references library (library_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    constraint library_pub_c1 unique (library_id,pub_id)
);
create index library_pub_idx1 on library_pub (library_id);
create index library_pub_idx2 on library_pub (pub_id);

COMMENT ON TABLE library_pub IS 'Attribution for a library.';


-- ================================================
-- TABLE: libraryprop
-- ================================================

create table libraryprop (
    libraryprop_id bigserial not null,
    primary key (libraryprop_id),
    library_id bigint not null,
    foreign key (library_id) references library (library_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id),
    value text null,
    rank int not null default 0,
    constraint libraryprop_c1 unique (library_id,type_id,rank)
);
create index libraryprop_idx1 on libraryprop (library_id);
create index libraryprop_idx2 on libraryprop (type_id);

COMMENT ON TABLE libraryprop IS 'Tag-value properties - follows standard chado model.';


-- ================================================
-- TABLE: libraryprop_pub
-- ================================================

create table libraryprop_pub (
    libraryprop_pub_id bigserial not null,
    primary key (libraryprop_pub_id),
    libraryprop_id bigint not null,
    foreign key (libraryprop_id) references libraryprop (libraryprop_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
    constraint libraryprop_pub_c1 unique (libraryprop_id,pub_id)
);
create index libraryprop_pub_idx1 on libraryprop_pub (libraryprop_id);
create index libraryprop_pub_idx2 on libraryprop_pub (pub_id);

COMMENT ON TABLE libraryprop_pub IS 'Attribution for libraryprop.';


-- ================================================
-- TABLE: library_cvterm
-- ================================================

create table library_cvterm (
    library_cvterm_id bigserial not null,
    primary key (library_cvterm_id),
    library_id bigint not null,
    foreign key (library_id) references library (library_id) on delete cascade INITIALLY DEFERRED,
    cvterm_id bigint not null,
    foreign key (cvterm_id) references cvterm (cvterm_id),
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id),
    constraint library_cvterm_c1 unique (library_id,cvterm_id,pub_id)
);
create index library_cvterm_idx1 on library_cvterm (library_id);
create index library_cvterm_idx2 on library_cvterm (cvterm_id);
create index library_cvterm_idx3 on library_cvterm (pub_id);

COMMENT ON TABLE library_cvterm IS 'The table library_cvterm links a library to controlled vocabularies which describe the library.  For instance, there might be a link to the anatomy cv for "head" or "testes" for a head or testes library.';


-- ================================================
-- TABLE: library_feature
-- ================================================

create table library_feature (
    library_feature_id bigserial not null,
    primary key (library_feature_id),
    library_id bigint not null,
    foreign key (library_id) references library (library_id) on delete cascade INITIALLY DEFERRED,
    feature_id bigint not null,
    foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
    constraint library_feature_c1 unique (library_id,feature_id)
);
create index library_feature_idx1 on library_feature (library_id);
create index library_feature_idx2 on library_feature (feature_id);

COMMENT ON TABLE library_feature IS 'library_feature links a library to the clones which are contained in the library.  Examples of such linked features might be "cDNA_clone" or  "genomic_clone".';


-- ================================================
-- TABLE: library_dbxref
-- ================================================

create table library_dbxref (
    library_dbxref_id bigserial not null,
    primary key (library_dbxref_id),
    library_id bigint not null,
    foreign key (library_id) references library (library_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id bigint not null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED,
    is_current boolean not null default 'true',
    constraint library_dbxref_c1 unique (library_id,dbxref_id)
);
create index library_dbxref_idx1 on library_dbxref (library_id);
create index library_dbxref_idx2 on library_dbxref (dbxref_id);

COMMENT ON TABLE library_dbxref IS 'Links a library to dbxrefs.';


-- ================================================
-- TABLE: library_expression
-- ================================================

create table library_expression (
    library_expression_id bigserial not null,
    primary key (library_expression_id),
    library_id bigint not null,
    foreign key (library_id) references library (library_id) on delete cascade INITIALLY DEFERRED,
    expression_id bigint not null,
    foreign key (expression_id) references expression (expression_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id),
    constraint library_expression_c1 unique (library_id,expression_id)
);
create index library_expression_idx1 on library_expression (library_id);
create index library_expression_idx2 on library_expression (expression_id);
create index library_expression_idx3 on library_expression (pub_id);

COMMENT ON TABLE library_expression IS 'Links a library to expression statements.';


-- ================================================
-- TABLE: library_expressionprop
-- ================================================

create table library_expressionprop (
    library_expressionprop_id bigserial not null,
    primary key (library_expressionprop_id),
    library_expression_id bigint not null,
    foreign key (library_expression_id) references library_expression (library_expression_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id),
    value text null,
    rank int not null default 0,
    constraint library_expressionprop_c1 unique (library_expression_id,type_id,rank)
);
create index library_expressionprop_idx1 on library_expressionprop (library_expression_id);
create index library_expressionprop_idx2 on library_expressionprop (type_id);

COMMENT ON TABLE library_expressionprop IS 'Attributes of a library_expression relationship.';


-- ================================================
-- TABLE: library_featureprop
-- ================================================

create table library_featureprop (
    library_featureprop_id bigserial not null,
    primary key (library_featureprop_id),
    library_feature_id bigint not null,
    foreign key (library_feature_id) references library_feature (library_feature_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id),
    value text null,
    rank int not null default 0,
    constraint library_featureprop_c1 unique (library_feature_id,type_id,rank)
);
create index library_featureprop_idx1 on library_featureprop (library_feature_id);
create index library_featureprop_idx2 on library_featureprop (type_id);

COMMENT ON TABLE library_featureprop IS 'Attributes of a library_feature relationship.';

-- ================================================
-- TABLE: library_relationship
-- ================================================

create table library_relationship (
    library_relationship_id bigserial not null,
    primary key (library_relationship_id),
    subject_id bigint not null,
    foreign key (subject_id) references library (library_id) on delete cascade INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references library (library_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id),
    constraint library_relationship_c1 unique (subject_id,object_id,type_id)
);
create index library_relationship_idx1 on library_relationship (subject_id);
create index library_relationship_idx2 on library_relationship (object_id);
create index library_relationship_idx3 on library_relationship (type_id);

COMMENT ON TABLE library_relationship IS 'Relationships between libraries.';


-- ================================================
-- TABLE: library_relationship_pub
-- ================================================

create table library_relationship_pub (
    library_relationship_pub_id bigserial not null,
    primary key (library_relationship_pub_id),
    library_relationship_id bigint not null,
    foreign key (library_relationship_id) references library_relationship (library_relationship_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint not null,
    foreign key (pub_id) references pub (pub_id),
    constraint library_relationship_pub_c1 unique (library_relationship_id,pub_id)
);
create index library_relationship_pub_idx1 on library_relationship_pub (library_relationship_id);
create index library_relationship_pub_idx2 on library_relationship_pub (pub_id);

COMMENT ON TABLE library_relationship_pub IS 'Provenance of library_relationship.';


-- ================================================
-- TABLE: library_contact
-- ================================================

CREATE TABLE library_contact (
    library_contact_id bigserial primary key NOT NULL,
    library_id bigint NOT NULL,
    contact_id bigint NOT NULL,
    CONSTRAINT library_contact_c1 UNIQUE (library_id, contact_id),
    FOREIGN KEY (library_id) REFERENCES library(library_id) ON DELETE CASCADE,
    FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE
);

CREATE INDEX library_contact_idx1 ON library USING btree (library_id);
CREATE INDEX library_contact_idx2 ON contact USING btree (contact_id);

COMMENT ON TABLE library_contact IS 'Links contact(s) with a library.  Used to indicate a particular person or organization responsible for creation of or that can provide more information on a particular library.';

-- $Id: stock.sql,v 1.7 2007-03-23 15:18:03 scottcain Exp $
-- ==========================================
-- Chado stock module
--
-- DEPENDENCIES
-- ============
-- :import cvterm from cv
-- :import pub from pub
-- :import dbxref from db
-- :import organism from organism
-- :import genotype from genetic
-- :import contact from contact
-- :import feature from sequence
-- :import featuremap from map
-- ================================================
-- TABLE: stock
-- ================================================

create table stock (
       stock_id bigserial not null,
       primary key (stock_id),
       dbxref_id bigint,
       foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
       organism_id bigint,
       foreign key (organism_id) references organism (organism_id) on delete cascade INITIALLY DEFERRED,
       name varchar(255),
       uniquename text not null,
       description text,
       type_id bigint not null,
       foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
       is_obsolete boolean not null default 'false',
       constraint stock_c1 unique (organism_id,uniquename,type_id)
);
create index stock_name_ind1 on stock (name);
create index stock_idx1 on stock (dbxref_id);
create index stock_idx2 on stock (organism_id);
create index stock_idx3 on stock (type_id);
create index stock_idx4 on stock (uniquename);

COMMENT ON TABLE stock IS 'Any stock can be globally identified by the
combination of organism, uniquename and stock type. A stock is the physical entities, either living or preserved, held by collections. Stocks belong to a collection; they have IDs, type, organism, description and may have a genotype.';
COMMENT ON COLUMN stock.dbxref_id IS 'The dbxref_id is an optional primary stable identifier for this stock. Secondary indentifiers and external dbxrefs go in table: stock_dbxref.';
COMMENT ON COLUMN stock.organism_id IS 'The organism_id is the organism to which the stock belongs. This column should only be left blank if the organism cannot be determined.';
COMMENT ON COLUMN stock.type_id IS 'The type_id foreign key links to a controlled vocabulary of stock types. The would include living stock, genomic DNA, preserved specimen. Secondary cvterms for stocks would go in stock_cvterm.';
COMMENT ON COLUMN stock.description IS 'The description is the genetic description provided in the stock list.';
COMMENT ON COLUMN stock.name IS 'The name is a human-readable local name for a stock.';


-- ================================================
-- TABLE: stock_pub
-- ================================================

create table stock_pub (
       stock_pub_id bigserial not null,
       primary key (stock_pub_id),
       stock_id bigint not null,
       foreign key (stock_id) references stock (stock_id)  on delete cascade INITIALLY DEFERRED,
       pub_id bigint not null,
       foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
       constraint stock_pub_c1 unique (stock_id,pub_id)
);
create index stock_pub_idx1 on stock_pub (stock_id);
create index stock_pub_idx2 on stock_pub (pub_id);

COMMENT ON TABLE stock_pub IS 'Provenance. Linking table between stocks and, for example, a stocklist computer file.';


-- ================================================
-- TABLE: stockprop
-- ================================================

create table stockprop (
       stockprop_id bigserial not null,
       primary key (stockprop_id),
       stock_id bigint not null,
       foreign key (stock_id) references stock (stock_id) on delete cascade INITIALLY DEFERRED,
       type_id bigint not null,
       foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
       value text null,
       rank int not null default 0,
       constraint stockprop_c1 unique (stock_id,type_id,rank)
);
create index stockprop_idx1 on stockprop (stock_id);
create index stockprop_idx2 on stockprop (type_id);

COMMENT ON TABLE stockprop IS 'A stock can have any number of
slot-value property tags attached to it. This is an alternative to
hardcoding a list of columns in the relational schema, and is
completely extensible. There is a unique constraint, stockprop_c1, for
the combination of stock_id, rank, and type_id. Multivalued property-value pairs must be differentiated by rank.';


-- ================================================
-- TABLE: stockprop_pub
-- ================================================

create table stockprop_pub (
     stockprop_pub_id bigserial not null,
     primary key (stockprop_pub_id),
     stockprop_id bigint not null,
     foreign key (stockprop_id) references stockprop (stockprop_id) on delete cascade INITIALLY DEFERRED,
     pub_id bigint not null,
     foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
     constraint stockprop_pub_c1 unique (stockprop_id,pub_id)
);
create index stockprop_pub_idx1 on stockprop_pub (stockprop_id);
create index stockprop_pub_idx2 on stockprop_pub (pub_id);

COMMENT ON TABLE stockprop_pub IS 'Provenance. Any stockprop assignment can optionally be supported by a publication.';


-- ================================================
-- TABLE: stock_relationship
-- ================================================

create table stock_relationship (
       stock_relationship_id bigserial not null,
       primary key (stock_relationship_id),
       subject_id bigint not null,
       foreign key (subject_id) references stock (stock_id) on delete cascade INITIALLY DEFERRED,
       object_id bigint not null,
       foreign key (object_id) references stock (stock_id) on delete cascade INITIALLY DEFERRED,
       type_id bigint not null,
       foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
       value text null,
       rank int not null default 0,
       constraint stock_relationship_c1 unique (subject_id,object_id,type_id,rank)
);
create index stock_relationship_idx1 on stock_relationship (subject_id);
create index stock_relationship_idx2 on stock_relationship (object_id);
create index stock_relationship_idx3 on stock_relationship (type_id);

COMMENT ON COLUMN stock_relationship.subject_id IS 'stock_relationship.subject_id is the subject of the subj-predicate-obj sentence. This is typically the substock.';
COMMENT ON COLUMN stock_relationship.object_id IS 'stock_relationship.object_id is the object of the subj-predicate-obj sentence. This is typically the container stock.';
COMMENT ON COLUMN stock_relationship.type_id IS 'stock_relationship.type_id is relationship type between subject and object. This is a cvterm, typically from the OBO relationship ontology, although other relationship types are allowed.';
COMMENT ON COLUMN stock_relationship.rank IS 'stock_relationship.rank is the ordering of subject stocks with respect to the object stock may be important where rank is used to order these; starts from zero.';
COMMENT ON COLUMN stock_relationship.value IS 'stock_relationship.value is for additional notes or comments.';



-- ================================================
-- TABLE: stock_relationship_cvterm
-- ================================================

CREATE TABLE stock_relationship_cvterm (
	stock_relationship_cvterm_id bigserial NOT NULL,
	PRIMARY KEY (stock_relationship_cvterm_id),
	stock_relationship_id bigint NOT NULL,
	FOREIGN KEY (stock_relationship_id) references stock_relationship (stock_relationship_id) ON DELETE CASCADE INITIALLY DEFERRED,
	cvterm_id bigint NOT NULL,
	FOREIGN KEY (cvterm_id) REFERENCES cvterm (cvterm_id) ON DELETE RESTRICT,
	pub_id bigint,
	FOREIGN KEY (pub_id) REFERENCES pub (pub_id) ON DELETE RESTRICT
);
COMMENT ON TABLE stock_relationship_cvterm is 'For germplasm maintenance and pedigree data, stock_relationship. type_id will record cvterms such as "is a female parent of", "a parent for mutation", "is a group_id of", "is a source_id of", etc The cvterms for higher categories such as "generative", "derivative" or "maintenance" can be stored in table stock_relationship_cvterm';


-- ================================================
-- TABLE: stock_relationship_pub
-- ================================================

create table stock_relationship_pub (
      stock_relationship_pub_id bigserial not null,
      primary key (stock_relationship_pub_id),
      stock_relationship_id bigint not null,
      foreign key (stock_relationship_id) references stock_relationship (stock_relationship_id) on delete cascade INITIALLY DEFERRED,
      pub_id bigint not null,
      foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
      constraint stock_relationship_pub_c1 unique (stock_relationship_id,pub_id)
);
create index stock_relationship_pub_idx1 on stock_relationship_pub (stock_relationship_id);
create index stock_relationship_pub_idx2 on stock_relationship_pub (pub_id);

COMMENT ON TABLE stock_relationship_pub IS 'Provenance. Attach optional evidence to a stock_relationship in the form of a publication.';


-- ================================================
-- TABLE: stock_dbxref
-- ================================================

create table stock_dbxref (
     stock_dbxref_id bigserial not null,
     primary key (stock_dbxref_id),
     stock_id bigint not null,
     foreign key (stock_id) references stock (stock_id) on delete cascade INITIALLY DEFERRED,
     dbxref_id bigint not null,
     foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED,
     is_current boolean not null default 'true',
     constraint stock_dbxref_c1 unique (stock_id,dbxref_id)
);
create index stock_dbxref_idx1 on stock_dbxref (stock_id);
create index stock_dbxref_idx2 on stock_dbxref (dbxref_id);

COMMENT ON TABLE stock_dbxref IS 'stock_dbxref links a stock to dbxrefs. This is for secondary identifiers; primary identifiers should use stock.dbxref_id.';
COMMENT ON COLUMN stock_dbxref.is_current IS 'The is_current boolean indicates whether the linked dbxref is the current -official- dbxref for the linked stock.';


-- ================================================
-- TABLE: stock_cvterm
-- ================================================

create table stock_cvterm (
     stock_cvterm_id bigserial not null,
     primary key (stock_cvterm_id),
     stock_id bigint not null,
     foreign key (stock_id) references stock (stock_id) on delete cascade INITIALLY DEFERRED,
     cvterm_id bigint not null,
     foreign key (cvterm_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
     pub_id bigint not null,
     foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
     is_not boolean not null default false,
     rank int not null default 0,
     constraint stock_cvterm_c1 unique (stock_id,cvterm_id,pub_id,rank)
 );
create index stock_cvterm_idx1 on stock_cvterm (stock_id);
create index stock_cvterm_idx2 on stock_cvterm (cvterm_id);
create index stock_cvterm_idx3 on stock_cvterm (pub_id);

COMMENT ON TABLE stock_cvterm IS 'stock_cvterm links a stock to cvterms. This is for secondary cvterms; primary cvterms should use stock.type_id.';


-- ================================================
-- TABLE: stock_cvtermprop
-- ================================================

create table stock_cvtermprop (
    stock_cvtermprop_id bigserial not null,
    primary key (stock_cvtermprop_id),
    stock_cvterm_id bigint not null,
    foreign key (stock_cvterm_id) references stock_cvterm (stock_cvterm_id) on delete cascade,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint stock_cvtermprop_c1 unique (stock_cvterm_id,type_id,rank)
);
create index stock_cvtermprop_idx1 on stock_cvtermprop (stock_cvterm_id);
create index stock_cvtermprop_idx2 on stock_cvtermprop (type_id);

COMMENT ON TABLE stock_cvtermprop IS 'Extensible properties for
stock to cvterm associations. Examples: GO evidence codes;
qualifiers; metadata such as the date on which the entry was curated
and the source of the association. See the stockprop table for
meanings of type_id, value and rank.';

COMMENT ON COLUMN stock_cvtermprop.type_id IS 'The name of the
property/slot is a cvterm. The meaning of the property is defined in
that cvterm. cvterms may come from the OBO evidence code cv.';

COMMENT ON COLUMN stock_cvtermprop.value IS 'The value of the
property, represented as text. Numeric values are converted to their
text representation. This is less efficient than using native database
types, but is easier to query.';

COMMENT ON COLUMN stock_cvtermprop.rank IS 'Property-Value
ordering. Any stock_cvterm can have multiple values for any particular
property type - these are ordered in a list using rank, counting from
zero. For properties that are single-valued rather than multi-valued,
the default 0 value should be used.';


-- ================================================
-- TABLE: stock_genotype
-- ================================================

create table stock_genotype (
       stock_genotype_id bigserial not null,
       primary key (stock_genotype_id),
       stock_id bigint not null,
       foreign key (stock_id) references stock (stock_id) on delete cascade,
       genotype_id bigint not null,
       foreign key (genotype_id) references genotype (genotype_id) on delete cascade,
       constraint stock_genotype_c1 unique (stock_id, genotype_id)
);
create index stock_genotype_idx1 on stock_genotype (stock_id);
create index stock_genotype_idx2 on stock_genotype (genotype_id);

COMMENT ON TABLE stock_genotype IS 'Simple table linking a stock to
a genotype. Features with genotypes can be linked to stocks thru feature_genotype -> genotype -> stock_genotype -> stock.';


-- ================================================
-- TABLE: stockcollection
-- ================================================

create table stockcollection (
	stockcollection_id bigserial not null,
        primary key (stockcollection_id),
	type_id bigint not null,
        foreign key (type_id) references cvterm (cvterm_id) on delete cascade,
        contact_id bigint null,
        foreign key (contact_id) references contact (contact_id) on delete set null INITIALLY DEFERRED,
	name varchar(255),
	uniquename text not null,
	constraint stockcollection_c1 unique (uniquename,type_id)
);
create index stockcollection_name_ind1 on stockcollection (name);
create index stockcollection_idx1 on stockcollection (contact_id);
create index stockcollection_idx2 on stockcollection (type_id);
create index stockcollection_idx3 on stockcollection (uniquename);

COMMENT ON TABLE stockcollection IS 'The lab or stock center distributing the stocks in their collection.';
COMMENT ON COLUMN stockcollection.uniquename IS 'uniqename is the value of the collection cv.';
COMMENT ON COLUMN stockcollection.type_id IS 'type_id is the collection type cv.';
COMMENT ON COLUMN stockcollection.name IS 'name is the collection.';
COMMENT ON COLUMN stockcollection.contact_id IS 'contact_id links to the contact information for the collection.';


-- ================================================
-- TABLE: stockcollectionprop
-- ================================================

create table stockcollectionprop (
    stockcollectionprop_id bigserial not null,
    primary key (stockcollectionprop_id),
    stockcollection_id bigint not null,
    foreign key (stockcollection_id) references stockcollection (stockcollection_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id),
    value text null,
    rank int not null default 0,
    constraint stockcollectionprop_c1 unique (stockcollection_id,type_id,rank)
);
create index stockcollectionprop_idx1 on stockcollectionprop (stockcollection_id);
create index stockcollectionprop_idx2 on stockcollectionprop (type_id);

COMMENT ON TABLE stockcollectionprop IS 'The table stockcollectionprop
contains the value of the stock collection such as website/email URLs;
the value of the stock collection order URLs.';
COMMENT ON COLUMN stockcollectionprop.type_id IS 'The cv for the type_id is "stockcollection property type".';


-- ================================================
-- TABLE: stockcollection_stock
-- ================================================

create table stockcollection_stock (
    stockcollection_stock_id bigserial not null,
    primary key (stockcollection_stock_id),
    stockcollection_id bigint not null,
    foreign key (stockcollection_id) references stockcollection (stockcollection_id) on delete cascade INITIALLY DEFERRED,
    stock_id bigint not null,
    foreign key (stock_id) references stock (stock_id) on delete cascade INITIALLY DEFERRED,
    constraint stockcollection_stock_c1 unique (stockcollection_id,stock_id)
);
create index stockcollection_stock_idx1 on stockcollection_stock (stockcollection_id);
create index stockcollection_stock_idx2 on stockcollection_stock (stock_id);

COMMENT ON TABLE stockcollection_stock IS 'stockcollection_stock links
a stock collection to the stocks which are contained in the collection.';



-- ================================================
-- TABLE: stock_dbxrefprop
-- ================================================

create table stock_dbxrefprop (
       stock_dbxrefprop_id bigserial not null,
       primary key (stock_dbxrefprop_id),
       stock_dbxref_id bigint not null,
       foreign key (stock_dbxref_id) references stock_dbxref (stock_dbxref_id) on delete cascade INITIALLY DEFERRED,
       type_id bigint not null,
       foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
       value text null,
       rank int not null default 0,
       constraint stock_dbxrefprop_c1 unique (stock_dbxref_id,type_id,rank)
);
create index stock_dbxrefprop_idx1 on stock_dbxrefprop (stock_dbxref_id);
create index stock_dbxrefprop_idx2 on stock_dbxrefprop (type_id);

COMMENT ON TABLE stock_dbxrefprop IS 'A stock_dbxref can have any number of
slot-value property tags attached to it. This is useful for storing properties related to dbxref annotations of stocks, such as evidence codes, and references, and metadata, such as create/modify dates. This is an alternative to
hardcoding a list of columns in the relational schema, and is
completely extensible. There is a unique constraint, stock_dbxrefprop_c1, for
the combination of stock_dbxref_id, rank, and type_id. Multivalued property-value pairs must be differentiated by rank.';

-- ================================================
-- TABLE: stockcollection_db
-- ================================================

CREATE TABLE stockcollection_db (
    stockcollection_db_id bigserial primary key NOT NULL,
    stockcollection_id bigint NOT NULL,
    db_id bigint NOT NULL,
    CONSTRAINT stockcollection_db_c1 UNIQUE (stockcollection_id, db_id),
    FOREIGN KEY (db_id) REFERENCES db(db_id) ON DELETE CASCADE,
    FOREIGN KEY (stockcollection_id) REFERENCES stockcollection(stockcollection_id) ON DELETE CASCADE
);

CREATE INDEX stockcollection_db_idx1 ON stockcollection_db USING btree (stockcollection_id);
CREATE INDEX stockcollection_db_idx2 ON stockcollection_db USING btree (db_id);

COMMENT ON TABLE stockcollection_db IS 'Stock collections may be respresented
by an external online database. This table associates a stock collection with
a database where its member stocks can be found. Individual stock that are part
of this collction should have entries in the stock_dbxref table with the same
db_id record';


-- ================================================
-- TABLE: stock_feature
-- ================================================

CREATE TABLE stock_feature (
  stock_feature_id bigserial NOT NULL,
  feature_id bigint NOT NULL,
  stock_id bigint NOT NULL,
  type_id bigint NOT NULL,
  rank INT NOT NULL DEFAULT 0,
  PRIMARY KEY (stock_feature_id),
  FOREIGN KEY (feature_id) REFERENCES feature (feature_id) ON DELETE CASCADE INITIALLY DEFERRED,
  FOREIGN KEY (stock_id) REFERENCES stock (stock_id) ON DELETE CASCADE INITIALLY DEFERRED,
  FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE CASCADE INITIALLY DEFERRED,
  CONSTRAINT stock_feature_c1 UNIQUE (feature_id, stock_id, type_id, rank)
);
create index stock_feature_idx1 on stock_feature (stock_feature_id);
create index stock_feature_idx2 on stock_feature (feature_id);
create index stock_feature_idx3 on stock_feature (stock_id);
create index stock_feature_idx4 on stock_feature (type_id);

COMMENT ON TABLE stock_feature  IS 'Links a stock to a feature.';


-- ================================================
-- TABLE: stock_featuremap
-- ================================================

CREATE TABLE stock_featuremap (
  stock_featuremap_id bigserial NOT NULL,
  featuremap_id bigint NOT NULL,
  stock_id bigint NOT NULL,
  type_id bigint,
  PRIMARY KEY (stock_featuremap_id),
  FOREIGN KEY (featuremap_id) REFERENCES featuremap (featuremap_id) ON DELETE CASCADE INITIALLY DEFERRED,
  FOREIGN KEY (stock_id) REFERENCES stock (stock_id)  ON DELETE CASCADE INITIALLY DEFERRED,
  FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE CASCADE INITIALLY DEFERRED,
  CONSTRAINT stock_featuremap_c1 UNIQUE (featuremap_id, stock_id, type_id)
);

create index stock_featuremap_idx1 on stock_featuremap (featuremap_id);
create index stock_featuremap_idx2 on stock_featuremap (stock_id);
create index stock_featuremap_idx3 on stock_featuremap (type_id);

COMMENT ON TABLE stock_featuremap  IS 'Links a featuremap to a stock.';


-- ================================================
-- TABLE: stock_library
-- ================================================
CREATE TABLE stock_library (
    stock_library_id bigserial primary key NOT NULL,
    library_id bigint NOT NULL,
    stock_id bigint NOT NULL,
    CONSTRAINT stock_library_c1 UNIQUE (library_id, stock_id),
    FOREIGN KEY (library_id) REFERENCES library(library_id) ON DELETE CASCADE,
    FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE
);

CREATE INDEX stock_library_idx1 ON stock_library USING btree (library_id);
CREATE INDEX stock_library_idx2 ON stock_library USING btree (stock_id);

COMMENT ON TABLE stock_library IS 'Links a stock with a library.';

-- ==========================================
-- Chado project module. Used primarily by other Chado modules to
-- group experiments, stocks, and so forth that are associated with
-- eachother administratively or organizationally.
--
-- =================================================================
-- Dependencies:
--
-- :import cvterm from cv
-- :import pub from pub
-- :import contact from contact
-- :import dbxref from db
-- :import analysis from companalysis
-- :import feature from sequence
-- :import stock from stock
-- =================================================================


-- ================================================
-- TABLE: project
-- ================================================

create table project (
    project_id bigserial not null,
    primary key (project_id),
    name varchar(255) not null,
    description text,
    constraint project_c1 unique (name)
);

COMMENT ON TABLE project IS
'A project is some kind of planned endeavor.  Used primarily by other
Chado modules to group experiments, stocks, and so forth that are
associated with eachother administratively or organizationally.';

-- ================================================
-- TABLE: projectprop
-- ================================================

CREATE TABLE projectprop (
	projectprop_id bigserial NOT NULL,
	PRIMARY KEY (projectprop_id),
	project_id bigint NOT NULL,
	FOREIGN KEY (project_id) REFERENCES project (project_id) ON DELETE CASCADE,
	type_id bigint NOT NULL,
	FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE CASCADE,
	value text,
	rank int not null default 0,
	CONSTRAINT projectprop_c1 UNIQUE (project_id, type_id, rank)
);
COMMENT ON TABLE project IS
'Standard Chado flexible property table for projects.';

-- ================================================
-- TABLE: project_relationship
-- ================================================

CREATE TABLE project_relationship (
	project_relationship_id bigserial NOT NULL,
	PRIMARY KEY (project_relationship_id),
	subject_project_id bigint NOT NULL,
	FOREIGN KEY (subject_project_id) REFERENCES project (project_id) ON DELETE CASCADE,
	object_project_id bigint NOT NULL,
	FOREIGN KEY (object_project_id) REFERENCES project (project_id) ON DELETE CASCADE,
	type_id bigint NOT NULL,
	FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE RESTRICT,
	CONSTRAINT project_relationship_c1 UNIQUE (subject_project_id, object_project_id, type_id)
);

COMMENT ON TABLE project_relationship IS
'Linking table for relating projects to each other.  For example, a
given project could be composed of several smaller subprojects';

COMMENT ON COLUMN project_relationship.type_id IS
'The cvterm type of the relationship being stated, such as "part of".';

-- ================================================
-- TABLE: project_pub
-- ================================================

create table project_pub (
       project_pub_id bigserial not null,
       primary key (project_pub_id),
       project_id bigint not null,
       foreign key (project_id) references project (project_id) on delete cascade INITIALLY DEFERRED,
       pub_id bigint not null,
       foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
       constraint project_pub_c1 unique (project_id,pub_id)
);
create index project_pub_idx1 on project_pub (project_id);
create index project_pub_idx2 on project_pub (pub_id);

COMMENT ON TABLE project_pub IS 'Linking table for associating projects and publications.';

-- ================================================
-- TABLE: project_contact
-- ================================================

create table project_contact (
       project_contact_id bigserial not null,
       primary key (project_contact_id),
       project_id bigint not null,
       foreign key (project_id) references project (project_id) on delete cascade INITIALLY DEFERRED,
       contact_id bigint not null,
       foreign key (contact_id) references contact (contact_id) on delete cascade INITIALLY DEFERRED,
       constraint project_contact_c1 unique (project_id,contact_id)
);
create index project_contact_idx1 on project_contact (project_id);
create index project_contact_idx2 on project_contact (contact_id);

COMMENT ON TABLE project_contact IS 'Linking table for associating projects and contacts.';

-- ================================================
-- TABLE: project_dbxref
-- ================================================

create table project_dbxref (
  project_dbxref_id bigserial not null,
  project_id bigint not null,
  dbxref_id bigint not null,
  is_current boolean not null default 'true',
  primary key (project_dbxref_id),
  foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED,
  foreign key (project_id) references project (project_id) on delete cascade INITIALLY DEFERRED,
  constraint project_dbxref_c1 unique (project_id,dbxref_id)
);
create index project_dbxref_idx1 on project_dbxref (project_id);
create index project_dbxref_idx2 on project_dbxref (dbxref_id);

COMMENT ON TABLE project_dbxref IS 'project_dbxref links a project to dbxrefs.';
COMMENT ON COLUMN project_dbxref.is_current IS 'The is_current boolean indicates whether the linked dbxref is the current -official- dbxref for the linked project.';

-- ================================================
-- TABLE: project_analysis
-- ================================================

create table project_analysis (
       project_analysis_id bigserial not null,
       primary key (project_analysis_id),
       project_id bigint not null,
       foreign key (project_id) references project (project_id) on delete cascade INITIALLY DEFERRED,
       analysis_id bigint not null,
       foreign key (analysis_id) references analysis (analysis_id) on delete cascade INITIALLY DEFERRED,
       rank int not null default 0,
       constraint project_analysis_c1 unique (project_id,analysis_id)
);
create index project_analysis_idx1 on project_analysis (project_id);
create index project_analysis_idx2 on project_analysis (analysis_id);

COMMENT ON TABLE project_analysis IS 'Links an analysis to a project that may contain multiple analyses.
The rank column can be used to specify a simple ordering in which analyses were executed.';


-- ================================================
-- TABLE: project_feature
-- ================================================

CREATE TABLE project_feature (
    project_feature_id bigserial primary key NOT NULL,
    feature_id bigint NOT NULL,
    project_id bigint NOT NULL,
    CONSTRAINT project_feature_c1 UNIQUE (feature_id, project_id),
    FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE,
    FOREIGN KEY (project_id) REFERENCES project(project_id) ON DELETE CASCADE
);

CREATE INDEX project_feature_idx1 ON project_feature USING btree (feature_id);
CREATE INDEX project_feature_idx2 ON project_feature USING btree (project_id);

COMMENT ON TABLE project_feature IS 'This table is intended associate records in the feature table with a project.';

-- ================================================
-- TABLE: project_stock
-- ================================================

CREATE TABLE project_stock (
    project_stock_id bigserial primary key NOT NULL,
    stock_id bigint NOT NULL,
    project_id bigint NOT NULL,
    CONSTRAINT project_stock_c1 UNIQUE (stock_id, project_id),
    FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE,
    FOREIGN KEY (project_id) REFERENCES project(project_id) ON DELETE CASCADE
);

CREATE INDEX project_stock_idx1 ON project_stock USING btree (stock_id);
CREATE INDEX project_stock_idx2 ON project_stock USING btree (project_id);


COMMENT ON TABLE project_stock IS 'This table is intended associate records in the stock table with a project.';
-- $Id: mage.sql,v 1.3 2008-03-19 18:32:51 scottcain Exp $
-- ==========================================
-- Chado mage module
--
-- =================================================================
-- Dependencies:
--
-- :import feature from sequence
-- :import cvterm from cv
-- :import pub from pub
-- :import organism from organism
-- :import contact from contact
-- :import dbxref from db
-- :import tableinfo from general
-- :import project from project
-- :import analysis from companalysis
-- =================================================================

-- ================================================
-- TABLE: mageml
-- ================================================

create table mageml (
    mageml_id bigserial not null,
    primary key (mageml_id),
    mage_package text not null,
    mage_ml text not null
);

COMMENT ON TABLE mageml IS 'This table is for storing extra bits of MAGEml in a denormalized form. More normalization would require many more tables.';

-- ================================================
-- TABLE: magedocumentation
-- ================================================

create table magedocumentation (
    magedocumentation_id bigserial not null,
    primary key (magedocumentation_id),
    mageml_id bigint not null,
    foreign key (mageml_id) references mageml (mageml_id) on delete cascade INITIALLY DEFERRED,
    tableinfo_id bigint not null,
    foreign key (tableinfo_id) references tableinfo (tableinfo_id) on delete cascade INITIALLY DEFERRED,
    row_id int not null,
    mageidentifier text not null
);
create index magedocumentation_idx1 on magedocumentation (mageml_id);
create index magedocumentation_idx2 on magedocumentation (tableinfo_id);
create index magedocumentation_idx3 on magedocumentation (row_id);

COMMENT ON TABLE magedocumentation IS NULL;

-- ================================================
-- TABLE: protocol
-- ================================================

create table protocol (
    protocol_id bigserial not null,
    primary key (protocol_id),
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint null,
    foreign key (pub_id) references pub (pub_id) on delete set null INITIALLY DEFERRED,
    dbxref_id bigint null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
    name text not null,
    uri text null,
    protocoldescription text null,
    hardwaredescription text null,
    softwaredescription text null,
    constraint protocol_c1 unique (name)
);
create index protocol_idx1 on protocol (type_id);
create index protocol_idx2 on protocol (pub_id);
create index protocol_idx3 on protocol (dbxref_id);

COMMENT ON TABLE protocol IS 'Procedural notes on how data was prepared and processed.';

-- ================================================
-- TABLE: protocolparam
-- ================================================

create table protocolparam (
    protocolparam_id bigserial not null,
    primary key (protocolparam_id),
    protocol_id bigint not null,
    foreign key (protocol_id) references protocol (protocol_id) on delete cascade INITIALLY DEFERRED,
    name text not null,
    datatype_id bigint null,
    foreign key (datatype_id) references cvterm (cvterm_id) on delete set null INITIALLY DEFERRED,
    unittype_id bigint null,
    foreign key (unittype_id) references cvterm (cvterm_id) on delete set null INITIALLY DEFERRED,
    value text null,
    rank int not null default 0
);
create index protocolparam_idx1 on protocolparam (protocol_id);
create index protocolparam_idx2 on protocolparam (datatype_id);
create index protocolparam_idx3 on protocolparam (unittype_id);

COMMENT ON TABLE protocolparam IS 'Parameters related to a
protocol. For example, if the protocol is a soak, this might include attributes of bath temperature and duration.';

-- ================================================
-- TABLE: channel
-- ================================================

create table channel (
    channel_id bigserial not null,
    primary key (channel_id),
    name text not null,
    definition text not null,
    constraint channel_c1 unique (name)
);

COMMENT ON TABLE channel IS 'Different array platforms can record signals from one or more channels (cDNA arrays typically use two CCD, but Affymetrix uses only one).';

-- ================================================
-- TABLE: arraydesign
-- ================================================

create table arraydesign (
    arraydesign_id bigserial not null,
    primary key (arraydesign_id),
    manufacturer_id bigint not null,
    foreign key (manufacturer_id) references contact (contact_id) on delete cascade INITIALLY DEFERRED,
    platformtype_id bigint not null,
    foreign key (platformtype_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    substratetype_id bigint null,
    foreign key (substratetype_id) references cvterm (cvterm_id) on delete set null INITIALLY DEFERRED,
    protocol_id bigint null,
    foreign key (protocol_id) references protocol (protocol_id) on delete set null INITIALLY DEFERRED,
    dbxref_id bigint null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
    name text not null,
    version text null,
    description text null,
    array_dimensions text null,
    element_dimensions text null,
    num_of_elements int null,
    num_array_columns int null,
    num_array_rows int null,
    num_grid_columns int null,
    num_grid_rows int null,
    num_sub_columns int null,
    num_sub_rows int null,
    constraint arraydesign_c1 unique (name)
);
create index arraydesign_idx1 on arraydesign (manufacturer_id);
create index arraydesign_idx2 on arraydesign (platformtype_id);
create index arraydesign_idx3 on arraydesign (substratetype_id);
create index arraydesign_idx4 on arraydesign (protocol_id);
create index arraydesign_idx5 on arraydesign (dbxref_id);

COMMENT ON TABLE arraydesign IS 'General properties about an array.
An array is a template used to generate physical slides, etc.  It
contains layout information, as well as global array properties, such
as material (glass, nylon) and spot dimensions (in rows/columns).';

-- ================================================
-- TABLE: arraydesignprop
-- ================================================

create table arraydesignprop (
    arraydesignprop_id bigserial not null,
    primary key (arraydesignprop_id),
    arraydesign_id bigint not null,
    foreign key (arraydesign_id) references arraydesign (arraydesign_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint arraydesignprop_c1 unique (arraydesign_id,type_id,rank)
);
create index arraydesignprop_idx1 on arraydesignprop (arraydesign_id);
create index arraydesignprop_idx2 on arraydesignprop (type_id);

COMMENT ON TABLE arraydesignprop IS 'Extra array design properties that are not accounted for in arraydesign.';

-- ================================================
-- TABLE: assay
-- ================================================

create table assay (
    assay_id bigserial not null,
    primary key (assay_id),
    arraydesign_id bigint not null,
    foreign key (arraydesign_id) references arraydesign (arraydesign_id) on delete cascade INITIALLY DEFERRED,
    protocol_id bigint null,
    foreign key (protocol_id) references protocol (protocol_id) on delete set null INITIALLY DEFERRED,
    assaydate timestamp null default current_timestamp,
    arrayidentifier text null,
    arraybatchidentifier text null,
    operator_id bigint not null,
    foreign key (operator_id) references contact (contact_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id bigint null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
    name text null,
    description text null,
    constraint assay_c1 unique (name)
);
create index assay_idx1 on assay (arraydesign_id);
create index assay_idx2 on assay (protocol_id);
create index assay_idx3 on assay (operator_id);
create index assay_idx4 on assay (dbxref_id);

COMMENT ON TABLE assay IS 'An assay consists of a physical instance of
an array, combined with the conditions used to create the array
(protocols, technician information). The assay can be thought of as a hybridization.';

-- ================================================
-- TABLE: assayprop
-- ================================================

create table assayprop (
    assayprop_id bigserial not null,
    primary key (assayprop_id),
    assay_id bigint not null,
    foreign key (assay_id) references assay (assay_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint assayprop_c1 unique (assay_id,type_id,rank)
);
create index assayprop_idx1 on assayprop (assay_id);
create index assayprop_idx2 on assayprop (type_id);

COMMENT ON TABLE assayprop IS 'Extra assay properties that are not accounted for in assay.';

-- ================================================
-- TABLE: assay_project
-- ================================================

create table assay_project (
    assay_project_id bigserial not null,
    primary key (assay_project_id),
    assay_id bigint not null,
    foreign key (assay_id) references assay (assay_id) INITIALLY DEFERRED,
    project_id bigint not null,
    foreign key (project_id) references project (project_id) INITIALLY DEFERRED,
    constraint assay_project_c1 unique (assay_id,project_id)
);
create index assay_project_idx1 on assay_project (assay_id);
create index assay_project_idx2 on assay_project (project_id);

COMMENT ON TABLE assay_project IS 'Link assays to projects.';

-- ================================================
-- TABLE: biomaterial
-- ================================================

create table biomaterial (
    biomaterial_id bigserial not null,
    primary key (biomaterial_id),
    taxon_id bigint null,
    foreign key (taxon_id) references organism (organism_id) on delete set null INITIALLY DEFERRED,
    biosourceprovider_id bigint null,
    foreign key (biosourceprovider_id) references contact (contact_id) on delete set null INITIALLY DEFERRED,
    dbxref_id bigint null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
    name text null,
    description text null,
    constraint biomaterial_c1 unique (name)
);
create index biomaterial_idx1 on biomaterial (taxon_id);
create index biomaterial_idx2 on biomaterial (biosourceprovider_id);
create index biomaterial_idx3 on biomaterial (dbxref_id);

COMMENT ON TABLE biomaterial IS 'A biomaterial represents the MAGE concept of BioSource, BioSample, and LabeledExtract. It is essentially some biological material (tissue, cells, serum) that may have been processed. Processed biomaterials should be traceable back to raw biomaterials via the biomaterialrelationship table.';

-- ================================================
-- TABLE: biomaterial_relationship
-- ================================================

create table biomaterial_relationship (
    biomaterial_relationship_id bigserial not null,
    primary key (biomaterial_relationship_id),
    subject_id bigint not null,
    foreign key (subject_id) references biomaterial (biomaterial_id) INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references biomaterial (biomaterial_id) INITIALLY DEFERRED,
    constraint biomaterial_relationship_c1 unique (subject_id,object_id,type_id)
);
create index biomaterial_relationship_idx1 on biomaterial_relationship (subject_id);
create index biomaterial_relationship_idx2 on biomaterial_relationship (object_id);
create index biomaterial_relationship_idx3 on biomaterial_relationship (type_id);

COMMENT ON TABLE biomaterial_relationship IS 'Relate biomaterials to one another. This is a way to track a series of treatments or material splits/merges, for instance.';

-- ================================================
-- TABLE: biomaterialprop
-- ================================================

create table biomaterialprop (
    biomaterialprop_id bigserial not null,
    primary key (biomaterialprop_id),
    biomaterial_id bigint not null,
    foreign key (biomaterial_id) references biomaterial (biomaterial_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint biomaterialprop_c1 unique (biomaterial_id,type_id,rank)
);
create index biomaterialprop_idx1 on biomaterialprop (biomaterial_id);
create index biomaterialprop_idx2 on biomaterialprop (type_id);

COMMENT ON TABLE biomaterialprop IS 'Extra biomaterial properties that are not accounted for in biomaterial.';

-- ================================================
-- TABLE: biomaterial_dbxref
-- ================================================

create table biomaterial_dbxref (
    biomaterial_dbxref_id bigserial not null,
    primary key (biomaterial_dbxref_id),
    biomaterial_id bigint not null,
    foreign key (biomaterial_id) references biomaterial (biomaterial_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id bigint not null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED,
    constraint biomaterial_dbxref_c1 unique (biomaterial_id,dbxref_id)
);
create index biomaterial_dbxref_idx1 on biomaterial_dbxref (biomaterial_id);
create index biomaterial_dbxref_idx2 on biomaterial_dbxref (dbxref_id);

-- ================================================
-- TABLE: treatment
-- ================================================

create table treatment (
    treatment_id bigserial not null,
    primary key (treatment_id),
    rank int not null default 0,
    biomaterial_id bigint not null,
    foreign key (biomaterial_id) references biomaterial (biomaterial_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    protocol_id bigint null,
    foreign key (protocol_id) references protocol (protocol_id) on delete set null INITIALLY DEFERRED,
    name text null
);
create index treatment_idx1 on treatment (biomaterial_id);
create index treatment_idx2 on treatment (type_id);
create index treatment_idx3 on treatment (protocol_id);

COMMENT ON TABLE treatment IS 'A biomaterial may undergo multiple
treatments. Examples of treatments: apoxia, fluorophore and biotin labeling.';

-- ================================================
-- TABLE: biomaterial_treatment
-- ================================================

create table biomaterial_treatment (
    biomaterial_treatment_id bigserial not null,
    primary key (biomaterial_treatment_id),
    biomaterial_id bigint not null,
    foreign key (biomaterial_id) references biomaterial (biomaterial_id) on delete cascade INITIALLY DEFERRED,
    treatment_id bigint not null,
    foreign key (treatment_id) references treatment (treatment_id) on delete cascade INITIALLY DEFERRED,
    unittype_id bigint null,
    foreign key (unittype_id) references cvterm (cvterm_id) on delete set null INITIALLY DEFERRED,
    value float(15) null,
    rank int not null default 0,
    constraint biomaterial_treatment_c1 unique (biomaterial_id,treatment_id)
);
create index biomaterial_treatment_idx1 on biomaterial_treatment (biomaterial_id);
create index biomaterial_treatment_idx2 on biomaterial_treatment (treatment_id);
create index biomaterial_treatment_idx3 on biomaterial_treatment (unittype_id);

COMMENT ON TABLE biomaterial_treatment IS 'Link biomaterials to treatments. Treatments have an order of operations (rank), and associated measurements (unittype_id, value).';

-- ================================================
-- TABLE: assay_biomaterial
-- ================================================

create table assay_biomaterial (
    assay_biomaterial_id bigserial not null,
    primary key (assay_biomaterial_id),
    assay_id bigint not null,
    foreign key (assay_id) references assay (assay_id) on delete cascade INITIALLY DEFERRED,
    biomaterial_id bigint not null,
    foreign key (biomaterial_id) references biomaterial (biomaterial_id) on delete cascade INITIALLY DEFERRED,
    channel_id bigint null,
    foreign key (channel_id) references channel (channel_id) on delete set null INITIALLY DEFERRED,
    rank int not null default 0,
    constraint assay_biomaterial_c1 unique (assay_id,biomaterial_id,channel_id,rank)
);
create index assay_biomaterial_idx1 on assay_biomaterial (assay_id);
create index assay_biomaterial_idx2 on assay_biomaterial (biomaterial_id);
create index assay_biomaterial_idx3 on assay_biomaterial (channel_id);

COMMENT ON TABLE assay_biomaterial IS 'A biomaterial can be hybridized many times (technical replicates), or combined with other biomaterials in a single hybridization (for two-channel arrays).';

-- ================================================
-- TABLE: acquisition
-- ================================================

create table acquisition (
    acquisition_id bigserial not null,
    primary key (acquisition_id),
    assay_id bigint not null,
    foreign key (assay_id) references  assay (assay_id) on delete cascade INITIALLY DEFERRED,
    protocol_id bigint null,
    foreign key (protocol_id) references protocol (protocol_id) on delete set null INITIALLY DEFERRED,
    channel_id bigint null,
    foreign key (channel_id) references channel (channel_id) on delete set null INITIALLY DEFERRED,
    acquisitiondate timestamp null default current_timestamp,
    name text null,
    uri text null,
    constraint acquisition_c1 unique (name)
);
create index acquisition_idx1 on acquisition (assay_id);
create index acquisition_idx2 on acquisition (protocol_id);
create index acquisition_idx3 on acquisition (channel_id);

COMMENT ON TABLE acquisition IS 'This represents the scanning of hybridized material. The output of this process is typically a digital image of an array.';

-- ================================================
-- TABLE: acquisitionprop
-- ================================================

create table acquisitionprop (
    acquisitionprop_id bigserial not null,
    primary key (acquisitionprop_id),
    acquisition_id bigint not null,
    foreign key (acquisition_id) references acquisition (acquisition_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint acquisitionprop_c1 unique (acquisition_id,type_id,rank)
);
create index acquisitionprop_idx1 on acquisitionprop (acquisition_id);
create index acquisitionprop_idx2 on acquisitionprop (type_id);

COMMENT ON TABLE acquisitionprop IS 'Parameters associated with image acquisition.';

-- ================================================
-- TABLE: acquisition_relationship
-- ================================================

create table acquisition_relationship (
    acquisition_relationship_id bigserial not null,
    primary key (acquisition_relationship_id),
    subject_id bigint not null,
    foreign key (subject_id) references acquisition (acquisition_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references acquisition (acquisition_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint acquisition_relationship_c1 unique (subject_id,object_id,type_id,rank)
);
create index acquisition_relationship_idx1 on acquisition_relationship (subject_id);
create index acquisition_relationship_idx2 on acquisition_relationship (type_id);
create index acquisition_relationship_idx3 on acquisition_relationship (object_id);

COMMENT ON TABLE acquisition_relationship IS 'Multiple monochrome images may be merged to form a multi-color image. Red-green images of 2-channel hybridizations are an example of this.';

-- ================================================
-- TABLE: quantification
-- ================================================

create table quantification (
    quantification_id bigserial not null,
    primary key (quantification_id),
    acquisition_id bigint not null,
    foreign key (acquisition_id) references acquisition (acquisition_id) on delete cascade INITIALLY DEFERRED,
    operator_id bigint null,
    foreign key (operator_id) references contact (contact_id) on delete set null INITIALLY DEFERRED,
    protocol_id bigint null,
    foreign key (protocol_id) references protocol (protocol_id) on delete set null INITIALLY DEFERRED,
    analysis_id bigint not null,
    foreign key (analysis_id) references analysis (analysis_id) on delete cascade INITIALLY DEFERRED,
    quantificationdate timestamp null default current_timestamp,
    name text null,
    uri text null,
    constraint quantification_c1 unique (name,analysis_id)
);
create index quantification_idx1 on quantification (acquisition_id);
create index quantification_idx2 on quantification (operator_id);
create index quantification_idx3 on quantification (protocol_id);
create index quantification_idx4 on quantification (analysis_id);

COMMENT ON TABLE quantification IS 'Quantification is the transformation of an image acquisition to numeric data. This typically involves statistical procedures.';

-- ================================================
-- TABLE: quantificationprop
-- ================================================

create table quantificationprop (
    quantificationprop_id bigserial not null,
    primary key (quantificationprop_id),
    quantification_id bigint not null,
    foreign key (quantification_id) references quantification (quantification_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint quantificationprop_c1 unique (quantification_id,type_id,rank)
);
create index quantificationprop_idx1 on quantificationprop (quantification_id);
create index quantificationprop_idx2 on quantificationprop (type_id);

COMMENT ON TABLE quantificationprop IS 'Extra quantification properties that are not accounted for in quantification.';

-- ================================================
-- TABLE: quantification_relationship
-- ================================================

create table quantification_relationship (
    quantification_relationship_id bigserial not null,
    primary key (quantification_relationship_id),
    subject_id bigint not null,
    foreign key (subject_id) references quantification (quantification_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references quantification (quantification_id) on delete cascade INITIALLY DEFERRED,
    constraint quantification_relationship_c1 unique (subject_id,object_id,type_id)
);
create index quantification_relationship_idx1 on quantification_relationship (subject_id);
create index quantification_relationship_idx2 on quantification_relationship (type_id);
create index quantification_relationship_idx3 on quantification_relationship (object_id);

COMMENT ON TABLE quantification_relationship IS 'There may be multiple rounds of quantification, this allows us to keep an audit trail of what values went where.';

-- ================================================
-- TABLE: control
-- ================================================

create table control (
    control_id bigserial not null,
    primary key (control_id),
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    assay_id bigint not null,
    foreign key (assay_id) references assay (assay_id) on delete cascade INITIALLY DEFERRED,
    tableinfo_id bigint not null,
    foreign key (tableinfo_id) references tableinfo (tableinfo_id) on delete cascade INITIALLY DEFERRED,
    row_id int not null,
    name text null,
    value text null,
    rank int not null default 0
);
create index control_idx1 on control (type_id);
create index control_idx2 on control (assay_id);
create index control_idx3 on control (tableinfo_id);
create index control_idx4 on control (row_id);

COMMENT ON TABLE control IS NULL;

-- ================================================
-- TABLE: element
-- ================================================

create table element (
    element_id bigserial not null,
    primary key (element_id),
    feature_id bigint null,
    foreign key (feature_id) references feature (feature_id) on delete set null INITIALLY DEFERRED,
    arraydesign_id bigint not null,
    foreign key (arraydesign_id) references arraydesign (arraydesign_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint null,
    foreign key (type_id) references cvterm (cvterm_id) on delete set null INITIALLY DEFERRED,
    dbxref_id bigint null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
    constraint element_c1 unique (feature_id,arraydesign_id)
);
create index element_idx1 on element (feature_id);
create index element_idx2 on element (arraydesign_id);
create index element_idx3 on element (type_id);
create index element_idx4 on element (dbxref_id);

COMMENT ON TABLE element IS 'Represents a feature of the array. This is typically a region of the array coated or bound to DNA.';

-- ================================================
-- TABLE: element_result
-- ================================================

create table elementresult (
    elementresult_id bigserial not null,
    primary key (elementresult_id),
    element_id bigint not null,
    foreign key (element_id) references element (element_id) on delete cascade INITIALLY DEFERRED,
    quantification_id bigint not null,
    foreign key (quantification_id) references quantification (quantification_id) on delete cascade INITIALLY DEFERRED,
    signal float not null,
    constraint elementresult_c1 unique (element_id,quantification_id)
);
create index elementresult_idx1 on elementresult (element_id);
create index elementresult_idx2 on elementresult (quantification_id);
create index elementresult_idx3 on elementresult (signal);

COMMENT ON TABLE elementresult IS 'An element on an array produces a measurement when hybridized to a biomaterial (traceable through quantification_id). This is the base data from which tables that actually contain data inherit.';

-- ================================================
-- TABLE: element_relationship
-- ================================================

create table element_relationship (
    element_relationship_id bigserial not null,
    primary key (element_relationship_id),
    subject_id bigint not null,
    foreign key (subject_id) references element (element_id) INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references element (element_id) INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint element_relationship_c1 unique (subject_id,object_id,type_id,rank)
);
create index element_relationship_idx1 on element_relationship (subject_id);
create index element_relationship_idx2 on element_relationship (type_id);
create index element_relationship_idx3 on element_relationship (object_id);
create index element_relationship_idx4 on element_relationship (value);

COMMENT ON TABLE element_relationship IS 'Sometimes we want to combine measurements from multiple elements to get a composite value. Affymetrix combines many probes to form a probeset measurement, for instance.';

-- ================================================
-- TABLE: elementresult_relationship
-- ================================================

create table elementresult_relationship (
    elementresult_relationship_id bigserial not null,
    primary key (elementresult_relationship_id),
    subject_id bigint not null,
    foreign key (subject_id) references elementresult (elementresult_id) INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references elementresult (elementresult_id) INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint elementresult_relationship_c1 unique (subject_id,object_id,type_id,rank)
);
create index elementresult_relationship_idx1 on elementresult_relationship (subject_id);
create index elementresult_relationship_idx2 on elementresult_relationship (type_id);
create index elementresult_relationship_idx3 on elementresult_relationship (object_id);
create index elementresult_relationship_idx4 on elementresult_relationship (value);

COMMENT ON TABLE elementresult_relationship IS 'Sometimes we want to combine measurements from multiple elements to get a composite value. Affymetrix combines many probes to form a probeset measurement, for instance.';

-- ================================================
-- TABLE: study
-- ================================================

create table study (
    study_id bigserial not null,
    primary key (study_id),
    contact_id bigint not null,
    foreign key (contact_id) references contact (contact_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint null,
    foreign key (pub_id) references pub (pub_id) on delete set null INITIALLY DEFERRED,
    dbxref_id bigint null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
    name text not null,
    description text null,
    constraint study_c1 unique (name)
);
create index study_idx1 on study (contact_id);
create index study_idx2 on study (pub_id);
create index study_idx3 on study (dbxref_id);

COMMENT ON TABLE study IS NULL;

-- ================================================
-- TABLE: study_assay
-- ================================================

create table study_assay (
    study_assay_id bigserial not null,
    primary key (study_assay_id),
    study_id bigint not null,
    foreign key (study_id) references study (study_id) on delete cascade INITIALLY DEFERRED,
    assay_id bigint not null,
    foreign key (assay_id) references assay (assay_id) on delete cascade INITIALLY DEFERRED,
    constraint study_assay_c1 unique (study_id,assay_id)
);
create index study_assay_idx1 on study_assay (study_id);
create index study_assay_idx2 on study_assay (assay_id);

COMMENT ON TABLE study_assay IS NULL;

-- ================================================
-- TABLE: studydesign
-- ================================================

create table studydesign (
    studydesign_id bigserial not null,
    primary key (studydesign_id),
    study_id bigint not null,
    foreign key (study_id) references study (study_id) on delete cascade INITIALLY DEFERRED,
    description text null
);
create index studydesign_idx1 on studydesign (study_id);

COMMENT ON TABLE studydesign IS NULL;

-- ================================================
-- TABLE: studydesignprop
-- ================================================

create table studydesignprop (
    studydesignprop_id bigserial not null,
    primary key (studydesignprop_id),
    studydesign_id bigint not null,
    foreign key (studydesign_id) references studydesign (studydesign_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint studydesignprop_c1 unique (studydesign_id,type_id,rank)
);
create index studydesignprop_idx1 on studydesignprop (studydesign_id);
create index studydesignprop_idx2 on studydesignprop (type_id);

COMMENT ON TABLE studydesignprop IS NULL;

-- ================================================
-- TABLE: studyfactor
-- ================================================

create table studyfactor (
    studyfactor_id bigserial not null,
    primary key (studyfactor_id),
    studydesign_id bigint not null,
    foreign key (studydesign_id) references studydesign (studydesign_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint null,
    foreign key (type_id) references cvterm (cvterm_id) on delete set null INITIALLY DEFERRED,
    name text not null,
    description text null
);
create index studyfactor_idx1 on studyfactor (studydesign_id);
create index studyfactor_idx2 on studyfactor (type_id);

COMMENT ON TABLE studyfactor IS NULL;

-- ================================================
-- TABLE: studyfactorvalue
-- ================================================

create table studyfactorvalue (
    studyfactorvalue_id bigserial not null,
    primary key (studyfactorvalue_id),
    studyfactor_id bigint not null,
    foreign key (studyfactor_id) references studyfactor (studyfactor_id) on delete cascade INITIALLY DEFERRED,
    assay_id bigint not null,
    foreign key (assay_id) references assay (assay_id) on delete cascade INITIALLY DEFERRED,
    factorvalue text null,
    name text null,
    rank int not null default 0
);
create index studyfactorvalue_idx1 on studyfactorvalue (studyfactor_id);
create index studyfactorvalue_idx2 on studyfactorvalue (assay_id);

COMMENT ON TABLE studyfactorvalue IS NULL;

--
-- studyprop and studyprop_feature added for Kara Dolinski's group
--
-- Here is her description of it:
--Both of the tables are used for our YFGdb project
--(http://yfgdb.princeton.edu/), which uses chado.
--
--Here is how we use those tables, using the following example:
--
--http://yfgdb.princeton.edu/cgi-bin/display.cgi?db=pmid&amp;id=15575969
--
--The above data set is represented as a row in the STUDY table.  We have
--lots of attributes that we want to store about each STUDY (status, etc)
--and in the official schema, the only prop table we could use was the
--STUDYDESIGN_PROP table.  This forced us to go through the STUDYDESIGN
--table when we often have no real data to store in that table (small
--percent of our collection use MAGE-ML unfortunately, and even fewer
--provide all the data in the MAGE model, of which STUDYDESIGN is a vestige).
--So, we created a STUDYPROP table.  I'd think this table would be
--generally useful to people storing various types of data sets via the
--STUDY table.
--
--The other new table is STUDYPROP_FEATURE.  This basically allows us to
--group features together per study.  For example, we can store microarray
--clustering results by saying that the STUDYPROP type is 'cluster'  (via
--type_id -> CVTERM of course), the value is 'cluster id 123', and then
--that cluster would be associated with all the features that are in that
--cluster via STUDYPROP_FEATURE.  Adding type_id to STUDYPROP_FEATURE is
-- fine by us!
--
--studyprop
create table studyprop (
    studyprop_id bigserial not null,
        primary key (studyprop_id),
    study_id bigint not null,
        foreign key (study_id) references study (study_id) on delete cascade,
    type_id bigint not null,
        foreign key (type_id) references cvterm (cvterm_id) on delete cascade,
    value text null,
    rank int not null default 0,
    unique (study_id,type_id,rank)
);

create index studyprop_idx1 on studyprop (study_id);
create index studyprop_idx2 on studyprop (type_id);


--studyprop_feature
CREATE TABLE studyprop_feature (
    studyprop_feature_id bigserial NOT NULL,
    primary key (studyprop_feature_id),
    studyprop_id bigint NOT NULL,
    foreign key (studyprop_id) references studyprop(studyprop_id) on delete cascade,
    feature_id bigint NOT NULL,
    foreign key (feature_id) references feature (feature_id) on delete cascade,
    type_id bigint,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade,
    unique (studyprop_id, feature_id)
    );


create index studyprop_feature_idx1 on studyprop_feature (studyprop_id);
create index studyprop_feature_idx2 on studyprop_feature (feature_id);

-- ==========================================
-- Chado cell line module
--
-- ============
-- DEPENDENCIES
-- ============
-- :import feature from sequence
-- :import synonym from sequence
-- :import library from library
-- :import cvterm from cv
-- :import dbxref from db
-- :import pub from pub
-- :import organism from organism
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-- ================================================
-- TABLE: cell_line
-- ================================================

create table cell_line (
        cell_line_id bigserial not null,
        primary key (cell_line_id),
        name varchar(255) null,
        uniquename varchar(255) not null,
	organism_id bigint not null,
	foreign key (organism_id) references organism (organism_id) on delete cascade INITIALLY DEFERRED,
	timeaccessioned timestamp not null default current_timestamp,
	timelastmodified timestamp not null default current_timestamp,
        constraint cell_line_c1 unique (uniquename, organism_id)
);
grant all on cell_line to PUBLIC;


-- ================================================
-- TABLE: cell_line_relationship
-- ================================================

create table cell_line_relationship (
	cell_line_relationship_id bigserial not null,
	primary key (cell_line_relationship_id),
        subject_id bigint not null,
	foreign key (subject_id) references cell_line (cell_line_id) on delete cascade INITIALLY DEFERRED,
        object_id bigint not null,
	foreign key (object_id) references cell_line (cell_line_id) on delete cascade INITIALLY DEFERRED,
	type_id bigint not null,
	foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
	constraint cell_line_relationship_c1 unique (subject_id, object_id, type_id)
);
grant all on cell_line_relationship to PUBLIC;


-- ================================================
-- TABLE: cell_line_synonym
-- ================================================

create table cell_line_synonym (
	cell_line_synonym_id bigserial not null,
	primary key (cell_line_synonym_id),
	cell_line_id bigint not null,
	foreign key (cell_line_id) references cell_line (cell_line_id) on delete cascade INITIALLY DEFERRED,
	synonym_id bigint not null,
	foreign key (synonym_id) references synonym (synonym_id) on delete cascade INITIALLY DEFERRED,
	pub_id bigint not null,
	foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
	is_current boolean not null default 'false',
	is_internal boolean not null default 'false',
	constraint cell_line_synonym_c1 unique (synonym_id,cell_line_id,pub_id)
);
grant all on cell_line_synonym to PUBLIC;


-- ================================================
-- TABLE: cell_line_cvterm
-- ================================================

create table cell_line_cvterm (
	cell_line_cvterm_id bigserial not null,
	primary key (cell_line_cvterm_id),
	cell_line_id bigint not null,
	foreign key (cell_line_id) references cell_line (cell_line_id) on delete cascade INITIALLY DEFERRED,
	cvterm_id bigint not null,
	foreign key (cvterm_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
	pub_id bigint not null,
	foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
	rank int not null default 0,
	constraint cell_line_cvterm_c1 unique (cell_line_id,cvterm_id,pub_id,rank)
);
grant all on cell_line_cvterm to PUBLIC;


-- ================================================
-- TABLE: cell_line_dbxref
-- ================================================

create table cell_line_dbxref (
	cell_line_dbxref_id bigserial not null,
	primary key (cell_line_dbxref_id),
	cell_line_id bigint not null,
	foreign key (cell_line_id) references cell_line (cell_line_id) on delete cascade INITIALLY DEFERRED,
	dbxref_id bigint not null,
	foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED,
	is_current boolean not null default 'true',
	constraint cell_line_dbxref_c1 unique (cell_line_id,dbxref_id)
);
grant all on cell_line_dbxref to PUBLIC;


-- ================================================
-- TABLE: cell_lineprop
-- ================================================

create table cell_lineprop (
	cell_lineprop_id bigserial not null,
	primary key (cell_lineprop_id),
	cell_line_id bigint not null,
	foreign key (cell_line_id) references cell_line (cell_line_id) on delete cascade INITIALLY DEFERRED,
	type_id bigint not null,
	foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
	value text null,
	rank int not null default 0,
	constraint cell_lineprop_c1 unique (cell_line_id,type_id,rank)
);
grant all on cell_lineprop to PUBLIC;


-- ================================================
-- TABLE: cell_lineprop_pub
-- ================================================

create table cell_lineprop_pub (
	cell_lineprop_pub_id bigserial not null,
	primary key (cell_lineprop_pub_id),
	cell_lineprop_id bigint not null,
	foreign key (cell_lineprop_id) references cell_lineprop (cell_lineprop_id) on delete cascade INITIALLY DEFERRED,
	pub_id bigint not null,
	foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
	constraint cell_lineprop_pub_c1 unique (cell_lineprop_id,pub_id)
);
grant all on cell_lineprop_pub to PUBLIC;


-- ================================================
-- TABLE: cell_line_feature
-- ================================================

create table cell_line_feature (
	cell_line_feature_id bigserial not null,
	primary key (cell_line_feature_id),
	cell_line_id bigint not null,
	foreign key (cell_line_id) references cell_line (cell_line_id) on delete cascade INITIALLY DEFERRED,
	feature_id bigint not null,
	foreign key (feature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,
	pub_id bigint not null,
	foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
	constraint cell_line_feature_c1 unique (cell_line_id, feature_id, pub_id)
);
grant all on cell_line_feature to PUBLIC;


-- ================================================
-- TABLE: cell_line_cvtermprop
-- ================================================

create table cell_line_cvtermprop (
	cell_line_cvtermprop_id bigserial not null,
	primary key (cell_line_cvtermprop_id),
	cell_line_cvterm_id bigint not null,
	foreign key (cell_line_cvterm_id) references cell_line_cvterm (cell_line_cvterm_id) on delete cascade INITIALLY DEFERRED,
	type_id bigint not null,
	foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
	value text null,
	rank int not null default 0,
	constraint cell_line_cvtermprop_c1 unique (cell_line_cvterm_id, type_id, rank)
);
grant all on cell_line_cvtermprop to PUBLIC;


-- ================================================
-- TABLE: cell_line_pub
-- ================================================

create table cell_line_pub (
	cell_line_pub_id bigserial not null,
	primary key (cell_line_pub_id),
	cell_line_id bigint not null,
	foreign key (cell_line_id) references cell_line (cell_line_id) on delete cascade INITIALLY DEFERRED,
	pub_id bigint not null,
	foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
	constraint cell_line_pub_c1 unique (cell_line_id, pub_id)
);
grant all on cell_line_pub to PUBLIC;


-- ================================================
-- TABLE: cell_line_library
-- ================================================

create table cell_line_library (
	cell_line_library_id bigserial not null,
	primary key (cell_line_library_id),
	cell_line_id bigint not null,
	foreign key (cell_line_id) references cell_line (cell_line_id) on delete cascade INITIALLY DEFERRED,
	library_id bigint not null,
	foreign key (library_id) references library (library_id) on delete cascade INITIALLY DEFERRED,
	pub_id bigint not null,
	foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
	constraint cell_line_library_c1 unique (cell_line_id, library_id, pub_id)
);
grant all on cell_line_library to PUBLIC;

-- =================================================================
-- Dependencies:
--
-- :import feature from sequence
-- :import cvterm from cv
-- :import pub from pub
-- :import phenotype from phenotype
-- :import organism from organism
-- :import genotype from genetic
-- :import contact from contact
-- :import project from project
-- :import stock from stock
-- :import synonym
-- =================================================================


-- this probably needs some work, depending on how cross-database we
-- want to be.  In Postgres, at least, there are much better ways to
-- represent geo information.

-- ================================================
-- TABLE: nd_geolocation
-- ================================================

CREATE TABLE nd_geolocation (
    nd_geolocation_id bigserial PRIMARY KEY NOT NULL,
    description text,
    latitude real,
    longitude real,
    geodetic_datum character varying(32),
    altitude real
);
CREATE INDEX nd_geolocation_idx1 ON nd_geolocation (latitude);
CREATE INDEX nd_geolocation_idx2 ON nd_geolocation (longitude);
CREATE INDEX nd_geolocation_idx3 ON nd_geolocation (altitude);

COMMENT ON TABLE nd_geolocation IS 'The geo-referencable location of the stock. NOTE: This entity is subject to change as a more general and possibly more OpenGIS-compliant geolocation module may be introduced into Chado.';

COMMENT ON COLUMN nd_geolocation.description IS 'A textual representation of the location, if this is the original georeference. Optional if the original georeference is available in lat/long coordinates.';


COMMENT ON COLUMN nd_geolocation.latitude IS 'The decimal latitude coordinate of the georeference, using positive and negative sign to indicate N and S, respectively.';

COMMENT ON COLUMN nd_geolocation.longitude IS 'The decimal longitude coordinate of the georeference, using positive and negative sign to indicate E and W, respectively.';

COMMENT ON COLUMN nd_geolocation.geodetic_datum IS 'The geodetic system on which the geo-reference coordinates are based. For geo-references measured between 1984 and 2010, this will typically be WGS84.';

COMMENT ON COLUMN nd_geolocation.altitude IS 'The altitude (elevation) of the location in meters. If the altitude is only known as a range, this is the average, and altitude_dev will hold half of the width of the range.';

-- ================================================
-- TABLE: nd_experiment
-- ================================================

CREATE TABLE nd_experiment (
    nd_experiment_id bigserial PRIMARY KEY NOT NULL,
    nd_geolocation_id bigint NOT NULL references nd_geolocation (nd_geolocation_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED
);
CREATE INDEX nd_experiment_idx1 ON nd_experiment (nd_geolocation_id);
CREATE INDEX nd_experiment_idx2 ON nd_experiment (type_id);

COMMENT ON TABLE nd_experiment IS 'This is the core table for the natural diversity module,
representing each individual assay that is undertaken (this is usually *not* an
entire experiment). Each nd_experiment should give rise to a single genotype or
phenotype and be described via 1 (or more) protocols. Collections of assays that
relate to each other should be linked to the same record in the project table.';

-- ================================================
-- TABLE: nd_experiment_project
-- ================================================
--
--used to be nd_diversityexperiment_project
--then was nd_assay_project
CREATE TABLE nd_experiment_project (
    nd_experiment_project_id bigserial PRIMARY KEY NOT NULL,
    project_id bigint not null references project (project_id) on delete cascade INITIALLY DEFERRED,
    nd_experiment_id bigint NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    CONSTRAINT nd_experiment_project_c1 unique (project_id, nd_experiment_id)
);
CREATE INDEX nd_experiment_project_idx1 ON nd_experiment_project (project_id);
CREATE INDEX nd_experiment_project_idx2 ON nd_experiment_project (nd_experiment_id);

COMMENT ON TABLE nd_experiment_project IS 'Used to group together related nd_experiment records. All nd_experiments
should be linked to at least one project.';

-- ================================================
-- TABLE: nd_experimentprop
-- ================================================

CREATE TABLE nd_experimentprop (
    nd_experimentprop_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_id bigint NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED ,
    value text null,
    rank int NOT NULL default 0,
    constraint nd_experimentprop_c1 unique (nd_experiment_id,type_id,rank)
);

CREATE INDEX nd_experimentprop_idx1 ON nd_experimentprop (nd_experiment_id);
CREATE INDEX nd_experimentprop_idx2 ON nd_experimentprop (type_id);

COMMENT ON TABLE nd_experimentprop IS 'An nd_experiment can have any number of
slot-value property tags attached to it. This is an alternative to
hardcoding a list of columns in the relational schema, and is
completely extensible. There is a unique constraint, stockprop_c1, for
the combination of stock_id, rank, and type_id. Multivalued property-value pairs must be differentiated by rank.';

-- ================================================
-- TABLE: nd_experiment_pub
-- ================================================

CREATE TABLE nd_experiment_pub (
       nd_experiment_pub_id bigserial PRIMARY KEY not null,
       nd_experiment_id bigint not null,
       foreign key (nd_experiment_id) references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
       pub_id bigint not null,
       foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
       constraint nd_experiment_pub_c1 unique (nd_experiment_id,pub_id)
);
create index nd_experiment_pub_idx1 on nd_experiment_pub (nd_experiment_id);
create index nd_experiment_pub_idx2 on nd_experiment_pub (pub_id);

COMMENT ON TABLE nd_experiment_pub IS 'Linking nd_experiment(s) to publication(s)';

-- ================================================
-- TABLE: nd_geolocationprop
-- ================================================

CREATE TABLE nd_geolocationprop (
    nd_geolocationprop_id bigserial PRIMARY KEY NOT NULL,
    nd_geolocation_id bigint NOT NULL references nd_geolocation (nd_geolocation_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int NOT NULL DEFAULT 0,
    constraint nd_geolocationprop_c1 unique (nd_geolocation_id,type_id,rank)
);
CREATE INDEX nd_geolocationprop_idx1 ON nd_geolocationprop (nd_geolocation_id);
CREATE INDEX nd_geolocationprop_idx2 ON nd_geolocationprop (type_id);

COMMENT ON TABLE nd_geolocationprop IS 'Property/value associations for geolocations. This table can store the properties such as location and environment';

COMMENT ON COLUMN nd_geolocationprop.type_id IS 'The name of the property as a reference to a controlled vocabulary term.';

COMMENT ON COLUMN nd_geolocationprop.value IS 'The value of the property.';

COMMENT ON COLUMN nd_geolocationprop.rank IS 'The rank of the property value, if the property has an array of values.';

-- ================================================
-- TABLE: nd_protocol
-- ================================================

CREATE TABLE nd_protocol (
    nd_protocol_id bigserial PRIMARY KEY  NOT NULL,
    name character varying(255) NOT NULL unique,
    type_id bigint NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED
);
CREATE INDEX nd_protocol_idx1 ON nd_protocol (type_id);

COMMENT ON TABLE nd_protocol IS 'A protocol can be anything that is done as part of the experiment.';

COMMENT ON COLUMN nd_protocol.name IS 'The protocol name.';

-- ================================================
-- TABLE: nd_reagent
-- ===============================================

CREATE TABLE nd_reagent (
    nd_reagent_id bigserial PRIMARY KEY NOT NULL,
    name character varying(80) NOT NULL,
    type_id bigint NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    feature_id bigint NULL references feature (feature_id) on delete cascade INITIALLY DEFERRED
);
CREATE INDEX nd_reagent_idx1 ON nd_reagent (type_id);
CREATE INDEX nd_reagent_idx2 ON nd_reagent (feature_id);

COMMENT ON TABLE nd_reagent IS 'A reagent such as a primer, an enzyme, an adapter oligo, a linker oligo. Reagents are used in genotyping experiments, or in any other kind of experiment.';

COMMENT ON COLUMN nd_reagent.name IS 'The name of the reagent. The name should be unique for a given type.';

COMMENT ON COLUMN nd_reagent.type_id IS 'The type of the reagent, for example linker oligomer, or forward primer.';

COMMENT ON COLUMN nd_reagent.feature_id IS 'If the reagent is a primer, the feature that it corresponds to. More generally, the corresponding feature for any reagent that has a sequence that maps to another sequence.';

-- ================================================
-- TABLE: nd_protocol_reagent
-- ================================================

CREATE TABLE nd_protocol_reagent (
    nd_protocol_reagent_id bigserial PRIMARY KEY NOT NULL,
    nd_protocol_id bigint NOT NULL references nd_protocol (nd_protocol_id) on delete cascade INITIALLY DEFERRED,
    reagent_id bigint NOT NULL references nd_reagent (nd_reagent_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED
);

CREATE INDEX nd_protocol_reagent_idx1 ON nd_protocol_reagent (nd_protocol_id);
CREATE INDEX nd_protocol_reagent_idx2 ON nd_protocol_reagent (reagent_id);
CREATE INDEX nd_protocol_reagent_idx3 ON nd_protocol_reagent (type_id);

-- ================================================
-- TABLE: nd_protocolprop
-- ================================================

CREATE TABLE nd_protocolprop (
    nd_protocolprop_id bigserial PRIMARY KEY NOT NULL,
    nd_protocol_id bigint NOT NULL references nd_protocol (nd_protocol_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int DEFAULT 0 NOT NULL,
    constraint nd_protocolprop_c1 unique (nd_protocol_id,type_id,rank)
);

CREATE INDEX nd_protocolprop_idx1 ON nd_protocolprop (nd_protocol_id);
CREATE INDEX nd_protocolprop_idx2 ON nd_protocolprop (type_id);

COMMENT ON TABLE nd_protocolprop IS 'Property/value associations for protocol.';

COMMENT ON COLUMN nd_protocolprop.nd_protocol_id IS 'The protocol to which the property applies.';

COMMENT ON COLUMN nd_protocolprop.type_id IS 'The name of the property as a reference to a controlled vocabulary term.';

COMMENT ON COLUMN nd_protocolprop.value IS 'The value of the property.';

COMMENT ON COLUMN nd_protocolprop.rank IS 'The rank of the property value, if the property has an array of values.';

-- ================================================
-- TABLE: nd_experiment_stock
-- ================================================

CREATE TABLE nd_experiment_stock (
    nd_experiment_stock_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_id bigint NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    stock_id bigint NOT NULL references stock (stock_id)  on delete cascade INITIALLY DEFERRED,
    type_id bigint NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED
);
CREATE INDEX nd_experiment_stock_idx1 ON nd_experiment_stock (nd_experiment_id);
CREATE INDEX nd_experiment_stock_idx2 ON nd_experiment_stock (stock_id);
CREATE INDEX nd_experiment_stock_idx3 ON nd_experiment_stock (type_id);

COMMENT ON TABLE nd_experiment_stock IS 'Part of a stock or a clone of a stock that is used in an experiment';


COMMENT ON COLUMN nd_experiment_stock.stock_id IS 'stock used in the extraction or the corresponding stock for the clone';

-- ================================================
-- TABLE: nd_experiment_protocol
-- ================================================

CREATE TABLE nd_experiment_protocol (
    nd_experiment_protocol_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_id bigint NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    nd_protocol_id bigint NOT NULL references nd_protocol (nd_protocol_id) on delete cascade INITIALLY DEFERRED
);
CREATE INDEX nd_experiment_protocol_idx1 ON nd_experiment_protocol (nd_experiment_id);
CREATE INDEX nd_experiment_protocol_idx2 ON nd_experiment_protocol (nd_protocol_id);

COMMENT ON TABLE nd_experiment_protocol IS 'Linking table: experiments to the protocols they involve.';

-- ================================================
-- TABLE: nd_experiment_phenotype
-- ================================================

CREATE TABLE nd_experiment_phenotype (
    nd_experiment_phenotype_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_id bigint NOT NULL REFERENCES nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    phenotype_id bigint NOT NULL references phenotype (phenotype_id) on delete cascade INITIALLY DEFERRED,
   constraint nd_experiment_phenotype_c1 unique (nd_experiment_id,phenotype_id)
);
CREATE INDEX nd_experiment_phenotype_idx1 ON nd_experiment_phenotype (nd_experiment_id);
CREATE INDEX nd_experiment_phenotype_idx2 ON nd_experiment_phenotype (phenotype_id);

COMMENT ON TABLE nd_experiment_phenotype IS 'Linking table: experiments to the phenotypes they produce. There is a one-to-one relationship between an experiment and a phenotype since each phenotype record should point to one experiment. Add a new experiment_id for each phenotype record.';

-- ================================================
-- TABLE: nd_experiment_genotype
-- ================================================

CREATE TABLE nd_experiment_genotype (
    nd_experiment_genotype_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_id bigint NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    genotype_id bigint NOT NULL references genotype (genotype_id) on delete cascade INITIALLY DEFERRED ,
    constraint nd_experiment_genotype_c1 unique (nd_experiment_id,genotype_id)
);

CREATE INDEX nd_experiment_genotype_idx1 ON nd_experiment_genotype (nd_experiment_id);
CREATE INDEX nd_experiment_genotype_idx2 ON nd_experiment_genotype (genotype_id);

COMMENT ON TABLE nd_experiment_genotype IS 'Linking table: experiments to the genotypes they produce. There is a one-to-one relationship between an experiment and a genotype since each genotype record should point to one experiment. Add a new experiment_id for each genotype record.';

-- ================================================
-- TABLE: nd_reagent_relationship
-- ================================================

CREATE TABLE nd_reagent_relationship (
    nd_reagent_relationship_id bigserial PRIMARY KEY NOT NULL,
    subject_reagent_id bigint NOT NULL references nd_reagent (nd_reagent_id) on delete cascade INITIALLY DEFERRED,
    object_reagent_id bigint NOT NULL  references nd_reagent (nd_reagent_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint NOT NULL  references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED
);

CREATE INDEX nd_reagent_relationship_idx1 ON nd_reagent_relationship (subject_reagent_id);
CREATE INDEX nd_reagent_relationship_idx2 ON nd_reagent_relationship (object_reagent_id);
CREATE INDEX nd_reagent_relationship_idx3 ON nd_reagent_relationship (type_id);

COMMENT ON TABLE nd_reagent_relationship IS 'Relationships between reagents. Some reagents form a group. i.e., they are used all together or not at all. Examples are adapter/linker/enzyme experiment reagents.';

COMMENT ON COLUMN nd_reagent_relationship.subject_reagent_id IS 'The subject reagent in the relationship. In parent/child terminology, the subject is the child. For example, in "linkerA 3prime-overhang-linker enzymeA" linkerA is the subject, 3prime-overhand-linker is the type, and enzymeA is the object.';

COMMENT ON COLUMN nd_reagent_relationship.object_reagent_id IS 'The object reagent in the relationship. In parent/child terminology, the object is the parent. For example, in "linkerA 3prime-overhang-linker enzymeA" linkerA is the subject, 3prime-overhand-linker is the type, and enzymeA is the object.';

COMMENT ON COLUMN nd_reagent_relationship.type_id IS 'The type (or predicate) of the relationship. For example, in "linkerA 3prime-overhang-linker enzymeA" linkerA is the subject, 3prime-overhand-linker is the type, and enzymeA is the object.';

-- ================================================
-- TABLE: nd_reagentprop
-- ================================================

CREATE TABLE nd_reagentprop (
    nd_reagentprop_id bigserial PRIMARY KEY NOT NULL,
    nd_reagent_id bigint NOT NULL references nd_reagent (nd_reagent_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int DEFAULT 0 NOT NULL,
    constraint nd_reagentprop_c1 unique (nd_reagent_id,type_id,rank)
);

CREATE INDEX nd_reagentprop_idx1 ON nd_reagentprop (nd_reagent_id);
CREATE INDEX nd_reagentprop_idx2 ON nd_reagentprop (type_id);

-- ================================================
-- TABLE: nd_experiment_stockprop
-- ================================================

CREATE TABLE nd_experiment_stockprop (
    nd_experiment_stockprop_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_stock_id bigint NOT NULL references nd_experiment_stock (nd_experiment_stock_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int DEFAULT 0 NOT NULL,
    constraint nd_experiment_stockprop_c1 unique (nd_experiment_stock_id,type_id,rank)
);

CREATE INDEX nd_experiment_stockprop_idx1 ON nd_experiment_stockprop (nd_experiment_stock_id);
CREATE INDEX nd_experiment_stockprop_idx2 ON nd_experiment_stockprop (type_id);

COMMENT ON TABLE nd_experiment_stockprop IS 'Property/value associations for experiment_stocks. This table can store the properties such as treatment';

COMMENT ON COLUMN nd_experiment_stockprop.nd_experiment_stock_id IS 'The experiment_stock to which the property applies.';

COMMENT ON COLUMN nd_experiment_stockprop.type_id IS 'The name of the property as a reference to a controlled vocabulary term.';

COMMENT ON COLUMN nd_experiment_stockprop.value IS 'The value of the property.';

COMMENT ON COLUMN nd_experiment_stockprop.rank IS 'The rank of the property value, if the property has an array of values.';

-- ================================================
-- TABLE: nd_experiment_stock_dbxref
-- ================================================

CREATE TABLE nd_experiment_stock_dbxref (
    nd_experiment_stock_dbxref_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_stock_id bigint NOT NULL references nd_experiment_stock (nd_experiment_stock_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id bigint NOT NULL references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED
);
CREATE INDEX nd_experiment_stock_dbxref_idx1 ON nd_experiment_stock_dbxref (nd_experiment_stock_id);
CREATE INDEX nd_experiment_stock_dbxref_idx2 ON nd_experiment_stock_dbxref (dbxref_id);

COMMENT ON TABLE nd_experiment_stock_dbxref IS 'Cross-reference experiment_stock to accessions, images, etc';

-- ================================================
-- TABLE: nd_experiment_dbxref
-- ===============================================

CREATE TABLE nd_experiment_dbxref (
    nd_experiment_dbxref_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_id bigint NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id bigint NOT NULL references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED
);

CREATE INDEX nd_experiment_dbxref_idx1 ON nd_experiment_dbxref (nd_experiment_id);
CREATE INDEX nd_experiment_dbxref_idx2 ON nd_experiment_dbxref (dbxref_id);

COMMENT ON TABLE nd_experiment_dbxref IS 'Cross-reference experiment to accessions, images, etc';

-- ================================================
-- TABLE: nd_experiment_contact
-- ================================================

CREATE TABLE nd_experiment_contact (
    nd_experiment_contact_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_id bigint NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    contact_id bigint NOT NULL references contact (contact_id) on delete cascade INITIALLY DEFERRED
);
CREATE INDEX nd_experiment_contact_idx1 ON nd_experiment_contact (nd_experiment_id);
CREATE INDEX nd_experiment_contact_idx2 ON nd_experiment_contact (contact_id);

-- ================================================
-- TABLE: nd_experiment_analysis
-- ================================================

CREATE TABLE nd_experiment_analysis (
  nd_experiment_analysis_id bigserial PRIMARY KEY NOT NULL,
  nd_experiment_id bigint NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
  analysis_id bigint NOT NULL references analysis (analysis_id)  on delete cascade INITIALLY DEFERRED,
  type_id bigint NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED
);
CREATE INDEX nd_experiment_analysis_idx1 ON nd_experiment_analysis (nd_experiment_id);
CREATE INDEX nd_experiment_analysis_idx2 ON nd_experiment_analysis (analysis_id);
CREATE INDEX nd_experiment_analysis_idx3 ON nd_experiment_analysis (type_id);

COMMENT ON TABLE nd_experiment_analysis IS 'An analysis that is used in an experiment';

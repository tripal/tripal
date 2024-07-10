SET search_path = chado,public;
-- $Id: general.sql,v 1.31 2007-03-01 02:45:54 briano Exp $
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

CREATE VIEW db_dbxref_count AS
  SELECT db.name,count(*) AS num_dbxrefs FROM db INNER JOIN dbxref USING (db_id) GROUP BY db.name;
COMMENT ON VIEW db_dbxref_count IS 'per-db dbxref counts';

CREATE OR REPLACE FUNCTION store_db (VARCHAR) 
  RETURNS BIGINT AS 
'DECLARE
   v_name             ALIAS FOR $1;

   v_db_id            BIGINT;
 BEGIN
    SELECT INTO v_db_id db_id
      FROM db
      WHERE name=v_name;
    IF NOT FOUND THEN
      INSERT INTO db
       (name)
         VALUES
       (v_name);
       RETURN currval(''db_db_id_seq'');
    END IF;
    RETURN v_db_id;
 END;
' LANGUAGE 'plpgsql';
  
CREATE OR REPLACE FUNCTION store_dbxref (VARCHAR,VARCHAR) 
  RETURNS BIGINT AS 
'DECLARE
   v_dbname                ALIAS FOR $1;
   v_accession             ALIAS FOR $2;

   v_db_id                 BIGINT;
   v_dbxref_id             BIGINT;
 BEGIN
    SELECT INTO v_db_id
      store_db(v_dbname);
    SELECT INTO v_dbxref_id dbxref_id
      FROM dbxref
      WHERE db_id=v_db_id       AND
            accession=v_accession;
    IF NOT FOUND THEN
      INSERT INTO dbxref
       (db_id,accession)
         VALUES
       (v_db_id,v_accession);
       RETURN currval(''dbxref_dbxref_id_seq'');
    END IF;
    RETURN v_dbxref_id;
 END;
' LANGUAGE 'plpgsql';
  
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

CREATE OR REPLACE VIEW cv_root AS
 SELECT 
  cv_id,
  cvterm_id AS root_cvterm_id
 FROM cvterm
 WHERE 
  cvterm_id NOT IN ( SELECT subject_id FROM cvterm_relationship)    AND
  is_obsolete=0;

COMMENT ON VIEW cv_root IS 'the roots of a cv are the set of terms
which have no parents (terms that are not the subject of a
relation). Most cvs will have a single root, some may have >1. All
will have at least 1';

CREATE OR REPLACE VIEW cv_leaf AS
 SELECT 
  cv_id,
  cvterm_id
 FROM cvterm
 WHERE 
  cvterm_id NOT IN ( SELECT object_id FROM cvterm_relationship);

COMMENT ON VIEW cv_leaf IS 'the leaves of a cv are the set of terms
which have no children (terms that are not the object of a
relation). All cvs will have at least 1 leaf';

CREATE OR REPLACE VIEW common_ancestor_cvterm AS
 SELECT
  p1.subject_id          AS cvterm1_id,
  p2.subject_id          AS cvterm2_id,
  p1.object_id           AS ancestor_cvterm_id,
  p1.pathdistance        AS pathdistance1,
  p2.pathdistance        AS pathdistance2,
  p1.pathdistance + p2.pathdistance
                         AS total_pathdistance
 FROM
  cvtermpath AS p1,
  cvtermpath AS p2
 WHERE 
  p1.object_id = p2.object_id;

COMMENT ON VIEW common_ancestor_cvterm IS 'The common ancestor of any
two terms is the intersection of both terms ancestors. Two terms can
have multiple common ancestors. Use total_pathdistance to get the
least common ancestor';

CREATE OR REPLACE VIEW common_descendant_cvterm AS
 SELECT
  p1.object_id           AS cvterm1_id,
  p2.object_id           AS cvterm2_id,
  p1.subject_id          AS ancestor_cvterm_id,
  p1.pathdistance        AS pathdistance1,
  p2.pathdistance        AS pathdistance2,
  p1.pathdistance + p2.pathdistance
                         AS total_pathdistance
 FROM
  cvtermpath AS p1,
  cvtermpath AS p2
 WHERE 
  p1.subject_id = p2.subject_id;

COMMENT ON VIEW common_descendant_cvterm IS 'The common descendant of
any two terms is the intersection of both terms descendants. Two terms
can have multiple common descendants. Use total_pathdistance to get
the least common ancestor';

CREATE OR REPLACE VIEW stats_paths_to_root AS
 SELECT 
  subject_id                            AS cvterm_id, 
  count(DISTINCT cvtermpath_id)         AS total_paths,
  avg(pathdistance)                     AS avg_distance,
  min(pathdistance)                     AS min_distance,
  max(pathdistance)                     AS max_distance
 FROM cvtermpath INNER JOIN cv_root ON (object_id=root_cvterm_id)
 GROUP BY cvterm_id;

COMMENT ON VIEW stats_paths_to_root IS 'per-cvterm statistics on its
placement in the DAG relative to the root. There may be multiple paths
from any term to the root. This gives the total number of paths, and
the average minimum and maximum distances. Here distance is defined by
cvtermpath.pathdistance';
CREATE VIEW cv_cvterm_count AS
  SELECT cv.name,count(*) AS num_terms_excl_obs FROM cv INNER JOIN cvterm USING (cv_id) WHERE is_obsolete=0 GROUP BY cv.name;
COMMENT ON VIEW cv_cvterm_count IS 'per-cv terms counts (excludes obsoletes)';

CREATE VIEW cv_cvterm_count_with_obs AS
  SELECT cv.name,count(*) AS num_terms_incl_obs FROM cv INNER JOIN cvterm USING (cv_id) GROUP BY cv.name;
COMMENT ON VIEW cv_cvterm_count_with_obs IS 'per-cv terms counts (includes obsoletes)';

CREATE VIEW cv_link_count AS
 SELECT cv.name AS cv_name,
        relation.name AS relation_name,
        relation_cv.name AS relation_cv_name,
        count(*) AS num_links
 FROM cv 
  INNER JOIN cvterm ON (cvterm.cv_id=cv.cv_id) 
  INNER JOIN cvterm_relationship ON (cvterm.cvterm_id=subject_id)
  INNER JOIN cvterm AS relation ON (type_id=relation.cvterm_id)
  INNER JOIN cv AS relation_cv ON (relation.cv_id=relation_cv.cv_id) 
 GROUP BY cv.name,relation.name,relation_cv.name;

COMMENT ON VIEW cv_link_count IS 'per-cv summary of number of
links (cvterm_relationships) broken down by
relationship_type. num_links is the total # of links of the specified
type in which the subject_id of the link is in the named cv';

CREATE VIEW cv_path_count AS
 SELECT cv.name AS cv_name,
        relation.name AS relation_name,
        relation_cv.name AS relation_cv_name,
        count(*) AS num_paths
 FROM cv 
  INNER JOIN cvterm ON (cvterm.cv_id=cv.cv_id) 
  INNER JOIN cvtermpath ON (cvterm.cvterm_id=subject_id)
  INNER JOIN cvterm AS relation ON (type_id=relation.cvterm_id)
  INNER JOIN cv AS relation_cv ON (relation.cv_id=relation_cv.cv_id) 
 GROUP BY cv.name,relation.name,relation_cv.name;

COMMENT ON VIEW cv_path_count IS 'per-cv summary of number of
paths (cvtermpaths) broken down by relationship_type. num_paths is the
total # of paths of the specified type in which the subject_id of the
path is in the named cv. See also: cv_distinct_relations';

CREATE OR REPLACE FUNCTION _get_all_subject_ids(bigint) RETURNS SETOF cvtermpath AS
'
DECLARE
    root alias for $1;
    cterm cvtermpath%ROWTYPE;
    cterm2 cvtermpath%ROWTYPE;
BEGIN

    FOR cterm IN SELECT * FROM cvterm_relationship WHERE object_id = root LOOP
        RETURN NEXT cterm;
        FOR cterm2 IN SELECT * FROM _get_all_subject_ids(cterm.subject_id) LOOP
            RETURN NEXT cterm2;
        END LOOP;
    END LOOP;
    RETURN;
END;   
'
LANGUAGE 'plpgsql';

---arg: parent term id
---return: all children term id and their parent term id with relationship type id
CREATE OR REPLACE FUNCTION get_all_subject_ids(bigint) RETURNS SETOF cvtermpath AS
'
DECLARE
    root alias for $1;
    cterm cvtermpath%ROWTYPE;
    exist_c int;
BEGIN

    SELECT INTO exist_c count(*) FROM cvtermpath WHERE object_id = root and pathdistance <= 0;
    IF (exist_c > 0) THEN
        FOR cterm IN SELECT * FROM cvtermpath WHERE object_id = root and pathdistance > 0 LOOP
            RETURN NEXT cterm;
        END LOOP;
    ELSE
        FOR cterm IN SELECT * FROM _get_all_subject_ids(root) LOOP
            RETURN NEXT cterm;
        END LOOP;
    END IF;
    RETURN;
END;   
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_graph_below(bigint) RETURNS SETOF cvtermpath AS
'
DECLARE
    root alias for $1;
    cterm cvtermpath%ROWTYPE;
    cterm2 cvtermpath%ROWTYPE;

BEGIN

    FOR cterm IN SELECT * FROM cvterm_relationship WHERE object_id = root LOOP
        RETURN NEXT cterm;
        FOR cterm2 IN SELECT * FROM get_all_subject_ids(cterm.subject_id) LOOP
            RETURN NEXT cterm2;
        END LOOP;
    END LOOP;
    RETURN;
END;   
'
LANGUAGE 'plpgsql';


CREATE OR REPLACE FUNCTION get_graph_above(bigint) RETURNS SETOF cvtermpath AS
'
DECLARE
    leaf alias for $1;
    cterm cvtermpath%ROWTYPE;
    cterm2 cvtermpath%ROWTYPE;

BEGIN

    FOR cterm IN SELECT * FROM cvterm_relationship WHERE subject_id = leaf LOOP
        RETURN NEXT cterm;
        FOR cterm2 IN SELECT * FROM get_all_object_ids(cterm.object_id) LOOP
            RETURN NEXT cterm2;
        END LOOP;
    END LOOP;
    RETURN;
END;   
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION _get_all_object_ids(bigint) RETURNS SETOF cvtermpath AS
'
DECLARE
    leaf alias for $1;
    cterm cvtermpath%ROWTYPE;
    cterm2 cvtermpath%ROWTYPE;
BEGIN

    FOR cterm IN SELECT * FROM cvterm_relationship WHERE subject_id = leaf LOOP
        RETURN NEXT cterm;
        FOR cterm2 IN SELECT * FROM _get_all_object_ids(cterm.object_id) LOOP
            RETURN NEXT cterm2;
        END LOOP;
    END LOOP;
    RETURN;
END;   
'
LANGUAGE 'plpgsql';

---arg: child term id
---return: all parent term id and their childrent term id with relationship type id
CREATE OR REPLACE FUNCTION get_all_object_ids(bigint) RETURNS SETOF cvtermpath AS
'
DECLARE
    leaf alias for $1;
    cterm cvtermpath%ROWTYPE;
    exist_c int;
BEGIN


    SELECT INTO exist_c count(*) FROM cvtermpath WHERE object_id = leaf and pathdistance <= 0;
    IF (exist_c > 0) THEN
        FOR cterm IN SELECT * FROM cvtermpath WHERE subject_id = leaf AND pathdistance > 0 LOOP
            RETURN NEXT cterm;
        END LOOP;
    ELSE
        FOR cterm IN SELECT * FROM _get_all_object_ids(leaf) LOOP
            RETURN NEXT cterm;
        END LOOP;
    END IF;
    RETURN;
END;   
'
LANGUAGE 'plpgsql';

---arg: sql statement which must be in the form of select cvterm_id from ...
---return: a set of cvterm ids that includes what is in sql statement and their children (subject ids)
CREATE OR REPLACE FUNCTION get_it_sub_cvterm_ids(text) RETURNS SETOF cvterm AS
'
DECLARE
    query alias for $1;
    cterm cvterm%ROWTYPE;
    cterm2 cvterm%ROWTYPE;
BEGIN
    FOR cterm IN EXECUTE query LOOP
        RETURN NEXT cterm;
        FOR cterm2 IN SELECT subject_id as cvterm_id FROM get_all_subject_ids(cterm.cvterm_id) LOOP
            RETURN NEXT cterm2;
        END LOOP;
    END LOOP;
    RETURN;
END;   
'
LANGUAGE 'plpgsql';
--- example: select * from fill_cvtermpath(7); where 7 is cv_id for an ontology
--- fill path from the node to its children and their children
CREATE OR REPLACE FUNCTION _fill_cvtermpath4node(BIGINT, BIGINT, BIGINT, BIGINT, INTEGER) RETURNS INTEGER AS
'
DECLARE
    origin alias for $1;
    child_id alias for $2;
    cvid alias for $3;
    typeid alias for $4;
    depth alias for $5;
    cterm cvterm_relationship%ROWTYPE;
    exist_c int;

BEGIN

    --- RAISE NOTICE ''depth=% root=%'', depth,child_id;
    --- not check type_id as it may be null and not very meaningful in cvtermpath when pathdistance > 1
    SELECT INTO exist_c count(*) FROM cvtermpath WHERE cv_id = cvid AND object_id = origin AND subject_id = child_id AND pathdistance = depth;

    IF (exist_c = 0) THEN
        INSERT INTO cvtermpath (object_id, subject_id, cv_id, type_id, pathdistance) VALUES(origin, child_id, cvid, typeid, depth);
    END IF;
    FOR cterm IN SELECT * FROM cvterm_relationship WHERE object_id = child_id LOOP
        PERFORM _fill_cvtermpath4node(origin, cterm.subject_id, cvid, cterm.type_id, depth+1);
    END LOOP;
    RETURN 1;
END;
'
LANGUAGE 'plpgsql';


CREATE OR REPLACE FUNCTION _fill_cvtermpath4root(BIGINT, BIGINT) RETURNS INTEGER AS
'
DECLARE
    rootid alias for $1;
    cvid alias for $2;
    ttype bigint;
    cterm cvterm_relationship%ROWTYPE;
    child cvterm_relationship%ROWTYPE;

BEGIN

    SELECT INTO ttype cvterm_id FROM cvterm WHERE (name = ''isa'' OR name = ''is_a'');
    PERFORM _fill_cvtermpath4node(rootid, rootid, cvid, ttype, 0);
    FOR cterm IN SELECT * FROM cvterm_relationship WHERE object_id = rootid LOOP
        PERFORM _fill_cvtermpath4root(cterm.subject_id, cvid);
        -- RAISE NOTICE ''DONE for term, %'', cterm.subject_id;
    END LOOP;
    RETURN 1;
END;
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION fill_cvtermpath(BIGINT) RETURNS INTEGER AS
'
DECLARE
    cvid alias for $1;
    root cvterm%ROWTYPE;

BEGIN

    DELETE FROM cvtermpath WHERE cv_id = cvid;

    FOR root IN SELECT DISTINCT t.* from cvterm t LEFT JOIN cvterm_relationship r ON (t.cvterm_id = r.subject_id) INNER JOIN cvterm_relationship r2 ON (t.cvterm_id = r2.object_id) WHERE t.cv_id = cvid AND r.subject_id is null LOOP
        PERFORM _fill_cvtermpath4root(root.cvterm_id, root.cv_id);
    END LOOP;
    RETURN 1;
END;   
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION fill_cvtermpath(cv.name%TYPE) RETURNS INTEGER AS
'
DECLARE
    cvname alias for $1;
    cv_id   bigint;
    rtn     int;
BEGIN

    SELECT INTO cv_id cv.cv_id from cv WHERE cv.name = cvname;
    SELECT INTO rtn fill_cvtermpath(cv_id);
    RETURN rtn;
END;   
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION _fill_cvtermpath4node2detect_cycle(BIGINT, BIGINT, BIGINT, BIGINT, INTEGER) RETURNS BIGINT AS
'
DECLARE
    origin alias for $1;
    child_id alias for $2;
    cvid alias for $3;
    typeid alias for $4;
    depth alias for $5;
    cterm cvterm_relationship%ROWTYPE;
    exist_c int;
    ccount  int;
    ecount  int;
    rtn     bigint;
BEGIN

    EXECUTE ''SELECT * FROM tmpcvtermpath p1, tmpcvtermpath p2 WHERE p1.subject_id=p2.object_id AND p1.object_id=p2.subject_id AND p1.object_id = ''|| origin || '' AND p2.subject_id = '' || child_id || ''AND '' || depth || ''> 0'';
    GET DIAGNOSTICS ccount = ROW_COUNT;
    IF (ccount > 0) THEN
        --RAISE EXCEPTION ''FOUND CYCLE: node % on cycle path'',origin;
        RETURN origin;
    END IF;

    EXECUTE ''SELECT * FROM tmpcvtermpath WHERE cv_id = '' || cvid || '' AND object_id = '' || origin || '' AND subject_id = '' || child_id || '' AND '' || origin || ''<>'' || child_id;
    GET DIAGNOSTICS ecount = ROW_COUNT;
    IF (ecount > 0) THEN
        --RAISE NOTICE ''FOUND TWICE (node), will check root obj % subj %'',origin, child_id;
        SELECT INTO rtn _fill_cvtermpath4root2detect_cycle(child_id, cvid);
        IF (rtn > 0) THEN
            RETURN rtn;
        END IF;
    END IF;

    EXECUTE ''SELECT * FROM tmpcvtermpath WHERE cv_id = '' || cvid || '' AND object_id = '' || origin || '' AND subject_id = '' || child_id || '' AND pathdistance = '' || depth;
    GET DIAGNOSTICS exist_c = ROW_COUNT;
    IF (exist_c = 0) THEN
        EXECUTE ''INSERT INTO tmpcvtermpath (object_id, subject_id, cv_id, type_id, pathdistance) VALUES('' || origin || '', '' || child_id || '', '' || cvid || '', '' || typeid || '', '' || depth || '')'';
    END IF;

    FOR cterm IN SELECT * FROM cvterm_relationship WHERE object_id = child_id LOOP
        --RAISE NOTICE ''DOING for node, % %'', origin, cterm.subject_id;
        SELECT INTO rtn _fill_cvtermpath4node2detect_cycle(origin, cterm.subject_id, cvid, cterm.type_id, depth+1);
        IF (rtn > 0) THEN
            RETURN rtn;
        END IF;
    END LOOP;
    RETURN 0;
END;
'
LANGUAGE 'plpgsql';


CREATE OR REPLACE FUNCTION _fill_cvtermpath4root2detect_cycle(BIGINT, BIGINT) RETURNS BIGINT AS
'
DECLARE
    rootid alias for $1;
    cvid alias for $2;
    ttype bigint;
    ccount int;
    cterm cvterm_relationship%ROWTYPE;
    child cvterm_relationship%ROWTYPE;
    rtn     bigint;
BEGIN

    SELECT INTO ttype cvterm_id FROM cvterm WHERE (name = ''isa'' OR name = ''is_a'');
    SELECT INTO rtn _fill_cvtermpath4node2detect_cycle(rootid, rootid, cvid, ttype, 0);
    IF (rtn > 0) THEN
        RETURN rtn;
    END IF;
    FOR cterm IN SELECT * FROM cvterm_relationship WHERE object_id = rootid LOOP
        EXECUTE ''SELECT * FROM tmpcvtermpath p1, tmpcvtermpath p2 WHERE p1.subject_id=p2.object_id AND p1.object_id=p2.subject_id AND p1.object_id='' || rootid || '' AND p1.subject_id='' || cterm.subject_id;
        GET DIAGNOSTICS ccount = ROW_COUNT;
        IF (ccount > 0) THEN
            --RAISE NOTICE ''FOUND TWICE (root), will check root obj % subj %'',rootid,cterm.subject_id;
            SELECT INTO rtn _fill_cvtermpath4node2detect_cycle(rootid, cterm.subject_id, cvid, ttype, 0);
            IF (rtn > 0) THEN
                RETURN rtn;
            END IF;
        ELSE
            SELECT INTO rtn _fill_cvtermpath4root2detect_cycle(cterm.subject_id, cvid);
            IF (rtn > 0) THEN
                RETURN rtn;
            END IF;
        END IF;
    END LOOP;
    RETURN 0;
END;
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_cycle_cvterm_id(BIGINT, BIGINT) RETURNS BIGINT AS
'
DECLARE
    cvid alias for $1;
    rootid alias for $2;
    rtn     bigint;
BEGIN

    CREATE TEMP TABLE tmpcvtermpath(object_id bigint, subject_id bigint, cv_id bigint, type_id bigint, pathdistance int);
    CREATE INDEX tmp_cvtpath1 ON tmpcvtermpath(object_id, subject_id);

    SELECT INTO rtn _fill_cvtermpath4root2detect_cycle(rootid, cvid);
    IF (rtn > 0) THEN
        DROP TABLE tmpcvtermpath;
        RETURN rtn;
    END IF;
    DROP TABLE tmpcvtermpath;
    RETURN 0;
END;   
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_cycle_cvterm_ids(BIGINT) RETURNS SETOF BIGINT AS
'
DECLARE
    cvid alias for $1;
    root cvterm%ROWTYPE;
    rtn     bigint;
BEGIN


    FOR root IN SELECT DISTINCT t.* from cvterm t WHERE cv_id = cvid LOOP
        SELECT INTO rtn get_cycle_cvterm_id(cvid,root.cvterm_id);
        IF (rtn > 0) THEN
            RETURN NEXT rtn;
        END IF;
    END LOOP;
    RETURN;
END;   
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_cycle_cvterm_id(BIGINT) RETURNS BIGINT AS
'
DECLARE
    cvid alias for $1;
    root cvterm%ROWTYPE;
    rtn     bigint;
BEGIN

    CREATE TEMP TABLE tmpcvtermpath(object_id bigint, subject_id bigint, cv_id bigint, type_id bigint, pathdistance int);
    CREATE INDEX tmp_cvtpath1 ON tmpcvtermpath(object_id, subject_id);

    FOR root IN SELECT DISTINCT t.* from cvterm t LEFT JOIN cvterm_relationship r ON (t.cvterm_id = r.subject_id) INNER JOIN cvterm_relationship r2 ON (t.cvterm_id = r2.object_id) WHERE t.cv_id = cvid AND r.subject_id is null LOOP
        SELECT INTO rtn _fill_cvtermpath4root2detect_cycle(root.cvterm_id, root.cv_id);
        IF (rtn > 0) THEN
            DROP TABLE tmpcvtermpath;
            RETURN rtn;
        END IF;
    END LOOP;
    DROP TABLE tmpcvtermpath;
    RETURN 0;
END;   
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_cycle_cvterm_id(cv.name%TYPE) RETURNS BIGINT AS
'
DECLARE
    cvname alias for $1;
    cv_id bigint;
    rtn bigint;
BEGIN

    SELECT INTO cv_id cv.cv_id from cv WHERE cv.name = cvname;
    SELECT INTO rtn  get_cycle_cvterm_id(cv_id);

    RETURN rtn;
END;   
'
LANGUAGE 'plpgsql';
-- $Id: contact.sql,v 1.5 2007-02-25 17:00:17 briano Exp $
-- ==========================================

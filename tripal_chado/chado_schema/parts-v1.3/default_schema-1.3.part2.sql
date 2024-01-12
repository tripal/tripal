SET search_path = chado,public;
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



CREATE OR REPLACE FUNCTION get_organism_id(VARCHAR,VARCHAR) RETURNS BIGINT
 AS '
  SELECT organism_id 
  FROM organism
  WHERE genus=$1
    AND species=$2
 ' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION get_organism_id(VARCHAR) RETURNS BIGINT
 AS ' 
SELECT organism_id
  FROM organism
  WHERE genus=substring($1,1,position('' '' IN $1)-1)
    AND species=substring($1,position('' '' IN $1)+1)
 ' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION get_organism_id_abbrev(VARCHAR) RETURNS BIGINT
 AS '
SELECT organism_id
  FROM organism
  WHERE substr(genus,1,1)=substring($1,1,1)
    AND species=substring($1,position('' '' IN $1)+1)
 ' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION store_organism (VARCHAR,VARCHAR,VARCHAR) 
  RETURNS BIGINT AS 
'DECLARE
   v_genus            ALIAS FOR $1;
   v_species          ALIAS FOR $2;
   v_common_name      ALIAS FOR $3;

   v_organism_id      BIGINT;
 BEGIN
    SELECT INTO v_organism_id organism_id
      FROM organism
      WHERE genus=v_genus               AND
            species=v_species;
    IF NOT FOUND THEN
      INSERT INTO organism
       (genus,species,common_name)
         VALUES
       (v_genus,v_species,v_common_name);
       RETURN currval(''organism_organism_id_seq'');
    ELSE
      UPDATE organism
       SET common_name=v_common_name
      WHERE organism_id = v_organism_id;
    END IF;
    RETURN v_organism_id;
 END;
' LANGUAGE 'plpgsql';
  
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
create index feature_idx1b on feature (feature_id, dbxref_id) where dbxref_id is not null;

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
create index featureloc_idx1b on featureloc (feature_id, fmin, fmax);

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
create index feature_relationship_idx1b on feature_relationship (object_id, subject_id, type_id);

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

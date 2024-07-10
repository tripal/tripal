SET search_path = chado,pg_catalog;
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

CREATE OR REPLACE FUNCTION store_analysis (VARCHAR,VARCHAR,VARCHAR) 
  RETURNS BIGINT AS 
'DECLARE
   v_program            ALIAS FOR $1;
   v_programversion     ALIAS FOR $2;
   v_sourcename         ALIAS FOR $3;
   pkval                BIGINT;
 BEGIN
    SELECT INTO pkval analysis_id
      FROM analysis
      WHERE program=v_program AND
            programversion=v_programversion AND
            sourcename=v_sourcename;
    IF NOT FOUND THEN
      INSERT INTO analysis 
       (program,programversion,sourcename)
         VALUES
       (v_program,v_programversion,v_sourcename);
      RETURN currval(''analysis_analysis_id_seq'');
    END IF;
    RETURN pkval;
 END;
' LANGUAGE 'plpgsql';

--CREATE OR REPLACE FUNCTION store_analysisfeature
--()
--RETURNS INT AS
--'DECLARE
--  v_srcfeature_id       ALIAS FOR $1;
  
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

CREATE OR REPLACE FUNCTION phylonode_depth(bigint)
 RETURNS FLOAT AS
 'DECLARE  id    ALIAS FOR $1;
  DECLARE  depth FLOAT := 0;
  DECLARE  curr_node phylonode%ROWTYPE;
  BEGIN
   SELECT INTO curr_node *
    FROM phylonode 
    WHERE phylonode_id=id;
   depth = depth + curr_node.distance;
   IF curr_node.parent_phylonode_id IS NULL
    THEN RETURN depth;
    ELSE RETURN depth + phylonode_depth(curr_node.parent_phylonode_id);
   END IF;
 END
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION phylonode_height(bigint)
 RETURNS FLOAT AS
'
  SELECT coalesce(max(phylonode_height(phylonode_id) + distance), 0.0)
    FROM phylonode
    WHERE parent_phylonode_id = $1
'
LANGUAGE 'sql';

-- $Id: expression.sql,v 1.14 2007-03-23 15:18:02 scottcain Exp $
-- ==========================================

--
-- Note by Lacey-anne Sanderson:
-- File contains views associated with the chado schema.
--
-- ==========================================
-- Chado general module
--
-- =================================================================
-- Dependencies:
--
--    None
-- =================================================================

CREATE VIEW db_dbxref_count AS
  SELECT db.name,count(*) AS num_dbxrefs FROM db INNER JOIN dbxref USING (db_id) GROUP BY db.name;
COMMENT ON VIEW db_dbxref_count IS 'per-db dbxref counts';

-- ==========================================
-- Chado cv module
--
-- =================================================================
-- Dependencies:
--
-- :import dbxref from db
-- =================================================================

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

CREATE VIEW type_feature_count AS
  SELECT t.name AS type,count(*) AS num_features
   FROM cvterm AS t INNER JOIN feature ON (type_id=t.cvterm_id)
  GROUP BY t.name;
COMMENT ON VIEW type_feature_count IS 'per-feature-type feature counts';

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

-- VIEW gffatts: a view to get feature attributes in a format that
-- will make it easy to convert them to GFF attributes

CREATE OR REPLACE VIEW gffatts (
    feature_id,
    type,
    attribute
) AS
SELECT feature_id, 'Ontology_term' AS type,  s.name AS attribute
FROM cvterm s, feature_cvterm fs
WHERE fs.cvterm_id = s.cvterm_id
UNION ALL
SELECT feature_id, 'Dbxref' AS type, d.name || ':' || s.accession AS attribute
FROM dbxref s, feature_dbxref fs, db d
WHERE fs.dbxref_id = s.dbxref_id and s.db_id = d.db_id
UNION ALL
SELECT feature_id, 'Alias' AS type, s.name AS attribute
FROM synonym s, feature_synonym fs
WHERE fs.synonym_id = s.synonym_id
UNION ALL
SELECT fp.feature_id,cv.name,fp.value
FROM featureprop fp, cvterm cv
WHERE fp.type_id = cv.cvterm_id
UNION ALL
SELECT feature_id, 'pub' AS type, s.series_name || ':' || s.title AS attribute
FROM pub s, feature_pub fs
WHERE fs.pub_id = s.pub_id;

--creates a view that can be used to assemble a GFF3 compliant attribute string
CREATE OR REPLACE VIEW gff3atts (
    feature_id,
    type,
    attribute
) AS
SELECT feature_id,
      'Ontology_term' AS type,
      CASE WHEN db.name like '%Gene Ontology%'    THEN 'GO:'|| dbx.accession
           WHEN db.name like 'Sequence Ontology%' THEN 'SO:'|| dbx.accession
           ELSE                            CAST(db.name||':'|| dbx.accession AS varchar)
      END
FROM cvterm s, dbxref dbx, feature_cvterm fs, db
WHERE fs.cvterm_id = s.cvterm_id and s.dbxref_id=dbx.dbxref_id and
      db.db_id = dbx.db_id
UNION ALL
SELECT feature_id, 'Dbxref' AS type, d.name || ':' || s.accession AS
attribute
FROM dbxref s, feature_dbxref fs, db d
WHERE fs.dbxref_id = s.dbxref_id and s.db_id = d.db_id and
      d.name != 'GFF_source'
UNION ALL
SELECT f.feature_id, 'Alias' AS type, s.name AS attribute
FROM synonym s, feature_synonym fs, feature f
WHERE fs.synonym_id = s.synonym_id and f.feature_id = fs.feature_id and
      f.name != s.name and f.uniquename != s.name
UNION ALL
SELECT fp.feature_id,cv.name,fp.value
FROM featureprop fp, cvterm cv
WHERE fp.type_id = cv.cvterm_id
UNION ALL
SELECT feature_id, 'pub' AS type, s.series_name || ':' || s.title AS
attribute
FROM pub s, feature_pub fs
WHERE fs.pub_id = s.pub_id
UNION ALL
SELECT fr.subject_id as feature_id, 'Parent' as type,  parent.uniquename
as attribute
FROM feature_relationship fr, feature parent
WHERE  fr.object_id=parent.feature_id AND fr.type_id = (SELECT cvterm_id
FROM cvterm WHERE name='part_of' and cv_id in (select cv_id
  FROM cv WHERE name='relationship'))
UNION ALL
SELECT fr.subject_id as feature_id, 'Derives_from' as type,
parent.uniquename as attribute
FROM feature_relationship fr, feature parent
WHERE  fr.object_id=parent.feature_id AND fr.type_id = (SELECT cvterm_id
FROM cvterm WHERE name='derives_from' and cv_id in (select cv_id
  FROM cv WHERE name='relationship'))
UNION ALL
SELECT fl.feature_id, 'Target' as type, target.name || ' ' || fl.fmin+1
|| ' ' || fl.fmax || ' ' || fl.strand as attribute
FROM featureloc fl, feature target
WHERE fl.srcfeature_id=target.feature_id
        AND fl.rank != 0
UNION ALL
SELECT feature_id, 'ID' as type, uniquename as attribute
FROM feature
WHERE type_id NOT IN (SELECT cvterm_id FROM cvterm WHERE name='CDS')
UNION ALL
SELECT feature_id, 'chado_feature_id' as type, CAST(feature_id AS
varchar) as attribute
FROM feature
UNION ALL
SELECT feature_id, 'Name' as type, name as attribute
FROM feature;


--replaced with Rob B's improved view
CREATE OR REPLACE VIEW gff3view (
feature_id, ref, source, type, fstart, fend,
score, strand, phase, seqlen, name, organism_id
) AS
SELECT
f.feature_id, sf.name,
 COALESCE(gffdbx.accession,'.'::varchar(255)), cv.name,
fl.fmin+1, fl.fmax,
 COALESCE(CAST(af.significance AS text), '.'),
 CASE WHEN fl.strand=-1 THEN '-'
      WHEN fl.strand=1  THEN '+'
      ELSE '.'
 END,
 COALESCE(CAST(fl.phase AS text), '.'), f.seqlen, f.name, f.organism_id
FROM feature f
LEFT JOIN featureloc fl ON (f.feature_id = fl.feature_id)
LEFT JOIN feature sf ON (fl.srcfeature_id = sf.feature_id)
LEFT JOIN ( SELECT fd.feature_id, d.accession
FROM feature_dbxref fd
JOIN dbxref d using(dbxref_id)
JOIN db using(db_id)
WHERE db.name = 'GFF_source'
) as gffdbx
ON (f.feature_id=gffdbx.feature_id)
LEFT JOIN cvterm cv ON (f.type_id = cv.cvterm_id)
LEFT JOIN analysisfeature af ON (f.feature_id = af.feature_id);

--------------------------------
---- all_feature_names ---------
--------------------------------
-- This is a view to replace the denormaliziation of the synonym
-- table.  It contains names and uniquenames from feature and
-- synonym.names from the synonym table, so that GBrowse has one
-- place to search for names.
--
-- To materialize this view, run gmod_materialized_view_tool.pl -c and
-- answer the questions with these responses:
--
--   all_feature_names
--
--   public.all_feature_names
--
--   y   (yes, replace the existing view)
--
--   (some update frequency, I chose daily)
--
--   feature_id bigint,name varchar(255),organism_id bigint
--
--   (the select part of the view below, all on one line)
--
--   feature_id,name
--
--   create index all_feature_names_lower_name on all_feature_names (lower(name))
--
--   y
--
-- OR, you could execute this command (the materialized view tool has been
-- updated to allow this all to be supplied on the command line):
--
-- (yes, it's all one really long line, to make copy and pasting easier)
-- gmod_materialized_view_tool.pl --create_view --view_name all_feature_names --table_name public.all_feature_names --refresh_time daily --column_def "feature_id bigint,name varchar(255),organism_id bigint" --sql_query "SELECT feature_id,CAST(substring(uniquename from 0 for 255) as varchar(255)) as name,organism_id FROM feature UNION SELECT feature_id, name, organism_id FROM feature where name is not null UNION SELECT fs.feature_id,s.name,f.organism_id FROM feature_synonym fs, synonym s, feature f WHERE fs.synonym_id = s.synonym_id AND fs.feature_id = f.feature_id UNION SELECT fp.feature_id, CAST(substring(fp.value from 0 for 255) as varchar(255)) as name,f.organism_id FROM featureprop fp, feature f WHERE f.feature_id = fp.feature_id UNION SELECT fd.feature_id, d.accession, f.organism_id FROM feature_dbxref fd, dbxref d,feature f WHERE fd.dbxref_id = d.dbxref_id AND fd.feature_id = f.feature_id" --index_fields "feature_id,name" --special_index "create index all_feature_names_lower_name on all_feature_names (lower(name))" --yes
--
--
-- OR, even more complicated, you could use this command to create a materialized view
-- for use with full text searching on PostgreSQL 8.4 or better:
--
-- gmod_materialized_view_tool.pl --create_view --view_name all_feature_names --table_name public.all_feature_names --refresh_time daily --column_def "feature_id bigint,name varchar(255),organism_id bigint,searchable_name tsvector" --sql_query "SELECT feature_id, CAST(substring(uniquename FROM 0 FOR 255) AS varchar(255)) AS name, organism_id, to_tsvector('english', CAST(substring(uniquename FROM 0 FOR 255) AS varchar(255))) AS searchable_name FROM feature UNION SELECT feature_id, name, organism_id, to_tsvector('english', name) AS searchable_name FROM feature WHERE name IS NOT NULL UNION SELECT fs.feature_id, s.name, f.organism_id, to_tsvector('english', s.name) AS searchable_name FROM feature_synonym fs, synonym s, feature f WHERE fs.synonym_id = s.synonym_id AND fs.feature_id = f.feature_id UNION SELECT fp.feature_id, CAST(substring(fp.value FROM 0 FOR 255) AS varchar(255)) AS name, f.organism_id, to_tsvector('english',CAST(substring(fp.value FROM 0 FOR 255) AS varchar(255))) AS searchable_name FROM featureprop fp, feature f WHERE f.feature_id = fp.feature_id UNION SELECT fd.feature_id, d.accession, f.organism_id,to_tsvector('english',d.accession) AS searchable_name FROM feature_dbxref fd, dbxref d,feature f WHERE fd.dbxref_id = d.dbxref_id AND fd.feature_id = f.feature_id" --index_fields "feature_id,name" --special_index "CREATE INDEX searchable_all_feature_names_idx ON all_feature_names USING gin(searchable_name)" --yes
--
CREATE OR REPLACE VIEW all_feature_names (
  feature_id,
  name,
  organism_id
) AS
SELECT feature_id,CAST(substring(uniquename from 0 for 255) as varchar(255)) as name,organism_id FROM feature
UNION
SELECT feature_id, name, organism_id FROM feature where name is not null
UNION
SELECT fs.feature_id,s.name,f.organism_id FROM feature_synonym fs, synonym s, feature f
  WHERE fs.synonym_id = s.synonym_id AND fs.feature_id = f.feature_id
UNION
SELECT fp.feature_id, CAST(substring(fp.value from 0 for 255) as varchar(255)) as name,f.organism_id FROM featureprop fp, feature f
  WHERE f.feature_id = fp.feature_id
UNION
SELECT fd.feature_id, d.accession, f.organism_id FROM feature_dbxref fd, dbxref d,feature f
  WHERE fd.dbxref_id = d.dbxref_id AND fd.feature_id = f.feature_id;

--------------------------------
---- dfeatureloc ---------------
--------------------------------
-- dfeatureloc is meant as an alternate representation of
-- the data in featureloc (see the descrption of featureloc
-- in sequence.sql).  In dfeatureloc, fmin and fmax are
-- replaced with nbeg and nend.  Whereas fmin and fmax
-- are absolute coordinates relative to the parent feature, nbeg
-- and nend are the beginning and ending coordinates
-- relative to the feature itself.  For example, nbeg would
-- mark the 5' end of a gene and nend would mark the 3' end.

CREATE OR REPLACE VIEW dfeatureloc (
 featureloc_id,
 feature_id,
 srcfeature_id,
 nbeg,
 is_nbeg_partial,
 nend,
 is_nend_partial,
 strand,
 phase,
 residue_info,
 locgroup,
 rank
) AS
SELECT featureloc_id, feature_id, srcfeature_id, fmin, is_fmin_partial,
       fmax, is_fmax_partial, strand, phase, residue_info, locgroup, rank
FROM featureloc
WHERE (strand < 0 or phase < 0)
UNION
SELECT featureloc_id, feature_id, srcfeature_id, fmax, is_fmax_partial,
       fmin, is_fmin_partial, strand, phase, residue_info, locgroup, rank
FROM featureloc
WHERE (strand is NULL or strand >= 0 or phase >= 0) ;

--------------------------------
---- f_type --------------------
--------------------------------
CREATE OR REPLACE VIEW f_type
AS
  SELECT  f.feature_id,
          f.name,
          f.dbxref_id,
          c.name AS type,
          f.residues,
          f.seqlen,
          f.md5checksum,
          f.type_id,
          f.timeaccessioned,
          f.timelastmodified
    FROM  feature f, cvterm c
   WHERE  f.type_id = c.cvterm_id;

--------------------------------
---- fnr_type ------------------
--------------------------------
CREATE OR REPLACE VIEW fnr_type
AS
  SELECT  f.feature_id,
          f.name,
          f.dbxref_id,
          c.name AS type,
          f.residues,
          f.seqlen,
          f.md5checksum,
          f.type_id,
          f.timeaccessioned,
          f.timelastmodified
    FROM  feature f left outer join analysisfeature af
          on (f.feature_id = af.feature_id), cvterm c
   WHERE  f.type_id = c.cvterm_id
          and af.feature_id is null;

--------------------------------
---- f_loc ---------------------
--------------------------------
-- Note from Scott:  I changed this view to depend on dfeatureloc,
-- since I don't know what it is used for.  The change should
-- be transparent.  I also changed dbxrefstr to dbxref_id since
-- dbxrefstr is no longer in feature

CREATE OR REPLACE VIEW f_loc
AS
  SELECT  f.feature_id,
          f.name,
          f.dbxref_id,
          fl.nbeg,
          fl.nend,
          fl.strand
    FROM  dfeatureloc fl, f_type f
   WHERE  f.feature_id = fl.feature_id;

--------------------------------
---- fp_key -------------------
--------------------------------
CREATE OR REPLACE VIEW fp_key
AS
  SELECT  fp.feature_id,
          c.name AS pkey,
          fp.value
    FROM  featureprop fp, cvterm c
   WHERE  fp.featureprop_id = c.cvterm_id;

-- [symmetric,reflexive]
-- intervals have at least one interbase point in common
-- (i.e. overlap OR abut)
-- EXAMPLE QUERY:
--   (features of same type that overlap)
--   SELECT r.*
--   FROM feature AS x
--   INNER JOIN feature_meets AS r ON (x.feature_id=r.subject_id)
--   INNER JOIN feature AS y ON (y.feature_id=r.object_id)
--   WHERE x.type_id=y.type_id
CREATE OR REPLACE VIEW feature_meets (
  subject_id,
  object_id
) AS
SELECT
 x.feature_id,
 y.feature_id
FROM
 featureloc AS x,
 featureloc AS y
WHERE
 x.srcfeature_id=y.srcfeature_id
 AND
 ( x.fmax >= y.fmin AND x.fmin <= y.fmax );

COMMENT ON VIEW feature_meets IS 'intervals have at least one
interbase point in common (ie overlap OR abut). symmetric,reflexive';

-- [symmetric,reflexive]
-- as above, strands match
CREATE OR REPLACE VIEW feature_meets_on_same_strand (
  subject_id,
  object_id
) AS
SELECT
 x.feature_id,
 y.feature_id
FROM
 featureloc AS x,
 featureloc AS y
WHERE
 x.srcfeature_id=y.srcfeature_id
 AND
 x.strand = y.strand
 AND
 ( x.fmax >= y.fmin AND x.fmin <= y.fmax );

COMMENT ON VIEW feature_meets_on_same_strand IS 'as feature_meets, but
featurelocs must be on the same strand. symmetric,reflexive';


-- [symmetric]
-- intervals have no interbase points in common and do not abut
CREATE OR REPLACE VIEW feature_disjoint (
  subject_id,
  object_id
) AS
SELECT
 x.feature_id,
 y.feature_id
FROM
 featureloc AS x,
 featureloc AS y
WHERE
 x.srcfeature_id=y.srcfeature_id
 AND
 ( x.fmax < y.fmin AND x.fmin > y.fmax );

COMMENT ON VIEW feature_disjoint IS 'featurelocs do not meet. symmetric';

-- 4-ary relation
CREATE OR REPLACE VIEW feature_union AS
SELECT
  x.feature_id  AS subject_id,
  y.feature_id  AS object_id,
  x.srcfeature_id,
  x.strand      AS subject_strand,
  y.strand      AS object_strand,
  CASE WHEN x.fmin<y.fmin THEN x.fmin ELSE y.fmin END AS fmin,
  CASE WHEN x.fmax>y.fmax THEN x.fmax ELSE y.fmax END AS fmax
FROM
 featureloc AS x,
 featureloc AS y
WHERE
 x.srcfeature_id=y.srcfeature_id
 AND
 ( x.fmax >= y.fmin AND x.fmin <= y.fmax );

COMMENT ON VIEW feature_union IS 'set-union on interval defined by featureloc. featurelocs must meet';


-- 4-ary relation
CREATE OR REPLACE VIEW feature_intersection AS
SELECT
  x.feature_id  AS subject_id,
  y.feature_id  AS object_id,
  x.srcfeature_id,
  x.strand      AS subject_strand,
  y.strand      AS object_strand,
  CASE WHEN x.fmin<y.fmin THEN y.fmin ELSE x.fmin END AS fmin,
  CASE WHEN x.fmax>y.fmax THEN y.fmax ELSE x.fmax END AS fmax
FROM
 featureloc AS x,
 featureloc AS y
WHERE
 x.srcfeature_id=y.srcfeature_id
 AND
 ( x.fmax >= y.fmin AND x.fmin <= y.fmax );

COMMENT ON VIEW feature_intersection IS 'set-intersection on interval defined by featureloc. featurelocs must meet';

-- 4-ary relation
-- subtract object interval from subject interval
--  (may leave zero, one or two intervals)
CREATE OR REPLACE VIEW feature_difference (
  subject_id,
  object_id,
  srcfeature_id,
  fmin,
  fmax,
  strand
) AS
-- left interval
SELECT
  x.feature_id,
  y.feature_id,
  x.strand,
  x.srcfeature_id,
  x.fmin,
  y.fmin
FROM
 featureloc AS x,
 featureloc AS y
WHERE
 x.srcfeature_id=y.srcfeature_id
 AND
 (x.fmin < y.fmin AND x.fmax >= y.fmax )
UNION
-- right interval
SELECT
  x.feature_id,
  y.feature_id,
  x.strand,
  x.srcfeature_id,
  y.fmax,
  x.fmax
FROM
 featureloc AS x,
 featureloc AS y
WHERE
 x.srcfeature_id=y.srcfeature_id
 AND
 (x.fmax > y.fmax AND x.fmin <= y.fmin );

COMMENT ON VIEW feature_difference IS 'set-distance on interval defined by featureloc. featurelocs must meet';

-- 4-ary relation
CREATE OR REPLACE VIEW feature_distance AS
SELECT
  x.feature_id  AS subject_id,
  y.feature_id  AS object_id,
  x.srcfeature_id,
  x.strand      AS subject_strand,
  y.strand      AS object_strand,
  CASE WHEN x.fmax <= y.fmin THEN (x.fmax-y.fmin) ELSE (y.fmax-x.fmin) END AS distance
FROM
 featureloc AS x,
 featureloc AS y
WHERE
 x.srcfeature_id=y.srcfeature_id
 AND
 ( x.fmax <= y.fmin OR x.fmin >= y.fmax );

COMMENT ON VIEW feature_difference IS 'size of gap between two features. must be abutting or disjoint';

-- [transitive,reflexive]
-- (should this be made non-reflexive?)
-- subject intervals contains (or is same as) object interval
CREATE OR REPLACE VIEW feature_contains (
  subject_id,
  object_id
) AS
SELECT
 x.feature_id,
 y.feature_id
FROM
 featureloc AS x,
 featureloc AS y
WHERE
 x.srcfeature_id=y.srcfeature_id
 AND
 ( y.fmin >= x.fmin AND y.fmin <= x.fmax );

COMMENT ON VIEW feature_contains IS 'subject intervals contains (or is
same as) object interval. transitive,reflexive';

-- featureset relations:
--  a featureset relation is true between any two features x and y
--  if the relation is true for any x' and y' where x' and y' are
--  subfeatures of x and y

-- see feature_meets
-- example: two transcripts meet if any of their exons or CDSs overlap
-- or abut
CREATE OR REPLACE VIEW featureset_meets (
  subject_id,
  object_id
) AS
SELECT
 x.object_id,
 y.object_id
FROM
 feature_meets AS r
 INNER JOIN feature_relationship AS x ON (r.subject_id = x.subject_id)
 INNER JOIN feature_relationship AS y ON (r.object_id = y.subject_id);


-- ==========================================
-- SO Schema
--
-- =================================================================

-- The standard Chado pattern for protein coding genes
-- is a feature of type 'gene' with 'mRNA' features as parts
-- REQUIRES: 'mrna' view from so-bridge.sql
CREATE OR REPLACE VIEW protein_coding_gene AS
 SELECT
  DISTINCT gene.*
 FROM
  feature AS gene
  INNER JOIN feature_relationship AS fr ON (gene.feature_id=fr.object_id)
  INNER JOIN so.mrna ON (mrna.feature_id=fr.subject_id);


-- introns are implicit from surrounding exons
-- combines intron features with location and parent transcript
-- the same intron appearing in multiple transcripts will appear
-- multiple times
CREATE VIEW intron_combined_view AS
 SELECT
  x1.feature_id         AS exon1_id,
  x2.feature_id         AS exon2_id,
  CASE WHEN l1.strand=-1  THEN l2.fmax ELSE l1.fmax END AS fmin,
  CASE WHEN l1.strand=-1  THEN l1.fmin ELSE l2.fmin END AS fmax,
  l1.strand             AS strand,
  l1.srcfeature_id      AS srcfeature_id,
  r1.rank               AS intron_rank,
  r1.object_id          AS transcript_id
 FROM
 cvterm
  INNER JOIN
   feature                AS x1    ON (x1.type_id=cvterm.cvterm_id)
    INNER JOIN
     feature_relationship AS r1    ON (x1.feature_id=r1.subject_id)
    INNER JOIN
     featureloc           AS l1    ON (x1.feature_id=l1.feature_id)
  INNER JOIN
   feature                AS x2    ON (x2.type_id=cvterm.cvterm_id)
    INNER JOIN
     feature_relationship AS r2    ON (x2.feature_id=r2.subject_id)
    INNER JOIN
     featureloc           AS l2    ON (x2.feature_id=l2.feature_id)
 WHERE
  cvterm.name='exon'            AND
  (r2.rank - r1.rank) = 1       AND
  r1.object_id=r2.object_id     AND
  l1.strand = l2.strand         AND
  l1.srcfeature_id = l2.srcfeature_id         AND
  l1.locgroup=0                 AND
  l2.locgroup=0;

-- intron locations. intron IDs are the (exon1,exon2) ID pair
-- this means that introns may be counted twice if the start of
-- the 5' exon or the end of the 3' exon vary
-- introns shared by transcripts will not appear twice
CREATE VIEW intronloc_view AS
 SELECT DISTINCT
  exon1_id,
  exon2_id,
  fmin,
  fmax,
  strand,
  srcfeature_id
 FROM intron_combined_view;

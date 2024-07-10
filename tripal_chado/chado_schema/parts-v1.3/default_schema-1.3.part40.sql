SET search_path = chado,pg_catalog;
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

-- FUNCTION gfffeatureatts (integer) is a function to get 
-- data in the same format as the gffatts view so that 
-- it can be easily converted to GFF attributes.

CREATE FUNCTION  gfffeatureatts (bigint)
RETURNS SETOF gffatts
AS
'
SELECT feature_id, ''Ontology_term'' AS type,  s.name AS attribute
FROM cvterm s, feature_cvterm fs
WHERE fs.feature_id= $1 AND fs.cvterm_id = s.cvterm_id
UNION
SELECT feature_id, ''Dbxref'' AS type, d.name || '':'' || s.accession AS attribute
FROM dbxref s, feature_dbxref fs, db d
WHERE fs.feature_id= $1 AND fs.dbxref_id = s.dbxref_id AND s.db_id = d.db_id
UNION
SELECT feature_id, ''Alias'' AS type, s.name AS attribute
FROM synonym s, feature_synonym fs
WHERE fs.feature_id= $1 AND fs.synonym_id = s.synonym_id
UNION
SELECT fp.feature_id,cv.name,fp.value
FROM featureprop fp, cvterm cv
WHERE fp.feature_id= $1 AND fp.type_id = cv.cvterm_id 
UNION
SELECT feature_id, ''pub'' AS type, s.series_name || '':'' || s.title AS attribute
FROM pub s, feature_pub fs
WHERE fs.feature_id= $1 AND fs.pub_id = s.pub_id
'
LANGUAGE SQL;


--
-- functions for creating coordinate based functions
--
-- create a point
CREATE OR REPLACE FUNCTION featureslice(bigint, bigint) RETURNS setof featureloc AS
  'SELECT * from featureloc where boxquery($1, $2) <@ boxrange(fmin,fmax)'
LANGUAGE 'sql';

--uses the gff3atts to create a GFF3 compliant attribute string
CREATE OR REPLACE FUNCTION gffattstring (bigint) RETURNS varchar AS
'DECLARE
  return_string      varchar;
  f_id               ALIAS FOR $1;
  atts_view          gffatts%ROWTYPE;
  feature_row        feature%ROWTYPE;
  name               varchar;
  uniquename         varchar;
  parent             varchar;
  escape_loc         bigint; 
BEGIN
  --Get name from feature.name
  --Get ID from feature.uniquename
                                                                                
  SELECT INTO feature_row * FROM feature WHERE feature_id = f_id;
  name  = feature_row.name;
  return_string = ''ID='' || feature_row.uniquename;
  IF name IS NOT NULL AND name != ''''
  THEN
    return_string = return_string ||'';'' || ''Name='' || name;
  END IF;
                                                                                
  --Get Parent from feature_relationship
  SELECT INTO feature_row * FROM feature f, feature_relationship fr
    WHERE fr.subject_id = f_id AND fr.object_id = f.feature_id;
  IF FOUND
  THEN
    return_string = return_string||'';''||''Parent=''||feature_row.uniquename;
  END IF;
                                                                                
  FOR atts_view IN SELECT * FROM gff3atts WHERE feature_id = f_id  LOOP
    escape_loc = position('';'' in atts_view.attribute);
    IF escape_loc > 0 THEN
      atts_view.attribute = replace(atts_view.attribute, '';'', ''%3B'');
    END IF;
    return_string = return_string || '';''
                     || atts_view.type || ''=''
                     || atts_view.attribute;
  END LOOP;
                                                                                
  RETURN return_string;
END;
'
LANGUAGE plpgsql;

--creates a view that is suitable for creating a GFF3 string
--CREATE OR REPLACE VIEW gff3view (
--REMOVED and RECREATED in sequence-gff-views.sql to avoid 
--using the function above
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

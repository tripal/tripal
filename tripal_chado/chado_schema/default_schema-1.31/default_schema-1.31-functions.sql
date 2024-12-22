--
-- Note by Lacey-anne Sanderson:
-- File contains functions associated with the chado schema.
--
-- ==========================================
-- Chado general module
--
-- =================================================================
-- Dependencies:
--
--    None
-- =================================================================

CREATE OR REPLACE FUNCTION store_db (VARCHAR)
  RETURNS INT AS
'DECLARE
   v_name             ALIAS FOR $1;
   v_db_id            INTEGER;
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
  RETURNS INT AS
'DECLARE
   v_dbname                ALIAS FOR $1;
   v_accession             ALIAS FOR $2;
   v_db_id                 INTEGER;
   v_dbxref_id             INTEGER;
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

-- ==========================================
-- Chado cv module
--
-- =================================================================
-- Dependencies:
--
-- :import dbxref from db
-- =================================================================

CREATE OR REPLACE FUNCTION _get_all_subject_ids(integer) RETURNS SETOF cvtermpath AS
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
CREATE OR REPLACE FUNCTION get_all_subject_ids(integer) RETURNS SETOF cvtermpath AS
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

CREATE OR REPLACE FUNCTION get_graph_below(integer) RETURNS SETOF cvtermpath AS
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

CREATE OR REPLACE FUNCTION get_graph_above(integer) RETURNS SETOF cvtermpath AS
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

CREATE OR REPLACE FUNCTION _get_all_object_ids(integer) RETURNS SETOF cvtermpath AS
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
CREATE OR REPLACE FUNCTION get_all_object_ids(integer) RETURNS SETOF cvtermpath AS
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
CREATE OR REPLACE FUNCTION _fill_cvtermpath4node(INTEGER, INTEGER, INTEGER, INTEGER, INTEGER) RETURNS INTEGER AS
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

CREATE OR REPLACE FUNCTION _fill_cvtermpath4root(INTEGER, INTEGER) RETURNS INTEGER AS
'
DECLARE
    rootid alias for $1;
    cvid alias for $2;
    ttype int;
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

CREATE OR REPLACE FUNCTION fill_cvtermpath(INTEGER) RETURNS INTEGER AS
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
    cv_id   int;
    rtn     int;
BEGIN
    SELECT INTO cv_id cv.cv_id from cv WHERE cv.name = cvname;
    SELECT INTO rtn fill_cvtermpath(cv_id);
    RETURN rtn;
END;
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION _fill_cvtermpath4node2detect_cycle(INTEGER, INTEGER, INTEGER, INTEGER, INTEGER) RETURNS INTEGER AS
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
    rtn     int;
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

CREATE OR REPLACE FUNCTION _fill_cvtermpath4root2detect_cycle(INTEGER, INTEGER) RETURNS INTEGER AS
'
DECLARE
    rootid alias for $1;
    cvid alias for $2;
    ttype int;
    ccount int;
    cterm cvterm_relationship%ROWTYPE;
    child cvterm_relationship%ROWTYPE;
    rtn     int;
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

CREATE OR REPLACE FUNCTION get_cycle_cvterm_id(INTEGER, INTEGER) RETURNS INTEGER AS
'
DECLARE
    cvid alias for $1;
    rootid alias for $2;
    rtn     int;
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

CREATE OR REPLACE FUNCTION get_cycle_cvterm_ids(INTEGER) RETURNS SETOF INTEGER AS
'
DECLARE
    cvid alias for $1;
    root cvterm%ROWTYPE;
    rtn     int;
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

CREATE OR REPLACE FUNCTION get_cycle_cvterm_id(INTEGER) RETURNS INTEGER AS
'
DECLARE
    cvid alias for $1;
    root cvterm%ROWTYPE;
    rtn     int;
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

CREATE OR REPLACE FUNCTION get_cycle_cvterm_id(cv.name%TYPE) RETURNS INTEGER AS
'
DECLARE
    cvname alias for $1;
    cv_id bigint;
    rtn int;
BEGIN
    SELECT INTO cv_id cv.cv_id from cv WHERE cv.name = cvname;
    SELECT INTO rtn  get_cycle_cvterm_id(cv_id);
    RETURN rtn;
END;
'
LANGUAGE 'plpgsql';

-- ==========================================
-- Chado organism module
--
-- ============
-- DEPENDENCIES
-- ============
-- :import cvterm from cv
-- :import dbxref from db
-- ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
  RETURNS INT AS
'DECLARE
   v_genus            ALIAS FOR $1;
   v_species          ALIAS FOR $2;
   v_common_name      ALIAS FOR $3;
   v_organism_id      INTEGER;
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
--
-- functions operating on featureloc ranges
--

-- create a point
CREATE OR REPLACE FUNCTION create_point (bigint, bigint) RETURNS point AS
 'SELECT point ($1, $2)'
LANGUAGE 'sql';

-- create a range box
-- (make this immutable so we can index it)
CREATE OR REPLACE FUNCTION boxrange (bigint, bigint) RETURNS box AS
 'SELECT box (create_point(CAST(0 AS bigint), $1), create_point($2,500000000))'
LANGUAGE 'sql' IMMUTABLE;

-- create a query box
CREATE OR REPLACE FUNCTION boxquery (bigint, bigint) RETURNS box AS
 'SELECT box (create_point($1, $2), create_point($1, $2))'
LANGUAGE 'sql' IMMUTABLE;

--functional index that depends on the above functions
CREATE INDEX binloc_boxrange ON featureloc USING GIST (boxrange(fmin, fmax));

CREATE OR REPLACE FUNCTION featureloc_slice(bigint, bigint) RETURNS setof featureloc AS
  'SELECT * from featureloc where boxquery($1, $2) @ boxrange(fmin,fmax)'
LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION featureloc_slice(varchar, bigint, bigint)
  RETURNS setof featureloc AS
  'SELECT featureloc.*
   FROM featureloc
   INNER JOIN feature AS srcf ON (srcf.feature_id = featureloc.srcfeature_id)
   WHERE boxquery($2, $3) @ boxrange(fmin,fmax)
   AND srcf.name = $1 '
LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION featureloc_slice(int, bigint, bigint)
  RETURNS setof featureloc AS
  'SELECT *
   FROM featureloc
   WHERE boxquery($2, $3) @ boxrange(fmin,fmax)
   AND srcfeature_id = $1 '
LANGUAGE 'sql';

-- can we not just do these as views?
CREATE OR REPLACE FUNCTION feature_overlaps(bigint)
 RETURNS setof feature AS
 'SELECT feature.*
  FROM feature
   INNER JOIN featureloc AS x ON (x.feature_id=feature.feature_id)
   INNER JOIN featureloc AS y ON (y.feature_id = $1)
  WHERE
   x.srcfeature_id = y.srcfeature_id            AND
   ( x.fmax >= y.fmin AND x.fmin <= y.fmax ) '
LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION feature_disjoint_from(bigint)
 RETURNS setof feature AS
 'SELECT feature.*
  FROM feature
   INNER JOIN featureloc AS x ON (x.feature_id=feature.feature_id)
   INNER JOIN featureloc AS y ON (y.feature_id = $1)
  WHERE
   x.srcfeature_id = y.srcfeature_id            AND
   ( x.fmax < y.fmin OR x.fmin > y.fmax ) '
LANGUAGE 'sql';

--Evolution of the methods found in range.plpgsql (C. Pommier)
--Goal : increase performances of segment fetching
--       Implies to optimise featureloc_slice

--Background : The existing featureloc_slice uses uses a spatial rtree index. The spatial objects used are a boxrange ((0,fmin), (fmax,500000000)) and a boxquery ((fmin,fmax),(fmin,fmax)) . The boxranges are indexed.
--             To speed up things (for gbrowse) featureloc_slice has been overiden to filter simultaneously on the boxrange and the srcfeature_id. This gives good results.
--             The goal here is to push this logic further and to include the srcfeature_id filter directly into the boxrange object. We propose to consider the following boxs :
--             boxrange : ((srcfeature_id,fmin),(srcfeature_id,fmax))
--             boxquery : ((srcfeature_id,fmin),(srcfeature_id,fmax))

CREATE OR REPLACE FUNCTION boxrange (bigint, bigint, bigint) RETURNS box AS
 'SELECT box (create_point($1, $2), create_point($1,$3))'
LANGUAGE 'sql' IMMUTABLE;

-- create a query box
CREATE OR REPLACE FUNCTION boxquery (bigint, bigint, bigint) RETURNS box AS
 'SELECT box (create_point($1, $2), create_point($1, $3))'
LANGUAGE 'sql' IMMUTABLE;

CREATE INDEX binloc_boxrange_src ON featureloc USING GIST (boxrange(srcfeature_id,fmin, fmax));

CREATE OR REPLACE FUNCTION featureloc_slice(bigint, bigint, bigint)
  RETURNS setof featureloc AS
  'SELECT *
   FROM featureloc
   WHERE boxquery($1, $2, $3) && boxrange(srcfeature_id,fmin,fmax)'
LANGUAGE 'sql';

-- reverse_string
CREATE OR REPLACE FUNCTION reverse_string(TEXT) RETURNS TEXT AS
'
 DECLARE
  reversed_string TEXT;
  incoming ALIAS FOR $1;
 BEGIN
   reversed_string = '''';
   FOR i IN REVERSE char_length(incoming)..1 loop
     reversed_string = reversed_string || substring(incoming FROM i FOR 1);
   END loop;
 RETURN reversed_string;
END'
language plpgsql;

-- complements DNA
CREATE OR REPLACE FUNCTION complement_residues(text) RETURNS text AS
 'SELECT (translate($1,
                   ''acgtrymkswhbvdnxACGTRYMKSWHBVDNX'',
                   ''tgcayrkmswdvbhnxTGCAYRKMSWDVBHNX''))'
LANGUAGE 'sql';

-- revcomp
CREATE OR REPLACE FUNCTION reverse_complement(TEXT) RETURNS TEXT AS
 'SELECT reverse_string(complement_residues($1))'
LANGUAGE 'sql';

-- DNA to AA
CREATE OR REPLACE FUNCTION translate_dna(TEXT,INT) RETURNS TEXT AS
'
 DECLARE
  dnaseq ALIAS FOR $1;
  gcode ALIAS FOR $2;
  translation TEXT;
  dnaseqlen INT;
  codon CHAR(3);
  aa CHAR(1);
  i INT;
 BEGIN
   translation = '''';
   dnaseqlen = char_length(dnaseq);
   i=1;
   WHILE i+1 < dnaseqlen loop
     codon = substring(dnaseq,i,3);
     aa = translate_codon(codon,gcode);
     translation = translation || aa;
     i = i+3;
   END loop;
 RETURN translation;
END'
language plpgsql;

-- DNA to AA, default genetic code
CREATE OR REPLACE FUNCTION translate_dna(TEXT) RETURNS TEXT AS
 'SELECT translate_dna($1,1)'
LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION translate_codon(TEXT,INT) RETURNS CHAR AS
 'SELECT aa FROM genetic_code.gencode_codon_aa WHERE codon=$1 AND gencode_id=$2'
LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION concat_pair (text, text) RETURNS text AS
 'SELECT $1 || $2'
LANGUAGE 'sql';

CREATE AGGREGATE concat (
sfunc = concat_pair,
basetype = text,
stype = text,
initcond = ''
);

--function to 'unshare' exons.  It looks for exons that have the same fmin
--and fmax and belong to the same gene and only keeps one.  The other,
--redundant exons are marked obsolete in the feature table.  Nothing
--is done with those features' entries in the featureprop, feature_dbxref,
--feature_pub, or feature_cvterm tables.  For the moment, I'm assuming
--that any annotations that they have when this script is run are
--identical to their non-obsoleted doppelgangers.  If that's not the case,
--they could be merged via query.
--
--The bulk of this code was contributed by Robin Houston at
--GeneDB/Sanger Centre.

CREATE OR REPLACE FUNCTION share_exons () RETURNS void AS '
  DECLARE
  BEGIN
    /* Generate a table of shared exons */
    CREATE temporary TABLE shared_exons AS
      SELECT gene.feature_id as gene_feature_id
           , gene.uniquename as gene_uniquename
           , transcript1.uniquename as transcript1
           , exon1.feature_id as exon1_feature_id
           , exon1.uniquename as exon1_uniquename
           , transcript2.uniquename as transcript2
           , exon2.feature_id as exon2_feature_id
           , exon2.uniquename as exon2_uniquename
           , exon1_loc.fmin /* = exon2_loc.fmin */
           , exon1_loc.fmax /* = exon2_loc.fmax */
      FROM feature gene
        JOIN cvterm gene_type ON gene.type_id = gene_type.cvterm_id
        JOIN cv gene_type_cv USING (cv_id)
        JOIN feature_relationship gene_transcript1 ON gene.feature_id = gene_transcript1.object_id
        JOIN feature transcript1 ON gene_transcript1.subject_id = transcript1.feature_id
        JOIN cvterm transcript1_type ON transcript1.type_id = transcript1_type.cvterm_id
        JOIN cv transcript1_type_cv ON transcript1_type.cv_id = transcript1_type_cv.cv_id
        JOIN feature_relationship transcript1_exon1 ON transcript1_exon1.object_id = transcript1.feature_id
        JOIN feature exon1 ON transcript1_exon1.subject_id = exon1.feature_id
        JOIN cvterm exon1_type ON exon1.type_id = exon1_type.cvterm_id
        JOIN cv exon1_type_cv ON exon1_type.cv_id = exon1_type_cv.cv_id
        JOIN featureloc exon1_loc ON exon1_loc.feature_id = exon1.feature_id
        JOIN feature_relationship gene_transcript2 ON gene.feature_id = gene_transcript2.object_id
        JOIN feature transcript2 ON gene_transcript2.subject_id = transcript2.feature_id
        JOIN cvterm transcript2_type ON transcript2.type_id = transcript2_type.cvterm_id
        JOIN cv transcript2_type_cv ON transcript2_type.cv_id = transcript2_type_cv.cv_id
        JOIN feature_relationship transcript2_exon2 ON transcript2_exon2.object_id = transcript2.feature_id
        JOIN feature exon2 ON transcript2_exon2.subject_id = exon2.feature_id
        JOIN cvterm exon2_type ON exon2.type_id = exon2_type.cvterm_id
        JOIN cv exon2_type_cv ON exon2_type.cv_id = exon2_type_cv.cv_id
        JOIN featureloc exon2_loc ON exon2_loc.feature_id = exon2.feature_id
      WHERE gene_type_cv.name = ''sequence''
        AND gene_type.name = ''gene''
        AND transcript1_type_cv.name = ''sequence''
        AND transcript1_type.name = ''mRNA''
        AND transcript2_type_cv.name = ''sequence''
        AND transcript2_type.name = ''mRNA''
        AND exon1_type_cv.name = ''sequence''
        AND exon1_type.name = ''exon''
        AND exon2_type_cv.name = ''sequence''
        AND exon2_type.name = ''exon''
        AND exon1.feature_id < exon2.feature_id
        AND exon1_loc.rank = 0
        AND exon2_loc.rank = 0
        AND exon1_loc.fmin = exon2_loc.fmin
        AND exon1_loc.fmax = exon2_loc.fmax
    ;

    /* Choose one of the shared exons to be the canonical representative.
       We pick the one with the smallest feature_id.
     */
    CREATE temporary TABLE canonical_exon_representatives AS
      SELECT gene_feature_id, min(exon1_feature_id) AS canonical_feature_id, fmin
      FROM shared_exons
      GROUP BY gene_feature_id,fmin
    ;

    CREATE temporary TABLE exon_replacements AS
      SELECT DISTINCT shared_exons.exon2_feature_id AS actual_feature_id
                    , canonical_exon_representatives.canonical_feature_id
                    , canonical_exon_representatives.fmin
      FROM shared_exons
        JOIN canonical_exon_representatives USING (gene_feature_id)
      WHERE shared_exons.exon2_feature_id <> canonical_exon_representatives.canonical_feature_id
        AND shared_exons.fmin = canonical_exon_representatives.fmin
    ;

    UPDATE feature_relationship
      SET subject_id = (
            SELECT canonical_feature_id
            FROM exon_replacements
            WHERE feature_relationship.subject_id = exon_replacements.actual_feature_id)
      WHERE subject_id IN (
        SELECT actual_feature_id FROM exon_replacements
    );

    UPDATE feature_relationship
      SET object_id = (
            SELECT canonical_feature_id
            FROM exon_replacements
            WHERE feature_relationship.subject_id = exon_replacements.actual_feature_id)
      WHERE object_id IN (
        SELECT actual_feature_id FROM exon_replacements
    );

    UPDATE feature
      SET is_obsolete = true
      WHERE feature_id IN (
        SELECT actual_feature_id FROM exon_replacements
    );
  END;
' LANGUAGE 'plpgsql';

--This is a function to seek out exons of transcripts and orders them,
--using feature_relationship.rank, in "transcript order" numbering
--from 0, taking strand into account. It will not touch transcripts that
--already have their exons ordered (in case they have a non-obvious
--ordering due to trans splicing). It takes as an argument the
--feature.type_id of the parent transcript type (typically, mRNA, although
--non coding transcript types should work too).

CREATE OR REPLACE FUNCTION order_exons (integer) RETURNS void AS '
  DECLARE
    parent_type      ALIAS FOR $1;
    exon_id          int;
    part_of          int;
    exon_type        int;
    strand           int;
    arow             RECORD;
    order_by         varchar;
    rowcount         int;
    exon_count       int;
    ordered_exons    int;
    transcript_id    int;
    transcript_row   feature%ROWTYPE;
  BEGIN
    SELECT INTO part_of cvterm_id FROM cvterm WHERE name=''part_of''
      AND cv_id IN (SELECT cv_id FROM cv WHERE name=''relationship'');
    --SELECT INTO exon_type cvterm_id FROM cvterm WHERE name=''exon''
    --  AND cv_id IN (SELECT cv_id FROM cv WHERE name=''sequence'');

    --RAISE NOTICE ''part_of %, exon %'',part_of,exon_type;

    FOR transcript_row IN
      SELECT * FROM feature WHERE type_id = parent_type
    LOOP
      transcript_id = transcript_row.feature_id;
      SELECT INTO rowcount count(*) FROM feature_relationship
        WHERE object_id = transcript_id
          AND rank = 0;

      --Dont modify this transcript if there are already numbered exons or
      --if there is only one exon
      IF rowcount = 1 THEN
        --RAISE NOTICE ''skipping transcript %, row count %'',transcript_id,rowcount;
        CONTINUE;
      END IF;

      --need to reverse the order if the strand is negative
      SELECT INTO strand strand FROM featureloc WHERE feature_id=transcript_id;
      IF strand > 0 THEN
          order_by = ''fl.fmin'';
      ELSE
          order_by = ''fl.fmax desc'';
      END IF;

      exon_count = 0;
      FOR arow IN EXECUTE
        ''SELECT fr.*, fl.fmin, fl.fmax
          FROM feature_relationship fr, featureloc fl
          WHERE fr.object_id  = ''||transcript_id||''
            AND fr.subject_id = fl.feature_id
            AND fr.type_id    = ''||part_of||''
            ORDER BY ''||order_by
      LOOP
        --number the exons for a given transcript
        UPDATE feature_relationship
          SET rank = exon_count
          WHERE feature_relationship_id = arow.feature_relationship_id;
        exon_count = exon_count + 1;
      END LOOP;

    END LOOP;

  END;
' LANGUAGE 'plpgsql';
-- down the graph: eg from  chromosome to contig
CREATE OR REPLACE FUNCTION project_point_up(int,int,int,int)
 RETURNS int AS
'SELECT
  CASE WHEN $4<0
   THEN $3-$1             -- rev strand
   ELSE $1-$2             -- fwd strand
  END AS p'
LANGUAGE 'sql';

-- down the graph: eg from contig to chromosome
CREATE OR REPLACE FUNCTION project_point_down(int,int,int,int)
 RETURNS int AS
'SELECT
  CASE WHEN $4<0
   THEN $3-$1
   ELSE $1+$2
  END AS p'
LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION project_featureloc_up(int,int)
 RETURNS featureloc AS
'
DECLARE
    in_featureloc_id alias for $1;
    up_srcfeature_id alias for $2;
    in_featureloc featureloc%ROWTYPE;
    up_featureloc featureloc%ROWTYPE;
    nu_featureloc featureloc%ROWTYPE;
    nu_fmin INT;
    nu_fmax INT;
    nu_strand INT;
BEGIN
 SELECT INTO in_featureloc
   featureloc.*
  FROM featureloc
  WHERE featureloc_id = in_featureloc_id;

 SELECT INTO up_featureloc
   up_fl.*
  FROM featureloc AS in_fl
  INNER JOIN featureloc AS up_fl
    ON (in_fl.srcfeature_id = up_fl.feature_id)
  WHERE
   in_fl.featureloc_id = in_featureloc_id AND
   up_fl.srcfeature_id = up_srcfeature_id;

  IF up_featureloc.strand IS NULL
   THEN RETURN NULL;
  END IF;

  IF up_featureloc.strand < 0
  THEN
   nu_fmin = project_point_up(in_featureloc.fmax,
                              up_featureloc.fmin,up_featureloc.fmax,-1);
   nu_fmax = project_point_up(in_featureloc.fmin,
                              up_featureloc.fmin,up_featureloc.fmax,-1);
   nu_strand = -in_featureloc.strand;
  ELSE
   nu_fmin = project_point_up(in_featureloc.fmin,
                              up_featureloc.fmin,up_featureloc.fmax,1);
   nu_fmax = project_point_up(in_featureloc.fmax,
                              up_featureloc.fmin,up_featureloc.fmax,1);
   nu_strand = in_featureloc.strand;
  END IF;
  in_featureloc.fmin = nu_fmin;
  in_featureloc.fmax = nu_fmax;
  in_featureloc.strand = nu_strand;
  in_featureloc.srcfeature_id = up_featureloc.srcfeature_id;
  RETURN in_featureloc;
END
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION project_point_g2t(int,int,int)
 RETURNS INT AS '
 DECLARE
    in_p             alias for $1;
    srcf_id          alias for $2;
    t_id             alias for $3;
    e_floc           featureloc%ROWTYPE;
    out_p            INT;
    exon_cvterm_id   INT;
BEGIN
 SELECT INTO exon_cvterm_id get_feature_type_id(''exon'');
 SELECT INTO out_p
  CASE
   WHEN strand<0 THEN fmax-p
   ELSE p-fmin
   END AS p
  FROM featureloc
   INNER JOIN feature USING (feature_id)
   INNER JOIN feature_relationship ON (feature.feature_id=subject_id)
  WHERE
   object_id = t_id                     AND
   feature.type_id = exon_cvterm_id     AND
   featureloc.srcfeature_id = srcf_id   AND
   in_p >= fmin                         AND
   in_p <= fmax;
  RETURN in_featureloc;
END
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_cv_id_for_feature() RETURNS BIGINT
 AS 'SELECT cv_id FROM cv WHERE name=''sequence''' LANGUAGE 'sql';
CREATE OR REPLACE FUNCTION get_cv_id_for_featureprop() RETURNS BIGINT
 AS 'SELECT cv_id FROM cv WHERE name=''feature_property''' LANGUAGE 'sql';
CREATE OR REPLACE FUNCTION get_cv_id_for_feature_relationsgip() RETURNS BIGINT
 AS 'SELECT cv_id FROM cv WHERE name=''relationship''' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION get_feature_type_id(VARCHAR) RETURNS BIGINT
 AS '
  SELECT cvterm_id
  FROM cv INNER JOIN cvterm USING (cv_id)
  WHERE cvterm.name=$1 AND cv.name=''sequence''
 ' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION get_featureprop_type_id(VARCHAR) RETURNS BIGINT
 AS '
  SELECT cvterm_id
  FROM cv INNER JOIN cvterm USING (cv_id)
  WHERE cvterm.name=$1 AND cv.name=''feature_property''
 ' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION get_feature_relationship_type_id(VARCHAR) RETURNS BIGINT
 AS '
  SELECT cvterm_id
  FROM cv INNER JOIN cvterm USING (cv_id)
  WHERE cvterm.name=$1 AND cv.name=''relationship''
 ' LANGUAGE 'sql';

-- depends on sequence-cv-helper
CREATE OR REPLACE FUNCTION get_feature_id(VARCHAR,VARCHAR,VARCHAR) RETURNS BIGINT
 AS '
  SELECT feature_id
  FROM feature
  WHERE uniquename=$1
    AND type_id=get_feature_type_id($2)
    AND organism_id=get_organism_id($3)
 ' LANGUAGE 'sql';

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

CREATE OR REPLACE FUNCTION store_analysis (VARCHAR,VARCHAR,VARCHAR)
  RETURNS INT AS
'DECLARE
   v_program            ALIAS FOR $1;
   v_programversion     ALIAS FOR $2;
   v_sourcename         ALIAS FOR $3;
   pkval                INTEGER;
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

-- FUNCTION gfffeatureatts (integer) is a function to get
-- data in the same format as the gffatts view so that
-- it can be easily converted to GFF attributes.

CREATE FUNCTION  gfffeatureatts (integer)
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
CREATE OR REPLACE FUNCTION featureslice(int, int) RETURNS setof featureloc AS
  'SELECT * from featureloc where boxquery($1, $2) @ boxrange(fmin,fmax)'
LANGUAGE 'sql';

--uses the gff3atts to create a GFF3 compliant attribute string
CREATE OR REPLACE FUNCTION gffattstring (integer) RETURNS varchar AS
'DECLARE
  return_string      varchar;
  f_id               ALIAS FOR $1;
  atts_view          gffatts%ROWTYPE;
  feature_row        feature%ROWTYPE;
  name               varchar;
  uniquename         varchar;
  parent             varchar;
  escape_loc         int;
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

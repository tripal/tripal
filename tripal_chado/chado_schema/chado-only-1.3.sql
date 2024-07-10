--SET search_path = 'chado';

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

CREATE TYPE feature_by_fx_type AS (
	feature_id bigint,
	depth integer
);

CREATE TYPE obj_type AS ENUM (
    'TABLE',
    'VIEW',
    'COLUMN',
    'SEQUENCE',
    'FUNCTION',
    'SCHEMA',
    'DATABASE'
);

CREATE TYPE perm_type AS ENUM (
    'SELECT',
    'INSERT',
    'UPDATE',
    'DELETE',
    'TRUNCATE',
    'REFERENCES',
    'TRIGGER',
    'USAGE',
    'CREATE',
    'EXECUTE',
    'CONNECT',
    'TEMPORARY'
);

CREATE TYPE soi_type AS (
	type_id bigint,
	subject_id bigint,
	object_id bigint
);

CREATE FUNCTION _fill_cvtermpath4node(bigint, bigint, bigint, bigint, integer) RETURNS integer
    LANGUAGE plpgsql
    AS $_$
DECLARE
    origin alias for $1;
    child_id alias for $2;
    cvid alias for $3;
    typeid alias for $4;
    depth alias for $5;
    cterm cvterm_relationship%ROWTYPE;
    exist_c int;

BEGIN

    --- RAISE NOTICE 'depth=% root=%', depth,child_id;
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
$_$;

CREATE FUNCTION _fill_cvtermpath4node2detect_cycle(bigint, bigint, bigint, bigint, integer) RETURNS bigint
    LANGUAGE plpgsql
    AS $_$
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

    EXECUTE 'SELECT * FROM tmpcvtermpath p1, tmpcvtermpath p2 WHERE p1.subject_id=p2.object_id AND p1.object_id=p2.subject_id AND p1.object_id = '|| origin || ' AND p2.subject_id = ' || child_id || 'AND ' || depth || '> 0';
    GET DIAGNOSTICS ccount = ROW_COUNT;
    IF (ccount > 0) THEN
        --RAISE EXCEPTION 'FOUND CYCLE: node % on cycle path',origin;
        RETURN origin;
    END IF;

    EXECUTE 'SELECT * FROM tmpcvtermpath WHERE cv_id = ' || cvid || ' AND object_id = ' || origin || ' AND subject_id = ' || child_id || ' AND ' || origin || '<>' || child_id;
    GET DIAGNOSTICS ecount = ROW_COUNT;
    IF (ecount > 0) THEN
        --RAISE NOTICE 'FOUND TWICE (node), will check root obj % subj %',origin, child_id;
        SELECT INTO rtn _fill_cvtermpath4root2detect_cycle(child_id, cvid);
        IF (rtn > 0) THEN
            RETURN rtn;
        END IF;
    END IF;

    EXECUTE 'SELECT * FROM tmpcvtermpath WHERE cv_id = ' || cvid || ' AND object_id = ' || origin || ' AND subject_id = ' || child_id || ' AND pathdistance = ' || depth;
    GET DIAGNOSTICS exist_c = ROW_COUNT;
    IF (exist_c = 0) THEN
        EXECUTE 'INSERT INTO tmpcvtermpath (object_id, subject_id, cv_id, type_id, pathdistance) VALUES(' || origin || ', ' || child_id || ', ' || cvid || ', ' || typeid || ', ' || depth || ')';
    END IF;

    FOR cterm IN SELECT * FROM cvterm_relationship WHERE object_id = child_id LOOP
        --RAISE NOTICE 'DOING for node, % %', origin, cterm.subject_id;
        SELECT INTO rtn _fill_cvtermpath4node2detect_cycle(origin, cterm.subject_id, cvid, cterm.type_id, depth+1);
        IF (rtn > 0) THEN
            RETURN rtn;
        END IF;
    END LOOP;
    RETURN 0;
END;
$_$;

CREATE FUNCTION _fill_cvtermpath4root(bigint, bigint) RETURNS integer
    LANGUAGE plpgsql
    AS $_$
DECLARE
    rootid alias for $1;
    cvid alias for $2;
    ttype bigint;
    cterm cvterm_relationship%ROWTYPE;
    child cvterm_relationship%ROWTYPE;

BEGIN

    SELECT INTO ttype cvterm_id FROM cvterm WHERE (name = 'isa' OR name = 'is_a');
    PERFORM _fill_cvtermpath4node(rootid, rootid, cvid, ttype, 0);
    FOR cterm IN SELECT * FROM cvterm_relationship WHERE object_id = rootid LOOP
        PERFORM _fill_cvtermpath4root(cterm.subject_id, cvid);
        -- RAISE NOTICE 'DONE for term, %', cterm.subject_id;
    END LOOP;
    RETURN 1;
END;
$_$;

CREATE FUNCTION _fill_cvtermpath4root2detect_cycle(bigint, bigint) RETURNS bigint
    LANGUAGE plpgsql
    AS $_$
DECLARE
    rootid alias for $1;
    cvid alias for $2;
    ttype bigint;
    ccount int;
    cterm cvterm_relationship%ROWTYPE;
    child cvterm_relationship%ROWTYPE;
    rtn     bigint;
BEGIN

    SELECT INTO ttype cvterm_id FROM cvterm WHERE (name = 'isa' OR name = 'is_a');
    SELECT INTO rtn _fill_cvtermpath4node2detect_cycle(rootid, rootid, cvid, ttype, 0);
    IF (rtn > 0) THEN
        RETURN rtn;
    END IF;
    FOR cterm IN SELECT * FROM cvterm_relationship WHERE object_id = rootid LOOP
        EXECUTE 'SELECT * FROM tmpcvtermpath p1, tmpcvtermpath p2 WHERE p1.subject_id=p2.object_id AND p1.object_id=p2.subject_id AND p1.object_id=' || rootid || ' AND p1.subject_id=' || cterm.subject_id;
        GET DIAGNOSTICS ccount = ROW_COUNT;
        IF (ccount > 0) THEN
            --RAISE NOTICE 'FOUND TWICE (root), will check root obj % subj %',rootid,cterm.subject_id;
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
$_$;

CREATE FUNCTION _fill_cvtermpath4soi(bigint, bigint) RETURNS integer
    LANGUAGE plpgsql
    AS $_$
DECLARE
    rootid alias for $1;
    cvid alias for $2;
    ttype bigint;
    cterm soi_type%ROWTYPE;

BEGIN

    SELECT INTO ttype cvterm_id FROM cvterm WHERE name = 'isa';
    --RAISE NOTICE 'got ttype %',ttype;
    PERFORM _fill_cvtermpath4soinode(rootid, rootid, cvid, ttype, 0);
    FOR cterm IN SELECT tmp_type AS type_id, subject_id FROM tmpcvtr WHERE object_id = rootid LOOP
        PERFORM _fill_cvtermpath4soi(cterm.subject_id, cvid);
    END LOOP;
    RETURN 1;
END;
$_$;

CREATE FUNCTION _fill_cvtermpath4soinode(bigint, bigint, bigint, bigint, integer) RETURNS integer
    LANGUAGE plpgsql
    AS $_$
DECLARE
    origin alias for $1;
    child_id alias for $2;
    cvid alias for $3;
    typeid alias for $4;
    depth alias for $5;
    cterm soi_type%ROWTYPE;
    exist_c int;

BEGIN

    --RAISE NOTICE 'depth=% o=%, root=%, cv=%, t=%', depth,origin,child_id,cvid,typeid;
    SELECT INTO exist_c count(*) FROM cvtermpath WHERE cv_id = cvid AND object_id = origin AND subject_id = child_id AND pathdistance = depth;
    --- longest path
    IF (exist_c > 0) THEN
        UPDATE cvtermpath SET pathdistance = depth WHERE cv_id = cvid AND object_id = origin AND subject_id = child_id;
    ELSE
        INSERT INTO cvtermpath (object_id, subject_id, cv_id, type_id, pathdistance) VALUES(origin, child_id, cvid, typeid, depth);
    END IF;

    FOR cterm IN SELECT tmp_type AS type_id, subject_id FROM tmpcvtr WHERE object_id = child_id LOOP
        PERFORM _fill_cvtermpath4soinode(origin, cterm.subject_id, cvid, cterm.type_id, depth+1);
    END LOOP;
    RETURN 1;
END;
$_$;

-- SET default_tablespace = '';

-- SET default_table_access_method = heap;

CREATE TABLE cvtermpath (
    cvtermpath_id bigint NOT NULL,
    type_id bigint,
    subject_id bigint NOT NULL,
    object_id bigint NOT NULL,
    cv_id bigint NOT NULL,
    pathdistance integer
);

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

CREATE FUNCTION _get_all_object_ids(bigint) RETURNS SETOF cvtermpath
    LANGUAGE plpgsql
    AS $_$
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
$_$;

CREATE FUNCTION _get_all_subject_ids(bigint) RETURNS SETOF cvtermpath
    LANGUAGE plpgsql
    AS $_$
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
$_$;

CREATE FUNCTION boxquery(bigint, bigint) RETURNS box
    LANGUAGE sql IMMUTABLE
    AS $_$SELECT box (create_point($1, $2), create_point($1, $2))$_$;

CREATE FUNCTION boxquery(bigint, bigint, bigint) RETURNS box
    LANGUAGE sql IMMUTABLE
    AS $_$SELECT box (create_point($1, $2), create_point($1, $3))$_$;

CREATE FUNCTION boxrange(bigint, bigint) RETURNS box
    LANGUAGE sql IMMUTABLE
    AS $_$SELECT box (create_point(0, $1), create_point($2,500000000))$_$;

CREATE FUNCTION boxrange(bigint, bigint, bigint) RETURNS box
    LANGUAGE sql IMMUTABLE
    AS $_$SELECT box (create_point($1, $2), create_point($1,$3))$_$;

CREATE FUNCTION complement_residues(text) RETURNS text
    LANGUAGE sql
    AS $_$SELECT (translate($1,
                   'acgtrymkswhbvdnxACGTRYMKSWHBVDNX',
                   'tgcayrkmswdvbhnxTGCAYRKMSWDVBHNX'))$_$;

CREATE FUNCTION concat_pair(text, text) RETURNS text
    LANGUAGE sql
    AS $_$SELECT $1 || $2$_$;

CREATE FUNCTION create_point(bigint, bigint) RETURNS point
    LANGUAGE sql
    AS $_$SELECT point ($1, $2)$_$;

CREATE FUNCTION create_soi() RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
    parent soi_type%ROWTYPE;
    isa_id cvterm.cvterm_id%TYPE;
    soi_term TEXT := 'soi';
    soi_def TEXT := 'ontology of SO feature instantiated in database';
    soi_cvid bigint;
    soiterm_id bigint;
    pcount INTEGER;
    count INTEGER := 0;
    cquery TEXT;
BEGIN

    SELECT INTO isa_id cvterm_id FROM cvterm WHERE name = 'isa';

    SELECT INTO soi_cvid cv_id FROM cv WHERE name = soi_term;
    IF (soi_cvid > 0) THEN
        DELETE FROM cvtermpath WHERE cv_id = soi_cvid;
        DELETE FROM cvterm WHERE cv_id = soi_cvid;
    ELSE
        INSERT INTO cv (name, definition) VALUES(soi_term, soi_def);
    END IF;
    SELECT INTO soi_cvid cv_id FROM cv WHERE name = soi_term;
    INSERT INTO cvterm (name, cv_id) VALUES(soi_term, soi_cvid);
    SELECT INTO soiterm_id cvterm_id FROM cvterm WHERE name = soi_term;

    CREATE TEMP TABLE tmpcvtr (tmp_type BIGINT, type_id bigint, subject_id bigint, object_id bigint);
    CREATE UNIQUE INDEX u_tmpcvtr ON tmpcvtr(subject_id, object_id);

    INSERT INTO tmpcvtr (tmp_type, type_id, subject_id, object_id)
        SELECT DISTINCT isa_id, soiterm_id, f.type_id, soiterm_id FROM feature f, cvterm t
        WHERE f.type_id = t.cvterm_id AND f.type_id > 0;
    EXECUTE 'select * from tmpcvtr where type_id = ' || soiterm_id || ';';
    get diagnostics pcount = row_count;
    raise notice 'all types in feature %',pcount;

    FOR parent IN SELECT DISTINCT 0, t.cvterm_id, 0 FROM feature c, feature_relationship fr, cvterm t
            WHERE t.cvterm_id = c.type_id AND c.feature_id = fr.subject_id LOOP
        DELETE FROM tmpcvtr WHERE type_id = soiterm_id and object_id = soiterm_id
            AND subject_id = parent.subject_id;
    END LOOP;
    EXECUTE 'select * from tmpcvtr where type_id = ' || soiterm_id || ';';
    get diagnostics pcount = row_count;
    raise notice 'all types in feature after delete child %',pcount;

    --- create feature type relationship (store in tmpcvtr)
    CREATE TEMP TABLE tmproot (cv_id bigint not null, cvterm_id bigint not null, status INTEGER DEFAULT 0);
    cquery := 'SELECT * FROM tmproot tmp WHERE tmp.status = 0;';
    ---temp use tmpcvtr to hold instantiated SO relationship for speed
    ---use soterm_id as type_id, will delete from tmpcvtr
    ---us tmproot for this as well
    INSERT INTO tmproot (cv_id, cvterm_id, status) SELECT DISTINCT soi_cvid, c.subject_id, 0 FROM tmpcvtr c
        WHERE c.object_id = soiterm_id;
    EXECUTE cquery;
    GET DIAGNOSTICS pcount = ROW_COUNT;
    WHILE (pcount > 0) LOOP
        RAISE NOTICE 'num child temp (to be inserted) in tmpcvtr: %',pcount;
        INSERT INTO tmpcvtr (tmp_type, type_id, subject_id, object_id)
            SELECT DISTINCT fr.type_id, soiterm_id, c.type_id, p.cvterm_id FROM feature c, feature_relationship fr,
            tmproot p, feature pf, cvterm t WHERE c.feature_id = fr.subject_id AND fr.object_id = pf.feature_id
            AND p.cvterm_id = pf.type_id AND t.cvterm_id = c.type_id AND p.status = 0;
        UPDATE tmproot SET status = 1 WHERE status = 0;
        INSERT INTO tmproot (cv_id, cvterm_id, status)
            SELECT DISTINCT soi_cvid, c.type_id, 0 FROM feature c, feature_relationship fr,
            tmproot tmp, feature p, cvterm t WHERE c.feature_id = fr.subject_id AND fr.object_id = p.feature_id
            AND tmp.cvterm_id = p.type_id AND t.cvterm_id = c.type_id AND tmp.status = 1;
        UPDATE tmproot SET status = 2 WHERE status = 1;
        EXECUTE cquery;
        GET DIAGNOSTICS pcount = ROW_COUNT;
    END LOOP;
    DELETE FROM tmproot;

    ---get transitive closure for soi
    PERFORM _fill_cvtermpath4soi(soiterm_id, soi_cvid);

    DROP TABLE tmpcvtr;
    DROP TABLE tmproot;

    RETURN 1;
END;
$$;

CREATE TABLE feature (
    feature_id bigint NOT NULL,
    dbxref_id bigint,
    organism_id bigint NOT NULL,
    name character varying(255),
    uniquename text NOT NULL,
    residues text,
    seqlen bigint,
    md5checksum character(32),
    type_id bigint NOT NULL,
    is_analysis boolean DEFAULT false NOT NULL,
    is_obsolete boolean DEFAULT false NOT NULL,
    timeaccessioned timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    timelastmodified timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);
ALTER TABLE ONLY feature ALTER COLUMN residues SET STORAGE EXTERNAL;

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

CREATE FUNCTION feature_disjoint_from(bigint) RETURNS SETOF feature
    LANGUAGE sql
    AS $_$SELECT feature.*
  FROM feature
   INNER JOIN featureloc AS x ON (x.feature_id=feature.feature_id)
   INNER JOIN featureloc AS y ON (y.feature_id = $1)
  WHERE
   x.srcfeature_id = y.srcfeature_id            AND
   ( x.fmax < y.fmin OR x.fmin > y.fmax ) $_$;

CREATE FUNCTION feature_overlaps(bigint) RETURNS SETOF feature
    LANGUAGE sql
    AS $_$SELECT feature.*
  FROM feature
   INNER JOIN featureloc AS x ON (x.feature_id=feature.feature_id)
   INNER JOIN featureloc AS y ON (y.feature_id = $1)
  WHERE
   x.srcfeature_id = y.srcfeature_id            AND
   ( x.fmax >= y.fmin AND x.fmin <= y.fmax ) $_$;

CREATE TABLE featureloc (
    featureloc_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    srcfeature_id bigint,
    fmin bigint,
    is_fmin_partial boolean DEFAULT false NOT NULL,
    fmax bigint,
    is_fmax_partial boolean DEFAULT false NOT NULL,
    strand smallint,
    phase integer,
    residue_info text,
    locgroup integer DEFAULT 0 NOT NULL,
    rank integer DEFAULT 0 NOT NULL,
    CONSTRAINT featureloc_c2 CHECK ((fmin <= fmax))
);

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

COMMENT ON COLUMN featureloc.is_fmin_partial IS 'This is typically
false, but may be true if the value for column:fmin is inaccurate or
the leftmost part of the range is unknown/unbounded.';

COMMENT ON COLUMN featureloc.fmax IS 'The rightmost/maximal boundary in the linear range represented by the featureloc. Sometimes (e.g. in bioperl) this is called -end- although this is confusing because it does not necessarily represent the 3-prime coordinate. Important: This is space-based (interbase) coordinates, counting from zero. No conversion is required to go from fmax to the rightmost coordinate in a base-oriented system that counts from 1 (e.g. GFF, Bioperl).';

COMMENT ON COLUMN featureloc.is_fmax_partial IS 'This is typically
false, but may be true if the value for column:fmax is inaccurate or
the rightmost part of the range is unknown/unbounded.';

COMMENT ON COLUMN featureloc.strand IS 'The orientation/directionality of the
location. Should be 0, -1 or +1.';

COMMENT ON COLUMN featureloc.phase IS 'Phase of translation with
respect to srcfeature_id.
Values are 0, 1, 2. It may not be possible to manifest this column for
some features such as exons, because the phase is dependant on the
spliceform (the same exon can appear in multiple spliceforms). This column is mostly useful for predicted exons and CDSs.';

COMMENT ON COLUMN featureloc.residue_info IS 'Alternative residues,
when these differ from feature.residues. For instance, a SNP feature
located on a wild and mutant protein would have different alternative residues.
for alignment/similarity features, the alternative residues is used to
represent the alignment string (CIGAR format). Note on variation
features; even if we do not want to instantiate a mutant
chromosome/contig feature, we can still represent a SNP etc with 2
locations, one (rank 0) on the genome, the other (rank 1) would have
most fields null, except for alternative residues.';

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

COMMENT ON COLUMN featureloc.rank IS 'Used when a feature has >1
location, otherwise the default rank 0 is used. Some features (e.g.
blast hits and HSPs) have two locations - one on the query and one on
the subject. Rank is used to differentiate these. Rank=0 is always
used for the query, Rank=1 for the subject. For multiple alignments,
assignment of rank is arbitrary. Rank is also used for
sequence_variant features, such as SNPs. Rank=0 indicates the wildtype
(or baseline) feature, Rank=1 indicates the mutant (or compared) feature.';

CREATE FUNCTION feature_subalignments(bigint) RETURNS SETOF featureloc
    LANGUAGE plpgsql
    AS $_$
DECLARE
  return_data featureloc%ROWTYPE;
  f_id ALIAS FOR $1;
  feature_data feature%rowtype;
  featureloc_data featureloc%rowtype;

  s text;

  fmin bigint;
  slen bigint;
BEGIN
  --RAISE NOTICE 'feature_id is %', featureloc_data.feature_id;
  SELECT INTO feature_data * FROM feature WHERE feature_id = f_id;

  FOR featureloc_data IN SELECT * FROM featureloc WHERE feature_id = f_id LOOP

    --RAISE NOTICE 'fmin is %', featureloc_data.fmin;

    return_data.feature_id      = f_id;
    return_data.srcfeature_id   = featureloc_data.srcfeature_id;
    return_data.is_fmin_partial = featureloc_data.is_fmin_partial;
    return_data.is_fmax_partial = featureloc_data.is_fmax_partial;
    return_data.strand          = featureloc_data.strand;
    return_data.phase           = featureloc_data.phase;
    return_data.residue_info    = featureloc_data.residue_info;
    return_data.locgroup        = featureloc_data.locgroup;
    return_data.rank            = featureloc_data.rank;

    s = feature_data.residues;
    fmin = featureloc_data.fmin;
    slen = char_length(s);

    WHILE char_length(s) LOOP
      --RAISE NOTICE 'residues is %', s;

      --trim off leading match
      s = trim(leading '|ATCGNatcgn' from s);
      --if leading match detected
      IF slen > char_length(s) THEN
        return_data.fmin = fmin;
        return_data.fmax = featureloc_data.fmin + (slen - char_length(s));

        --if the string started with a match, return it,
        --otherwise, trim the gaps first (ie do not return this iteration)
        RETURN NEXT return_data;
      END IF;

      --trim off leading gap
      s = trim(leading '-' from s);

      fmin = featureloc_data.fmin + (slen - char_length(s));
    END LOOP;
  END LOOP;

  RETURN;

END;
$_$;

CREATE FUNCTION featureloc_slice(bigint, bigint) RETURNS SETOF featureloc
    LANGUAGE sql
    AS $_$SELECT * from featureloc where boxquery($1, $2) <@ boxrange(fmin,fmax)$_$;

CREATE FUNCTION featureloc_slice(bigint, bigint, bigint) RETURNS SETOF featureloc
    LANGUAGE sql
    AS $_$SELECT *
   FROM featureloc
   WHERE boxquery($1, $2, $3) && boxrange(srcfeature_id,fmin,fmax)$_$;

CREATE FUNCTION featureloc_slice(character varying, bigint, bigint) RETURNS SETOF featureloc
    LANGUAGE sql
    AS $_$SELECT featureloc.*
   FROM featureloc
   INNER JOIN feature AS srcf ON (srcf.feature_id = featureloc.srcfeature_id)
   WHERE boxquery($2, $3) <@ boxrange(fmin,fmax)
   AND srcf.name = $1 $_$;

CREATE FUNCTION featureslice(bigint, bigint) RETURNS SETOF featureloc
    LANGUAGE sql
    AS $_$SELECT * from featureloc where boxquery($1, $2) <@ boxrange(fmin,fmax)$_$;

CREATE FUNCTION fill_cvtermpath(bigint) RETURNS integer
    LANGUAGE plpgsql
    AS $_$
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
$_$;

CREATE FUNCTION fill_cvtermpath(character varying) RETURNS integer
    LANGUAGE plpgsql
    AS $_$
DECLARE
    cvname alias for $1;
    cv_id   bigint;
    rtn     int;
BEGIN

    SELECT INTO cv_id cv.cv_id from cv WHERE cv.name = cvname;
    SELECT INTO rtn fill_cvtermpath(cv_id);
    RETURN rtn;
END;
$_$;

CREATE FUNCTION get_all_object_ids(bigint) RETURNS SETOF cvtermpath
    LANGUAGE plpgsql
    AS $_$
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
$_$;

CREATE FUNCTION get_all_subject_ids(bigint) RETURNS SETOF cvtermpath
    LANGUAGE plpgsql
    AS $_$
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
$_$;

CREATE FUNCTION get_cv_id_for_feature() RETURNS bigint
    LANGUAGE sql
    AS $$SELECT cv_id FROM cv WHERE name='sequence'$$;

CREATE FUNCTION get_cv_id_for_feature_relationsgip() RETURNS bigint
    LANGUAGE sql
    AS $$SELECT cv_id FROM cv WHERE name='relationship'$$;

CREATE FUNCTION get_cv_id_for_featureprop() RETURNS bigint
    LANGUAGE sql
    AS $$SELECT cv_id FROM cv WHERE name='feature_property'$$;

CREATE FUNCTION get_cycle_cvterm_id(bigint) RETURNS bigint
    LANGUAGE plpgsql
    AS $_$
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
$_$;

CREATE FUNCTION get_cycle_cvterm_id(character varying) RETURNS bigint
    LANGUAGE plpgsql
    AS $_$
DECLARE
    cvname alias for $1;
    cv_id bigint;
    rtn bigint;
BEGIN

    SELECT INTO cv_id cv.cv_id from cv WHERE cv.name = cvname;
    SELECT INTO rtn  get_cycle_cvterm_id(cv_id);

    RETURN rtn;
END;
$_$;

CREATE FUNCTION get_cycle_cvterm_id(bigint, bigint) RETURNS bigint
    LANGUAGE plpgsql
    AS $_$
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
$_$;

CREATE FUNCTION get_cycle_cvterm_ids(bigint) RETURNS SETOF bigint
    LANGUAGE plpgsql
    AS $_$
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
$_$;

CREATE FUNCTION get_feature_id(character varying, character varying, character varying) RETURNS bigint
    LANGUAGE sql
    AS $_$
  SELECT feature_id
  FROM feature
  WHERE uniquename=$1
    AND type_id=get_feature_type_id($2)
    AND organism_id=get_organism_id($3)
 $_$;

CREATE FUNCTION get_feature_ids(text) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    sql alias for $1;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;
    myrc3 feature_by_fx_type%ROWTYPE;

BEGIN

    FOR myrc IN EXECUTE sql LOOP
        RETURN NEXT myrc;
        FOR myrc2 IN SELECT * FROM get_up_feature_ids(myrc.feature_id) LOOP
            RETURN NEXT myrc2;
        END LOOP;
        FOR myrc3 IN SELECT * FROM get_sub_feature_ids(myrc.feature_id) LOOP
            RETURN NEXT myrc3;
        END LOOP;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_feature_ids_by_child_count(character varying, character varying, integer, character varying, character) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    ptype alias for $1;
    ctype alias for $2;
    ccount alias for $3;
    operator alias for $4;
    is_an alias for $5;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type %ROWTYPE;

BEGIN

    query := 'SELECT DISTINCT f.feature_id
        FROM feature f INNER join (select count(*) as c, p.feature_id FROM feature p
        INNER join cvterm pt ON (p.type_id = pt.cvterm_id) INNER join feature_relationship fr
        ON (p.feature_id = fr.object_id) INNER join feature c ON (c.feature_id = fr.subject_id)
        INNER join cvterm ct ON (c.type_id = ct.cvterm_id)
        WHERE pt.name = ' || quote_literal(ptype) || ' AND ct.name = ' || quote_literal(ctype)
        || ' AND p.is_analysis = ' || quote_literal(is_an) || ' group by p.feature_id) as cq
        ON (cq.feature_id = f.feature_id) WHERE cq.c ' || operator || ccount || ';';
    ---RAISE NOTICE '%', query;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_feature_ids_by_ont(character varying, character varying) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    aspect alias for $1;
    term alias for $2;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := 'SELECT DISTINCT fcvt.feature_id
        FROM feature_cvterm fcvt, cv, cvterm t WHERE cv.cv_id = t.cv_id AND
        t.cvterm_id = fcvt.cvterm_id AND cv.name = ' || quote_literal(aspect) ||
        ' AND t.name = ' || quote_literal(term) || ';';
    IF (STRPOS(term, '%') > 0) THEN
        query := 'SELECT DISTINCT fcvt.feature_id
            FROM feature_cvterm fcvt, cv, cvterm t WHERE cv.cv_id = t.cv_id AND
            t.cvterm_id = fcvt.cvterm_id AND cv.name = ' || quote_literal(aspect) ||
            ' AND t.name like ' || quote_literal(term) || ';';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_feature_ids_by_ont_root(character varying, character varying) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    aspect alias for $1;
    term alias for $2;
    query TEXT;
    subquery TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    subquery := 'SELECT t.cvterm_id FROM cv, cvterm t WHERE cv.cv_id = t.cv_id
        AND cv.name = ' || quote_literal(aspect) || ' AND t.name = ' || quote_literal(term) || ';';
    IF (STRPOS(term, '%') > 0) THEN
        subquery := 'SELECT t.cvterm_id FROM cv, cvterm t WHERE cv.cv_id = t.cv_id
            AND cv.name = ' || quote_literal(aspect) || ' AND t.name like ' || quote_literal(term) || ';';
    END IF;
    query := 'SELECT DISTINCT fcvt.feature_id
        FROM feature_cvterm fcvt INNER JOIN (SELECT cvterm_id FROM get_it_sub_cvterm_ids(' || quote_literal(subquery) || ')) AS ont ON (fcvt.cvterm_id = ont.cvterm_id);';

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_feature_ids_by_property(character varying, character varying) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    p_type alias for $1;
    p_val alias for $2;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := 'SELECT DISTINCT fprop.feature_id
        FROM featureprop fprop, cvterm t WHERE t.cvterm_id = fprop.type_id AND t.name = ' ||
        quote_literal(p_type) || ' AND fprop.value = ' || quote_literal(p_val) || ';';
    IF (STRPOS(p_val, '%') > 0) THEN
        query := 'SELECT DISTINCT fprop.feature_id
            FROM featureprop fprop, cvterm t WHERE t.cvterm_id = fprop.type_id AND t.name = ' ||
            quote_literal(p_type) || ' AND fprop.value like ' || quote_literal(p_val) || ';';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_feature_ids_by_propval(character varying) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    p_val alias for $1;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := 'SELECT DISTINCT fprop.feature_id
        FROM featureprop fprop WHERE fprop.value = ' || quote_literal(p_val) || ';';
    IF (STRPOS(p_val, '%') > 0) THEN
        query := 'SELECT DISTINCT fprop.feature_id
            FROM featureprop fprop WHERE fprop.value like ' || quote_literal(p_val) || ';';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_feature_ids_by_type(character varying, character) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    gtype alias for $1;
    is_an alias for $2;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := 'SELECT DISTINCT f.feature_id
        FROM feature f, cvterm t WHERE t.cvterm_id = f.type_id AND t.name = ' || quote_literal(gtype) ||
        ' AND f.is_analysis = ' || quote_literal(is_an) || ';';
    IF (STRPOS(gtype, '%') > 0) THEN
        query := 'SELECT DISTINCT f.feature_id
            FROM feature f, cvterm t WHERE t.cvterm_id = f.type_id AND t.name like '
            || quote_literal(gtype) || ' AND f.is_analysis = ' || quote_literal(is_an) || ';';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_feature_ids_by_type_name(character varying, text, character) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    gtype alias for $1;
    name alias for $2;
    is_an alias for $3;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := 'SELECT DISTINCT f.feature_id
        FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id)
        WHERE t.name = ' || quote_literal(gtype) || ' AND (f.uniquename = ' || quote_literal(name)
        || ' OR f.name = ' || quote_literal(name) || ') AND f.is_analysis = ' || quote_literal(is_an) || ';';

    IF (STRPOS(name, '%') > 0) THEN
        query := 'SELECT DISTINCT f.feature_id
            FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id)
            WHERE t.name = ' || quote_literal(gtype) || ' AND (f.uniquename like ' || quote_literal(name)
            || ' OR f.name like ' || quote_literal(name) || ') AND f.is_analysis = ' || quote_literal(is_an) || ';';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_feature_ids_by_type_src(character varying, text, character) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    gtype alias for $1;
    src alias for $2;
    is_an alias for $3;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := 'SELECT DISTINCT f.feature_id
        FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id) INNER join featureloc fl
        ON (f.feature_id = fl.feature_id) INNER join feature src ON (src.feature_id = fl.srcfeature_id)
        WHERE t.name = ' || quote_literal(gtype) || ' AND src.uniquename = ' || quote_literal(src)
        || ' AND f.is_analysis = ' || quote_literal(is_an) || ';';

    IF (STRPOS(gtype, '%') > 0) THEN
        query := 'SELECT DISTINCT f.feature_id
            FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id) INNER join featureloc fl
            ON (f.feature_id = fl.feature_id) INNER join feature src ON (src.feature_id = fl.srcfeature_id)
            WHERE t.name like ' || quote_literal(gtype) || ' AND src.uniquename = ' || quote_literal(src)
            || ' AND f.is_analysis = ' || quote_literal(is_an) || ';';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_feature_relationship_type_id(character varying) RETURNS bigint
    LANGUAGE sql
    AS $_$
  SELECT cvterm_id
  FROM cv INNER JOIN cvterm USING (cv_id)
  WHERE cvterm.name=$1 AND cv.name='relationship'
 $_$;

CREATE FUNCTION get_feature_type_id(character varying) RETURNS bigint
    LANGUAGE sql
    AS $_$
  SELECT cvterm_id
  FROM cv INNER JOIN cvterm USING (cv_id)
  WHERE cvterm.name=$1 AND cv.name='sequence'
 $_$;

CREATE FUNCTION get_featureprop_type_id(character varying) RETURNS bigint
    LANGUAGE sql
    AS $_$
  SELECT cvterm_id
  FROM cv INNER JOIN cvterm USING (cv_id)
  WHERE cvterm.name=$1 AND cv.name='feature_property'
 $_$;

CREATE FUNCTION get_graph_above(bigint) RETURNS SETOF cvtermpath
    LANGUAGE plpgsql
    AS $_$
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
$_$;

CREATE FUNCTION get_graph_below(bigint) RETURNS SETOF cvtermpath
    LANGUAGE plpgsql
    AS $_$
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
$_$;

CREATE TABLE cvterm (
    cvterm_id bigint NOT NULL,
    cv_id bigint NOT NULL,
    name character varying(1024) NOT NULL,
    definition text,
    dbxref_id bigint NOT NULL,
    is_obsolete integer DEFAULT 0 NOT NULL,
    is_relationshiptype integer DEFAULT 0 NOT NULL
);

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

CREATE FUNCTION get_it_sub_cvterm_ids(text) RETURNS SETOF cvterm
    LANGUAGE plpgsql
    AS $_$
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
$_$;

CREATE FUNCTION get_organism_id(character varying) RETURNS bigint
    LANGUAGE sql
    AS $_$
SELECT organism_id
  FROM organism
  WHERE genus=substring($1,1,position(' ' IN $1)-1)
    AND species=substring($1,position(' ' IN $1)+1)
 $_$;

CREATE FUNCTION get_organism_id(character varying, character varying) RETURNS bigint
    LANGUAGE sql
    AS $_$
  SELECT organism_id
  FROM organism
  WHERE genus=$1
    AND species=$2
 $_$;

CREATE FUNCTION get_organism_id_abbrev(character varying) RETURNS bigint
    LANGUAGE sql
    AS $_$
SELECT organism_id
  FROM organism
  WHERE substr(genus,1,1)=substring($1,1,1)
    AND species=substring($1,position(' ' IN $1)+1)
 $_$;

CREATE FUNCTION get_sub_feature_ids(bigint) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    root alias for $1;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN
    FOR myrc IN SELECT DISTINCT subject_id AS feature_id FROM feature_relationship WHERE object_id = root LOOP
        RETURN NEXT myrc;
        FOR myrc2 IN SELECT * FROM get_sub_feature_ids(myrc.feature_id) LOOP
            RETURN NEXT myrc2;
        END LOOP;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_sub_feature_ids(text) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    sql alias for $1;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN
    FOR myrc IN EXECUTE sql LOOP
        FOR myrc2 IN SELECT * FROM get_sub_feature_ids(myrc.feature_id) LOOP
            RETURN NEXT myrc2;
        END LOOP;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_sub_feature_ids(bigint, integer) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    root alias for $1;
    depth alias for $2;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN
    FOR myrc IN SELECT DISTINCT subject_id AS feature_id, depth FROM feature_relationship WHERE object_id = root LOOP
        RETURN NEXT myrc;
        FOR myrc2 IN SELECT * FROM get_sub_feature_ids(myrc.feature_id,depth+1) LOOP
            RETURN NEXT myrc2;
        END LOOP;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_sub_feature_ids_by_type_src(character varying, text, character) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    gtype alias for $1;
    src alias for $2;
    is_an alias for $3;
    query text;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := 'SELECT DISTINCT f.feature_id FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id)
        INNER join featureloc fl
        ON (f.feature_id = fl.feature_id) INNER join feature src ON (src.feature_id = fl.srcfeature_id)
        WHERE t.name = ' || quote_literal(gtype) || ' AND src.uniquename = ' || quote_literal(src)
        || ' AND f.is_analysis = ' || quote_literal(is_an) || ';';

    IF (STRPOS(gtype, '%') > 0) THEN
        query := 'SELECT DISTINCT f.feature_id FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id)
             INNER join featureloc fl
            ON (f.feature_id = fl.feature_id) INNER join feature src ON (src.feature_id = fl.srcfeature_id)
            WHERE t.name like ' || quote_literal(gtype) || ' AND src.uniquename = ' || quote_literal(src)
            || ' AND f.is_analysis = ' || quote_literal(is_an) || ';';
    END IF;
    FOR myrc IN SELECT * FROM get_sub_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_up_feature_ids(bigint) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    leaf alias for $1;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;
BEGIN
    FOR myrc IN SELECT DISTINCT object_id AS feature_id FROM feature_relationship WHERE subject_id = leaf LOOP
        RETURN NEXT myrc;
        FOR myrc2 IN SELECT * FROM get_up_feature_ids(myrc.feature_id) LOOP
            RETURN NEXT myrc2;
        END LOOP;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_up_feature_ids(text) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    sql alias for $1;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN
    FOR myrc IN EXECUTE sql LOOP
        FOR myrc2 IN SELECT * FROM get_up_feature_ids(myrc.feature_id) LOOP
            RETURN NEXT myrc2;
        END LOOP;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION get_up_feature_ids(bigint, integer) RETURNS SETOF feature_by_fx_type
    LANGUAGE plpgsql
    AS $_$
DECLARE
    leaf alias for $1;
    depth alias for $2;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;
BEGIN
    FOR myrc IN SELECT DISTINCT object_id AS feature_id, depth FROM feature_relationship WHERE subject_id = leaf LOOP
        RETURN NEXT myrc;
        FOR myrc2 IN SELECT * FROM get_up_feature_ids(myrc.feature_id,depth+1) LOOP
            RETURN NEXT myrc2;
        END LOOP;
    END LOOP;
    RETURN;
END;
$_$;

CREATE FUNCTION gffattstring(bigint) RETURNS character varying
    LANGUAGE plpgsql
    AS $_$DECLARE
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
  return_string = 'ID=' || feature_row.uniquename;
  IF name IS NOT NULL AND name != ''
  THEN
    return_string = return_string ||';' || 'Name=' || name;
  END IF;

  --Get Parent from feature_relationship
  SELECT INTO feature_row * FROM feature f, feature_relationship fr
    WHERE fr.subject_id = f_id AND fr.object_id = f.feature_id;
  IF FOUND
  THEN
    return_string = return_string||';'||'Parent='||feature_row.uniquename;
  END IF;

  FOR atts_view IN SELECT * FROM gff3atts WHERE feature_id = f_id  LOOP
    escape_loc = position(';' in atts_view.attribute);
    IF escape_loc > 0 THEN
      atts_view.attribute = replace(atts_view.attribute, ';', '%3B');
    END IF;
    return_string = return_string || ';'
                     || atts_view.type || '='
                     || atts_view.attribute;
  END LOOP;

  RETURN return_string;
END;
$_$;

CREATE TABLE db (
    db_id bigint NOT NULL,
    name character varying(255) NOT NULL,
    description character varying(255),
    urlprefix character varying(255),
    url character varying(255)
);

COMMENT ON TABLE db IS 'A database authority. Typical databases in
bioinformatics are FlyBase, GO, UniProt, NCBI, MGI, etc. The authority
is generally known by this shortened form, which is unique within the
bioinformatics and biomedical realm.  To Do - add support for URIs,
URNs (e.g. LSIDs). We can do this by treating the URL as a URI -
however, some applications may expect this to be resolvable - to be
decided.';

CREATE TABLE dbxref (
    dbxref_id bigint NOT NULL,
    db_id bigint NOT NULL,
    accession character varying(1024) NOT NULL,
    version character varying(255) DEFAULT ''::character varying NOT NULL,
    description text
);

COMMENT ON TABLE dbxref IS 'A unique, global, public, stable identifier. Not necessarily an external reference - can reference data items inside the particular chado instance being used. Typically a row in a table can be uniquely identified with a primary identifier (called dbxref_id); a table may also have secondary identifiers (in a linking table <T>_dbxref). A dbxref is generally written as <DB>:<ACCESSION> or as <DB>:<ACCESSION>:<VERSION>.';

COMMENT ON COLUMN dbxref.accession IS 'The local part of the identifier. Guaranteed by the db authority to be unique for that db.';

CREATE TABLE feature_cvterm (
    feature_cvterm_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    cvterm_id bigint NOT NULL,
    pub_id bigint NOT NULL,
    is_not boolean DEFAULT false NOT NULL,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE feature_cvterm IS 'Associate a term from a cv with a feature, for example, GO annotation.';

COMMENT ON COLUMN feature_cvterm.pub_id IS 'Provenance for the annotation. Each annotation should have a single primary publication (which may be of the appropriate type for computational analyses) where more details can be found. Additional provenance dbxrefs can be attached using feature_cvterm_dbxref.';

COMMENT ON COLUMN feature_cvterm.is_not IS 'If this is set to true, then this annotation is interpreted as a NEGATIVE annotation - i.e. the feature does NOT have the specified function, process, component, part, etc. See GO docs for more details.';

CREATE TABLE feature_dbxref (
    feature_dbxref_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    dbxref_id bigint NOT NULL,
    is_current boolean DEFAULT true NOT NULL
);

COMMENT ON TABLE feature_dbxref IS 'Links a feature to dbxrefs.';

COMMENT ON COLUMN feature_dbxref.is_current IS 'True if this secondary dbxref is
the most up to date accession in the corresponding db. Retired accessions
should set this field to false';

CREATE TABLE feature_pub (
    feature_pub_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE feature_pub IS 'Provenance. Linking table between features and publications that mention them.';

CREATE TABLE feature_synonym (
    feature_synonym_id bigint NOT NULL,
    synonym_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    pub_id bigint NOT NULL,
    is_current boolean DEFAULT false NOT NULL,
    is_internal boolean DEFAULT false NOT NULL
);

COMMENT ON TABLE feature_synonym IS 'Linking table between feature and synonym.';

COMMENT ON COLUMN feature_synonym.pub_id IS 'The pub_id link is for relating the usage of a given synonym to the publication in which it was used.';

COMMENT ON COLUMN feature_synonym.is_current IS 'The is_current boolean indicates whether the linked synonym is the  current -official- symbol for the linked feature.';

COMMENT ON COLUMN feature_synonym.is_internal IS 'Typically a synonym exists so that somebody querying the db with an obsolete name can find the object theyre looking for (under its current name.  If the synonym has been used publicly and deliberately (e.g. in a paper), it may also be listed in reports as a synonym. If the synonym was not used deliberately (e.g. there was a typo which went public), then the is_internal boolean may be set to -true- so that it is known that the synonym is -internal- and should be queryable but should not be listed in reports as a valid synonym.';

CREATE TABLE featureprop (
    featureprop_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

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

CREATE TABLE pub (
    pub_id bigint NOT NULL,
    title text,
    volumetitle text,
    volume character varying(255),
    series_name character varying(255),
    issue character varying(255),
    pyear character varying(255),
    pages character varying(255),
    miniref character varying(255),
    uniquename text NOT NULL,
    type_id bigint NOT NULL,
    is_obsolete boolean DEFAULT false,
    publisher character varying(255),
    pubplace character varying(255)
);

COMMENT ON TABLE pub IS 'A documented provenance artefact - publications,
documents, personal communication.';

COMMENT ON COLUMN pub.title IS 'Descriptive general heading.';

COMMENT ON COLUMN pub.volumetitle IS 'Title of part if one of a series.';

COMMENT ON COLUMN pub.series_name IS 'Full name of (journal) series.';

COMMENT ON COLUMN pub.pages IS 'Page number range[s], e.g. 457--459, viii + 664pp, lv--lvii.';

COMMENT ON COLUMN pub.type_id IS 'The type of the publication (book, journal, poem, graffiti, etc). Uses pub cv.';

CREATE TABLE synonym (
    synonym_id bigint NOT NULL,
    name character varying(255) NOT NULL,
    type_id bigint NOT NULL,
    synonym_sgml character varying(255) NOT NULL
);

COMMENT ON TABLE synonym IS 'A synonym for a feature. One feature can have multiple synonyms, and the same synonym can apply to multiple features.';

COMMENT ON COLUMN synonym.name IS 'The synonym itself. Should be human-readable machine-searchable ascii text.';

COMMENT ON COLUMN synonym.type_id IS 'Types would be symbol and fullname for now.';

COMMENT ON COLUMN synonym.synonym_sgml IS 'The fully specified synonym, with any non-ascii characters encoded in SGML.';

CREATE VIEW gffatts AS
 SELECT fs.feature_id,
    'Ontology_term'::text AS type,
    s.name AS attribute
   FROM cvterm s,
    feature_cvterm fs
  WHERE (fs.cvterm_id = s.cvterm_id)
UNION ALL
 SELECT fs.feature_id,
    'Dbxref'::text AS type,
    (((d.name)::text || ':'::text) || (s.accession)::text) AS attribute
   FROM dbxref s,
    feature_dbxref fs,
    db d
  WHERE ((fs.dbxref_id = s.dbxref_id) AND (s.db_id = d.db_id))
UNION ALL
 SELECT fs.feature_id,
    'Alias'::text AS type,
    s.name AS attribute
   FROM synonym s,
    feature_synonym fs
  WHERE (fs.synonym_id = s.synonym_id)
UNION ALL
 SELECT fp.feature_id,
    cv.name AS type,
    fp.value AS attribute
   FROM featureprop fp,
    cvterm cv
  WHERE (fp.type_id = cv.cvterm_id)
UNION ALL
 SELECT fs.feature_id,
    'pub'::text AS type,
    (((s.series_name)::text || ':'::text) || s.title) AS attribute
   FROM pub s,
    feature_pub fs
  WHERE (fs.pub_id = s.pub_id);

CREATE FUNCTION gfffeatureatts(bigint) RETURNS SETOF gffatts
    LANGUAGE sql
    AS $_$
SELECT feature_id, 'Ontology_term' AS type,  s.name AS attribute
FROM cvterm s, feature_cvterm fs
WHERE fs.feature_id= $1 AND fs.cvterm_id = s.cvterm_id
UNION
SELECT feature_id, 'Dbxref' AS type, d.name || ':' || s.accession AS attribute
FROM dbxref s, feature_dbxref fs, db d
WHERE fs.feature_id= $1 AND fs.dbxref_id = s.dbxref_id AND s.db_id = d.db_id
UNION
SELECT feature_id, 'Alias' AS type, s.name AS attribute
FROM synonym s, feature_synonym fs
WHERE fs.feature_id= $1 AND fs.synonym_id = s.synonym_id
UNION
SELECT fp.feature_id,cv.name,fp.value
FROM featureprop fp, cvterm cv
WHERE fp.feature_id= $1 AND fp.type_id = cv.cvterm_id
UNION
SELECT feature_id, 'pub' AS type, s.series_name || ':' || s.title AS attribute
FROM pub s, feature_pub fs
WHERE fs.feature_id= $1 AND fs.pub_id = s.pub_id
$_$;

CREATE FUNCTION order_exons(bigint) RETURNS void
    LANGUAGE plpgsql
    AS $_$
  DECLARE
    parent_type      ALIAS FOR $1;
    exon_id          bigint;
    part_of          bigint;
    exon_type        bigint;
    strand           int;
    arow             RECORD;
    order_by         varchar;
    rowcount         int;
    exon_count       int;
    ordered_exons    int;
    transcript_id    bigint;
    transcript_row   feature%ROWTYPE;
  BEGIN
    SELECT INTO part_of cvterm_id FROM cvterm WHERE name='part_of'
      AND cv_id IN (SELECT cv_id FROM cv WHERE name='relationship');
    --SELECT INTO exon_type cvterm_id FROM cvterm WHERE name='exon'
    --  AND cv_id IN (SELECT cv_id FROM cv WHERE name='sequence');

    --RAISE NOTICE 'part_of %, exon %',part_of,exon_type;

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
        --RAISE NOTICE 'skipping transcript %, row count %',transcript_id,rowcount;
        CONTINUE;
      END IF;

      --need to reverse the order if the strand is negative
      SELECT INTO strand strand FROM featureloc WHERE feature_id=transcript_id;
      IF strand > 0 THEN
          order_by = 'fl.fmin';
      ELSE
          order_by = 'fl.fmax desc';
      END IF;

      exon_count = 0;
      FOR arow IN EXECUTE
        'SELECT fr.*, fl.fmin, fl.fmax
          FROM feature_relationship fr, featureloc fl
          WHERE fr.object_id  = '||transcript_id||'
            AND fr.subject_id = fl.feature_id
            AND fr.type_id    = '||part_of||'
            ORDER BY '||order_by
      LOOP
        --number the exons for a given transcript
        UPDATE feature_relationship
          SET rank = exon_count
          WHERE feature_relationship_id = arow.feature_relationship_id;
        exon_count = exon_count + 1;
      END LOOP;

    END LOOP;

  END;
$_$;

CREATE FUNCTION phylonode_depth(bigint) RETURNS double precision
    LANGUAGE plpgsql
    AS $_$DECLARE  id    ALIAS FOR $1;
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
$_$;

CREATE FUNCTION phylonode_height(bigint) RETURNS double precision
    LANGUAGE sql
    AS $_$
  SELECT coalesce(max(phylonode_height(phylonode_id) + distance), 0.0)
    FROM phylonode
    WHERE parent_phylonode_id = $1
$_$;

CREATE FUNCTION project_featureloc_up(bigint, bigint) RETURNS featureloc
    LANGUAGE plpgsql
    AS $_$
DECLARE
    in_featureloc_id alias for $1;
    up_srcfeature_id alias for $2;
    in_featureloc featureloc%ROWTYPE;
    up_featureloc featureloc%ROWTYPE;
    nu_featureloc featureloc%ROWTYPE;
    nu_fmin BIGINT;
    nu_fmax BIGINT;
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
$_$;

CREATE FUNCTION project_point_down(bigint, bigint, bigint, bigint) RETURNS bigint
    LANGUAGE sql
    AS $_$SELECT
  CASE WHEN $4<0
   THEN $3-$1
   ELSE $1+$2
  END AS p$_$;

CREATE FUNCTION project_point_g2t(bigint, bigint, bigint) RETURNS bigint
    LANGUAGE plpgsql
    AS $_$
 DECLARE
    in_p             alias for $1;
    srcf_id          alias for $2;
    t_id             alias for $3;
    e_floc           featureloc%ROWTYPE;
    out_p            BIGINT;
    exon_cvterm_id   BIGINT;
BEGIN
 SELECT INTO exon_cvterm_id get_feature_type_id('exon');
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
$_$;

CREATE FUNCTION project_point_up(bigint, bigint, bigint, bigint) RETURNS bigint
    LANGUAGE sql
    AS $_$SELECT
  CASE WHEN $4<0
   THEN $3-$1             -- rev strand
   ELSE $1-$2             -- fwd strand
  END AS p$_$;

CREATE FUNCTION reverse_complement(text) RETURNS text
    LANGUAGE sql
    AS $_$SELECT reverse_string(complement_residues($1))$_$;

CREATE FUNCTION reverse_string(text) RETURNS text
    LANGUAGE plpgsql
    AS $_$
 DECLARE
  reversed_string TEXT;
  incoming ALIAS FOR $1;
 BEGIN
   reversed_string = '';
   FOR i IN REVERSE char_length(incoming)..1 loop
     reversed_string = reversed_string || substring(incoming FROM i FOR 1);
   END loop;
 RETURN reversed_string;
END$_$;

CREATE FUNCTION share_exons() RETURNS void
    LANGUAGE plpgsql
    AS $$
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
      WHERE gene_type_cv.name = 'sequence'
        AND gene_type.name = 'gene'
        AND transcript1_type_cv.name = 'sequence'
        AND transcript1_type.name = 'mRNA'
        AND transcript2_type_cv.name = 'sequence'
        AND transcript2_type.name = 'mRNA'
        AND exon1_type_cv.name = 'sequence'
        AND exon1_type.name = 'exon'
        AND exon2_type_cv.name = 'sequence'
        AND exon2_type.name = 'exon'
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
$$;

CREATE FUNCTION store_analysis(character varying, character varying, character varying) RETURNS bigint
    LANGUAGE plpgsql
    AS $_$DECLARE
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
      RETURN currval('analysis_analysis_id_seq');
    END IF;
    RETURN pkval;
 END;
$_$;

CREATE FUNCTION store_db(character varying) RETURNS bigint
    LANGUAGE plpgsql
    AS $_$DECLARE
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
       RETURN currval('db_db_id_seq');
    END IF;
    RETURN v_db_id;
 END;
$_$;

CREATE FUNCTION store_dbxref(character varying, character varying) RETURNS bigint
    LANGUAGE plpgsql
    AS $_$DECLARE
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
       RETURN currval('dbxref_dbxref_id_seq');
    END IF;
    RETURN v_dbxref_id;
 END;
$_$;

CREATE FUNCTION store_feature(integer, integer, integer, integer, integer, integer, character varying, character varying, integer, boolean) RETURNS integer
    LANGUAGE plpgsql
    AS $_$DECLARE
  v_srcfeature_id       ALIAS FOR $1;
  v_fmin                ALIAS FOR $2;
  v_fmax                ALIAS FOR $3;
  v_strand              ALIAS FOR $4;
  v_dbxref_id           ALIAS FOR $5;
  v_organism_id         ALIAS FOR $6;
  v_name                ALIAS FOR $7;
  v_uniquename          ALIAS FOR $8;
  v_type_id             ALIAS FOR $9;
  v_is_analysis         ALIAS FOR $10;
  v_feature_id          INT;
  v_featureloc_id       INT;
 BEGIN
    IF v_dbxref_id IS NULL THEN
      SELECT INTO v_feature_id feature_id
      FROM feature
      WHERE uniquename=v_uniquename     AND
            organism_id=v_organism_id   AND
            type_id=v_type_id;
    ELSE
      SELECT INTO v_feature_id feature_id
      FROM feature
      WHERE dbxref_id=v_dbxref_id;
    END IF;
    IF NOT FOUND THEN
      INSERT INTO feature
       ( dbxref_id           ,
         organism_id         ,
         name                ,
         uniquename          ,
         type_id             ,
         is_analysis         )
        VALUES
        ( v_dbxref_id           ,
          v_organism_id         ,
          v_name                ,
          v_uniquename          ,
          v_type_id             ,
          v_is_analysis         );
      v_feature_id = currval('feature_feature_id_seq');
    ELSE
      UPDATE feature SET
        dbxref_id   =  v_dbxref_id           ,
        organism_id =  v_organism_id         ,
        name        =  v_name                ,
        uniquename  =  v_uniquename          ,
        type_id     =  v_type_id             ,
        is_analysis =  v_is_analysis
      WHERE
        feature_id=v_feature_id;
    END IF;
  PERFORM store_featureloc(v_feature_id,
                           v_srcfeature_id,
                           v_fmin,
                           v_fmax,
                           v_strand,
                           0,
                           0);
  RETURN v_feature_id;
 END;
$_$;

CREATE FUNCTION store_feature_synonym(integer, character varying, integer, boolean, boolean, integer) RETURNS integer
    LANGUAGE plpgsql
    AS $_$DECLARE
  v_feature_id          ALIAS FOR $1;
  v_syn                 ALIAS FOR $2;
  v_type_id             ALIAS FOR $3;
  v_is_current          ALIAS FOR $4;
  v_is_internal         ALIAS FOR $5;
  v_pub_id              ALIAS FOR $6;
  v_synonym_id          INT;
  v_feature_synonym_id  INT;
 BEGIN
    IF v_feature_id IS NULL THEN RAISE EXCEPTION 'feature_id cannot be null';
    END IF;
    SELECT INTO v_synonym_id synonym_id
      FROM synonym
      WHERE name=v_syn                  AND
            type_id=v_type_id;
    IF NOT FOUND THEN
      INSERT INTO synonym
        ( name,
          synonym_sgml,
          type_id)
        VALUES
        ( v_syn,
          v_syn,
          v_type_id);
      v_synonym_id = currval('synonym_synonym_id_seq');
    END IF;
    SELECT INTO v_feature_synonym_id feature_synonym_id
        FROM feature_synonym
        WHERE feature_id=v_feature_id   AND
              synonym_id=v_synonym_id   AND
              pub_id=v_pub_id;
    IF NOT FOUND THEN
      INSERT INTO feature_synonym
        ( feature_id,
          synonym_id,
          pub_id,
          is_current,
          is_internal)
        VALUES
        ( v_feature_id,
          v_synonym_id,
          v_pub_id,
          v_is_current,
          v_is_internal);
      v_feature_synonym_id = currval('feature_synonym_feature_synonym_id_seq');
    ELSE
      UPDATE feature_synonym
        SET is_current=v_is_current, is_internal=v_is_internal
        WHERE feature_synonym_id=v_feature_synonym_id;
    END IF;
  RETURN v_feature_synonym_id;
 END;
$_$;

CREATE FUNCTION store_featureloc(integer, integer, integer, integer, integer, integer, integer) RETURNS integer
    LANGUAGE plpgsql
    AS $_$DECLARE
  v_feature_id          ALIAS FOR $1;
  v_srcfeature_id       ALIAS FOR $2;
  v_fmin                ALIAS FOR $3;
  v_fmax                ALIAS FOR $4;
  v_strand              ALIAS FOR $5;
  v_rank                ALIAS FOR $6;
  v_locgroup            ALIAS FOR $7;
  v_featureloc_id       INT;
 BEGIN
    IF v_feature_id IS NULL THEN RAISE EXCEPTION 'feature_id cannot be null';
    END IF;
    SELECT INTO v_featureloc_id featureloc_id
      FROM featureloc
      WHERE feature_id=v_feature_id     AND
            rank=v_rank                 AND
            locgroup=v_locgroup;
    IF NOT FOUND THEN
      INSERT INTO featureloc
        ( feature_id,
          srcfeature_id,
          fmin,
          fmax,
          strand,
          rank,
          locgroup)
        VALUES
        (  v_feature_id,
           v_srcfeature_id,
           v_fmin,
           v_fmax,
           v_strand,
           v_rank,
           v_locgroup);
      v_featureloc_id = currval('featureloc_featureloc_id_seq');
    ELSE
      UPDATE featureloc SET
        feature_id    =  v_feature_id,
        srcfeature_id =  v_srcfeature_id,
        fmin          =  v_fmin,
        fmax          =  v_fmax,
        strand        =  v_strand,
        rank          =  v_rank,
        locgroup      =  v_locgroup
      WHERE
        featureloc_id=v_featureloc_id;
    END IF;
  RETURN v_featureloc_id;
 END;
$_$;

CREATE FUNCTION store_organism(character varying, character varying, character varying) RETURNS bigint
    LANGUAGE plpgsql
    AS $_$DECLARE
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
       RETURN currval('organism_organism_id_seq');
    ELSE
      UPDATE organism
       SET common_name=v_common_name
      WHERE organism_id = v_organism_id;
    END IF;
    RETURN v_organism_id;
 END;
$_$;

CREATE FUNCTION subsequence(bigint, bigint, bigint, integer) RETURNS text
    LANGUAGE sql
    AS $_$SELECT
  CASE WHEN $4<0
   THEN reverse_complement(substring(srcf.residues,CAST(($2+1) as int),CAST(($3-$2) as int)))
   ELSE substring(residues,CAST(($2+1) as int),CAST(($3-$2) as int))
  END AS residues
  FROM feature AS srcf
  WHERE
   srcf.feature_id=$1$_$;

CREATE FUNCTION subsequence_by_feature(bigint) RETURNS text
    LANGUAGE sql
    AS $_$SELECT subsequence_by_feature($1,0,0)$_$;

CREATE FUNCTION subsequence_by_feature(bigint, integer, integer) RETURNS text
    LANGUAGE sql
    AS $_$SELECT
  CASE WHEN strand<0
   THEN reverse_complement(substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int)))
   ELSE substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int))
  END AS residues
  FROM feature AS srcf
   INNER JOIN featureloc ON (srcf.feature_id=featureloc.srcfeature_id)
  WHERE
   featureloc.feature_id=$1 AND
   featureloc.rank=$2 AND
   featureloc.locgroup=$3$_$;

CREATE FUNCTION subsequence_by_featureloc(bigint) RETURNS text
    LANGUAGE sql
    AS $_$SELECT
  CASE WHEN strand<0
   THEN reverse_complement(substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int)))
   ELSE substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int))
  END AS residues
  FROM feature AS srcf
   INNER JOIN featureloc ON (srcf.feature_id=featureloc.srcfeature_id)
  WHERE
   featureloc_id=$1$_$;

CREATE FUNCTION subsequence_by_subfeatures(bigint) RETURNS text
    LANGUAGE sql
    AS $_$
SELECT subsequence_by_subfeatures($1,get_feature_relationship_type_id('part_of'),0,0)
$_$;

CREATE FUNCTION subsequence_by_subfeatures(bigint, bigint) RETURNS text
    LANGUAGE sql
    AS $_$SELECT subsequence_by_subfeatures($1,$2,0,0)$_$;

CREATE FUNCTION subsequence_by_subfeatures(bigint, bigint, integer, integer) RETURNS text
    LANGUAGE plpgsql
    AS $_$
DECLARE v_feature_id ALIAS FOR $1;
DECLARE v_rtype_id   ALIAS FOR $2;
DECLARE v_rank       ALIAS FOR $3;
DECLARE v_locgroup   ALIAS FOR $4;
DECLARE subseq       TEXT;
DECLARE seqrow       RECORD;
BEGIN
  subseq = '';
 FOR seqrow IN
   SELECT
    CASE WHEN strand<0
     THEN reverse_complement(substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int)))
     ELSE substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int))
    END AS residues
    FROM feature AS srcf
     INNER JOIN featureloc ON (srcf.feature_id=featureloc.srcfeature_id)
     INNER JOIN feature_relationship AS fr
       ON (fr.subject_id=featureloc.feature_id)
    WHERE
     fr.object_id=v_feature_id AND
     fr.type_id=v_rtype_id AND
     featureloc.rank=v_rank AND
     featureloc.locgroup=v_locgroup
    ORDER BY fr.rank
  LOOP
   subseq = subseq  || seqrow.residues;
  END LOOP;
 RETURN subseq;
END
$_$;

CREATE FUNCTION subsequence_by_typed_subfeatures(bigint, bigint) RETURNS text
    LANGUAGE sql
    AS $_$SELECT subsequence_by_typed_subfeatures($1,$2,0,0)$_$;

CREATE FUNCTION subsequence_by_typed_subfeatures(bigint, bigint, integer, integer) RETURNS text
    LANGUAGE plpgsql
    AS $_$
DECLARE v_feature_id ALIAS FOR $1;
DECLARE v_ftype_id   ALIAS FOR $2;
DECLARE v_rank       ALIAS FOR $3;
DECLARE v_locgroup   ALIAS FOR $4;
DECLARE subseq       TEXT;
DECLARE seqrow       RECORD;
BEGIN
  subseq = '';
 FOR seqrow IN
   SELECT
    CASE WHEN strand<0
     THEN reverse_complement(substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int)))
     ELSE substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int))
    END AS residues
  FROM feature AS srcf
   INNER JOIN featureloc ON (srcf.feature_id=featureloc.srcfeature_id)
   INNER JOIN feature AS subf ON (subf.feature_id=featureloc.feature_id)
   INNER JOIN feature_relationship AS fr ON (fr.subject_id=subf.feature_id)
  WHERE
     fr.object_id=v_feature_id AND
     subf.type_id=v_ftype_id AND
     featureloc.rank=v_rank AND
     featureloc.locgroup=v_locgroup
  ORDER BY fr.rank
   LOOP
   subseq = subseq  || seqrow.residues;
  END LOOP;
 RETURN subseq;
END
$_$;

CREATE FUNCTION translate_codon(text, bigint) RETURNS character
    LANGUAGE sql
    AS $_$SELECT aa FROM genetic_code.gencode_codon_aa WHERE codon=$1 AND gencode_id=$2$_$;

CREATE FUNCTION translate_dna(text) RETURNS text
    LANGUAGE sql
    AS $_$SELECT translate_dna($1,1)$_$;

CREATE FUNCTION translate_dna(text, bigint) RETURNS text
    LANGUAGE plpgsql
    AS $_$
 DECLARE
  dnaseq ALIAS FOR $1;
  gcode ALIAS FOR $2;
  translation TEXT;
  dnaseqlen BIGINT;
  codon CHAR(3);
  aa CHAR(1);
  i INT;
 BEGIN
   translation = '';
   dnaseqlen = char_length(dnaseq);
   i=1;
   WHILE i+1 < dnaseqlen loop
     codon = substring(dnaseq,i,3);
     aa = translate_codon(codon,gcode);
     translation = translation || aa;
     i = i+3;
   END loop;
 RETURN translation;
END$_$;

CREATE AGGREGATE concat(text) (
    SFUNC = concat_pair,
    STYPE = text,
    INITCOND = ''
);

CREATE TABLE acquisition (
    acquisition_id bigint NOT NULL,
    assay_id bigint NOT NULL,
    protocol_id bigint,
    channel_id bigint,
    acquisitiondate timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    name text,
    uri text
);

COMMENT ON TABLE acquisition IS 'This represents the scanning of hybridized material. The output of this process is typically a digital image of an array.';

CREATE SEQUENCE acquisition_acquisition_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE acquisition_acquisition_id_seq OWNED BY acquisition.acquisition_id;

CREATE TABLE acquisition_relationship (
    acquisition_relationship_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    type_id bigint NOT NULL,
    object_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE acquisition_relationship IS 'Multiple monochrome images may be merged to form a multi-color image. Red-green images of 2-channel hybridizations are an example of this.';

CREATE SEQUENCE acquisition_relationship_acquisition_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE acquisition_relationship_acquisition_relationship_id_seq OWNED BY acquisition_relationship.acquisition_relationship_id;

CREATE TABLE acquisitionprop (
    acquisitionprop_id bigint NOT NULL,
    acquisition_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE acquisitionprop IS 'Parameters associated with image acquisition.';

CREATE SEQUENCE acquisitionprop_acquisitionprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE acquisitionprop_acquisitionprop_id_seq OWNED BY acquisitionprop.acquisitionprop_id;

CREATE VIEW all_feature_names AS
 SELECT feature.feature_id,
    ("substring"(feature.uniquename, 0, 255))::character varying(255) AS name,
    feature.organism_id
   FROM feature
UNION
 SELECT feature.feature_id,
    feature.name,
    feature.organism_id
   FROM feature
  WHERE (feature.name IS NOT NULL)
UNION
 SELECT fs.feature_id,
    s.name,
    f.organism_id
   FROM feature_synonym fs,
    synonym s,
    feature f
  WHERE ((fs.synonym_id = s.synonym_id) AND (fs.feature_id = f.feature_id))
UNION
 SELECT fp.feature_id,
    ("substring"(fp.value, 0, 255))::character varying(255) AS name,
    f.organism_id
   FROM featureprop fp,
    feature f
  WHERE (f.feature_id = fp.feature_id)
UNION
 SELECT fd.feature_id,
    d.accession AS name,
    f.organism_id
   FROM feature_dbxref fd,
    dbxref d,
    feature f
  WHERE ((fd.dbxref_id = d.dbxref_id) AND (fd.feature_id = f.feature_id));

CREATE TABLE analysis (
    analysis_id bigint NOT NULL,
    name character varying(255),
    description text,
    program character varying(255) NOT NULL,
    programversion character varying(255) NOT NULL,
    algorithm character varying(255),
    sourcename character varying(255),
    sourceversion character varying(255),
    sourceuri text,
    timeexecuted timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
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

CREATE SEQUENCE analysis_analysis_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE analysis_analysis_id_seq OWNED BY analysis.analysis_id;

CREATE TABLE analysis_cvterm (
    analysis_cvterm_id bigint NOT NULL,
    analysis_id bigint NOT NULL,
    cvterm_id bigint NOT NULL,
    is_not boolean DEFAULT false NOT NULL,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE analysis_cvterm IS 'Associate a term from a cv with an analysis.';

COMMENT ON COLUMN analysis_cvterm.is_not IS 'If this is set to true, then this
annotation is interpreted as a NEGATIVE annotation - i.e. the analysis does
NOT have the specified term.';

CREATE SEQUENCE analysis_cvterm_analysis_cvterm_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE analysis_cvterm_analysis_cvterm_id_seq OWNED BY analysis_cvterm.analysis_cvterm_id;

CREATE TABLE analysis_dbxref (
    analysis_dbxref_id bigint NOT NULL,
    analysis_id bigint NOT NULL,
    dbxref_id bigint NOT NULL,
    is_current boolean DEFAULT true NOT NULL
);

COMMENT ON TABLE analysis_dbxref IS 'Links an analysis to dbxrefs.';

COMMENT ON COLUMN analysis_dbxref.is_current IS 'True if this dbxref
is the most up to date accession in the corresponding db. Retired
accessions should set this field to false';

CREATE SEQUENCE analysis_dbxref_analysis_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE analysis_dbxref_analysis_dbxref_id_seq OWNED BY analysis_dbxref.analysis_dbxref_id;

CREATE TABLE analysis_pub (
    analysis_pub_id bigint NOT NULL,
    analysis_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE analysis_pub IS 'Provenance. Linking table between analyses and the publications that mention them.';

CREATE SEQUENCE analysis_pub_analysis_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE analysis_pub_analysis_pub_id_seq OWNED BY analysis_pub.analysis_pub_id;

CREATE TABLE analysis_relationship (
    analysis_relationship_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    object_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON COLUMN analysis_relationship.subject_id IS 'analysis_relationship.subject_id i
s the subject of the subj-predicate-obj sentence.';

COMMENT ON COLUMN analysis_relationship.object_id IS 'analysis_relationship.object_id
is the object of the subj-predicate-obj sentence.';

COMMENT ON COLUMN analysis_relationship.type_id IS 'analysis_relationship.type_id
is relationship type between subject and object. This is a cvterm, typically
from the OBO relationship ontology, although other relationship types are allowed.';

COMMENT ON COLUMN analysis_relationship.value IS 'analysis_relationship.value
is for additional notes or comments.';

COMMENT ON COLUMN analysis_relationship.rank IS 'analysis_relationship.rank is
the ordering of subject analysiss with respect to the object analysis may be
important where rank is used to order these; starts from zero.';

CREATE SEQUENCE analysis_relationship_analysis_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE analysis_relationship_analysis_relationship_id_seq OWNED BY analysis_relationship.analysis_relationship_id;

CREATE TABLE analysisfeature (
    analysisfeature_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    analysis_id bigint NOT NULL,
    rawscore double precision,
    normscore double precision,
    significance double precision,
    identity double precision
);

COMMENT ON TABLE analysisfeature IS 'Computational analyses generate features (e.g. Genscan generates transcripts and exons; sim4 alignments generate similarity/match features). analysisfeatures are stored using the feature table from the sequence module. The analysisfeature table is used to decorate these features, with analysis specific attributes. A feature is an analysisfeature if and only if there is a corresponding entry in the analysisfeature table. analysisfeatures will have two or more featureloc entries,
 with rank indicating query/subject';

COMMENT ON COLUMN analysisfeature.rawscore IS 'This is the native score generated by the program; for example, the bitscore generated by blast, sim4 or genscan scores. One should not assume that high is necessarily better than low.';

COMMENT ON COLUMN analysisfeature.normscore IS 'This is the rawscore but
    semi-normalized. Complete normalization to allow comparison of
    features generated by different programs would be nice but too
    difficult. Instead the normalization should strive to enforce the
    following semantics: * normscores are floating point numbers >= 0,
    * high normscores are better than low one. For most programs, it would be sufficient to make the normscore the same as this rawscore, providing these semantics are satisfied.';

COMMENT ON COLUMN analysisfeature.significance IS 'This is some kind of expectation or probability metric, representing the probability that the analysis would appear randomly given the model. As such, any program or person querying this table can assume the following semantics:
   * 0 <= significance <= n, where n is a positive number, theoretically unbounded but unlikely to be more than 10
  * low numbers are better than high numbers.';

COMMENT ON COLUMN analysisfeature.identity IS 'Percent identity between the locations compared.  Note that these 4 metrics do not cover the full range of scores possible; it would be undesirable to list every score possible, as this should be kept extensible. instead, for non-standard scores, use the analysisprop table.';

CREATE SEQUENCE analysisfeature_analysisfeature_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE analysisfeature_analysisfeature_id_seq OWNED BY analysisfeature.analysisfeature_id;

CREATE TABLE analysisfeatureprop (
    analysisfeatureprop_id bigint NOT NULL,
    analysisfeature_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer NOT NULL
);

CREATE SEQUENCE analysisfeatureprop_analysisfeatureprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE analysisfeatureprop_analysisfeatureprop_id_seq OWNED BY analysisfeatureprop.analysisfeatureprop_id;

CREATE TABLE analysisprop (
    analysisprop_id bigint NOT NULL,
    analysis_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

CREATE SEQUENCE analysisprop_analysisprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE analysisprop_analysisprop_id_seq OWNED BY analysisprop.analysisprop_id;

CREATE TABLE arraydesign (
    arraydesign_id bigint NOT NULL,
    manufacturer_id bigint NOT NULL,
    platformtype_id bigint NOT NULL,
    substratetype_id bigint,
    protocol_id bigint,
    dbxref_id bigint,
    name text NOT NULL,
    version text,
    description text,
    array_dimensions text,
    element_dimensions text,
    num_of_elements integer,
    num_array_columns integer,
    num_array_rows integer,
    num_grid_columns integer,
    num_grid_rows integer,
    num_sub_columns integer,
    num_sub_rows integer
);

COMMENT ON TABLE arraydesign IS 'General properties about an array.
An array is a template used to generate physical slides, etc.  It
contains layout information, as well as global array properties, such
as material (glass, nylon) and spot dimensions (in rows/columns).';

CREATE SEQUENCE arraydesign_arraydesign_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE arraydesign_arraydesign_id_seq OWNED BY arraydesign.arraydesign_id;

CREATE TABLE arraydesignprop (
    arraydesignprop_id bigint NOT NULL,
    arraydesign_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE arraydesignprop IS 'Extra array design properties that are not accounted for in arraydesign.';

CREATE SEQUENCE arraydesignprop_arraydesignprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE arraydesignprop_arraydesignprop_id_seq OWNED BY arraydesignprop.arraydesignprop_id;

CREATE TABLE assay (
    assay_id bigint NOT NULL,
    arraydesign_id bigint NOT NULL,
    protocol_id bigint,
    assaydate timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    arrayidentifier text,
    arraybatchidentifier text,
    operator_id bigint NOT NULL,
    dbxref_id bigint,
    name text,
    description text
);

COMMENT ON TABLE assay IS 'An assay consists of a physical instance of
an array, combined with the conditions used to create the array
(protocols, technician information). The assay can be thought of as a hybridization.';

CREATE SEQUENCE assay_assay_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE assay_assay_id_seq OWNED BY assay.assay_id;

CREATE TABLE assay_biomaterial (
    assay_biomaterial_id bigint NOT NULL,
    assay_id bigint NOT NULL,
    biomaterial_id bigint NOT NULL,
    channel_id bigint,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE assay_biomaterial IS 'A biomaterial can be hybridized many times (technical replicates), or combined with other biomaterials in a single hybridization (for two-channel arrays).';

CREATE SEQUENCE assay_biomaterial_assay_biomaterial_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE assay_biomaterial_assay_biomaterial_id_seq OWNED BY assay_biomaterial.assay_biomaterial_id;

CREATE TABLE assay_project (
    assay_project_id bigint NOT NULL,
    assay_id bigint NOT NULL,
    project_id bigint NOT NULL
);

COMMENT ON TABLE assay_project IS 'Link assays to projects.';

CREATE SEQUENCE assay_project_assay_project_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE assay_project_assay_project_id_seq OWNED BY assay_project.assay_project_id;

CREATE TABLE assayprop (
    assayprop_id bigint NOT NULL,
    assay_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE assayprop IS 'Extra assay properties that are not accounted for in assay.';

CREATE SEQUENCE assayprop_assayprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE assayprop_assayprop_id_seq OWNED BY assayprop.assayprop_id;

CREATE TABLE biomaterial (
    biomaterial_id bigint NOT NULL,
    taxon_id bigint,
    biosourceprovider_id bigint,
    dbxref_id bigint,
    name text,
    description text
);

COMMENT ON TABLE biomaterial IS 'A biomaterial represents the MAGE concept of BioSource, BioSample, and LabeledExtract. It is essentially some biological material (tissue, cells, serum) that may have been processed. Processed biomaterials should be traceable back to raw biomaterials via the biomaterialrelationship table.';

CREATE SEQUENCE biomaterial_biomaterial_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE biomaterial_biomaterial_id_seq OWNED BY biomaterial.biomaterial_id;

CREATE TABLE biomaterial_dbxref (
    biomaterial_dbxref_id bigint NOT NULL,
    biomaterial_id bigint NOT NULL,
    dbxref_id bigint NOT NULL
);

CREATE SEQUENCE biomaterial_dbxref_biomaterial_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE biomaterial_dbxref_biomaterial_dbxref_id_seq OWNED BY biomaterial_dbxref.biomaterial_dbxref_id;

CREATE TABLE biomaterial_relationship (
    biomaterial_relationship_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    type_id bigint NOT NULL,
    object_id bigint NOT NULL
);

COMMENT ON TABLE biomaterial_relationship IS 'Relate biomaterials to one another. This is a way to track a series of treatments or material splits/merges, for instance.';

CREATE SEQUENCE biomaterial_relationship_biomaterial_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE biomaterial_relationship_biomaterial_relationship_id_seq OWNED BY biomaterial_relationship.biomaterial_relationship_id;

CREATE TABLE biomaterial_treatment (
    biomaterial_treatment_id bigint NOT NULL,
    biomaterial_id bigint NOT NULL,
    treatment_id bigint NOT NULL,
    unittype_id bigint,
    value real,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE biomaterial_treatment IS 'Link biomaterials to treatments. Treatments have an order of operations (rank), and associated measurements (unittype_id, value).';

CREATE SEQUENCE biomaterial_treatment_biomaterial_treatment_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE biomaterial_treatment_biomaterial_treatment_id_seq OWNED BY biomaterial_treatment.biomaterial_treatment_id;

CREATE TABLE biomaterialprop (
    biomaterialprop_id bigint NOT NULL,
    biomaterial_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE biomaterialprop IS 'Extra biomaterial properties that are not accounted for in biomaterial.';

CREATE SEQUENCE biomaterialprop_biomaterialprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE biomaterialprop_biomaterialprop_id_seq OWNED BY biomaterialprop.biomaterialprop_id;

CREATE TABLE cell_line (
    cell_line_id bigint NOT NULL,
    name character varying(255),
    uniquename character varying(255) NOT NULL,
    organism_id bigint NOT NULL,
    timeaccessioned timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    timelastmodified timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);

CREATE SEQUENCE cell_line_cell_line_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cell_line_cell_line_id_seq OWNED BY cell_line.cell_line_id;

CREATE TABLE cell_line_cvterm (
    cell_line_cvterm_id bigint NOT NULL,
    cell_line_id bigint NOT NULL,
    cvterm_id bigint NOT NULL,
    pub_id bigint NOT NULL,
    rank integer DEFAULT 0 NOT NULL
);

CREATE SEQUENCE cell_line_cvterm_cell_line_cvterm_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cell_line_cvterm_cell_line_cvterm_id_seq OWNED BY cell_line_cvterm.cell_line_cvterm_id;

CREATE TABLE cell_line_cvtermprop (
    cell_line_cvtermprop_id bigint NOT NULL,
    cell_line_cvterm_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

CREATE SEQUENCE cell_line_cvtermprop_cell_line_cvtermprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cell_line_cvtermprop_cell_line_cvtermprop_id_seq OWNED BY cell_line_cvtermprop.cell_line_cvtermprop_id;

CREATE TABLE cell_line_dbxref (
    cell_line_dbxref_id bigint NOT NULL,
    cell_line_id bigint NOT NULL,
    dbxref_id bigint NOT NULL,
    is_current boolean DEFAULT true NOT NULL
);

CREATE SEQUENCE cell_line_dbxref_cell_line_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cell_line_dbxref_cell_line_dbxref_id_seq OWNED BY cell_line_dbxref.cell_line_dbxref_id;

CREATE TABLE cell_line_feature (
    cell_line_feature_id bigint NOT NULL,
    cell_line_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

CREATE SEQUENCE cell_line_feature_cell_line_feature_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cell_line_feature_cell_line_feature_id_seq OWNED BY cell_line_feature.cell_line_feature_id;

CREATE TABLE cell_line_library (
    cell_line_library_id bigint NOT NULL,
    cell_line_id bigint NOT NULL,
    library_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

CREATE SEQUENCE cell_line_library_cell_line_library_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cell_line_library_cell_line_library_id_seq OWNED BY cell_line_library.cell_line_library_id;

CREATE TABLE cell_line_pub (
    cell_line_pub_id bigint NOT NULL,
    cell_line_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

CREATE SEQUENCE cell_line_pub_cell_line_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cell_line_pub_cell_line_pub_id_seq OWNED BY cell_line_pub.cell_line_pub_id;

CREATE TABLE cell_line_relationship (
    cell_line_relationship_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    object_id bigint NOT NULL,
    type_id bigint NOT NULL
);

CREATE SEQUENCE cell_line_relationship_cell_line_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cell_line_relationship_cell_line_relationship_id_seq OWNED BY cell_line_relationship.cell_line_relationship_id;

CREATE TABLE cell_line_synonym (
    cell_line_synonym_id bigint NOT NULL,
    cell_line_id bigint NOT NULL,
    synonym_id bigint NOT NULL,
    pub_id bigint NOT NULL,
    is_current boolean DEFAULT false NOT NULL,
    is_internal boolean DEFAULT false NOT NULL
);

CREATE SEQUENCE cell_line_synonym_cell_line_synonym_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cell_line_synonym_cell_line_synonym_id_seq OWNED BY cell_line_synonym.cell_line_synonym_id;

CREATE TABLE cell_lineprop (
    cell_lineprop_id bigint NOT NULL,
    cell_line_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

CREATE SEQUENCE cell_lineprop_cell_lineprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cell_lineprop_cell_lineprop_id_seq OWNED BY cell_lineprop.cell_lineprop_id;

CREATE TABLE cell_lineprop_pub (
    cell_lineprop_pub_id bigint NOT NULL,
    cell_lineprop_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

CREATE SEQUENCE cell_lineprop_pub_cell_lineprop_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cell_lineprop_pub_cell_lineprop_pub_id_seq OWNED BY cell_lineprop_pub.cell_lineprop_pub_id;

CREATE TABLE chadoprop (
    chadoprop_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE chadoprop IS 'This table is different from other prop tables in the database, as it is for storing information about the database itself, like schema version';

COMMENT ON COLUMN chadoprop.type_id IS 'The name of the property or slot is a cvterm. The meaning of the property is defined in that cvterm.';

COMMENT ON COLUMN chadoprop.value IS 'The value of the property, represented as text. Numeric values are converted to their text representation.';

COMMENT ON COLUMN chadoprop.rank IS 'Property-Value ordering. Any
cv can have multiple values for any particular property type -
these are ordered in a list using rank, counting from zero. For
properties that are single-valued rather than multi-valued, the
default 0 value should be used.';

CREATE SEQUENCE chadoprop_chadoprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE chadoprop_chadoprop_id_seq OWNED BY chadoprop.chadoprop_id;

CREATE TABLE channel (
    channel_id bigint NOT NULL,
    name text NOT NULL,
    definition text NOT NULL
);

COMMENT ON TABLE channel IS 'Different array platforms can record signals from one or more channels (cDNA arrays typically use two CCD, but Affymetrix uses only one).';

CREATE SEQUENCE channel_channel_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE channel_channel_id_seq OWNED BY channel.channel_id;

CREATE VIEW common_ancestor_cvterm AS
 SELECT p1.subject_id AS cvterm1_id,
    p2.subject_id AS cvterm2_id,
    p1.object_id AS ancestor_cvterm_id,
    p1.pathdistance AS pathdistance1,
    p2.pathdistance AS pathdistance2,
    (p1.pathdistance + p2.pathdistance) AS total_pathdistance
   FROM cvtermpath p1,
    cvtermpath p2
  WHERE (p1.object_id = p2.object_id);

COMMENT ON VIEW common_ancestor_cvterm IS 'The common ancestor of any
two terms is the intersection of both terms ancestors. Two terms can
have multiple common ancestors. Use total_pathdistance to get the
least common ancestor';

CREATE VIEW common_descendant_cvterm AS
 SELECT p1.object_id AS cvterm1_id,
    p2.object_id AS cvterm2_id,
    p1.subject_id AS ancestor_cvterm_id,
    p1.pathdistance AS pathdistance1,
    p2.pathdistance AS pathdistance2,
    (p1.pathdistance + p2.pathdistance) AS total_pathdistance
   FROM cvtermpath p1,
    cvtermpath p2
  WHERE (p1.subject_id = p2.subject_id);

COMMENT ON VIEW common_descendant_cvterm IS 'The common descendant of
any two terms is the intersection of both terms descendants. Two terms
can have multiple common descendants. Use total_pathdistance to get
the least common ancestor';

CREATE TABLE contact (
    contact_id bigint NOT NULL,
    type_id bigint,
    name character varying(255) NOT NULL,
    description character varying(255)
);

COMMENT ON TABLE contact IS 'Model persons, institutes, groups, organizations, etc.';

COMMENT ON COLUMN contact.type_id IS 'What type of contact is this?  E.g. "person", "lab".';

CREATE SEQUENCE contact_contact_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE contact_contact_id_seq OWNED BY contact.contact_id;

CREATE TABLE contact_relationship (
    contact_relationship_id bigint NOT NULL,
    type_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    object_id bigint NOT NULL
);

COMMENT ON TABLE contact_relationship IS 'Model relationships between contacts';

COMMENT ON COLUMN contact_relationship.type_id IS 'Relationship type between subject and object. This is a cvterm, typically from the OBO relationship ontology, although other relationship types are allowed.';

COMMENT ON COLUMN contact_relationship.subject_id IS 'The subject of the subj-predicate-obj sentence. In a DAG, this corresponds to the child node.';

COMMENT ON COLUMN contact_relationship.object_id IS 'The object of the subj-predicate-obj sentence. In a DAG, this corresponds to the parent node.';

CREATE SEQUENCE contact_relationship_contact_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE contact_relationship_contact_relationship_id_seq OWNED BY contact_relationship.contact_relationship_id;

CREATE TABLE contactprop (
    contactprop_id bigint NOT NULL,
    contact_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE contactprop IS 'A contact can have any number of slot-value property
tags attached to it. This is an alternative to hardcoding a list of columns in the
relational schema, and is completely extensible.';

CREATE SEQUENCE contactprop_contactprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE contactprop_contactprop_id_seq OWNED BY contactprop.contactprop_id;

CREATE TABLE control (
    control_id bigint NOT NULL,
    type_id bigint NOT NULL,
    assay_id bigint NOT NULL,
    tableinfo_id bigint NOT NULL,
    row_id integer NOT NULL,
    name text,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

CREATE SEQUENCE control_control_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE control_control_id_seq OWNED BY control.control_id;

CREATE TABLE cv (
    cv_id bigint NOT NULL,
    name character varying(255) NOT NULL,
    definition text
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

CREATE SEQUENCE cv_cv_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cv_cv_id_seq OWNED BY cv.cv_id;

CREATE VIEW cv_cvterm_count AS
 SELECT cv.name,
    count(*) AS num_terms_excl_obs
   FROM (cv
     JOIN cvterm USING (cv_id))
  WHERE (cvterm.is_obsolete = 0)
  GROUP BY cv.name;

COMMENT ON VIEW cv_cvterm_count IS 'per-cv terms counts (excludes obsoletes)';

CREATE VIEW cv_cvterm_count_with_obs AS
 SELECT cv.name,
    count(*) AS num_terms_incl_obs
   FROM (cv
     JOIN cvterm USING (cv_id))
  GROUP BY cv.name;

COMMENT ON VIEW cv_cvterm_count_with_obs IS 'per-cv terms counts (includes obsoletes)';

CREATE TABLE cvterm_relationship (
    cvterm_relationship_id bigint NOT NULL,
    type_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    object_id bigint NOT NULL
);

COMMENT ON TABLE cvterm_relationship IS 'A relationship linking two
cvterms. Each cvterm_relationship constitutes an edge in the graph
defined by the collection of cvterms and cvterm_relationships. The
meaning of the cvterm_relationship depends on the definition of the
cvterm R refered to by type_id. However, in general the definitions
are such that the statement "all SUBJs REL some OBJ" is true. The
cvterm_relationship statement is about the subject, not the
object. For example "insect wing part_of thorax".';

COMMENT ON COLUMN cvterm_relationship.type_id IS 'The nature of the
relationship between subject and object. Note that relations are also
housed in the cvterm table, typically from the OBO relationship
ontology, although other relationship types are allowed.';

COMMENT ON COLUMN cvterm_relationship.subject_id IS 'The subject of
the subj-predicate-obj sentence. The cvterm_relationship is about the
subject. In a graph, this typically corresponds to the child node.';

COMMENT ON COLUMN cvterm_relationship.object_id IS 'The object of the
subj-predicate-obj sentence. The cvterm_relationship refers to the
object. In a graph, this typically corresponds to the parent node.';

CREATE VIEW cv_leaf AS
 SELECT cvterm.cv_id,
    cvterm.cvterm_id
   FROM cvterm
  WHERE (NOT (cvterm.cvterm_id IN ( SELECT cvterm_relationship.object_id
           FROM cvterm_relationship)));

COMMENT ON VIEW cv_leaf IS 'the leaves of a cv are the set of terms
which have no children (terms that are not the object of a
relation). All cvs will have at least 1 leaf';

CREATE VIEW cv_link_count AS
 SELECT cv.name AS cv_name,
    relation.name AS relation_name,
    relation_cv.name AS relation_cv_name,
    count(*) AS num_links
   FROM ((((cv
     JOIN cvterm ON ((cvterm.cv_id = cv.cv_id)))
     JOIN cvterm_relationship ON ((cvterm.cvterm_id = cvterm_relationship.subject_id)))
     JOIN cvterm relation ON ((cvterm_relationship.type_id = relation.cvterm_id)))
     JOIN cv relation_cv ON ((relation.cv_id = relation_cv.cv_id)))
  GROUP BY cv.name, relation.name, relation_cv.name;

COMMENT ON VIEW cv_link_count IS 'per-cv summary of number of
links (cvterm_relationships) broken down by
relationship_type. num_links is the total # of links of the specified
type in which the subject_id of the link is in the named cv';

CREATE VIEW cv_path_count AS
 SELECT cv.name AS cv_name,
    relation.name AS relation_name,
    relation_cv.name AS relation_cv_name,
    count(*) AS num_paths
   FROM ((((cv
     JOIN cvterm ON ((cvterm.cv_id = cv.cv_id)))
     JOIN cvtermpath ON ((cvterm.cvterm_id = cvtermpath.subject_id)))
     JOIN cvterm relation ON ((cvtermpath.type_id = relation.cvterm_id)))
     JOIN cv relation_cv ON ((relation.cv_id = relation_cv.cv_id)))
  GROUP BY cv.name, relation.name, relation_cv.name;

COMMENT ON VIEW cv_path_count IS 'per-cv summary of number of
paths (cvtermpaths) broken down by relationship_type. num_paths is the
total # of paths of the specified type in which the subject_id of the
path is in the named cv. See also: cv_distinct_relations';

CREATE VIEW cv_root AS
 SELECT cvterm.cv_id,
    cvterm.cvterm_id AS root_cvterm_id
   FROM cvterm
  WHERE ((NOT (cvterm.cvterm_id IN ( SELECT cvterm_relationship.subject_id
           FROM cvterm_relationship))) AND (cvterm.is_obsolete = 0));

COMMENT ON VIEW cv_root IS 'the roots of a cv are the set of terms
which have no parents (terms that are not the subject of a
relation). Most cvs will have a single root, some may have >1. All
will have at least 1';

CREATE TABLE cvprop (
    cvprop_id bigint NOT NULL,
    cv_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE cvprop IS 'Additional extensible properties can be attached to a cv using this table.  A notable example would be the cv version';

COMMENT ON COLUMN cvprop.type_id IS 'The name of the property or slot is a cvterm. The meaning of the property is defined in that cvterm.';

COMMENT ON COLUMN cvprop.value IS 'The value of the property, represented as text. Numeric values are converted to their text representation.';

COMMENT ON COLUMN cvprop.rank IS 'Property-Value ordering. Any
cv can have multiple values for any particular property type -
these are ordered in a list using rank, counting from zero. For
properties that are single-valued rather than multi-valued, the
default 0 value should be used.';

CREATE SEQUENCE cvprop_cvprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cvprop_cvprop_id_seq OWNED BY cvprop.cvprop_id;

CREATE SEQUENCE cvterm_cvterm_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cvterm_cvterm_id_seq OWNED BY cvterm.cvterm_id;

CREATE TABLE cvterm_dbxref (
    cvterm_dbxref_id bigint NOT NULL,
    cvterm_id bigint NOT NULL,
    dbxref_id bigint NOT NULL,
    is_for_definition integer DEFAULT 0 NOT NULL
);

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

CREATE SEQUENCE cvterm_dbxref_cvterm_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cvterm_dbxref_cvterm_dbxref_id_seq OWNED BY cvterm_dbxref.cvterm_dbxref_id;

CREATE SEQUENCE cvterm_relationship_cvterm_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cvterm_relationship_cvterm_relationship_id_seq OWNED BY cvterm_relationship.cvterm_relationship_id;

CREATE SEQUENCE cvtermpath_cvtermpath_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cvtermpath_cvtermpath_id_seq OWNED BY cvtermpath.cvtermpath_id;

CREATE TABLE cvtermprop (
    cvtermprop_id bigint NOT NULL,
    cvterm_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text DEFAULT ''::text NOT NULL,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE cvtermprop IS 'Additional extensible properties can be attached to a cvterm using this table. Corresponds to -AnnotationProperty- in W3C OWL format.';

COMMENT ON COLUMN cvtermprop.type_id IS 'The name of the property or slot is a cvterm. The meaning of the property is defined in that cvterm.';

COMMENT ON COLUMN cvtermprop.value IS 'The value of the property, represented as text. Numeric values are converted to their text representation.';

COMMENT ON COLUMN cvtermprop.rank IS 'Property-Value ordering. Any
cvterm can have multiple values for any particular property type -
these are ordered in a list using rank, counting from zero. For
properties that are single-valued rather than multi-valued, the
default 0 value should be used.';

CREATE SEQUENCE cvtermprop_cvtermprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cvtermprop_cvtermprop_id_seq OWNED BY cvtermprop.cvtermprop_id;

CREATE TABLE cvtermsynonym (
    cvtermsynonym_id bigint NOT NULL,
    cvterm_id bigint NOT NULL,
    synonym character varying(1024) NOT NULL,
    type_id bigint
);

COMMENT ON TABLE cvtermsynonym IS 'A cvterm actually represents a
distinct class or concept. A concept can be refered to by different
phrases or names. In addition to the primary name (cvterm.name) there
can be a number of alternative aliases or synonyms. For example, "T
cell" as a synonym for "T lymphocyte".';

COMMENT ON COLUMN cvtermsynonym.type_id IS 'A synonym can be exact,
narrower, or broader than.';

CREATE SEQUENCE cvtermsynonym_cvtermsynonym_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE cvtermsynonym_cvtermsynonym_id_seq OWNED BY cvtermsynonym.cvtermsynonym_id;

CREATE SEQUENCE db_db_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE db_db_id_seq OWNED BY db.db_id;

CREATE VIEW db_dbxref_count AS
 SELECT db.name,
    count(*) AS num_dbxrefs
   FROM (db
     JOIN dbxref USING (db_id))
  GROUP BY db.name;

COMMENT ON VIEW db_dbxref_count IS 'per-db dbxref counts';

CREATE TABLE dbprop (
    dbprop_id bigint NOT NULL,
    db_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE dbprop IS 'An external database can have any number of
slot-value property tags attached to it. This is an alternative to
hardcoding a list of columns in the relational schema, and is
completely extensible. There is a unique constraint, dbprop_c1, for
the combination of db_id, rank, and type_id. Multivalued property-value pairs must be differentiated by rank.';

CREATE SEQUENCE dbprop_dbprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE dbprop_dbprop_id_seq OWNED BY dbprop.dbprop_id;

CREATE SEQUENCE dbxref_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE dbxref_dbxref_id_seq OWNED BY dbxref.dbxref_id;

CREATE TABLE dbxrefprop (
    dbxrefprop_id bigint NOT NULL,
    dbxref_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text DEFAULT ''::text NOT NULL,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE dbxrefprop IS 'Metadata about a dbxref. Note that this is not defined in the dbxref module, as it depends on the cvterm table. This table has a structure analagous to cvtermprop.';

CREATE SEQUENCE dbxrefprop_dbxrefprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE dbxrefprop_dbxrefprop_id_seq OWNED BY dbxrefprop.dbxrefprop_id;

CREATE VIEW dfeatureloc AS
 SELECT featureloc.featureloc_id,
    featureloc.feature_id,
    featureloc.srcfeature_id,
    featureloc.fmin AS nbeg,
    featureloc.is_fmin_partial AS is_nbeg_partial,
    featureloc.fmax AS nend,
    featureloc.is_fmax_partial AS is_nend_partial,
    featureloc.strand,
    featureloc.phase,
    featureloc.residue_info,
    featureloc.locgroup,
    featureloc.rank
   FROM featureloc
  WHERE ((featureloc.strand < 0) OR (featureloc.phase < 0))
UNION
 SELECT featureloc.featureloc_id,
    featureloc.feature_id,
    featureloc.srcfeature_id,
    featureloc.fmax AS nbeg,
    featureloc.is_fmax_partial AS is_nbeg_partial,
    featureloc.fmin AS nend,
    featureloc.is_fmin_partial AS is_nend_partial,
    featureloc.strand,
    featureloc.phase,
    featureloc.residue_info,
    featureloc.locgroup,
    featureloc.rank
   FROM featureloc
  WHERE ((featureloc.strand IS NULL) OR (featureloc.strand >= 0) OR (featureloc.phase >= 0));

CREATE TABLE eimage (
    eimage_id bigint NOT NULL,
    eimage_data text,
    eimage_type character varying(255) NOT NULL,
    image_uri character varying(255)
);

COMMENT ON COLUMN eimage.eimage_data IS 'We expect images in eimage_data (e.g. JPEGs) to be uuencoded.';

COMMENT ON COLUMN eimage.eimage_type IS 'Describes the type of data in eimage_data.';

CREATE SEQUENCE eimage_eimage_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE eimage_eimage_id_seq OWNED BY eimage.eimage_id;

CREATE TABLE element (
    element_id bigint NOT NULL,
    feature_id bigint,
    arraydesign_id bigint NOT NULL,
    type_id bigint,
    dbxref_id bigint
);

COMMENT ON TABLE element IS 'Represents a feature of the array. This is typically a region of the array coated or bound to DNA.';

CREATE SEQUENCE element_element_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE element_element_id_seq OWNED BY element.element_id;

CREATE TABLE element_relationship (
    element_relationship_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    type_id bigint NOT NULL,
    object_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE element_relationship IS 'Sometimes we want to combine measurements from multiple elements to get a composite value. Affymetrix combines many probes to form a probeset measurement, for instance.';

CREATE SEQUENCE element_relationship_element_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE element_relationship_element_relationship_id_seq OWNED BY element_relationship.element_relationship_id;

CREATE TABLE elementresult (
    elementresult_id bigint NOT NULL,
    element_id bigint NOT NULL,
    quantification_id bigint NOT NULL,
    signal double precision NOT NULL
);

COMMENT ON TABLE elementresult IS 'An element on an array produces a measurement when hybridized to a biomaterial (traceable through quantification_id). This is the base data from which tables that actually contain data inherit.';

CREATE SEQUENCE elementresult_elementresult_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE elementresult_elementresult_id_seq OWNED BY elementresult.elementresult_id;

CREATE TABLE elementresult_relationship (
    elementresult_relationship_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    type_id bigint NOT NULL,
    object_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE elementresult_relationship IS 'Sometimes we want to combine measurements from multiple elements to get a composite value. Affymetrix combines many probes to form a probeset measurement, for instance.';

CREATE SEQUENCE elementresult_relationship_elementresult_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE elementresult_relationship_elementresult_relationship_id_seq OWNED BY elementresult_relationship.elementresult_relationship_id;

CREATE TABLE environment (
    environment_id bigint NOT NULL,
    uniquename text NOT NULL,
    description text
);

COMMENT ON TABLE environment IS 'The environmental component of a phenotype description.';

CREATE TABLE environment_cvterm (
    environment_cvterm_id bigint NOT NULL,
    environment_id bigint NOT NULL,
    cvterm_id bigint NOT NULL
);

CREATE SEQUENCE environment_cvterm_environment_cvterm_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE environment_cvterm_environment_cvterm_id_seq OWNED BY environment_cvterm.environment_cvterm_id;

CREATE SEQUENCE environment_environment_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE environment_environment_id_seq OWNED BY environment.environment_id;

CREATE TABLE expression (
    expression_id bigint NOT NULL,
    uniquename text NOT NULL,
    md5checksum character(32),
    description text
);

COMMENT ON TABLE expression IS 'The expression table is essentially a bridge table.';

CREATE TABLE expression_cvterm (
    expression_cvterm_id bigint NOT NULL,
    expression_id bigint NOT NULL,
    cvterm_id bigint NOT NULL,
    rank integer DEFAULT 0 NOT NULL,
    cvterm_type_id bigint NOT NULL
);

CREATE SEQUENCE expression_cvterm_expression_cvterm_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE expression_cvterm_expression_cvterm_id_seq OWNED BY expression_cvterm.expression_cvterm_id;

CREATE TABLE expression_cvtermprop (
    expression_cvtermprop_id bigint NOT NULL,
    expression_cvterm_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

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

CREATE SEQUENCE expression_cvtermprop_expression_cvtermprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE expression_cvtermprop_expression_cvtermprop_id_seq OWNED BY expression_cvtermprop.expression_cvtermprop_id;

CREATE SEQUENCE expression_expression_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE expression_expression_id_seq OWNED BY expression.expression_id;

CREATE TABLE expression_image (
    expression_image_id bigint NOT NULL,
    expression_id bigint NOT NULL,
    eimage_id bigint NOT NULL
);

CREATE SEQUENCE expression_image_expression_image_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE expression_image_expression_image_id_seq OWNED BY expression_image.expression_image_id;

CREATE TABLE expression_pub (
    expression_pub_id bigint NOT NULL,
    expression_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

CREATE SEQUENCE expression_pub_expression_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE expression_pub_expression_pub_id_seq OWNED BY expression_pub.expression_pub_id;

CREATE TABLE expressionprop (
    expressionprop_id bigint NOT NULL,
    expression_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

CREATE SEQUENCE expressionprop_expressionprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE expressionprop_expressionprop_id_seq OWNED BY expressionprop.expressionprop_id;

CREATE VIEW f_type AS
 SELECT f.feature_id,
    f.name,
    f.dbxref_id,
    c.name AS type,
    f.residues,
    f.seqlen,
    f.md5checksum,
    f.type_id,
    f.timeaccessioned,
    f.timelastmodified
   FROM feature f,
    cvterm c
  WHERE (f.type_id = c.cvterm_id);

CREATE VIEW f_loc AS
 SELECT f.feature_id,
    f.name,
    f.dbxref_id,
    fl.nbeg,
    fl.nend,
    fl.strand
   FROM dfeatureloc fl,
    f_type f
  WHERE (f.feature_id = fl.feature_id);

CREATE TABLE feature_contact (
    feature_contact_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    contact_id bigint NOT NULL
);

COMMENT ON TABLE feature_contact IS 'Links contact(s) with a feature.  Used to indicate a particular
person or organization responsible for discovery or that can provide more information on a particular feature.';

CREATE SEQUENCE feature_contact_feature_contact_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_contact_feature_contact_id_seq OWNED BY feature_contact.feature_contact_id;

CREATE VIEW feature_contains AS
 SELECT x.feature_id AS subject_id,
    y.feature_id AS object_id
   FROM featureloc x,
    featureloc y
  WHERE ((x.srcfeature_id = y.srcfeature_id) AND ((y.fmin >= x.fmin) AND (y.fmin <= x.fmax)));

COMMENT ON VIEW feature_contains IS 'subject intervals contains (or is
same as) object interval. transitive,reflexive';

CREATE TABLE feature_cvterm_dbxref (
    feature_cvterm_dbxref_id bigint NOT NULL,
    feature_cvterm_id bigint NOT NULL,
    dbxref_id bigint NOT NULL
);

COMMENT ON TABLE feature_cvterm_dbxref IS 'Additional dbxrefs for an association. Rows in the feature_cvterm table may be backed up by dbxrefs. For example, a feature_cvterm association that was inferred via a protein-protein interaction may be backed by by refering to the dbxref for the alternate protein. Corresponds to the WITH column in a GO gene association file (but can also be used for other analagous associations). See http://www.geneontology.org/doc/GO.annotation.shtml#file for more details.';

CREATE SEQUENCE feature_cvterm_dbxref_feature_cvterm_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_cvterm_dbxref_feature_cvterm_dbxref_id_seq OWNED BY feature_cvterm_dbxref.feature_cvterm_dbxref_id;

CREATE SEQUENCE feature_cvterm_feature_cvterm_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_cvterm_feature_cvterm_id_seq OWNED BY feature_cvterm.feature_cvterm_id;

CREATE TABLE feature_cvterm_pub (
    feature_cvterm_pub_id bigint NOT NULL,
    feature_cvterm_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE feature_cvterm_pub IS 'Secondary pubs for an
association. Each feature_cvterm association is supported by a single
primary publication. Additional secondary pubs can be added using this
linking table (in a GO gene association file, these corresponding to
any IDs after the pipe symbol in the publications column.';

CREATE SEQUENCE feature_cvterm_pub_feature_cvterm_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_cvterm_pub_feature_cvterm_pub_id_seq OWNED BY feature_cvterm_pub.feature_cvterm_pub_id;

CREATE TABLE feature_cvtermprop (
    feature_cvtermprop_id bigint NOT NULL,
    feature_cvterm_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

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

CREATE SEQUENCE feature_cvtermprop_feature_cvtermprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_cvtermprop_feature_cvtermprop_id_seq OWNED BY feature_cvtermprop.feature_cvtermprop_id;

CREATE SEQUENCE feature_dbxref_feature_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_dbxref_feature_dbxref_id_seq OWNED BY feature_dbxref.feature_dbxref_id;

CREATE VIEW feature_difference AS
 SELECT x.feature_id AS subject_id,
    y.feature_id AS object_id,
    x.strand AS srcfeature_id,
    x.srcfeature_id AS fmin,
    x.fmin AS fmax,
    y.fmin AS strand
   FROM featureloc x,
    featureloc y
  WHERE ((x.srcfeature_id = y.srcfeature_id) AND ((x.fmin < y.fmin) AND (x.fmax >= y.fmax)))
UNION
 SELECT x.feature_id AS subject_id,
    y.feature_id AS object_id,
    x.strand AS srcfeature_id,
    x.srcfeature_id AS fmin,
    y.fmax,
    x.fmax AS strand
   FROM featureloc x,
    featureloc y
  WHERE ((x.srcfeature_id = y.srcfeature_id) AND ((x.fmax > y.fmax) AND (x.fmin <= y.fmin)));

COMMENT ON VIEW feature_difference IS 'size of gap between two features. must be abutting or disjoint';

CREATE VIEW feature_disjoint AS
 SELECT x.feature_id AS subject_id,
    y.feature_id AS object_id
   FROM featureloc x,
    featureloc y
  WHERE ((x.srcfeature_id = y.srcfeature_id) AND ((x.fmax < y.fmin) AND (x.fmin > y.fmax)));

COMMENT ON VIEW feature_disjoint IS 'featurelocs do not meet. symmetric';

CREATE VIEW feature_distance AS
 SELECT x.feature_id AS subject_id,
    y.feature_id AS object_id,
    x.srcfeature_id,
    x.strand AS subject_strand,
    y.strand AS object_strand,
        CASE
            WHEN (x.fmax <= y.fmin) THEN (x.fmax - y.fmin)
            ELSE (y.fmax - x.fmin)
        END AS distance
   FROM featureloc x,
    featureloc y
  WHERE ((x.srcfeature_id = y.srcfeature_id) AND ((x.fmax <= y.fmin) OR (x.fmin >= y.fmax)));

CREATE TABLE feature_expression (
    feature_expression_id bigint NOT NULL,
    expression_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

CREATE SEQUENCE feature_expression_feature_expression_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_expression_feature_expression_id_seq OWNED BY feature_expression.feature_expression_id;

CREATE TABLE feature_expressionprop (
    feature_expressionprop_id bigint NOT NULL,
    feature_expression_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE feature_expressionprop IS 'Extensible properties for
feature_expression (comments, for example). Modeled on feature_cvtermprop.';

CREATE SEQUENCE feature_expressionprop_feature_expressionprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_expressionprop_feature_expressionprop_id_seq OWNED BY feature_expressionprop.feature_expressionprop_id;

CREATE SEQUENCE feature_feature_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_feature_id_seq OWNED BY feature.feature_id;

CREATE TABLE feature_genotype (
    feature_genotype_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    genotype_id bigint NOT NULL,
    chromosome_id bigint,
    rank integer NOT NULL,
    cgroup integer NOT NULL,
    cvterm_id bigint NOT NULL
);

COMMENT ON COLUMN feature_genotype.chromosome_id IS 'A feature of SO type "chromosome".';

COMMENT ON COLUMN feature_genotype.rank IS 'rank can be used for
n-ploid organisms or to preserve order.';

COMMENT ON COLUMN feature_genotype.cgroup IS 'Spatially distinguishable
group. group can be used for distinguishing the chromosomal groups,
for example (RNAi products and so on can be treated as different
groups, as they do not fall on a particular chromosome).';

CREATE SEQUENCE feature_genotype_feature_genotype_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_genotype_feature_genotype_id_seq OWNED BY feature_genotype.feature_genotype_id;

CREATE VIEW feature_intersection AS
 SELECT x.feature_id AS subject_id,
    y.feature_id AS object_id,
    x.srcfeature_id,
    x.strand AS subject_strand,
    y.strand AS object_strand,
        CASE
            WHEN (x.fmin < y.fmin) THEN y.fmin
            ELSE x.fmin
        END AS fmin,
        CASE
            WHEN (x.fmax > y.fmax) THEN y.fmax
            ELSE x.fmax
        END AS fmax
   FROM featureloc x,
    featureloc y
  WHERE ((x.srcfeature_id = y.srcfeature_id) AND ((x.fmax >= y.fmin) AND (x.fmin <= y.fmax)));

COMMENT ON VIEW feature_intersection IS 'set-intersection on interval defined by featureloc. featurelocs must meet';

CREATE VIEW feature_meets AS
 SELECT x.feature_id AS subject_id,
    y.feature_id AS object_id
   FROM featureloc x,
    featureloc y
  WHERE ((x.srcfeature_id = y.srcfeature_id) AND ((x.fmax >= y.fmin) AND (x.fmin <= y.fmax)));

COMMENT ON VIEW feature_meets IS 'intervals have at least one
interbase point in common (ie overlap OR abut). symmetric,reflexive';

CREATE VIEW feature_meets_on_same_strand AS
 SELECT x.feature_id AS subject_id,
    y.feature_id AS object_id
   FROM featureloc x,
    featureloc y
  WHERE ((x.srcfeature_id = y.srcfeature_id) AND (x.strand = y.strand) AND ((x.fmax >= y.fmin) AND (x.fmin <= y.fmax)));

COMMENT ON VIEW feature_meets_on_same_strand IS 'as feature_meets, but
featurelocs must be on the same strand. symmetric,reflexive';

CREATE TABLE feature_phenotype (
    feature_phenotype_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    phenotype_id bigint NOT NULL
);

COMMENT ON TABLE feature_phenotype IS 'Linking table between features and phenotypes.';

CREATE SEQUENCE feature_phenotype_feature_phenotype_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_phenotype_feature_phenotype_id_seq OWNED BY feature_phenotype.feature_phenotype_id;

CREATE SEQUENCE feature_pub_feature_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_pub_feature_pub_id_seq OWNED BY feature_pub.feature_pub_id;

CREATE TABLE feature_pubprop (
    feature_pubprop_id bigint NOT NULL,
    feature_pub_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE feature_pubprop IS 'Property or attribute of a feature_pub link.';

CREATE SEQUENCE feature_pubprop_feature_pubprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_pubprop_feature_pubprop_id_seq OWNED BY feature_pubprop.feature_pubprop_id;

CREATE TABLE feature_relationship (
    feature_relationship_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    object_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

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

COMMENT ON COLUMN feature_relationship.value IS 'Additional notes or comments.';

COMMENT ON COLUMN feature_relationship.rank IS 'The ordering of subject features with respect to the object feature may be important (for example, exon ordering on a transcript - not always derivable if you take trans spliced genes into consideration). Rank is used to order these; starts from zero.';

CREATE SEQUENCE feature_relationship_feature_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_relationship_feature_relationship_id_seq OWNED BY feature_relationship.feature_relationship_id;

CREATE TABLE feature_relationship_pub (
    feature_relationship_pub_id bigint NOT NULL,
    feature_relationship_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE feature_relationship_pub IS 'Provenance. Attach optional evidence to a feature_relationship in the form of a publication.';

CREATE SEQUENCE feature_relationship_pub_feature_relationship_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_relationship_pub_feature_relationship_pub_id_seq OWNED BY feature_relationship_pub.feature_relationship_pub_id;

CREATE TABLE feature_relationshipprop (
    feature_relationshipprop_id bigint NOT NULL,
    feature_relationship_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

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

CREATE SEQUENCE feature_relationshipprop_feature_relationshipprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_relationshipprop_feature_relationshipprop_id_seq OWNED BY feature_relationshipprop.feature_relationshipprop_id;

CREATE TABLE feature_relationshipprop_pub (
    feature_relationshipprop_pub_id bigint NOT NULL,
    feature_relationshipprop_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE feature_relationshipprop_pub IS 'Provenance for feature_relationshipprop.';

CREATE SEQUENCE feature_relationshipprop_pub_feature_relationshipprop_pub_i_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_relationshipprop_pub_feature_relationshipprop_pub_i_seq OWNED BY feature_relationshipprop_pub.feature_relationshipprop_pub_id;

CREATE SEQUENCE feature_synonym_feature_synonym_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE feature_synonym_feature_synonym_id_seq OWNED BY feature_synonym.feature_synonym_id;

CREATE VIEW feature_union AS
 SELECT x.feature_id AS subject_id,
    y.feature_id AS object_id,
    x.srcfeature_id,
    x.strand AS subject_strand,
    y.strand AS object_strand,
        CASE
            WHEN (x.fmin < y.fmin) THEN x.fmin
            ELSE y.fmin
        END AS fmin,
        CASE
            WHEN (x.fmax > y.fmax) THEN x.fmax
            ELSE y.fmax
        END AS fmax
   FROM featureloc x,
    featureloc y
  WHERE ((x.srcfeature_id = y.srcfeature_id) AND ((x.fmax >= y.fmin) AND (x.fmin <= y.fmax)));

COMMENT ON VIEW feature_union IS 'set-union on interval defined by featureloc. featurelocs must meet';

CREATE SEQUENCE feature_uniquename_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

CREATE SEQUENCE featureloc_featureloc_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE featureloc_featureloc_id_seq OWNED BY featureloc.featureloc_id;

CREATE TABLE featureloc_pub (
    featureloc_pub_id bigint NOT NULL,
    featureloc_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE featureloc_pub IS 'Provenance of featureloc. Linking table between featurelocs and publications that mention them.';

CREATE SEQUENCE featureloc_pub_featureloc_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE featureloc_pub_featureloc_pub_id_seq OWNED BY featureloc_pub.featureloc_pub_id;

CREATE TABLE featuremap (
    featuremap_id bigint NOT NULL,
    name character varying(255),
    description text,
    unittype_id bigint
);

CREATE TABLE featuremap_contact (
    featuremap_contact_id bigint NOT NULL,
    featuremap_id bigint NOT NULL,
    contact_id bigint NOT NULL
);

COMMENT ON TABLE featuremap_contact IS 'Links contact(s) with a featuremap.  Used to
indicate a particular person or organization responsible for constrution of or
that can provide more information on a particular featuremap.';

CREATE SEQUENCE featuremap_contact_featuremap_contact_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE featuremap_contact_featuremap_contact_id_seq OWNED BY featuremap_contact.featuremap_contact_id;

CREATE TABLE featuremap_dbxref (
    featuremap_dbxref_id bigint NOT NULL,
    featuremap_id bigint NOT NULL,
    dbxref_id bigint NOT NULL,
    is_current boolean DEFAULT true NOT NULL
);

CREATE SEQUENCE featuremap_dbxref_featuremap_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE featuremap_dbxref_featuremap_dbxref_id_seq OWNED BY featuremap_dbxref.featuremap_dbxref_id;

CREATE SEQUENCE featuremap_featuremap_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE featuremap_featuremap_id_seq OWNED BY featuremap.featuremap_id;

CREATE TABLE featuremap_organism (
    featuremap_organism_id bigint NOT NULL,
    featuremap_id bigint NOT NULL,
    organism_id bigint NOT NULL
);

COMMENT ON TABLE featuremap_organism IS 'Links a featuremap to the organism(s) with which it is associated.';

CREATE SEQUENCE featuremap_organism_featuremap_organism_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE featuremap_organism_featuremap_organism_id_seq OWNED BY featuremap_organism.featuremap_organism_id;

CREATE TABLE featuremap_pub (
    featuremap_pub_id bigint NOT NULL,
    featuremap_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

CREATE SEQUENCE featuremap_pub_featuremap_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE featuremap_pub_featuremap_pub_id_seq OWNED BY featuremap_pub.featuremap_pub_id;

CREATE TABLE featuremapprop (
    featuremapprop_id bigint NOT NULL,
    featuremap_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE featuremapprop IS 'A featuremap can have any number of slot-value property
tags attached to it. This is an alternative to hardcoding a list of columns in the
relational schema, and is completely extensible.';

CREATE SEQUENCE featuremapprop_featuremapprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE featuremapprop_featuremapprop_id_seq OWNED BY featuremapprop.featuremapprop_id;

CREATE TABLE featurepos (
    featurepos_id bigint NOT NULL,
    featuremap_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    map_feature_id bigint NOT NULL,
    mappos double precision NOT NULL
);

COMMENT ON COLUMN featurepos.map_feature_id IS 'map_feature_id
links to the feature (map) upon which the feature is being localized.';

CREATE SEQUENCE featurepos_featurepos_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE featurepos_featurepos_id_seq OWNED BY featurepos.featurepos_id;

CREATE TABLE featureposprop (
    featureposprop_id bigint NOT NULL,
    featurepos_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE featureposprop IS 'Property or attribute of a featurepos record.';

CREATE SEQUENCE featureposprop_featureposprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE featureposprop_featureposprop_id_seq OWNED BY featureposprop.featureposprop_id;

CREATE SEQUENCE featureprop_featureprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE featureprop_featureprop_id_seq OWNED BY featureprop.featureprop_id;

CREATE TABLE featureprop_pub (
    featureprop_pub_id bigint NOT NULL,
    featureprop_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE featureprop_pub IS 'Provenance. Any featureprop assignment can optionally be supported by a publication.';

CREATE SEQUENCE featureprop_pub_featureprop_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE featureprop_pub_featureprop_pub_id_seq OWNED BY featureprop_pub.featureprop_pub_id;

CREATE TABLE featurerange (
    featurerange_id bigint NOT NULL,
    featuremap_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    leftstartf_id bigint NOT NULL,
    leftendf_id bigint,
    rightstartf_id bigint,
    rightendf_id bigint NOT NULL,
    rangestr character varying(255)
);

COMMENT ON TABLE featurerange IS 'In cases where the start and end of a mapped feature is a range, leftendf and rightstartf are populated. leftstartf_id, leftendf_id, rightstartf_id, rightendf_id are the ids of features with respect to which the feature is being mapped. These may be cytological bands.';

COMMENT ON COLUMN featurerange.featuremap_id IS 'featuremap_id is the id of the feature being mapped.';

CREATE SEQUENCE featurerange_featurerange_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE featurerange_featurerange_id_seq OWNED BY featurerange.featurerange_id;

CREATE VIEW featureset_meets AS
 SELECT x.object_id AS subject_id,
    y.object_id
   FROM ((feature_meets r
     JOIN feature_relationship x ON ((r.subject_id = x.subject_id)))
     JOIN feature_relationship y ON ((r.object_id = y.subject_id)));

CREATE VIEW fnr_type AS
 SELECT f.feature_id,
    f.name,
    f.dbxref_id,
    c.name AS type,
    f.residues,
    f.seqlen,
    f.md5checksum,
    f.type_id,
    f.timeaccessioned,
    f.timelastmodified
   FROM (feature f
     LEFT JOIN analysisfeature af ON ((f.feature_id = af.feature_id))),
    cvterm c
  WHERE ((f.type_id = c.cvterm_id) AND (af.feature_id IS NULL));

CREATE VIEW fp_key AS
 SELECT fp.feature_id,
    c.name AS pkey,
    fp.value
   FROM featureprop fp,
    cvterm c
  WHERE (fp.featureprop_id = c.cvterm_id);

CREATE TABLE genotype (
    genotype_id bigint NOT NULL,
    name text,
    uniquename text NOT NULL,
    description text,
    type_id bigint NOT NULL
);

COMMENT ON TABLE genotype IS 'Genetic context. A genotype is defined by a collection of features, mutations, balancers, deficiencies, haplotype blocks, or engineered constructs.';

COMMENT ON COLUMN genotype.name IS 'Optional alternative name for a genotype,
for display purposes.';

COMMENT ON COLUMN genotype.uniquename IS 'The unique name for a genotype;
typically derived from the features making up the genotype.';

CREATE SEQUENCE genotype_genotype_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE genotype_genotype_id_seq OWNED BY genotype.genotype_id;

CREATE TABLE genotypeprop (
    genotypeprop_id bigint NOT NULL,
    genotype_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

CREATE SEQUENCE genotypeprop_genotypeprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE genotypeprop_genotypeprop_id_seq OWNED BY genotypeprop.genotypeprop_id;

CREATE VIEW gff3atts AS
 SELECT fs.feature_id,
    'Ontology_term'::text AS type,
        CASE
            WHEN ((db.name)::text ~~ '%Gene Ontology%'::text) THEN (('GO:'::text || (dbx.accession)::text))::character varying
            WHEN ((db.name)::text ~~ 'Sequence Ontology%'::text) THEN (('SO:'::text || (dbx.accession)::text))::character varying
            ELSE ((((db.name)::text || ':'::text) || (dbx.accession)::text))::character varying
        END AS attribute
   FROM cvterm s,
    dbxref dbx,
    feature_cvterm fs,
    db
  WHERE ((fs.cvterm_id = s.cvterm_id) AND (s.dbxref_id = dbx.dbxref_id) AND (db.db_id = dbx.db_id))
UNION ALL
 SELECT fs.feature_id,
    'Dbxref'::text AS type,
    (((d.name)::text || ':'::text) || (s.accession)::text) AS attribute
   FROM dbxref s,
    feature_dbxref fs,
    db d
  WHERE ((fs.dbxref_id = s.dbxref_id) AND (s.db_id = d.db_id) AND ((d.name)::text <> 'GFF_source'::text))
UNION ALL
 SELECT f.feature_id,
    'Alias'::text AS type,
    s.name AS attribute
   FROM synonym s,
    feature_synonym fs,
    feature f
  WHERE ((fs.synonym_id = s.synonym_id) AND (f.feature_id = fs.feature_id) AND ((f.name)::text <> (s.name)::text) AND (f.uniquename <> (s.name)::text))
UNION ALL
 SELECT fp.feature_id,
    cv.name AS type,
    fp.value AS attribute
   FROM featureprop fp,
    cvterm cv
  WHERE (fp.type_id = cv.cvterm_id)
UNION ALL
 SELECT fs.feature_id,
    'pub'::text AS type,
    (((s.series_name)::text || ':'::text) || s.title) AS attribute
   FROM pub s,
    feature_pub fs
  WHERE (fs.pub_id = s.pub_id)
UNION ALL
 SELECT fr.subject_id AS feature_id,
    'Parent'::text AS type,
    parent.uniquename AS attribute
   FROM feature_relationship fr,
    feature parent
  WHERE ((fr.object_id = parent.feature_id) AND (fr.type_id = ( SELECT cvterm.cvterm_id
           FROM cvterm
          WHERE (((cvterm.name)::text = 'part_of'::text) AND (cvterm.cv_id IN ( SELECT cv.cv_id
                   FROM cv
                  WHERE ((cv.name)::text = 'relationship'::text)))))))
UNION ALL
 SELECT fr.subject_id AS feature_id,
    'Derives_from'::text AS type,
    parent.uniquename AS attribute
   FROM feature_relationship fr,
    feature parent
  WHERE ((fr.object_id = parent.feature_id) AND (fr.type_id = ( SELECT cvterm.cvterm_id
           FROM cvterm
          WHERE (((cvterm.name)::text = 'derives_from'::text) AND (cvterm.cv_id IN ( SELECT cv.cv_id
                   FROM cv
                  WHERE ((cv.name)::text = 'relationship'::text)))))))
UNION ALL
 SELECT fl.feature_id,
    'Target'::text AS type,
    (((((((target.name)::text || ' '::text) || (fl.fmin + 1)) || ' '::text) || fl.fmax) || ' '::text) || fl.strand) AS attribute
   FROM featureloc fl,
    feature target
  WHERE ((fl.srcfeature_id = target.feature_id) AND (fl.rank <> 0))
UNION ALL
 SELECT feature.feature_id,
    'ID'::text AS type,
    feature.uniquename AS attribute
   FROM feature
  WHERE (NOT (feature.type_id IN ( SELECT cvterm.cvterm_id
           FROM cvterm
          WHERE ((cvterm.name)::text = 'CDS'::text))))
UNION ALL
 SELECT feature.feature_id,
    'chado_feature_id'::text AS type,
    (feature.feature_id)::character varying AS attribute
   FROM feature
UNION ALL
 SELECT feature.feature_id,
    'Name'::text AS type,
    feature.name AS attribute
   FROM feature;

CREATE VIEW gff3view AS
 SELECT f.feature_id,
    sf.name AS ref,
    COALESCE(gffdbx.accession, '.'::character varying(255)) AS source,
    cv.name AS type,
    (fl.fmin + 1) AS fstart,
    fl.fmax AS fend,
    COALESCE((af.significance)::text, '.'::text) AS score,
        CASE
            WHEN (fl.strand = '-1'::integer) THEN '-'::text
            WHEN (fl.strand = 1) THEN '+'::text
            ELSE '.'::text
        END AS strand,
    COALESCE((fl.phase)::text, '.'::text) AS phase,
    f.seqlen,
    f.name,
    f.organism_id
   FROM (((((feature f
     LEFT JOIN featureloc fl ON ((f.feature_id = fl.feature_id)))
     LEFT JOIN feature sf ON ((fl.srcfeature_id = sf.feature_id)))
     LEFT JOIN ( SELECT fd.feature_id,
            d.accession
           FROM ((feature_dbxref fd
             JOIN dbxref d USING (dbxref_id))
             JOIN db USING (db_id))
          WHERE ((db.name)::text = 'GFF_source'::text)) gffdbx ON ((f.feature_id = gffdbx.feature_id)))
     LEFT JOIN cvterm cv ON ((f.type_id = cv.cvterm_id)))
     LEFT JOIN analysisfeature af ON ((f.feature_id = af.feature_id)));

CREATE VIEW intron_combined_view AS
 SELECT x1.feature_id AS exon1_id,
    x2.feature_id AS exon2_id,
        CASE
            WHEN (l1.strand = '-1'::integer) THEN l2.fmax
            ELSE l1.fmax
        END AS fmin,
        CASE
            WHEN (l1.strand = '-1'::integer) THEN l1.fmin
            ELSE l2.fmin
        END AS fmax,
    l1.strand,
    l1.srcfeature_id,
    r1.rank AS intron_rank,
    r1.object_id AS transcript_id
   FROM ((((((cvterm
     JOIN feature x1 ON ((x1.type_id = cvterm.cvterm_id)))
     JOIN feature_relationship r1 ON ((x1.feature_id = r1.subject_id)))
     JOIN featureloc l1 ON ((x1.feature_id = l1.feature_id)))
     JOIN feature x2 ON ((x2.type_id = cvterm.cvterm_id)))
     JOIN feature_relationship r2 ON ((x2.feature_id = r2.subject_id)))
     JOIN featureloc l2 ON ((x2.feature_id = l2.feature_id)))
  WHERE (((cvterm.name)::text = 'exon'::text) AND ((r2.rank - r1.rank) = 1) AND (r1.object_id = r2.object_id) AND (l1.strand = l2.strand) AND (l1.srcfeature_id = l2.srcfeature_id) AND (l1.locgroup = 0) AND (l2.locgroup = 0));

CREATE VIEW intronloc_view AS
 SELECT DISTINCT intron_combined_view.exon1_id,
    intron_combined_view.exon2_id,
    intron_combined_view.fmin,
    intron_combined_view.fmax,
    intron_combined_view.strand,
    intron_combined_view.srcfeature_id
   FROM intron_combined_view;

CREATE TABLE library (
    library_id bigint NOT NULL,
    organism_id bigint NOT NULL,
    name character varying(255),
    uniquename text NOT NULL,
    type_id bigint NOT NULL,
    is_obsolete integer DEFAULT 0 NOT NULL,
    timeaccessioned timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    timelastmodified timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);

COMMENT ON COLUMN library.type_id IS 'The type_id foreign key links to a controlled vocabulary of library types. Examples of this would be: "cDNA_library" or "genomic_library"';

CREATE TABLE library_contact (
    library_contact_id bigint NOT NULL,
    library_id bigint NOT NULL,
    contact_id bigint NOT NULL
);

COMMENT ON TABLE library_contact IS 'Links contact(s) with a library.  Used to indicate a particular person or organization responsible for creation of or that can provide more information on a particular library.';

CREATE SEQUENCE library_contact_library_contact_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE library_contact_library_contact_id_seq OWNED BY library_contact.library_contact_id;

CREATE TABLE library_cvterm (
    library_cvterm_id bigint NOT NULL,
    library_id bigint NOT NULL,
    cvterm_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE library_cvterm IS 'The table library_cvterm links a library to controlled vocabularies which describe the library.  For instance, there might be a link to the anatomy cv for "head" or "testes" for a head or testes library.';

CREATE SEQUENCE library_cvterm_library_cvterm_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE library_cvterm_library_cvterm_id_seq OWNED BY library_cvterm.library_cvterm_id;

CREATE TABLE library_dbxref (
    library_dbxref_id bigint NOT NULL,
    library_id bigint NOT NULL,
    dbxref_id bigint NOT NULL,
    is_current boolean DEFAULT true NOT NULL
);

COMMENT ON TABLE library_dbxref IS 'Links a library to dbxrefs.';

CREATE SEQUENCE library_dbxref_library_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE library_dbxref_library_dbxref_id_seq OWNED BY library_dbxref.library_dbxref_id;

CREATE TABLE library_expression (
    library_expression_id bigint NOT NULL,
    library_id bigint NOT NULL,
    expression_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE library_expression IS 'Links a library to expression statements.';

CREATE SEQUENCE library_expression_library_expression_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE library_expression_library_expression_id_seq OWNED BY library_expression.library_expression_id;

CREATE TABLE library_expressionprop (
    library_expressionprop_id bigint NOT NULL,
    library_expression_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE library_expressionprop IS 'Attributes of a library_expression relationship.';

CREATE SEQUENCE library_expressionprop_library_expressionprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE library_expressionprop_library_expressionprop_id_seq OWNED BY library_expressionprop.library_expressionprop_id;

CREATE TABLE library_feature (
    library_feature_id bigint NOT NULL,
    library_id bigint NOT NULL,
    feature_id bigint NOT NULL
);

COMMENT ON TABLE library_feature IS 'library_feature links a library to the clones which are contained in the library.  Examples of such linked features might be "cDNA_clone" or  "genomic_clone".';

CREATE SEQUENCE library_feature_library_feature_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE library_feature_library_feature_id_seq OWNED BY library_feature.library_feature_id;

CREATE TABLE library_featureprop (
    library_featureprop_id bigint NOT NULL,
    library_feature_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE library_featureprop IS 'Attributes of a library_feature relationship.';

CREATE SEQUENCE library_featureprop_library_featureprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE library_featureprop_library_featureprop_id_seq OWNED BY library_featureprop.library_featureprop_id;

CREATE SEQUENCE library_library_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE library_library_id_seq OWNED BY library.library_id;

CREATE TABLE library_pub (
    library_pub_id bigint NOT NULL,
    library_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE library_pub IS 'Attribution for a library.';

CREATE SEQUENCE library_pub_library_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE library_pub_library_pub_id_seq OWNED BY library_pub.library_pub_id;

CREATE TABLE library_relationship (
    library_relationship_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    object_id bigint NOT NULL,
    type_id bigint NOT NULL
);

COMMENT ON TABLE library_relationship IS 'Relationships between libraries.';

CREATE SEQUENCE library_relationship_library_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE library_relationship_library_relationship_id_seq OWNED BY library_relationship.library_relationship_id;

CREATE TABLE library_relationship_pub (
    library_relationship_pub_id bigint NOT NULL,
    library_relationship_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE library_relationship_pub IS 'Provenance of library_relationship.';

CREATE SEQUENCE library_relationship_pub_library_relationship_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE library_relationship_pub_library_relationship_pub_id_seq OWNED BY library_relationship_pub.library_relationship_pub_id;

CREATE TABLE library_synonym (
    library_synonym_id bigint NOT NULL,
    synonym_id bigint NOT NULL,
    library_id bigint NOT NULL,
    pub_id bigint NOT NULL,
    is_current boolean DEFAULT true NOT NULL,
    is_internal boolean DEFAULT false NOT NULL
);

COMMENT ON TABLE library_synonym IS 'Linking table between library and synonym.';

COMMENT ON COLUMN library_synonym.pub_id IS 'The pub_id link is for
relating the usage of a given synonym to the publication in which it was used.';

COMMENT ON COLUMN library_synonym.is_current IS 'The is_current bit indicates whether the linked synonym is the current -official- symbol for the linked library.';

COMMENT ON COLUMN library_synonym.is_internal IS 'Typically a synonym
exists so that somebody querying the database with an obsolete name
can find the object they are looking for under its current name.  If
the synonym has been used publicly and deliberately (e.g. in a paper), it my also be listed in reports as a synonym.   If the synonym was not used deliberately (e.g., there was a typo which went public), then the is_internal bit may be set to "true" so that it is known that the synonym is "internal" and should be queryable but should not be listed in reports as a valid synonym.';

CREATE SEQUENCE library_synonym_library_synonym_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE library_synonym_library_synonym_id_seq OWNED BY library_synonym.library_synonym_id;

CREATE TABLE libraryprop (
    libraryprop_id bigint NOT NULL,
    library_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE libraryprop IS 'Tag-value properties - follows standard chado model.';

CREATE SEQUENCE libraryprop_libraryprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE libraryprop_libraryprop_id_seq OWNED BY libraryprop.libraryprop_id;

CREATE TABLE libraryprop_pub (
    libraryprop_pub_id bigint NOT NULL,
    libraryprop_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE libraryprop_pub IS 'Attribution for libraryprop.';

CREATE SEQUENCE libraryprop_pub_libraryprop_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE libraryprop_pub_libraryprop_pub_id_seq OWNED BY libraryprop_pub.libraryprop_pub_id;

CREATE TABLE magedocumentation (
    magedocumentation_id bigint NOT NULL,
    mageml_id bigint NOT NULL,
    tableinfo_id bigint NOT NULL,
    row_id integer NOT NULL,
    mageidentifier text NOT NULL
);

CREATE SEQUENCE magedocumentation_magedocumentation_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE magedocumentation_magedocumentation_id_seq OWNED BY magedocumentation.magedocumentation_id;

CREATE TABLE mageml (
    mageml_id bigint NOT NULL,
    mage_package text NOT NULL,
    mage_ml text NOT NULL
);

COMMENT ON TABLE mageml IS 'This table is for storing extra bits of MAGEml in a denormalized form. More normalization would require many more tables.';

CREATE SEQUENCE mageml_mageml_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE mageml_mageml_id_seq OWNED BY mageml.mageml_id;

CREATE TABLE nd_experiment (
    nd_experiment_id bigint NOT NULL,
    nd_geolocation_id bigint NOT NULL,
    type_id bigint NOT NULL
);

COMMENT ON TABLE nd_experiment IS 'This is the core table for the natural diversity module,
representing each individual assay that is undertaken (this is usually *not* an
entire experiment). Each nd_experiment should give rise to a single genotype or
phenotype and be described via 1 (or more) protocols. Collections of assays that
relate to each other should be linked to the same record in the project table.';

CREATE TABLE nd_experiment_analysis (
    nd_experiment_analysis_id bigint NOT NULL,
    nd_experiment_id bigint NOT NULL,
    analysis_id bigint NOT NULL,
    type_id bigint
);

COMMENT ON TABLE nd_experiment_analysis IS 'An analysis that is used in an experiment';

CREATE SEQUENCE nd_experiment_analysis_nd_experiment_analysis_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_experiment_analysis_nd_experiment_analysis_id_seq OWNED BY nd_experiment_analysis.nd_experiment_analysis_id;

CREATE TABLE nd_experiment_contact (
    nd_experiment_contact_id bigint NOT NULL,
    nd_experiment_id bigint NOT NULL,
    contact_id bigint NOT NULL
);

CREATE SEQUENCE nd_experiment_contact_nd_experiment_contact_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_experiment_contact_nd_experiment_contact_id_seq OWNED BY nd_experiment_contact.nd_experiment_contact_id;

CREATE TABLE nd_experiment_dbxref (
    nd_experiment_dbxref_id bigint NOT NULL,
    nd_experiment_id bigint NOT NULL,
    dbxref_id bigint NOT NULL
);

COMMENT ON TABLE nd_experiment_dbxref IS 'Cross-reference experiment to accessions, images, etc';

CREATE SEQUENCE nd_experiment_dbxref_nd_experiment_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_experiment_dbxref_nd_experiment_dbxref_id_seq OWNED BY nd_experiment_dbxref.nd_experiment_dbxref_id;

CREATE TABLE nd_experiment_genotype (
    nd_experiment_genotype_id bigint NOT NULL,
    nd_experiment_id bigint NOT NULL,
    genotype_id bigint NOT NULL
);

COMMENT ON TABLE nd_experiment_genotype IS 'Linking table: experiments to the genotypes they produce. There is a one-to-one relationship between an experiment and a genotype since each genotype record should point to one experiment. Add a new experiment_id for each genotype record.';

CREATE SEQUENCE nd_experiment_genotype_nd_experiment_genotype_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_experiment_genotype_nd_experiment_genotype_id_seq OWNED BY nd_experiment_genotype.nd_experiment_genotype_id;

CREATE SEQUENCE nd_experiment_nd_experiment_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_experiment_nd_experiment_id_seq OWNED BY nd_experiment.nd_experiment_id;

CREATE TABLE nd_experiment_phenotype (
    nd_experiment_phenotype_id bigint NOT NULL,
    nd_experiment_id bigint NOT NULL,
    phenotype_id bigint NOT NULL
);

COMMENT ON TABLE nd_experiment_phenotype IS 'Linking table: experiments to the phenotypes they produce. There is a one-to-one relationship between an experiment and a phenotype since each phenotype record should point to one experiment. Add a new experiment_id for each phenotype record.';

CREATE SEQUENCE nd_experiment_phenotype_nd_experiment_phenotype_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_experiment_phenotype_nd_experiment_phenotype_id_seq OWNED BY nd_experiment_phenotype.nd_experiment_phenotype_id;

CREATE TABLE nd_experiment_project (
    nd_experiment_project_id bigint NOT NULL,
    project_id bigint NOT NULL,
    nd_experiment_id bigint NOT NULL
);

COMMENT ON TABLE nd_experiment_project IS 'Used to group together related nd_experiment records. All nd_experiments
should be linked to at least one project.';

CREATE SEQUENCE nd_experiment_project_nd_experiment_project_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_experiment_project_nd_experiment_project_id_seq OWNED BY nd_experiment_project.nd_experiment_project_id;

CREATE TABLE nd_experiment_protocol (
    nd_experiment_protocol_id bigint NOT NULL,
    nd_experiment_id bigint NOT NULL,
    nd_protocol_id bigint NOT NULL
);

COMMENT ON TABLE nd_experiment_protocol IS 'Linking table: experiments to the protocols they involve.';

CREATE SEQUENCE nd_experiment_protocol_nd_experiment_protocol_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_experiment_protocol_nd_experiment_protocol_id_seq OWNED BY nd_experiment_protocol.nd_experiment_protocol_id;

CREATE TABLE nd_experiment_pub (
    nd_experiment_pub_id bigint NOT NULL,
    nd_experiment_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE nd_experiment_pub IS 'Linking nd_experiment(s) to publication(s)';

CREATE SEQUENCE nd_experiment_pub_nd_experiment_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_experiment_pub_nd_experiment_pub_id_seq OWNED BY nd_experiment_pub.nd_experiment_pub_id;

CREATE TABLE nd_experiment_stock (
    nd_experiment_stock_id bigint NOT NULL,
    nd_experiment_id bigint NOT NULL,
    stock_id bigint NOT NULL,
    type_id bigint NOT NULL
);

COMMENT ON TABLE nd_experiment_stock IS 'Part of a stock or a clone of a stock that is used in an experiment';

COMMENT ON COLUMN nd_experiment_stock.stock_id IS 'stock used in the extraction or the corresponding stock for the clone';

CREATE TABLE nd_experiment_stock_dbxref (
    nd_experiment_stock_dbxref_id bigint NOT NULL,
    nd_experiment_stock_id bigint NOT NULL,
    dbxref_id bigint NOT NULL
);

COMMENT ON TABLE nd_experiment_stock_dbxref IS 'Cross-reference experiment_stock to accessions, images, etc';

CREATE SEQUENCE nd_experiment_stock_dbxref_nd_experiment_stock_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_experiment_stock_dbxref_nd_experiment_stock_dbxref_id_seq OWNED BY nd_experiment_stock_dbxref.nd_experiment_stock_dbxref_id;

CREATE SEQUENCE nd_experiment_stock_nd_experiment_stock_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_experiment_stock_nd_experiment_stock_id_seq OWNED BY nd_experiment_stock.nd_experiment_stock_id;

CREATE TABLE nd_experiment_stockprop (
    nd_experiment_stockprop_id bigint NOT NULL,
    nd_experiment_stock_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE nd_experiment_stockprop IS 'Property/value associations for experiment_stocks. This table can store the properties such as treatment';

COMMENT ON COLUMN nd_experiment_stockprop.nd_experiment_stock_id IS 'The experiment_stock to which the property applies.';

COMMENT ON COLUMN nd_experiment_stockprop.type_id IS 'The name of the property as a reference to a controlled vocabulary term.';

COMMENT ON COLUMN nd_experiment_stockprop.value IS 'The value of the property.';

COMMENT ON COLUMN nd_experiment_stockprop.rank IS 'The rank of the property value, if the property has an array of values.';

CREATE SEQUENCE nd_experiment_stockprop_nd_experiment_stockprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_experiment_stockprop_nd_experiment_stockprop_id_seq OWNED BY nd_experiment_stockprop.nd_experiment_stockprop_id;

CREATE TABLE nd_experimentprop (
    nd_experimentprop_id bigint NOT NULL,
    nd_experiment_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE nd_experimentprop IS 'An nd_experiment can have any number of
slot-value property tags attached to it. This is an alternative to
hardcoding a list of columns in the relational schema, and is
completely extensible. There is a unique constraint, stockprop_c1, for
the combination of stock_id, rank, and type_id. Multivalued property-value pairs must be differentiated by rank.';

CREATE SEQUENCE nd_experimentprop_nd_experimentprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_experimentprop_nd_experimentprop_id_seq OWNED BY nd_experimentprop.nd_experimentprop_id;

CREATE TABLE nd_geolocation (
    nd_geolocation_id bigint NOT NULL,
    description text,
    latitude real,
    longitude real,
    geodetic_datum character varying(32),
    altitude real
);

COMMENT ON TABLE nd_geolocation IS 'The geo-referencable location of the stock. NOTE: This entity is subject to change as a more general and possibly more OpenGIS-compliant geolocation module may be introduced into Chado.';

COMMENT ON COLUMN nd_geolocation.description IS 'A textual representation of the location, if this is the original georeference. Optional if the original georeference is available in lat/long coordinates.';

COMMENT ON COLUMN nd_geolocation.latitude IS 'The decimal latitude coordinate of the georeference, using positive and negative sign to indicate N and S, respectively.';

COMMENT ON COLUMN nd_geolocation.longitude IS 'The decimal longitude coordinate of the georeference, using positive and negative sign to indicate E and W, respectively.';

COMMENT ON COLUMN nd_geolocation.geodetic_datum IS 'The geodetic system on which the geo-reference coordinates are based. For geo-references measured between 1984 and 2010, this will typically be WGS84.';

COMMENT ON COLUMN nd_geolocation.altitude IS 'The altitude (elevation) of the location in meters. If the altitude is only known as a range, this is the average, and altitude_dev will hold half of the width of the range.';

CREATE SEQUENCE nd_geolocation_nd_geolocation_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_geolocation_nd_geolocation_id_seq OWNED BY nd_geolocation.nd_geolocation_id;

CREATE TABLE nd_geolocationprop (
    nd_geolocationprop_id bigint NOT NULL,
    nd_geolocation_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE nd_geolocationprop IS 'Property/value associations for geolocations. This table can store the properties such as location and environment';

COMMENT ON COLUMN nd_geolocationprop.type_id IS 'The name of the property as a reference to a controlled vocabulary term.';

COMMENT ON COLUMN nd_geolocationprop.value IS 'The value of the property.';

COMMENT ON COLUMN nd_geolocationprop.rank IS 'The rank of the property value, if the property has an array of values.';

CREATE SEQUENCE nd_geolocationprop_nd_geolocationprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_geolocationprop_nd_geolocationprop_id_seq OWNED BY nd_geolocationprop.nd_geolocationprop_id;

CREATE TABLE nd_protocol (
    nd_protocol_id bigint NOT NULL,
    name character varying(255) NOT NULL,
    type_id bigint NOT NULL
);

COMMENT ON TABLE nd_protocol IS 'A protocol can be anything that is done as part of the experiment.';

COMMENT ON COLUMN nd_protocol.name IS 'The protocol name.';

CREATE SEQUENCE nd_protocol_nd_protocol_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_protocol_nd_protocol_id_seq OWNED BY nd_protocol.nd_protocol_id;

CREATE TABLE nd_protocol_reagent (
    nd_protocol_reagent_id bigint NOT NULL,
    nd_protocol_id bigint NOT NULL,
    reagent_id bigint NOT NULL,
    type_id bigint NOT NULL
);

CREATE SEQUENCE nd_protocol_reagent_nd_protocol_reagent_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_protocol_reagent_nd_protocol_reagent_id_seq OWNED BY nd_protocol_reagent.nd_protocol_reagent_id;

CREATE TABLE nd_protocolprop (
    nd_protocolprop_id bigint NOT NULL,
    nd_protocol_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE nd_protocolprop IS 'Property/value associations for protocol.';

COMMENT ON COLUMN nd_protocolprop.nd_protocol_id IS 'The protocol to which the property applies.';

COMMENT ON COLUMN nd_protocolprop.type_id IS 'The name of the property as a reference to a controlled vocabulary term.';

COMMENT ON COLUMN nd_protocolprop.value IS 'The value of the property.';

COMMENT ON COLUMN nd_protocolprop.rank IS 'The rank of the property value, if the property has an array of values.';

CREATE SEQUENCE nd_protocolprop_nd_protocolprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_protocolprop_nd_protocolprop_id_seq OWNED BY nd_protocolprop.nd_protocolprop_id;

CREATE TABLE nd_reagent (
    nd_reagent_id bigint NOT NULL,
    name character varying(80) NOT NULL,
    type_id bigint NOT NULL,
    feature_id bigint
);

COMMENT ON TABLE nd_reagent IS 'A reagent such as a primer, an enzyme, an adapter oligo, a linker oligo. Reagents are used in genotyping experiments, or in any other kind of experiment.';

COMMENT ON COLUMN nd_reagent.name IS 'The name of the reagent. The name should be unique for a given type.';

COMMENT ON COLUMN nd_reagent.type_id IS 'The type of the reagent, for example linker oligomer, or forward primer.';

COMMENT ON COLUMN nd_reagent.feature_id IS 'If the reagent is a primer, the feature that it corresponds to. More generally, the corresponding feature for any reagent that has a sequence that maps to another sequence.';

CREATE SEQUENCE nd_reagent_nd_reagent_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_reagent_nd_reagent_id_seq OWNED BY nd_reagent.nd_reagent_id;

CREATE TABLE nd_reagent_relationship (
    nd_reagent_relationship_id bigint NOT NULL,
    subject_reagent_id bigint NOT NULL,
    object_reagent_id bigint NOT NULL,
    type_id bigint NOT NULL
);

COMMENT ON TABLE nd_reagent_relationship IS 'Relationships between reagents. Some reagents form a group. i.e., they are used all together or not at all. Examples are adapter/linker/enzyme experiment reagents.';

COMMENT ON COLUMN nd_reagent_relationship.subject_reagent_id IS 'The subject reagent in the relationship. In parent/child terminology, the subject is the child. For example, in "linkerA 3prime-overhang-linker enzymeA" linkerA is the subject, 3prime-overhand-linker is the type, and enzymeA is the object.';

COMMENT ON COLUMN nd_reagent_relationship.object_reagent_id IS 'The object reagent in the relationship. In parent/child terminology, the object is the parent. For example, in "linkerA 3prime-overhang-linker enzymeA" linkerA is the subject, 3prime-overhand-linker is the type, and enzymeA is the object.';

COMMENT ON COLUMN nd_reagent_relationship.type_id IS 'The type (or predicate) of the relationship. For example, in "linkerA 3prime-overhang-linker enzymeA" linkerA is the subject, 3prime-overhand-linker is the type, and enzymeA is the object.';

CREATE SEQUENCE nd_reagent_relationship_nd_reagent_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_reagent_relationship_nd_reagent_relationship_id_seq OWNED BY nd_reagent_relationship.nd_reagent_relationship_id;

CREATE TABLE nd_reagentprop (
    nd_reagentprop_id bigint NOT NULL,
    nd_reagent_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

CREATE SEQUENCE nd_reagentprop_nd_reagentprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE nd_reagentprop_nd_reagentprop_id_seq OWNED BY nd_reagentprop.nd_reagentprop_id;

CREATE TABLE organism (
    organism_id bigint NOT NULL,
    abbreviation character varying(255),
    genus character varying(255) NOT NULL,
    species character varying(255) NOT NULL,
    common_name character varying(255),
    infraspecific_name character varying(1024),
    type_id bigint,
    comment text
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

COMMENT ON COLUMN organism.infraspecific_name IS 'The scientific name for any taxon
below the rank of species.  The rank should be specified using the type_id field
and the name is provided here.';

COMMENT ON COLUMN organism.type_id IS 'A controlled vocabulary term that
specifies the organism rank below species. It is used when an infraspecific
name is provided.  Ideally, the rank should be a valid ICN name such as
subspecies, varietas, subvarietas, forma and subforma';

CREATE TABLE organism_cvterm (
    organism_cvterm_id bigint NOT NULL,
    organism_id bigint NOT NULL,
    cvterm_id bigint NOT NULL,
    rank integer DEFAULT 0 NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE organism_cvterm IS 'organism to cvterm associations. Examples: taxonomic name';

COMMENT ON COLUMN organism_cvterm.rank IS 'Property-Value
ordering. Any organism_cvterm can have multiple values for any particular
property type - these are ordered in a list using rank, counting from
zero. For properties that are single-valued rather than multi-valued,
the default 0 value should be used';

CREATE SEQUENCE organism_cvterm_organism_cvterm_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE organism_cvterm_organism_cvterm_id_seq OWNED BY organism_cvterm.organism_cvterm_id;

CREATE TABLE organism_cvtermprop (
    organism_cvtermprop_id bigint NOT NULL,
    organism_cvterm_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

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

CREATE SEQUENCE organism_cvtermprop_organism_cvtermprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE organism_cvtermprop_organism_cvtermprop_id_seq OWNED BY organism_cvtermprop.organism_cvtermprop_id;

CREATE TABLE organism_dbxref (
    organism_dbxref_id bigint NOT NULL,
    organism_id bigint NOT NULL,
    dbxref_id bigint NOT NULL
);

COMMENT ON TABLE organism_dbxref IS 'Links an organism to a dbxref.';

CREATE SEQUENCE organism_dbxref_organism_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE organism_dbxref_organism_dbxref_id_seq OWNED BY organism_dbxref.organism_dbxref_id;

CREATE SEQUENCE organism_organism_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE organism_organism_id_seq OWNED BY organism.organism_id;

CREATE TABLE organism_pub (
    organism_pub_id bigint NOT NULL,
    organism_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE organism_pub IS 'Attribution for organism.';

CREATE SEQUENCE organism_pub_organism_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE organism_pub_organism_pub_id_seq OWNED BY organism_pub.organism_pub_id;

CREATE TABLE organism_relationship (
    organism_relationship_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    object_id bigint NOT NULL,
    type_id bigint NOT NULL,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE organism_relationship IS 'Specifies relationships between organisms
that are not taxonomic. For example, in breeding, relationships such as
"sterile_with", "incompatible_with", or "fertile_with" would be appropriate. Taxonomic
relatinoships should be housed in the phylogeny tables.';

CREATE SEQUENCE organism_relationship_organism_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE organism_relationship_organism_relationship_id_seq OWNED BY organism_relationship.organism_relationship_id;

CREATE TABLE organismprop (
    organismprop_id bigint NOT NULL,
    organism_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE organismprop IS 'Tag-value properties - follows standard chado model.';

CREATE SEQUENCE organismprop_organismprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE organismprop_organismprop_id_seq OWNED BY organismprop.organismprop_id;

CREATE TABLE organismprop_pub (
    organismprop_pub_id bigint NOT NULL,
    organismprop_id bigint NOT NULL,
    pub_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE organismprop_pub IS 'Attribution for organismprop.';

CREATE SEQUENCE organismprop_pub_organismprop_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE organismprop_pub_organismprop_pub_id_seq OWNED BY organismprop_pub.organismprop_pub_id;

CREATE TABLE phendesc (
    phendesc_id bigint NOT NULL,
    genotype_id bigint NOT NULL,
    environment_id bigint NOT NULL,
    description text NOT NULL,
    type_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE phendesc IS 'A summary of a _set_ of phenotypic statements for any one gcontext made in any one publication.';

CREATE SEQUENCE phendesc_phendesc_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phendesc_phendesc_id_seq OWNED BY phendesc.phendesc_id;

CREATE TABLE phenotype (
    phenotype_id bigint NOT NULL,
    uniquename text NOT NULL,
    name text,
    observable_id bigint,
    attr_id bigint,
    value text,
    cvalue_id bigint,
    assay_id bigint
);

COMMENT ON TABLE phenotype IS 'A phenotypic statement, or a single
atomic phenotypic observation, is a controlled sentence describing
observable effects of non-wild type function. E.g. Obs=eye, attribute=color, cvalue=red.';

COMMENT ON COLUMN phenotype.observable_id IS 'The entity: e.g. anatomy_part, biological_process.';

COMMENT ON COLUMN phenotype.attr_id IS 'Phenotypic attribute (quality, property, attribute, character) - drawn from PATO.';

COMMENT ON COLUMN phenotype.value IS 'Value of attribute - unconstrained free text. Used only if cvalue_id is not appropriate.';

COMMENT ON COLUMN phenotype.cvalue_id IS 'Phenotype attribute value (state).';

COMMENT ON COLUMN phenotype.assay_id IS 'Evidence type.';

CREATE TABLE phenotype_comparison (
    phenotype_comparison_id bigint NOT NULL,
    genotype1_id bigint NOT NULL,
    environment1_id bigint NOT NULL,
    genotype2_id bigint NOT NULL,
    environment2_id bigint NOT NULL,
    phenotype1_id bigint NOT NULL,
    phenotype2_id bigint,
    pub_id bigint NOT NULL,
    organism_id bigint NOT NULL
);

COMMENT ON TABLE phenotype_comparison IS 'Comparison of phenotypes e.g., genotype1/environment1/phenotype1 "non-suppressible" with respect to genotype2/environment2/phenotype2.';

CREATE TABLE phenotype_comparison_cvterm (
    phenotype_comparison_cvterm_id bigint NOT NULL,
    phenotype_comparison_id bigint NOT NULL,
    cvterm_id bigint NOT NULL,
    pub_id bigint NOT NULL,
    rank integer DEFAULT 0 NOT NULL
);

CREATE SEQUENCE phenotype_comparison_cvterm_phenotype_comparison_cvterm_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phenotype_comparison_cvterm_phenotype_comparison_cvterm_id_seq OWNED BY phenotype_comparison_cvterm.phenotype_comparison_cvterm_id;

CREATE SEQUENCE phenotype_comparison_phenotype_comparison_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phenotype_comparison_phenotype_comparison_id_seq OWNED BY phenotype_comparison.phenotype_comparison_id;

CREATE TABLE phenotype_cvterm (
    phenotype_cvterm_id bigint NOT NULL,
    phenotype_id bigint NOT NULL,
    cvterm_id bigint NOT NULL,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE phenotype_cvterm IS 'phenotype to cvterm associations.';

CREATE SEQUENCE phenotype_cvterm_phenotype_cvterm_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phenotype_cvterm_phenotype_cvterm_id_seq OWNED BY phenotype_cvterm.phenotype_cvterm_id;

CREATE SEQUENCE phenotype_phenotype_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phenotype_phenotype_id_seq OWNED BY phenotype.phenotype_id;

CREATE TABLE phenotypeprop (
    phenotypeprop_id bigint NOT NULL,
    phenotype_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE phenotypeprop IS 'A phenotype can have any number of slot-value property tags attached to it. This is an alternative to hardcoding a list of columns in the relational schema, and is completely extensible. There is a unique constraint, phenotypeprop_c1, for the combination of phenotype_id, rank, and type_id. Multivalued property-value pairs must be differentiated by rank.';

CREATE SEQUENCE phenotypeprop_phenotypeprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phenotypeprop_phenotypeprop_id_seq OWNED BY phenotypeprop.phenotypeprop_id;

CREATE TABLE phenstatement (
    phenstatement_id bigint NOT NULL,
    genotype_id bigint NOT NULL,
    environment_id bigint NOT NULL,
    phenotype_id bigint NOT NULL,
    type_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE phenstatement IS 'Phenotypes are things like "larval lethal".  Phenstatements are things like "dpp-1 is recessive larval lethal". So essentially phenstatement is a linking table expressing the relationship between genotype, environment, and phenotype.';

CREATE SEQUENCE phenstatement_phenstatement_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phenstatement_phenstatement_id_seq OWNED BY phenstatement.phenstatement_id;

CREATE TABLE phylonode (
    phylonode_id bigint NOT NULL,
    phylotree_id bigint NOT NULL,
    parent_phylonode_id bigint,
    left_idx integer NOT NULL,
    right_idx integer NOT NULL,
    type_id bigint,
    feature_id bigint,
    label character varying(255),
    distance double precision
);

COMMENT ON TABLE phylonode IS 'This is the most pervasive
       element in the phylogeny module, cataloging the "phylonodes" of
       tree graphs. Edges are implied by the parent_phylonode_id
       reflexive closure. For all nodes in a nested set implementation the left and right index will be *between* the parents left and right indexes.';

COMMENT ON COLUMN phylonode.parent_phylonode_id IS 'Root phylonode can have null parent_phylonode_id value.';

COMMENT ON COLUMN phylonode.type_id IS 'Type: e.g. root, interior, leaf.';

COMMENT ON COLUMN phylonode.feature_id IS 'Phylonodes can have optional features attached to them e.g. a protein or nucleotide sequence usually attached to a leaf of the phylotree for non-leaf nodes, the feature may be a feature that is an instance of SO:match; this feature is the alignment of all leaf features beneath it.';

CREATE TABLE phylonode_dbxref (
    phylonode_dbxref_id bigint NOT NULL,
    phylonode_id bigint NOT NULL,
    dbxref_id bigint NOT NULL
);

COMMENT ON TABLE phylonode_dbxref IS 'For example, for orthology, paralogy group identifiers; could also be used for NCBI taxonomy; for sequences, refer to phylonode_feature, feature associated dbxrefs.';

CREATE SEQUENCE phylonode_dbxref_phylonode_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phylonode_dbxref_phylonode_dbxref_id_seq OWNED BY phylonode_dbxref.phylonode_dbxref_id;

CREATE TABLE phylonode_organism (
    phylonode_organism_id bigint NOT NULL,
    phylonode_id bigint NOT NULL,
    organism_id bigint NOT NULL
);

COMMENT ON TABLE phylonode_organism IS 'This linking table should only be used for nodes in taxonomy trees; it provides a mapping between the node and an organism. One node can have zero or one organisms, one organism can have zero or more nodes (although typically it should only have one in the standard NCBI taxonomy tree).';

COMMENT ON COLUMN phylonode_organism.phylonode_id IS 'One phylonode cannot refer to >1 organism.';

CREATE SEQUENCE phylonode_organism_phylonode_organism_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phylonode_organism_phylonode_organism_id_seq OWNED BY phylonode_organism.phylonode_organism_id;

CREATE SEQUENCE phylonode_phylonode_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phylonode_phylonode_id_seq OWNED BY phylonode.phylonode_id;

CREATE TABLE phylonode_pub (
    phylonode_pub_id bigint NOT NULL,
    phylonode_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

CREATE SEQUENCE phylonode_pub_phylonode_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phylonode_pub_phylonode_pub_id_seq OWNED BY phylonode_pub.phylonode_pub_id;

CREATE TABLE phylonode_relationship (
    phylonode_relationship_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    object_id bigint NOT NULL,
    type_id bigint NOT NULL,
    rank integer,
    phylotree_id bigint NOT NULL
);

COMMENT ON TABLE phylonode_relationship IS 'This is for
relationships that are not strictly hierarchical; for example,
horizontal gene transfer. Most phylogenetic trees are strictly
hierarchical, nevertheless it is here for completeness.';

CREATE SEQUENCE phylonode_relationship_phylonode_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phylonode_relationship_phylonode_relationship_id_seq OWNED BY phylonode_relationship.phylonode_relationship_id;

CREATE TABLE phylonodeprop (
    phylonodeprop_id bigint NOT NULL,
    phylonode_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text DEFAULT ''::text NOT NULL,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON COLUMN phylonodeprop.type_id IS 'type_id could designate phylonode hierarchy relationships, for example: species taxonomy (kingdom, order, family, genus, species), "ortholog/paralog", "fold/superfold", etc.';

CREATE SEQUENCE phylonodeprop_phylonodeprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phylonodeprop_phylonodeprop_id_seq OWNED BY phylonodeprop.phylonodeprop_id;

CREATE TABLE phylotree (
    phylotree_id bigint NOT NULL,
    dbxref_id bigint NOT NULL,
    name character varying(255),
    type_id bigint,
    analysis_id bigint,
    comment text
);

COMMENT ON TABLE phylotree IS 'Global anchor for phylogenetic tree.';

COMMENT ON COLUMN phylotree.type_id IS 'Type: protein, nucleotide, taxonomy, for example. The type should be any SO type, or "taxonomy".';

CREATE SEQUENCE phylotree_phylotree_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phylotree_phylotree_id_seq OWNED BY phylotree.phylotree_id;

CREATE TABLE phylotree_pub (
    phylotree_pub_id bigint NOT NULL,
    phylotree_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE phylotree_pub IS 'Tracks citations global to the tree e.g. multiple sequence alignment supporting tree construction.';

CREATE SEQUENCE phylotree_pub_phylotree_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phylotree_pub_phylotree_pub_id_seq OWNED BY phylotree_pub.phylotree_pub_id;

CREATE TABLE phylotreeprop (
    phylotreeprop_id bigint NOT NULL,
    phylotree_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

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

CREATE SEQUENCE phylotreeprop_phylotreeprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE phylotreeprop_phylotreeprop_id_seq OWNED BY phylotreeprop.phylotreeprop_id;

CREATE TABLE project (
    project_id bigint NOT NULL,
    name character varying(255) NOT NULL,
    description text
);

COMMENT ON TABLE project IS 'Standard Chado flexible property table for projects.';

CREATE TABLE project_analysis (
    project_analysis_id bigint NOT NULL,
    project_id bigint NOT NULL,
    analysis_id bigint NOT NULL,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE project_analysis IS 'Links an analysis to a project that may contain multiple analyses.
The rank column can be used to specify a simple ordering in which analyses were executed.';

CREATE SEQUENCE project_analysis_project_analysis_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE project_analysis_project_analysis_id_seq OWNED BY project_analysis.project_analysis_id;

CREATE TABLE project_contact (
    project_contact_id bigint NOT NULL,
    project_id bigint NOT NULL,
    contact_id bigint NOT NULL
);

COMMENT ON TABLE project_contact IS 'Linking table for associating projects and contacts.';

CREATE SEQUENCE project_contact_project_contact_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE project_contact_project_contact_id_seq OWNED BY project_contact.project_contact_id;

CREATE TABLE project_dbxref (
    project_dbxref_id bigint NOT NULL,
    project_id bigint NOT NULL,
    dbxref_id bigint NOT NULL,
    is_current boolean DEFAULT true NOT NULL
);

COMMENT ON TABLE project_dbxref IS 'project_dbxref links a project to dbxrefs.';

COMMENT ON COLUMN project_dbxref.is_current IS 'The is_current boolean indicates whether the linked dbxref is the current -official- dbxref for the linked project.';

CREATE SEQUENCE project_dbxref_project_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE project_dbxref_project_dbxref_id_seq OWNED BY project_dbxref.project_dbxref_id;

CREATE TABLE project_feature (
    project_feature_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    project_id bigint NOT NULL
);

COMMENT ON TABLE project_feature IS 'This table is intended associate records in the feature table with a project.';

CREATE SEQUENCE project_feature_project_feature_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE project_feature_project_feature_id_seq OWNED BY project_feature.project_feature_id;

CREATE SEQUENCE project_project_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE project_project_id_seq OWNED BY project.project_id;

CREATE TABLE project_pub (
    project_pub_id bigint NOT NULL,
    project_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE project_pub IS 'Linking table for associating projects and publications.';

CREATE SEQUENCE project_pub_project_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE project_pub_project_pub_id_seq OWNED BY project_pub.project_pub_id;

CREATE TABLE project_relationship (
    project_relationship_id bigint NOT NULL,
    subject_project_id bigint NOT NULL,
    object_project_id bigint NOT NULL,
    type_id bigint NOT NULL
);

COMMENT ON TABLE project_relationship IS 'Linking table for relating projects to each other.  For example, a
given project could be composed of several smaller subprojects';

COMMENT ON COLUMN project_relationship.type_id IS 'The cvterm type of the relationship being stated, such as "part of".';

CREATE SEQUENCE project_relationship_project_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE project_relationship_project_relationship_id_seq OWNED BY project_relationship.project_relationship_id;

CREATE TABLE project_stock (
    project_stock_id bigint NOT NULL,
    stock_id bigint NOT NULL,
    project_id bigint NOT NULL
);

COMMENT ON TABLE project_stock IS 'This table is intended associate records in the stock table with a project.';

CREATE SEQUENCE project_stock_project_stock_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE project_stock_project_stock_id_seq OWNED BY project_stock.project_stock_id;

CREATE TABLE projectprop (
    projectprop_id bigint NOT NULL,
    project_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

CREATE SEQUENCE projectprop_projectprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE projectprop_projectprop_id_seq OWNED BY projectprop.projectprop_id;

CREATE TABLE protocol (
    protocol_id bigint NOT NULL,
    type_id bigint NOT NULL,
    pub_id bigint,
    dbxref_id bigint,
    name text NOT NULL,
    uri text,
    protocoldescription text,
    hardwaredescription text,
    softwaredescription text
);

COMMENT ON TABLE protocol IS 'Procedural notes on how data was prepared and processed.';

CREATE SEQUENCE protocol_protocol_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE protocol_protocol_id_seq OWNED BY protocol.protocol_id;

CREATE TABLE protocolparam (
    protocolparam_id bigint NOT NULL,
    protocol_id bigint NOT NULL,
    name text NOT NULL,
    datatype_id bigint,
    unittype_id bigint,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE protocolparam IS 'Parameters related to a
protocol. For example, if the protocol is a soak, this might include attributes of bath temperature and duration.';

CREATE SEQUENCE protocolparam_protocolparam_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE protocolparam_protocolparam_id_seq OWNED BY protocolparam.protocolparam_id;

CREATE TABLE pub_dbxref (
    pub_dbxref_id bigint NOT NULL,
    pub_id bigint NOT NULL,
    dbxref_id bigint NOT NULL,
    is_current boolean DEFAULT true NOT NULL
);

COMMENT ON TABLE pub_dbxref IS 'Handle links to repositories,
e.g. Pubmed, Biosis, zoorec, OCLC, Medline, ISSN, coden...';

CREATE SEQUENCE pub_dbxref_pub_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE pub_dbxref_pub_dbxref_id_seq OWNED BY pub_dbxref.pub_dbxref_id;

CREATE SEQUENCE pub_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE pub_pub_id_seq OWNED BY pub.pub_id;

CREATE TABLE pub_relationship (
    pub_relationship_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    object_id bigint NOT NULL,
    type_id bigint NOT NULL
);

COMMENT ON TABLE pub_relationship IS 'Handle relationships between
publications, e.g. when one publication makes others obsolete, when one
publication contains errata with respect to other publication(s), or
when one publication also appears in another pub.';

CREATE SEQUENCE pub_relationship_pub_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE pub_relationship_pub_relationship_id_seq OWNED BY pub_relationship.pub_relationship_id;

CREATE TABLE pubauthor (
    pubauthor_id bigint NOT NULL,
    pub_id bigint NOT NULL,
    rank integer NOT NULL,
    editor boolean DEFAULT false,
    surname character varying(100) NOT NULL,
    givennames character varying(100),
    suffix character varying(100)
);

COMMENT ON TABLE pubauthor IS 'An author for a publication. Note the denormalisation (hence lack of _ in table name) - this is deliberate as it is in general too hard to assign IDs to authors.';

COMMENT ON COLUMN pubauthor.rank IS 'Order of author in author list for this pub - order is important.';

COMMENT ON COLUMN pubauthor.editor IS 'Indicates whether the author is an editor for linked publication. Note: this is a boolean field but does not follow the normal chado convention for naming booleans.';

COMMENT ON COLUMN pubauthor.givennames IS 'First name, initials';

COMMENT ON COLUMN pubauthor.suffix IS 'Jr., Sr., etc';

CREATE TABLE pubauthor_contact (
    pubauthor_contact_id bigint NOT NULL,
    contact_id bigint NOT NULL,
    pubauthor_id bigint NOT NULL
);

COMMENT ON TABLE pubauthor_contact IS 'An author on a publication may have a corresponding entry in the contact table and this table can link the two.';

CREATE SEQUENCE pubauthor_contact_pubauthor_contact_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE pubauthor_contact_pubauthor_contact_id_seq OWNED BY pubauthor_contact.pubauthor_contact_id;

CREATE SEQUENCE pubauthor_pubauthor_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE pubauthor_pubauthor_id_seq OWNED BY pubauthor.pubauthor_id;

CREATE TABLE pubprop (
    pubprop_id bigint NOT NULL,
    pub_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text NOT NULL,
    rank integer
);

COMMENT ON TABLE pubprop IS 'Property-value pairs for a pub. Follows standard chado pattern.';

CREATE SEQUENCE pubprop_pubprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE pubprop_pubprop_id_seq OWNED BY pubprop.pubprop_id;

CREATE TABLE quantification (
    quantification_id bigint NOT NULL,
    acquisition_id bigint NOT NULL,
    operator_id bigint,
    protocol_id bigint,
    analysis_id bigint NOT NULL,
    quantificationdate timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    name text,
    uri text
);

COMMENT ON TABLE quantification IS 'Quantification is the transformation of an image acquisition to numeric data. This typically involves statistical procedures.';

CREATE SEQUENCE quantification_quantification_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE quantification_quantification_id_seq OWNED BY quantification.quantification_id;

CREATE TABLE quantification_relationship (
    quantification_relationship_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    type_id bigint NOT NULL,
    object_id bigint NOT NULL
);

COMMENT ON TABLE quantification_relationship IS 'There may be multiple rounds of quantification, this allows us to keep an audit trail of what values went where.';

CREATE SEQUENCE quantification_relationship_quantification_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE quantification_relationship_quantification_relationship_id_seq OWNED BY quantification_relationship.quantification_relationship_id;

CREATE TABLE quantificationprop (
    quantificationprop_id bigint NOT NULL,
    quantification_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE quantificationprop IS 'Extra quantification properties that are not accounted for in quantification.';

CREATE SEQUENCE quantificationprop_quantificationprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE quantificationprop_quantificationprop_id_seq OWNED BY quantificationprop.quantificationprop_id;

CREATE VIEW stats_paths_to_root AS
 SELECT cvtermpath.subject_id AS cvterm_id,
    count(DISTINCT cvtermpath.cvtermpath_id) AS total_paths,
    avg(cvtermpath.pathdistance) AS avg_distance,
    min(cvtermpath.pathdistance) AS min_distance,
    max(cvtermpath.pathdistance) AS max_distance
   FROM (cvtermpath
     JOIN cv_root ON ((cvtermpath.object_id = cv_root.root_cvterm_id)))
  GROUP BY cvtermpath.subject_id;

COMMENT ON VIEW stats_paths_to_root IS 'per-cvterm statistics on its
placement in the DAG relative to the root. There may be multiple paths
from any term to the root. This gives the total number of paths, and
the average minimum and maximum distances. Here distance is defined by
cvtermpath.pathdistance';

CREATE TABLE stock (
    stock_id bigint NOT NULL,
    dbxref_id bigint,
    organism_id bigint,
    name character varying(255),
    uniquename text NOT NULL,
    description text,
    type_id bigint NOT NULL,
    is_obsolete boolean DEFAULT false NOT NULL
);

COMMENT ON TABLE stock IS 'Any stock can be globally identified by the
combination of organism, uniquename and stock type. A stock is the physical entities, either living or preserved, held by collections. Stocks belong to a collection; they have IDs, type, organism, description and may have a genotype.';

COMMENT ON COLUMN stock.dbxref_id IS 'The dbxref_id is an optional primary stable identifier for this stock. Secondary indentifiers and external dbxrefs go in table: stock_dbxref.';

COMMENT ON COLUMN stock.organism_id IS 'The organism_id is the organism to which the stock belongs. This column should only be left blank if the organism cannot be determined.';

COMMENT ON COLUMN stock.name IS 'The name is a human-readable local name for a stock.';

COMMENT ON COLUMN stock.description IS 'The description is the genetic description provided in the stock list.';

COMMENT ON COLUMN stock.type_id IS 'The type_id foreign key links to a controlled vocabulary of stock types. The would include living stock, genomic DNA, preserved specimen. Secondary cvterms for stocks would go in stock_cvterm.';

CREATE TABLE stock_cvterm (
    stock_cvterm_id bigint NOT NULL,
    stock_id bigint NOT NULL,
    cvterm_id bigint NOT NULL,
    pub_id bigint NOT NULL,
    is_not boolean DEFAULT false NOT NULL,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE stock_cvterm IS 'stock_cvterm links a stock to cvterms. This is for secondary cvterms; primary cvterms should use stock.type_id.';

CREATE SEQUENCE stock_cvterm_stock_cvterm_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stock_cvterm_stock_cvterm_id_seq OWNED BY stock_cvterm.stock_cvterm_id;

CREATE TABLE stock_cvtermprop (
    stock_cvtermprop_id bigint NOT NULL,
    stock_cvterm_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

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

CREATE SEQUENCE stock_cvtermprop_stock_cvtermprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stock_cvtermprop_stock_cvtermprop_id_seq OWNED BY stock_cvtermprop.stock_cvtermprop_id;

CREATE TABLE stock_dbxref (
    stock_dbxref_id bigint NOT NULL,
    stock_id bigint NOT NULL,
    dbxref_id bigint NOT NULL,
    is_current boolean DEFAULT true NOT NULL
);

COMMENT ON TABLE stock_dbxref IS 'stock_dbxref links a stock to dbxrefs. This is for secondary identifiers; primary identifiers should use stock.dbxref_id.';

COMMENT ON COLUMN stock_dbxref.is_current IS 'The is_current boolean indicates whether the linked dbxref is the current -official- dbxref for the linked stock.';

CREATE SEQUENCE stock_dbxref_stock_dbxref_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stock_dbxref_stock_dbxref_id_seq OWNED BY stock_dbxref.stock_dbxref_id;

CREATE TABLE stock_dbxrefprop (
    stock_dbxrefprop_id bigint NOT NULL,
    stock_dbxref_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE stock_dbxrefprop IS 'A stock_dbxref can have any number of
slot-value property tags attached to it. This is useful for storing properties related to dbxref annotations of stocks, such as evidence codes, and references, and metadata, such as create/modify dates. This is an alternative to
hardcoding a list of columns in the relational schema, and is
completely extensible. There is a unique constraint, stock_dbxrefprop_c1, for
the combination of stock_dbxref_id, rank, and type_id. Multivalued property-value pairs must be differentiated by rank.';

CREATE SEQUENCE stock_dbxrefprop_stock_dbxrefprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stock_dbxrefprop_stock_dbxrefprop_id_seq OWNED BY stock_dbxrefprop.stock_dbxrefprop_id;

CREATE TABLE stock_feature (
    stock_feature_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    stock_id bigint NOT NULL,
    type_id bigint NOT NULL,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE stock_feature IS 'Links a stock to a feature.';

CREATE SEQUENCE stock_feature_stock_feature_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stock_feature_stock_feature_id_seq OWNED BY stock_feature.stock_feature_id;

CREATE TABLE stock_featuremap (
    stock_featuremap_id bigint NOT NULL,
    featuremap_id bigint NOT NULL,
    stock_id bigint NOT NULL,
    type_id bigint
);

COMMENT ON TABLE stock_featuremap IS 'Links a featuremap to a stock.';

CREATE SEQUENCE stock_featuremap_stock_featuremap_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stock_featuremap_stock_featuremap_id_seq OWNED BY stock_featuremap.stock_featuremap_id;

CREATE TABLE stock_genotype (
    stock_genotype_id bigint NOT NULL,
    stock_id bigint NOT NULL,
    genotype_id bigint NOT NULL
);

COMMENT ON TABLE stock_genotype IS 'Simple table linking a stock to
a genotype. Features with genotypes can be linked to stocks thru feature_genotype -> genotype -> stock_genotype -> stock.';

CREATE SEQUENCE stock_genotype_stock_genotype_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stock_genotype_stock_genotype_id_seq OWNED BY stock_genotype.stock_genotype_id;

CREATE TABLE stock_library (
    stock_library_id bigint NOT NULL,
    library_id bigint NOT NULL,
    stock_id bigint NOT NULL
);

COMMENT ON TABLE stock_library IS 'Links a stock with a library.';

CREATE SEQUENCE stock_library_stock_library_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stock_library_stock_library_id_seq OWNED BY stock_library.stock_library_id;

CREATE TABLE stock_pub (
    stock_pub_id bigint NOT NULL,
    stock_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE stock_pub IS 'Provenance. Linking table between stocks and, for example, a stocklist computer file.';

CREATE SEQUENCE stock_pub_stock_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stock_pub_stock_pub_id_seq OWNED BY stock_pub.stock_pub_id;

CREATE TABLE stock_relationship (
    stock_relationship_id bigint NOT NULL,
    subject_id bigint NOT NULL,
    object_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON COLUMN stock_relationship.subject_id IS 'stock_relationship.subject_id is the subject of the subj-predicate-obj sentence. This is typically the substock.';

COMMENT ON COLUMN stock_relationship.object_id IS 'stock_relationship.object_id is the object of the subj-predicate-obj sentence. This is typically the container stock.';

COMMENT ON COLUMN stock_relationship.type_id IS 'stock_relationship.type_id is relationship type between subject and object. This is a cvterm, typically from the OBO relationship ontology, although other relationship types are allowed.';

COMMENT ON COLUMN stock_relationship.value IS 'stock_relationship.value is for additional notes or comments.';

COMMENT ON COLUMN stock_relationship.rank IS 'stock_relationship.rank is the ordering of subject stocks with respect to the object stock may be important where rank is used to order these; starts from zero.';

CREATE TABLE stock_relationship_cvterm (
    stock_relationship_cvterm_id bigint NOT NULL,
    stock_relationship_id bigint NOT NULL,
    cvterm_id bigint NOT NULL,
    pub_id bigint
);

COMMENT ON TABLE stock_relationship_cvterm IS 'For germplasm maintenance and pedigree data, stock_relationship. type_id will record cvterms such as "is a female parent of", "a parent for mutation", "is a group_id of", "is a source_id of", etc The cvterms for higher categories such as "generative", "derivative" or "maintenance" can be stored in table stock_relationship_cvterm';

CREATE SEQUENCE stock_relationship_cvterm_stock_relationship_cvterm_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stock_relationship_cvterm_stock_relationship_cvterm_id_seq OWNED BY stock_relationship_cvterm.stock_relationship_cvterm_id;

CREATE TABLE stock_relationship_pub (
    stock_relationship_pub_id bigint NOT NULL,
    stock_relationship_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE stock_relationship_pub IS 'Provenance. Attach optional evidence to a stock_relationship in the form of a publication.';

CREATE SEQUENCE stock_relationship_pub_stock_relationship_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stock_relationship_pub_stock_relationship_pub_id_seq OWNED BY stock_relationship_pub.stock_relationship_pub_id;

CREATE SEQUENCE stock_relationship_stock_relationship_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stock_relationship_stock_relationship_id_seq OWNED BY stock_relationship.stock_relationship_id;

CREATE SEQUENCE stock_stock_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stock_stock_id_seq OWNED BY stock.stock_id;

CREATE TABLE stockcollection (
    stockcollection_id bigint NOT NULL,
    type_id bigint NOT NULL,
    contact_id bigint,
    name character varying(255),
    uniquename text NOT NULL
);

COMMENT ON TABLE stockcollection IS 'The lab or stock center distributing the stocks in their collection.';

COMMENT ON COLUMN stockcollection.type_id IS 'type_id is the collection type cv.';

COMMENT ON COLUMN stockcollection.contact_id IS 'contact_id links to the contact information for the collection.';

COMMENT ON COLUMN stockcollection.name IS 'name is the collection.';

COMMENT ON COLUMN stockcollection.uniquename IS 'uniqename is the value of the collection cv.';

CREATE TABLE stockcollection_db (
    stockcollection_db_id bigint NOT NULL,
    stockcollection_id bigint NOT NULL,
    db_id bigint NOT NULL
);

COMMENT ON TABLE stockcollection_db IS 'Stock collections may be respresented
by an external online database. This table associates a stock collection with
a database where its member stocks can be found. Individual stock that are part
of this collction should have entries in the stock_dbxref table with the same
db_id record';

CREATE SEQUENCE stockcollection_db_stockcollection_db_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stockcollection_db_stockcollection_db_id_seq OWNED BY stockcollection_db.stockcollection_db_id;

CREATE TABLE stockcollection_stock (
    stockcollection_stock_id bigint NOT NULL,
    stockcollection_id bigint NOT NULL,
    stock_id bigint NOT NULL
);

COMMENT ON TABLE stockcollection_stock IS 'stockcollection_stock links
a stock collection to the stocks which are contained in the collection.';

CREATE SEQUENCE stockcollection_stock_stockcollection_stock_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stockcollection_stock_stockcollection_stock_id_seq OWNED BY stockcollection_stock.stockcollection_stock_id;

CREATE SEQUENCE stockcollection_stockcollection_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stockcollection_stockcollection_id_seq OWNED BY stockcollection.stockcollection_id;

CREATE TABLE stockcollectionprop (
    stockcollectionprop_id bigint NOT NULL,
    stockcollection_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE stockcollectionprop IS 'The table stockcollectionprop
contains the value of the stock collection such as website/email URLs;
the value of the stock collection order URLs.';

COMMENT ON COLUMN stockcollectionprop.type_id IS 'The cv for the type_id is "stockcollection property type".';

CREATE SEQUENCE stockcollectionprop_stockcollectionprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stockcollectionprop_stockcollectionprop_id_seq OWNED BY stockcollectionprop.stockcollectionprop_id;

CREATE TABLE stockprop (
    stockprop_id bigint NOT NULL,
    stock_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

COMMENT ON TABLE stockprop IS 'A stock can have any number of
slot-value property tags attached to it. This is an alternative to
hardcoding a list of columns in the relational schema, and is
completely extensible. There is a unique constraint, stockprop_c1, for
the combination of stock_id, rank, and type_id. Multivalued property-value pairs must be differentiated by rank.';

CREATE TABLE stockprop_pub (
    stockprop_pub_id bigint NOT NULL,
    stockprop_id bigint NOT NULL,
    pub_id bigint NOT NULL
);

COMMENT ON TABLE stockprop_pub IS 'Provenance. Any stockprop assignment can optionally be supported by a publication.';

CREATE SEQUENCE stockprop_pub_stockprop_pub_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stockprop_pub_stockprop_pub_id_seq OWNED BY stockprop_pub.stockprop_pub_id;

CREATE SEQUENCE stockprop_stockprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE stockprop_stockprop_id_seq OWNED BY stockprop.stockprop_id;

CREATE TABLE study (
    study_id bigint NOT NULL,
    contact_id bigint NOT NULL,
    pub_id bigint,
    dbxref_id bigint,
    name text NOT NULL,
    description text
);

CREATE TABLE study_assay (
    study_assay_id bigint NOT NULL,
    study_id bigint NOT NULL,
    assay_id bigint NOT NULL
);

CREATE SEQUENCE study_assay_study_assay_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE study_assay_study_assay_id_seq OWNED BY study_assay.study_assay_id;

CREATE SEQUENCE study_study_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE study_study_id_seq OWNED BY study.study_id;

CREATE TABLE studydesign (
    studydesign_id bigint NOT NULL,
    study_id bigint NOT NULL,
    description text
);

CREATE SEQUENCE studydesign_studydesign_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE studydesign_studydesign_id_seq OWNED BY studydesign.studydesign_id;

CREATE TABLE studydesignprop (
    studydesignprop_id bigint NOT NULL,
    studydesign_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

CREATE SEQUENCE studydesignprop_studydesignprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE studydesignprop_studydesignprop_id_seq OWNED BY studydesignprop.studydesignprop_id;

CREATE TABLE studyfactor (
    studyfactor_id bigint NOT NULL,
    studydesign_id bigint NOT NULL,
    type_id bigint,
    name text NOT NULL,
    description text
);

CREATE SEQUENCE studyfactor_studyfactor_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE studyfactor_studyfactor_id_seq OWNED BY studyfactor.studyfactor_id;

CREATE TABLE studyfactorvalue (
    studyfactorvalue_id bigint NOT NULL,
    studyfactor_id bigint NOT NULL,
    assay_id bigint NOT NULL,
    factorvalue text,
    name text,
    rank integer DEFAULT 0 NOT NULL
);

CREATE SEQUENCE studyfactorvalue_studyfactorvalue_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE studyfactorvalue_studyfactorvalue_id_seq OWNED BY studyfactorvalue.studyfactorvalue_id;

CREATE TABLE studyprop (
    studyprop_id bigint NOT NULL,
    study_id bigint NOT NULL,
    type_id bigint NOT NULL,
    value text,
    rank integer DEFAULT 0 NOT NULL
);

CREATE TABLE studyprop_feature (
    studyprop_feature_id bigint NOT NULL,
    studyprop_id bigint NOT NULL,
    feature_id bigint NOT NULL,
    type_id bigint
);

CREATE SEQUENCE studyprop_feature_studyprop_feature_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE studyprop_feature_studyprop_feature_id_seq OWNED BY studyprop_feature.studyprop_feature_id;

CREATE SEQUENCE studyprop_studyprop_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE studyprop_studyprop_id_seq OWNED BY studyprop.studyprop_id;

CREATE SEQUENCE synonym_synonym_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE synonym_synonym_id_seq OWNED BY synonym.synonym_id;

CREATE TABLE tableinfo (
    tableinfo_id bigint NOT NULL,
    name character varying(30) NOT NULL,
    primary_key_column character varying(30),
    is_view integer DEFAULT 0 NOT NULL,
    view_on_table_id bigint,
    superclass_table_id bigint,
    is_updateable integer DEFAULT 1 NOT NULL,
    modification_date date DEFAULT now() NOT NULL
);

CREATE SEQUENCE tableinfo_tableinfo_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE tableinfo_tableinfo_id_seq OWNED BY tableinfo.tableinfo_id;

CREATE TABLE treatment (
    treatment_id bigint NOT NULL,
    rank integer DEFAULT 0 NOT NULL,
    biomaterial_id bigint NOT NULL,
    type_id bigint NOT NULL,
    protocol_id bigint,
    name text
);

COMMENT ON TABLE treatment IS 'A biomaterial may undergo multiple
treatments. Examples of treatments: apoxia, fluorophore and biotin labeling.';

CREATE SEQUENCE treatment_treatment_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER SEQUENCE treatment_treatment_id_seq OWNED BY treatment.treatment_id;

CREATE VIEW type_feature_count AS
 SELECT t.name AS type,
    count(*) AS num_features
   FROM (cvterm t
     JOIN feature ON ((feature.type_id = t.cvterm_id)))
  GROUP BY t.name;

COMMENT ON VIEW type_feature_count IS 'per-feature-type feature counts';

ALTER TABLE ONLY acquisition ALTER COLUMN acquisition_id SET DEFAULT nextval('acquisition_acquisition_id_seq'::regclass);

ALTER TABLE ONLY acquisition_relationship ALTER COLUMN acquisition_relationship_id SET DEFAULT nextval('acquisition_relationship_acquisition_relationship_id_seq'::regclass);

ALTER TABLE ONLY acquisitionprop ALTER COLUMN acquisitionprop_id SET DEFAULT nextval('acquisitionprop_acquisitionprop_id_seq'::regclass);

ALTER TABLE ONLY analysis ALTER COLUMN analysis_id SET DEFAULT nextval('analysis_analysis_id_seq'::regclass);

ALTER TABLE ONLY analysis_cvterm ALTER COLUMN analysis_cvterm_id SET DEFAULT nextval('analysis_cvterm_analysis_cvterm_id_seq'::regclass);

ALTER TABLE ONLY analysis_dbxref ALTER COLUMN analysis_dbxref_id SET DEFAULT nextval('analysis_dbxref_analysis_dbxref_id_seq'::regclass);

ALTER TABLE ONLY analysis_pub ALTER COLUMN analysis_pub_id SET DEFAULT nextval('analysis_pub_analysis_pub_id_seq'::regclass);

ALTER TABLE ONLY analysis_relationship ALTER COLUMN analysis_relationship_id SET DEFAULT nextval('analysis_relationship_analysis_relationship_id_seq'::regclass);

ALTER TABLE ONLY analysisfeature ALTER COLUMN analysisfeature_id SET DEFAULT nextval('analysisfeature_analysisfeature_id_seq'::regclass);

ALTER TABLE ONLY analysisfeatureprop ALTER COLUMN analysisfeatureprop_id SET DEFAULT nextval('analysisfeatureprop_analysisfeatureprop_id_seq'::regclass);

ALTER TABLE ONLY analysisprop ALTER COLUMN analysisprop_id SET DEFAULT nextval('analysisprop_analysisprop_id_seq'::regclass);

ALTER TABLE ONLY arraydesign ALTER COLUMN arraydesign_id SET DEFAULT nextval('arraydesign_arraydesign_id_seq'::regclass);

ALTER TABLE ONLY arraydesignprop ALTER COLUMN arraydesignprop_id SET DEFAULT nextval('arraydesignprop_arraydesignprop_id_seq'::regclass);

ALTER TABLE ONLY assay ALTER COLUMN assay_id SET DEFAULT nextval('assay_assay_id_seq'::regclass);

ALTER TABLE ONLY assay_biomaterial ALTER COLUMN assay_biomaterial_id SET DEFAULT nextval('assay_biomaterial_assay_biomaterial_id_seq'::regclass);

ALTER TABLE ONLY assay_project ALTER COLUMN assay_project_id SET DEFAULT nextval('assay_project_assay_project_id_seq'::regclass);

ALTER TABLE ONLY assayprop ALTER COLUMN assayprop_id SET DEFAULT nextval('assayprop_assayprop_id_seq'::regclass);

ALTER TABLE ONLY biomaterial ALTER COLUMN biomaterial_id SET DEFAULT nextval('biomaterial_biomaterial_id_seq'::regclass);

ALTER TABLE ONLY biomaterial_dbxref ALTER COLUMN biomaterial_dbxref_id SET DEFAULT nextval('biomaterial_dbxref_biomaterial_dbxref_id_seq'::regclass);

ALTER TABLE ONLY biomaterial_relationship ALTER COLUMN biomaterial_relationship_id SET DEFAULT nextval('biomaterial_relationship_biomaterial_relationship_id_seq'::regclass);

ALTER TABLE ONLY biomaterial_treatment ALTER COLUMN biomaterial_treatment_id SET DEFAULT nextval('biomaterial_treatment_biomaterial_treatment_id_seq'::regclass);

ALTER TABLE ONLY biomaterialprop ALTER COLUMN biomaterialprop_id SET DEFAULT nextval('biomaterialprop_biomaterialprop_id_seq'::regclass);

ALTER TABLE ONLY cell_line ALTER COLUMN cell_line_id SET DEFAULT nextval('cell_line_cell_line_id_seq'::regclass);

ALTER TABLE ONLY cell_line_cvterm ALTER COLUMN cell_line_cvterm_id SET DEFAULT nextval('cell_line_cvterm_cell_line_cvterm_id_seq'::regclass);

ALTER TABLE ONLY cell_line_cvtermprop ALTER COLUMN cell_line_cvtermprop_id SET DEFAULT nextval('cell_line_cvtermprop_cell_line_cvtermprop_id_seq'::regclass);

ALTER TABLE ONLY cell_line_dbxref ALTER COLUMN cell_line_dbxref_id SET DEFAULT nextval('cell_line_dbxref_cell_line_dbxref_id_seq'::regclass);

ALTER TABLE ONLY cell_line_feature ALTER COLUMN cell_line_feature_id SET DEFAULT nextval('cell_line_feature_cell_line_feature_id_seq'::regclass);

ALTER TABLE ONLY cell_line_library ALTER COLUMN cell_line_library_id SET DEFAULT nextval('cell_line_library_cell_line_library_id_seq'::regclass);

ALTER TABLE ONLY cell_line_pub ALTER COLUMN cell_line_pub_id SET DEFAULT nextval('cell_line_pub_cell_line_pub_id_seq'::regclass);

ALTER TABLE ONLY cell_line_relationship ALTER COLUMN cell_line_relationship_id SET DEFAULT nextval('cell_line_relationship_cell_line_relationship_id_seq'::regclass);

ALTER TABLE ONLY cell_line_synonym ALTER COLUMN cell_line_synonym_id SET DEFAULT nextval('cell_line_synonym_cell_line_synonym_id_seq'::regclass);

ALTER TABLE ONLY cell_lineprop ALTER COLUMN cell_lineprop_id SET DEFAULT nextval('cell_lineprop_cell_lineprop_id_seq'::regclass);

ALTER TABLE ONLY cell_lineprop_pub ALTER COLUMN cell_lineprop_pub_id SET DEFAULT nextval('cell_lineprop_pub_cell_lineprop_pub_id_seq'::regclass);

ALTER TABLE ONLY chadoprop ALTER COLUMN chadoprop_id SET DEFAULT nextval('chadoprop_chadoprop_id_seq'::regclass);

ALTER TABLE ONLY channel ALTER COLUMN channel_id SET DEFAULT nextval('channel_channel_id_seq'::regclass);

ALTER TABLE ONLY contact ALTER COLUMN contact_id SET DEFAULT nextval('contact_contact_id_seq'::regclass);

ALTER TABLE ONLY contact_relationship ALTER COLUMN contact_relationship_id SET DEFAULT nextval('contact_relationship_contact_relationship_id_seq'::regclass);

ALTER TABLE ONLY contactprop ALTER COLUMN contactprop_id SET DEFAULT nextval('contactprop_contactprop_id_seq'::regclass);

ALTER TABLE ONLY control ALTER COLUMN control_id SET DEFAULT nextval('control_control_id_seq'::regclass);

ALTER TABLE ONLY cv ALTER COLUMN cv_id SET DEFAULT nextval('cv_cv_id_seq'::regclass);

ALTER TABLE ONLY cvprop ALTER COLUMN cvprop_id SET DEFAULT nextval('cvprop_cvprop_id_seq'::regclass);

ALTER TABLE ONLY cvterm ALTER COLUMN cvterm_id SET DEFAULT nextval('cvterm_cvterm_id_seq'::regclass);

ALTER TABLE ONLY cvterm_dbxref ALTER COLUMN cvterm_dbxref_id SET DEFAULT nextval('cvterm_dbxref_cvterm_dbxref_id_seq'::regclass);

ALTER TABLE ONLY cvterm_relationship ALTER COLUMN cvterm_relationship_id SET DEFAULT nextval('cvterm_relationship_cvterm_relationship_id_seq'::regclass);

ALTER TABLE ONLY cvtermpath ALTER COLUMN cvtermpath_id SET DEFAULT nextval('cvtermpath_cvtermpath_id_seq'::regclass);

ALTER TABLE ONLY cvtermprop ALTER COLUMN cvtermprop_id SET DEFAULT nextval('cvtermprop_cvtermprop_id_seq'::regclass);

ALTER TABLE ONLY cvtermsynonym ALTER COLUMN cvtermsynonym_id SET DEFAULT nextval('cvtermsynonym_cvtermsynonym_id_seq'::regclass);

ALTER TABLE ONLY db ALTER COLUMN db_id SET DEFAULT nextval('db_db_id_seq'::regclass);

ALTER TABLE ONLY dbprop ALTER COLUMN dbprop_id SET DEFAULT nextval('dbprop_dbprop_id_seq'::regclass);

ALTER TABLE ONLY dbxref ALTER COLUMN dbxref_id SET DEFAULT nextval('dbxref_dbxref_id_seq'::regclass);

ALTER TABLE ONLY dbxrefprop ALTER COLUMN dbxrefprop_id SET DEFAULT nextval('dbxrefprop_dbxrefprop_id_seq'::regclass);

ALTER TABLE ONLY eimage ALTER COLUMN eimage_id SET DEFAULT nextval('eimage_eimage_id_seq'::regclass);

ALTER TABLE ONLY element ALTER COLUMN element_id SET DEFAULT nextval('element_element_id_seq'::regclass);

ALTER TABLE ONLY element_relationship ALTER COLUMN element_relationship_id SET DEFAULT nextval('element_relationship_element_relationship_id_seq'::regclass);

ALTER TABLE ONLY elementresult ALTER COLUMN elementresult_id SET DEFAULT nextval('elementresult_elementresult_id_seq'::regclass);

ALTER TABLE ONLY elementresult_relationship ALTER COLUMN elementresult_relationship_id SET DEFAULT nextval('elementresult_relationship_elementresult_relationship_id_seq'::regclass);

ALTER TABLE ONLY environment ALTER COLUMN environment_id SET DEFAULT nextval('environment_environment_id_seq'::regclass);

ALTER TABLE ONLY environment_cvterm ALTER COLUMN environment_cvterm_id SET DEFAULT nextval('environment_cvterm_environment_cvterm_id_seq'::regclass);

ALTER TABLE ONLY expression ALTER COLUMN expression_id SET DEFAULT nextval('expression_expression_id_seq'::regclass);

ALTER TABLE ONLY expression_cvterm ALTER COLUMN expression_cvterm_id SET DEFAULT nextval('expression_cvterm_expression_cvterm_id_seq'::regclass);

ALTER TABLE ONLY expression_cvtermprop ALTER COLUMN expression_cvtermprop_id SET DEFAULT nextval('expression_cvtermprop_expression_cvtermprop_id_seq'::regclass);

ALTER TABLE ONLY expression_image ALTER COLUMN expression_image_id SET DEFAULT nextval('expression_image_expression_image_id_seq'::regclass);

ALTER TABLE ONLY expression_pub ALTER COLUMN expression_pub_id SET DEFAULT nextval('expression_pub_expression_pub_id_seq'::regclass);

ALTER TABLE ONLY expressionprop ALTER COLUMN expressionprop_id SET DEFAULT nextval('expressionprop_expressionprop_id_seq'::regclass);

ALTER TABLE ONLY feature ALTER COLUMN feature_id SET DEFAULT nextval('feature_feature_id_seq'::regclass);

ALTER TABLE ONLY feature_contact ALTER COLUMN feature_contact_id SET DEFAULT nextval('feature_contact_feature_contact_id_seq'::regclass);

ALTER TABLE ONLY feature_cvterm ALTER COLUMN feature_cvterm_id SET DEFAULT nextval('feature_cvterm_feature_cvterm_id_seq'::regclass);

ALTER TABLE ONLY feature_cvterm_dbxref ALTER COLUMN feature_cvterm_dbxref_id SET DEFAULT nextval('feature_cvterm_dbxref_feature_cvterm_dbxref_id_seq'::regclass);

ALTER TABLE ONLY feature_cvterm_pub ALTER COLUMN feature_cvterm_pub_id SET DEFAULT nextval('feature_cvterm_pub_feature_cvterm_pub_id_seq'::regclass);

ALTER TABLE ONLY feature_cvtermprop ALTER COLUMN feature_cvtermprop_id SET DEFAULT nextval('feature_cvtermprop_feature_cvtermprop_id_seq'::regclass);

ALTER TABLE ONLY feature_dbxref ALTER COLUMN feature_dbxref_id SET DEFAULT nextval('feature_dbxref_feature_dbxref_id_seq'::regclass);

ALTER TABLE ONLY feature_expression ALTER COLUMN feature_expression_id SET DEFAULT nextval('feature_expression_feature_expression_id_seq'::regclass);

ALTER TABLE ONLY feature_expressionprop ALTER COLUMN feature_expressionprop_id SET DEFAULT nextval('feature_expressionprop_feature_expressionprop_id_seq'::regclass);

ALTER TABLE ONLY feature_genotype ALTER COLUMN feature_genotype_id SET DEFAULT nextval('feature_genotype_feature_genotype_id_seq'::regclass);

ALTER TABLE ONLY feature_phenotype ALTER COLUMN feature_phenotype_id SET DEFAULT nextval('feature_phenotype_feature_phenotype_id_seq'::regclass);

ALTER TABLE ONLY feature_pub ALTER COLUMN feature_pub_id SET DEFAULT nextval('feature_pub_feature_pub_id_seq'::regclass);

ALTER TABLE ONLY feature_pubprop ALTER COLUMN feature_pubprop_id SET DEFAULT nextval('feature_pubprop_feature_pubprop_id_seq'::regclass);

ALTER TABLE ONLY feature_relationship ALTER COLUMN feature_relationship_id SET DEFAULT nextval('feature_relationship_feature_relationship_id_seq'::regclass);

ALTER TABLE ONLY feature_relationship_pub ALTER COLUMN feature_relationship_pub_id SET DEFAULT nextval('feature_relationship_pub_feature_relationship_pub_id_seq'::regclass);

ALTER TABLE ONLY feature_relationshipprop ALTER COLUMN feature_relationshipprop_id SET DEFAULT nextval('feature_relationshipprop_feature_relationshipprop_id_seq'::regclass);

ALTER TABLE ONLY feature_relationshipprop_pub ALTER COLUMN feature_relationshipprop_pub_id SET DEFAULT nextval('feature_relationshipprop_pub_feature_relationshipprop_pub_i_seq'::regclass);

ALTER TABLE ONLY feature_synonym ALTER COLUMN feature_synonym_id SET DEFAULT nextval('feature_synonym_feature_synonym_id_seq'::regclass);

ALTER TABLE ONLY featureloc ALTER COLUMN featureloc_id SET DEFAULT nextval('featureloc_featureloc_id_seq'::regclass);

ALTER TABLE ONLY featureloc_pub ALTER COLUMN featureloc_pub_id SET DEFAULT nextval('featureloc_pub_featureloc_pub_id_seq'::regclass);

ALTER TABLE ONLY featuremap ALTER COLUMN featuremap_id SET DEFAULT nextval('featuremap_featuremap_id_seq'::regclass);

ALTER TABLE ONLY featuremap_contact ALTER COLUMN featuremap_contact_id SET DEFAULT nextval('featuremap_contact_featuremap_contact_id_seq'::regclass);

ALTER TABLE ONLY featuremap_dbxref ALTER COLUMN featuremap_dbxref_id SET DEFAULT nextval('featuremap_dbxref_featuremap_dbxref_id_seq'::regclass);

ALTER TABLE ONLY featuremap_organism ALTER COLUMN featuremap_organism_id SET DEFAULT nextval('featuremap_organism_featuremap_organism_id_seq'::regclass);

ALTER TABLE ONLY featuremap_pub ALTER COLUMN featuremap_pub_id SET DEFAULT nextval('featuremap_pub_featuremap_pub_id_seq'::regclass);

ALTER TABLE ONLY featuremapprop ALTER COLUMN featuremapprop_id SET DEFAULT nextval('featuremapprop_featuremapprop_id_seq'::regclass);

ALTER TABLE ONLY featurepos ALTER COLUMN featurepos_id SET DEFAULT nextval('featurepos_featurepos_id_seq'::regclass);

ALTER TABLE ONLY featureposprop ALTER COLUMN featureposprop_id SET DEFAULT nextval('featureposprop_featureposprop_id_seq'::regclass);

ALTER TABLE ONLY featureprop ALTER COLUMN featureprop_id SET DEFAULT nextval('featureprop_featureprop_id_seq'::regclass);

ALTER TABLE ONLY featureprop_pub ALTER COLUMN featureprop_pub_id SET DEFAULT nextval('featureprop_pub_featureprop_pub_id_seq'::regclass);

ALTER TABLE ONLY featurerange ALTER COLUMN featurerange_id SET DEFAULT nextval('featurerange_featurerange_id_seq'::regclass);

ALTER TABLE ONLY genotype ALTER COLUMN genotype_id SET DEFAULT nextval('genotype_genotype_id_seq'::regclass);

ALTER TABLE ONLY genotypeprop ALTER COLUMN genotypeprop_id SET DEFAULT nextval('genotypeprop_genotypeprop_id_seq'::regclass);

ALTER TABLE ONLY library ALTER COLUMN library_id SET DEFAULT nextval('library_library_id_seq'::regclass);

ALTER TABLE ONLY library_contact ALTER COLUMN library_contact_id SET DEFAULT nextval('library_contact_library_contact_id_seq'::regclass);

ALTER TABLE ONLY library_cvterm ALTER COLUMN library_cvterm_id SET DEFAULT nextval('library_cvterm_library_cvterm_id_seq'::regclass);

ALTER TABLE ONLY library_dbxref ALTER COLUMN library_dbxref_id SET DEFAULT nextval('library_dbxref_library_dbxref_id_seq'::regclass);

ALTER TABLE ONLY library_expression ALTER COLUMN library_expression_id SET DEFAULT nextval('library_expression_library_expression_id_seq'::regclass);

ALTER TABLE ONLY library_expressionprop ALTER COLUMN library_expressionprop_id SET DEFAULT nextval('library_expressionprop_library_expressionprop_id_seq'::regclass);

ALTER TABLE ONLY library_feature ALTER COLUMN library_feature_id SET DEFAULT nextval('library_feature_library_feature_id_seq'::regclass);

ALTER TABLE ONLY library_featureprop ALTER COLUMN library_featureprop_id SET DEFAULT nextval('library_featureprop_library_featureprop_id_seq'::regclass);

ALTER TABLE ONLY library_pub ALTER COLUMN library_pub_id SET DEFAULT nextval('library_pub_library_pub_id_seq'::regclass);

ALTER TABLE ONLY library_relationship ALTER COLUMN library_relationship_id SET DEFAULT nextval('library_relationship_library_relationship_id_seq'::regclass);

ALTER TABLE ONLY library_relationship_pub ALTER COLUMN library_relationship_pub_id SET DEFAULT nextval('library_relationship_pub_library_relationship_pub_id_seq'::regclass);

ALTER TABLE ONLY library_synonym ALTER COLUMN library_synonym_id SET DEFAULT nextval('library_synonym_library_synonym_id_seq'::regclass);

ALTER TABLE ONLY libraryprop ALTER COLUMN libraryprop_id SET DEFAULT nextval('libraryprop_libraryprop_id_seq'::regclass);

ALTER TABLE ONLY libraryprop_pub ALTER COLUMN libraryprop_pub_id SET DEFAULT nextval('libraryprop_pub_libraryprop_pub_id_seq'::regclass);

ALTER TABLE ONLY magedocumentation ALTER COLUMN magedocumentation_id SET DEFAULT nextval('magedocumentation_magedocumentation_id_seq'::regclass);

ALTER TABLE ONLY mageml ALTER COLUMN mageml_id SET DEFAULT nextval('mageml_mageml_id_seq'::regclass);

ALTER TABLE ONLY nd_experiment ALTER COLUMN nd_experiment_id SET DEFAULT nextval('nd_experiment_nd_experiment_id_seq'::regclass);

ALTER TABLE ONLY nd_experiment_analysis ALTER COLUMN nd_experiment_analysis_id SET DEFAULT nextval('nd_experiment_analysis_nd_experiment_analysis_id_seq'::regclass);

ALTER TABLE ONLY nd_experiment_contact ALTER COLUMN nd_experiment_contact_id SET DEFAULT nextval('nd_experiment_contact_nd_experiment_contact_id_seq'::regclass);

ALTER TABLE ONLY nd_experiment_dbxref ALTER COLUMN nd_experiment_dbxref_id SET DEFAULT nextval('nd_experiment_dbxref_nd_experiment_dbxref_id_seq'::regclass);

ALTER TABLE ONLY nd_experiment_genotype ALTER COLUMN nd_experiment_genotype_id SET DEFAULT nextval('nd_experiment_genotype_nd_experiment_genotype_id_seq'::regclass);

ALTER TABLE ONLY nd_experiment_phenotype ALTER COLUMN nd_experiment_phenotype_id SET DEFAULT nextval('nd_experiment_phenotype_nd_experiment_phenotype_id_seq'::regclass);

ALTER TABLE ONLY nd_experiment_project ALTER COLUMN nd_experiment_project_id SET DEFAULT nextval('nd_experiment_project_nd_experiment_project_id_seq'::regclass);

ALTER TABLE ONLY nd_experiment_protocol ALTER COLUMN nd_experiment_protocol_id SET DEFAULT nextval('nd_experiment_protocol_nd_experiment_protocol_id_seq'::regclass);

ALTER TABLE ONLY nd_experiment_pub ALTER COLUMN nd_experiment_pub_id SET DEFAULT nextval('nd_experiment_pub_nd_experiment_pub_id_seq'::regclass);

ALTER TABLE ONLY nd_experiment_stock ALTER COLUMN nd_experiment_stock_id SET DEFAULT nextval('nd_experiment_stock_nd_experiment_stock_id_seq'::regclass);

ALTER TABLE ONLY nd_experiment_stock_dbxref ALTER COLUMN nd_experiment_stock_dbxref_id SET DEFAULT nextval('nd_experiment_stock_dbxref_nd_experiment_stock_dbxref_id_seq'::regclass);

ALTER TABLE ONLY nd_experiment_stockprop ALTER COLUMN nd_experiment_stockprop_id SET DEFAULT nextval('nd_experiment_stockprop_nd_experiment_stockprop_id_seq'::regclass);

ALTER TABLE ONLY nd_experimentprop ALTER COLUMN nd_experimentprop_id SET DEFAULT nextval('nd_experimentprop_nd_experimentprop_id_seq'::regclass);

ALTER TABLE ONLY nd_geolocation ALTER COLUMN nd_geolocation_id SET DEFAULT nextval('nd_geolocation_nd_geolocation_id_seq'::regclass);

ALTER TABLE ONLY nd_geolocationprop ALTER COLUMN nd_geolocationprop_id SET DEFAULT nextval('nd_geolocationprop_nd_geolocationprop_id_seq'::regclass);

ALTER TABLE ONLY nd_protocol ALTER COLUMN nd_protocol_id SET DEFAULT nextval('nd_protocol_nd_protocol_id_seq'::regclass);

ALTER TABLE ONLY nd_protocol_reagent ALTER COLUMN nd_protocol_reagent_id SET DEFAULT nextval('nd_protocol_reagent_nd_protocol_reagent_id_seq'::regclass);

ALTER TABLE ONLY nd_protocolprop ALTER COLUMN nd_protocolprop_id SET DEFAULT nextval('nd_protocolprop_nd_protocolprop_id_seq'::regclass);

ALTER TABLE ONLY nd_reagent ALTER COLUMN nd_reagent_id SET DEFAULT nextval('nd_reagent_nd_reagent_id_seq'::regclass);

ALTER TABLE ONLY nd_reagent_relationship ALTER COLUMN nd_reagent_relationship_id SET DEFAULT nextval('nd_reagent_relationship_nd_reagent_relationship_id_seq'::regclass);

ALTER TABLE ONLY nd_reagentprop ALTER COLUMN nd_reagentprop_id SET DEFAULT nextval('nd_reagentprop_nd_reagentprop_id_seq'::regclass);

ALTER TABLE ONLY organism ALTER COLUMN organism_id SET DEFAULT nextval('organism_organism_id_seq'::regclass);

ALTER TABLE ONLY organism_cvterm ALTER COLUMN organism_cvterm_id SET DEFAULT nextval('organism_cvterm_organism_cvterm_id_seq'::regclass);

ALTER TABLE ONLY organism_cvtermprop ALTER COLUMN organism_cvtermprop_id SET DEFAULT nextval('organism_cvtermprop_organism_cvtermprop_id_seq'::regclass);

ALTER TABLE ONLY organism_dbxref ALTER COLUMN organism_dbxref_id SET DEFAULT nextval('organism_dbxref_organism_dbxref_id_seq'::regclass);

ALTER TABLE ONLY organism_pub ALTER COLUMN organism_pub_id SET DEFAULT nextval('organism_pub_organism_pub_id_seq'::regclass);

ALTER TABLE ONLY organism_relationship ALTER COLUMN organism_relationship_id SET DEFAULT nextval('organism_relationship_organism_relationship_id_seq'::regclass);

ALTER TABLE ONLY organismprop ALTER COLUMN organismprop_id SET DEFAULT nextval('organismprop_organismprop_id_seq'::regclass);

ALTER TABLE ONLY organismprop_pub ALTER COLUMN organismprop_pub_id SET DEFAULT nextval('organismprop_pub_organismprop_pub_id_seq'::regclass);

ALTER TABLE ONLY phendesc ALTER COLUMN phendesc_id SET DEFAULT nextval('phendesc_phendesc_id_seq'::regclass);

ALTER TABLE ONLY phenotype ALTER COLUMN phenotype_id SET DEFAULT nextval('phenotype_phenotype_id_seq'::regclass);

ALTER TABLE ONLY phenotype_comparison ALTER COLUMN phenotype_comparison_id SET DEFAULT nextval('phenotype_comparison_phenotype_comparison_id_seq'::regclass);

ALTER TABLE ONLY phenotype_comparison_cvterm ALTER COLUMN phenotype_comparison_cvterm_id SET DEFAULT nextval('phenotype_comparison_cvterm_phenotype_comparison_cvterm_id_seq'::regclass);

ALTER TABLE ONLY phenotype_cvterm ALTER COLUMN phenotype_cvterm_id SET DEFAULT nextval('phenotype_cvterm_phenotype_cvterm_id_seq'::regclass);

ALTER TABLE ONLY phenotypeprop ALTER COLUMN phenotypeprop_id SET DEFAULT nextval('phenotypeprop_phenotypeprop_id_seq'::regclass);

ALTER TABLE ONLY phenstatement ALTER COLUMN phenstatement_id SET DEFAULT nextval('phenstatement_phenstatement_id_seq'::regclass);

ALTER TABLE ONLY phylonode ALTER COLUMN phylonode_id SET DEFAULT nextval('phylonode_phylonode_id_seq'::regclass);

ALTER TABLE ONLY phylonode_dbxref ALTER COLUMN phylonode_dbxref_id SET DEFAULT nextval('phylonode_dbxref_phylonode_dbxref_id_seq'::regclass);

ALTER TABLE ONLY phylonode_organism ALTER COLUMN phylonode_organism_id SET DEFAULT nextval('phylonode_organism_phylonode_organism_id_seq'::regclass);

ALTER TABLE ONLY phylonode_pub ALTER COLUMN phylonode_pub_id SET DEFAULT nextval('phylonode_pub_phylonode_pub_id_seq'::regclass);

ALTER TABLE ONLY phylonode_relationship ALTER COLUMN phylonode_relationship_id SET DEFAULT nextval('phylonode_relationship_phylonode_relationship_id_seq'::regclass);

ALTER TABLE ONLY phylonodeprop ALTER COLUMN phylonodeprop_id SET DEFAULT nextval('phylonodeprop_phylonodeprop_id_seq'::regclass);

ALTER TABLE ONLY phylotree ALTER COLUMN phylotree_id SET DEFAULT nextval('phylotree_phylotree_id_seq'::regclass);

ALTER TABLE ONLY phylotree_pub ALTER COLUMN phylotree_pub_id SET DEFAULT nextval('phylotree_pub_phylotree_pub_id_seq'::regclass);

ALTER TABLE ONLY phylotreeprop ALTER COLUMN phylotreeprop_id SET DEFAULT nextval('phylotreeprop_phylotreeprop_id_seq'::regclass);

ALTER TABLE ONLY project ALTER COLUMN project_id SET DEFAULT nextval('project_project_id_seq'::regclass);

ALTER TABLE ONLY project_analysis ALTER COLUMN project_analysis_id SET DEFAULT nextval('project_analysis_project_analysis_id_seq'::regclass);

ALTER TABLE ONLY project_contact ALTER COLUMN project_contact_id SET DEFAULT nextval('project_contact_project_contact_id_seq'::regclass);

ALTER TABLE ONLY project_dbxref ALTER COLUMN project_dbxref_id SET DEFAULT nextval('project_dbxref_project_dbxref_id_seq'::regclass);

ALTER TABLE ONLY project_feature ALTER COLUMN project_feature_id SET DEFAULT nextval('project_feature_project_feature_id_seq'::regclass);

ALTER TABLE ONLY project_pub ALTER COLUMN project_pub_id SET DEFAULT nextval('project_pub_project_pub_id_seq'::regclass);

ALTER TABLE ONLY project_relationship ALTER COLUMN project_relationship_id SET DEFAULT nextval('project_relationship_project_relationship_id_seq'::regclass);

ALTER TABLE ONLY project_stock ALTER COLUMN project_stock_id SET DEFAULT nextval('project_stock_project_stock_id_seq'::regclass);

ALTER TABLE ONLY projectprop ALTER COLUMN projectprop_id SET DEFAULT nextval('projectprop_projectprop_id_seq'::regclass);

ALTER TABLE ONLY protocol ALTER COLUMN protocol_id SET DEFAULT nextval('protocol_protocol_id_seq'::regclass);

ALTER TABLE ONLY protocolparam ALTER COLUMN protocolparam_id SET DEFAULT nextval('protocolparam_protocolparam_id_seq'::regclass);

ALTER TABLE ONLY pub ALTER COLUMN pub_id SET DEFAULT nextval('pub_pub_id_seq'::regclass);

ALTER TABLE ONLY pub_dbxref ALTER COLUMN pub_dbxref_id SET DEFAULT nextval('pub_dbxref_pub_dbxref_id_seq'::regclass);

ALTER TABLE ONLY pub_relationship ALTER COLUMN pub_relationship_id SET DEFAULT nextval('pub_relationship_pub_relationship_id_seq'::regclass);

ALTER TABLE ONLY pubauthor ALTER COLUMN pubauthor_id SET DEFAULT nextval('pubauthor_pubauthor_id_seq'::regclass);

ALTER TABLE ONLY pubauthor_contact ALTER COLUMN pubauthor_contact_id SET DEFAULT nextval('pubauthor_contact_pubauthor_contact_id_seq'::regclass);

ALTER TABLE ONLY pubprop ALTER COLUMN pubprop_id SET DEFAULT nextval('pubprop_pubprop_id_seq'::regclass);

ALTER TABLE ONLY quantification ALTER COLUMN quantification_id SET DEFAULT nextval('quantification_quantification_id_seq'::regclass);

ALTER TABLE ONLY quantification_relationship ALTER COLUMN quantification_relationship_id SET DEFAULT nextval('quantification_relationship_quantification_relationship_id_seq'::regclass);

ALTER TABLE ONLY quantificationprop ALTER COLUMN quantificationprop_id SET DEFAULT nextval('quantificationprop_quantificationprop_id_seq'::regclass);

ALTER TABLE ONLY stock ALTER COLUMN stock_id SET DEFAULT nextval('stock_stock_id_seq'::regclass);

ALTER TABLE ONLY stock_cvterm ALTER COLUMN stock_cvterm_id SET DEFAULT nextval('stock_cvterm_stock_cvterm_id_seq'::regclass);

ALTER TABLE ONLY stock_cvtermprop ALTER COLUMN stock_cvtermprop_id SET DEFAULT nextval('stock_cvtermprop_stock_cvtermprop_id_seq'::regclass);

ALTER TABLE ONLY stock_dbxref ALTER COLUMN stock_dbxref_id SET DEFAULT nextval('stock_dbxref_stock_dbxref_id_seq'::regclass);

ALTER TABLE ONLY stock_dbxrefprop ALTER COLUMN stock_dbxrefprop_id SET DEFAULT nextval('stock_dbxrefprop_stock_dbxrefprop_id_seq'::regclass);

ALTER TABLE ONLY stock_feature ALTER COLUMN stock_feature_id SET DEFAULT nextval('stock_feature_stock_feature_id_seq'::regclass);

ALTER TABLE ONLY stock_featuremap ALTER COLUMN stock_featuremap_id SET DEFAULT nextval('stock_featuremap_stock_featuremap_id_seq'::regclass);

ALTER TABLE ONLY stock_genotype ALTER COLUMN stock_genotype_id SET DEFAULT nextval('stock_genotype_stock_genotype_id_seq'::regclass);

ALTER TABLE ONLY stock_library ALTER COLUMN stock_library_id SET DEFAULT nextval('stock_library_stock_library_id_seq'::regclass);

ALTER TABLE ONLY stock_pub ALTER COLUMN stock_pub_id SET DEFAULT nextval('stock_pub_stock_pub_id_seq'::regclass);

ALTER TABLE ONLY stock_relationship ALTER COLUMN stock_relationship_id SET DEFAULT nextval('stock_relationship_stock_relationship_id_seq'::regclass);

ALTER TABLE ONLY stock_relationship_cvterm ALTER COLUMN stock_relationship_cvterm_id SET DEFAULT nextval('stock_relationship_cvterm_stock_relationship_cvterm_id_seq'::regclass);

ALTER TABLE ONLY stock_relationship_pub ALTER COLUMN stock_relationship_pub_id SET DEFAULT nextval('stock_relationship_pub_stock_relationship_pub_id_seq'::regclass);

ALTER TABLE ONLY stockcollection ALTER COLUMN stockcollection_id SET DEFAULT nextval('stockcollection_stockcollection_id_seq'::regclass);

ALTER TABLE ONLY stockcollection_db ALTER COLUMN stockcollection_db_id SET DEFAULT nextval('stockcollection_db_stockcollection_db_id_seq'::regclass);

ALTER TABLE ONLY stockcollection_stock ALTER COLUMN stockcollection_stock_id SET DEFAULT nextval('stockcollection_stock_stockcollection_stock_id_seq'::regclass);

ALTER TABLE ONLY stockcollectionprop ALTER COLUMN stockcollectionprop_id SET DEFAULT nextval('stockcollectionprop_stockcollectionprop_id_seq'::regclass);

ALTER TABLE ONLY stockprop ALTER COLUMN stockprop_id SET DEFAULT nextval('stockprop_stockprop_id_seq'::regclass);

ALTER TABLE ONLY stockprop_pub ALTER COLUMN stockprop_pub_id SET DEFAULT nextval('stockprop_pub_stockprop_pub_id_seq'::regclass);

ALTER TABLE ONLY study ALTER COLUMN study_id SET DEFAULT nextval('study_study_id_seq'::regclass);

ALTER TABLE ONLY study_assay ALTER COLUMN study_assay_id SET DEFAULT nextval('study_assay_study_assay_id_seq'::regclass);

ALTER TABLE ONLY studydesign ALTER COLUMN studydesign_id SET DEFAULT nextval('studydesign_studydesign_id_seq'::regclass);

ALTER TABLE ONLY studydesignprop ALTER COLUMN studydesignprop_id SET DEFAULT nextval('studydesignprop_studydesignprop_id_seq'::regclass);

ALTER TABLE ONLY studyfactor ALTER COLUMN studyfactor_id SET DEFAULT nextval('studyfactor_studyfactor_id_seq'::regclass);

ALTER TABLE ONLY studyfactorvalue ALTER COLUMN studyfactorvalue_id SET DEFAULT nextval('studyfactorvalue_studyfactorvalue_id_seq'::regclass);

ALTER TABLE ONLY studyprop ALTER COLUMN studyprop_id SET DEFAULT nextval('studyprop_studyprop_id_seq'::regclass);

ALTER TABLE ONLY studyprop_feature ALTER COLUMN studyprop_feature_id SET DEFAULT nextval('studyprop_feature_studyprop_feature_id_seq'::regclass);

ALTER TABLE ONLY synonym ALTER COLUMN synonym_id SET DEFAULT nextval('synonym_synonym_id_seq'::regclass);

ALTER TABLE ONLY tableinfo ALTER COLUMN tableinfo_id SET DEFAULT nextval('tableinfo_tableinfo_id_seq'::regclass);

ALTER TABLE ONLY treatment ALTER COLUMN treatment_id SET DEFAULT nextval('treatment_treatment_id_seq'::regclass);

ALTER TABLE ONLY acquisition
    ADD CONSTRAINT acquisition_c1 UNIQUE (name);

ALTER TABLE ONLY acquisition
    ADD CONSTRAINT acquisition_pkey PRIMARY KEY (acquisition_id);

ALTER TABLE ONLY acquisition_relationship
    ADD CONSTRAINT acquisition_relationship_c1 UNIQUE (subject_id, object_id, type_id, rank);

ALTER TABLE ONLY acquisition_relationship
    ADD CONSTRAINT acquisition_relationship_pkey PRIMARY KEY (acquisition_relationship_id);

ALTER TABLE ONLY acquisitionprop
    ADD CONSTRAINT acquisitionprop_c1 UNIQUE (acquisition_id, type_id, rank);

ALTER TABLE ONLY acquisitionprop
    ADD CONSTRAINT acquisitionprop_pkey PRIMARY KEY (acquisitionprop_id);

ALTER TABLE ONLY analysis
    ADD CONSTRAINT analysis_c1 UNIQUE (program, programversion, sourcename);

ALTER TABLE ONLY analysis_cvterm
    ADD CONSTRAINT analysis_cvterm_c1 UNIQUE (analysis_id, cvterm_id, rank);

ALTER TABLE ONLY analysis_cvterm
    ADD CONSTRAINT analysis_cvterm_pkey PRIMARY KEY (analysis_cvterm_id);

ALTER TABLE ONLY analysis_dbxref
    ADD CONSTRAINT analysis_dbxref_c1 UNIQUE (analysis_id, dbxref_id);

ALTER TABLE ONLY analysis_dbxref
    ADD CONSTRAINT analysis_dbxref_pkey PRIMARY KEY (analysis_dbxref_id);

ALTER TABLE ONLY analysis
    ADD CONSTRAINT analysis_pkey PRIMARY KEY (analysis_id);

ALTER TABLE ONLY analysis_pub
    ADD CONSTRAINT analysis_pub_c1 UNIQUE (analysis_id, pub_id);

ALTER TABLE ONLY analysis_pub
    ADD CONSTRAINT analysis_pub_pkey PRIMARY KEY (analysis_pub_id);

ALTER TABLE ONLY analysis_relationship
    ADD CONSTRAINT analysis_relationship_c1 UNIQUE (subject_id, object_id, type_id, rank);

ALTER TABLE ONLY analysis_relationship
    ADD CONSTRAINT analysis_relationship_pkey PRIMARY KEY (analysis_relationship_id);

ALTER TABLE ONLY analysisfeature
    ADD CONSTRAINT analysisfeature_c1 UNIQUE (feature_id, analysis_id);

ALTER TABLE ONLY analysisfeatureprop
    ADD CONSTRAINT analysisfeature_id_type_id_rank UNIQUE (analysisfeature_id, type_id, rank);

ALTER TABLE ONLY analysisfeature
    ADD CONSTRAINT analysisfeature_pkey PRIMARY KEY (analysisfeature_id);

ALTER TABLE ONLY analysisfeatureprop
    ADD CONSTRAINT analysisfeatureprop_pkey PRIMARY KEY (analysisfeatureprop_id);

ALTER TABLE ONLY analysisprop
    ADD CONSTRAINT analysisprop_c1 UNIQUE (analysis_id, type_id, rank);

ALTER TABLE ONLY analysisprop
    ADD CONSTRAINT analysisprop_pkey PRIMARY KEY (analysisprop_id);

ALTER TABLE ONLY arraydesign
    ADD CONSTRAINT arraydesign_c1 UNIQUE (name);

ALTER TABLE ONLY arraydesign
    ADD CONSTRAINT arraydesign_pkey PRIMARY KEY (arraydesign_id);

ALTER TABLE ONLY arraydesignprop
    ADD CONSTRAINT arraydesignprop_c1 UNIQUE (arraydesign_id, type_id, rank);

ALTER TABLE ONLY arraydesignprop
    ADD CONSTRAINT arraydesignprop_pkey PRIMARY KEY (arraydesignprop_id);

ALTER TABLE ONLY assay_biomaterial
    ADD CONSTRAINT assay_biomaterial_c1 UNIQUE (assay_id, biomaterial_id, channel_id, rank);

ALTER TABLE ONLY assay_biomaterial
    ADD CONSTRAINT assay_biomaterial_pkey PRIMARY KEY (assay_biomaterial_id);

ALTER TABLE ONLY assay
    ADD CONSTRAINT assay_c1 UNIQUE (name);

ALTER TABLE ONLY assay
    ADD CONSTRAINT assay_pkey PRIMARY KEY (assay_id);

ALTER TABLE ONLY assay_project
    ADD CONSTRAINT assay_project_c1 UNIQUE (assay_id, project_id);

ALTER TABLE ONLY assay_project
    ADD CONSTRAINT assay_project_pkey PRIMARY KEY (assay_project_id);

ALTER TABLE ONLY assayprop
    ADD CONSTRAINT assayprop_c1 UNIQUE (assay_id, type_id, rank);

ALTER TABLE ONLY assayprop
    ADD CONSTRAINT assayprop_pkey PRIMARY KEY (assayprop_id);

ALTER TABLE ONLY biomaterial
    ADD CONSTRAINT biomaterial_c1 UNIQUE (name);

ALTER TABLE ONLY biomaterial_dbxref
    ADD CONSTRAINT biomaterial_dbxref_c1 UNIQUE (biomaterial_id, dbxref_id);

ALTER TABLE ONLY biomaterial_dbxref
    ADD CONSTRAINT biomaterial_dbxref_pkey PRIMARY KEY (biomaterial_dbxref_id);

ALTER TABLE ONLY biomaterial
    ADD CONSTRAINT biomaterial_pkey PRIMARY KEY (biomaterial_id);

ALTER TABLE ONLY biomaterial_relationship
    ADD CONSTRAINT biomaterial_relationship_c1 UNIQUE (subject_id, object_id, type_id);

ALTER TABLE ONLY biomaterial_relationship
    ADD CONSTRAINT biomaterial_relationship_pkey PRIMARY KEY (biomaterial_relationship_id);

ALTER TABLE ONLY biomaterial_treatment
    ADD CONSTRAINT biomaterial_treatment_c1 UNIQUE (biomaterial_id, treatment_id);

ALTER TABLE ONLY biomaterial_treatment
    ADD CONSTRAINT biomaterial_treatment_pkey PRIMARY KEY (biomaterial_treatment_id);

ALTER TABLE ONLY biomaterialprop
    ADD CONSTRAINT biomaterialprop_c1 UNIQUE (biomaterial_id, type_id, rank);

ALTER TABLE ONLY biomaterialprop
    ADD CONSTRAINT biomaterialprop_pkey PRIMARY KEY (biomaterialprop_id);

ALTER TABLE ONLY cell_line
    ADD CONSTRAINT cell_line_c1 UNIQUE (uniquename, organism_id);

ALTER TABLE ONLY cell_line_cvterm
    ADD CONSTRAINT cell_line_cvterm_c1 UNIQUE (cell_line_id, cvterm_id, pub_id, rank);

ALTER TABLE ONLY cell_line_cvterm
    ADD CONSTRAINT cell_line_cvterm_pkey PRIMARY KEY (cell_line_cvterm_id);

ALTER TABLE ONLY cell_line_cvtermprop
    ADD CONSTRAINT cell_line_cvtermprop_c1 UNIQUE (cell_line_cvterm_id, type_id, rank);

ALTER TABLE ONLY cell_line_cvtermprop
    ADD CONSTRAINT cell_line_cvtermprop_pkey PRIMARY KEY (cell_line_cvtermprop_id);

ALTER TABLE ONLY cell_line_dbxref
    ADD CONSTRAINT cell_line_dbxref_c1 UNIQUE (cell_line_id, dbxref_id);

ALTER TABLE ONLY cell_line_dbxref
    ADD CONSTRAINT cell_line_dbxref_pkey PRIMARY KEY (cell_line_dbxref_id);

ALTER TABLE ONLY cell_line_feature
    ADD CONSTRAINT cell_line_feature_c1 UNIQUE (cell_line_id, feature_id, pub_id);

ALTER TABLE ONLY cell_line_feature
    ADD CONSTRAINT cell_line_feature_pkey PRIMARY KEY (cell_line_feature_id);

ALTER TABLE ONLY cell_line_library
    ADD CONSTRAINT cell_line_library_c1 UNIQUE (cell_line_id, library_id, pub_id);

ALTER TABLE ONLY cell_line_library
    ADD CONSTRAINT cell_line_library_pkey PRIMARY KEY (cell_line_library_id);

ALTER TABLE ONLY cell_line
    ADD CONSTRAINT cell_line_pkey PRIMARY KEY (cell_line_id);

ALTER TABLE ONLY cell_line_pub
    ADD CONSTRAINT cell_line_pub_c1 UNIQUE (cell_line_id, pub_id);

ALTER TABLE ONLY cell_line_pub
    ADD CONSTRAINT cell_line_pub_pkey PRIMARY KEY (cell_line_pub_id);

ALTER TABLE ONLY cell_line_relationship
    ADD CONSTRAINT cell_line_relationship_c1 UNIQUE (subject_id, object_id, type_id);

ALTER TABLE ONLY cell_line_relationship
    ADD CONSTRAINT cell_line_relationship_pkey PRIMARY KEY (cell_line_relationship_id);

ALTER TABLE ONLY cell_line_synonym
    ADD CONSTRAINT cell_line_synonym_c1 UNIQUE (synonym_id, cell_line_id, pub_id);

ALTER TABLE ONLY cell_line_synonym
    ADD CONSTRAINT cell_line_synonym_pkey PRIMARY KEY (cell_line_synonym_id);

ALTER TABLE ONLY cell_lineprop
    ADD CONSTRAINT cell_lineprop_c1 UNIQUE (cell_line_id, type_id, rank);

ALTER TABLE ONLY cell_lineprop
    ADD CONSTRAINT cell_lineprop_pkey PRIMARY KEY (cell_lineprop_id);

ALTER TABLE ONLY cell_lineprop_pub
    ADD CONSTRAINT cell_lineprop_pub_c1 UNIQUE (cell_lineprop_id, pub_id);

ALTER TABLE ONLY cell_lineprop_pub
    ADD CONSTRAINT cell_lineprop_pub_pkey PRIMARY KEY (cell_lineprop_pub_id);

ALTER TABLE ONLY chadoprop
    ADD CONSTRAINT chadoprop_c1 UNIQUE (type_id, rank);

ALTER TABLE ONLY chadoprop
    ADD CONSTRAINT chadoprop_pkey PRIMARY KEY (chadoprop_id);

ALTER TABLE ONLY channel
    ADD CONSTRAINT channel_c1 UNIQUE (name);

ALTER TABLE ONLY channel
    ADD CONSTRAINT channel_pkey PRIMARY KEY (channel_id);

ALTER TABLE ONLY contact
    ADD CONSTRAINT contact_c1 UNIQUE (name);

ALTER TABLE ONLY contact
    ADD CONSTRAINT contact_pkey PRIMARY KEY (contact_id);

ALTER TABLE ONLY contact_relationship
    ADD CONSTRAINT contact_relationship_c1 UNIQUE (subject_id, object_id, type_id);

ALTER TABLE ONLY contact_relationship
    ADD CONSTRAINT contact_relationship_pkey PRIMARY KEY (contact_relationship_id);

ALTER TABLE ONLY contactprop
    ADD CONSTRAINT contactprop_c1 UNIQUE (contact_id, type_id, rank);

ALTER TABLE ONLY contactprop
    ADD CONSTRAINT contactprop_pkey PRIMARY KEY (contactprop_id);

ALTER TABLE ONLY control
    ADD CONSTRAINT control_pkey PRIMARY KEY (control_id);

ALTER TABLE ONLY cv
    ADD CONSTRAINT cv_c1 UNIQUE (name);

ALTER TABLE ONLY cv
    ADD CONSTRAINT cv_pkey PRIMARY KEY (cv_id);

ALTER TABLE ONLY cvprop
    ADD CONSTRAINT cvprop_c1 UNIQUE (cv_id, type_id, rank);

ALTER TABLE ONLY cvprop
    ADD CONSTRAINT cvprop_pkey PRIMARY KEY (cvprop_id);

ALTER TABLE ONLY cvterm
    ADD CONSTRAINT cvterm_c1 UNIQUE (name, cv_id, is_obsolete);

ALTER TABLE ONLY cvterm
    ADD CONSTRAINT cvterm_c2 UNIQUE (dbxref_id);

ALTER TABLE ONLY cvterm_dbxref
    ADD CONSTRAINT cvterm_dbxref_c1 UNIQUE (cvterm_id, dbxref_id);

ALTER TABLE ONLY cvterm_dbxref
    ADD CONSTRAINT cvterm_dbxref_pkey PRIMARY KEY (cvterm_dbxref_id);

ALTER TABLE ONLY cvterm
    ADD CONSTRAINT cvterm_pkey PRIMARY KEY (cvterm_id);

ALTER TABLE ONLY cvterm_relationship
    ADD CONSTRAINT cvterm_relationship_c1 UNIQUE (subject_id, object_id, type_id);

ALTER TABLE ONLY cvterm_relationship
    ADD CONSTRAINT cvterm_relationship_pkey PRIMARY KEY (cvterm_relationship_id);

ALTER TABLE ONLY cvtermpath
    ADD CONSTRAINT cvtermpath_c1 UNIQUE (subject_id, object_id, type_id, pathdistance);

ALTER TABLE ONLY cvtermpath
    ADD CONSTRAINT cvtermpath_pkey PRIMARY KEY (cvtermpath_id);

ALTER TABLE ONLY cvtermprop
    ADD CONSTRAINT cvtermprop_cvterm_id_type_id_value_rank_key UNIQUE (cvterm_id, type_id, value, rank);

ALTER TABLE ONLY cvtermprop
    ADD CONSTRAINT cvtermprop_pkey PRIMARY KEY (cvtermprop_id);

ALTER TABLE ONLY cvtermsynonym
    ADD CONSTRAINT cvtermsynonym_c1 UNIQUE (cvterm_id, synonym);

ALTER TABLE ONLY cvtermsynonym
    ADD CONSTRAINT cvtermsynonym_pkey PRIMARY KEY (cvtermsynonym_id);

ALTER TABLE ONLY db
    ADD CONSTRAINT db_c1 UNIQUE (name);

ALTER TABLE ONLY db
    ADD CONSTRAINT db_pkey PRIMARY KEY (db_id);

ALTER TABLE ONLY dbprop
    ADD CONSTRAINT dbprop_c1 UNIQUE (db_id, type_id, rank);

ALTER TABLE ONLY dbprop
    ADD CONSTRAINT dbprop_pkey PRIMARY KEY (dbprop_id);

ALTER TABLE ONLY dbxref
    ADD CONSTRAINT dbxref_c1 UNIQUE (db_id, accession, version);

ALTER TABLE ONLY dbxref
    ADD CONSTRAINT dbxref_pkey PRIMARY KEY (dbxref_id);

ALTER TABLE ONLY dbxrefprop
    ADD CONSTRAINT dbxrefprop_c1 UNIQUE (dbxref_id, type_id, rank);

ALTER TABLE ONLY dbxrefprop
    ADD CONSTRAINT dbxrefprop_pkey PRIMARY KEY (dbxrefprop_id);

ALTER TABLE ONLY eimage
    ADD CONSTRAINT eimage_pkey PRIMARY KEY (eimage_id);

ALTER TABLE ONLY element
    ADD CONSTRAINT element_c1 UNIQUE (feature_id, arraydesign_id);

ALTER TABLE ONLY element
    ADD CONSTRAINT element_pkey PRIMARY KEY (element_id);

ALTER TABLE ONLY element_relationship
    ADD CONSTRAINT element_relationship_c1 UNIQUE (subject_id, object_id, type_id, rank);

ALTER TABLE ONLY element_relationship
    ADD CONSTRAINT element_relationship_pkey PRIMARY KEY (element_relationship_id);

ALTER TABLE ONLY elementresult
    ADD CONSTRAINT elementresult_c1 UNIQUE (element_id, quantification_id);

ALTER TABLE ONLY elementresult
    ADD CONSTRAINT elementresult_pkey PRIMARY KEY (elementresult_id);

ALTER TABLE ONLY elementresult_relationship
    ADD CONSTRAINT elementresult_relationship_c1 UNIQUE (subject_id, object_id, type_id, rank);

ALTER TABLE ONLY elementresult_relationship
    ADD CONSTRAINT elementresult_relationship_pkey PRIMARY KEY (elementresult_relationship_id);

ALTER TABLE ONLY environment
    ADD CONSTRAINT environment_c1 UNIQUE (uniquename);

ALTER TABLE ONLY environment_cvterm
    ADD CONSTRAINT environment_cvterm_c1 UNIQUE (environment_id, cvterm_id);

ALTER TABLE ONLY environment_cvterm
    ADD CONSTRAINT environment_cvterm_pkey PRIMARY KEY (environment_cvterm_id);

ALTER TABLE ONLY environment
    ADD CONSTRAINT environment_pkey PRIMARY KEY (environment_id);

ALTER TABLE ONLY expression
    ADD CONSTRAINT expression_c1 UNIQUE (uniquename);

ALTER TABLE ONLY expression_cvterm
    ADD CONSTRAINT expression_cvterm_c1 UNIQUE (expression_id, cvterm_id, rank, cvterm_type_id);

ALTER TABLE ONLY expression_cvterm
    ADD CONSTRAINT expression_cvterm_pkey PRIMARY KEY (expression_cvterm_id);

ALTER TABLE ONLY expression_cvtermprop
    ADD CONSTRAINT expression_cvtermprop_c1 UNIQUE (expression_cvterm_id, type_id, rank);

ALTER TABLE ONLY expression_cvtermprop
    ADD CONSTRAINT expression_cvtermprop_pkey PRIMARY KEY (expression_cvtermprop_id);

ALTER TABLE ONLY expression_image
    ADD CONSTRAINT expression_image_c1 UNIQUE (expression_id, eimage_id);

ALTER TABLE ONLY expression_image
    ADD CONSTRAINT expression_image_pkey PRIMARY KEY (expression_image_id);

ALTER TABLE ONLY expression
    ADD CONSTRAINT expression_pkey PRIMARY KEY (expression_id);

ALTER TABLE ONLY expression_pub
    ADD CONSTRAINT expression_pub_c1 UNIQUE (expression_id, pub_id);

ALTER TABLE ONLY expression_pub
    ADD CONSTRAINT expression_pub_pkey PRIMARY KEY (expression_pub_id);

ALTER TABLE ONLY expressionprop
    ADD CONSTRAINT expressionprop_c1 UNIQUE (expression_id, type_id, rank);

ALTER TABLE ONLY expressionprop
    ADD CONSTRAINT expressionprop_pkey PRIMARY KEY (expressionprop_id);

ALTER TABLE ONLY feature
    ADD CONSTRAINT feature_c1 UNIQUE (organism_id, uniquename, type_id);

ALTER TABLE ONLY feature_contact
    ADD CONSTRAINT feature_contact_c1 UNIQUE (feature_id, contact_id);

ALTER TABLE ONLY feature_contact
    ADD CONSTRAINT feature_contact_pkey PRIMARY KEY (feature_contact_id);

ALTER TABLE ONLY feature_cvterm
    ADD CONSTRAINT feature_cvterm_c1 UNIQUE (feature_id, cvterm_id, pub_id, rank);

ALTER TABLE ONLY feature_cvterm_dbxref
    ADD CONSTRAINT feature_cvterm_dbxref_c1 UNIQUE (feature_cvterm_id, dbxref_id);

ALTER TABLE ONLY feature_cvterm_dbxref
    ADD CONSTRAINT feature_cvterm_dbxref_pkey PRIMARY KEY (feature_cvterm_dbxref_id);

ALTER TABLE ONLY feature_cvterm
    ADD CONSTRAINT feature_cvterm_pkey PRIMARY KEY (feature_cvterm_id);

ALTER TABLE ONLY feature_cvterm_pub
    ADD CONSTRAINT feature_cvterm_pub_c1 UNIQUE (feature_cvterm_id, pub_id);

ALTER TABLE ONLY feature_cvterm_pub
    ADD CONSTRAINT feature_cvterm_pub_pkey PRIMARY KEY (feature_cvterm_pub_id);

ALTER TABLE ONLY feature_cvtermprop
    ADD CONSTRAINT feature_cvtermprop_c1 UNIQUE (feature_cvterm_id, type_id, rank);

ALTER TABLE ONLY feature_cvtermprop
    ADD CONSTRAINT feature_cvtermprop_pkey PRIMARY KEY (feature_cvtermprop_id);

ALTER TABLE ONLY feature_dbxref
    ADD CONSTRAINT feature_dbxref_c1 UNIQUE (feature_id, dbxref_id);

ALTER TABLE ONLY feature_dbxref
    ADD CONSTRAINT feature_dbxref_pkey PRIMARY KEY (feature_dbxref_id);

ALTER TABLE ONLY feature_expression
    ADD CONSTRAINT feature_expression_c1 UNIQUE (expression_id, feature_id, pub_id);

ALTER TABLE ONLY feature_expression
    ADD CONSTRAINT feature_expression_pkey PRIMARY KEY (feature_expression_id);

ALTER TABLE ONLY feature_expressionprop
    ADD CONSTRAINT feature_expressionprop_c1 UNIQUE (feature_expression_id, type_id, rank);

ALTER TABLE ONLY feature_expressionprop
    ADD CONSTRAINT feature_expressionprop_pkey PRIMARY KEY (feature_expressionprop_id);

ALTER TABLE ONLY feature_genotype
    ADD CONSTRAINT feature_genotype_c1 UNIQUE (feature_id, genotype_id, cvterm_id, chromosome_id, rank, cgroup);

ALTER TABLE ONLY feature_genotype
    ADD CONSTRAINT feature_genotype_pkey PRIMARY KEY (feature_genotype_id);

ALTER TABLE ONLY feature_phenotype
    ADD CONSTRAINT feature_phenotype_c1 UNIQUE (feature_id, phenotype_id);

ALTER TABLE ONLY feature_phenotype
    ADD CONSTRAINT feature_phenotype_pkey PRIMARY KEY (feature_phenotype_id);

ALTER TABLE ONLY feature
    ADD CONSTRAINT feature_pkey PRIMARY KEY (feature_id);

ALTER TABLE ONLY feature_pub
    ADD CONSTRAINT feature_pub_c1 UNIQUE (feature_id, pub_id);

ALTER TABLE ONLY feature_pub
    ADD CONSTRAINT feature_pub_pkey PRIMARY KEY (feature_pub_id);

ALTER TABLE ONLY feature_pubprop
    ADD CONSTRAINT feature_pubprop_c1 UNIQUE (feature_pub_id, type_id, rank);

ALTER TABLE ONLY feature_pubprop
    ADD CONSTRAINT feature_pubprop_pkey PRIMARY KEY (feature_pubprop_id);

ALTER TABLE ONLY feature_relationship
    ADD CONSTRAINT feature_relationship_c1 UNIQUE (subject_id, object_id, type_id, rank);

ALTER TABLE ONLY feature_relationship
    ADD CONSTRAINT feature_relationship_pkey PRIMARY KEY (feature_relationship_id);

ALTER TABLE ONLY feature_relationship_pub
    ADD CONSTRAINT feature_relationship_pub_c1 UNIQUE (feature_relationship_id, pub_id);

ALTER TABLE ONLY feature_relationship_pub
    ADD CONSTRAINT feature_relationship_pub_pkey PRIMARY KEY (feature_relationship_pub_id);

ALTER TABLE ONLY feature_relationshipprop
    ADD CONSTRAINT feature_relationshipprop_c1 UNIQUE (feature_relationship_id, type_id, rank);

ALTER TABLE ONLY feature_relationshipprop
    ADD CONSTRAINT feature_relationshipprop_pkey PRIMARY KEY (feature_relationshipprop_id);

ALTER TABLE ONLY feature_relationshipprop_pub
    ADD CONSTRAINT feature_relationshipprop_pub_c1 UNIQUE (feature_relationshipprop_id, pub_id);

ALTER TABLE ONLY feature_relationshipprop_pub
    ADD CONSTRAINT feature_relationshipprop_pub_pkey PRIMARY KEY (feature_relationshipprop_pub_id);

ALTER TABLE ONLY feature_synonym
    ADD CONSTRAINT feature_synonym_c1 UNIQUE (synonym_id, feature_id, pub_id);

ALTER TABLE ONLY feature_synonym
    ADD CONSTRAINT feature_synonym_pkey PRIMARY KEY (feature_synonym_id);

ALTER TABLE ONLY featureloc
    ADD CONSTRAINT featureloc_c1 UNIQUE (feature_id, locgroup, rank);

ALTER TABLE ONLY featureloc
    ADD CONSTRAINT featureloc_pkey PRIMARY KEY (featureloc_id);

ALTER TABLE ONLY featureloc_pub
    ADD CONSTRAINT featureloc_pub_c1 UNIQUE (featureloc_id, pub_id);

ALTER TABLE ONLY featureloc_pub
    ADD CONSTRAINT featureloc_pub_pkey PRIMARY KEY (featureloc_pub_id);

ALTER TABLE ONLY featuremap
    ADD CONSTRAINT featuremap_c1 UNIQUE (name);

ALTER TABLE ONLY featuremap_contact
    ADD CONSTRAINT featuremap_contact_c1 UNIQUE (featuremap_id, contact_id);

ALTER TABLE ONLY featuremap_contact
    ADD CONSTRAINT featuremap_contact_pkey PRIMARY KEY (featuremap_contact_id);

ALTER TABLE ONLY featuremap_dbxref
    ADD CONSTRAINT featuremap_dbxref_pkey PRIMARY KEY (featuremap_dbxref_id);

ALTER TABLE ONLY featuremap_organism
    ADD CONSTRAINT featuremap_organism_c1 UNIQUE (featuremap_id, organism_id);

ALTER TABLE ONLY featuremap_organism
    ADD CONSTRAINT featuremap_organism_pkey PRIMARY KEY (featuremap_organism_id);

ALTER TABLE ONLY featuremap
    ADD CONSTRAINT featuremap_pkey PRIMARY KEY (featuremap_id);

ALTER TABLE ONLY featuremap_pub
    ADD CONSTRAINT featuremap_pub_pkey PRIMARY KEY (featuremap_pub_id);

ALTER TABLE ONLY featuremapprop
    ADD CONSTRAINT featuremapprop_c1 UNIQUE (featuremap_id, type_id, rank);

ALTER TABLE ONLY featuremapprop
    ADD CONSTRAINT featuremapprop_pkey PRIMARY KEY (featuremapprop_id);

ALTER TABLE ONLY featurepos
    ADD CONSTRAINT featurepos_pkey PRIMARY KEY (featurepos_id);

ALTER TABLE ONLY featureposprop
    ADD CONSTRAINT featureposprop_c1 UNIQUE (featurepos_id, type_id, rank);

ALTER TABLE ONLY featureposprop
    ADD CONSTRAINT featureposprop_pkey PRIMARY KEY (featureposprop_id);

ALTER TABLE ONLY featureprop
    ADD CONSTRAINT featureprop_c1 UNIQUE (feature_id, type_id, rank);

ALTER TABLE ONLY featureprop
    ADD CONSTRAINT featureprop_pkey PRIMARY KEY (featureprop_id);

ALTER TABLE ONLY featureprop_pub
    ADD CONSTRAINT featureprop_pub_c1 UNIQUE (featureprop_id, pub_id);

ALTER TABLE ONLY featureprop_pub
    ADD CONSTRAINT featureprop_pub_pkey PRIMARY KEY (featureprop_pub_id);

ALTER TABLE ONLY featurerange
    ADD CONSTRAINT featurerange_pkey PRIMARY KEY (featurerange_id);

ALTER TABLE ONLY genotype
    ADD CONSTRAINT genotype_c1 UNIQUE (uniquename);

ALTER TABLE ONLY genotype
    ADD CONSTRAINT genotype_pkey PRIMARY KEY (genotype_id);

ALTER TABLE ONLY genotypeprop
    ADD CONSTRAINT genotypeprop_c1 UNIQUE (genotype_id, type_id, rank);

ALTER TABLE ONLY genotypeprop
    ADD CONSTRAINT genotypeprop_pkey PRIMARY KEY (genotypeprop_id);

ALTER TABLE ONLY library
    ADD CONSTRAINT library_c1 UNIQUE (organism_id, uniquename, type_id);

ALTER TABLE ONLY library_contact
    ADD CONSTRAINT library_contact_c1 UNIQUE (library_id, contact_id);

ALTER TABLE ONLY library_contact
    ADD CONSTRAINT library_contact_pkey PRIMARY KEY (library_contact_id);

ALTER TABLE ONLY library_cvterm
    ADD CONSTRAINT library_cvterm_c1 UNIQUE (library_id, cvterm_id, pub_id);

ALTER TABLE ONLY library_cvterm
    ADD CONSTRAINT library_cvterm_pkey PRIMARY KEY (library_cvterm_id);

ALTER TABLE ONLY library_dbxref
    ADD CONSTRAINT library_dbxref_c1 UNIQUE (library_id, dbxref_id);

ALTER TABLE ONLY library_dbxref
    ADD CONSTRAINT library_dbxref_pkey PRIMARY KEY (library_dbxref_id);

ALTER TABLE ONLY library_expression
    ADD CONSTRAINT library_expression_c1 UNIQUE (library_id, expression_id);

ALTER TABLE ONLY library_expression
    ADD CONSTRAINT library_expression_pkey PRIMARY KEY (library_expression_id);

ALTER TABLE ONLY library_expressionprop
    ADD CONSTRAINT library_expressionprop_c1 UNIQUE (library_expression_id, type_id, rank);

ALTER TABLE ONLY library_expressionprop
    ADD CONSTRAINT library_expressionprop_pkey PRIMARY KEY (library_expressionprop_id);

ALTER TABLE ONLY library_feature
    ADD CONSTRAINT library_feature_c1 UNIQUE (library_id, feature_id);

ALTER TABLE ONLY library_feature
    ADD CONSTRAINT library_feature_pkey PRIMARY KEY (library_feature_id);

ALTER TABLE ONLY library_featureprop
    ADD CONSTRAINT library_featureprop_c1 UNIQUE (library_feature_id, type_id, rank);

ALTER TABLE ONLY library_featureprop
    ADD CONSTRAINT library_featureprop_pkey PRIMARY KEY (library_featureprop_id);

ALTER TABLE ONLY library
    ADD CONSTRAINT library_pkey PRIMARY KEY (library_id);

ALTER TABLE ONLY library_pub
    ADD CONSTRAINT library_pub_c1 UNIQUE (library_id, pub_id);

ALTER TABLE ONLY library_pub
    ADD CONSTRAINT library_pub_pkey PRIMARY KEY (library_pub_id);

ALTER TABLE ONLY library_relationship
    ADD CONSTRAINT library_relationship_c1 UNIQUE (subject_id, object_id, type_id);

ALTER TABLE ONLY library_relationship
    ADD CONSTRAINT library_relationship_pkey PRIMARY KEY (library_relationship_id);

ALTER TABLE ONLY library_relationship_pub
    ADD CONSTRAINT library_relationship_pub_c1 UNIQUE (library_relationship_id, pub_id);

ALTER TABLE ONLY library_relationship_pub
    ADD CONSTRAINT library_relationship_pub_pkey PRIMARY KEY (library_relationship_pub_id);

ALTER TABLE ONLY library_synonym
    ADD CONSTRAINT library_synonym_c1 UNIQUE (synonym_id, library_id, pub_id);

ALTER TABLE ONLY library_synonym
    ADD CONSTRAINT library_synonym_pkey PRIMARY KEY (library_synonym_id);

ALTER TABLE ONLY libraryprop
    ADD CONSTRAINT libraryprop_c1 UNIQUE (library_id, type_id, rank);

ALTER TABLE ONLY libraryprop
    ADD CONSTRAINT libraryprop_pkey PRIMARY KEY (libraryprop_id);

ALTER TABLE ONLY libraryprop_pub
    ADD CONSTRAINT libraryprop_pub_c1 UNIQUE (libraryprop_id, pub_id);

ALTER TABLE ONLY libraryprop_pub
    ADD CONSTRAINT libraryprop_pub_pkey PRIMARY KEY (libraryprop_pub_id);

ALTER TABLE ONLY magedocumentation
    ADD CONSTRAINT magedocumentation_pkey PRIMARY KEY (magedocumentation_id);

ALTER TABLE ONLY mageml
    ADD CONSTRAINT mageml_pkey PRIMARY KEY (mageml_id);

ALTER TABLE ONLY nd_experiment_analysis
    ADD CONSTRAINT nd_experiment_analysis_pkey PRIMARY KEY (nd_experiment_analysis_id);

ALTER TABLE ONLY nd_experiment_contact
    ADD CONSTRAINT nd_experiment_contact_pkey PRIMARY KEY (nd_experiment_contact_id);

ALTER TABLE ONLY nd_experiment_dbxref
    ADD CONSTRAINT nd_experiment_dbxref_pkey PRIMARY KEY (nd_experiment_dbxref_id);

ALTER TABLE ONLY nd_experiment_genotype
    ADD CONSTRAINT nd_experiment_genotype_c1 UNIQUE (nd_experiment_id, genotype_id);

ALTER TABLE ONLY nd_experiment_genotype
    ADD CONSTRAINT nd_experiment_genotype_pkey PRIMARY KEY (nd_experiment_genotype_id);

ALTER TABLE ONLY nd_experiment_phenotype
    ADD CONSTRAINT nd_experiment_phenotype_c1 UNIQUE (nd_experiment_id, phenotype_id);

ALTER TABLE ONLY nd_experiment_phenotype
    ADD CONSTRAINT nd_experiment_phenotype_pkey PRIMARY KEY (nd_experiment_phenotype_id);

ALTER TABLE ONLY nd_experiment
    ADD CONSTRAINT nd_experiment_pkey PRIMARY KEY (nd_experiment_id);

ALTER TABLE ONLY nd_experiment_project
    ADD CONSTRAINT nd_experiment_project_c1 UNIQUE (project_id, nd_experiment_id);

ALTER TABLE ONLY nd_experiment_project
    ADD CONSTRAINT nd_experiment_project_pkey PRIMARY KEY (nd_experiment_project_id);

ALTER TABLE ONLY nd_experiment_protocol
    ADD CONSTRAINT nd_experiment_protocol_pkey PRIMARY KEY (nd_experiment_protocol_id);

ALTER TABLE ONLY nd_experiment_pub
    ADD CONSTRAINT nd_experiment_pub_c1 UNIQUE (nd_experiment_id, pub_id);

ALTER TABLE ONLY nd_experiment_pub
    ADD CONSTRAINT nd_experiment_pub_pkey PRIMARY KEY (nd_experiment_pub_id);

ALTER TABLE ONLY nd_experiment_stock_dbxref
    ADD CONSTRAINT nd_experiment_stock_dbxref_pkey PRIMARY KEY (nd_experiment_stock_dbxref_id);

ALTER TABLE ONLY nd_experiment_stock
    ADD CONSTRAINT nd_experiment_stock_pkey PRIMARY KEY (nd_experiment_stock_id);

ALTER TABLE ONLY nd_experiment_stockprop
    ADD CONSTRAINT nd_experiment_stockprop_c1 UNIQUE (nd_experiment_stock_id, type_id, rank);

ALTER TABLE ONLY nd_experiment_stockprop
    ADD CONSTRAINT nd_experiment_stockprop_pkey PRIMARY KEY (nd_experiment_stockprop_id);

ALTER TABLE ONLY nd_experimentprop
    ADD CONSTRAINT nd_experimentprop_c1 UNIQUE (nd_experiment_id, type_id, rank);

ALTER TABLE ONLY nd_experimentprop
    ADD CONSTRAINT nd_experimentprop_pkey PRIMARY KEY (nd_experimentprop_id);

ALTER TABLE ONLY nd_geolocation
    ADD CONSTRAINT nd_geolocation_pkey PRIMARY KEY (nd_geolocation_id);

ALTER TABLE ONLY nd_geolocationprop
    ADD CONSTRAINT nd_geolocationprop_c1 UNIQUE (nd_geolocation_id, type_id, rank);

ALTER TABLE ONLY nd_geolocationprop
    ADD CONSTRAINT nd_geolocationprop_pkey PRIMARY KEY (nd_geolocationprop_id);

ALTER TABLE ONLY nd_protocol
    ADD CONSTRAINT nd_protocol_name_key UNIQUE (name);

ALTER TABLE ONLY nd_protocol
    ADD CONSTRAINT nd_protocol_pkey PRIMARY KEY (nd_protocol_id);

ALTER TABLE ONLY nd_protocol_reagent
    ADD CONSTRAINT nd_protocol_reagent_pkey PRIMARY KEY (nd_protocol_reagent_id);

ALTER TABLE ONLY nd_protocolprop
    ADD CONSTRAINT nd_protocolprop_c1 UNIQUE (nd_protocol_id, type_id, rank);

ALTER TABLE ONLY nd_protocolprop
    ADD CONSTRAINT nd_protocolprop_pkey PRIMARY KEY (nd_protocolprop_id);

ALTER TABLE ONLY nd_reagent
    ADD CONSTRAINT nd_reagent_pkey PRIMARY KEY (nd_reagent_id);

ALTER TABLE ONLY nd_reagent_relationship
    ADD CONSTRAINT nd_reagent_relationship_pkey PRIMARY KEY (nd_reagent_relationship_id);

ALTER TABLE ONLY nd_reagentprop
    ADD CONSTRAINT nd_reagentprop_c1 UNIQUE (nd_reagent_id, type_id, rank);

ALTER TABLE ONLY nd_reagentprop
    ADD CONSTRAINT nd_reagentprop_pkey PRIMARY KEY (nd_reagentprop_id);

ALTER TABLE ONLY organism
    ADD CONSTRAINT organism_c1 UNIQUE (genus, species, type_id, infraspecific_name);

ALTER TABLE ONLY organism_cvterm
    ADD CONSTRAINT organism_cvterm_c1 UNIQUE (organism_id, cvterm_id, pub_id);

ALTER TABLE ONLY organism_cvterm
    ADD CONSTRAINT organism_cvterm_pkey PRIMARY KEY (organism_cvterm_id);

ALTER TABLE ONLY organism_cvtermprop
    ADD CONSTRAINT organism_cvtermprop_c1 UNIQUE (organism_cvterm_id, type_id, rank);

ALTER TABLE ONLY organism_cvtermprop
    ADD CONSTRAINT organism_cvtermprop_pkey PRIMARY KEY (organism_cvtermprop_id);

ALTER TABLE ONLY organism_dbxref
    ADD CONSTRAINT organism_dbxref_c1 UNIQUE (organism_id, dbxref_id);

ALTER TABLE ONLY organism_dbxref
    ADD CONSTRAINT organism_dbxref_pkey PRIMARY KEY (organism_dbxref_id);

ALTER TABLE ONLY organism
    ADD CONSTRAINT organism_pkey PRIMARY KEY (organism_id);

ALTER TABLE ONLY organism_pub
    ADD CONSTRAINT organism_pub_c1 UNIQUE (organism_id, pub_id);

ALTER TABLE ONLY organism_pub
    ADD CONSTRAINT organism_pub_pkey PRIMARY KEY (organism_pub_id);

ALTER TABLE ONLY organism_relationship
    ADD CONSTRAINT organism_relationship_c1 UNIQUE (subject_id, object_id, type_id, rank);

ALTER TABLE ONLY organism_relationship
    ADD CONSTRAINT organism_relationship_pkey PRIMARY KEY (organism_relationship_id);

ALTER TABLE ONLY organismprop
    ADD CONSTRAINT organismprop_c1 UNIQUE (organism_id, type_id, rank);

ALTER TABLE ONLY organismprop
    ADD CONSTRAINT organismprop_pkey PRIMARY KEY (organismprop_id);

ALTER TABLE ONLY organismprop_pub
    ADD CONSTRAINT organismprop_pub_c1 UNIQUE (organismprop_id, pub_id);

ALTER TABLE ONLY organismprop_pub
    ADD CONSTRAINT organismprop_pub_pkey PRIMARY KEY (organismprop_pub_id);

ALTER TABLE ONLY phendesc
    ADD CONSTRAINT phendesc_c1 UNIQUE (genotype_id, environment_id, type_id, pub_id);

ALTER TABLE ONLY phendesc
    ADD CONSTRAINT phendesc_pkey PRIMARY KEY (phendesc_id);

ALTER TABLE ONLY phenotype
    ADD CONSTRAINT phenotype_c1 UNIQUE (uniquename);

ALTER TABLE ONLY phenotype_comparison
    ADD CONSTRAINT phenotype_comparison_c1 UNIQUE (genotype1_id, environment1_id, genotype2_id, environment2_id, phenotype1_id, pub_id);

ALTER TABLE ONLY phenotype_comparison_cvterm
    ADD CONSTRAINT phenotype_comparison_cvterm_c1 UNIQUE (phenotype_comparison_id, cvterm_id);

ALTER TABLE ONLY phenotype_comparison_cvterm
    ADD CONSTRAINT phenotype_comparison_cvterm_pkey PRIMARY KEY (phenotype_comparison_cvterm_id);

ALTER TABLE ONLY phenotype_comparison
    ADD CONSTRAINT phenotype_comparison_pkey PRIMARY KEY (phenotype_comparison_id);

ALTER TABLE ONLY phenotype_cvterm
    ADD CONSTRAINT phenotype_cvterm_c1 UNIQUE (phenotype_id, cvterm_id, rank);

ALTER TABLE ONLY phenotype_cvterm
    ADD CONSTRAINT phenotype_cvterm_pkey PRIMARY KEY (phenotype_cvterm_id);

ALTER TABLE ONLY phenotype
    ADD CONSTRAINT phenotype_pkey PRIMARY KEY (phenotype_id);

ALTER TABLE ONLY phenotypeprop
    ADD CONSTRAINT phenotypeprop_c1 UNIQUE (phenotype_id, type_id, rank);

ALTER TABLE ONLY phenotypeprop
    ADD CONSTRAINT phenotypeprop_pkey PRIMARY KEY (phenotypeprop_id);

ALTER TABLE ONLY phenstatement
    ADD CONSTRAINT phenstatement_c1 UNIQUE (genotype_id, phenotype_id, environment_id, type_id, pub_id);

ALTER TABLE ONLY phenstatement
    ADD CONSTRAINT phenstatement_pkey PRIMARY KEY (phenstatement_id);

ALTER TABLE ONLY phylonode_dbxref
    ADD CONSTRAINT phylonode_dbxref_phylonode_id_dbxref_id_key UNIQUE (phylonode_id, dbxref_id);

ALTER TABLE ONLY phylonode_dbxref
    ADD CONSTRAINT phylonode_dbxref_pkey PRIMARY KEY (phylonode_dbxref_id);

ALTER TABLE ONLY phylonode_organism
    ADD CONSTRAINT phylonode_organism_phylonode_id_key UNIQUE (phylonode_id);

ALTER TABLE ONLY phylonode_organism
    ADD CONSTRAINT phylonode_organism_pkey PRIMARY KEY (phylonode_organism_id);

ALTER TABLE ONLY phylonode
    ADD CONSTRAINT phylonode_phylotree_id_left_idx_key UNIQUE (phylotree_id, left_idx);

ALTER TABLE ONLY phylonode
    ADD CONSTRAINT phylonode_phylotree_id_right_idx_key UNIQUE (phylotree_id, right_idx);

ALTER TABLE ONLY phylonode
    ADD CONSTRAINT phylonode_pkey PRIMARY KEY (phylonode_id);

ALTER TABLE ONLY phylonode_pub
    ADD CONSTRAINT phylonode_pub_phylonode_id_pub_id_key UNIQUE (phylonode_id, pub_id);

ALTER TABLE ONLY phylonode_pub
    ADD CONSTRAINT phylonode_pub_pkey PRIMARY KEY (phylonode_pub_id);

ALTER TABLE ONLY phylonode_relationship
    ADD CONSTRAINT phylonode_relationship_pkey PRIMARY KEY (phylonode_relationship_id);

ALTER TABLE ONLY phylonode_relationship
    ADD CONSTRAINT phylonode_relationship_subject_id_object_id_type_id_key UNIQUE (subject_id, object_id, type_id);

ALTER TABLE ONLY phylonodeprop
    ADD CONSTRAINT phylonodeprop_phylonode_id_type_id_value_rank_key UNIQUE (phylonode_id, type_id, value, rank);

ALTER TABLE ONLY phylonodeprop
    ADD CONSTRAINT phylonodeprop_pkey PRIMARY KEY (phylonodeprop_id);

ALTER TABLE ONLY phylotree
    ADD CONSTRAINT phylotree_pkey PRIMARY KEY (phylotree_id);

ALTER TABLE ONLY phylotree_pub
    ADD CONSTRAINT phylotree_pub_phylotree_id_pub_id_key UNIQUE (phylotree_id, pub_id);

ALTER TABLE ONLY phylotree_pub
    ADD CONSTRAINT phylotree_pub_pkey PRIMARY KEY (phylotree_pub_id);

ALTER TABLE ONLY phylotreeprop
    ADD CONSTRAINT phylotreeprop_c1 UNIQUE (phylotree_id, type_id, rank);

ALTER TABLE ONLY phylotreeprop
    ADD CONSTRAINT phylotreeprop_pkey PRIMARY KEY (phylotreeprop_id);

ALTER TABLE ONLY project_analysis
    ADD CONSTRAINT project_analysis_c1 UNIQUE (project_id, analysis_id);

ALTER TABLE ONLY project_analysis
    ADD CONSTRAINT project_analysis_pkey PRIMARY KEY (project_analysis_id);

ALTER TABLE ONLY project
    ADD CONSTRAINT project_c1 UNIQUE (name);

ALTER TABLE ONLY project_contact
    ADD CONSTRAINT project_contact_c1 UNIQUE (project_id, contact_id);

ALTER TABLE ONLY project_contact
    ADD CONSTRAINT project_contact_pkey PRIMARY KEY (project_contact_id);

ALTER TABLE ONLY project_dbxref
    ADD CONSTRAINT project_dbxref_c1 UNIQUE (project_id, dbxref_id);

ALTER TABLE ONLY project_dbxref
    ADD CONSTRAINT project_dbxref_pkey PRIMARY KEY (project_dbxref_id);

ALTER TABLE ONLY project_feature
    ADD CONSTRAINT project_feature_c1 UNIQUE (feature_id, project_id);

ALTER TABLE ONLY project_feature
    ADD CONSTRAINT project_feature_pkey PRIMARY KEY (project_feature_id);

ALTER TABLE ONLY project
    ADD CONSTRAINT project_pkey PRIMARY KEY (project_id);

ALTER TABLE ONLY project_pub
    ADD CONSTRAINT project_pub_c1 UNIQUE (project_id, pub_id);

ALTER TABLE ONLY project_pub
    ADD CONSTRAINT project_pub_pkey PRIMARY KEY (project_pub_id);

ALTER TABLE ONLY project_relationship
    ADD CONSTRAINT project_relationship_c1 UNIQUE (subject_project_id, object_project_id, type_id);

ALTER TABLE ONLY project_relationship
    ADD CONSTRAINT project_relationship_pkey PRIMARY KEY (project_relationship_id);

ALTER TABLE ONLY project_stock
    ADD CONSTRAINT project_stock_c1 UNIQUE (stock_id, project_id);

ALTER TABLE ONLY project_stock
    ADD CONSTRAINT project_stock_pkey PRIMARY KEY (project_stock_id);

ALTER TABLE ONLY projectprop
    ADD CONSTRAINT projectprop_c1 UNIQUE (project_id, type_id, rank);

ALTER TABLE ONLY projectprop
    ADD CONSTRAINT projectprop_pkey PRIMARY KEY (projectprop_id);

ALTER TABLE ONLY protocol
    ADD CONSTRAINT protocol_c1 UNIQUE (name);

ALTER TABLE ONLY protocol
    ADD CONSTRAINT protocol_pkey PRIMARY KEY (protocol_id);

ALTER TABLE ONLY protocolparam
    ADD CONSTRAINT protocolparam_pkey PRIMARY KEY (protocolparam_id);

ALTER TABLE ONLY pub
    ADD CONSTRAINT pub_c1 UNIQUE (uniquename);

ALTER TABLE ONLY pub_dbxref
    ADD CONSTRAINT pub_dbxref_c1 UNIQUE (pub_id, dbxref_id);

ALTER TABLE ONLY pub_dbxref
    ADD CONSTRAINT pub_dbxref_pkey PRIMARY KEY (pub_dbxref_id);

ALTER TABLE ONLY pub
    ADD CONSTRAINT pub_pkey PRIMARY KEY (pub_id);

ALTER TABLE ONLY pub_relationship
    ADD CONSTRAINT pub_relationship_c1 UNIQUE (subject_id, object_id, type_id);

ALTER TABLE ONLY pub_relationship
    ADD CONSTRAINT pub_relationship_pkey PRIMARY KEY (pub_relationship_id);

ALTER TABLE ONLY pubauthor
    ADD CONSTRAINT pubauthor_c1 UNIQUE (pub_id, rank);

ALTER TABLE ONLY pubauthor_contact
    ADD CONSTRAINT pubauthor_contact_c1 UNIQUE (contact_id, pubauthor_id);

ALTER TABLE ONLY pubauthor_contact
    ADD CONSTRAINT pubauthor_contact_pkey PRIMARY KEY (pubauthor_contact_id);

ALTER TABLE ONLY pubauthor
    ADD CONSTRAINT pubauthor_pkey PRIMARY KEY (pubauthor_id);

ALTER TABLE ONLY pubprop
    ADD CONSTRAINT pubprop_c1 UNIQUE (pub_id, type_id, rank);

ALTER TABLE ONLY pubprop
    ADD CONSTRAINT pubprop_pkey PRIMARY KEY (pubprop_id);

ALTER TABLE ONLY quantification
    ADD CONSTRAINT quantification_c1 UNIQUE (name, analysis_id);

ALTER TABLE ONLY quantification
    ADD CONSTRAINT quantification_pkey PRIMARY KEY (quantification_id);

ALTER TABLE ONLY quantification_relationship
    ADD CONSTRAINT quantification_relationship_c1 UNIQUE (subject_id, object_id, type_id);

ALTER TABLE ONLY quantification_relationship
    ADD CONSTRAINT quantification_relationship_pkey PRIMARY KEY (quantification_relationship_id);

ALTER TABLE ONLY quantificationprop
    ADD CONSTRAINT quantificationprop_c1 UNIQUE (quantification_id, type_id, rank);

ALTER TABLE ONLY quantificationprop
    ADD CONSTRAINT quantificationprop_pkey PRIMARY KEY (quantificationprop_id);

ALTER TABLE ONLY stock
    ADD CONSTRAINT stock_c1 UNIQUE (organism_id, uniquename, type_id);

ALTER TABLE ONLY stock_cvterm
    ADD CONSTRAINT stock_cvterm_c1 UNIQUE (stock_id, cvterm_id, pub_id, rank);

ALTER TABLE ONLY stock_cvterm
    ADD CONSTRAINT stock_cvterm_pkey PRIMARY KEY (stock_cvterm_id);

ALTER TABLE ONLY stock_cvtermprop
    ADD CONSTRAINT stock_cvtermprop_c1 UNIQUE (stock_cvterm_id, type_id, rank);

ALTER TABLE ONLY stock_cvtermprop
    ADD CONSTRAINT stock_cvtermprop_pkey PRIMARY KEY (stock_cvtermprop_id);

ALTER TABLE ONLY stock_dbxref
    ADD CONSTRAINT stock_dbxref_c1 UNIQUE (stock_id, dbxref_id);

ALTER TABLE ONLY stock_dbxref
    ADD CONSTRAINT stock_dbxref_pkey PRIMARY KEY (stock_dbxref_id);

ALTER TABLE ONLY stock_dbxrefprop
    ADD CONSTRAINT stock_dbxrefprop_c1 UNIQUE (stock_dbxref_id, type_id, rank);

ALTER TABLE ONLY stock_dbxrefprop
    ADD CONSTRAINT stock_dbxrefprop_pkey PRIMARY KEY (stock_dbxrefprop_id);

ALTER TABLE ONLY stock_feature
    ADD CONSTRAINT stock_feature_c1 UNIQUE (feature_id, stock_id, type_id, rank);

ALTER TABLE ONLY stock_feature
    ADD CONSTRAINT stock_feature_pkey PRIMARY KEY (stock_feature_id);

ALTER TABLE ONLY stock_featuremap
    ADD CONSTRAINT stock_featuremap_c1 UNIQUE (featuremap_id, stock_id, type_id);

ALTER TABLE ONLY stock_featuremap
    ADD CONSTRAINT stock_featuremap_pkey PRIMARY KEY (stock_featuremap_id);

ALTER TABLE ONLY stock_genotype
    ADD CONSTRAINT stock_genotype_c1 UNIQUE (stock_id, genotype_id);

ALTER TABLE ONLY stock_genotype
    ADD CONSTRAINT stock_genotype_pkey PRIMARY KEY (stock_genotype_id);

ALTER TABLE ONLY stock_library
    ADD CONSTRAINT stock_library_c1 UNIQUE (library_id, stock_id);

ALTER TABLE ONLY stock_library
    ADD CONSTRAINT stock_library_pkey PRIMARY KEY (stock_library_id);

ALTER TABLE ONLY stock
    ADD CONSTRAINT stock_pkey PRIMARY KEY (stock_id);

ALTER TABLE ONLY stock_pub
    ADD CONSTRAINT stock_pub_c1 UNIQUE (stock_id, pub_id);

ALTER TABLE ONLY stock_pub
    ADD CONSTRAINT stock_pub_pkey PRIMARY KEY (stock_pub_id);

ALTER TABLE ONLY stock_relationship
    ADD CONSTRAINT stock_relationship_c1 UNIQUE (subject_id, object_id, type_id, rank);

ALTER TABLE ONLY stock_relationship_cvterm
    ADD CONSTRAINT stock_relationship_cvterm_pkey PRIMARY KEY (stock_relationship_cvterm_id);

ALTER TABLE ONLY stock_relationship
    ADD CONSTRAINT stock_relationship_pkey PRIMARY KEY (stock_relationship_id);

ALTER TABLE ONLY stock_relationship_pub
    ADD CONSTRAINT stock_relationship_pub_c1 UNIQUE (stock_relationship_id, pub_id);

ALTER TABLE ONLY stock_relationship_pub
    ADD CONSTRAINT stock_relationship_pub_pkey PRIMARY KEY (stock_relationship_pub_id);

ALTER TABLE ONLY stockcollection
    ADD CONSTRAINT stockcollection_c1 UNIQUE (uniquename, type_id);

ALTER TABLE ONLY stockcollection_db
    ADD CONSTRAINT stockcollection_db_c1 UNIQUE (stockcollection_id, db_id);

ALTER TABLE ONLY stockcollection_db
    ADD CONSTRAINT stockcollection_db_pkey PRIMARY KEY (stockcollection_db_id);

ALTER TABLE ONLY stockcollection
    ADD CONSTRAINT stockcollection_pkey PRIMARY KEY (stockcollection_id);

ALTER TABLE ONLY stockcollection_stock
    ADD CONSTRAINT stockcollection_stock_c1 UNIQUE (stockcollection_id, stock_id);

ALTER TABLE ONLY stockcollection_stock
    ADD CONSTRAINT stockcollection_stock_pkey PRIMARY KEY (stockcollection_stock_id);

ALTER TABLE ONLY stockcollectionprop
    ADD CONSTRAINT stockcollectionprop_c1 UNIQUE (stockcollection_id, type_id, rank);

ALTER TABLE ONLY stockcollectionprop
    ADD CONSTRAINT stockcollectionprop_pkey PRIMARY KEY (stockcollectionprop_id);

ALTER TABLE ONLY stockprop
    ADD CONSTRAINT stockprop_c1 UNIQUE (stock_id, type_id, rank);

ALTER TABLE ONLY stockprop
    ADD CONSTRAINT stockprop_pkey PRIMARY KEY (stockprop_id);

ALTER TABLE ONLY stockprop_pub
    ADD CONSTRAINT stockprop_pub_c1 UNIQUE (stockprop_id, pub_id);

ALTER TABLE ONLY stockprop_pub
    ADD CONSTRAINT stockprop_pub_pkey PRIMARY KEY (stockprop_pub_id);

ALTER TABLE ONLY study_assay
    ADD CONSTRAINT study_assay_c1 UNIQUE (study_id, assay_id);

ALTER TABLE ONLY study_assay
    ADD CONSTRAINT study_assay_pkey PRIMARY KEY (study_assay_id);

ALTER TABLE ONLY study
    ADD CONSTRAINT study_c1 UNIQUE (name);

ALTER TABLE ONLY study
    ADD CONSTRAINT study_pkey PRIMARY KEY (study_id);

ALTER TABLE ONLY studydesign
    ADD CONSTRAINT studydesign_pkey PRIMARY KEY (studydesign_id);

ALTER TABLE ONLY studydesignprop
    ADD CONSTRAINT studydesignprop_c1 UNIQUE (studydesign_id, type_id, rank);

ALTER TABLE ONLY studydesignprop
    ADD CONSTRAINT studydesignprop_pkey PRIMARY KEY (studydesignprop_id);

ALTER TABLE ONLY studyfactor
    ADD CONSTRAINT studyfactor_pkey PRIMARY KEY (studyfactor_id);

ALTER TABLE ONLY studyfactorvalue
    ADD CONSTRAINT studyfactorvalue_pkey PRIMARY KEY (studyfactorvalue_id);

ALTER TABLE ONLY studyprop_feature
    ADD CONSTRAINT studyprop_feature_pkey PRIMARY KEY (studyprop_feature_id);

ALTER TABLE ONLY studyprop_feature
    ADD CONSTRAINT studyprop_feature_studyprop_id_feature_id_key UNIQUE (studyprop_id, feature_id);

ALTER TABLE ONLY studyprop
    ADD CONSTRAINT studyprop_pkey PRIMARY KEY (studyprop_id);

ALTER TABLE ONLY studyprop
    ADD CONSTRAINT studyprop_study_id_type_id_rank_key UNIQUE (study_id, type_id, rank);

ALTER TABLE ONLY synonym
    ADD CONSTRAINT synonym_c1 UNIQUE (name, type_id);

ALTER TABLE ONLY synonym
    ADD CONSTRAINT synonym_pkey PRIMARY KEY (synonym_id);

ALTER TABLE ONLY tableinfo
    ADD CONSTRAINT tableinfo_c1 UNIQUE (name);

ALTER TABLE ONLY tableinfo
    ADD CONSTRAINT tableinfo_pkey PRIMARY KEY (tableinfo_id);

ALTER TABLE ONLY treatment
    ADD CONSTRAINT treatment_pkey PRIMARY KEY (treatment_id);

CREATE INDEX acquisition_idx1 ON acquisition USING btree (assay_id);

CREATE INDEX acquisition_idx2 ON acquisition USING btree (protocol_id);

CREATE INDEX acquisition_idx3 ON acquisition USING btree (channel_id);

CREATE INDEX acquisition_relationship_idx1 ON acquisition_relationship USING btree (subject_id);

CREATE INDEX acquisition_relationship_idx2 ON acquisition_relationship USING btree (type_id);

CREATE INDEX acquisition_relationship_idx3 ON acquisition_relationship USING btree (object_id);

CREATE INDEX acquisitionprop_idx1 ON acquisitionprop USING btree (acquisition_id);

CREATE INDEX acquisitionprop_idx2 ON acquisitionprop USING btree (type_id);

CREATE INDEX analysis_cvterm_idx1 ON analysis_cvterm USING btree (analysis_id);

CREATE INDEX analysis_cvterm_idx2 ON analysis_cvterm USING btree (cvterm_id);

CREATE INDEX analysis_dbxref_idx1 ON analysis_dbxref USING btree (analysis_id);

CREATE INDEX analysis_dbxref_idx2 ON analysis_dbxref USING btree (dbxref_id);

CREATE INDEX analysis_pub_idx1 ON analysis_pub USING btree (analysis_id);

CREATE INDEX analysis_pub_idx2 ON analysis_pub USING btree (pub_id);

CREATE INDEX analysis_relationship_idx1 ON analysis_relationship USING btree (subject_id);

CREATE INDEX analysis_relationship_idx2 ON analysis_relationship USING btree (object_id);

CREATE INDEX analysis_relationship_idx3 ON analysis_relationship USING btree (type_id);

CREATE INDEX analysisfeature_idx1 ON analysisfeature USING btree (feature_id);

CREATE INDEX analysisfeature_idx2 ON analysisfeature USING btree (analysis_id);

CREATE INDEX analysisfeatureprop_idx1 ON analysisfeatureprop USING btree (analysisfeature_id);

CREATE INDEX analysisfeatureprop_idx2 ON analysisfeatureprop USING btree (type_id);

CREATE INDEX analysisprop_idx1 ON analysisprop USING btree (analysis_id);

CREATE INDEX analysisprop_idx2 ON analysisprop USING btree (type_id);

CREATE INDEX arraydesign_idx1 ON arraydesign USING btree (manufacturer_id);

CREATE INDEX arraydesign_idx2 ON arraydesign USING btree (platformtype_id);

CREATE INDEX arraydesign_idx3 ON arraydesign USING btree (substratetype_id);

CREATE INDEX arraydesign_idx4 ON arraydesign USING btree (protocol_id);

CREATE INDEX arraydesign_idx5 ON arraydesign USING btree (dbxref_id);

CREATE INDEX arraydesignprop_idx1 ON arraydesignprop USING btree (arraydesign_id);

CREATE INDEX arraydesignprop_idx2 ON arraydesignprop USING btree (type_id);

CREATE INDEX assay_biomaterial_idx1 ON assay_biomaterial USING btree (assay_id);

CREATE INDEX assay_biomaterial_idx2 ON assay_biomaterial USING btree (biomaterial_id);

CREATE INDEX assay_biomaterial_idx3 ON assay_biomaterial USING btree (channel_id);

CREATE INDEX assay_idx1 ON assay USING btree (arraydesign_id);

CREATE INDEX assay_idx2 ON assay USING btree (protocol_id);

CREATE INDEX assay_idx3 ON assay USING btree (operator_id);

CREATE INDEX assay_idx4 ON assay USING btree (dbxref_id);

CREATE INDEX assay_project_idx1 ON assay_project USING btree (assay_id);

CREATE INDEX assay_project_idx2 ON assay_project USING btree (project_id);

CREATE INDEX assayprop_idx1 ON assayprop USING btree (assay_id);

CREATE INDEX assayprop_idx2 ON assayprop USING btree (type_id);

CREATE INDEX binloc_boxrange ON featureloc USING gist (boxrange(fmin, fmax));

CREATE INDEX binloc_boxrange_src ON featureloc USING gist (boxrange(srcfeature_id, fmin, fmax));

CREATE INDEX biomaterial_dbxref_idx1 ON biomaterial_dbxref USING btree (biomaterial_id);

CREATE INDEX biomaterial_dbxref_idx2 ON biomaterial_dbxref USING btree (dbxref_id);

CREATE INDEX biomaterial_idx1 ON biomaterial USING btree (taxon_id);

CREATE INDEX biomaterial_idx2 ON biomaterial USING btree (biosourceprovider_id);

CREATE INDEX biomaterial_idx3 ON biomaterial USING btree (dbxref_id);

CREATE INDEX biomaterial_relationship_idx1 ON biomaterial_relationship USING btree (subject_id);

CREATE INDEX biomaterial_relationship_idx2 ON biomaterial_relationship USING btree (object_id);

CREATE INDEX biomaterial_relationship_idx3 ON biomaterial_relationship USING btree (type_id);

CREATE INDEX biomaterial_treatment_idx1 ON biomaterial_treatment USING btree (biomaterial_id);

CREATE INDEX biomaterial_treatment_idx2 ON biomaterial_treatment USING btree (treatment_id);

CREATE INDEX biomaterial_treatment_idx3 ON biomaterial_treatment USING btree (unittype_id);

CREATE INDEX biomaterialprop_idx1 ON biomaterialprop USING btree (biomaterial_id);

CREATE INDEX biomaterialprop_idx2 ON biomaterialprop USING btree (type_id);

CREATE INDEX contact_relationship_idx1 ON contact_relationship USING btree (type_id);

CREATE INDEX contact_relationship_idx2 ON contact_relationship USING btree (subject_id);

CREATE INDEX contact_relationship_idx3 ON contact_relationship USING btree (object_id);

CREATE INDEX contactprop_idx1 ON contactprop USING btree (contact_id);

CREATE INDEX contactprop_idx2 ON contactprop USING btree (type_id);

CREATE INDEX control_idx1 ON control USING btree (type_id);

CREATE INDEX control_idx2 ON control USING btree (assay_id);

CREATE INDEX control_idx3 ON control USING btree (tableinfo_id);

CREATE INDEX control_idx4 ON control USING btree (row_id);

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

CREATE INDEX cvterm_dbxref_idx1 ON cvterm_dbxref USING btree (cvterm_id);

CREATE INDEX cvterm_dbxref_idx2 ON cvterm_dbxref USING btree (dbxref_id);

CREATE INDEX cvterm_idx1 ON cvterm USING btree (cv_id);

CREATE INDEX cvterm_idx2 ON cvterm USING btree (name);

CREATE INDEX cvterm_idx3 ON cvterm USING btree (dbxref_id);

CREATE INDEX cvterm_relationship_idx1 ON cvterm_relationship USING btree (type_id);

CREATE INDEX cvterm_relationship_idx2 ON cvterm_relationship USING btree (subject_id);

CREATE INDEX cvterm_relationship_idx3 ON cvterm_relationship USING btree (object_id);

CREATE INDEX cvtermpath_idx1 ON cvtermpath USING btree (type_id);

CREATE INDEX cvtermpath_idx2 ON cvtermpath USING btree (subject_id);

CREATE INDEX cvtermpath_idx3 ON cvtermpath USING btree (object_id);

CREATE INDEX cvtermpath_idx4 ON cvtermpath USING btree (cv_id);

CREATE INDEX cvtermprop_idx1 ON cvtermprop USING btree (cvterm_id);

CREATE INDEX cvtermprop_idx2 ON cvtermprop USING btree (type_id);

CREATE INDEX cvtermsynonym_idx1 ON cvtermsynonym USING btree (cvterm_id);

CREATE INDEX dbprop_idx1 ON dbprop USING btree (db_id);

CREATE INDEX dbprop_idx2 ON dbprop USING btree (type_id);

CREATE INDEX dbxref_idx1 ON dbxref USING btree (db_id);

CREATE INDEX dbxref_idx2 ON dbxref USING btree (accession);

CREATE INDEX dbxref_idx3 ON dbxref USING btree (version);

CREATE INDEX dbxrefprop_idx1 ON dbxrefprop USING btree (dbxref_id);

CREATE INDEX dbxrefprop_idx2 ON dbxrefprop USING btree (type_id);

CREATE INDEX element_idx1 ON element USING btree (feature_id);

CREATE INDEX element_idx2 ON element USING btree (arraydesign_id);

CREATE INDEX element_idx3 ON element USING btree (type_id);

CREATE INDEX element_idx4 ON element USING btree (dbxref_id);

CREATE INDEX element_relationship_idx1 ON element_relationship USING btree (subject_id);

CREATE INDEX element_relationship_idx2 ON element_relationship USING btree (type_id);

CREATE INDEX element_relationship_idx3 ON element_relationship USING btree (object_id);

CREATE INDEX element_relationship_idx4 ON element_relationship USING btree (value);

CREATE INDEX elementresult_idx1 ON elementresult USING btree (element_id);

CREATE INDEX elementresult_idx2 ON elementresult USING btree (quantification_id);

CREATE INDEX elementresult_idx3 ON elementresult USING btree (signal);

CREATE INDEX elementresult_relationship_idx1 ON elementresult_relationship USING btree (subject_id);

CREATE INDEX elementresult_relationship_idx2 ON elementresult_relationship USING btree (type_id);

CREATE INDEX elementresult_relationship_idx3 ON elementresult_relationship USING btree (object_id);

CREATE INDEX elementresult_relationship_idx4 ON elementresult_relationship USING btree (value);

CREATE INDEX environment_cvterm_idx1 ON environment_cvterm USING btree (environment_id);

CREATE INDEX environment_cvterm_idx2 ON environment_cvterm USING btree (cvterm_id);

CREATE INDEX environment_idx1 ON environment USING btree (uniquename);

CREATE INDEX expression_cvterm_idx1 ON expression_cvterm USING btree (expression_id);

CREATE INDEX expression_cvterm_idx2 ON expression_cvterm USING btree (cvterm_id);

CREATE INDEX expression_cvterm_idx3 ON expression_cvterm USING btree (cvterm_type_id);

CREATE INDEX expression_cvtermprop_idx1 ON expression_cvtermprop USING btree (expression_cvterm_id);

CREATE INDEX expression_cvtermprop_idx2 ON expression_cvtermprop USING btree (type_id);

CREATE INDEX expression_image_idx1 ON expression_image USING btree (expression_id);

CREATE INDEX expression_image_idx2 ON expression_image USING btree (eimage_id);

CREATE INDEX expression_pub_idx1 ON expression_pub USING btree (expression_id);

CREATE INDEX expression_pub_idx2 ON expression_pub USING btree (pub_id);

CREATE INDEX expressionprop_idx1 ON expressionprop USING btree (expression_id);

CREATE INDEX expressionprop_idx2 ON expressionprop USING btree (type_id);

CREATE INDEX feature_contact_idx1 ON feature_contact USING btree (feature_id);

CREATE INDEX feature_contact_idx2 ON feature_contact USING btree (contact_id);

CREATE INDEX feature_cvterm_dbxref_idx1 ON feature_cvterm_dbxref USING btree (feature_cvterm_id);

CREATE INDEX feature_cvterm_dbxref_idx2 ON feature_cvterm_dbxref USING btree (dbxref_id);

CREATE INDEX feature_cvterm_idx1 ON feature_cvterm USING btree (feature_id);

CREATE INDEX feature_cvterm_idx2 ON feature_cvterm USING btree (cvterm_id);

CREATE INDEX feature_cvterm_idx3 ON feature_cvterm USING btree (pub_id);

CREATE INDEX feature_cvterm_pub_idx1 ON feature_cvterm_pub USING btree (feature_cvterm_id);

CREATE INDEX feature_cvterm_pub_idx2 ON feature_cvterm_pub USING btree (pub_id);

CREATE INDEX feature_cvtermprop_idx1 ON feature_cvtermprop USING btree (feature_cvterm_id);

CREATE INDEX feature_cvtermprop_idx2 ON feature_cvtermprop USING btree (type_id);

CREATE INDEX feature_dbxref_idx1 ON feature_dbxref USING btree (feature_id);

CREATE INDEX feature_dbxref_idx2 ON feature_dbxref USING btree (dbxref_id);

CREATE INDEX feature_expression_idx1 ON feature_expression USING btree (expression_id);

CREATE INDEX feature_expression_idx2 ON feature_expression USING btree (feature_id);

CREATE INDEX feature_expression_idx3 ON feature_expression USING btree (pub_id);

CREATE INDEX feature_expressionprop_idx1 ON feature_expressionprop USING btree (feature_expression_id);

CREATE INDEX feature_expressionprop_idx2 ON feature_expressionprop USING btree (type_id);

CREATE INDEX feature_genotype_idx1 ON feature_genotype USING btree (feature_id);

CREATE INDEX feature_genotype_idx2 ON feature_genotype USING btree (genotype_id);

CREATE INDEX feature_idx1 ON feature USING btree (dbxref_id);

CREATE INDEX feature_idx1b ON feature USING btree (feature_id, dbxref_id) WHERE (dbxref_id IS NOT NULL);

CREATE INDEX feature_idx2 ON feature USING btree (organism_id);

CREATE INDEX feature_idx3 ON feature USING btree (type_id);

CREATE INDEX feature_idx4 ON feature USING btree (uniquename);

CREATE INDEX feature_idx5 ON feature USING btree (lower((name)::text));

CREATE INDEX feature_name_ind1 ON feature USING btree (name);

CREATE INDEX feature_phenotype_idx1 ON feature_phenotype USING btree (feature_id);

CREATE INDEX feature_phenotype_idx2 ON feature_phenotype USING btree (phenotype_id);

CREATE INDEX feature_pub_idx1 ON feature_pub USING btree (feature_id);

CREATE INDEX feature_pub_idx2 ON feature_pub USING btree (pub_id);

CREATE INDEX feature_pubprop_idx1 ON feature_pubprop USING btree (feature_pub_id);

CREATE INDEX feature_relationship_idx1 ON feature_relationship USING btree (subject_id);

CREATE INDEX feature_relationship_idx1b ON feature_relationship USING btree (object_id, subject_id, type_id);

CREATE INDEX feature_relationship_idx2 ON feature_relationship USING btree (object_id);

CREATE INDEX feature_relationship_idx3 ON feature_relationship USING btree (type_id);

CREATE INDEX feature_relationship_pub_idx1 ON feature_relationship_pub USING btree (feature_relationship_id);

CREATE INDEX feature_relationship_pub_idx2 ON feature_relationship_pub USING btree (pub_id);

CREATE INDEX feature_relationshipprop_idx1 ON feature_relationshipprop USING btree (feature_relationship_id);

CREATE INDEX feature_relationshipprop_idx2 ON feature_relationshipprop USING btree (type_id);

CREATE INDEX feature_relationshipprop_pub_idx1 ON feature_relationshipprop_pub USING btree (feature_relationshipprop_id);

CREATE INDEX feature_relationshipprop_pub_idx2 ON feature_relationshipprop_pub USING btree (pub_id);

CREATE INDEX feature_synonym_idx1 ON feature_synonym USING btree (synonym_id);

CREATE INDEX feature_synonym_idx2 ON feature_synonym USING btree (feature_id);

CREATE INDEX feature_synonym_idx3 ON feature_synonym USING btree (pub_id);

CREATE INDEX featureloc_idx1 ON featureloc USING btree (feature_id);

CREATE INDEX featureloc_idx1b ON featureloc USING btree (feature_id, fmin, fmax);

CREATE INDEX featureloc_idx2 ON featureloc USING btree (srcfeature_id);

CREATE INDEX featureloc_idx3 ON featureloc USING btree (srcfeature_id, fmin, fmax);

CREATE INDEX featureloc_pub_idx1 ON featureloc_pub USING btree (featureloc_id);

CREATE INDEX featureloc_pub_idx2 ON featureloc_pub USING btree (pub_id);

CREATE INDEX featuremap_contact_idx1 ON featuremap_contact USING btree (featuremap_id);

CREATE INDEX featuremap_contact_idx2 ON featuremap_contact USING btree (contact_id);

CREATE INDEX featuremap_dbxref_idx1 ON featuremap_dbxref USING btree (featuremap_id);

CREATE INDEX featuremap_dbxref_idx2 ON featuremap_dbxref USING btree (dbxref_id);

CREATE INDEX featuremap_organism_idx1 ON featuremap_organism USING btree (featuremap_id);

CREATE INDEX featuremap_organism_idx2 ON featuremap_organism USING btree (organism_id);

CREATE INDEX featuremap_pub_idx1 ON featuremap_pub USING btree (featuremap_id);

CREATE INDEX featuremap_pub_idx2 ON featuremap_pub USING btree (pub_id);

CREATE INDEX featuremapprop_idx1 ON featuremapprop USING btree (featuremap_id);

CREATE INDEX featuremapprop_idx2 ON featuremapprop USING btree (type_id);

CREATE INDEX featurepos_idx1 ON featurepos USING btree (featuremap_id);

CREATE INDEX featurepos_idx2 ON featurepos USING btree (feature_id);

CREATE INDEX featurepos_idx3 ON featurepos USING btree (map_feature_id);

CREATE INDEX featureposprop_idx1 ON featureposprop USING btree (featurepos_id);

CREATE INDEX featureposprop_idx2 ON featureposprop USING btree (type_id);

COMMENT ON INDEX featureprop_c1 IS 'For any one feature, multivalued
property-value pairs must be differentiated by rank.';

CREATE INDEX featureprop_idx1 ON featureprop USING btree (feature_id);

CREATE INDEX featureprop_idx2 ON featureprop USING btree (type_id);

CREATE INDEX featureprop_pub_idx1 ON featureprop_pub USING btree (featureprop_id);

CREATE INDEX featureprop_pub_idx2 ON featureprop_pub USING btree (pub_id);

CREATE INDEX featurerange_idx1 ON featurerange USING btree (featuremap_id);

CREATE INDEX featurerange_idx2 ON featurerange USING btree (feature_id);

CREATE INDEX featurerange_idx3 ON featurerange USING btree (leftstartf_id);

CREATE INDEX featurerange_idx4 ON featurerange USING btree (leftendf_id);

CREATE INDEX featurerange_idx5 ON featurerange USING btree (rightstartf_id);

CREATE INDEX featurerange_idx6 ON featurerange USING btree (rightendf_id);

CREATE INDEX genotype_idx1 ON genotype USING btree (uniquename);

CREATE INDEX genotype_idx2 ON genotype USING btree (name);

CREATE INDEX genotypeprop_idx1 ON genotypeprop USING btree (genotype_id);

CREATE INDEX genotypeprop_idx2 ON genotypeprop USING btree (type_id);

CREATE INDEX library_contact_idx1 ON library USING btree (library_id);

CREATE INDEX library_contact_idx2 ON contact USING btree (contact_id);

CREATE INDEX library_cvterm_idx1 ON library_cvterm USING btree (library_id);

CREATE INDEX library_cvterm_idx2 ON library_cvterm USING btree (cvterm_id);

CREATE INDEX library_cvterm_idx3 ON library_cvterm USING btree (pub_id);

CREATE INDEX library_dbxref_idx1 ON library_dbxref USING btree (library_id);

CREATE INDEX library_dbxref_idx2 ON library_dbxref USING btree (dbxref_id);

CREATE INDEX library_expression_idx1 ON library_expression USING btree (library_id);

CREATE INDEX library_expression_idx2 ON library_expression USING btree (expression_id);

CREATE INDEX library_expression_idx3 ON library_expression USING btree (pub_id);

CREATE INDEX library_expressionprop_idx1 ON library_expressionprop USING btree (library_expression_id);

CREATE INDEX library_expressionprop_idx2 ON library_expressionprop USING btree (type_id);

CREATE INDEX library_feature_idx1 ON library_feature USING btree (library_id);

CREATE INDEX library_feature_idx2 ON library_feature USING btree (feature_id);

CREATE INDEX library_featureprop_idx1 ON library_featureprop USING btree (library_feature_id);

CREATE INDEX library_featureprop_idx2 ON library_featureprop USING btree (type_id);

CREATE INDEX library_idx1 ON library USING btree (organism_id);

CREATE INDEX library_idx2 ON library USING btree (type_id);

CREATE INDEX library_idx3 ON library USING btree (uniquename);

CREATE INDEX library_name_ind1 ON library USING btree (name);

CREATE INDEX library_pub_idx1 ON library_pub USING btree (library_id);

CREATE INDEX library_pub_idx2 ON library_pub USING btree (pub_id);

CREATE INDEX library_relationship_idx1 ON library_relationship USING btree (subject_id);

CREATE INDEX library_relationship_idx2 ON library_relationship USING btree (object_id);

CREATE INDEX library_relationship_idx3 ON library_relationship USING btree (type_id);

CREATE INDEX library_relationship_pub_idx1 ON library_relationship_pub USING btree (library_relationship_id);

CREATE INDEX library_relationship_pub_idx2 ON library_relationship_pub USING btree (pub_id);

CREATE INDEX library_synonym_idx1 ON library_synonym USING btree (synonym_id);

CREATE INDEX library_synonym_idx2 ON library_synonym USING btree (library_id);

CREATE INDEX library_synonym_idx3 ON library_synonym USING btree (pub_id);

CREATE INDEX libraryprop_idx1 ON libraryprop USING btree (library_id);

CREATE INDEX libraryprop_idx2 ON libraryprop USING btree (type_id);

CREATE INDEX libraryprop_pub_idx1 ON libraryprop_pub USING btree (libraryprop_id);

CREATE INDEX libraryprop_pub_idx2 ON libraryprop_pub USING btree (pub_id);

CREATE INDEX magedocumentation_idx1 ON magedocumentation USING btree (mageml_id);

CREATE INDEX magedocumentation_idx2 ON magedocumentation USING btree (tableinfo_id);

CREATE INDEX magedocumentation_idx3 ON magedocumentation USING btree (row_id);

CREATE INDEX nd_experiment_analysis_idx1 ON nd_experiment_analysis USING btree (nd_experiment_id);

CREATE INDEX nd_experiment_analysis_idx2 ON nd_experiment_analysis USING btree (analysis_id);

CREATE INDEX nd_experiment_analysis_idx3 ON nd_experiment_analysis USING btree (type_id);

CREATE INDEX nd_experiment_contact_idx1 ON nd_experiment_contact USING btree (nd_experiment_id);

CREATE INDEX nd_experiment_contact_idx2 ON nd_experiment_contact USING btree (contact_id);

CREATE INDEX nd_experiment_dbxref_idx1 ON nd_experiment_dbxref USING btree (nd_experiment_id);

CREATE INDEX nd_experiment_dbxref_idx2 ON nd_experiment_dbxref USING btree (dbxref_id);

CREATE INDEX nd_experiment_genotype_idx1 ON nd_experiment_genotype USING btree (nd_experiment_id);

CREATE INDEX nd_experiment_genotype_idx2 ON nd_experiment_genotype USING btree (genotype_id);

CREATE INDEX nd_experiment_idx1 ON nd_experiment USING btree (nd_geolocation_id);

CREATE INDEX nd_experiment_idx2 ON nd_experiment USING btree (type_id);

CREATE INDEX nd_experiment_phenotype_idx1 ON nd_experiment_phenotype USING btree (nd_experiment_id);

CREATE INDEX nd_experiment_phenotype_idx2 ON nd_experiment_phenotype USING btree (phenotype_id);

CREATE INDEX nd_experiment_project_idx1 ON nd_experiment_project USING btree (project_id);

CREATE INDEX nd_experiment_project_idx2 ON nd_experiment_project USING btree (nd_experiment_id);

CREATE INDEX nd_experiment_protocol_idx1 ON nd_experiment_protocol USING btree (nd_experiment_id);

CREATE INDEX nd_experiment_protocol_idx2 ON nd_experiment_protocol USING btree (nd_protocol_id);

CREATE INDEX nd_experiment_pub_idx1 ON nd_experiment_pub USING btree (nd_experiment_id);

CREATE INDEX nd_experiment_pub_idx2 ON nd_experiment_pub USING btree (pub_id);

CREATE INDEX nd_experiment_stock_dbxref_idx1 ON nd_experiment_stock_dbxref USING btree (nd_experiment_stock_id);

CREATE INDEX nd_experiment_stock_dbxref_idx2 ON nd_experiment_stock_dbxref USING btree (dbxref_id);

CREATE INDEX nd_experiment_stock_idx1 ON nd_experiment_stock USING btree (nd_experiment_id);

CREATE INDEX nd_experiment_stock_idx2 ON nd_experiment_stock USING btree (stock_id);

CREATE INDEX nd_experiment_stock_idx3 ON nd_experiment_stock USING btree (type_id);

CREATE INDEX nd_experiment_stockprop_idx1 ON nd_experiment_stockprop USING btree (nd_experiment_stock_id);

CREATE INDEX nd_experiment_stockprop_idx2 ON nd_experiment_stockprop USING btree (type_id);

CREATE INDEX nd_experimentprop_idx1 ON nd_experimentprop USING btree (nd_experiment_id);

CREATE INDEX nd_experimentprop_idx2 ON nd_experimentprop USING btree (type_id);

CREATE INDEX nd_geolocation_idx1 ON nd_geolocation USING btree (latitude);

CREATE INDEX nd_geolocation_idx2 ON nd_geolocation USING btree (longitude);

CREATE INDEX nd_geolocation_idx3 ON nd_geolocation USING btree (altitude);

CREATE INDEX nd_geolocationprop_idx1 ON nd_geolocationprop USING btree (nd_geolocation_id);

CREATE INDEX nd_geolocationprop_idx2 ON nd_geolocationprop USING btree (type_id);

CREATE INDEX nd_protocol_idx1 ON nd_protocol USING btree (type_id);

CREATE INDEX nd_protocol_reagent_idx1 ON nd_protocol_reagent USING btree (nd_protocol_id);

CREATE INDEX nd_protocol_reagent_idx2 ON nd_protocol_reagent USING btree (reagent_id);

CREATE INDEX nd_protocol_reagent_idx3 ON nd_protocol_reagent USING btree (type_id);

CREATE INDEX nd_protocolprop_idx1 ON nd_protocolprop USING btree (nd_protocol_id);

CREATE INDEX nd_protocolprop_idx2 ON nd_protocolprop USING btree (type_id);

CREATE INDEX nd_reagent_idx1 ON nd_reagent USING btree (type_id);

CREATE INDEX nd_reagent_idx2 ON nd_reagent USING btree (feature_id);

CREATE INDEX nd_reagent_relationship_idx1 ON nd_reagent_relationship USING btree (subject_reagent_id);

CREATE INDEX nd_reagent_relationship_idx2 ON nd_reagent_relationship USING btree (object_reagent_id);

CREATE INDEX nd_reagent_relationship_idx3 ON nd_reagent_relationship USING btree (type_id);

CREATE INDEX nd_reagentprop_idx1 ON nd_reagentprop USING btree (nd_reagent_id);

CREATE INDEX nd_reagentprop_idx2 ON nd_reagentprop USING btree (type_id);

CREATE INDEX organism_cvterm_idx1 ON organism_cvterm USING btree (organism_id);

CREATE INDEX organism_cvterm_idx2 ON organism_cvterm USING btree (cvterm_id);

CREATE INDEX organism_cvtermprop_idx1 ON organism_cvtermprop USING btree (organism_cvterm_id);

CREATE INDEX organism_cvtermprop_idx2 ON organism_cvtermprop USING btree (type_id);

CREATE INDEX organism_dbxref_idx1 ON organism_dbxref USING btree (organism_id);

CREATE INDEX organism_dbxref_idx2 ON organism_dbxref USING btree (dbxref_id);

CREATE INDEX organism_pub_idx1 ON organism_pub USING btree (organism_id);

CREATE INDEX organism_pub_idx2 ON organism_pub USING btree (pub_id);

CREATE INDEX organism_relationship_idx1 ON organism_relationship USING btree (subject_id);

CREATE INDEX organism_relationship_idx2 ON organism_relationship USING btree (object_id);

CREATE INDEX organism_relationship_idx3 ON organism_relationship USING btree (type_id);

CREATE INDEX organismprop_idx1 ON organismprop USING btree (organism_id);

CREATE INDEX organismprop_idx2 ON organismprop USING btree (type_id);

CREATE INDEX organismprop_pub_idx1 ON organismprop_pub USING btree (organismprop_id);

CREATE INDEX organismprop_pub_idx2 ON organismprop_pub USING btree (pub_id);

CREATE INDEX phendesc_idx1 ON phendesc USING btree (genotype_id);

CREATE INDEX phendesc_idx2 ON phendesc USING btree (environment_id);

CREATE INDEX phendesc_idx3 ON phendesc USING btree (pub_id);

CREATE INDEX phenotype_comparison_cvterm_idx1 ON phenotype_comparison_cvterm USING btree (phenotype_comparison_id);

CREATE INDEX phenotype_comparison_cvterm_idx2 ON phenotype_comparison_cvterm USING btree (cvterm_id);

CREATE INDEX phenotype_comparison_idx1 ON phenotype_comparison USING btree (genotype1_id);

CREATE INDEX phenotype_comparison_idx2 ON phenotype_comparison USING btree (genotype2_id);

CREATE INDEX phenotype_comparison_idx4 ON phenotype_comparison USING btree (pub_id);

CREATE INDEX phenotype_cvterm_idx1 ON phenotype_cvterm USING btree (phenotype_id);

CREATE INDEX phenotype_cvterm_idx2 ON phenotype_cvterm USING btree (cvterm_id);

CREATE INDEX phenotype_idx1 ON phenotype USING btree (cvalue_id);

CREATE INDEX phenotype_idx2 ON phenotype USING btree (observable_id);

CREATE INDEX phenotype_idx3 ON phenotype USING btree (attr_id);

CREATE INDEX phenotypeprop_idx1 ON phenotypeprop USING btree (phenotype_id);

CREATE INDEX phenotypeprop_idx2 ON phenotypeprop USING btree (type_id);

CREATE INDEX phenstatement_idx1 ON phenstatement USING btree (genotype_id);

CREATE INDEX phenstatement_idx2 ON phenstatement USING btree (phenotype_id);

CREATE INDEX phylonode_dbxref_idx1 ON phylonode_dbxref USING btree (phylonode_id);

CREATE INDEX phylonode_dbxref_idx2 ON phylonode_dbxref USING btree (dbxref_id);

CREATE INDEX phylonode_organism_idx1 ON phylonode_organism USING btree (phylonode_id);

CREATE INDEX phylonode_organism_idx2 ON phylonode_organism USING btree (organism_id);

CREATE INDEX phylonode_parent_phylonode_id_idx ON phylonode USING btree (parent_phylonode_id);

CREATE INDEX phylonode_pub_idx1 ON phylonode_pub USING btree (phylonode_id);

CREATE INDEX phylonode_pub_idx2 ON phylonode_pub USING btree (pub_id);

CREATE INDEX phylonode_relationship_idx1 ON phylonode_relationship USING btree (subject_id);

CREATE INDEX phylonode_relationship_idx2 ON phylonode_relationship USING btree (object_id);

CREATE INDEX phylonode_relationship_idx3 ON phylonode_relationship USING btree (type_id);

CREATE INDEX phylonodeprop_idx1 ON phylonodeprop USING btree (phylonode_id);

CREATE INDEX phylonodeprop_idx2 ON phylonodeprop USING btree (type_id);

CREATE INDEX phylotree_idx1 ON phylotree USING btree (phylotree_id);

CREATE INDEX phylotree_pub_idx1 ON phylotree_pub USING btree (phylotree_id);

CREATE INDEX phylotree_pub_idx2 ON phylotree_pub USING btree (pub_id);

COMMENT ON INDEX phylotreeprop_c1 IS 'For any one phylotree, multivalued
property-value pairs must be differentiated by rank.';

CREATE INDEX phylotreeprop_idx1 ON phylotreeprop USING btree (phylotree_id);

CREATE INDEX phylotreeprop_idx2 ON phylotreeprop USING btree (type_id);

CREATE INDEX project_analysis_idx1 ON project_analysis USING btree (project_id);

CREATE INDEX project_analysis_idx2 ON project_analysis USING btree (analysis_id);

CREATE INDEX project_contact_idx1 ON project_contact USING btree (project_id);

CREATE INDEX project_contact_idx2 ON project_contact USING btree (contact_id);

CREATE INDEX project_dbxref_idx1 ON project_dbxref USING btree (project_id);

CREATE INDEX project_dbxref_idx2 ON project_dbxref USING btree (dbxref_id);

CREATE INDEX project_feature_idx1 ON project_feature USING btree (feature_id);

CREATE INDEX project_feature_idx2 ON project_feature USING btree (project_id);

CREATE INDEX project_pub_idx1 ON project_pub USING btree (project_id);

CREATE INDEX project_pub_idx2 ON project_pub USING btree (pub_id);

CREATE INDEX project_stock_idx1 ON project_stock USING btree (stock_id);

CREATE INDEX project_stock_idx2 ON project_stock USING btree (project_id);

CREATE INDEX protocol_idx1 ON protocol USING btree (type_id);

CREATE INDEX protocol_idx2 ON protocol USING btree (pub_id);

CREATE INDEX protocol_idx3 ON protocol USING btree (dbxref_id);

CREATE INDEX protocolparam_idx1 ON protocolparam USING btree (protocol_id);

CREATE INDEX protocolparam_idx2 ON protocolparam USING btree (datatype_id);

CREATE INDEX protocolparam_idx3 ON protocolparam USING btree (unittype_id);

CREATE INDEX pub_dbxref_idx1 ON pub_dbxref USING btree (pub_id);

CREATE INDEX pub_dbxref_idx2 ON pub_dbxref USING btree (dbxref_id);

CREATE INDEX pub_idx1 ON pub USING btree (type_id);

CREATE INDEX pub_relationship_idx1 ON pub_relationship USING btree (subject_id);

CREATE INDEX pub_relationship_idx2 ON pub_relationship USING btree (object_id);

CREATE INDEX pub_relationship_idx3 ON pub_relationship USING btree (type_id);

CREATE INDEX pubauthor_contact_idx1 ON pubauthor USING btree (pubauthor_id);

CREATE INDEX pubauthor_contact_idx2 ON contact USING btree (contact_id);

CREATE INDEX pubauthor_idx2 ON pubauthor USING btree (pub_id);

CREATE INDEX pubprop_idx1 ON pubprop USING btree (pub_id);

CREATE INDEX pubprop_idx2 ON pubprop USING btree (type_id);

CREATE INDEX quantification_idx1 ON quantification USING btree (acquisition_id);

CREATE INDEX quantification_idx2 ON quantification USING btree (operator_id);

CREATE INDEX quantification_idx3 ON quantification USING btree (protocol_id);

CREATE INDEX quantification_idx4 ON quantification USING btree (analysis_id);

CREATE INDEX quantification_relationship_idx1 ON quantification_relationship USING btree (subject_id);

CREATE INDEX quantification_relationship_idx2 ON quantification_relationship USING btree (type_id);

CREATE INDEX quantification_relationship_idx3 ON quantification_relationship USING btree (object_id);

CREATE INDEX quantificationprop_idx1 ON quantificationprop USING btree (quantification_id);

CREATE INDEX quantificationprop_idx2 ON quantificationprop USING btree (type_id);

CREATE INDEX stock_cvterm_idx1 ON stock_cvterm USING btree (stock_id);

CREATE INDEX stock_cvterm_idx2 ON stock_cvterm USING btree (cvterm_id);

CREATE INDEX stock_cvterm_idx3 ON stock_cvterm USING btree (pub_id);

CREATE INDEX stock_cvtermprop_idx1 ON stock_cvtermprop USING btree (stock_cvterm_id);

CREATE INDEX stock_cvtermprop_idx2 ON stock_cvtermprop USING btree (type_id);

CREATE INDEX stock_dbxref_idx1 ON stock_dbxref USING btree (stock_id);

CREATE INDEX stock_dbxref_idx2 ON stock_dbxref USING btree (dbxref_id);

CREATE INDEX stock_dbxrefprop_idx1 ON stock_dbxrefprop USING btree (stock_dbxref_id);

CREATE INDEX stock_dbxrefprop_idx2 ON stock_dbxrefprop USING btree (type_id);

CREATE INDEX stock_feature_idx1 ON stock_feature USING btree (stock_feature_id);

CREATE INDEX stock_feature_idx2 ON stock_feature USING btree (feature_id);

CREATE INDEX stock_feature_idx3 ON stock_feature USING btree (stock_id);

CREATE INDEX stock_feature_idx4 ON stock_feature USING btree (type_id);

CREATE INDEX stock_featuremap_idx1 ON stock_featuremap USING btree (featuremap_id);

CREATE INDEX stock_featuremap_idx2 ON stock_featuremap USING btree (stock_id);

CREATE INDEX stock_featuremap_idx3 ON stock_featuremap USING btree (type_id);

CREATE INDEX stock_genotype_idx1 ON stock_genotype USING btree (stock_id);

CREATE INDEX stock_genotype_idx2 ON stock_genotype USING btree (genotype_id);

CREATE INDEX stock_idx1 ON stock USING btree (dbxref_id);

CREATE INDEX stock_idx2 ON stock USING btree (organism_id);

CREATE INDEX stock_idx3 ON stock USING btree (type_id);

CREATE INDEX stock_idx4 ON stock USING btree (uniquename);

CREATE INDEX stock_library_idx1 ON stock_library USING btree (library_id);

CREATE INDEX stock_library_idx2 ON stock_library USING btree (stock_id);

CREATE INDEX stock_name_ind1 ON stock USING btree (name);

CREATE INDEX stock_pub_idx1 ON stock_pub USING btree (stock_id);

CREATE INDEX stock_pub_idx2 ON stock_pub USING btree (pub_id);

CREATE INDEX stock_relationship_idx1 ON stock_relationship USING btree (subject_id);

CREATE INDEX stock_relationship_idx2 ON stock_relationship USING btree (object_id);

CREATE INDEX stock_relationship_idx3 ON stock_relationship USING btree (type_id);

CREATE INDEX stock_relationship_pub_idx1 ON stock_relationship_pub USING btree (stock_relationship_id);

CREATE INDEX stock_relationship_pub_idx2 ON stock_relationship_pub USING btree (pub_id);

CREATE INDEX stockcollection_db_idx1 ON stockcollection_db USING btree (stockcollection_id);

CREATE INDEX stockcollection_db_idx2 ON stockcollection_db USING btree (db_id);

CREATE INDEX stockcollection_idx1 ON stockcollection USING btree (contact_id);

CREATE INDEX stockcollection_idx2 ON stockcollection USING btree (type_id);

CREATE INDEX stockcollection_idx3 ON stockcollection USING btree (uniquename);

CREATE INDEX stockcollection_name_ind1 ON stockcollection USING btree (name);

CREATE INDEX stockcollection_stock_idx1 ON stockcollection_stock USING btree (stockcollection_id);

CREATE INDEX stockcollection_stock_idx2 ON stockcollection_stock USING btree (stock_id);

CREATE INDEX stockcollectionprop_idx1 ON stockcollectionprop USING btree (stockcollection_id);

CREATE INDEX stockcollectionprop_idx2 ON stockcollectionprop USING btree (type_id);

CREATE INDEX stockprop_idx1 ON stockprop USING btree (stock_id);

CREATE INDEX stockprop_idx2 ON stockprop USING btree (type_id);

CREATE INDEX stockprop_pub_idx1 ON stockprop_pub USING btree (stockprop_id);

CREATE INDEX stockprop_pub_idx2 ON stockprop_pub USING btree (pub_id);

CREATE INDEX study_assay_idx1 ON study_assay USING btree (study_id);

CREATE INDEX study_assay_idx2 ON study_assay USING btree (assay_id);

CREATE INDEX study_idx1 ON study USING btree (contact_id);

CREATE INDEX study_idx2 ON study USING btree (pub_id);

CREATE INDEX study_idx3 ON study USING btree (dbxref_id);

CREATE INDEX studydesign_idx1 ON studydesign USING btree (study_id);

CREATE INDEX studydesignprop_idx1 ON studydesignprop USING btree (studydesign_id);

CREATE INDEX studydesignprop_idx2 ON studydesignprop USING btree (type_id);

CREATE INDEX studyfactor_idx1 ON studyfactor USING btree (studydesign_id);

CREATE INDEX studyfactor_idx2 ON studyfactor USING btree (type_id);

CREATE INDEX studyfactorvalue_idx1 ON studyfactorvalue USING btree (studyfactor_id);

CREATE INDEX studyfactorvalue_idx2 ON studyfactorvalue USING btree (assay_id);

CREATE INDEX studyprop_feature_idx1 ON studyprop_feature USING btree (studyprop_id);

CREATE INDEX studyprop_feature_idx2 ON studyprop_feature USING btree (feature_id);

CREATE INDEX studyprop_idx1 ON studyprop USING btree (study_id);

CREATE INDEX studyprop_idx2 ON studyprop USING btree (type_id);

CREATE INDEX synonym_idx1 ON synonym USING btree (type_id);

CREATE INDEX synonym_idx2 ON synonym USING btree (lower((synonym_sgml)::text));

CREATE INDEX treatment_idx1 ON treatment USING btree (biomaterial_id);

CREATE INDEX treatment_idx2 ON treatment USING btree (type_id);

CREATE INDEX treatment_idx3 ON treatment USING btree (protocol_id);

ALTER TABLE ONLY acquisition
    ADD CONSTRAINT acquisition_assay_id_fkey FOREIGN KEY (assay_id) REFERENCES assay(assay_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY acquisition
    ADD CONSTRAINT acquisition_channel_id_fkey FOREIGN KEY (channel_id) REFERENCES channel(channel_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY acquisition
    ADD CONSTRAINT acquisition_protocol_id_fkey FOREIGN KEY (protocol_id) REFERENCES protocol(protocol_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY acquisition_relationship
    ADD CONSTRAINT acquisition_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES acquisition(acquisition_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY acquisition_relationship
    ADD CONSTRAINT acquisition_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES acquisition(acquisition_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY acquisition_relationship
    ADD CONSTRAINT acquisition_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY acquisitionprop
    ADD CONSTRAINT acquisitionprop_acquisition_id_fkey FOREIGN KEY (acquisition_id) REFERENCES acquisition(acquisition_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY acquisitionprop
    ADD CONSTRAINT acquisitionprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysis_cvterm
    ADD CONSTRAINT analysis_cvterm_analysis_id_fkey FOREIGN KEY (analysis_id) REFERENCES analysis(analysis_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysis_cvterm
    ADD CONSTRAINT analysis_cvterm_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysis_dbxref
    ADD CONSTRAINT analysis_dbxref_analysis_id_fkey FOREIGN KEY (analysis_id) REFERENCES analysis(analysis_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysis_dbxref
    ADD CONSTRAINT analysis_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysis_pub
    ADD CONSTRAINT analysis_pub_analysis_id_fkey FOREIGN KEY (analysis_id) REFERENCES analysis(analysis_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysis_pub
    ADD CONSTRAINT analysis_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysis_relationship
    ADD CONSTRAINT analysis_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES analysis(analysis_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysis_relationship
    ADD CONSTRAINT analysis_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES analysis(analysis_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysis_relationship
    ADD CONSTRAINT analysis_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysisfeature
    ADD CONSTRAINT analysisfeature_analysis_id_fkey FOREIGN KEY (analysis_id) REFERENCES analysis(analysis_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysisfeature
    ADD CONSTRAINT analysisfeature_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysisfeatureprop
    ADD CONSTRAINT analysisfeatureprop_analysisfeature_id_fkey FOREIGN KEY (analysisfeature_id) REFERENCES analysisfeature(analysisfeature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysisfeatureprop
    ADD CONSTRAINT analysisfeatureprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysisprop
    ADD CONSTRAINT analysisprop_analysis_id_fkey FOREIGN KEY (analysis_id) REFERENCES analysis(analysis_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY analysisprop
    ADD CONSTRAINT analysisprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY arraydesign
    ADD CONSTRAINT arraydesign_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY arraydesign
    ADD CONSTRAINT arraydesign_manufacturer_id_fkey FOREIGN KEY (manufacturer_id) REFERENCES contact(contact_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY arraydesign
    ADD CONSTRAINT arraydesign_platformtype_id_fkey FOREIGN KEY (platformtype_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY arraydesign
    ADD CONSTRAINT arraydesign_protocol_id_fkey FOREIGN KEY (protocol_id) REFERENCES protocol(protocol_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY arraydesign
    ADD CONSTRAINT arraydesign_substratetype_id_fkey FOREIGN KEY (substratetype_id) REFERENCES cvterm(cvterm_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY arraydesignprop
    ADD CONSTRAINT arraydesignprop_arraydesign_id_fkey FOREIGN KEY (arraydesign_id) REFERENCES arraydesign(arraydesign_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY arraydesignprop
    ADD CONSTRAINT arraydesignprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY assay
    ADD CONSTRAINT assay_arraydesign_id_fkey FOREIGN KEY (arraydesign_id) REFERENCES arraydesign(arraydesign_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY assay_biomaterial
    ADD CONSTRAINT assay_biomaterial_assay_id_fkey FOREIGN KEY (assay_id) REFERENCES assay(assay_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY assay_biomaterial
    ADD CONSTRAINT assay_biomaterial_biomaterial_id_fkey FOREIGN KEY (biomaterial_id) REFERENCES biomaterial(biomaterial_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY assay_biomaterial
    ADD CONSTRAINT assay_biomaterial_channel_id_fkey FOREIGN KEY (channel_id) REFERENCES channel(channel_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY assay
    ADD CONSTRAINT assay_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY assay
    ADD CONSTRAINT assay_operator_id_fkey FOREIGN KEY (operator_id) REFERENCES contact(contact_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY assay_project
    ADD CONSTRAINT assay_project_assay_id_fkey FOREIGN KEY (assay_id) REFERENCES assay(assay_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY assay_project
    ADD CONSTRAINT assay_project_project_id_fkey FOREIGN KEY (project_id) REFERENCES project(project_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY assay
    ADD CONSTRAINT assay_protocol_id_fkey FOREIGN KEY (protocol_id) REFERENCES protocol(protocol_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY assayprop
    ADD CONSTRAINT assayprop_assay_id_fkey FOREIGN KEY (assay_id) REFERENCES assay(assay_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY assayprop
    ADD CONSTRAINT assayprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY biomaterial
    ADD CONSTRAINT biomaterial_biosourceprovider_id_fkey FOREIGN KEY (biosourceprovider_id) REFERENCES contact(contact_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY biomaterial_dbxref
    ADD CONSTRAINT biomaterial_dbxref_biomaterial_id_fkey FOREIGN KEY (biomaterial_id) REFERENCES biomaterial(biomaterial_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY biomaterial_dbxref
    ADD CONSTRAINT biomaterial_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY biomaterial
    ADD CONSTRAINT biomaterial_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY biomaterial_relationship
    ADD CONSTRAINT biomaterial_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES biomaterial(biomaterial_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY biomaterial_relationship
    ADD CONSTRAINT biomaterial_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES biomaterial(biomaterial_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY biomaterial_relationship
    ADD CONSTRAINT biomaterial_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY biomaterial
    ADD CONSTRAINT biomaterial_taxon_id_fkey FOREIGN KEY (taxon_id) REFERENCES organism(organism_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY biomaterial_treatment
    ADD CONSTRAINT biomaterial_treatment_biomaterial_id_fkey FOREIGN KEY (biomaterial_id) REFERENCES biomaterial(biomaterial_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY biomaterial_treatment
    ADD CONSTRAINT biomaterial_treatment_treatment_id_fkey FOREIGN KEY (treatment_id) REFERENCES treatment(treatment_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY biomaterial_treatment
    ADD CONSTRAINT biomaterial_treatment_unittype_id_fkey FOREIGN KEY (unittype_id) REFERENCES cvterm(cvterm_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY biomaterialprop
    ADD CONSTRAINT biomaterialprop_biomaterial_id_fkey FOREIGN KEY (biomaterial_id) REFERENCES biomaterial(biomaterial_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY biomaterialprop
    ADD CONSTRAINT biomaterialprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_cvterm
    ADD CONSTRAINT cell_line_cvterm_cell_line_id_fkey FOREIGN KEY (cell_line_id) REFERENCES cell_line(cell_line_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_cvterm
    ADD CONSTRAINT cell_line_cvterm_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_cvterm
    ADD CONSTRAINT cell_line_cvterm_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_cvtermprop
    ADD CONSTRAINT cell_line_cvtermprop_cell_line_cvterm_id_fkey FOREIGN KEY (cell_line_cvterm_id) REFERENCES cell_line_cvterm(cell_line_cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_cvtermprop
    ADD CONSTRAINT cell_line_cvtermprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_dbxref
    ADD CONSTRAINT cell_line_dbxref_cell_line_id_fkey FOREIGN KEY (cell_line_id) REFERENCES cell_line(cell_line_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_dbxref
    ADD CONSTRAINT cell_line_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_feature
    ADD CONSTRAINT cell_line_feature_cell_line_id_fkey FOREIGN KEY (cell_line_id) REFERENCES cell_line(cell_line_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_feature
    ADD CONSTRAINT cell_line_feature_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_feature
    ADD CONSTRAINT cell_line_feature_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_library
    ADD CONSTRAINT cell_line_library_cell_line_id_fkey FOREIGN KEY (cell_line_id) REFERENCES cell_line(cell_line_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_library
    ADD CONSTRAINT cell_line_library_library_id_fkey FOREIGN KEY (library_id) REFERENCES library(library_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_library
    ADD CONSTRAINT cell_line_library_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line
    ADD CONSTRAINT cell_line_organism_id_fkey FOREIGN KEY (organism_id) REFERENCES organism(organism_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_pub
    ADD CONSTRAINT cell_line_pub_cell_line_id_fkey FOREIGN KEY (cell_line_id) REFERENCES cell_line(cell_line_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_pub
    ADD CONSTRAINT cell_line_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_relationship
    ADD CONSTRAINT cell_line_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES cell_line(cell_line_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_relationship
    ADD CONSTRAINT cell_line_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES cell_line(cell_line_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_relationship
    ADD CONSTRAINT cell_line_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_synonym
    ADD CONSTRAINT cell_line_synonym_cell_line_id_fkey FOREIGN KEY (cell_line_id) REFERENCES cell_line(cell_line_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_synonym
    ADD CONSTRAINT cell_line_synonym_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_line_synonym
    ADD CONSTRAINT cell_line_synonym_synonym_id_fkey FOREIGN KEY (synonym_id) REFERENCES synonym(synonym_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_lineprop
    ADD CONSTRAINT cell_lineprop_cell_line_id_fkey FOREIGN KEY (cell_line_id) REFERENCES cell_line(cell_line_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_lineprop_pub
    ADD CONSTRAINT cell_lineprop_pub_cell_lineprop_id_fkey FOREIGN KEY (cell_lineprop_id) REFERENCES cell_lineprop(cell_lineprop_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_lineprop_pub
    ADD CONSTRAINT cell_lineprop_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cell_lineprop
    ADD CONSTRAINT cell_lineprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY chadoprop
    ADD CONSTRAINT chadoprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY contact_relationship
    ADD CONSTRAINT contact_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES contact(contact_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY contact_relationship
    ADD CONSTRAINT contact_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES contact(contact_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY contact_relationship
    ADD CONSTRAINT contact_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY contact
    ADD CONSTRAINT contact_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id);

ALTER TABLE ONLY contactprop
    ADD CONSTRAINT contactprop_contact_id_fkey FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE;

ALTER TABLE ONLY contactprop
    ADD CONSTRAINT contactprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY control
    ADD CONSTRAINT control_assay_id_fkey FOREIGN KEY (assay_id) REFERENCES assay(assay_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY control
    ADD CONSTRAINT control_tableinfo_id_fkey FOREIGN KEY (tableinfo_id) REFERENCES tableinfo(tableinfo_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY control
    ADD CONSTRAINT control_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvprop
    ADD CONSTRAINT cvprop_cv_id_fkey FOREIGN KEY (cv_id) REFERENCES cv(cv_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvprop
    ADD CONSTRAINT cvprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvterm
    ADD CONSTRAINT cvterm_cv_id_fkey FOREIGN KEY (cv_id) REFERENCES cv(cv_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvterm_dbxref
    ADD CONSTRAINT cvterm_dbxref_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvterm_dbxref
    ADD CONSTRAINT cvterm_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvterm
    ADD CONSTRAINT cvterm_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvterm_relationship
    ADD CONSTRAINT cvterm_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvterm_relationship
    ADD CONSTRAINT cvterm_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvterm_relationship
    ADD CONSTRAINT cvterm_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvtermpath
    ADD CONSTRAINT cvtermpath_cv_id_fkey FOREIGN KEY (cv_id) REFERENCES cv(cv_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvtermpath
    ADD CONSTRAINT cvtermpath_object_id_fkey FOREIGN KEY (object_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvtermpath
    ADD CONSTRAINT cvtermpath_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvtermpath
    ADD CONSTRAINT cvtermpath_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvtermprop
    ADD CONSTRAINT cvtermprop_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY cvtermprop
    ADD CONSTRAINT cvtermprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY cvtermsynonym
    ADD CONSTRAINT cvtermsynonym_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY cvtermsynonym
    ADD CONSTRAINT cvtermsynonym_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY dbprop
    ADD CONSTRAINT dbprop_db_id_fkey FOREIGN KEY (db_id) REFERENCES db(db_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY dbprop
    ADD CONSTRAINT dbprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY dbxref
    ADD CONSTRAINT dbxref_db_id_fkey FOREIGN KEY (db_id) REFERENCES db(db_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY dbxrefprop
    ADD CONSTRAINT dbxrefprop_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY dbxrefprop
    ADD CONSTRAINT dbxrefprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY element
    ADD CONSTRAINT element_arraydesign_id_fkey FOREIGN KEY (arraydesign_id) REFERENCES arraydesign(arraydesign_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY element
    ADD CONSTRAINT element_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY element
    ADD CONSTRAINT element_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY element_relationship
    ADD CONSTRAINT element_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES element(element_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY element_relationship
    ADD CONSTRAINT element_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES element(element_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY element_relationship
    ADD CONSTRAINT element_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY element
    ADD CONSTRAINT element_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY elementresult
    ADD CONSTRAINT elementresult_element_id_fkey FOREIGN KEY (element_id) REFERENCES element(element_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY elementresult
    ADD CONSTRAINT elementresult_quantification_id_fkey FOREIGN KEY (quantification_id) REFERENCES quantification(quantification_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY elementresult_relationship
    ADD CONSTRAINT elementresult_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES elementresult(elementresult_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY elementresult_relationship
    ADD CONSTRAINT elementresult_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES elementresult(elementresult_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY elementresult_relationship
    ADD CONSTRAINT elementresult_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY environment_cvterm
    ADD CONSTRAINT environment_cvterm_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY environment_cvterm
    ADD CONSTRAINT environment_cvterm_environment_id_fkey FOREIGN KEY (environment_id) REFERENCES environment(environment_id) ON DELETE CASCADE;

ALTER TABLE ONLY expression_cvterm
    ADD CONSTRAINT expression_cvterm_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY expression_cvterm
    ADD CONSTRAINT expression_cvterm_cvterm_type_id_fkey FOREIGN KEY (cvterm_type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY expression_cvterm
    ADD CONSTRAINT expression_cvterm_expression_id_fkey FOREIGN KEY (expression_id) REFERENCES expression(expression_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY expression_cvtermprop
    ADD CONSTRAINT expression_cvtermprop_expression_cvterm_id_fkey FOREIGN KEY (expression_cvterm_id) REFERENCES expression_cvterm(expression_cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY expression_cvtermprop
    ADD CONSTRAINT expression_cvtermprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY expression_image
    ADD CONSTRAINT expression_image_eimage_id_fkey FOREIGN KEY (eimage_id) REFERENCES eimage(eimage_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY expression_image
    ADD CONSTRAINT expression_image_expression_id_fkey FOREIGN KEY (expression_id) REFERENCES expression(expression_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY expression_pub
    ADD CONSTRAINT expression_pub_expression_id_fkey FOREIGN KEY (expression_id) REFERENCES expression(expression_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY expression_pub
    ADD CONSTRAINT expression_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY expressionprop
    ADD CONSTRAINT expressionprop_expression_id_fkey FOREIGN KEY (expression_id) REFERENCES expression(expression_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY expressionprop
    ADD CONSTRAINT expressionprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_contact
    ADD CONSTRAINT feature_contact_contact_id_fkey FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE;

ALTER TABLE ONLY feature_contact
    ADD CONSTRAINT feature_contact_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE;

ALTER TABLE ONLY feature_cvterm
    ADD CONSTRAINT feature_cvterm_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_cvterm_dbxref
    ADD CONSTRAINT feature_cvterm_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_cvterm_dbxref
    ADD CONSTRAINT feature_cvterm_dbxref_feature_cvterm_id_fkey FOREIGN KEY (feature_cvterm_id) REFERENCES feature_cvterm(feature_cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY feature_cvterm
    ADD CONSTRAINT feature_cvterm_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_cvterm_pub
    ADD CONSTRAINT feature_cvterm_pub_feature_cvterm_id_fkey FOREIGN KEY (feature_cvterm_id) REFERENCES feature_cvterm(feature_cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY feature_cvterm
    ADD CONSTRAINT feature_cvterm_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_cvterm_pub
    ADD CONSTRAINT feature_cvterm_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_cvtermprop
    ADD CONSTRAINT feature_cvtermprop_feature_cvterm_id_fkey FOREIGN KEY (feature_cvterm_id) REFERENCES feature_cvterm(feature_cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY feature_cvtermprop
    ADD CONSTRAINT feature_cvtermprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_dbxref
    ADD CONSTRAINT feature_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_dbxref
    ADD CONSTRAINT feature_dbxref_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature
    ADD CONSTRAINT feature_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_expression
    ADD CONSTRAINT feature_expression_expression_id_fkey FOREIGN KEY (expression_id) REFERENCES expression(expression_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_expression
    ADD CONSTRAINT feature_expression_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_expression
    ADD CONSTRAINT feature_expression_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_expressionprop
    ADD CONSTRAINT feature_expressionprop_feature_expression_id_fkey FOREIGN KEY (feature_expression_id) REFERENCES feature_expression(feature_expression_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_expressionprop
    ADD CONSTRAINT feature_expressionprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_genotype
    ADD CONSTRAINT feature_genotype_chromosome_id_fkey FOREIGN KEY (chromosome_id) REFERENCES feature(feature_id) ON DELETE SET NULL;

ALTER TABLE ONLY feature_genotype
    ADD CONSTRAINT feature_genotype_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY feature_genotype
    ADD CONSTRAINT feature_genotype_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE;

ALTER TABLE ONLY feature_genotype
    ADD CONSTRAINT feature_genotype_genotype_id_fkey FOREIGN KEY (genotype_id) REFERENCES genotype(genotype_id) ON DELETE CASCADE;

ALTER TABLE ONLY feature
    ADD CONSTRAINT feature_organism_id_fkey FOREIGN KEY (organism_id) REFERENCES organism(organism_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_phenotype
    ADD CONSTRAINT feature_phenotype_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE;

ALTER TABLE ONLY feature_phenotype
    ADD CONSTRAINT feature_phenotype_phenotype_id_fkey FOREIGN KEY (phenotype_id) REFERENCES phenotype(phenotype_id) ON DELETE CASCADE;

ALTER TABLE ONLY feature_pub
    ADD CONSTRAINT feature_pub_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_pub
    ADD CONSTRAINT feature_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_pubprop
    ADD CONSTRAINT feature_pubprop_feature_pub_id_fkey FOREIGN KEY (feature_pub_id) REFERENCES feature_pub(feature_pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_pubprop
    ADD CONSTRAINT feature_pubprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_relationship
    ADD CONSTRAINT feature_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_relationship_pub
    ADD CONSTRAINT feature_relationship_pub_feature_relationship_id_fkey FOREIGN KEY (feature_relationship_id) REFERENCES feature_relationship(feature_relationship_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_relationship_pub
    ADD CONSTRAINT feature_relationship_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_relationship
    ADD CONSTRAINT feature_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_relationship
    ADD CONSTRAINT feature_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_relationshipprop
    ADD CONSTRAINT feature_relationshipprop_feature_relationship_id_fkey FOREIGN KEY (feature_relationship_id) REFERENCES feature_relationship(feature_relationship_id) ON DELETE CASCADE;

ALTER TABLE ONLY feature_relationshipprop_pub
    ADD CONSTRAINT feature_relationshipprop_pub_feature_relationshipprop_id_fkey FOREIGN KEY (feature_relationshipprop_id) REFERENCES feature_relationshipprop(feature_relationshipprop_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_relationshipprop_pub
    ADD CONSTRAINT feature_relationshipprop_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_relationshipprop
    ADD CONSTRAINT feature_relationshipprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_synonym
    ADD CONSTRAINT feature_synonym_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_synonym
    ADD CONSTRAINT feature_synonym_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature_synonym
    ADD CONSTRAINT feature_synonym_synonym_id_fkey FOREIGN KEY (synonym_id) REFERENCES synonym(synonym_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY feature
    ADD CONSTRAINT feature_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featureloc
    ADD CONSTRAINT featureloc_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featureloc_pub
    ADD CONSTRAINT featureloc_pub_featureloc_id_fkey FOREIGN KEY (featureloc_id) REFERENCES featureloc(featureloc_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featureloc_pub
    ADD CONSTRAINT featureloc_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featureloc
    ADD CONSTRAINT featureloc_srcfeature_id_fkey FOREIGN KEY (srcfeature_id) REFERENCES feature(feature_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featuremap_contact
    ADD CONSTRAINT featuremap_contact_contact_id_fkey FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE;

ALTER TABLE ONLY featuremap_contact
    ADD CONSTRAINT featuremap_contact_featuremap_id_fkey FOREIGN KEY (featuremap_id) REFERENCES featuremap(featuremap_id) ON DELETE CASCADE;

ALTER TABLE ONLY featuremap_dbxref
    ADD CONSTRAINT featuremap_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE;

ALTER TABLE ONLY featuremap_dbxref
    ADD CONSTRAINT featuremap_dbxref_featuremap_id_fkey FOREIGN KEY (featuremap_id) REFERENCES featuremap(featuremap_id) ON DELETE CASCADE;

ALTER TABLE ONLY featuremap_organism
    ADD CONSTRAINT featuremap_organism_featuremap_id_fkey FOREIGN KEY (featuremap_id) REFERENCES featuremap(featuremap_id) ON DELETE CASCADE;

ALTER TABLE ONLY featuremap_organism
    ADD CONSTRAINT featuremap_organism_organism_id_fkey FOREIGN KEY (organism_id) REFERENCES organism(organism_id) ON DELETE CASCADE;

ALTER TABLE ONLY featuremap_pub
    ADD CONSTRAINT featuremap_pub_featuremap_id_fkey FOREIGN KEY (featuremap_id) REFERENCES featuremap(featuremap_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featuremap_pub
    ADD CONSTRAINT featuremap_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featuremap
    ADD CONSTRAINT featuremap_unittype_id_fkey FOREIGN KEY (unittype_id) REFERENCES cvterm(cvterm_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featuremapprop
    ADD CONSTRAINT featuremapprop_featuremap_id_fkey FOREIGN KEY (featuremap_id) REFERENCES featuremap(featuremap_id) ON DELETE CASCADE;

ALTER TABLE ONLY featuremapprop
    ADD CONSTRAINT featuremapprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY featurepos
    ADD CONSTRAINT featurepos_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featurepos
    ADD CONSTRAINT featurepos_featuremap_id_fkey FOREIGN KEY (featuremap_id) REFERENCES featuremap(featuremap_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featurepos
    ADD CONSTRAINT featurepos_map_feature_id_fkey FOREIGN KEY (map_feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featureposprop
    ADD CONSTRAINT featureposprop_featurepos_id_fkey FOREIGN KEY (featurepos_id) REFERENCES featurepos(featurepos_id) ON DELETE CASCADE;

ALTER TABLE ONLY featureposprop
    ADD CONSTRAINT featureposprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY featureprop
    ADD CONSTRAINT featureprop_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featureprop_pub
    ADD CONSTRAINT featureprop_pub_featureprop_id_fkey FOREIGN KEY (featureprop_id) REFERENCES featureprop(featureprop_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featureprop_pub
    ADD CONSTRAINT featureprop_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featureprop
    ADD CONSTRAINT featureprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featurerange
    ADD CONSTRAINT featurerange_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featurerange
    ADD CONSTRAINT featurerange_featuremap_id_fkey FOREIGN KEY (featuremap_id) REFERENCES featuremap(featuremap_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featurerange
    ADD CONSTRAINT featurerange_leftendf_id_fkey FOREIGN KEY (leftendf_id) REFERENCES feature(feature_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featurerange
    ADD CONSTRAINT featurerange_leftstartf_id_fkey FOREIGN KEY (leftstartf_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featurerange
    ADD CONSTRAINT featurerange_rightendf_id_fkey FOREIGN KEY (rightendf_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY featurerange
    ADD CONSTRAINT featurerange_rightstartf_id_fkey FOREIGN KEY (rightstartf_id) REFERENCES feature(feature_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY genotype
    ADD CONSTRAINT genotype_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY genotypeprop
    ADD CONSTRAINT genotypeprop_genotype_id_fkey FOREIGN KEY (genotype_id) REFERENCES genotype(genotype_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY genotypeprop
    ADD CONSTRAINT genotypeprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_contact
    ADD CONSTRAINT library_contact_contact_id_fkey FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE;

ALTER TABLE ONLY library_contact
    ADD CONSTRAINT library_contact_library_id_fkey FOREIGN KEY (library_id) REFERENCES library(library_id) ON DELETE CASCADE;

ALTER TABLE ONLY library_cvterm
    ADD CONSTRAINT library_cvterm_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id);

ALTER TABLE ONLY library_cvterm
    ADD CONSTRAINT library_cvterm_library_id_fkey FOREIGN KEY (library_id) REFERENCES library(library_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_cvterm
    ADD CONSTRAINT library_cvterm_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id);

ALTER TABLE ONLY library_dbxref
    ADD CONSTRAINT library_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_dbxref
    ADD CONSTRAINT library_dbxref_library_id_fkey FOREIGN KEY (library_id) REFERENCES library(library_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_expression
    ADD CONSTRAINT library_expression_expression_id_fkey FOREIGN KEY (expression_id) REFERENCES expression(expression_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_expression
    ADD CONSTRAINT library_expression_library_id_fkey FOREIGN KEY (library_id) REFERENCES library(library_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_expression
    ADD CONSTRAINT library_expression_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id);

ALTER TABLE ONLY library_expressionprop
    ADD CONSTRAINT library_expressionprop_library_expression_id_fkey FOREIGN KEY (library_expression_id) REFERENCES library_expression(library_expression_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_expressionprop
    ADD CONSTRAINT library_expressionprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id);

ALTER TABLE ONLY library_feature
    ADD CONSTRAINT library_feature_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_feature
    ADD CONSTRAINT library_feature_library_id_fkey FOREIGN KEY (library_id) REFERENCES library(library_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_featureprop
    ADD CONSTRAINT library_featureprop_library_feature_id_fkey FOREIGN KEY (library_feature_id) REFERENCES library_feature(library_feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_featureprop
    ADD CONSTRAINT library_featureprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id);

ALTER TABLE ONLY library
    ADD CONSTRAINT library_organism_id_fkey FOREIGN KEY (organism_id) REFERENCES organism(organism_id);

ALTER TABLE ONLY library_pub
    ADD CONSTRAINT library_pub_library_id_fkey FOREIGN KEY (library_id) REFERENCES library(library_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_pub
    ADD CONSTRAINT library_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_relationship
    ADD CONSTRAINT library_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES library(library_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_relationship_pub
    ADD CONSTRAINT library_relationship_pub_library_relationship_id_fkey FOREIGN KEY (library_relationship_id) REFERENCES library_relationship(library_relationship_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_relationship_pub
    ADD CONSTRAINT library_relationship_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id);

ALTER TABLE ONLY library_relationship
    ADD CONSTRAINT library_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES library(library_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_relationship
    ADD CONSTRAINT library_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id);

ALTER TABLE ONLY library_synonym
    ADD CONSTRAINT library_synonym_library_id_fkey FOREIGN KEY (library_id) REFERENCES library(library_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_synonym
    ADD CONSTRAINT library_synonym_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library_synonym
    ADD CONSTRAINT library_synonym_synonym_id_fkey FOREIGN KEY (synonym_id) REFERENCES synonym(synonym_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY library
    ADD CONSTRAINT library_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id);

ALTER TABLE ONLY libraryprop
    ADD CONSTRAINT libraryprop_library_id_fkey FOREIGN KEY (library_id) REFERENCES library(library_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY libraryprop_pub
    ADD CONSTRAINT libraryprop_pub_libraryprop_id_fkey FOREIGN KEY (libraryprop_id) REFERENCES libraryprop(libraryprop_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY libraryprop_pub
    ADD CONSTRAINT libraryprop_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY libraryprop
    ADD CONSTRAINT libraryprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id);

ALTER TABLE ONLY magedocumentation
    ADD CONSTRAINT magedocumentation_mageml_id_fkey FOREIGN KEY (mageml_id) REFERENCES mageml(mageml_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY magedocumentation
    ADD CONSTRAINT magedocumentation_tableinfo_id_fkey FOREIGN KEY (tableinfo_id) REFERENCES tableinfo(tableinfo_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_analysis
    ADD CONSTRAINT nd_experiment_analysis_analysis_id_fkey FOREIGN KEY (analysis_id) REFERENCES analysis(analysis_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_analysis
    ADD CONSTRAINT nd_experiment_analysis_nd_experiment_id_fkey FOREIGN KEY (nd_experiment_id) REFERENCES nd_experiment(nd_experiment_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_analysis
    ADD CONSTRAINT nd_experiment_analysis_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_contact
    ADD CONSTRAINT nd_experiment_contact_contact_id_fkey FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_contact
    ADD CONSTRAINT nd_experiment_contact_nd_experiment_id_fkey FOREIGN KEY (nd_experiment_id) REFERENCES nd_experiment(nd_experiment_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_dbxref
    ADD CONSTRAINT nd_experiment_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_dbxref
    ADD CONSTRAINT nd_experiment_dbxref_nd_experiment_id_fkey FOREIGN KEY (nd_experiment_id) REFERENCES nd_experiment(nd_experiment_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_genotype
    ADD CONSTRAINT nd_experiment_genotype_genotype_id_fkey FOREIGN KEY (genotype_id) REFERENCES genotype(genotype_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_genotype
    ADD CONSTRAINT nd_experiment_genotype_nd_experiment_id_fkey FOREIGN KEY (nd_experiment_id) REFERENCES nd_experiment(nd_experiment_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment
    ADD CONSTRAINT nd_experiment_nd_geolocation_id_fkey FOREIGN KEY (nd_geolocation_id) REFERENCES nd_geolocation(nd_geolocation_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_phenotype
    ADD CONSTRAINT nd_experiment_phenotype_nd_experiment_id_fkey FOREIGN KEY (nd_experiment_id) REFERENCES nd_experiment(nd_experiment_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_phenotype
    ADD CONSTRAINT nd_experiment_phenotype_phenotype_id_fkey FOREIGN KEY (phenotype_id) REFERENCES phenotype(phenotype_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_project
    ADD CONSTRAINT nd_experiment_project_nd_experiment_id_fkey FOREIGN KEY (nd_experiment_id) REFERENCES nd_experiment(nd_experiment_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_project
    ADD CONSTRAINT nd_experiment_project_project_id_fkey FOREIGN KEY (project_id) REFERENCES project(project_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_protocol
    ADD CONSTRAINT nd_experiment_protocol_nd_experiment_id_fkey FOREIGN KEY (nd_experiment_id) REFERENCES nd_experiment(nd_experiment_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_protocol
    ADD CONSTRAINT nd_experiment_protocol_nd_protocol_id_fkey FOREIGN KEY (nd_protocol_id) REFERENCES nd_protocol(nd_protocol_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_pub
    ADD CONSTRAINT nd_experiment_pub_nd_experiment_id_fkey FOREIGN KEY (nd_experiment_id) REFERENCES nd_experiment(nd_experiment_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_pub
    ADD CONSTRAINT nd_experiment_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_stock_dbxref
    ADD CONSTRAINT nd_experiment_stock_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_stock_dbxref
    ADD CONSTRAINT nd_experiment_stock_dbxref_nd_experiment_stock_id_fkey FOREIGN KEY (nd_experiment_stock_id) REFERENCES nd_experiment_stock(nd_experiment_stock_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_stock
    ADD CONSTRAINT nd_experiment_stock_nd_experiment_id_fkey FOREIGN KEY (nd_experiment_id) REFERENCES nd_experiment(nd_experiment_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_stock
    ADD CONSTRAINT nd_experiment_stock_stock_id_fkey FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_stock
    ADD CONSTRAINT nd_experiment_stock_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_stockprop
    ADD CONSTRAINT nd_experiment_stockprop_nd_experiment_stock_id_fkey FOREIGN KEY (nd_experiment_stock_id) REFERENCES nd_experiment_stock(nd_experiment_stock_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment_stockprop
    ADD CONSTRAINT nd_experiment_stockprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experiment
    ADD CONSTRAINT nd_experiment_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experimentprop
    ADD CONSTRAINT nd_experimentprop_nd_experiment_id_fkey FOREIGN KEY (nd_experiment_id) REFERENCES nd_experiment(nd_experiment_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_experimentprop
    ADD CONSTRAINT nd_experimentprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_geolocationprop
    ADD CONSTRAINT nd_geolocationprop_nd_geolocation_id_fkey FOREIGN KEY (nd_geolocation_id) REFERENCES nd_geolocation(nd_geolocation_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_geolocationprop
    ADD CONSTRAINT nd_geolocationprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_protocol_reagent
    ADD CONSTRAINT nd_protocol_reagent_nd_protocol_id_fkey FOREIGN KEY (nd_protocol_id) REFERENCES nd_protocol(nd_protocol_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_protocol_reagent
    ADD CONSTRAINT nd_protocol_reagent_reagent_id_fkey FOREIGN KEY (reagent_id) REFERENCES nd_reagent(nd_reagent_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_protocol_reagent
    ADD CONSTRAINT nd_protocol_reagent_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_protocol
    ADD CONSTRAINT nd_protocol_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_protocolprop
    ADD CONSTRAINT nd_protocolprop_nd_protocol_id_fkey FOREIGN KEY (nd_protocol_id) REFERENCES nd_protocol(nd_protocol_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_protocolprop
    ADD CONSTRAINT nd_protocolprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_reagent
    ADD CONSTRAINT nd_reagent_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_reagent_relationship
    ADD CONSTRAINT nd_reagent_relationship_object_reagent_id_fkey FOREIGN KEY (object_reagent_id) REFERENCES nd_reagent(nd_reagent_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_reagent_relationship
    ADD CONSTRAINT nd_reagent_relationship_subject_reagent_id_fkey FOREIGN KEY (subject_reagent_id) REFERENCES nd_reagent(nd_reagent_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_reagent_relationship
    ADD CONSTRAINT nd_reagent_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_reagent
    ADD CONSTRAINT nd_reagent_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_reagentprop
    ADD CONSTRAINT nd_reagentprop_nd_reagent_id_fkey FOREIGN KEY (nd_reagent_id) REFERENCES nd_reagent(nd_reagent_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY nd_reagentprop
    ADD CONSTRAINT nd_reagentprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY organism_cvterm
    ADD CONSTRAINT organism_cvterm_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY organism_cvterm
    ADD CONSTRAINT organism_cvterm_organism_id_fkey FOREIGN KEY (organism_id) REFERENCES organism(organism_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY organism_cvterm
    ADD CONSTRAINT organism_cvterm_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY organism_cvtermprop
    ADD CONSTRAINT organism_cvtermprop_organism_cvterm_id_fkey FOREIGN KEY (organism_cvterm_id) REFERENCES organism_cvterm(organism_cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY organism_cvtermprop
    ADD CONSTRAINT organism_cvtermprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY organism_dbxref
    ADD CONSTRAINT organism_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY organism_dbxref
    ADD CONSTRAINT organism_dbxref_organism_id_fkey FOREIGN KEY (organism_id) REFERENCES organism(organism_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY organism_pub
    ADD CONSTRAINT organism_pub_organism_id_fkey FOREIGN KEY (organism_id) REFERENCES organism(organism_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY organism_pub
    ADD CONSTRAINT organism_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY organism_relationship
    ADD CONSTRAINT organism_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES organism(organism_id) ON DELETE CASCADE;

ALTER TABLE ONLY organism_relationship
    ADD CONSTRAINT organism_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES organism(organism_id) ON DELETE CASCADE;

ALTER TABLE ONLY organism_relationship
    ADD CONSTRAINT organism_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY organism
    ADD CONSTRAINT organism_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY organismprop
    ADD CONSTRAINT organismprop_organism_id_fkey FOREIGN KEY (organism_id) REFERENCES organism(organism_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY organismprop_pub
    ADD CONSTRAINT organismprop_pub_organismprop_id_fkey FOREIGN KEY (organismprop_id) REFERENCES organismprop(organismprop_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY organismprop_pub
    ADD CONSTRAINT organismprop_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY organismprop
    ADD CONSTRAINT organismprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY phendesc
    ADD CONSTRAINT phendesc_environment_id_fkey FOREIGN KEY (environment_id) REFERENCES environment(environment_id) ON DELETE CASCADE;

ALTER TABLE ONLY phendesc
    ADD CONSTRAINT phendesc_genotype_id_fkey FOREIGN KEY (genotype_id) REFERENCES genotype(genotype_id) ON DELETE CASCADE;

ALTER TABLE ONLY phendesc
    ADD CONSTRAINT phendesc_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE;

ALTER TABLE ONLY phendesc
    ADD CONSTRAINT phendesc_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotype
    ADD CONSTRAINT phenotype_assay_id_fkey FOREIGN KEY (assay_id) REFERENCES cvterm(cvterm_id) ON DELETE SET NULL;

ALTER TABLE ONLY phenotype
    ADD CONSTRAINT phenotype_attr_id_fkey FOREIGN KEY (attr_id) REFERENCES cvterm(cvterm_id) ON DELETE SET NULL;

ALTER TABLE ONLY phenotype_comparison_cvterm
    ADD CONSTRAINT phenotype_comparison_cvterm_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotype_comparison_cvterm
    ADD CONSTRAINT phenotype_comparison_cvterm_phenotype_comparison_id_fkey FOREIGN KEY (phenotype_comparison_id) REFERENCES phenotype_comparison(phenotype_comparison_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotype_comparison_cvterm
    ADD CONSTRAINT phenotype_comparison_cvterm_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotype_comparison
    ADD CONSTRAINT phenotype_comparison_environment1_id_fkey FOREIGN KEY (environment1_id) REFERENCES environment(environment_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotype_comparison
    ADD CONSTRAINT phenotype_comparison_environment2_id_fkey FOREIGN KEY (environment2_id) REFERENCES environment(environment_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotype_comparison
    ADD CONSTRAINT phenotype_comparison_genotype1_id_fkey FOREIGN KEY (genotype1_id) REFERENCES genotype(genotype_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotype_comparison
    ADD CONSTRAINT phenotype_comparison_genotype2_id_fkey FOREIGN KEY (genotype2_id) REFERENCES genotype(genotype_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotype_comparison
    ADD CONSTRAINT phenotype_comparison_organism_id_fkey FOREIGN KEY (organism_id) REFERENCES organism(organism_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotype_comparison
    ADD CONSTRAINT phenotype_comparison_phenotype1_id_fkey FOREIGN KEY (phenotype1_id) REFERENCES phenotype(phenotype_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotype_comparison
    ADD CONSTRAINT phenotype_comparison_phenotype2_id_fkey FOREIGN KEY (phenotype2_id) REFERENCES phenotype(phenotype_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotype_comparison
    ADD CONSTRAINT phenotype_comparison_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotype
    ADD CONSTRAINT phenotype_cvalue_id_fkey FOREIGN KEY (cvalue_id) REFERENCES cvterm(cvterm_id) ON DELETE SET NULL;

ALTER TABLE ONLY phenotype_cvterm
    ADD CONSTRAINT phenotype_cvterm_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotype_cvterm
    ADD CONSTRAINT phenotype_cvterm_phenotype_id_fkey FOREIGN KEY (phenotype_id) REFERENCES phenotype(phenotype_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotype
    ADD CONSTRAINT phenotype_observable_id_fkey FOREIGN KEY (observable_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenotypeprop
    ADD CONSTRAINT phenotypeprop_phenotype_id_fkey FOREIGN KEY (phenotype_id) REFERENCES phenotype(phenotype_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY phenotypeprop
    ADD CONSTRAINT phenotypeprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY phenstatement
    ADD CONSTRAINT phenstatement_environment_id_fkey FOREIGN KEY (environment_id) REFERENCES environment(environment_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenstatement
    ADD CONSTRAINT phenstatement_genotype_id_fkey FOREIGN KEY (genotype_id) REFERENCES genotype(genotype_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenstatement
    ADD CONSTRAINT phenstatement_phenotype_id_fkey FOREIGN KEY (phenotype_id) REFERENCES phenotype(phenotype_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenstatement
    ADD CONSTRAINT phenstatement_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE;

ALTER TABLE ONLY phenstatement
    ADD CONSTRAINT phenstatement_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonode_dbxref
    ADD CONSTRAINT phylonode_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonode_dbxref
    ADD CONSTRAINT phylonode_dbxref_phylonode_id_fkey FOREIGN KEY (phylonode_id) REFERENCES phylonode(phylonode_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonode
    ADD CONSTRAINT phylonode_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonode_organism
    ADD CONSTRAINT phylonode_organism_organism_id_fkey FOREIGN KEY (organism_id) REFERENCES organism(organism_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonode_organism
    ADD CONSTRAINT phylonode_organism_phylonode_id_fkey FOREIGN KEY (phylonode_id) REFERENCES phylonode(phylonode_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonode
    ADD CONSTRAINT phylonode_parent_phylonode_id_fkey FOREIGN KEY (parent_phylonode_id) REFERENCES phylonode(phylonode_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonode
    ADD CONSTRAINT phylonode_phylotree_id_fkey FOREIGN KEY (phylotree_id) REFERENCES phylotree(phylotree_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonode_pub
    ADD CONSTRAINT phylonode_pub_phylonode_id_fkey FOREIGN KEY (phylonode_id) REFERENCES phylonode(phylonode_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonode_pub
    ADD CONSTRAINT phylonode_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonode_relationship
    ADD CONSTRAINT phylonode_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES phylonode(phylonode_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonode_relationship
    ADD CONSTRAINT phylonode_relationship_phylotree_id_fkey FOREIGN KEY (phylotree_id) REFERENCES phylotree(phylotree_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonode_relationship
    ADD CONSTRAINT phylonode_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES phylonode(phylonode_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonode_relationship
    ADD CONSTRAINT phylonode_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonode
    ADD CONSTRAINT phylonode_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonodeprop
    ADD CONSTRAINT phylonodeprop_phylonode_id_fkey FOREIGN KEY (phylonode_id) REFERENCES phylonode(phylonode_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylonodeprop
    ADD CONSTRAINT phylonodeprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylotree
    ADD CONSTRAINT phylotree_analysis_id_fkey FOREIGN KEY (analysis_id) REFERENCES analysis(analysis_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylotree
    ADD CONSTRAINT phylotree_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylotree_pub
    ADD CONSTRAINT phylotree_pub_phylotree_id_fkey FOREIGN KEY (phylotree_id) REFERENCES phylotree(phylotree_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylotree_pub
    ADD CONSTRAINT phylotree_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylotree
    ADD CONSTRAINT phylotree_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY phylotreeprop
    ADD CONSTRAINT phylotreeprop_phylotree_id_fkey FOREIGN KEY (phylotree_id) REFERENCES phylotree(phylotree_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY phylotreeprop
    ADD CONSTRAINT phylotreeprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY project_analysis
    ADD CONSTRAINT project_analysis_analysis_id_fkey FOREIGN KEY (analysis_id) REFERENCES analysis(analysis_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY project_analysis
    ADD CONSTRAINT project_analysis_project_id_fkey FOREIGN KEY (project_id) REFERENCES project(project_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY project_contact
    ADD CONSTRAINT project_contact_contact_id_fkey FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY project_contact
    ADD CONSTRAINT project_contact_project_id_fkey FOREIGN KEY (project_id) REFERENCES project(project_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY project_dbxref
    ADD CONSTRAINT project_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY project_dbxref
    ADD CONSTRAINT project_dbxref_project_id_fkey FOREIGN KEY (project_id) REFERENCES project(project_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY project_feature
    ADD CONSTRAINT project_feature_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE;

ALTER TABLE ONLY project_feature
    ADD CONSTRAINT project_feature_project_id_fkey FOREIGN KEY (project_id) REFERENCES project(project_id) ON DELETE CASCADE;

ALTER TABLE ONLY project_pub
    ADD CONSTRAINT project_pub_project_id_fkey FOREIGN KEY (project_id) REFERENCES project(project_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY project_pub
    ADD CONSTRAINT project_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY project_relationship
    ADD CONSTRAINT project_relationship_object_project_id_fkey FOREIGN KEY (object_project_id) REFERENCES project(project_id) ON DELETE CASCADE;

ALTER TABLE ONLY project_relationship
    ADD CONSTRAINT project_relationship_subject_project_id_fkey FOREIGN KEY (subject_project_id) REFERENCES project(project_id) ON DELETE CASCADE;

ALTER TABLE ONLY project_relationship
    ADD CONSTRAINT project_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE RESTRICT;

ALTER TABLE ONLY project_stock
    ADD CONSTRAINT project_stock_project_id_fkey FOREIGN KEY (project_id) REFERENCES project(project_id) ON DELETE CASCADE;

ALTER TABLE ONLY project_stock
    ADD CONSTRAINT project_stock_stock_id_fkey FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE;

ALTER TABLE ONLY projectprop
    ADD CONSTRAINT projectprop_project_id_fkey FOREIGN KEY (project_id) REFERENCES project(project_id) ON DELETE CASCADE;

ALTER TABLE ONLY projectprop
    ADD CONSTRAINT projectprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY protocol
    ADD CONSTRAINT protocol_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY protocol
    ADD CONSTRAINT protocol_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY protocol
    ADD CONSTRAINT protocol_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY protocolparam
    ADD CONSTRAINT protocolparam_datatype_id_fkey FOREIGN KEY (datatype_id) REFERENCES cvterm(cvterm_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY protocolparam
    ADD CONSTRAINT protocolparam_protocol_id_fkey FOREIGN KEY (protocol_id) REFERENCES protocol(protocol_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY protocolparam
    ADD CONSTRAINT protocolparam_unittype_id_fkey FOREIGN KEY (unittype_id) REFERENCES cvterm(cvterm_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY pub_dbxref
    ADD CONSTRAINT pub_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY pub_dbxref
    ADD CONSTRAINT pub_dbxref_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY pub_relationship
    ADD CONSTRAINT pub_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY pub_relationship
    ADD CONSTRAINT pub_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY pub_relationship
    ADD CONSTRAINT pub_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY pub
    ADD CONSTRAINT pub_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY pubauthor_contact
    ADD CONSTRAINT pubauthor_contact_contact_id_fkey FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE;

ALTER TABLE ONLY pubauthor_contact
    ADD CONSTRAINT pubauthor_contact_pubauthor_id_fkey FOREIGN KEY (pubauthor_id) REFERENCES pubauthor(pubauthor_id) ON DELETE CASCADE;

ALTER TABLE ONLY pubauthor
    ADD CONSTRAINT pubauthor_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY pubprop
    ADD CONSTRAINT pubprop_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY pubprop
    ADD CONSTRAINT pubprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY quantification
    ADD CONSTRAINT quantification_acquisition_id_fkey FOREIGN KEY (acquisition_id) REFERENCES acquisition(acquisition_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY quantification
    ADD CONSTRAINT quantification_analysis_id_fkey FOREIGN KEY (analysis_id) REFERENCES analysis(analysis_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY quantification
    ADD CONSTRAINT quantification_operator_id_fkey FOREIGN KEY (operator_id) REFERENCES contact(contact_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY quantification
    ADD CONSTRAINT quantification_protocol_id_fkey FOREIGN KEY (protocol_id) REFERENCES protocol(protocol_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY quantification_relationship
    ADD CONSTRAINT quantification_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES quantification(quantification_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY quantification_relationship
    ADD CONSTRAINT quantification_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES quantification(quantification_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY quantification_relationship
    ADD CONSTRAINT quantification_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY quantificationprop
    ADD CONSTRAINT quantificationprop_quantification_id_fkey FOREIGN KEY (quantification_id) REFERENCES quantification(quantification_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY quantificationprop
    ADD CONSTRAINT quantificationprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_cvterm
    ADD CONSTRAINT stock_cvterm_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_cvterm
    ADD CONSTRAINT stock_cvterm_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_cvterm
    ADD CONSTRAINT stock_cvterm_stock_id_fkey FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_cvtermprop
    ADD CONSTRAINT stock_cvtermprop_stock_cvterm_id_fkey FOREIGN KEY (stock_cvterm_id) REFERENCES stock_cvterm(stock_cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY stock_cvtermprop
    ADD CONSTRAINT stock_cvtermprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_dbxref
    ADD CONSTRAINT stock_dbxref_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock
    ADD CONSTRAINT stock_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_dbxref
    ADD CONSTRAINT stock_dbxref_stock_id_fkey FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_dbxrefprop
    ADD CONSTRAINT stock_dbxrefprop_stock_dbxref_id_fkey FOREIGN KEY (stock_dbxref_id) REFERENCES stock_dbxref(stock_dbxref_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_dbxrefprop
    ADD CONSTRAINT stock_dbxrefprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_feature
    ADD CONSTRAINT stock_feature_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_feature
    ADD CONSTRAINT stock_feature_stock_id_fkey FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_feature
    ADD CONSTRAINT stock_feature_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_featuremap
    ADD CONSTRAINT stock_featuremap_featuremap_id_fkey FOREIGN KEY (featuremap_id) REFERENCES featuremap(featuremap_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_featuremap
    ADD CONSTRAINT stock_featuremap_stock_id_fkey FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_featuremap
    ADD CONSTRAINT stock_featuremap_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_genotype
    ADD CONSTRAINT stock_genotype_genotype_id_fkey FOREIGN KEY (genotype_id) REFERENCES genotype(genotype_id) ON DELETE CASCADE;

ALTER TABLE ONLY stock_genotype
    ADD CONSTRAINT stock_genotype_stock_id_fkey FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE;

ALTER TABLE ONLY stock_library
    ADD CONSTRAINT stock_library_library_id_fkey FOREIGN KEY (library_id) REFERENCES library(library_id) ON DELETE CASCADE;

ALTER TABLE ONLY stock_library
    ADD CONSTRAINT stock_library_stock_id_fkey FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE;

ALTER TABLE ONLY stock
    ADD CONSTRAINT stock_organism_id_fkey FOREIGN KEY (organism_id) REFERENCES organism(organism_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_pub
    ADD CONSTRAINT stock_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_pub
    ADD CONSTRAINT stock_pub_stock_id_fkey FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_relationship_cvterm
    ADD CONSTRAINT stock_relationship_cvterm_cvterm_id_fkey FOREIGN KEY (cvterm_id) REFERENCES cvterm(cvterm_id) ON DELETE RESTRICT;

ALTER TABLE ONLY stock_relationship_cvterm
    ADD CONSTRAINT stock_relationship_cvterm_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE RESTRICT;

ALTER TABLE ONLY stock_relationship_cvterm
    ADD CONSTRAINT stock_relationship_cvterm_stock_relationship_id_fkey FOREIGN KEY (stock_relationship_id) REFERENCES stock_relationship(stock_relationship_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_relationship
    ADD CONSTRAINT stock_relationship_object_id_fkey FOREIGN KEY (object_id) REFERENCES stock(stock_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_relationship_pub
    ADD CONSTRAINT stock_relationship_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_relationship_pub
    ADD CONSTRAINT stock_relationship_pub_stock_relationship_id_fkey FOREIGN KEY (stock_relationship_id) REFERENCES stock_relationship(stock_relationship_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_relationship
    ADD CONSTRAINT stock_relationship_subject_id_fkey FOREIGN KEY (subject_id) REFERENCES stock(stock_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock_relationship
    ADD CONSTRAINT stock_relationship_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stock
    ADD CONSTRAINT stock_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stockcollection
    ADD CONSTRAINT stockcollection_contact_id_fkey FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stockcollection_db
    ADD CONSTRAINT stockcollection_db_db_id_fkey FOREIGN KEY (db_id) REFERENCES db(db_id) ON DELETE CASCADE;

ALTER TABLE ONLY stockcollection_db
    ADD CONSTRAINT stockcollection_db_stockcollection_id_fkey FOREIGN KEY (stockcollection_id) REFERENCES stockcollection(stockcollection_id) ON DELETE CASCADE;

ALTER TABLE ONLY stockcollection_stock
    ADD CONSTRAINT stockcollection_stock_stock_id_fkey FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stockcollection_stock
    ADD CONSTRAINT stockcollection_stock_stockcollection_id_fkey FOREIGN KEY (stockcollection_id) REFERENCES stockcollection(stockcollection_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stockcollection
    ADD CONSTRAINT stockcollection_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY stockcollectionprop
    ADD CONSTRAINT stockcollectionprop_stockcollection_id_fkey FOREIGN KEY (stockcollection_id) REFERENCES stockcollection(stockcollection_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stockcollectionprop
    ADD CONSTRAINT stockcollectionprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id);

ALTER TABLE ONLY stockprop_pub
    ADD CONSTRAINT stockprop_pub_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stockprop_pub
    ADD CONSTRAINT stockprop_pub_stockprop_id_fkey FOREIGN KEY (stockprop_id) REFERENCES stockprop(stockprop_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stockprop
    ADD CONSTRAINT stockprop_stock_id_fkey FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY stockprop
    ADD CONSTRAINT stockprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY study_assay
    ADD CONSTRAINT study_assay_assay_id_fkey FOREIGN KEY (assay_id) REFERENCES assay(assay_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY study_assay
    ADD CONSTRAINT study_assay_study_id_fkey FOREIGN KEY (study_id) REFERENCES study(study_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY study
    ADD CONSTRAINT study_contact_id_fkey FOREIGN KEY (contact_id) REFERENCES contact(contact_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY study
    ADD CONSTRAINT study_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES dbxref(dbxref_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY study
    ADD CONSTRAINT study_pub_id_fkey FOREIGN KEY (pub_id) REFERENCES pub(pub_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY studydesign
    ADD CONSTRAINT studydesign_study_id_fkey FOREIGN KEY (study_id) REFERENCES study(study_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY studydesignprop
    ADD CONSTRAINT studydesignprop_studydesign_id_fkey FOREIGN KEY (studydesign_id) REFERENCES studydesign(studydesign_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY studydesignprop
    ADD CONSTRAINT studydesignprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY studyfactor
    ADD CONSTRAINT studyfactor_studydesign_id_fkey FOREIGN KEY (studydesign_id) REFERENCES studydesign(studydesign_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY studyfactor
    ADD CONSTRAINT studyfactor_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY studyfactorvalue
    ADD CONSTRAINT studyfactorvalue_assay_id_fkey FOREIGN KEY (assay_id) REFERENCES assay(assay_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY studyfactorvalue
    ADD CONSTRAINT studyfactorvalue_studyfactor_id_fkey FOREIGN KEY (studyfactor_id) REFERENCES studyfactor(studyfactor_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY studyprop_feature
    ADD CONSTRAINT studyprop_feature_feature_id_fkey FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE;

ALTER TABLE ONLY studyprop_feature
    ADD CONSTRAINT studyprop_feature_studyprop_id_fkey FOREIGN KEY (studyprop_id) REFERENCES studyprop(studyprop_id) ON DELETE CASCADE;

ALTER TABLE ONLY studyprop_feature
    ADD CONSTRAINT studyprop_feature_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY studyprop
    ADD CONSTRAINT studyprop_study_id_fkey FOREIGN KEY (study_id) REFERENCES study(study_id) ON DELETE CASCADE;

ALTER TABLE ONLY studyprop
    ADD CONSTRAINT studyprop_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE;

ALTER TABLE ONLY synonym
    ADD CONSTRAINT synonym_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY treatment
    ADD CONSTRAINT treatment_biomaterial_id_fkey FOREIGN KEY (biomaterial_id) REFERENCES biomaterial(biomaterial_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY treatment
    ADD CONSTRAINT treatment_protocol_id_fkey FOREIGN KEY (protocol_id) REFERENCES protocol(protocol_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;

ALTER TABLE ONLY treatment
    ADD CONSTRAINT treatment_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;
    
---
--- The following additions are part of the pull request on Chado:
--- https://github.com/GMOD/Chado/pull/105 
--- Once a new version of Chado is made these changes will be integrated
--- but we need them now to prevent create_point errors when deleting features
--- or cloning Chado.
---

CREATE OR REPLACE FUNCTION fill_cvtermpath(INTEGER) RETURNS INTEGER
    LANGUAGE plpgsql
    AS $_$
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
$_$;

CREATE OR REPLACE FUNCTION fill_cvtermpath(cv.name%TYPE) RETURNS INTEGER
    LANGUAGE plpgsql
    AS $_$
DECLARE
    cvname alias for $1;
    cv_id   int;
    rtn     int;
BEGIN
    SELECT INTO cv_id cv.cv_id from cv WHERE cv.name = cvname;
    SELECT INTO rtn fill_cvtermpath(cv_id);
    RETURN rtn;
END;
$_$;

-- create a range box
-- (make this immutable so we can index it)
CREATE OR REPLACE FUNCTION boxrange (bigint, bigint) RETURNS box AS
 'SELECT box (create_point(CAST(0 AS bigint), $1), create_point($2,500000000))'
LANGUAGE 'sql' IMMUTABLE SET SEARCH_PATH FROM CURRENT;

CREATE OR REPLACE FUNCTION boxrange (bigint, bigint, bigint) RETURNS box AS
 'SELECT box (create_point($1, $2), create_point($1,$3))'
LANGUAGE 'sql' IMMUTABLE SET SEARCH_PATH FROM CURRENT;


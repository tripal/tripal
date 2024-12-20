-- ==========================================
-- Frange Schema FUNCTIONS
-- Meant to be in public schema
-- =================================================================

--- create ontology that has instantiated located_sequence_feature part of SO
--- way as it is written, the function can not be execute more than once in one connection
--- when you get error like ERROR:  relation with OID NNNNN does not exist
--- as this is not meant to execute >1 times in one session so it should never happen
--- except at testing and test failed
--- disconnect and try again, in other words, it can NOT be executed >1 time in one connection
--- if using EXECUTE, we can avoid this problem but code is hard to write and read (lots of ', escape char)

--NOTE: private, don't call directly as relying on having temp table tmpcvtr

--DROP TYPE soi_type CASCADE;
CREATE TYPE soi_type AS (
    type_id bigint,
    subject_id bigint,
    object_id bigint
);

CREATE OR REPLACE FUNCTION _fill_cvtermpath4soinode(INTEGER, INTEGER, INTEGER, INTEGER, INTEGER) RETURNS INTEGER AS
'
DECLARE
    origin alias for $1;
    child_id alias for $2;
    cvid alias for $3;
    typeid alias for $4;
    depth alias for $5;
    cterm soi_type%ROWTYPE;
    exist_c int;

BEGIN

    --RAISE NOTICE ''depth=% o=%, root=%, cv=%, t=%'', depth,origin,child_id,cvid,typeid;
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
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION _fill_cvtermpath4soi(INTEGER, INTEGER) RETURNS INTEGER AS
'
DECLARE
    rootid alias for $1;
    cvid alias for $2;
    ttype int;
    cterm soi_type%ROWTYPE;

BEGIN

    SELECT INTO ttype cvterm_id FROM cvterm WHERE name = ''isa'';
    --RAISE NOTICE ''got ttype %'',ttype;
    PERFORM _fill_cvtermpath4soinode(rootid, rootid, cvid, ttype, 0);
    FOR cterm IN SELECT tmp_type AS type_id, subject_id FROM tmpcvtr WHERE object_id = rootid LOOP
        PERFORM _fill_cvtermpath4soi(cterm.subject_id, cvid);
    END LOOP;
    RETURN 1;
END;
'
LANGUAGE 'plpgsql';

--- use tmpcvtr to temp store soi (virtural ontology)
--- using tmp tables is faster than using recursive function to create feature type relationship
--- since it gets feature type rel set by set instead of one by one
--- and getting feature type rel is very expensive
--- call _fillcvtermpath4soi to create path for the virtual ontology

CREATE OR REPLACE FUNCTION create_soi() RETURNS INTEGER AS
'
DECLARE
    parent soi_type%ROWTYPE;
    isa_id cvterm.cvterm_id%TYPE;
    soi_term TEXT := ''soi'';
    soi_def TEXT := ''ontology of SO feature instantiated in database'';
    soi_cvid bigint;
    soiterm_id bigint;
    pcount INTEGER;
    count INTEGER := 0;
    cquery TEXT;
BEGIN

    SELECT INTO isa_id cvterm_id FROM cvterm WHERE name = ''isa'';

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

    CREATE TEMP TABLE tmpcvtr (tmp_type INT, type_id bigint, subject_id bigint, object_id bigint);
    CREATE UNIQUE INDEX u_tmpcvtr ON tmpcvtr(subject_id, object_id);

    INSERT INTO tmpcvtr (tmp_type, type_id, subject_id, object_id)
        SELECT DISTINCT isa_id, soiterm_id, f.type_id, soiterm_id FROM feature f, cvterm t
        WHERE f.type_id = t.cvterm_id AND f.type_id > 0;
    EXECUTE ''select * from tmpcvtr where type_id = '' || soiterm_id || '';'';
    get diagnostics pcount = row_count;
    raise notice ''all types in feature %'',pcount;
--- do it hard way, delete any child feature type from above (NOT IN clause did not work)
    FOR parent IN SELECT DISTINCT 0, t.cvterm_id, 0 FROM feature c, feature_relationship fr, cvterm t
            WHERE t.cvterm_id = c.type_id AND c.feature_id = fr.subject_id LOOP
        DELETE FROM tmpcvtr WHERE type_id = soiterm_id and object_id = soiterm_id
            AND subject_id = parent.subject_id;
    END LOOP;
    EXECUTE ''select * from tmpcvtr where type_id = '' || soiterm_id || '';'';
    get diagnostics pcount = row_count;
    raise notice ''all types in feature after delete child %'',pcount;

    --- create feature type relationship (store in tmpcvtr)
    CREATE TEMP TABLE tmproot (cv_id bigint not null, cvterm_id bigint not null, status INTEGER DEFAULT 0);
    cquery := ''SELECT * FROM tmproot tmp WHERE tmp.status = 0;'';
    ---temp use tmpcvtr to hold instantiated SO relationship for speed
    ---use soterm_id as type_id, will delete from tmpcvtr
    ---us tmproot for this as well
    INSERT INTO tmproot (cv_id, cvterm_id, status) SELECT DISTINCT soi_cvid, c.subject_id, 0 FROM tmpcvtr c
        WHERE c.object_id = soiterm_id;
    EXECUTE cquery;
    GET DIAGNOSTICS pcount = ROW_COUNT;
    WHILE (pcount > 0) LOOP
        RAISE NOTICE ''num child temp (to be inserted) in tmpcvtr: %'',pcount;
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
'
LANGUAGE 'plpgsql';

---bad precedence: change customed type name
---drop here to remove old function
--DROP TYPE feature_by_cvt_type CASCADE;
--DROP TYPE fxgsfids_type CASCADE;

--DROP TYPE feature_by_fx_type CASCADE;
CREATE TYPE feature_by_fx_type AS (
    feature_id bigint,
    depth INT
);

CREATE OR REPLACE FUNCTION get_sub_feature_ids(text) RETURNS SETOF feature_by_fx_type AS
'
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
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_up_feature_ids(text) RETURNS SETOF feature_by_fx_type AS
'
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
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_feature_ids(text) RETURNS SETOF feature_by_fx_type AS
'
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
'
LANGUAGE 'plpgsql';


CREATE OR REPLACE FUNCTION get_sub_feature_ids(integer) RETURNS SETOF feature_by_fx_type AS
'
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
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_up_feature_ids(integer) RETURNS SETOF feature_by_fx_type AS
'
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
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_sub_feature_ids(integer, integer) RETURNS SETOF feature_by_fx_type AS
'
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
'
LANGUAGE 'plpgsql';

--- depth is reversed and meanless when union with results from get_sub_feature_ids
CREATE OR REPLACE FUNCTION get_up_feature_ids(integer, integer) RETURNS SETOF feature_by_fx_type AS
'
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
'
LANGUAGE 'plpgsql';

--- children feature ids only (not include itself--parent) for SO type and range (src)
CREATE OR REPLACE FUNCTION get_sub_feature_ids_by_type_src(cvterm.name%TYPE,feature.uniquename%TYPE,char(1)) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    gtype alias for $1;
    src alias for $2;
    is_an alias for $3;
    query text;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT f.feature_id FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id)
        INNER join featureloc fl
        ON (f.feature_id = fl.feature_id) INNER join feature src ON (src.feature_id = fl.srcfeature_id)
        WHERE t.name = '' || quote_literal(gtype) || '' AND src.uniquename = '' || quote_literal(src)
        || '' AND f.is_analysis = '' || quote_literal(is_an) || '';'';

    IF (STRPOS(gtype, ''%'') > 0) THEN
        query := ''SELECT DISTINCT f.feature_id FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id)
             INNER join featureloc fl
            ON (f.feature_id = fl.feature_id) INNER join feature src ON (src.feature_id = fl.srcfeature_id)
            WHERE t.name like '' || quote_literal(gtype) || '' AND src.uniquename = '' || quote_literal(src)
            || '' AND f.is_analysis = '' || quote_literal(is_an) || '';'';
    END IF;
    FOR myrc IN SELECT * FROM get_sub_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

--- by SO type, usefull for tRNA, ncRNA, etc
CREATE OR REPLACE FUNCTION get_feature_ids_by_type(cvterm.name%TYPE, char(1)) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    gtype alias for $1;
    is_an alias for $2;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT f.feature_id
        FROM feature f, cvterm t WHERE t.cvterm_id = f.type_id AND t.name = '' || quote_literal(gtype) ||
        '' AND f.is_analysis = '' || quote_literal(is_an) || '';'';
    IF (STRPOS(gtype, ''%'') > 0) THEN
        query := ''SELECT DISTINCT f.feature_id
            FROM feature f, cvterm t WHERE t.cvterm_id = f.type_id AND t.name like ''
            || quote_literal(gtype) || '' AND f.is_analysis = '' || quote_literal(is_an) || '';'';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_feature_ids_by_type_src(cvterm.name%TYPE, feature.uniquename%TYPE, char(1)) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    gtype alias for $1;
    src alias for $2;
    is_an alias for $3;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT f.feature_id
        FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id) INNER join featureloc fl
        ON (f.feature_id = fl.feature_id) INNER join feature src ON (src.feature_id = fl.srcfeature_id)
        WHERE t.name = '' || quote_literal(gtype) || '' AND src.uniquename = '' || quote_literal(src)
        || '' AND f.is_analysis = '' || quote_literal(is_an) || '';'';

    IF (STRPOS(gtype, ''%'') > 0) THEN
        query := ''SELECT DISTINCT f.feature_id
            FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id) INNER join featureloc fl
            ON (f.feature_id = fl.feature_id) INNER join feature src ON (src.feature_id = fl.srcfeature_id)
            WHERE t.name like '' || quote_literal(gtype) || '' AND src.uniquename = '' || quote_literal(src)
            || '' AND f.is_analysis = '' || quote_literal(is_an) || '';'';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_feature_ids_by_type_name(cvterm.name%TYPE, feature.uniquename%TYPE, char(1)) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    gtype alias for $1;
    name alias for $2;
    is_an alias for $3;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT f.feature_id
        FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id)
        WHERE t.name = '' || quote_literal(gtype) || '' AND (f.uniquename = '' || quote_literal(name)
        || '' OR f.name = '' || quote_literal(name) || '') AND f.is_analysis = '' || quote_literal(is_an) || '';'';

    IF (STRPOS(name, ''%'') > 0) THEN
        query := ''SELECT DISTINCT f.feature_id
            FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id)
            WHERE t.name = '' || quote_literal(gtype) || '' AND (f.uniquename like '' || quote_literal(name)
            || '' OR f.name like '' || quote_literal(name) || '') AND f.is_analysis = '' || quote_literal(is_an) || '';'';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

--- get all feature ids (including children) for feature that has an ontology term (say GO function)
CREATE OR REPLACE FUNCTION get_feature_ids_by_ont(cv.name%TYPE,cvterm.name%TYPE) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    aspect alias for $1;
    term alias for $2;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT fcvt.feature_id
        FROM feature_cvterm fcvt, cv, cvterm t WHERE cv.cv_id = t.cv_id AND
        t.cvterm_id = fcvt.cvterm_id AND cv.name = '' || quote_literal(aspect) ||
        '' AND t.name = '' || quote_literal(term) || '';'';
    IF (STRPOS(term, ''%'') > 0) THEN
        query := ''SELECT DISTINCT fcvt.feature_id
            FROM feature_cvterm fcvt, cv, cvterm t WHERE cv.cv_id = t.cv_id AND
            t.cvterm_id = fcvt.cvterm_id AND cv.name = '' || quote_literal(aspect) ||
            '' AND t.name like '' || quote_literal(term) || '';'';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_feature_ids_by_ont_root(cv.name%TYPE,cvterm.name%TYPE) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    aspect alias for $1;
    term alias for $2;
    query TEXT;
    subquery TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    subquery := ''SELECT t.cvterm_id FROM cv, cvterm t WHERE cv.cv_id = t.cv_id
        AND cv.name = '' || quote_literal(aspect) || '' AND t.name = '' || quote_literal(term) || '';'';
    IF (STRPOS(term, ''%'') > 0) THEN
        subquery := ''SELECT t.cvterm_id FROM cv, cvterm t WHERE cv.cv_id = t.cv_id
            AND cv.name = '' || quote_literal(aspect) || '' AND t.name like '' || quote_literal(term) || '';'';
    END IF;
    query := ''SELECT DISTINCT fcvt.feature_id
        FROM feature_cvterm fcvt INNER JOIN (SELECT cvterm_id FROM get_it_sub_cvterm_ids('' || quote_literal(subquery) || '')) AS ont ON (fcvt.cvterm_id = ont.cvterm_id);'';

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

--- get all feature ids (including children) for feature with the property (type, val)
CREATE OR REPLACE FUNCTION get_feature_ids_by_property(cvterm.name%TYPE,varchar) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    p_type alias for $1;
    p_val alias for $2;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT fprop.feature_id
        FROM featureprop fprop, cvterm t WHERE t.cvterm_id = fprop.type_id AND t.name = '' ||
        quote_literal(p_type) || '' AND fprop.value = '' || quote_literal(p_val) || '';'';
    IF (STRPOS(p_val, ''%'') > 0) THEN
        query := ''SELECT DISTINCT fprop.feature_id
            FROM featureprop fprop, cvterm t WHERE t.cvterm_id = fprop.type_id AND t.name = '' ||
            quote_literal(p_type) || '' AND fprop.value like '' || quote_literal(p_val) || '';'';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

--- get all feature ids (including children) for feature with the property val
CREATE OR REPLACE FUNCTION get_feature_ids_by_propval(varchar) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    p_val alias for $1;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT fprop.feature_id
        FROM featureprop fprop WHERE fprop.value = '' || quote_literal(p_val) || '';'';
    IF (STRPOS(p_val, ''%'') > 0) THEN
        query := ''SELECT DISTINCT fprop.feature_id
            FROM featureprop fprop WHERE fprop.value like '' || quote_literal(p_val) || '';'';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';


---4 args: ptype, ctype, count, operator (valid SQL number comparison operator), and is_analysis
---get feature ids for any node with type = ptype whose child node type = ctype
---and child node feature count comparing (using operator) to ccount
CREATE OR REPLACE FUNCTION get_feature_ids_by_child_count(cvterm.name%TYPE, cvterm.name%TYPE, INTEGER, varchar, char(1)) RETURNS SETOF feature_by_fx_type AS
'
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

    query := ''SELECT DISTINCT f.feature_id
        FROM feature f INNER join (select count(*) as c, p.feature_id FROM feature p
        INNER join cvterm pt ON (p.type_id = pt.cvterm_id) INNER join feature_relationship fr
        ON (p.feature_id = fr.object_id) INNER join feature c ON (c.feature_id = fr.subject_id)
        INNER join cvterm ct ON (c.type_id = ct.cvterm_id)
        WHERE pt.name = '' || quote_literal(ptype) || '' AND ct.name = '' || quote_literal(ctype)
        || '' AND p.is_analysis = '' || quote_literal(is_an) || '' group by p.feature_id) as cq
        ON (cq.feature_id = f.feature_id) WHERE cq.c '' || operator || ccount || '';'';
    ---RAISE NOTICE ''%'', query;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

/*
 * Set the search_path to the current_schema
 * so that we can use SET SEARCH_PATH FROM CURRENT
 * in function definitions.
 * 
 * This fix addresses a problem that was introduced
 * by [CVE-2018-1058](https://nvd.nist.gov/vuln/detail/CVE-2018-1058).
 *
 * https://github.com/GMOD/Chado/issues/65
 *
 */
SELECT set_config('search_path',
  string_agg(quote_ident(s),','),
  false)
FROM unnest(current_schemas(false)) s;

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
LANGUAGE 'plpgsql' SET SEARCH_PATH FROM CURRENT;

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
LANGUAGE 'plpgsql' SET SEARCH_PATH FROM CURRENT;

-- create a range box
-- (make this immutable so we can index it)
CREATE OR REPLACE FUNCTION boxrange (bigint, bigint) RETURNS box AS
 'SELECT box (create_point(CAST(0 AS bigint), $1), create_point($2,500000000))'
LANGUAGE 'sql' IMMUTABLE SET SEARCH_PATH FROM CURRENT;

CREATE OR REPLACE FUNCTION boxrange (bigint, bigint, bigint) RETURNS box AS
 'SELECT box (create_point($1, $2), create_point($1,$3))'
LANGUAGE 'sql' IMMUTABLE SET SEARCH_PATH FROM CURRENT;

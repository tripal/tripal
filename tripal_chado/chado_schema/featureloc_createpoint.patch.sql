
--
-- Patch to ensure these functions are in the public schema as intented by Chado.
-- Sometimes this is not the case after using the Tripal installer,
-- so this patch will be applied to fix that.
--
-- How to apply patch manually:
-- drush sql-query --file=featureloc_createpoint.patch.sql
--

SET search_path = public,pg_catalog;
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
 'SELECT box (create_point(0, $1), create_point($2,500000000))'
LANGUAGE 'sql' IMMUTABLE;

-- create a query box
CREATE OR REPLACE FUNCTION boxquery (bigint, bigint) RETURNS box AS
 'SELECT box (create_point($1, $2), create_point($1, $2))'
LANGUAGE 'sql' IMMUTABLE;

--functional index that depends on the above functions
DROP INDEX IF EXISTS chado.binloc_boxrange;
CREATE INDEX binloc_boxrange ON chado.featureloc USING GIST (boxrange(fmin, fmax));

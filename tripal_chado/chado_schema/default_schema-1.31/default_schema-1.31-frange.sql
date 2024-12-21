--
-- Note by Lacey-anne Sanderson:
-- File contains the Frange schema.
--
CREATE SCHEMA frange;
SET search_path = frange,public,pg_catalog;

CREATE TABLE featuregroup (
    featuregroup_id bigserial not null,
    primary key (featuregroup_id),

    subject_id bigint not null,
    foreign key (subject_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,

    object_id bigint not null,
    foreign key (object_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,

    group_id bigint not null,
    foreign key (group_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,

    srcfeature_id bigint null,
    foreign key (srcfeature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,

    fmin bigint null,
    fmax bigint null,
    strand int null,
    is_root int not null default 0,

    constraint featuregroup_c1 unique (subject_id,object_id,group_id,srcfeature_id,fmin,fmax,strand)
);
CREATE INDEX featuregroup_idx1 ON featuregroup (subject_id);
CREATE INDEX featuregroup_idx2 ON featuregroup (object_id);
CREATE INDEX featuregroup_idx3 ON featuregroup (group_id);
CREATE INDEX featuregroup_idx4 ON featuregroup (srcfeature_id);
CREATE INDEX featuregroup_idx5 ON featuregroup (strand);
CREATE INDEX featuregroup_idx6 ON featuregroup (is_root);

CREATE OR REPLACE FUNCTION groupoverlaps(bigint, bigint, varchar) RETURNS setof featuregroup AS '
  SELECT g2.*
  FROM  featuregroup g1,
        featuregroup g2
  WHERE g1.is_root = 1
    AND ( g1.srcfeature_id = g2.srcfeature_id OR g2.srcfeature_id IS NULL )
    AND g1.group_id = g2.group_id
    AND g1.srcfeature_id = (SELECT feature_id FROM feature WHERE uniquename = $3)
    AND boxquery($1, $2) @ boxrange(g1.fmin,g2.fmax)
' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION groupcontains(bigint, bigint, varchar) RETURNS setof featuregroup AS '
  SELECT *
  FROM groupoverlaps($1,$2,$3)
  WHERE fmin <= $1 AND fmax >= $2
' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION groupinside(bigint, bigint, varchar) RETURNS setof featuregroup AS '
  SELECT *
  FROM groupoverlaps($1,$2,$3)
  WHERE fmin >= $1 AND fmax <= $2
' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION groupidentical(bigint, bigint, varchar) RETURNS setof featuregroup AS '
  SELECT *
  FROM groupoverlaps($1,$2,$3)
  WHERE fmin = $1 AND fmax = $2
' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION groupoverlaps(bigint, bigint) RETURNS setof featuregroup AS '
  SELECT *
  FROM featuregroup
  WHERE is_root = 1
    AND boxquery($1, $2) @ boxrange(fmin,fmax)
' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION groupoverlaps(_int8, _int8, _varchar) RETURNS setof featuregroup AS '
DECLARE
    mins alias for $1;
    maxs alias for $2;
    srcs alias for $3;
    f featuregroup%ROWTYPE;
    i int;
    s int;
BEGIN
    i := 1;
    FOR i in array_lower( mins, 1 ) .. array_upper( mins, 1 ) LOOP
        SELECT INTO s feature_id FROM feature WHERE uniquename = srcs[i];
        FOR f IN
            SELECT *
            FROM  featuregroup WHERE group_id IN (
                SELECT group_id FROM featuregroup
                WHERE (srcfeature_id = s OR srcfeature_id IS NULL)
                  AND group_id IN (
                      SELECT group_id FROM groupoverlaps( mins[i], maxs[i] )
                      WHERE  srcfeature_id = s
                  )
            )
        LOOP
            RETURN NEXT f;
        END LOOP;
    END LOOP;
    RETURN;
END;
' LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION groupcontains(_int8, _int8, _varchar) RETURNS setof featuregroup AS '
DECLARE
    mins alias for $1;
    maxs alias for $2;
    srcs alias for $3;
    f featuregroup%ROWTYPE;
    i int;
    s int;
BEGIN
    i := 1;
    FOR i in array_lower( mins, 1 ) .. array_upper( mins, 1 ) LOOP
        SELECT INTO s feature_id FROM feature WHERE uniquename = srcs[i];
        FOR f IN
            SELECT *
            FROM  featuregroup WHERE group_id IN (
                SELECT group_id FROM featuregroup
                WHERE (srcfeature_id = s OR srcfeature_id IS NULL)
                  AND fmin <= mins[i]
                  AND fmax >= maxs[i]
                  AND group_id IN (
                      SELECT group_id FROM groupoverlaps( mins[i], maxs[i] )
                      WHERE  srcfeature_id = s
                  )
            )
        LOOP
            RETURN NEXT f;
        END LOOP;
    END LOOP;
    RETURN;
END;
' LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION groupinside(_int8, _int8, _varchar) RETURNS setof featuregroup AS '
DECLARE
    mins alias for $1;
    maxs alias for $2;
    srcs alias for $3;
    f featuregroup%ROWTYPE;
    i int;
    s int;
BEGIN
    i := 1;
    FOR i in array_lower( mins, 1 ) .. array_upper( mins, 1 ) LOOP
        SELECT INTO s feature_id FROM feature WHERE uniquename = srcs[i];
        FOR f IN
            SELECT *
            FROM  featuregroup WHERE group_id IN (
                SELECT group_id FROM featuregroup
                WHERE (srcfeature_id = s OR srcfeature_id IS NULL)
                  AND fmin >= mins[i]
                  AND fmax <= maxs[i]
                  AND group_id IN (
                      SELECT group_id FROM groupoverlaps( mins[i], maxs[i] )
                      WHERE  srcfeature_id = s
                  )
            )
        LOOP
            RETURN NEXT f;
        END LOOP;
    END LOOP;
    RETURN;
END;
' LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION groupidentical(_int8, _int8, _varchar) RETURNS setof featuregroup AS '
DECLARE
    mins alias for $1;
    maxs alias for $2;
    srcs alias for $3;
    f featuregroup%ROWTYPE;
    i int;
    s int;
BEGIN
    i := 1;
    FOR i in array_lower( mins, 1 ) .. array_upper( mins, 1 ) LOOP
        SELECT INTO s feature_id FROM feature WHERE uniquename = srcs[i];
        FOR f IN
            SELECT *
            FROM  featuregroup WHERE group_id IN (
                SELECT group_id FROM featuregroup
                WHERE (srcfeature_id = s OR srcfeature_id IS NULL)
                  AND fmin = mins[i]
                  AND fmax = maxs[i]
                  AND group_id IN (
                      SELECT group_id FROM groupoverlaps( mins[i], maxs[i] )
                      WHERE  srcfeature_id = s
                  )
            )
        LOOP
            RETURN NEXT f;
        END LOOP;
    END LOOP;
    RETURN;
END;
' LANGUAGE 'plpgsql';

--functional index that depends on the above functions
CREATE INDEX bingroup_boxrange ON featuregroup USING gist (boxrange(fmin, fmax)) WHERE is_root = 1;

CREATE OR REPLACE FUNCTION _fill_featuregroup(bigint, bigint) RETURNS INTEGER AS '
DECLARE
    groupid alias for $1;
    parentid alias for $2;
    g featuregroup%ROWTYPE;
BEGIN
    FOR g IN
        SELECT DISTINCT 0, fr.subject_id, fr.object_id, groupid, fl.srcfeature_id, fl.fmin, fl.fmax, fl.strand, 0
        FROM  feature_relationship AS fr,
              featureloc AS fl
        WHERE fr.object_id = parentid
          AND fr.subject_id = fl.feature_id
    LOOP
        INSERT INTO featuregroup
            (subject_id, object_id, group_id, srcfeature_id, fmin, fmax, strand, is_root)
        VALUES
            (g.subject_id, g.object_id, g.group_id, g.srcfeature_id, g.fmin, g.fmax, g.strand, 0);
        PERFORM _fill_featuregroup(groupid,g.subject_id);
    END LOOP;
    RETURN 1;
END;
' LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION fill_featuregroup() RETURNS INTEGER AS '
DECLARE
    p featuregroup%ROWTYPE;
    l featureloc%ROWTYPE;
    isa bigint;
    -- c int;  the c variable isnt used
BEGIN
    TRUNCATE featuregroup;
    SELECT INTO isa cvterm_id FROM cvterm WHERE (name = ''isa'' OR name = ''is_a'');

    -- Recursion is the biggest performance killer for this function.
    -- We can dodge the first round of recursion using the "fr1 / GROUP BY" approach.
    -- Luckily, most feature graphs are only 2 levels deep, so most recursion is
    -- avoidable.

    RAISE NOTICE ''Loading root and singleton features.'';
    FOR p IN
        SELECT DISTINCT 0, f.feature_id, f.feature_id, f.feature_id, srcfeature_id, fmin, fmax, strand, 1
        FROM feature AS f
        LEFT JOIN feature_relationship ON (f.feature_id = object_id)
        LEFT JOIN featureloc           ON (f.feature_id = featureloc.feature_id)
        WHERE f.feature_id NOT IN ( SELECT subject_id FROM feature_relationship )
          AND srcfeature_id IS NOT NULL
    LOOP
        INSERT INTO featuregroup
            (subject_id, object_id, group_id, srcfeature_id, fmin, fmax, strand, is_root)
        VALUES
            (p.object_id, p.object_id, p.object_id, p.srcfeature_id, p.fmin, p.fmax, p.strand, 1);
    END LOOP;

    RAISE NOTICE ''Loading child features.  If your database contains grandchild'';
    RAISE NOTICE ''features, they will be loaded recursively and may take a long time.'';

    FOR p IN
        SELECT DISTINCT 0, fr0.subject_id, fr0.object_id, fr0.object_id, fl.srcfeature_id, fl.fmin, fl.fmax, fl.strand, count(fr1.subject_id)
        FROM  feature_relationship AS fr0
        LEFT JOIN feature_relationship AS fr1 ON ( fr0.subject_id = fr1.object_id),
        featureloc AS fl
        WHERE fr0.subject_id = fl.feature_id
          AND fr0.object_id IN (
                  SELECT f.feature_id
                  FROM feature AS f
                  LEFT JOIN feature_relationship ON (f.feature_id = object_id)
                  LEFT JOIN featureloc           ON (f.feature_id = featureloc.feature_id)
                  WHERE f.feature_id NOT IN ( SELECT subject_id FROM feature_relationship )
                    AND f.feature_id     IN ( SELECT object_id  FROM feature_relationship )
                    AND srcfeature_id IS NOT NULL
              )
        GROUP BY fr0.subject_id, fr0.object_id, fl.srcfeature_id, fl.fmin, fl.fmax, fl.strand
    LOOP
        INSERT INTO featuregroup
            (subject_id, object_id, group_id, srcfeature_id, fmin, fmax, strand, is_root)
        VALUES
            (p.subject_id, p.object_id, p.object_id, p.srcfeature_id, p.fmin, p.fmax, p.strand, 0);
        IF ( p.is_root > 0 ) THEN
            PERFORM _fill_featuregroup(p.subject_id,p.subject_id);
        END IF;
    END LOOP;

    RETURN 1;
END;
' LANGUAGE 'plpgsql';

-- This currently only defines SQL for materialized views used
-- by the OBO importer, PR #2280.
INSERT INTO [tripal_mviews] VALUES (5, 9, 'cv_root_mview', '
  SELECT DISTINCT CVT.name, CVT.cvterm_id, CV.cv_id, CV.name
  FROM cvterm CVT
    LEFT JOIN cvterm_relationship CVTR ON CVT.cvterm_id = CVTR.subject_id
    INNER JOIN cvterm_relationship CVTR2 ON CVT.cvterm_id = CVTR2.object_id
  INNER JOIN cv CV on CV.cv_id = CVT.cv_id
  WHERE CVTR.subject_id is NULL and
    CVT.is_relationshiptype = 0 and CVT.is_obsolete = 0
', 1667003601, 'Populated with 9 rows', 'A list of the root terms for all controlled vocabularies. This is needed for viewing CV trees')
;
INSERT INTO [tripal_mviews] VALUES (6, 10, 'db2cv_mview', '
  SELECT DISTINCT CV.cv_id, CV.name as cvname, DB.db_id, DB.name as dbname,
    COUNT(CVT.cvterm_id) as num_terms
  FROM cv CV
    INNER JOIN cvterm CVT on CVT.cv_id = CV.cv_id
    INNER JOIN dbxref DBX on DBX.dbxref_id = CVT.dbxref_id
    INNER JOIN db DB on DB.db_id = DBX.db_id
  WHERE CVT.is_relationshiptype = 0 and CVT.is_obsolete = 0
  GROUP BY CV.cv_id, CV.name, DB.db_id, DB.name
  ORDER BY DB.name
', 1667003601, 'Populated with 41 rows', 'A table for quick lookup of the vocabularies and the databases they are associated with.')

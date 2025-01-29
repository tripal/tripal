/* https://github.com/GMOD/Chado/issues/140 */
/* Upgrade unique constraints with nullable columns if PostgreSQL version > 15 */
CREATE OR REPLACE PROCEDURE addUniqueLinkerConstraint(table_name varchar, constraint_name varchar, columns varchar[])
  LANGUAGE plpgsql
  AS $$
  DECLARE
    newer_than_15 boolean;
  BEGIN
    -- Determine the version of PostgreSQL
    SELECT CASE WHEN current_setting('server_version_num')::INT > 150000 THEN true ELSE false END AS supported INTO newer_than_15;

    -- IF the version is newer then we can use the new UNIQUE NULLS NOT DISTINCT
    -- which does not treat 2 records that are the same but include NULL as distict.
    IF newer_than_15 THEN
      EXECUTE format('ALTER TABLE %s ADD CONSTRAINT %s UNIQUE NULLS NOT DISTINCT (%s)', table_name, constraint_name, array_to_string(columns, ','));
    -- IF the version is <15 then we use the original UNIQUE style constraint
    ELSE
      EXECUTE format('ALTER TABLE %s ADD CONSTRAINT %s UNIQUE (%s)', table_name, constraint_name, array_to_string(columns, ','));
    END IF;
  END
$$;
/* Contact Linkers */
/* -- Feature */
ALTER TABLE feature_contact ADD COLUMN type_id bigint;
COMMENT ON COLUMN feature_contact.type_id IS 'Indicates the type of linkage such as the role of the contact. For example, a type_id referencing the term Curator (NCIT:C69141) indicates that the linked contact curated a particular gene model.';
ALTER TABLE feature_contact ADD COLUMN rank int DEFAULT 0;
COMMENT ON COLUMN feature_contact.rank IS 'Indicates the ordering of contacts with the same type_id. Currently this is not part of the unique key; therefore, there should only be one rank per unique feature - contact combination.';
ALTER TABLE feature_contact ADD FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL;
ALTER TABLE feature_contact DROP CONSTRAINT feature_contact_c1;
CALL addUniqueLinkerConstraint('feature_contact', 'feature_contact_c1', ARRAY['feature_id', 'contact_id', 'type_id']);
/* -- Featuremap */
ALTER TABLE featuremap_contact ADD COLUMN type_id bigint;
COMMENT ON COLUMN featuremap_contact.type_id IS 'Indicates the type of linkage such as the role of the contact. For example, a type_id referencing the term Curator (NCIT:C69141) indicates that the linked contact curated this genetic map.';
ALTER TABLE featuremap_contact ADD COLUMN rank int DEFAULT 0;
COMMENT ON COLUMN featuremap_contact.rank IS 'Indicates the ordering of contacts with the same type_id. Currently this is not part of the unique key; therefore, there should only be one rank per unique featuremap - contact combination.';
ALTER TABLE featuremap_contact ADD FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL;
ALTER TABLE featuremap_contact DROP CONSTRAINT featuremap_contact_c1;
CALL addUniqueLinkerConstraint('featuremap_contact', 'featuremap_contact_c1', ARRAY['featuremap_id', 'contact_id', 'type_id']);
/* -- Library */
ALTER TABLE library_contact ADD COLUMN type_id bigint;
COMMENT ON COLUMN library_contact.type_id IS 'Indicates the type of linkage such as the role of the contact. For example, a type_id referencing the term Distributor (NCIT:C48289) indicates that the linked contact organization distributes this library.';
ALTER TABLE library_contact ADD COLUMN rank int DEFAULT 0;
COMMENT ON COLUMN library_contact.rank IS 'Indicates the ordering of contacts with the same type_id. Currently this is not part of the unique key; therefore, there should only be one rank per unique library - contact combination.';
ALTER TABLE library_contact ADD FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL;
ALTER TABLE library_contact DROP CONSTRAINT library_contact_c1;
CALL addUniqueLinkerConstraint('library_contact', 'library_contact_c1', ARRAY['library_id', 'contact_id', 'type_id']);
/* -- ND Experiment */
ALTER TABLE nd_experiment_contact ADD COLUMN type_id bigint;
COMMENT ON COLUMN nd_experiment_contact.type_id IS 'Indicates the type of linkage such as the role of the contact. For example, a type_id referencing the term Data Collector (AGRO:00000379) indicates that the data in this natural diversity experiment was collected by the linked contact.';
ALTER TABLE nd_experiment_contact ADD COLUMN rank int DEFAULT 0;
COMMENT ON COLUMN nd_experiment_contact.rank IS 'Indicates the ordering of contacts with the same type_id. Currently this is not part of the unique key; therefore, there should only be one rank per unique ND Experiment - contact combination.';
ALTER TABLE nd_experiment_contact ADD FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL;
CALL addUniqueLinkerConstraint('nd_experiment_contact', 'nd_experiment_contact_c1', ARRAY['nd_experiment_id', 'contact_id', 'type_id']);
/* -- Project */
ALTER TABLE project_contact ADD COLUMN type_id bigint;
COMMENT ON COLUMN project_contact.type_id IS 'Indicates the type of linkage such as the role of the contact. For example, a type_id referencing the term Funder (EFO:0001736) indicates that the linked contact organization funded the research described in this project.';
ALTER TABLE project_contact ADD COLUMN rank int DEFAULT 0;
COMMENT ON COLUMN project_contact.rank IS 'Indicates the ordering of contacts with the same type_id. Currently this is not part of the unique key; therefore, there should only be one rank per unique project - contact combination.';
ALTER TABLE project_contact ADD FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL;
ALTER TABLE project_contact DROP CONSTRAINT project_contact_c1;
CALL addUniqueLinkerConstraint('project_contact', 'project_contact_c1', ARRAY['project_id', 'contact_id', 'type_id']);
/* -- Pubauthor */
ALTER TABLE pubauthor_contact ADD COLUMN type_id bigint;
COMMENT ON COLUMN pubauthor_contact.type_id IS 'Indicates the type of linkage such as the role of the contact. For example, a type_id referencing the term Exact (NCIT:C86021) indicates that the linked contact represents the same person as the author of the publication.';
ALTER TABLE pubauthor_contact ADD COLUMN rank int DEFAULT 0;
COMMENT ON COLUMN pubauthor_contact.rank IS 'Indicates the ordering of contacts with the same type_id. Currently this is not part of the unique key; therefore, there should only be one rank per unique publication author - contact combination.';
ALTER TABLE pubauthor_contact ADD FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL;
ALTER TABLE pubauthor_contact DROP CONSTRAINT pubauthor_contact_c1;
CALL addUniqueLinkerConstraint('pubauthor_contact', 'pubauthor_contact_c1', ARRAY['pubauthor_id', 'contact_id', 'type_id']);
/* Project Linkers */
/* -- Features */
ALTER TABLE project_feature ADD COLUMN type_id bigint;
COMMENT ON COLUMN project_feature.type_id IS 'Indicates the type of linkage such as the way the project uses this item. For example, a type_id referencing the term Reference Object (NCIT:C48294) indicates that the linked project references this feature in the course of their research.';
ALTER TABLE project_feature ADD COLUMN rank int DEFAULT 0;
COMMENT ON COLUMN project_feature.rank IS 'Indicates the ordering of contacts with the same type_id. Currently this is not part of the unique key; therefore, there should only be one rank per unique feature - project combination.';
ALTER TABLE project_feature ADD FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL;
ALTER TABLE project_feature DROP CONSTRAINT project_feature_c1;
CALL addUniqueLinkerConstraint('project_feature', 'project_feature_c1', ARRAY['feature_id', 'project_id', 'type_id']);
/* -- Publications */
ALTER TABLE project_pub ADD COLUMN type_id bigint;
COMMENT ON COLUMN project_pub.type_id IS 'Indicates the type of linkage such as the way the project uses this item. For example, a type_id referencing the term Reference Object (NCIT:C48294) indicates that the linked project references this publication in the course of their research.';
ALTER TABLE project_pub ADD COLUMN rank int DEFAULT 0;
COMMENT ON COLUMN project_pub.rank IS 'Indicates the ordering of contacts with the same type_id. Currently this is not part of the unique key; therefore, there should only be one rank per unique publication - project combination.';
ALTER TABLE project_pub ADD FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL;
ALTER TABLE project_pub DROP CONSTRAINT project_pub_c1;
CALL addUniqueLinkerConstraint('project_pub', 'project_pub_c1', ARRAY['pub_id', 'project_id', 'type_id']);
/* -- ND Experiments */
ALTER TABLE nd_experiment_project ADD COLUMN type_id bigint;
COMMENT ON COLUMN nd_experiment_project.type_id IS 'Indicates the type of linkage such as the way the project uses this item. For example, a type_id referencing the term Output (REPRODUCEME:Output) indicates that the linked project carried out this experiment in the course of their research.';
ALTER TABLE nd_experiment_project ADD COLUMN rank int DEFAULT 0;
COMMENT ON COLUMN nd_experiment_project.rank IS 'Indicates the ordering of contacts with the same type_id. Currently this is not part of the unique key; therefore, there should only be one rank per unique nd experiment - project combination.';
ALTER TABLE nd_experiment_project ADD FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL;
ALTER TABLE nd_experiment_project DROP CONSTRAINT nd_experiment_project_c1;
CALL addUniqueLinkerConstraint('nd_experiment_project', 'nd_experiment_project_c1', ARRAY['nd_experiment_id', 'project_id', 'type_id']);
/* -- Analysis */
/*    Already has a rank */
COMMENT ON COLUMN project_analysis.rank IS 'Indicates the ordering of contacts with the same type_id. Currently this is not part of the unique key; therefore, there should only be one rank per unique analysis - project combination.';
ALTER TABLE project_analysis ADD COLUMN type_id bigint;
COMMENT ON COLUMN project_analysis.type_id IS 'Indicates the type of linkage such as the way the project uses this item. For example, a type_id referencing the term Output (REPRODUCEME:Output) indicates that the linked project carried out this analysis in the course of their research.';
ALTER TABLE project_analysis ADD FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL;
ALTER TABLE project_analysis DROP CONSTRAINT project_analysis_c1;
CALL addUniqueLinkerConstraint('project_analysis', 'project_analysis_c1', ARRAY['analysis_id', 'project_id', 'type_id']);
/* -- Stock */
ALTER TABLE project_stock ADD COLUMN type_id bigint;
COMMENT ON COLUMN project_stock.type_id IS 'Indicates the type of linkage such as the way the project uses this item. For example, a type_id referencing the term Output (REPRODUCEME:Output) indicates that the linked project produced this genetic stock (e.g. bred a new cultivar, extracted a DNA sample) in the course of their research.';
ALTER TABLE project_stock ADD COLUMN rank int DEFAULT 0;
COMMENT ON COLUMN project_stock.rank IS 'Indicates the ordering of contacts with the same type_id. Currently this is not part of the unique key; therefore, there should only be one rank per unique stock - project combination.';
ALTER TABLE project_stock ADD FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL;
ALTER TABLE project_stock DROP CONSTRAINT project_stock_c1;
CALL addUniqueLinkerConstraint('project_stock', 'project_stock_c1', ARRAY['stock_id', 'project_id', 'type_id']);
/* -- Assay */
ALTER TABLE assay_project ADD COLUMN type_id bigint;
COMMENT ON COLUMN assay_project.type_id IS 'Indicates the type of linkage such as the way the project uses this item. For example, a type_id referencing the term Output (REPRODUCEME:Output) indicates that the linked project carried out this assay in the course of their research.';
ALTER TABLE assay_project ADD COLUMN rank int DEFAULT 0;
COMMENT ON COLUMN assay_project.rank IS 'Indicates the ordering of contacts with the same type_id. Currently this is not part of the unique key; therefore, there should only be one rank per unique assay - project combination.';
ALTER TABLE assay_project ADD FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL;
ALTER TABLE assay_project DROP CONSTRAINT assay_project_c1;
CALL addUniqueLinkerConstraint('assay_project', 'assay_project_c1', ARRAY['assay_id', 'project_id', 'type_id']);
/* -- Dbxref */
ALTER TABLE project_dbxref ADD COLUMN type_id bigint;
COMMENT ON COLUMN project_dbxref.type_id IS 'Indicates the type of linkage such as the way the project uses this item. For example, a type_id referencing the term doi (REPRODUCEME:doi) indicates that the linked dbxref is a persistent identifier for this project.';
ALTER TABLE project_dbxref ADD COLUMN rank int DEFAULT 0;
COMMENT ON COLUMN project_dbxref.rank IS 'Indicates the ordering of contacts with the same type_id. Currently this is not part of the unique key; therefore, there should only be one rank per unique dbxref - project combination.';
ALTER TABLE project_dbxref ADD FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL;
ALTER TABLE project_dbxref DROP CONSTRAINT project_dbxref_c1;
CALL addUniqueLinkerConstraint('project_dbxref', 'project_dbxref_c1', ARRAY['dbxref_id', 'project_id', 'type_id']);
/* -- biomaterial_project */
ALTER TABLE biomaterial_project ADD COLUMN type_id bigint;
COMMENT ON COLUMN biomaterial_project.type_id IS 'Indicates the type of linkage such as the way the project uses this item. For example, a type_id referencing the term Output (REPRODUCEME:Output) indicates that the linked project collected this genetic stock (e.g. collected a field sample, extracted a DNA sample) in the course of their research.';
ALTER TABLE biomaterial_project ADD COLUMN rank int DEFAULT 0;
COMMENT ON COLUMN biomaterial_project.rank IS 'Indicates the ordering of contacts with the same type_id. Currently this is not part of the unique key; therefore, there should only be one rank per unique biomaterial - project combination.';
ALTER TABLE biomaterial_project ADD FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE SET NULL;
ALTER TABLE biomaterial_project DROP CONSTRAINT biomaterial_project_c1;
CALL addUniqueLinkerConstraint('biomaterial_project', 'biomaterial_project_c1', ARRAY['biomaterial_id', 'project_id', 'type_id']);

SET search_path = chado,pg_catalog;
-- TABLE: nd_experiment_protocol
-- ================================================

CREATE TABLE nd_experiment_protocol (
    nd_experiment_protocol_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_id bigint NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    nd_protocol_id bigint NOT NULL references nd_protocol (nd_protocol_id) on delete cascade INITIALLY DEFERRED
);
CREATE INDEX nd_experiment_protocol_idx1 ON nd_experiment_protocol (nd_experiment_id);
CREATE INDEX nd_experiment_protocol_idx2 ON nd_experiment_protocol (nd_protocol_id);

COMMENT ON TABLE nd_experiment_protocol IS 'Linking table: experiments to the protocols they involve.';

-- ================================================
-- TABLE: nd_experiment_phenotype
-- ================================================

CREATE TABLE nd_experiment_phenotype (
    nd_experiment_phenotype_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_id bigint NOT NULL REFERENCES nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    phenotype_id bigint NOT NULL references phenotype (phenotype_id) on delete cascade INITIALLY DEFERRED,
   constraint nd_experiment_phenotype_c1 unique (nd_experiment_id,phenotype_id)
);
CREATE INDEX nd_experiment_phenotype_idx1 ON nd_experiment_phenotype (nd_experiment_id);
CREATE INDEX nd_experiment_phenotype_idx2 ON nd_experiment_phenotype (phenotype_id);

COMMENT ON TABLE nd_experiment_phenotype IS 'Linking table: experiments to the phenotypes they produce. There is a one-to-one relationship between an experiment and a phenotype since each phenotype record should point to one experiment. Add a new experiment_id for each phenotype record.';

-- ================================================
-- TABLE: nd_experiment_genotype
-- ================================================

CREATE TABLE nd_experiment_genotype (
    nd_experiment_genotype_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_id bigint NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    genotype_id bigint NOT NULL references genotype (genotype_id) on delete cascade INITIALLY DEFERRED ,
    constraint nd_experiment_genotype_c1 unique (nd_experiment_id,genotype_id)
);

CREATE INDEX nd_experiment_genotype_idx1 ON nd_experiment_genotype (nd_experiment_id);
CREATE INDEX nd_experiment_genotype_idx2 ON nd_experiment_genotype (genotype_id);

COMMENT ON TABLE nd_experiment_genotype IS 'Linking table: experiments to the genotypes they produce. There is a one-to-one relationship between an experiment and a genotype since each genotype record should point to one experiment. Add a new experiment_id for each genotype record.';

-- ================================================
-- TABLE: nd_reagent_relationship
-- ================================================

CREATE TABLE nd_reagent_relationship (
    nd_reagent_relationship_id bigserial PRIMARY KEY NOT NULL,
    subject_reagent_id bigint NOT NULL references nd_reagent (nd_reagent_id) on delete cascade INITIALLY DEFERRED,
    object_reagent_id bigint NOT NULL  references nd_reagent (nd_reagent_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint NOT NULL  references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED
);

CREATE INDEX nd_reagent_relationship_idx1 ON nd_reagent_relationship (subject_reagent_id);
CREATE INDEX nd_reagent_relationship_idx2 ON nd_reagent_relationship (object_reagent_id);
CREATE INDEX nd_reagent_relationship_idx3 ON nd_reagent_relationship (type_id);

COMMENT ON TABLE nd_reagent_relationship IS 'Relationships between reagents. Some reagents form a group. i.e., they are used all together or not at all. Examples are adapter/linker/enzyme experiment reagents.';

COMMENT ON COLUMN nd_reagent_relationship.subject_reagent_id IS 'The subject reagent in the relationship. In parent/child terminology, the subject is the child. For example, in "linkerA 3prime-overhang-linker enzymeA" linkerA is the subject, 3prime-overhand-linker is the type, and enzymeA is the object.';

COMMENT ON COLUMN nd_reagent_relationship.object_reagent_id IS 'The object reagent in the relationship. In parent/child terminology, the object is the parent. For example, in "linkerA 3prime-overhang-linker enzymeA" linkerA is the subject, 3prime-overhand-linker is the type, and enzymeA is the object.';

COMMENT ON COLUMN nd_reagent_relationship.type_id IS 'The type (or predicate) of the relationship. For example, in "linkerA 3prime-overhang-linker enzymeA" linkerA is the subject, 3prime-overhand-linker is the type, and enzymeA is the object.';

-- ================================================
-- TABLE: nd_reagentprop
-- ================================================

CREATE TABLE nd_reagentprop (
    nd_reagentprop_id bigserial PRIMARY KEY NOT NULL,
    nd_reagent_id bigint NOT NULL references nd_reagent (nd_reagent_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int DEFAULT 0 NOT NULL,
    constraint nd_reagentprop_c1 unique (nd_reagent_id,type_id,rank)
);

CREATE INDEX nd_reagentprop_idx1 ON nd_reagentprop (nd_reagent_id);
CREATE INDEX nd_reagentprop_idx2 ON nd_reagentprop (type_id);

-- ================================================
-- TABLE: nd_experiment_stockprop
-- ================================================

CREATE TABLE nd_experiment_stockprop (
    nd_experiment_stockprop_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_stock_id bigint NOT NULL references nd_experiment_stock (nd_experiment_stock_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int DEFAULT 0 NOT NULL,
    constraint nd_experiment_stockprop_c1 unique (nd_experiment_stock_id,type_id,rank)
);

CREATE INDEX nd_experiment_stockprop_idx1 ON nd_experiment_stockprop (nd_experiment_stock_id);
CREATE INDEX nd_experiment_stockprop_idx2 ON nd_experiment_stockprop (type_id);

COMMENT ON TABLE nd_experiment_stockprop IS 'Property/value associations for experiment_stocks. This table can store the properties such as treatment';

COMMENT ON COLUMN nd_experiment_stockprop.nd_experiment_stock_id IS 'The experiment_stock to which the property applies.';

COMMENT ON COLUMN nd_experiment_stockprop.type_id IS 'The name of the property as a reference to a controlled vocabulary term.';

COMMENT ON COLUMN nd_experiment_stockprop.value IS 'The value of the property.';

COMMENT ON COLUMN nd_experiment_stockprop.rank IS 'The rank of the property value, if the property has an array of values.';

-- ================================================
-- TABLE: nd_experiment_stock_dbxref
-- ================================================

CREATE TABLE nd_experiment_stock_dbxref (
    nd_experiment_stock_dbxref_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_stock_id bigint NOT NULL references nd_experiment_stock (nd_experiment_stock_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id bigint NOT NULL references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED
);
CREATE INDEX nd_experiment_stock_dbxref_idx1 ON nd_experiment_stock_dbxref (nd_experiment_stock_id);
CREATE INDEX nd_experiment_stock_dbxref_idx2 ON nd_experiment_stock_dbxref (dbxref_id);

COMMENT ON TABLE nd_experiment_stock_dbxref IS 'Cross-reference experiment_stock to accessions, images, etc';

-- ================================================
-- TABLE: nd_experiment_dbxref
-- ===============================================

CREATE TABLE nd_experiment_dbxref (
    nd_experiment_dbxref_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_id bigint NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id bigint NOT NULL references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED
);

CREATE INDEX nd_experiment_dbxref_idx1 ON nd_experiment_dbxref (nd_experiment_id);
CREATE INDEX nd_experiment_dbxref_idx2 ON nd_experiment_dbxref (dbxref_id);

COMMENT ON TABLE nd_experiment_dbxref IS 'Cross-reference experiment to accessions, images, etc';

-- ================================================
-- TABLE: nd_experiment_contact
-- ================================================

CREATE TABLE nd_experiment_contact (
    nd_experiment_contact_id bigserial PRIMARY KEY NOT NULL,
    nd_experiment_id bigint NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    contact_id bigint NOT NULL references contact (contact_id) on delete cascade INITIALLY DEFERRED
);
CREATE INDEX nd_experiment_contact_idx1 ON nd_experiment_contact (nd_experiment_id);
CREATE INDEX nd_experiment_contact_idx2 ON nd_experiment_contact (contact_id);

-- ================================================
-- TABLE: nd_experiment_analysis
-- ================================================

CREATE TABLE nd_experiment_analysis (
  nd_experiment_analysis_id bigserial PRIMARY KEY NOT NULL,
  nd_experiment_id bigint NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
  analysis_id bigint NOT NULL references analysis (analysis_id)  on delete cascade INITIALLY DEFERRED,
  type_id bigint NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED
);
CREATE INDEX nd_experiment_analysis_idx1 ON nd_experiment_analysis (nd_experiment_id);
CREATE INDEX nd_experiment_analysis_idx2 ON nd_experiment_analysis (analysis_id);
CREATE INDEX nd_experiment_analysis_idx3 ON nd_experiment_analysis (type_id);

COMMENT ON TABLE nd_experiment_analysis IS 'An analysis that is used in an experiment';

SET search_path = chado,pg_catalog;
-- TABLE: projectprop
-- ================================================

CREATE TABLE projectprop (
	projectprop_id bigserial NOT NULL,
	PRIMARY KEY (projectprop_id),
	project_id bigint NOT NULL,
	FOREIGN KEY (project_id) REFERENCES project (project_id) ON DELETE CASCADE,
	type_id bigint NOT NULL,
	FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE CASCADE,
	value text,
	rank int not null default 0,
	CONSTRAINT projectprop_c1 UNIQUE (project_id, type_id, rank)
);
COMMENT ON TABLE project IS
'Standard Chado flexible property table for projects.';

-- ================================================
-- TABLE: project_relationship
-- ================================================

CREATE TABLE project_relationship (
	project_relationship_id bigserial NOT NULL,
	PRIMARY KEY (project_relationship_id),
	subject_project_id bigint NOT NULL,
	FOREIGN KEY (subject_project_id) REFERENCES project (project_id) ON DELETE CASCADE,
	object_project_id bigint NOT NULL,
	FOREIGN KEY (object_project_id) REFERENCES project (project_id) ON DELETE CASCADE,
	type_id bigint NOT NULL,
	FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE RESTRICT,
	CONSTRAINT project_relationship_c1 UNIQUE (subject_project_id, object_project_id, type_id)
);

COMMENT ON TABLE project_relationship IS
'Linking table for relating projects to each other.  For example, a
given project could be composed of several smaller subprojects';

COMMENT ON COLUMN project_relationship.type_id IS
'The cvterm type of the relationship being stated, such as "part of".';

-- ================================================
-- TABLE: project_pub
-- ================================================

create table project_pub (
       project_pub_id bigserial not null,
       primary key (project_pub_id),
       project_id bigint not null,
       foreign key (project_id) references project (project_id) on delete cascade INITIALLY DEFERRED,
       pub_id bigint not null,
       foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
       constraint project_pub_c1 unique (project_id,pub_id)
);
create index project_pub_idx1 on project_pub (project_id);
create index project_pub_idx2 on project_pub (pub_id);

COMMENT ON TABLE project_pub IS 'Linking table for associating projects and publications.';

-- ================================================
-- TABLE: project_contact
-- ================================================

create table project_contact (
       project_contact_id bigserial not null,
       primary key (project_contact_id),
       project_id bigint not null,
       foreign key (project_id) references project (project_id) on delete cascade INITIALLY DEFERRED,
       contact_id bigint not null,
       foreign key (contact_id) references contact (contact_id) on delete cascade INITIALLY DEFERRED,
       constraint project_contact_c1 unique (project_id,contact_id)
);
create index project_contact_idx1 on project_contact (project_id);
create index project_contact_idx2 on project_contact (contact_id);

COMMENT ON TABLE project_contact IS 'Linking table for associating projects and contacts.';

-- ================================================
-- TABLE: project_dbxref
-- ================================================

create table project_dbxref (
  project_dbxref_id bigserial not null,
  project_id bigint not null,
  dbxref_id bigint not null,
  is_current boolean not null default 'true',
  primary key (project_dbxref_id),
  foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED,
  foreign key (project_id) references project (project_id) on delete cascade INITIALLY DEFERRED,
  constraint project_dbxref_c1 unique (project_id,dbxref_id)
);
create index project_dbxref_idx1 on project_dbxref (project_id);
create index project_dbxref_idx2 on project_dbxref (dbxref_id);

COMMENT ON TABLE project_dbxref IS 'project_dbxref links a project to dbxrefs.';
COMMENT ON COLUMN project_dbxref.is_current IS 'The is_current boolean indicates whether the linked dbxref is the current -official- dbxref for the linked project.';

-- ================================================
-- TABLE: project_analysis
-- ================================================

create table project_analysis (
       project_analysis_id bigserial not null,
       primary key (project_analysis_id),
       project_id bigint not null,
       foreign key (project_id) references project (project_id) on delete cascade INITIALLY DEFERRED,
       analysis_id bigint not null,
       foreign key (analysis_id) references analysis (analysis_id) on delete cascade INITIALLY DEFERRED,
       rank int not null default 0,
       constraint project_analysis_c1 unique (project_id,analysis_id)
);
create index project_analysis_idx1 on project_analysis (project_id);
create index project_analysis_idx2 on project_analysis (analysis_id);

COMMENT ON TABLE project_analysis IS 'Links an analysis to a project that may contain multiple analyses. 
The rank column can be used to specify a simple ordering in which analyses were executed.';


-- ================================================
-- TABLE: project_feature
-- ================================================

CREATE TABLE project_feature (
    project_feature_id bigserial primary key NOT NULL,
    feature_id bigint NOT NULL,
    project_id bigint NOT NULL,
    CONSTRAINT project_feature_c1 UNIQUE (feature_id, project_id),
    FOREIGN KEY (feature_id) REFERENCES feature(feature_id) ON DELETE CASCADE,
    FOREIGN KEY (project_id) REFERENCES project(project_id) ON DELETE CASCADE
);

CREATE INDEX project_feature_idx1 ON project_feature USING btree (feature_id);
CREATE INDEX project_feature_idx2 ON project_feature USING btree (project_id);

COMMENT ON TABLE project_feature IS 'This table is intended associate records in the feature table with a project.';

-- ================================================
-- TABLE: project_stock
-- ================================================

CREATE TABLE project_stock (
    project_stock_id bigserial primary key NOT NULL,
    stock_id bigint NOT NULL,
    project_id bigint NOT NULL,
    CONSTRAINT project_stock_c1 UNIQUE (stock_id, project_id),
    FOREIGN KEY (stock_id) REFERENCES stock(stock_id) ON DELETE CASCADE,
    FOREIGN KEY (project_id) REFERENCES project(project_id) ON DELETE CASCADE
);

CREATE INDEX project_stock_idx1 ON project_stock USING btree (stock_id);
CREATE INDEX project_stock_idx2 ON project_stock USING btree (project_id);


COMMENT ON TABLE project_stock IS 'This table is intended associate records in the stock table with a project.';
-- $Id: mage.sql,v 1.3 2008-03-19 18:32:51 scottcain Exp $
-- ==========================================
-- Chado mage module
--
-- =================================================================
-- Dependencies:
--
-- :import feature from sequence
-- :import cvterm from cv
-- :import pub from pub
-- :import organism from organism
-- :import contact from contact
-- :import dbxref from db
-- :import tableinfo from general
-- :import project from project
-- :import analysis from companalysis
-- =================================================================

-- ================================================
-- TABLE: mageml
-- ================================================

create table mageml (
    mageml_id bigserial not null,
    primary key (mageml_id),
    mage_package text not null,
    mage_ml text not null
);

COMMENT ON TABLE mageml IS 'This table is for storing extra bits of MAGEml in a denormalized form. More normalization would require many more tables.';

-- ================================================
-- TABLE: magedocumentation
-- ================================================

create table magedocumentation (
    magedocumentation_id bigserial not null,
    primary key (magedocumentation_id),
    mageml_id bigint not null,
    foreign key (mageml_id) references mageml (mageml_id) on delete cascade INITIALLY DEFERRED,
    tableinfo_id bigint not null,
    foreign key (tableinfo_id) references tableinfo (tableinfo_id) on delete cascade INITIALLY DEFERRED,
    row_id int not null,
    mageidentifier text not null
);
create index magedocumentation_idx1 on magedocumentation (mageml_id);
create index magedocumentation_idx2 on magedocumentation (tableinfo_id);
create index magedocumentation_idx3 on magedocumentation (row_id);

COMMENT ON TABLE magedocumentation IS NULL;

-- ================================================
-- TABLE: protocol
-- ================================================

create table protocol (
    protocol_id bigserial not null,
    primary key (protocol_id),
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint null,
    foreign key (pub_id) references pub (pub_id) on delete set null INITIALLY DEFERRED,
    dbxref_id bigint null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
    name text not null,
    uri text null,
    protocoldescription text null,
    hardwaredescription text null,
    softwaredescription text null,
    constraint protocol_c1 unique (name)
);
create index protocol_idx1 on protocol (type_id);
create index protocol_idx2 on protocol (pub_id);
create index protocol_idx3 on protocol (dbxref_id);

COMMENT ON TABLE protocol IS 'Procedural notes on how data was prepared and processed.';

-- ================================================
-- TABLE: protocolparam
-- ================================================

create table protocolparam (
    protocolparam_id bigserial not null,
    primary key (protocolparam_id),
    protocol_id bigint not null,
    foreign key (protocol_id) references protocol (protocol_id) on delete cascade INITIALLY DEFERRED,
    name text not null,
    datatype_id bigint null,
    foreign key (datatype_id) references cvterm (cvterm_id) on delete set null INITIALLY DEFERRED,
    unittype_id bigint null,
    foreign key (unittype_id) references cvterm (cvterm_id) on delete set null INITIALLY DEFERRED,
    value text null,
    rank int not null default 0
);
create index protocolparam_idx1 on protocolparam (protocol_id);
create index protocolparam_idx2 on protocolparam (datatype_id);
create index protocolparam_idx3 on protocolparam (unittype_id);

COMMENT ON TABLE protocolparam IS 'Parameters related to a
protocol. For example, if the protocol is a soak, this might include attributes of bath temperature and duration.';

-- ================================================
-- TABLE: channel
-- ================================================

create table channel (
    channel_id bigserial not null,
    primary key (channel_id),
    name text not null,
    definition text not null,
    constraint channel_c1 unique (name)
);

COMMENT ON TABLE channel IS 'Different array platforms can record signals from one or more channels (cDNA arrays typically use two CCD, but Affymetrix uses only one).';

-- ================================================
-- TABLE: arraydesign
-- ================================================

create table arraydesign (
    arraydesign_id bigserial not null,
    primary key (arraydesign_id),
    manufacturer_id bigint not null,
    foreign key (manufacturer_id) references contact (contact_id) on delete cascade INITIALLY DEFERRED,
    platformtype_id bigint not null,
    foreign key (platformtype_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    substratetype_id bigint null,
    foreign key (substratetype_id) references cvterm (cvterm_id) on delete set null INITIALLY DEFERRED,
    protocol_id bigint null,
    foreign key (protocol_id) references protocol (protocol_id) on delete set null INITIALLY DEFERRED,
    dbxref_id bigint null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
    name text not null,
    version text null,
    description text null,
    array_dimensions text null,
    element_dimensions text null,
    num_of_elements int null,
    num_array_columns int null,
    num_array_rows int null,
    num_grid_columns int null,
    num_grid_rows int null,
    num_sub_columns int null,
    num_sub_rows int null,
    constraint arraydesign_c1 unique (name)
);
create index arraydesign_idx1 on arraydesign (manufacturer_id);
create index arraydesign_idx2 on arraydesign (platformtype_id);
create index arraydesign_idx3 on arraydesign (substratetype_id);
create index arraydesign_idx4 on arraydesign (protocol_id);
create index arraydesign_idx5 on arraydesign (dbxref_id);

COMMENT ON TABLE arraydesign IS 'General properties about an array.
An array is a template used to generate physical slides, etc.  It
contains layout information, as well as global array properties, such
as material (glass, nylon) and spot dimensions (in rows/columns).';

-- ================================================
-- TABLE: arraydesignprop
-- ================================================

create table arraydesignprop (
    arraydesignprop_id bigserial not null,
    primary key (arraydesignprop_id),
    arraydesign_id bigint not null,
    foreign key (arraydesign_id) references arraydesign (arraydesign_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint arraydesignprop_c1 unique (arraydesign_id,type_id,rank)
);
create index arraydesignprop_idx1 on arraydesignprop (arraydesign_id);
create index arraydesignprop_idx2 on arraydesignprop (type_id);

COMMENT ON TABLE arraydesignprop IS 'Extra array design properties that are not accounted for in arraydesign.';

-- ================================================
-- TABLE: assay
-- ================================================

create table assay (
    assay_id bigserial not null,
    primary key (assay_id),
    arraydesign_id bigint not null,
    foreign key (arraydesign_id) references arraydesign (arraydesign_id) on delete cascade INITIALLY DEFERRED,
    protocol_id bigint null,
    foreign key (protocol_id) references protocol (protocol_id) on delete set null INITIALLY DEFERRED,
    assaydate timestamp null default current_timestamp,
    arrayidentifier text null,
    arraybatchidentifier text null,
    operator_id bigint not null,
    foreign key (operator_id) references contact (contact_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id bigint null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
    name text null,
    description text null,
    constraint assay_c1 unique (name)
);
create index assay_idx1 on assay (arraydesign_id);
create index assay_idx2 on assay (protocol_id);
create index assay_idx3 on assay (operator_id);
create index assay_idx4 on assay (dbxref_id);

COMMENT ON TABLE assay IS 'An assay consists of a physical instance of
an array, combined with the conditions used to create the array
(protocols, technician information). The assay can be thought of as a hybridization.';

-- ================================================
-- TABLE: assayprop
-- ================================================

create table assayprop (
    assayprop_id bigserial not null,
    primary key (assayprop_id),
    assay_id bigint not null,
    foreign key (assay_id) references assay (assay_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint assayprop_c1 unique (assay_id,type_id,rank)
);
create index assayprop_idx1 on assayprop (assay_id);
create index assayprop_idx2 on assayprop (type_id);

COMMENT ON TABLE assayprop IS 'Extra assay properties that are not accounted for in assay.';

-- ================================================
-- TABLE: assay_project
-- ================================================

create table assay_project (
    assay_project_id bigserial not null,
    primary key (assay_project_id),
    assay_id bigint not null,
    foreign key (assay_id) references assay (assay_id) INITIALLY DEFERRED,
    project_id bigint not null,
    foreign key (project_id) references project (project_id) INITIALLY DEFERRED,
    constraint assay_project_c1 unique (assay_id,project_id)
);
create index assay_project_idx1 on assay_project (assay_id);
create index assay_project_idx2 on assay_project (project_id);

COMMENT ON TABLE assay_project IS 'Link assays to projects.';

-- ================================================
-- TABLE: biomaterial
-- ================================================

create table biomaterial (
    biomaterial_id bigserial not null,
    primary key (biomaterial_id),
    taxon_id bigint null,
    foreign key (taxon_id) references organism (organism_id) on delete set null INITIALLY DEFERRED,
    biosourceprovider_id bigint null,
    foreign key (biosourceprovider_id) references contact (contact_id) on delete set null INITIALLY DEFERRED,
    dbxref_id bigint null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
    name text null,
    description text null,
    constraint biomaterial_c1 unique (name)
);
create index biomaterial_idx1 on biomaterial (taxon_id);
create index biomaterial_idx2 on biomaterial (biosourceprovider_id);
create index biomaterial_idx3 on biomaterial (dbxref_id);

COMMENT ON TABLE biomaterial IS 'A biomaterial represents the MAGE concept of BioSource, BioSample, and LabeledExtract. It is essentially some biological material (tissue, cells, serum) that may have been processed. Processed biomaterials should be traceable back to raw biomaterials via the biomaterialrelationship table.';

-- ================================================
-- TABLE: biomaterial_relationship
-- ================================================

create table biomaterial_relationship (
    biomaterial_relationship_id bigserial not null,
    primary key (biomaterial_relationship_id),
    subject_id bigint not null,
    foreign key (subject_id) references biomaterial (biomaterial_id) INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references biomaterial (biomaterial_id) INITIALLY DEFERRED,
    constraint biomaterial_relationship_c1 unique (subject_id,object_id,type_id)
);
create index biomaterial_relationship_idx1 on biomaterial_relationship (subject_id);
create index biomaterial_relationship_idx2 on biomaterial_relationship (object_id);
create index biomaterial_relationship_idx3 on biomaterial_relationship (type_id);

COMMENT ON TABLE biomaterial_relationship IS 'Relate biomaterials to one another. This is a way to track a series of treatments or material splits/merges, for instance.';

-- ================================================
-- TABLE: biomaterialprop
-- ================================================

create table biomaterialprop (
    biomaterialprop_id bigserial not null,
    primary key (biomaterialprop_id),
    biomaterial_id bigint not null,
    foreign key (biomaterial_id) references biomaterial (biomaterial_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint biomaterialprop_c1 unique (biomaterial_id,type_id,rank)
);
create index biomaterialprop_idx1 on biomaterialprop (biomaterial_id);
create index biomaterialprop_idx2 on biomaterialprop (type_id);

COMMENT ON TABLE biomaterialprop IS 'Extra biomaterial properties that are not accounted for in biomaterial.';

-- ================================================
-- TABLE: biomaterial_dbxref
-- ================================================

create table biomaterial_dbxref (
    biomaterial_dbxref_id bigserial not null,
    primary key (biomaterial_dbxref_id),
    biomaterial_id bigint not null,
    foreign key (biomaterial_id) references biomaterial (biomaterial_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id bigint not null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED,
    constraint biomaterial_dbxref_c1 unique (biomaterial_id,dbxref_id)
);
create index biomaterial_dbxref_idx1 on biomaterial_dbxref (biomaterial_id);
create index biomaterial_dbxref_idx2 on biomaterial_dbxref (dbxref_id);

-- ================================================
-- TABLE: treatment
-- ================================================

create table treatment (
    treatment_id bigserial not null,
    primary key (treatment_id),
    rank int not null default 0,
    biomaterial_id bigint not null,
    foreign key (biomaterial_id) references biomaterial (biomaterial_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    protocol_id bigint null,
    foreign key (protocol_id) references protocol (protocol_id) on delete set null INITIALLY DEFERRED,
    name text null
);
create index treatment_idx1 on treatment (biomaterial_id);
create index treatment_idx2 on treatment (type_id);
create index treatment_idx3 on treatment (protocol_id);

COMMENT ON TABLE treatment IS 'A biomaterial may undergo multiple
treatments. Examples of treatments: apoxia, fluorophore and biotin labeling.';

-- ================================================
-- TABLE: biomaterial_treatment
-- ================================================

create table biomaterial_treatment (
    biomaterial_treatment_id bigserial not null,
    primary key (biomaterial_treatment_id),
    biomaterial_id bigint not null,
    foreign key (biomaterial_id) references biomaterial (biomaterial_id) on delete cascade INITIALLY DEFERRED,
    treatment_id bigint not null,
    foreign key (treatment_id) references treatment (treatment_id) on delete cascade INITIALLY DEFERRED,
    unittype_id bigint null,
    foreign key (unittype_id) references cvterm (cvterm_id) on delete set null INITIALLY DEFERRED,
    value float(15) null,
    rank int not null default 0,
    constraint biomaterial_treatment_c1 unique (biomaterial_id,treatment_id)
);
create index biomaterial_treatment_idx1 on biomaterial_treatment (biomaterial_id);
create index biomaterial_treatment_idx2 on biomaterial_treatment (treatment_id);
create index biomaterial_treatment_idx3 on biomaterial_treatment (unittype_id);

COMMENT ON TABLE biomaterial_treatment IS 'Link biomaterials to treatments. Treatments have an order of operations (rank), and associated measurements (unittype_id, value).';

-- ================================================
-- TABLE: assay_biomaterial
-- ================================================

create table assay_biomaterial (
    assay_biomaterial_id bigserial not null,
    primary key (assay_biomaterial_id),
    assay_id bigint not null,
    foreign key (assay_id) references assay (assay_id) on delete cascade INITIALLY DEFERRED,
    biomaterial_id bigint not null,
    foreign key (biomaterial_id) references biomaterial (biomaterial_id) on delete cascade INITIALLY DEFERRED,
    channel_id bigint null,
    foreign key (channel_id) references channel (channel_id) on delete set null INITIALLY DEFERRED,
    rank int not null default 0,
    constraint assay_biomaterial_c1 unique (assay_id,biomaterial_id,channel_id,rank)
);
create index assay_biomaterial_idx1 on assay_biomaterial (assay_id);
create index assay_biomaterial_idx2 on assay_biomaterial (biomaterial_id);
create index assay_biomaterial_idx3 on assay_biomaterial (channel_id);

COMMENT ON TABLE assay_biomaterial IS 'A biomaterial can be hybridized many times (technical replicates), or combined with other biomaterials in a single hybridization (for two-channel arrays).';

-- ================================================
-- TABLE: acquisition
-- ================================================

create table acquisition (
    acquisition_id bigserial not null,
    primary key (acquisition_id),
    assay_id bigint not null,
    foreign key (assay_id) references  assay (assay_id) on delete cascade INITIALLY DEFERRED,
    protocol_id bigint null,
    foreign key (protocol_id) references protocol (protocol_id) on delete set null INITIALLY DEFERRED,
    channel_id bigint null,
    foreign key (channel_id) references channel (channel_id) on delete set null INITIALLY DEFERRED,
    acquisitiondate timestamp null default current_timestamp,
    name text null,
    uri text null,
    constraint acquisition_c1 unique (name)
);
create index acquisition_idx1 on acquisition (assay_id);
create index acquisition_idx2 on acquisition (protocol_id);
create index acquisition_idx3 on acquisition (channel_id);

COMMENT ON TABLE acquisition IS 'This represents the scanning of hybridized material. The output of this process is typically a digital image of an array.';

-- ================================================
-- TABLE: acquisitionprop
-- ================================================

create table acquisitionprop (
    acquisitionprop_id bigserial not null,
    primary key (acquisitionprop_id),
    acquisition_id bigint not null,
    foreign key (acquisition_id) references acquisition (acquisition_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint acquisitionprop_c1 unique (acquisition_id,type_id,rank)
);
create index acquisitionprop_idx1 on acquisitionprop (acquisition_id);
create index acquisitionprop_idx2 on acquisitionprop (type_id);

COMMENT ON TABLE acquisitionprop IS 'Parameters associated with image acquisition.';

-- ================================================
-- TABLE: acquisition_relationship
-- ================================================

create table acquisition_relationship (
    acquisition_relationship_id bigserial not null,
    primary key (acquisition_relationship_id),
    subject_id bigint not null,
    foreign key (subject_id) references acquisition (acquisition_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references acquisition (acquisition_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint acquisition_relationship_c1 unique (subject_id,object_id,type_id,rank)
);
create index acquisition_relationship_idx1 on acquisition_relationship (subject_id);
create index acquisition_relationship_idx2 on acquisition_relationship (type_id);
create index acquisition_relationship_idx3 on acquisition_relationship (object_id);

COMMENT ON TABLE acquisition_relationship IS 'Multiple monochrome images may be merged to form a multi-color image. Red-green images of 2-channel hybridizations are an example of this.';

-- ================================================
-- TABLE: quantification
-- ================================================

create table quantification (
    quantification_id bigserial not null,
    primary key (quantification_id),
    acquisition_id bigint not null,
    foreign key (acquisition_id) references acquisition (acquisition_id) on delete cascade INITIALLY DEFERRED,
    operator_id bigint null,
    foreign key (operator_id) references contact (contact_id) on delete set null INITIALLY DEFERRED,
    protocol_id bigint null,
    foreign key (protocol_id) references protocol (protocol_id) on delete set null INITIALLY DEFERRED,
    analysis_id bigint not null,
    foreign key (analysis_id) references analysis (analysis_id) on delete cascade INITIALLY DEFERRED,
    quantificationdate timestamp null default current_timestamp,
    name text null,
    uri text null,
    constraint quantification_c1 unique (name,analysis_id)
);
create index quantification_idx1 on quantification (acquisition_id);
create index quantification_idx2 on quantification (operator_id);
create index quantification_idx3 on quantification (protocol_id);
create index quantification_idx4 on quantification (analysis_id);

COMMENT ON TABLE quantification IS 'Quantification is the transformation of an image acquisition to numeric data. This typically involves statistical procedures.';

-- ================================================
-- TABLE: quantificationprop
-- ================================================

create table quantificationprop (
    quantificationprop_id bigserial not null,
    primary key (quantificationprop_id),
    quantification_id bigint not null,
    foreign key (quantification_id) references quantification (quantification_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint quantificationprop_c1 unique (quantification_id,type_id,rank)
);
create index quantificationprop_idx1 on quantificationprop (quantification_id);
create index quantificationprop_idx2 on quantificationprop (type_id);

COMMENT ON TABLE quantificationprop IS 'Extra quantification properties that are not accounted for in quantification.';

-- ================================================
-- TABLE: quantification_relationship
-- ================================================

create table quantification_relationship (
    quantification_relationship_id bigserial not null,
    primary key (quantification_relationship_id),
    subject_id bigint not null,
    foreign key (subject_id) references quantification (quantification_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references quantification (quantification_id) on delete cascade INITIALLY DEFERRED,
    constraint quantification_relationship_c1 unique (subject_id,object_id,type_id)
);
create index quantification_relationship_idx1 on quantification_relationship (subject_id);
create index quantification_relationship_idx2 on quantification_relationship (type_id);
create index quantification_relationship_idx3 on quantification_relationship (object_id);

COMMENT ON TABLE quantification_relationship IS 'There may be multiple rounds of quantification, this allows us to keep an audit trail of what values went where.';

-- ================================================
-- TABLE: control
-- ================================================

create table control (
    control_id bigserial not null,
    primary key (control_id),
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    assay_id bigint not null,
    foreign key (assay_id) references assay (assay_id) on delete cascade INITIALLY DEFERRED,
    tableinfo_id bigint not null,
    foreign key (tableinfo_id) references tableinfo (tableinfo_id) on delete cascade INITIALLY DEFERRED,
    row_id int not null,
    name text null,
    value text null,
    rank int not null default 0
);
create index control_idx1 on control (type_id);
create index control_idx2 on control (assay_id);
create index control_idx3 on control (tableinfo_id);
create index control_idx4 on control (row_id);

COMMENT ON TABLE control IS NULL;

-- ================================================
-- TABLE: element
-- ================================================

create table element (
    element_id bigserial not null,
    primary key (element_id),
    feature_id bigint null,
    foreign key (feature_id) references feature (feature_id) on delete set null INITIALLY DEFERRED,
    arraydesign_id bigint not null,
    foreign key (arraydesign_id) references arraydesign (arraydesign_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint null,
    foreign key (type_id) references cvterm (cvterm_id) on delete set null INITIALLY DEFERRED,
    dbxref_id bigint null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
    constraint element_c1 unique (feature_id,arraydesign_id)
);
create index element_idx1 on element (feature_id);
create index element_idx2 on element (arraydesign_id);
create index element_idx3 on element (type_id);
create index element_idx4 on element (dbxref_id);

COMMENT ON TABLE element IS 'Represents a feature of the array. This is typically a region of the array coated or bound to DNA.';

-- ================================================
-- TABLE: element_result
-- ================================================

create table elementresult (
    elementresult_id bigserial not null,
    primary key (elementresult_id),
    element_id bigint not null,
    foreign key (element_id) references element (element_id) on delete cascade INITIALLY DEFERRED,
    quantification_id bigint not null,
    foreign key (quantification_id) references quantification (quantification_id) on delete cascade INITIALLY DEFERRED,
    signal float not null,
    constraint elementresult_c1 unique (element_id,quantification_id)
);
create index elementresult_idx1 on elementresult (element_id);
create index elementresult_idx2 on elementresult (quantification_id);
create index elementresult_idx3 on elementresult (signal);

COMMENT ON TABLE elementresult IS 'An element on an array produces a measurement when hybridized to a biomaterial (traceable through quantification_id). This is the base data from which tables that actually contain data inherit.';

-- ================================================
-- TABLE: element_relationship
-- ================================================

create table element_relationship (
    element_relationship_id bigserial not null,
    primary key (element_relationship_id),
    subject_id bigint not null,
    foreign key (subject_id) references element (element_id) INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references element (element_id) INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint element_relationship_c1 unique (subject_id,object_id,type_id,rank)
);
create index element_relationship_idx1 on element_relationship (subject_id);
create index element_relationship_idx2 on element_relationship (type_id);
create index element_relationship_idx3 on element_relationship (object_id);
create index element_relationship_idx4 on element_relationship (value);

COMMENT ON TABLE element_relationship IS 'Sometimes we want to combine measurements from multiple elements to get a composite value. Affymetrix combines many probes to form a probeset measurement, for instance.';

-- ================================================
-- TABLE: elementresult_relationship
-- ================================================

create table elementresult_relationship (
    elementresult_relationship_id bigserial not null,
    primary key (elementresult_relationship_id),
    subject_id bigint not null,
    foreign key (subject_id) references elementresult (elementresult_id) INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) INITIALLY DEFERRED,
    object_id bigint not null,
    foreign key (object_id) references elementresult (elementresult_id) INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint elementresult_relationship_c1 unique (subject_id,object_id,type_id,rank)
);
create index elementresult_relationship_idx1 on elementresult_relationship (subject_id);
create index elementresult_relationship_idx2 on elementresult_relationship (type_id);
create index elementresult_relationship_idx3 on elementresult_relationship (object_id);
create index elementresult_relationship_idx4 on elementresult_relationship (value);

COMMENT ON TABLE elementresult_relationship IS 'Sometimes we want to combine measurements from multiple elements to get a composite value. Affymetrix combines many probes to form a probeset measurement, for instance.';

-- ================================================
-- TABLE: study
-- ================================================

create table study (
    study_id bigserial not null,
    primary key (study_id),
    contact_id bigint not null,
    foreign key (contact_id) references contact (contact_id) on delete cascade INITIALLY DEFERRED,
    pub_id bigint null,
    foreign key (pub_id) references pub (pub_id) on delete set null INITIALLY DEFERRED,
    dbxref_id bigint null,
    foreign key (dbxref_id) references dbxref (dbxref_id) on delete set null INITIALLY DEFERRED,
    name text not null,
    description text null,
    constraint study_c1 unique (name)
);
create index study_idx1 on study (contact_id);
create index study_idx2 on study (pub_id);
create index study_idx3 on study (dbxref_id);

COMMENT ON TABLE study IS NULL;

-- ================================================
-- TABLE: study_assay
-- ================================================

create table study_assay (
    study_assay_id bigserial not null,
    primary key (study_assay_id),
    study_id bigint not null,
    foreign key (study_id) references study (study_id) on delete cascade INITIALLY DEFERRED,
    assay_id bigint not null,
    foreign key (assay_id) references assay (assay_id) on delete cascade INITIALLY DEFERRED,
    constraint study_assay_c1 unique (study_id,assay_id)
);
create index study_assay_idx1 on study_assay (study_id);
create index study_assay_idx2 on study_assay (assay_id);

COMMENT ON TABLE study_assay IS NULL;

-- ================================================
-- TABLE: studydesign
-- ================================================

create table studydesign (
    studydesign_id bigserial not null,
    primary key (studydesign_id),
    study_id bigint not null,
    foreign key (study_id) references study (study_id) on delete cascade INITIALLY DEFERRED,
    description text null
);
create index studydesign_idx1 on studydesign (study_id);

COMMENT ON TABLE studydesign IS NULL;

-- ================================================
-- TABLE: studydesignprop
-- ================================================

create table studydesignprop (
    studydesignprop_id bigserial not null,
    primary key (studydesignprop_id),
    studydesign_id bigint not null,
    foreign key (studydesign_id) references studydesign (studydesign_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint not null,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank int not null default 0,
    constraint studydesignprop_c1 unique (studydesign_id,type_id,rank)
);
create index studydesignprop_idx1 on studydesignprop (studydesign_id);
create index studydesignprop_idx2 on studydesignprop (type_id);

COMMENT ON TABLE studydesignprop IS NULL;

-- ================================================
-- TABLE: studyfactor
-- ================================================

create table studyfactor (
    studyfactor_id bigserial not null,
    primary key (studyfactor_id),
    studydesign_id bigint not null,
    foreign key (studydesign_id) references studydesign (studydesign_id) on delete cascade INITIALLY DEFERRED,
    type_id bigint null,
    foreign key (type_id) references cvterm (cvterm_id) on delete set null INITIALLY DEFERRED,
    name text not null,
    description text null
);
create index studyfactor_idx1 on studyfactor (studydesign_id);
create index studyfactor_idx2 on studyfactor (type_id);

COMMENT ON TABLE studyfactor IS NULL;

-- ================================================
-- TABLE: studyfactorvalue
-- ================================================

create table studyfactorvalue (
    studyfactorvalue_id bigserial not null,
    primary key (studyfactorvalue_id),
    studyfactor_id bigint not null,
    foreign key (studyfactor_id) references studyfactor (studyfactor_id) on delete cascade INITIALLY DEFERRED,
    assay_id bigint not null,
    foreign key (assay_id) references assay (assay_id) on delete cascade INITIALLY DEFERRED,
    factorvalue text null,
    name text null,
    rank int not null default 0
);
create index studyfactorvalue_idx1 on studyfactorvalue (studyfactor_id);
create index studyfactorvalue_idx2 on studyfactorvalue (assay_id);

COMMENT ON TABLE studyfactorvalue IS NULL;

--
-- studyprop and studyprop_feature added for Kara Dolinski's group
-- 
-- Here is her description of it:
--Both of the tables are used for our YFGdb project 
--(http://yfgdb.princeton.edu/), which uses chado. 
--
--Here is how we use those tables, using the following example:
--
--http://yfgdb.princeton.edu/cgi-bin/display.cgi?db=pmid&amp;id=15575969
--
--The above data set is represented as a row in the STUDY table.  We have
--lots of attributes that we want to store about each STUDY (status, etc)
--and in the official schema, the only prop table we could use was the
--STUDYDESIGN_PROP table.  This forced us to go through the STUDYDESIGN 
--table when we often have no real data to store in that table (small 
--percent of our collection use MAGE-ML unfortunately, and even fewer 
--provide all the data in the MAGE model, of which STUDYDESIGN is a vestige). 
--So, we created a STUDYPROP table.  I'd think this table would be 
--generally useful to people storing various types of data sets via the 
--STUDY table.
--
--The other new table is STUDYPROP_FEATURE.  This basically allows us to
--group features together per study.  For example, we can store microarray
--clustering results by saying that the STUDYPROP type is 'cluster'  (via 
--type_id -> CVTERM of course), the value is 'cluster id 123', and then
--that cluster would be associated with all the features that are in that
--cluster via STUDYPROP_FEATURE.  Adding type_id to STUDYPROP_FEATURE is
-- fine by us!
--
--studyprop
create table studyprop (
    studyprop_id bigserial not null,
        primary key (studyprop_id),
    study_id bigint not null,
        foreign key (study_id) references study (study_id) on delete cascade,
    type_id bigint not null,
        foreign key (type_id) references cvterm (cvterm_id) on delete cascade,  
    value text null,
    rank int not null default 0,
    unique (study_id,type_id,rank)
);

create index studyprop_idx1 on studyprop (study_id);
create index studyprop_idx2 on studyprop (type_id);


--studyprop_feature
CREATE TABLE studyprop_feature (
    studyprop_feature_id bigserial NOT NULL,
    primary key (studyprop_feature_id),
    studyprop_id bigint NOT NULL,
    foreign key (studyprop_id) references studyprop(studyprop_id) on delete cascade,
    feature_id bigint NOT NULL,
    foreign key (feature_id) references feature (feature_id) on delete cascade,
    type_id bigint,
    foreign key (type_id) references cvterm (cvterm_id) on delete cascade,
    unique (studyprop_id, feature_id)
    );
 

create index studyprop_feature_idx1 on studyprop_feature (studyprop_id);
create index studyprop_feature_idx2 on studyprop_feature (feature_id);

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

-- ================================================
-- TABLE: cell_line
-- ================================================

create table cell_line (
        cell_line_id bigserial not null,
        primary key (cell_line_id),
        name varchar(255) null,
        uniquename varchar(255) not null,
	organism_id bigint not null,
	foreign key (organism_id) references organism (organism_id) on delete cascade INITIALLY DEFERRED,
	timeaccessioned timestamp not null default current_timestamp,
	timelastmodified timestamp not null default current_timestamp,
        constraint cell_line_c1 unique (uniquename, organism_id)
);
grant all on cell_line to PUBLIC;


-- ================================================

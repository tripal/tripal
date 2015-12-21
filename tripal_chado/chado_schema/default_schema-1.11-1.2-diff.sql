 create table cvprop (
     cvprop_id serial not null,
     primary key (cvprop_id),
     cv_id int not null,
     foreign key (cv_id) references cv (cv_id) INITIALLY DEFERRED,
     type_id int not null,
     foreign key (type_id) references cvterm (cvterm_id) INITIALLY DEFERRED,
     value text,
     rank int not null default 0,
     constraint cvprop_c1 unique (cv_id,type_id,rank)
 );
 
 COMMENT ON TABLE cvprop IS 'Additional extensible properties can be attached to a cv using this table.  A notable example would be the cv version';
 
 COMMENT ON COLUMN cvprop.type_id IS 'The name of the property or slot is a cvterm. The meaning of the property is defined in that cvterm.';
 COMMENT ON COLUMN cvprop.value IS 'The value of the property, represented as text. Numeric values are converted to their text representation.';
 
 COMMENT ON COLUMN cvprop.rank IS 'Property-Value ordering. Any
 cv can have multiple values for any particular property type -
 these are ordered in a list using rank, counting from zero. For
 properties that are single-valued rather than multi-valued, the
 default 0 value should be used.';
 
 create table chadoprop (
     chadoprop_id serial not null,
     primary key (chadoprop_id),
     type_id int not null,
     foreign key (type_id) references cvterm (cvterm_id) INITIALLY DEFERRED,
     value text,
     rank int not null default 0,
     constraint chadoprop_c1 unique (type_id,rank)
 );
 
 COMMENT ON TABLE chadoprop IS 'This table is different from other prop tables in the database, as it is for storing information about the database itself, like schema version';
 
 COMMENT ON COLUMN chadoprop.type_id IS 'The name of the property or slot is a cvterm. The meaning of the property is defined in that cvterm.';
 COMMENT ON COLUMN chadoprop.value IS 'The value of the property, represented as text. Numeric values are converted to their text representation.';
 
 COMMENT ON COLUMN chadoprop.rank IS 'Property-Value ordering. Any
 cv can have multiple values for any particular property type -
 these are ordered in a list using rank, counting from zero. For
 properties that are single-valued rather than multi-valued, the
 default 0 value should be used.';

ALTER TABLE genetic_code.gencode_startcodon ADD  CONSTRAINT gencode_startcodon_unique UNIQUE( gencode_id, codon );

ALTER TABLE phenotype ADD COLUMN name TEXT default null;

ALTER TABLE genotype ADD COLUMN type_id INT NOT NULL;
ALTER TABLE genotype ADD CONSTRAINT genotype_type_id_fkey FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE CASCADE;


 create table genotypeprop (
     genotypeprop_id serial not null,
     primary key (genotypeprop_id),
     genotype_id int not null,
     foreign key (genotype_id) references genotype (genotype_id) on delete cascade INITIALLY DEFERRED,
     type_id int not null,
     foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
     value text null,
     rank int not null default 0,
     constraint genotypeprop_c1 unique (genotype_id,type_id,rank)
 );
 create index genotypeprop_idx1 on genotypeprop (genotype_id);
 create index genotypeprop_idx2 on genotypeprop (type_id);
 
 CREATE TABLE projectprop (
 	projectprop_id serial NOT NULL,
 	PRIMARY KEY (projectprop_id),
 	project_id integer NOT NULL,
 	FOREIGN KEY (project_id) REFERENCES project (project_id) ON DELETE CASCADE,
 	type_id integer NOT NULL,
 	FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE CASCADE,
 	value text,
 	rank integer not null default 0,
 	CONSTRAINT projectprop_c1 UNIQUE (project_id, type_id, rank)
 );
 
 -- ================================================
 -- TABLE: project_relationship
 -- ================================================
 
 CREATE TABLE project_relationship (
 	project_relationship_id serial NOT NULL,
 	PRIMARY KEY (project_relationship_id),
 	subject_project_id integer NOT NULL,
 	FOREIGN KEY (subject_project_id) REFERENCES project (project_id) ON DELETE CASCADE,
 	object_project_id integer NOT NULL,
 	FOREIGN KEY (object_project_id) REFERENCES project (project_id) ON DELETE CASCADE,
 	type_id integer NOT NULL,
 	FOREIGN KEY (type_id) REFERENCES cvterm (cvterm_id) ON DELETE RESTRICT,
 	CONSTRAINT project_relationship_c1 UNIQUE (subject_project_id, object_project_id, type_id)
 );
 COMMENT ON TABLE project_relationship IS 'A project can be composed of several smaller scale projects';
 COMMENT ON COLUMN project_relationship.type_id IS 'The type of relationship being stated, such as "is part of".';
 
 
 create table project_pub (
        project_pub_id serial not null,
        primary key (project_pub_id),
        project_id int not null,
        foreign key (project_id) references project (project_id) on delete cascade INITIALLY DEFERRED,
        pub_id int not null,
        foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
        constraint project_pub_c1 unique (project_id,pub_id)
 );
 create index project_pub_idx1 on project_pub (project_id);
 create index project_pub_idx2 on project_pub (pub_id);
 
 COMMENT ON TABLE project_pub IS 'Linking project(s) to publication(s)';
 
 
 create table project_contact (
        project_contact_id serial not null,
        primary key (project_contact_id),
        project_id int not null,
        foreign key (project_id) references project (project_id) on delete cascade INITIALLY DEFERRED,
        contact_id int not null,
        foreign key (contact_id) references contact (contact_id) on delete cascade INITIALLY DEFERRED,
        constraint project_contact_c1 unique (project_id,contact_id)
 );
 create index project_contact_idx1 on project_contact (project_id);
 create index project_contact_idx2 on project_contact (contact_id);
 
 COMMENT ON TABLE project_contact IS 'Linking project(s) to contact(s)';

ALTER TABLE stock alter organism_id drop not null;

 COMMENT ON COLUMN stock.organism_id IS 'The organism_id is the organism to which the stock belongs. This column is mandatory.';
 
 CREATE TABLE stock_relationship_cvterm (
 	stock_relationship_cvterm_id SERIAL NOT NULL,
 	PRIMARY KEY (stock_relationship_cvterm_id),
 	stock_relationship_id integer NOT NULL,
 	FOREIGN KEY (stock_relationship_id) references stock_relationship (stock_relationship_id) ON DELETE CASCADE INITIALLY DEFERRED,
 	cvterm_id integer NOT NULL,
 	FOREIGN KEY (cvterm_id) REFERENCES cvterm (cvterm_id) ON DELETE RESTRICT,
 	pub_id integer,
 	FOREIGN KEY (pub_id) REFERENCES pub (pub_id) ON DELETE RESTRICT
 );
COMMENT ON TABLE stock_relationship_cvterm is 'For germplasm maintenance and pedigree data, stock_relationship. type_id will record cvterms such as "is a female parent of", "a parent for mutation", "is a group_id of", "is a source_id of", etc The cvterms for higher categories such as "generative", "derivative" or "maintenance" can be stored in table stock_relationship_cvterm';


alter table stock_cvterm add column is_not boolean not null default false;
alter table stock_cvterm add column rank integer not null default 0;
alter table stock_cvterm drop constraint stock_cvterm_c1;
alter table stock_cvterm add constraint stock_cvterm_c1 unique (stock_id,cvterm_id,pub_id,rank)
;


 create table stock_cvtermprop (
     stock_cvtermprop_id serial not null,
     primary key (stock_cvtermprop_id),
     stock_cvterm_id int not null,
     foreign key (stock_cvterm_id) references stock_cvterm (stock_cvterm_id) on delete cascade,
     type_id int not null,
     foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
     value text null,
     rank int not null default 0,
     constraint stock_cvtermprop_c1 unique (stock_cvterm_id,type_id,rank)
 );
 create index stock_cvtermprop_idx1 on stock_cvtermprop (stock_cvterm_id);
 create index stock_cvtermprop_idx2 on stock_cvtermprop (type_id);
 
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
 
 create table stock_dbxrefprop (
        stock_dbxrefprop_id serial not null,
        primary key (stock_dbxrefprop_id),
        stock_dbxref_id int not null,
        foreign key (stock_dbxref_id) references stock_dbxref (stock_dbxref_id) on delete cascade INITIALLY DEFERRED,
        type_id int not null,
        foreign key (type_id) references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
        value text null,
        rank int not null default 0,
        constraint stock_dbxrefprop_c1 unique (stock_dbxref_id,type_id,rank)
 );
 create index stock_dbxrefprop_idx1 on stock_dbxrefprop (stock_dbxref_id);
 create index stock_dbxrefprop_idx2 on stock_dbxrefprop (type_id);
 
 COMMENT ON TABLE stock_dbxrefprop IS 'A stock_dbxref can have any number of
 slot-value property tags attached to it. This is useful for storing properties related to dbxref annotations of stocks, such as evidence codes, and references, and metadata, such as create/modify dates. This is an alternative to
 hardcoding a list of columns in the relational schema, and is
 completely extensible. There is a unique constraint, stock_dbxrefprop_c1, for
 the combination of stock_dbxref_id, rank, and type_id. Multivalued property-value pairs must be differentiated by rank.';

-- VIEW gffatts: a view to get feature attributes in a format that
-- will make it easy to convert them to GFF attributes

CREATE OR REPLACE VIEW gffatts (
    feature_id,
    type,
    attribute
) AS
SELECT feature_id, 'Ontology_term' AS type,  s.name AS attribute
FROM cvterm s, feature_cvterm fs
WHERE fs.cvterm_id = s.cvterm_id
UNION ALL
SELECT feature_id, 'Dbxref' AS type, d.name || ':' || s.accession AS attribute
FROM dbxref s, feature_dbxref fs, db d
WHERE fs.dbxref_id = s.dbxref_id and s.db_id = d.db_id
UNION ALL
SELECT feature_id, 'Alias' AS type, s.name AS attribute
FROM synonym s, feature_synonym fs
WHERE fs.synonym_id = s.synonym_id
UNION ALL
SELECT fp.feature_id,cv.name,fp.value
FROM featureprop fp, cvterm cv
WHERE fp.type_id = cv.cvterm_id
UNION ALL
SELECT feature_id, 'pub' AS type, s.series_name || ':' || s.title AS attribute
FROM pub s, feature_pub fs
WHERE fs.pub_id = s.pub_id;

CREATE OR REPLACE VIEW gff3atts (
    feature_id,
    type,
    attribute
) AS
SELECT feature_id,
      'Ontology_term' AS type,
      CASE WHEN db.name like '%Gene Ontology%'    THEN 'GO:'|| dbx.accession
           WHEN db.name like 'Sequence Ontology%' THEN 'SO:'|| dbx.accession
           ELSE                            CAST(db.name||':'|| dbx.accession AS varchar)
      END
FROM cvterm s, dbxref dbx, feature_cvterm fs, db
WHERE fs.cvterm_id = s.cvterm_id and s.dbxref_id=dbx.dbxref_id and
      db.db_id = dbx.db_id
UNION ALL
SELECT feature_id, 'Dbxref' AS type, d.name || ':' || s.accession AS
attribute
FROM dbxref s, feature_dbxref fs, db d
WHERE fs.dbxref_id = s.dbxref_id and s.db_id = d.db_id and
      d.name != 'GFF_source'
UNION ALL
SELECT f.feature_id, 'Alias' AS type, s.name AS attribute
FROM synonym s, feature_synonym fs, feature f
WHERE fs.synonym_id = s.synonym_id and f.feature_id = fs.feature_id and
      f.name != s.name and f.uniquename != s.name
UNION ALL
SELECT fp.feature_id,cv.name,fp.value
FROM featureprop fp, cvterm cv
WHERE fp.type_id = cv.cvterm_id
UNION ALL
SELECT feature_id, 'pub' AS type, s.series_name || ':' || s.title AS
attribute
FROM pub s, feature_pub fs
WHERE fs.pub_id = s.pub_id
UNION ALL
SELECT fr.subject_id as feature_id, 'Parent' as type,  parent.uniquename
as attribute
FROM feature_relationship fr, feature parent
WHERE  fr.object_id=parent.feature_id AND fr.type_id = (SELECT cvterm_id
FROM cvterm WHERE name='part_of' and cv_id in (select cv_id
  FROM cv WHERE name='relationship'))
UNION ALL
SELECT fr.subject_id as feature_id, 'Derives_from' as type,
parent.uniquename as attribute
FROM feature_relationship fr, feature parent
WHERE  fr.object_id=parent.feature_id AND fr.type_id = (SELECT cvterm_id
FROM cvterm WHERE name='derives_from' and cv_id in (select cv_id
  FROM cv WHERE name='relationship'))
UNION ALL
SELECT fl.feature_id, 'Target' as type, target.name || ' ' || fl.fmin+1
|| ' ' || fl.fmax || ' ' || fl.strand as attribute
FROM featureloc fl, feature target
WHERE fl.srcfeature_id=target.feature_id
        AND fl.rank != 0
UNION ALL
SELECT feature_id, 'ID' as type, uniquename as attribute
FROM feature
WHERE type_id NOT IN (SELECT cvterm_id FROM cvterm WHERE name='CDS')
UNION ALL
SELECT feature_id, 'chado_feature_id' as type, CAST(feature_id AS
varchar) as attribute
FROM feature
UNION ALL
SELECT feature_id, 'Name' as type, name as attribute
FROM feature;


-- =================================================================
-- Dependencies:
--
-- :import feature from sequence
-- :import cvterm from cv
-- :import pub from pub
-- :import phenotype from phenotype
-- :import organism from organism
-- :import genotype from genetic
-- :import contact from contact
-- :import project from project
-- :import stock from stock
-- :import synonym
-- =================================================================


-- this probably needs some work, depending on how cross-database we
-- want to be.  In Postgres, at least, there are much better ways to 
-- represent geo information.

CREATE TABLE nd_geolocation (
    nd_geolocation_id serial PRIMARY KEY NOT NULL,
    description character varying(255),
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



CREATE TABLE nd_experiment (
    nd_experiment_id serial PRIMARY KEY NOT NULL,
    nd_geolocation_id integer NOT NULL references nd_geolocation (nd_geolocation_id) on delete cascade INITIALLY DEFERRED,
    type_id integer NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED 
);

--
--used to be nd_diversityexperiment_project
--then was nd_assay_project
CREATE TABLE nd_experiment_project (
    nd_experiment_project_id serial PRIMARY KEY NOT NULL,
    project_id integer not null references project (project_id) on delete cascade INITIALLY DEFERRED,
    nd_experiment_id integer NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED
);



CREATE TABLE nd_experimentprop (
    nd_experimentprop_id serial PRIMARY KEY NOT NULL,
    nd_experiment_id integer NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    type_id integer NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED ,
    value text null,
    rank integer NOT NULL default 0,
    constraint nd_experimentprop_c1 unique (nd_experiment_id,type_id,rank)
);

CREATE TABLE nd_experiment_pub (
       nd_experiment_pub_id serial PRIMARY KEY not null,
       nd_experiment_id int not null,
       foreign key (nd_experiment_id) references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
       pub_id int not null,
       foreign key (pub_id) references pub (pub_id) on delete cascade INITIALLY DEFERRED,
       constraint nd_experiment_pub_c1 unique (nd_experiment_id,pub_id)
);
create index nd_experiment_pub_idx1 on nd_experiment_pub (nd_experiment_id);
create index nd_experiment_pub_idx2 on nd_experiment_pub (pub_id);

COMMENT ON TABLE nd_experiment_pub IS 'Linking nd_experiment(s) to publication(s)';




CREATE TABLE nd_geolocationprop (
    nd_geolocationprop_id serial PRIMARY KEY NOT NULL,
    nd_geolocation_id integer NOT NULL references nd_geolocation (nd_geolocation_id) on delete cascade INITIALLY DEFERRED,
    type_id integer NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank integer NOT NULL DEFAULT 0,
    constraint nd_geolocationprop_c1 unique (nd_geolocation_id,type_id,rank)
);

COMMENT ON TABLE nd_geolocationprop IS 'Property/value associations for geolocations. This table can store the properties such as location and environment';

COMMENT ON COLUMN nd_geolocationprop.type_id IS 'The name of the property as a reference to a controlled vocabulary term.';

COMMENT ON COLUMN nd_geolocationprop.value IS 'The value of the property.';

COMMENT ON COLUMN nd_geolocationprop.rank IS 'The rank of the property value, if the property has an array of values.';


CREATE TABLE nd_protocol (
    nd_protocol_id serial PRIMARY KEY  NOT NULL,
    name character varying(255) NOT NULL unique,
    type_id integer NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED
);

COMMENT ON TABLE nd_protocol IS 'A protocol can be anything that is done as part of the experiment.';

COMMENT ON COLUMN nd_protocol.name IS 'The protocol name.';

CREATE TABLE nd_reagent (
    nd_reagent_id serial PRIMARY KEY NOT NULL,
    name character varying(80) NOT NULL,
    type_id integer NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    feature_id integer
);

COMMENT ON TABLE nd_reagent IS 'A reagent such as a primer, an enzyme, an adapter oligo, a linker oligo. Reagents are used in genotyping experiments, or in any other kind of experiment.';

COMMENT ON COLUMN nd_reagent.name IS 'The name of the reagent. The name should be unique for a given type.';

COMMENT ON COLUMN nd_reagent.type_id IS 'The type of the reagent, for example linker oligomer, or forward primer.';

COMMENT ON COLUMN nd_reagent.feature_id IS 'If the reagent is a primer, the feature that it corresponds to. More generally, the corresponding feature for any reagent that has a sequence that maps to another sequence.';



CREATE TABLE nd_protocol_reagent (
    nd_protocol_reagent_id serial PRIMARY KEY NOT NULL,
    nd_protocol_id integer NOT NULL references nd_protocol (nd_protocol_id) on delete cascade INITIALLY DEFERRED,
    reagent_id integer NOT NULL references nd_reagent (nd_reagent_id) on delete cascade INITIALLY DEFERRED,
    type_id integer NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED
);


CREATE TABLE nd_protocolprop (
    nd_protocolprop_id serial PRIMARY KEY NOT NULL,
    nd_protocol_id integer NOT NULL references nd_protocol (nd_protocol_id) on delete cascade INITIALLY DEFERRED,
    type_id integer NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank integer DEFAULT 0 NOT NULL,
    constraint nd_protocolprop_c1 unique (nd_protocol_id,type_id,rank)
);

COMMENT ON TABLE nd_protocolprop IS 'Property/value associations for protocol.';

COMMENT ON COLUMN nd_protocolprop.nd_protocol_id IS 'The protocol to which the property applies.';

COMMENT ON COLUMN nd_protocolprop.type_id IS 'The name of the property as a reference to a controlled vocabulary term.';

COMMENT ON COLUMN nd_protocolprop.value IS 'The value of the property.';

COMMENT ON COLUMN nd_protocolprop.rank IS 'The rank of the property value, if the property has an array of values.';



CREATE TABLE nd_experiment_stock (
    nd_experiment_stock_id serial PRIMARY KEY NOT NULL,
    nd_experiment_id integer NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    stock_id integer NOT NULL references stock (stock_id)  on delete cascade INITIALLY DEFERRED,
    type_id integer NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED
);

COMMENT ON TABLE nd_experiment_stock IS 'Part of a stock or a clone of a stock that is used in an experiment';


COMMENT ON COLUMN nd_experiment_stock.stock_id IS 'stock used in the extraction or the corresponding stock for the clone';


CREATE TABLE nd_experiment_protocol (
    nd_experiment_protocol_id serial PRIMARY KEY NOT NULL,
    nd_experiment_id integer NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    nd_protocol_id integer NOT NULL references nd_protocol (nd_protocol_id) on delete cascade INITIALLY DEFERRED
);

COMMENT ON TABLE nd_experiment_protocol IS 'Linking table: experiments to the protocols they involve.';


CREATE TABLE nd_experiment_phenotype (
    nd_experiment_phenotype_id serial PRIMARY KEY NOT NULL,
    nd_experiment_id integer NOT NULL REFERENCES nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    phenotype_id integer NOT NULL references phenotype (phenotype_id) on delete cascade INITIALLY DEFERRED,
   constraint nd_experiment_phenotype_c1 unique (nd_experiment_id,phenotype_id)
); 

COMMENT ON TABLE nd_experiment_phenotype IS 'Linking table: experiments to the phenotypes they produce. There is a one-to-one relationship between an experiment and a phenotype since each phenotype record should point to one experiment. Add a new experiment_id for each phenotype record.';

CREATE TABLE nd_experiment_genotype (
    nd_experiment_genotype_id serial PRIMARY KEY NOT NULL,
    nd_experiment_id integer NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    genotype_id integer NOT NULL references genotype (genotype_id) on delete cascade INITIALLY DEFERRED ,
    constraint nd_experiment_genotype_c1 unique (nd_experiment_id,genotype_id)
);

COMMENT ON TABLE nd_experiment_genotype IS 'Linking table: experiments to the genotypes they produce. There is a one-to-one relationship between an experiment and a genotype since each genotype record should point to one experiment. Add a new experiment_id for each genotype record.';


CREATE TABLE nd_reagent_relationship (
    nd_reagent_relationship_id serial PRIMARY KEY NOT NULL,
    subject_reagent_id integer NOT NULL references nd_reagent (nd_reagent_id) on delete cascade INITIALLY DEFERRED,
    object_reagent_id integer NOT NULL  references nd_reagent (nd_reagent_id) on delete cascade INITIALLY DEFERRED,
    type_id integer NOT NULL  references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED
);

COMMENT ON TABLE nd_reagent_relationship IS 'Relationships between reagents. Some reagents form a group. i.e., they are used all together or not at all. Examples are adapter/linker/enzyme experiment reagents.';

COMMENT ON COLUMN nd_reagent_relationship.subject_reagent_id IS 'The subject reagent in the relationship. In parent/child terminology, the subject is the child. For example, in "linkerA 3prime-overhang-linker enzymeA" linkerA is the subject, 3prime-overhand-linker is the type, and enzymeA is the object.';

COMMENT ON COLUMN nd_reagent_relationship.object_reagent_id IS 'The object reagent in the relationship. In parent/child terminology, the object is the parent. For example, in "linkerA 3prime-overhang-linker enzymeA" linkerA is the subject, 3prime-overhand-linker is the type, and enzymeA is the object.';

COMMENT ON COLUMN nd_reagent_relationship.type_id IS 'The type (or predicate) of the relationship. For example, in "linkerA 3prime-overhang-linker enzymeA" linkerA is the subject, 3prime-overhand-linker is the type, and enzymeA is the object.';


CREATE TABLE nd_reagentprop (
    nd_reagentprop_id serial PRIMARY KEY NOT NULL,
    nd_reagent_id integer NOT NULL references nd_reagent (nd_reagent_id) on delete cascade INITIALLY DEFERRED,
    type_id integer NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank integer DEFAULT 0 NOT NULL,
    constraint nd_reagentprop_c1 unique (nd_reagent_id,type_id,rank)
);

CREATE TABLE nd_experiment_stockprop (
    nd_experiment_stockprop_id serial PRIMARY KEY NOT NULL,
    nd_experiment_stock_id integer NOT NULL references nd_experiment_stock (nd_experiment_stock_id) on delete cascade INITIALLY DEFERRED,
    type_id integer NOT NULL references cvterm (cvterm_id) on delete cascade INITIALLY DEFERRED,
    value text null,
    rank integer DEFAULT 0 NOT NULL,
    constraint nd_experiment_stockprop_c1 unique (nd_experiment_stock_id,type_id,rank)
);

COMMENT ON TABLE nd_experiment_stockprop IS 'Property/value associations for experiment_stocks. This table can store the properties such as treatment';

COMMENT ON COLUMN nd_experiment_stockprop.nd_experiment_stock_id IS 'The experiment_stock to which the property applies.';

COMMENT ON COLUMN nd_experiment_stockprop.type_id IS 'The name of the property as a reference to a controlled vocabulary term.';

COMMENT ON COLUMN nd_experiment_stockprop.value IS 'The value of the property.';

COMMENT ON COLUMN nd_experiment_stockprop.rank IS 'The rank of the property value, if the property has an array of values.';


CREATE TABLE nd_experiment_stock_dbxref (
    nd_experiment_stock_dbxref_id serial PRIMARY KEY NOT NULL,
    nd_experiment_stock_id integer NOT NULL references nd_experiment_stock (nd_experiment_stock_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id integer NOT NULL references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED
);

COMMENT ON TABLE nd_experiment_stock_dbxref IS 'Cross-reference experiment_stock to accessions, images, etc';



CREATE TABLE nd_experiment_dbxref (
    nd_experiment_dbxref_id serial PRIMARY KEY NOT NULL,
    nd_experiment_id integer NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    dbxref_id integer NOT NULL references dbxref (dbxref_id) on delete cascade INITIALLY DEFERRED
);

COMMENT ON TABLE nd_experiment_dbxref IS 'Cross-reference experiment to accessions, images, etc';


CREATE TABLE nd_experiment_contact (
    nd_experiment_contact_id serial PRIMARY KEY NOT NULL,
    nd_experiment_id integer NOT NULL references nd_experiment (nd_experiment_id) on delete cascade INITIALLY DEFERRED,
    contact_id integer NOT NULL references contact (contact_id) on delete cascade INITIALLY DEFERRED
);

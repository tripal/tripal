create table chado.db_relationship (
    db_relationship_id bigserial not null,
    type_id bigint not null,
    subject_id bigint not null,
    object_id bigint not null,
    primary key (db_relationship_id),
    foreign key (type_id) references chado.db (db_id) on delete cascade INITIALLY DEFERRED,
    foreign key (subject_id) references chado.db (db_id) on delete cascade INITIALLY DEFERRED,
    foreign key (object_id) references chado.db (db_id) on delete cascade INITIALLY DEFERRED,
    constraint db_relationship_c1 unique (subject_id,object_id,type_id)
);
create index db_relationship_idx1 on chado.db_relationship USING btree (type_id);
create index db_relationship_idx2 on chado.db_relationship USING btree (subject_id);
create index db_relationship_idx3 on chado.db_relationship USING btree (object_id);

COMMENT ON TABLE chado.db_relationship IS 'Specifies relationships between databases.  This is
particularly useful for ontologies that use multiple prefix IDs for its vocabularies. For example,
the EDAM ontology uses the prefixes "data", "format", "operation" and others. Each of these would
have a record in the db table.  An "EDAM" record could be added for the entire ontology to the
db table and the previous records could be linked as "part_of" EDAM.  As another example
databases housing cross-references may have sub databases such as NCBI (e.g. Taxonomy, SRA, etc).
This table can use a "part_of" record to link all of them to NCBI.'



CREATE TABLE public.tripal_admin_notifications (
    note_id integer NOT NULL,
    details text NOT NULL,
    title text NOT NULL,
    actions text,
    submitter_id text NOT NULL,
    enabled integer DEFAULT 1 NOT NULL,
    type text
);


ALTER TABLE public.tripal_admin_notifications OWNER TO drupal;


COMMENT ON TABLE public.tripal_admin_notifications IS 'This table is used for information describing administrative
     notifications. For example, when new fields are available.';



COMMENT ON COLUMN public.tripal_admin_notifications.details IS 'Description and additional information relating to the notification.';



COMMENT ON COLUMN public.tripal_admin_notifications.title IS 'Title of the notification.';



COMMENT ON COLUMN public.tripal_admin_notifications.actions IS 'Actions that can be performed on the notification,::text like disimissal or import.';



COMMENT ON COLUMN public.tripal_admin_notifications.submitter_id IS 'A unique id that should be specific to the notification to ensure notifications are not duplicated.';



COMMENT ON COLUMN public.tripal_admin_notifications.enabled IS 'Boolean indicating whether the notification is enabled or disabled (disabled will not be shown on the dashboard).';



COMMENT ON COLUMN public.tripal_admin_notifications.type IS 'Type of the notification, relating to what tripal function the notification belongs to, IE Fields, Jobs, Vocabulary.';



CREATE SEQUENCE public.tripal_admin_notifications_note_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_admin_notifications_note_id_seq OWNER TO drupal;


ALTER SEQUENCE public.tripal_admin_notifications_note_id_seq OWNED BY public.tripal_admin_notifications.note_id;



CREATE TABLE public.tripal_collection (
    collection_id integer NOT NULL,
    collection_name character varying(1024) NOT NULL,
    description text,
    uid integer NOT NULL,
    create_date integer NOT NULL,
    CONSTRAINT tripal_collection_collection_id_check CHECK ((collection_id >= 0))
);


ALTER TABLE public.tripal_collection OWNER TO drupal;


COMMENT ON COLUMN public.tripal_collection.uid IS 'The user Id of the person who created the collection.';



COMMENT ON COLUMN public.tripal_collection.create_date IS 'UNIX integer start time';



CREATE TABLE public.tripal_collection_bundle (
    collection_bundle_id integer NOT NULL,
    collection_id bigint NOT NULL,
    bundle_name character varying(1024) NOT NULL,
    ids text NOT NULL,
    fields text NOT NULL,
    site_id integer,
    CONSTRAINT tripal_collection_bundle_collection_bundle_id_check CHECK ((collection_bundle_id >= 0)),
    CONSTRAINT tripal_collection_bundle_collection_id_check CHECK ((collection_id >= 0))
);


ALTER TABLE public.tripal_collection_bundle OWNER TO drupal;


COMMENT ON COLUMN public.tripal_collection_bundle.ids IS 'An array of entity IDs.';



COMMENT ON COLUMN public.tripal_collection_bundle.fields IS 'An array of numeric field IDs.';



COMMENT ON COLUMN public.tripal_collection_bundle.site_id IS 'The ID of the site from the Tripal Sites table.';



CREATE SEQUENCE public.tripal_collection_bundle_collection_bundle_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_collection_bundle_collection_bundle_id_seq OWNER TO drupal;


ALTER SEQUENCE public.tripal_collection_bundle_collection_bundle_id_seq OWNED BY public.tripal_collection_bundle.collection_bundle_id;



CREATE SEQUENCE public.tripal_collection_collection_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_collection_collection_id_seq OWNER TO drupal;


ALTER SEQUENCE public.tripal_collection_collection_id_seq OWNED BY public.tripal_collection.collection_id;



CREATE TABLE public.tripal_custom_quota (
    uid bigint NOT NULL,
    custom_quota bigint NOT NULL,
    custom_expiration bigint NOT NULL
);


ALTER TABLE public.tripal_custom_quota OWNER TO drupal;


CREATE TABLE public.tripal_custom_tables (
    table_id integer NOT NULL,
    table_name character varying(255) NOT NULL,
    schema text NOT NULL,
    hidden smallint DEFAULT 0,
    chado character varying(64) NOT NULL,
    CONSTRAINT tripal_custom_tables_table_id_check CHECK ((table_id >= 0))
);


ALTER TABLE public.tripal_custom_tables OWNER TO drupal;


COMMENT ON COLUMN public.tripal_custom_tables.hidden IS 'Set to true if this custom table is not for end-users to manage, but for the Tripal module.';



COMMENT ON COLUMN public.tripal_custom_tables.chado IS 'The name of the Chado schema where this table exists.';



CREATE SEQUENCE public.tripal_custom_tables_table_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_custom_tables_table_id_seq OWNER TO drupal;


ALTER SEQUENCE public.tripal_custom_tables_table_id_seq OWNED BY public.tripal_custom_tables.table_id;



CREATE TABLE public.tripal_cv_obo (
    obo_id integer NOT NULL,
    name character varying(255),
    path character varying(1024),
    CONSTRAINT tripal_cv_obo_obo_id_check CHECK ((obo_id >= 0))
);


ALTER TABLE public.tripal_cv_obo OWNER TO drupal;


CREATE SEQUENCE public.tripal_cv_obo_obo_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_cv_obo_obo_id_seq OWNER TO drupal;


ALTER SEQUENCE public.tripal_cv_obo_obo_id_seq OWNED BY public.tripal_cv_obo.obo_id;



CREATE TABLE public.tripal_entity (
    id integer NOT NULL,
    type character varying(32) NOT NULL,
    uid bigint,
    title character varying(1024),
    status smallint NOT NULL,
    created integer,
    changed integer,
    CONSTRAINT tripal_entity_id_check CHECK ((id >= 0)),
    CONSTRAINT tripal_entity_uid_check CHECK ((uid >= 0))
);


ALTER TABLE public.tripal_entity OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity IS 'The base table for tripal_entity entities.';



COMMENT ON COLUMN public.tripal_entity.type IS 'The ID of the target entity.';



COMMENT ON COLUMN public.tripal_entity.uid IS 'The ID of the target entity.';



CREATE TABLE public.tripal_entity__phylotree_schema_comment (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    phylotree_schema_comment_value text,
    phylotree_schema_comment_record_id integer,
    CONSTRAINT tripal_entity__phylotree_schema_comment_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__phylotree_schema_comment_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__phylotree_schema_comment_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__phylotree_schema_comment OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__phylotree_schema_comment IS 'Data storage for tripal_entity field phylotree_schema_comment.';



COMMENT ON COLUMN public.tripal_entity__phylotree_schema_comment.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__phylotree_schema_comment.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__phylotree_schema_comment.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__phylotree_schema_comment.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__phylotree_schema_comment.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__phylotree_schema_comment.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__phylotree_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    phylotree_schema_name_value character varying(255),
    phylotree_schema_name_record_id integer,
    CONSTRAINT tripal_entity__phylotree_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__phylotree_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__phylotree_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__phylotree_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__phylotree_schema_name IS 'Data storage for tripal_entity field phylotree_schema_name.';



COMMENT ON COLUMN public.tripal_entity__phylotree_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__phylotree_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__phylotree_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__phylotree_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__phylotree_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__phylotree_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__physical_map_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    physical_map_schema_description_value text,
    physical_map_schema_description_record_id integer,
    CONSTRAINT tripal_entity__physical_map_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__physical_map_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__physical_map_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__physical_map_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__physical_map_schema_description IS 'Data storage for tripal_entity field physical_map_schema_description.';



COMMENT ON COLUMN public.tripal_entity__physical_map_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__physical_map_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__physical_map_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__physical_map_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__physical_map_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__physical_map_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__physical_map_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    physical_map_schema_name_value character varying(255),
    physical_map_schema_name_record_id integer,
    CONSTRAINT tripal_entity__physical_map_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__physical_map_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__physical_map_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__physical_map_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__physical_map_schema_name IS 'Data storage for tripal_entity field physical_map_schema_name.';



COMMENT ON COLUMN public.tripal_entity__physical_map_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__physical_map_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__physical_map_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__physical_map_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__physical_map_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__physical_map_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__dna_library_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    dna_library_data_0842_value text,
    dna_library_data_0842_record_id integer,
    CONSTRAINT tripal_entity__dna_library_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__dna_library_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__dna_library_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__dna_library_data_0842 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__dna_library_data_0842 IS 'Data storage for tripal_entity field dna_library_data_0842.';



COMMENT ON COLUMN public.tripal_entity__dna_library_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__dna_library_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__dna_library_data_0842.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__dna_library_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__dna_library_data_0842.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__dna_library_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__dna_library_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    dna_library_schema_name_value character varying(255),
    dna_library_schema_name_record_id integer,
    CONSTRAINT tripal_entity__dna_library_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__dna_library_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__dna_library_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__dna_library_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__dna_library_schema_name IS 'Data storage for tripal_entity field dna_library_schema_name.';



COMMENT ON COLUMN public.tripal_entity__dna_library_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__dna_library_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__dna_library_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__dna_library_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__dna_library_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__dna_library_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genome_assembly_data_1047 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genome_assembly_data_1047_value text,
    genome_assembly_data_1047_record_id integer,
    CONSTRAINT tripal_entity__genome_assembly_data_1047_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genome_assembly_data_1047_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genome_assembly_data_1047_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genome_assembly_data_1047 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genome_assembly_data_1047 IS 'Data storage for tripal_entity field genome_assembly_data_1047.';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_data_1047.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_data_1047.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_data_1047.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_data_1047.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_data_1047.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_data_1047.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genome_assembly_iao_0000064 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genome_assembly_iao_0000064_value character varying(255),
    genome_assembly_iao_0000064_record_id integer,
    CONSTRAINT tripal_entity__genome_assembly_iao_0000064_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genome_assembly_iao_0000064_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genome_assembly_iao_0000064_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genome_assembly_iao_0000064 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genome_assembly_iao_0000064 IS 'Data storage for tripal_entity field genome_assembly_iao_0000064.';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_iao_0000064.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_iao_0000064.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_iao_0000064.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_iao_0000064.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_iao_0000064.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_iao_0000064.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genome_assembly_iao_0000129 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genome_assembly_iao_0000129_value character varying(255),
    genome_assembly_iao_0000129_record_id integer,
    CONSTRAINT tripal_entity__genome_assembly_iao_0000129_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genome_assembly_iao_0000129_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genome_assembly_iao_0000129_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genome_assembly_iao_0000129 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genome_assembly_iao_0000129 IS 'Data storage for tripal_entity field genome_assembly_iao_0000129.';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_iao_0000129.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_iao_0000129.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_iao_0000129.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_iao_0000129.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_iao_0000129.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_iao_0000129.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genome_assembly_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genome_assembly_schema_description_value text,
    genome_assembly_schema_description_record_id integer,
    CONSTRAINT tripal_entity__genome_assembly_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genome_assembly_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genome_assembly_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genome_assembly_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genome_assembly_schema_description IS 'Data storage for tripal_entity field genome_assembly_schema_description.';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genome_assembly_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genome_assembly_schema_name_value character varying(255),
    genome_assembly_schema_name_record_id integer,
    CONSTRAINT tripal_entity__genome_assembly_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genome_assembly_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genome_assembly_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genome_assembly_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genome_assembly_schema_name IS 'Data storage for tripal_entity field genome_assembly_schema_name.';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genome_assembly_swo_0000001 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genome_assembly_swo_0000001_value character varying(255),
    genome_assembly_swo_0000001_record_id integer,
    CONSTRAINT tripal_entity__genome_assembly_swo_0000001_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genome_assembly_swo_0000001_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genome_assembly_swo_0000001_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genome_assembly_swo_0000001 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genome_assembly_swo_0000001 IS 'Data storage for tripal_entity field genome_assembly_swo_0000001.';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_swo_0000001.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_swo_0000001.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_swo_0000001.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_swo_0000001.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_swo_0000001.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genome_assembly_swo_0000001.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genome_annotation_data_1047 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genome_annotation_data_1047_value text,
    genome_annotation_data_1047_record_id integer,
    CONSTRAINT tripal_entity__genome_annotation_data_1047_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genome_annotation_data_1047_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genome_annotation_data_1047_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genome_annotation_data_1047 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genome_annotation_data_1047 IS 'Data storage for tripal_entity field genome_annotation_data_1047.';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_data_1047.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_data_1047.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_data_1047.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_data_1047.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_data_1047.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_data_1047.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genome_annotation_iao_0000064 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genome_annotation_iao_0000064_value character varying(255),
    genome_annotation_iao_0000064_record_id integer,
    CONSTRAINT tripal_entity__genome_annotation_iao_0000064_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genome_annotation_iao_0000064_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genome_annotation_iao_0000064_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genome_annotation_iao_0000064 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genome_annotation_iao_0000064 IS 'Data storage for tripal_entity field genome_annotation_iao_0000064.';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_iao_0000064.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_iao_0000064.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_iao_0000064.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_iao_0000064.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_iao_0000064.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_iao_0000064.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genome_annotation_iao_0000129 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genome_annotation_iao_0000129_value character varying(255),
    genome_annotation_iao_0000129_record_id integer,
    CONSTRAINT tripal_entity__genome_annotation_iao_0000129_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genome_annotation_iao_0000129_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genome_annotation_iao_0000129_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genome_annotation_iao_0000129 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genome_annotation_iao_0000129 IS 'Data storage for tripal_entity field genome_annotation_iao_0000129.';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_iao_0000129.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_iao_0000129.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_iao_0000129.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_iao_0000129.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_iao_0000129.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_iao_0000129.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genome_annotation_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genome_annotation_schema_description_value text,
    genome_annotation_schema_description_record_id integer,
    CONSTRAINT tripal_entity__genome_annotation_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genome_annotation_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genome_annotation_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genome_annotation_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genome_annotation_schema_description IS 'Data storage for tripal_entity field genome_annotation_schema_description.';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genome_annotation_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genome_annotation_schema_name_value character varying(255),
    genome_annotation_schema_name_record_id integer,
    CONSTRAINT tripal_entity__genome_annotation_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genome_annotation_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genome_annotation_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genome_annotation_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genome_annotation_schema_name IS 'Data storage for tripal_entity field genome_annotation_schema_name.';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genome_annotation_swo_0000001 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genome_annotation_swo_0000001_value character varying(255),
    genome_annotation_swo_0000001_record_id integer,
    CONSTRAINT tripal_entity__genome_annotation_swo_0000001_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genome_annotation_swo_0000001_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genome_annotation_swo_0000001_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genome_annotation_swo_0000001 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genome_annotation_swo_0000001 IS 'Data storage for tripal_entity field genome_annotation_swo_0000001.';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_swo_0000001.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_swo_0000001.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_swo_0000001.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_swo_0000001.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_swo_0000001.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genome_annotation_swo_0000001.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genome_project_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genome_project_schema_description_value text,
    genome_project_schema_description_record_id integer,
    CONSTRAINT tripal_entity__genome_project_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genome_project_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genome_project_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genome_project_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genome_project_schema_description IS 'Data storage for tripal_entity field genome_project_schema_description.';



COMMENT ON COLUMN public.tripal_entity__genome_project_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genome_project_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genome_project_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genome_project_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genome_project_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genome_project_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genome_project_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genome_project_schema_name_value character varying(255),
    genome_project_schema_name_record_id integer,
    CONSTRAINT tripal_entity__genome_project_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genome_project_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genome_project_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genome_project_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genome_project_schema_name IS 'Data storage for tripal_entity field genome_project_schema_name.';



COMMENT ON COLUMN public.tripal_entity__genome_project_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genome_project_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genome_project_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genome_project_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genome_project_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genome_project_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genetic_map_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genetic_map_schema_description_value text,
    genetic_map_schema_description_record_id integer,
    CONSTRAINT tripal_entity__genetic_map_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genetic_map_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genetic_map_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genetic_map_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genetic_map_schema_description IS 'Data storage for tripal_entity field genetic_map_schema_description.';



COMMENT ON COLUMN public.tripal_entity__genetic_map_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genetic_map_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genetic_map_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genetic_map_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genetic_map_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genetic_map_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genetic_map_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genetic_map_schema_name_value character varying(255),
    genetic_map_schema_name_record_id integer,
    CONSTRAINT tripal_entity__genetic_map_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genetic_map_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genetic_map_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genetic_map_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genetic_map_schema_name IS 'Data storage for tripal_entity field genetic_map_schema_name.';



COMMENT ON COLUMN public.tripal_entity__genetic_map_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genetic_map_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genetic_map_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genetic_map_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genetic_map_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genetic_map_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__QTL_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    QTL_data_0842_value text,
    QTL_data_0842_record_id integer,
    CONSTRAINT tripal_entity__QTL_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__QTL_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__QTL_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__QTL_data_0842 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__QTL_data_0842 IS 'Data storage for tripal_entity field QTL_data_0842.';



COMMENT ON COLUMN public.tripal_entity__QTL_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__QTL_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__QTL_data_0842.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__QTL_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__QTL_data_0842.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__QTL_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__QTL_data_1249 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    QTL_data_1249_value integer,
    QTL_data_1249_record_id integer,
    CONSTRAINT tripal_entity__QTL_data_1249_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__QTL_data_1249_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__QTL_data_1249_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__QTL_data_1249 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__QTL_data_1249 IS 'Data storage for tripal_entity field QTL_data_1249.';



COMMENT ON COLUMN public.tripal_entity__QTL_data_1249.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__QTL_data_1249.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__QTL_data_1249.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__QTL_data_1249.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__QTL_data_1249.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__QTL_data_1249.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__QTL_data_2044 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    QTL_data_2044_value text,
    QTL_data_2044_record_id integer,
    CONSTRAINT tripal_entity__QTL_data_2044_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__QTL_data_2044_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__QTL_data_2044_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__QTL_data_2044 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__QTL_data_2044 IS 'Data storage for tripal_entity field QTL_data_2044.';



COMMENT ON COLUMN public.tripal_entity__QTL_data_2044.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__QTL_data_2044.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__QTL_data_2044.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__QTL_data_2044.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__QTL_data_2044.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__QTL_data_2044.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__QTL_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    QTL_schema_name_value character varying(255),
    QTL_schema_name_record_id integer,
    CONSTRAINT tripal_entity__QTL_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__QTL_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__QTL_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__QTL_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__QTL_schema_name IS 'Data storage for tripal_entity field QTL_schema_name.';



COMMENT ON COLUMN public.tripal_entity__QTL_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__QTL_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__QTL_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__QTL_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__QTL_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__QTL_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__sequence_variant_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    sequence_variant_data_0842_value text,
    sequence_variant_data_0842_record_id integer,
    CONSTRAINT tripal_entity__sequence_variant_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__sequence_variant_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__sequence_variant_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__sequence_variant_data_0842 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__sequence_variant_data_0842 IS 'Data storage for tripal_entity field sequence_variant_data_0842.';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_0842.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_0842.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__sequence_variant_data_1249 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    sequence_variant_data_1249_value integer,
    sequence_variant_data_1249_record_id integer,
    CONSTRAINT tripal_entity__sequence_variant_data_1249_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__sequence_variant_data_1249_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__sequence_variant_data_1249_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__sequence_variant_data_1249 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__sequence_variant_data_1249 IS 'Data storage for tripal_entity field sequence_variant_data_1249.';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_1249.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_1249.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_1249.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_1249.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_1249.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_1249.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__sequence_variant_data_2044 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    sequence_variant_data_2044_value text,
    sequence_variant_data_2044_record_id integer,
    CONSTRAINT tripal_entity__sequence_variant_data_2044_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__sequence_variant_data_2044_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__sequence_variant_data_2044_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__sequence_variant_data_2044 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__sequence_variant_data_2044 IS 'Data storage for tripal_entity field sequence_variant_data_2044.';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_2044.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_2044.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_2044.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_2044.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_2044.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_data_2044.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__sequence_variant_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    sequence_variant_schema_name_value character varying(255),
    sequence_variant_schema_name_record_id integer,
    CONSTRAINT tripal_entity__sequence_variant_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__sequence_variant_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__sequence_variant_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__sequence_variant_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__sequence_variant_schema_name IS 'Data storage for tripal_entity field sequence_variant_schema_name.';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__sequence_variant_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genetic_marker_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genetic_marker_data_0842_value text,
    genetic_marker_data_0842_record_id integer,
    CONSTRAINT tripal_entity__genetic_marker_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genetic_marker_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genetic_marker_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genetic_marker_data_0842 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genetic_marker_data_0842 IS 'Data storage for tripal_entity field genetic_marker_data_0842.';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_0842.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_0842.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genetic_marker_data_1249 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genetic_marker_data_1249_value integer,
    genetic_marker_data_1249_record_id integer,
    CONSTRAINT tripal_entity__genetic_marker_data_1249_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genetic_marker_data_1249_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genetic_marker_data_1249_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genetic_marker_data_1249 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genetic_marker_data_1249 IS 'Data storage for tripal_entity field genetic_marker_data_1249.';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_1249.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_1249.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_1249.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_1249.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_1249.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_1249.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genetic_marker_data_2044 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genetic_marker_data_2044_value text,
    genetic_marker_data_2044_record_id integer,
    CONSTRAINT tripal_entity__genetic_marker_data_2044_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genetic_marker_data_2044_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genetic_marker_data_2044_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genetic_marker_data_2044 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genetic_marker_data_2044 IS 'Data storage for tripal_entity field genetic_marker_data_2044.';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_2044.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_2044.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_2044.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_2044.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_2044.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_data_2044.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__genetic_marker_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    genetic_marker_schema_name_value character varying(255),
    genetic_marker_schema_name_record_id integer,
    CONSTRAINT tripal_entity__genetic_marker_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__genetic_marker_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__genetic_marker_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__genetic_marker_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__genetic_marker_schema_name IS 'Data storage for tripal_entity field genetic_marker_schema_name.';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__genetic_marker_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__organism_local_abbreviation (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    organism_local_abbreviation_value character varying(255),
    organism_local_abbreviation_record_id integer,
    CONSTRAINT tripal_entity__organism_local_abbreviation_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__organism_local_abbreviation_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__organism_local_abbreviation_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__organism_local_abbreviation OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__organism_local_abbreviation IS 'Data storage for tripal_entity field organism_local_abbreviation.';



COMMENT ON COLUMN public.tripal_entity__organism_local_abbreviation.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__organism_local_abbreviation.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__organism_local_abbreviation.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__organism_local_abbreviation.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__organism_local_abbreviation.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__organism_local_abbreviation.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__organism_ncbitaxon_common_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    organism_ncbitaxon_common_name_value character varying(255),
    organism_ncbitaxon_common_name_record_id integer,
    CONSTRAINT tripal_entity__organism_ncbitaxon_common_na_revision_id_check CHECK ((revision_id >= 0)),
    CONSTRAINT tripal_entity__organism_ncbitaxon_common_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__organism_ncbitaxon_common_name_entity_id_check CHECK ((entity_id >= 0))
);


ALTER TABLE public.tripal_entity__organism_ncbitaxon_common_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__organism_ncbitaxon_common_name IS 'Data storage for tripal_entity field organism_ncbitaxon_common_name.';



COMMENT ON COLUMN public.tripal_entity__organism_ncbitaxon_common_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__organism_ncbitaxon_common_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__organism_ncbitaxon_common_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__organism_ncbitaxon_common_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__organism_ncbitaxon_common_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__organism_ncbitaxon_common_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__organism_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    organism_schema_description_value text,
    organism_schema_description_record_id integer,
    CONSTRAINT tripal_entity__organism_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__organism_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__organism_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__organism_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__organism_schema_description IS 'Data storage for tripal_entity field organism_schema_description.';



COMMENT ON COLUMN public.tripal_entity__organism_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__organism_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__organism_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__organism_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__organism_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__organism_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__organism_taxrank_0000005 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    organism_taxrank_0000005_value character varying(255),
    organism_taxrank_0000005_record_id integer,
    CONSTRAINT tripal_entity__organism_taxrank_0000005_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__organism_taxrank_0000005_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__organism_taxrank_0000005_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__organism_taxrank_0000005 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__organism_taxrank_0000005 IS 'Data storage for tripal_entity field organism_taxrank_0000005.';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000005.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000005.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000005.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000005.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000005.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000005.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__organism_taxrank_0000006 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    organism_taxrank_0000006_value character varying(255),
    organism_taxrank_0000006_record_id integer,
    CONSTRAINT tripal_entity__organism_taxrank_0000006_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__organism_taxrank_0000006_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__organism_taxrank_0000006_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__organism_taxrank_0000006 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__organism_taxrank_0000006 IS 'Data storage for tripal_entity field organism_taxrank_0000006.';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000006.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000006.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000006.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000006.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000006.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000006.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__organism_taxrank_0000045 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    organism_taxrank_0000045_value character varying(1024),
    organism_taxrank_0000045_record_id integer,
    CONSTRAINT tripal_entity__organism_taxrank_0000045_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__organism_taxrank_0000045_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__organism_taxrank_0000045_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__organism_taxrank_0000045 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__organism_taxrank_0000045 IS 'Data storage for tripal_entity field organism_taxrank_0000045.';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000045.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000045.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000045.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000045.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000045.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__organism_taxrank_0000045.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__phenotypic_marker_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    phenotypic_marker_data_0842_value text,
    phenotypic_marker_data_0842_record_id integer,
    CONSTRAINT tripal_entity__phenotypic_marker_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__phenotypic_marker_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__phenotypic_marker_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__phenotypic_marker_data_0842 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__phenotypic_marker_data_0842 IS 'Data storage for tripal_entity field phenotypic_marker_data_0842.';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_0842.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_0842.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__phenotypic_marker_data_1249 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    phenotypic_marker_data_1249_value integer,
    phenotypic_marker_data_1249_record_id integer,
    CONSTRAINT tripal_entity__phenotypic_marker_data_1249_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__phenotypic_marker_data_1249_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__phenotypic_marker_data_1249_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__phenotypic_marker_data_1249 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__phenotypic_marker_data_1249 IS 'Data storage for tripal_entity field phenotypic_marker_data_1249.';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_1249.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_1249.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_1249.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_1249.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_1249.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_1249.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__phenotypic_marker_data_2044 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    phenotypic_marker_data_2044_value text,
    phenotypic_marker_data_2044_record_id integer,
    CONSTRAINT tripal_entity__phenotypic_marker_data_2044_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__phenotypic_marker_data_2044_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__phenotypic_marker_data_2044_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__phenotypic_marker_data_2044 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__phenotypic_marker_data_2044 IS 'Data storage for tripal_entity field phenotypic_marker_data_2044.';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_2044.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_2044.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_2044.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_2044.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_2044.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_data_2044.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__phenotypic_marker_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    phenotypic_marker_schema_name_value character varying(255),
    phenotypic_marker_schema_name_record_id integer,
    CONSTRAINT tripal_entity__phenotypic_marker_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__phenotypic_marker_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__phenotypic_marker_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__phenotypic_marker_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__phenotypic_marker_schema_name IS 'Data storage for tripal_entity field phenotypic_marker_schema_name.';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__phenotypic_marker_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__germplasm_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    germplasm_data_0842_value text,
    germplasm_data_0842_record_id integer,
    CONSTRAINT tripal_entity__germplasm_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__germplasm_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__germplasm_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__germplasm_data_0842 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__germplasm_data_0842 IS 'Data storage for tripal_entity field germplasm_data_0842.';



COMMENT ON COLUMN public.tripal_entity__germplasm_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__germplasm_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__germplasm_data_0842.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__germplasm_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__germplasm_data_0842.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__germplasm_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__germplasm_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    germplasm_schema_description_value text,
    germplasm_schema_description_record_id integer,
    CONSTRAINT tripal_entity__germplasm_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__germplasm_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__germplasm_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__germplasm_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__germplasm_schema_description IS 'Data storage for tripal_entity field germplasm_schema_description.';



COMMENT ON COLUMN public.tripal_entity__germplasm_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__germplasm_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__germplasm_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__germplasm_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__germplasm_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__germplasm_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__germplasm_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    germplasm_schema_name_value character varying(255),
    germplasm_schema_name_record_id integer,
    CONSTRAINT tripal_entity__germplasm_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__germplasm_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__germplasm_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__germplasm_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__germplasm_schema_name IS 'Data storage for tripal_entity field germplasm_schema_name.';



COMMENT ON COLUMN public.tripal_entity__germplasm_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__germplasm_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__germplasm_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__germplasm_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__germplasm_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__germplasm_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__breeding_cross_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    breeding_cross_data_0842_value text,
    breeding_cross_data_0842_record_id integer,
    CONSTRAINT tripal_entity__breeding_cross_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__breeding_cross_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__breeding_cross_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__breeding_cross_data_0842 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__breeding_cross_data_0842 IS 'Data storage for tripal_entity field breeding_cross_data_0842.';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_data_0842.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_data_0842.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__breeding_cross_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    breeding_cross_schema_description_value text,
    breeding_cross_schema_description_record_id integer,
    CONSTRAINT tripal_entity__breeding_cross_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__breeding_cross_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__breeding_cross_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__breeding_cross_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__breeding_cross_schema_description IS 'Data storage for tripal_entity field breeding_cross_schema_description.';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__breeding_cross_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    breeding_cross_schema_name_value character varying(255),
    breeding_cross_schema_name_record_id integer,
    CONSTRAINT tripal_entity__breeding_cross_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__breeding_cross_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__breeding_cross_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__breeding_cross_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__breeding_cross_schema_name IS 'Data storage for tripal_entity field breeding_cross_schema_name.';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__breeding_cross_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__germplasm_variety_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    germplasm_variety_data_0842_value text,
    germplasm_variety_data_0842_record_id integer,
    CONSTRAINT tripal_entity__germplasm_variety_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__germplasm_variety_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__germplasm_variety_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__germplasm_variety_data_0842 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__germplasm_variety_data_0842 IS 'Data storage for tripal_entity field germplasm_variety_data_0842.';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_data_0842.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_data_0842.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__germplasm_variety_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    germplasm_variety_schema_description_value text,
    germplasm_variety_schema_description_record_id integer,
    CONSTRAINT tripal_entity__germplasm_variety_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__germplasm_variety_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__germplasm_variety_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__germplasm_variety_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__germplasm_variety_schema_description IS 'Data storage for tripal_entity field germplasm_variety_schema_description.';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__germplasm_variety_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    germplasm_variety_schema_name_value character varying(255),
    germplasm_variety_schema_name_record_id integer,
    CONSTRAINT tripal_entity__germplasm_variety_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__germplasm_variety_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__germplasm_variety_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__germplasm_variety_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__germplasm_variety_schema_name IS 'Data storage for tripal_entity field germplasm_variety_schema_name.';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__germplasm_variety_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__RIL_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    RIL_data_0842_value text,
    RIL_data_0842_record_id integer,
    CONSTRAINT tripal_entity__RIL_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__RIL_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__RIL_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__RIL_data_0842 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__RIL_data_0842 IS 'Data storage for tripal_entity field RIL_data_0842.';



COMMENT ON COLUMN public.tripal_entity__RIL_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__RIL_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__RIL_data_0842.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__RIL_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__RIL_data_0842.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__RIL_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__RIL_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    RIL_schema_description_value text,
    RIL_schema_description_record_id integer,
    CONSTRAINT tripal_entity__RIL_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__RIL_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__RIL_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__RIL_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__RIL_schema_description IS 'Data storage for tripal_entity field RIL_schema_description.';



COMMENT ON COLUMN public.tripal_entity__RIL_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__RIL_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__RIL_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__RIL_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__RIL_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__RIL_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__RIL_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    RIL_schema_name_value character varying(255),
    RIL_schema_name_record_id integer,
    CONSTRAINT tripal_entity__RIL_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__RIL_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__RIL_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__RIL_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__RIL_schema_name IS 'Data storage for tripal_entity field RIL_schema_name.';



COMMENT ON COLUMN public.tripal_entity__RIL_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__RIL_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__RIL_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__RIL_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__RIL_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__RIL_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__biosample_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    biosample_schema_description_value text,
    biosample_schema_description_record_id integer,
    CONSTRAINT tripal_entity__biosample_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__biosample_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__biosample_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__biosample_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__biosample_schema_description IS 'Data storage for tripal_entity field biosample_schema_description.';



COMMENT ON COLUMN public.tripal_entity__biosample_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__biosample_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__biosample_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__biosample_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__biosample_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__biosample_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__biosample_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    biosample_schema_name_value text,
    biosample_schema_name_record_id integer,
    CONSTRAINT tripal_entity__biosample_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__biosample_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__biosample_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__biosample_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__biosample_schema_name IS 'Data storage for tripal_entity field biosample_schema_name.';



COMMENT ON COLUMN public.tripal_entity__biosample_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__biosample_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__biosample_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__biosample_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__biosample_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__biosample_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__array_design_iao_0000129 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    array_design_iao_0000129_value text,
    array_design_iao_0000129_record_id integer,
    CONSTRAINT tripal_entity__array_design_iao_0000129_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__array_design_iao_0000129_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__array_design_iao_0000129_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__array_design_iao_0000129 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__array_design_iao_0000129 IS 'Data storage for tripal_entity field array_design_iao_0000129.';



COMMENT ON COLUMN public.tripal_entity__array_design_iao_0000129.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__array_design_iao_0000129.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__array_design_iao_0000129.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__array_design_iao_0000129.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__array_design_iao_0000129.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__array_design_iao_0000129.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__array_design_local_array_dimensio (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    array_design_local_array_dimensio_value text,
    array_design_local_array_dimensio_record_id integer,
    CONSTRAINT tripal_entity__array_design_local_array_dimens_revision_id_check CHECK ((revision_id >= 0)),
    CONSTRAINT tripal_entity__array_design_local_array_dimensio_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__array_design_local_array_dimensio_entity_id_check CHECK ((entity_id >= 0))
);


ALTER TABLE public.tripal_entity__array_design_local_array_dimensio OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__array_design_local_array_dimensio IS 'Data storage for tripal_entity field array_design_local_array_dimensio.';



COMMENT ON COLUMN public.tripal_entity__array_design_local_array_dimensio.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__array_design_local_array_dimensio.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__array_design_local_array_dimensio.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__array_design_local_array_dimensio.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__array_design_local_array_dimensio.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__array_design_local_array_dimensio.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__array_design_local_element_dimens (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    array_design_local_element_dimens_value text,
    array_design_local_element_dimens_record_id integer,
    CONSTRAINT tripal_entity__array_design_local_element_dime_revision_id_check CHECK ((revision_id >= 0)),
    CONSTRAINT tripal_entity__array_design_local_element_dimens_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__array_design_local_element_dimens_entity_id_check CHECK ((entity_id >= 0))
);


ALTER TABLE public.tripal_entity__array_design_local_element_dimens OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__array_design_local_element_dimens IS 'Data storage for tripal_entity field array_design_local_element_dimens.';



COMMENT ON COLUMN public.tripal_entity__array_design_local_element_dimens.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__array_design_local_element_dimens.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__array_design_local_element_dimens.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__array_design_local_element_dimens.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__array_design_local_element_dimens.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__array_design_local_element_dimens.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__array_design_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    array_design_schema_description_value text,
    array_design_schema_description_record_id integer,
    CONSTRAINT tripal_entity__array_design_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__array_design_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__array_design_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__array_design_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__array_design_schema_description IS 'Data storage for tripal_entity field array_design_schema_description.';



COMMENT ON COLUMN public.tripal_entity__array_design_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__array_design_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__array_design_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__array_design_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__array_design_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__array_design_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__array_design_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    array_design_schema_name_value text,
    array_design_schema_name_record_id integer,
    CONSTRAINT tripal_entity__array_design_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__array_design_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__array_design_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__array_design_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__array_design_schema_name IS 'Data storage for tripal_entity field array_design_schema_name.';



COMMENT ON COLUMN public.tripal_entity__array_design_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__array_design_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__array_design_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__array_design_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__array_design_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__array_design_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__analysis_data_1047 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    analysis_data_1047_value text,
    analysis_data_1047_record_id integer,
    CONSTRAINT tripal_entity__analysis_data_1047_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__analysis_data_1047_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__analysis_data_1047_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__analysis_data_1047 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__analysis_data_1047 IS 'Data storage for tripal_entity field analysis_data_1047.';



COMMENT ON COLUMN public.tripal_entity__analysis_data_1047.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__analysis_data_1047.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__analysis_data_1047.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__analysis_data_1047.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__analysis_data_1047.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__analysis_data_1047.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__analysis_iao_0000064 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    analysis_iao_0000064_value character varying(255),
    analysis_iao_0000064_record_id integer,
    CONSTRAINT tripal_entity__analysis_iao_0000064_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__analysis_iao_0000064_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__analysis_iao_0000064_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__analysis_iao_0000064 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__analysis_iao_0000064 IS 'Data storage for tripal_entity field analysis_iao_0000064.';



COMMENT ON COLUMN public.tripal_entity__analysis_iao_0000064.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__analysis_iao_0000064.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__analysis_iao_0000064.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__analysis_iao_0000064.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__analysis_iao_0000064.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__analysis_iao_0000064.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__analysis_iao_0000129 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    analysis_iao_0000129_value character varying(255),
    analysis_iao_0000129_record_id integer,
    CONSTRAINT tripal_entity__analysis_iao_0000129_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__analysis_iao_0000129_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__analysis_iao_0000129_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__analysis_iao_0000129 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__analysis_iao_0000129 IS 'Data storage for tripal_entity field analysis_iao_0000129.';



COMMENT ON COLUMN public.tripal_entity__analysis_iao_0000129.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__analysis_iao_0000129.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__analysis_iao_0000129.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__analysis_iao_0000129.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__analysis_iao_0000129.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__analysis_iao_0000129.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__analysis_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    analysis_schema_description_value text,
    analysis_schema_description_record_id integer,
    CONSTRAINT tripal_entity__analysis_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__analysis_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__analysis_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__analysis_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__analysis_schema_description IS 'Data storage for tripal_entity field analysis_schema_description.';



COMMENT ON COLUMN public.tripal_entity__analysis_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__analysis_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__analysis_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__analysis_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__analysis_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__analysis_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__analysis_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    analysis_schema_name_value character varying(255),
    analysis_schema_name_record_id integer,
    CONSTRAINT tripal_entity__analysis_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__analysis_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__analysis_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__analysis_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__analysis_schema_name IS 'Data storage for tripal_entity field analysis_schema_name.';



COMMENT ON COLUMN public.tripal_entity__analysis_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__analysis_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__analysis_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__analysis_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__analysis_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__analysis_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__analysis_swo_0000001 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    analysis_swo_0000001_value character varying(255),
    analysis_swo_0000001_record_id integer,
    CONSTRAINT tripal_entity__analysis_swo_0000001_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__analysis_swo_0000001_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__analysis_swo_0000001_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__analysis_swo_0000001 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__analysis_swo_0000001 IS 'Data storage for tripal_entity field analysis_swo_0000001.';



COMMENT ON COLUMN public.tripal_entity__analysis_swo_0000001.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__analysis_swo_0000001.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__analysis_swo_0000001.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__analysis_swo_0000001.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__analysis_swo_0000001.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__analysis_swo_0000001.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__project_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    project_schema_description_value text,
    project_schema_description_record_id integer,
    CONSTRAINT tripal_entity__project_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__project_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__project_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__project_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__project_schema_description IS 'Data storage for tripal_entity field project_schema_description.';



COMMENT ON COLUMN public.tripal_entity__project_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__project_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__project_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__project_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__project_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__project_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__project_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    project_schema_name_value character varying(255),
    project_schema_name_record_id integer,
    CONSTRAINT tripal_entity__project_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__project_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__project_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__project_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__project_schema_name IS 'Data storage for tripal_entity field project_schema_name.';



COMMENT ON COLUMN public.tripal_entity__project_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__project_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__project_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__project_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__project_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__project_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__study_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    study_schema_description_value text,
    study_schema_description_record_id integer,
    CONSTRAINT tripal_entity__study_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__study_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__study_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__study_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__study_schema_description IS 'Data storage for tripal_entity field study_schema_description.';



COMMENT ON COLUMN public.tripal_entity__study_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__study_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__study_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__study_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__study_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__study_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__study_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    study_schema_name_value text,
    study_schema_name_record_id integer,
    CONSTRAINT tripal_entity__study_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__study_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__study_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__study_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__study_schema_name IS 'Data storage for tripal_entity field study_schema_name.';



COMMENT ON COLUMN public.tripal_entity__study_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__study_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__study_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__study_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__study_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__study_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__contact_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    contact_schema_description_value character varying(255),
    contact_schema_description_record_id integer,
    CONSTRAINT tripal_entity__contact_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__contact_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__contact_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__contact_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__contact_schema_description IS 'Data storage for tripal_entity field contact_schema_description.';



COMMENT ON COLUMN public.tripal_entity__contact_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__contact_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__contact_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__contact_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__contact_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__contact_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__contact_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    contact_schema_name_value character varying(255),
    contact_schema_name_record_id integer,
    CONSTRAINT tripal_entity__contact_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__contact_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__contact_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__contact_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__contact_schema_name IS 'Data storage for tripal_entity field contact_schema_name.';



COMMENT ON COLUMN public.tripal_entity__contact_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__contact_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__contact_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__contact_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__contact_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__contact_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__protocol_data_1047 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    protocol_data_1047_value text,
    protocol_data_1047_record_id integer,
    CONSTRAINT tripal_entity__protocol_data_1047_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__protocol_data_1047_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__protocol_data_1047_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__protocol_data_1047 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__protocol_data_1047 IS 'Data storage for tripal_entity field protocol_data_1047.';



COMMENT ON COLUMN public.tripal_entity__protocol_data_1047.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__protocol_data_1047.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__protocol_data_1047.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__protocol_data_1047.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__protocol_data_1047.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__protocol_data_1047.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__protocol_efo_0000548 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    protocol_efo_0000548_value text,
    protocol_efo_0000548_record_id integer,
    CONSTRAINT tripal_entity__protocol_efo_0000548_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__protocol_efo_0000548_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__protocol_efo_0000548_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__protocol_efo_0000548 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__protocol_efo_0000548 IS 'Data storage for tripal_entity field protocol_efo_0000548.';



COMMENT ON COLUMN public.tripal_entity__protocol_efo_0000548.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__protocol_efo_0000548.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__protocol_efo_0000548.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__protocol_efo_0000548.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__protocol_efo_0000548.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__protocol_efo_0000548.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__protocol_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    protocol_schema_description_value text,
    protocol_schema_description_record_id integer,
    CONSTRAINT tripal_entity__protocol_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__protocol_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__protocol_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__protocol_schema_description OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__protocol_schema_description IS 'Data storage for tripal_entity field protocol_schema_description.';



COMMENT ON COLUMN public.tripal_entity__protocol_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__protocol_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__protocol_schema_description.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__protocol_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__protocol_schema_description.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__protocol_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__protocol_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    protocol_schema_name_value text,
    protocol_schema_name_record_id integer,
    CONSTRAINT tripal_entity__protocol_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__protocol_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__protocol_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__protocol_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__protocol_schema_name IS 'Data storage for tripal_entity field protocol_schema_name.';



COMMENT ON COLUMN public.tripal_entity__protocol_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__protocol_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__protocol_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__protocol_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__protocol_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__protocol_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__protocol_swo_0000001 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    protocol_swo_0000001_value text,
    protocol_swo_0000001_record_id integer,
    CONSTRAINT tripal_entity__protocol_swo_0000001_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__protocol_swo_0000001_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__protocol_swo_0000001_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__protocol_swo_0000001 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__protocol_swo_0000001 IS 'Data storage for tripal_entity field protocol_swo_0000001.';



COMMENT ON COLUMN public.tripal_entity__protocol_swo_0000001.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__protocol_swo_0000001.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__protocol_swo_0000001.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__protocol_swo_0000001.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__protocol_swo_0000001.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__protocol_swo_0000001.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__gene_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    gene_data_0842_value text,
    gene_data_0842_record_id integer,
    CONSTRAINT tripal_entity__gene_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__gene_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__gene_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__gene_data_0842 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__gene_data_0842 IS 'Data storage for tripal_entity field gene_data_0842.';



COMMENT ON COLUMN public.tripal_entity__gene_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__gene_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__gene_data_0842.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__gene_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__gene_data_0842.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__gene_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__gene_data_1249 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    gene_data_1249_value integer,
    gene_data_1249_record_id integer,
    CONSTRAINT tripal_entity__gene_data_1249_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__gene_data_1249_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__gene_data_1249_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__gene_data_1249 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__gene_data_1249 IS 'Data storage for tripal_entity field gene_data_1249.';



COMMENT ON COLUMN public.tripal_entity__gene_data_1249.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__gene_data_1249.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__gene_data_1249.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__gene_data_1249.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__gene_data_1249.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__gene_data_1249.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__gene_data_2044 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    gene_data_2044_value text,
    gene_data_2044_record_id integer,
    CONSTRAINT tripal_entity__gene_data_2044_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__gene_data_2044_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__gene_data_2044_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__gene_data_2044 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__gene_data_2044 IS 'Data storage for tripal_entity field gene_data_2044.';



COMMENT ON COLUMN public.tripal_entity__gene_data_2044.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__gene_data_2044.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__gene_data_2044.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__gene_data_2044.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__gene_data_2044.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__gene_data_2044.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__gene_obi_0100026 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    gene_obi_0100026_value integer,
    gene_obi_0100026_rdfs_label character varying(2558),
    "gene_obi_0100026_TAXRANK_0000005" character varying(255),
    "gene_obi_0100026_TAXRANK_0000006" character varying(255),
    "gene_obi_0100026_TAXRANK_0000045" character varying(1024),
    gene_obi_0100026_local_infraspecific_type integer,
    gene_obi_0100026_record_id integer,
    CONSTRAINT tripal_entity__gene_obi_0100026_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__gene_obi_0100026_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__gene_obi_0100026_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__gene_obi_0100026 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__gene_obi_0100026 IS 'Data storage for tripal_entity field gene_obi_0100026.';



COMMENT ON COLUMN public.tripal_entity__gene_obi_0100026.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__gene_obi_0100026.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__gene_obi_0100026.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__gene_obi_0100026.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__gene_obi_0100026.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__gene_obi_0100026.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__gene_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    gene_schema_name_value character varying(255),
    gene_schema_name_record_id integer,
    CONSTRAINT tripal_entity__gene_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__gene_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__gene_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__gene_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__gene_schema_name IS 'Data storage for tripal_entity field gene_schema_name.';



COMMENT ON COLUMN public.tripal_entity__gene_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__gene_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__gene_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__gene_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__gene_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__gene_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__mRNA_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    mRNA_data_0842_value text,
    mRNA_data_0842_record_id integer,
    CONSTRAINT tripal_entity__mRNA_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__mRNA_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__mRNA_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__mRNA_data_0842 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__mRNA_data_0842 IS 'Data storage for tripal_entity field mRNA_data_0842.';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_0842.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_0842.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__mRNA_data_1249 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    mRNA_data_1249_value integer,
    mRNA_data_1249_record_id integer,
    CONSTRAINT tripal_entity__mRNA_data_1249_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__mRNA_data_1249_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__mRNA_data_1249_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__mRNA_data_1249 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__mRNA_data_1249 IS 'Data storage for tripal_entity field mRNA_data_1249.';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_1249.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_1249.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_1249.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_1249.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_1249.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_1249.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__mRNA_data_2044 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    mRNA_data_2044_value text,
    mRNA_data_2044_record_id integer,
    CONSTRAINT tripal_entity__mRNA_data_2044_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__mRNA_data_2044_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__mRNA_data_2044_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__mRNA_data_2044 OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__mRNA_data_2044 IS 'Data storage for tripal_entity field mRNA_data_2044.';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_2044.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_2044.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_2044.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_2044.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_2044.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__mRNA_data_2044.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__mRNA_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    mRNA_schema_name_value character varying(255),
    mRNA_schema_name_record_id integer,
    CONSTRAINT tripal_entity__mRNA_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__mRNA_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__mRNA_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__mRNA_schema_name OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__mRNA_schema_name IS 'Data storage for tripal_entity field mRNA_schema_name.';



COMMENT ON COLUMN public.tripal_entity__mRNA_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__mRNA_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__mRNA_schema_name.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__mRNA_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__mRNA_schema_name.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__mRNA_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE TABLE public.tripal_entity__chado_additional_type_default (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    chado_additional_type_default_value integer,
    chado_additional_type_default_record_id integer,
    CONSTRAINT tripal_entity__chado_additional_type_default_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__chado_additional_type_default_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__chado_additional_type_default_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__chado_additional_type_default OWNER TO drupal;


COMMENT ON TABLE public.tripal_entity__chado_additional_type_default IS 'Data storage for tripal_entity field chado_additional_type_default.';



COMMENT ON COLUMN public.tripal_entity__chado_additional_type_default.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';



COMMENT ON COLUMN public.tripal_entity__chado_additional_type_default.deleted IS 'A boolean indicating whether this data item has been deleted';



COMMENT ON COLUMN public.tripal_entity__chado_additional_type_default.entity_id IS 'The entity id this data is attached to';



COMMENT ON COLUMN public.tripal_entity__chado_additional_type_default.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';



COMMENT ON COLUMN public.tripal_entity__chado_additional_type_default.langcode IS 'The language code for this data item.';



COMMENT ON COLUMN public.tripal_entity__chado_additional_type_default.delta IS 'The sequence number for this data item, used for multi-value fields';



CREATE SEQUENCE public.tripal_entity_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_entity_id_seq OWNER TO drupal;


ALTER SEQUENCE public.tripal_entity_id_seq OWNED BY public.tripal_entity.id;



CREATE TABLE public.tripal_expiration_files (
    fid integer NOT NULL,
    expiration_date bigint NOT NULL
);


ALTER TABLE public.tripal_expiration_files OWNER TO drupal;


CREATE TABLE public.tripal_id_space_collection (
    name character varying(255) NOT NULL,
    plugin_id character varying(255) NOT NULL
);


ALTER TABLE public.tripal_id_space_collection OWNER TO drupal;


CREATE TABLE public.tripal_import (
    import_id integer NOT NULL,
    uid bigint NOT NULL,
    class character varying(256) NOT NULL,
    fid text,
    arguments text,
    submit_date integer NOT NULL,
    CONSTRAINT tripal_import_import_id_check CHECK ((import_id >= 0)),
    CONSTRAINT tripal_import_uid_check CHECK ((uid >= 0))
);


ALTER TABLE public.tripal_import OWNER TO drupal;


COMMENT ON COLUMN public.tripal_import.uid IS 'The Drupal userid of the submitee.';



COMMENT ON COLUMN public.tripal_import.fid IS 'The file IDs of the to import. This only applies if the file was uploaded (i.e. not already on the server) and is mangaged by Drupal. Multiple fids are separated using a | character.';



COMMENT ON COLUMN public.tripal_import.arguments IS 'Holds a serialized PHP array containing the key/value paris that are used for arguments of the job.';



COMMENT ON COLUMN public.tripal_import.submit_date IS 'UNIX integer submit time';



CREATE SEQUENCE public.tripal_import_import_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_import_import_id_seq OWNER TO drupal;


ALTER SEQUENCE public.tripal_import_import_id_seq OWNED BY public.tripal_import.import_id;



CREATE TABLE IF NOT EXISTS public.tripal_pub_library_query
(
    pub_library_query_id integer NOT NULL DEFAULT nextval('tripal_pub_library_query_pub_library_query_id_seq'::regclass),
    name character varying(255) COLLATE pg_catalog."default" NOT NULL,
    criteria text COLLATE pg_catalog."default" NOT NULL,
    disabled bigint DEFAULT 0,
    do_contact bigint DEFAULT 0,
    CONSTRAINT tripal_pub_library_query____pkey PRIMARY KEY (pub_library_query_id),
    CONSTRAINT tripal_pub_library_query_disabled_check CHECK (disabled >= 0),
    CONSTRAINT tripal_pub_library_query_do_contact_check CHECK (do_contact >= 0)
)

TABLESPACE pg_default;

ALTER TABLE IF EXISTS public.tripal_pub_library_query
    OWNER to drupal;

COMMENT ON COLUMN public.tripal_pub_library_query.criteria
    IS 'Contains a serialized PHP array containing the search criteria';
-- Index: tripal_pub_library_query__name__idx

-- DROP INDEX IF EXISTS public.tripal_pub_library_query__name__idx;

CREATE INDEX IF NOT EXISTS tripal_pub_library_query__name__idx
    ON public.tripal_pub_library_query USING btree
    (name COLLATE pg_catalog."default" ASC NULLS LAST);


CREATE TABLE public.tripal_jobs (
    job_id integer NOT NULL,
    uid bigint NOT NULL,
    job_name character varying(255) NOT NULL,
    modulename character varying(50) NOT NULL,
    callback character varying(255) NOT NULL,
    arguments text,
    progress bigint DEFAULT 0,
    status character varying(50) NOT NULL,
    submit_date integer NOT NULL,
    start_time integer,
    end_time integer,
    error_msg text,
    pid bigint,
    priority bigint DEFAULT '0'::bigint NOT NULL,
    mlock bigint,
    lock bigint,
    includes text,
    CONSTRAINT tripal_jobs_job_id_check CHECK ((job_id >= 0)),
    CONSTRAINT tripal_jobs_lock_check CHECK ((lock >= 0)),
    CONSTRAINT tripal_jobs_mlock_check CHECK ((mlock >= 0)),
    CONSTRAINT tripal_jobs_pid_check CHECK ((pid >= 0)),
    CONSTRAINT tripal_jobs_priority_check CHECK ((priority >= 0)),
    CONSTRAINT tripal_jobs_progress_check CHECK ((progress >= 0)),
    CONSTRAINT tripal_jobs_uid_check CHECK ((uid >= 0))
);


ALTER TABLE public.tripal_jobs OWNER TO drupal;


COMMENT ON COLUMN public.tripal_jobs.uid IS 'The Drupal userid of the submitee';



COMMENT ON COLUMN public.tripal_jobs.modulename IS 'The module name that provides the callback for this job';



COMMENT ON COLUMN public.tripal_jobs.progress IS 'a value from 0 to 100 indicating percent complete';



COMMENT ON COLUMN public.tripal_jobs.submit_date IS 'UNIX integer submit time';



COMMENT ON COLUMN public.tripal_jobs.start_time IS 'UNIX integer start time';



COMMENT ON COLUMN public.tripal_jobs.end_time IS 'UNIX integer end time';



COMMENT ON COLUMN public.tripal_jobs.pid IS 'The process id for the job';



COMMENT ON COLUMN public.tripal_jobs.priority IS 'The job priority';



COMMENT ON COLUMN public.tripal_jobs.mlock IS 'If set to 1 then all jobs for the module are held until this one finishes';



COMMENT ON COLUMN public.tripal_jobs.lock IS 'If set to 1 then all jobs are held until this one finishes';



COMMENT ON COLUMN public.tripal_jobs.includes IS 'A serialized array of file paths that should be included prior to executing the job.';



CREATE SEQUENCE public.tripal_jobs_job_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_jobs_job_id_seq OWNER TO drupal;


ALTER SEQUENCE public.tripal_jobs_job_id_seq OWNED BY public.tripal_jobs.job_id;



CREATE TABLE public.tripal_mviews (
    mview_id integer NOT NULL,
    table_id integer NOT NULL,
    name character varying(255) NOT NULL,
    query text NOT NULL,
    last_update integer,
    status text,
    comment text,
    CONSTRAINT tripal_mviews_mview_id_check CHECK ((mview_id >= 0))
);


ALTER TABLE public.tripal_mviews OWNER TO drupal;


COMMENT ON COLUMN public.tripal_mviews.table_id IS 'The custom table ID';



COMMENT ON COLUMN public.tripal_mviews.last_update IS 'UNIX integer time';



CREATE SEQUENCE public.tripal_mviews_mview_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_mviews_mview_id_seq OWNER TO drupal;


ALTER SEQUENCE public.tripal_mviews_mview_id_seq OWNED BY public.tripal_mviews.mview_id;



CREATE TABLE public.tripal_token_formats (
    tripal_format_id integer NOT NULL,
    content_type character varying(255) NOT NULL,
    application character varying(255) NOT NULL,
    format text NOT NULL,
    tokens text NOT NULL,
    CONSTRAINT tripal_token_formats_tripal_format_id_check CHECK ((tripal_format_id >= 0))
);


ALTER TABLE public.tripal_token_formats OWNER TO drupal;



CREATE SEQUENCE public.tripal_token_formats_tripal_format_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_token_formats_tripal_format_id_seq OWNER TO drupal;


ALTER SEQUENCE public.tripal_token_formats_tripal_format_id_seq OWNED BY public.tripal_token_formats.tripal_format_id;


CREATE TABLE public.tripal_variables (
    variable_id integer NOT NULL,
    name character varying(255) NOT NULL,
    description text NOT NULL
);


ALTER TABLE public.tripal_variables OWNER TO drupal;


COMMENT ON TABLE public.tripal_variables IS 'This table houses a list of unique variable names that can be used in the tripal_node_variables table.';


CREATE SEQUENCE public.tripal_variables_variable_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_variables_variable_id_seq OWNER TO drupal;


ALTER SEQUENCE public.tripal_variables_variable_id_seq OWNED BY public.tripal_variables.variable_id;


CREATE TABLE public.tripal_vocabulary_collection (
    name character varying(255) NOT NULL,
    plugin_id character varying(255) NOT NULL
);


ALTER TABLE public.tripal_vocabulary_collection OWNER TO drupal;


INSERT INTO public.tripal_custom_tables VALUES (1, 'tripal_gff_temp', 'a:4:{s:5:"table";s:15:"tripal_gff_temp";s:6:"fields";a:4:{s:10:"feature_id";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:11:"organism_id";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:10:"uniquename";a:2:{s:4:"type";s:4:"text";s:8:"not null";b:1;}s:9:"type_name";a:3:{s:4:"type";s:7:"varchar";s:6:"length";s:4:"1024";s:8:"not null";b:1;}}s:7:"indexes";a:2:{s:20:"tripal_gff_temp_idx0";a:1:{i:0;s:11:"organism_id";}s:20:"tripal_gff_temp_idx1";a:1:{i:0;s:10:"uniquename";}}s:11:"unique keys";a:2:{s:19:"tripal_gff_temp_uq0";a:1:{i:0;s:10:"feature_id";}s:19:"tripal_gff_temp_uq1";a:3:{i:0;s:10:"uniquename";i:1;s:11:"organism_id";i:2;s:9:"type_name";}}}', 1, 'chado');
INSERT INTO public.tripal_custom_tables VALUES (8, 'analysis_organism', 'a:5:{s:5:"table";s:17:"analysis_organism";s:11:"description";s:87:"This view is for associating an organism (via it''s associated features) to an analysis.";s:6:"fields";a:2:{s:11:"analysis_id";a:3:{s:4:"size";s:3:"big";s:4:"type";s:3:"int";s:8:"not null";b:1;}s:11:"organism_id";a:3:{s:4:"size";s:3:"big";s:4:"type";s:3:"int";s:8:"not null";b:1;}}s:7:"indexes";a:2:{s:20:"networkmod_qtl_indx0";a:1:{i:0;s:11:"analysis_id";}s:20:"networkmod_qtl_indx1";a:1:{i:0;s:11:"organism_id";}}s:12:"foreign keys";a:2:{s:8:"analysis";a:2:{s:5:"table";s:8:"analysis";s:7:"columns";a:1:{s:11:"analysis_id";s:11:"analysis_id";}}s:8:"organism";a:2:{s:5:"table";s:8:"organism";s:7:"columns";a:1:{s:11:"organism_id";s:11:"organism_id";}}}}', 0, 'chado');
INSERT INTO public.tripal_custom_tables VALUES (2, 'tripal_gffcds_temp', 'a:3:{s:5:"table";s:18:"tripal_gffcds_temp";s:6:"fields";a:6:{s:10:"feature_id";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:9:"parent_id";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:5:"phase";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:0;}s:6:"strand";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:4:"fmin";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:4:"fmax";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}}s:7:"indexes";a:1:{s:20:"tripal_gff_temp_idx0";a:1:{i:0;s:9:"parent_id";}}}', 1, 'chado');
INSERT INTO public.tripal_custom_tables VALUES (3, 'tripal_gffprotein_temp', 'a:4:{s:5:"table";s:22:"tripal_gffprotein_temp";s:6:"fields";a:4:{s:10:"feature_id";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:9:"parent_id";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:4:"fmin";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:4:"fmax";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}}s:7:"indexes";a:1:{s:20:"tripal_gff_temp_idx0";a:1:{i:0;s:9:"parent_id";}}s:11:"unique keys";a:1:{s:19:"tripal_gff_temp_uq0";a:1:{i:0;s:10:"feature_id";}}}', 1, 'chado');
INSERT INTO public.tripal_custom_tables VALUES (4, 'tripal_obo_temp', 'a:4:{s:5:"table";s:15:"tripal_obo_temp";s:6:"fields";a:3:{s:2:"id";a:3:{s:4:"type";s:7:"varchar";s:6:"length";i:255;s:8:"not null";b:1;}s:6:"stanza";a:2:{s:4:"type";s:4:"text";s:8:"not null";b:1;}s:4:"type";a:3:{s:4:"type";s:7:"varchar";s:6:"length";i:50;s:8:"not null";b:1;}}s:7:"indexes";a:2:{s:20:"tripal_obo_temp_idx0";a:1:{i:0;s:2:"id";}s:20:"tripal_obo_temp_idx1";a:1:{i:0;s:4:"type";}}s:11:"unique keys";a:1:{s:16:"tripal_obo_temp0";a:1:{i:0;s:2:"id";}}}', 1, 'chado');
INSERT INTO public.tripal_custom_tables VALUES (5, 'organism_stock_count', 'a:4:{s:11:"description";s:49:"Stores the type and number of stocks per organism";s:5:"table";s:20:"organism_stock_count";s:6:"fields";a:7:{s:11:"organism_id";a:3:{s:4:"size";s:3:"big";s:4:"type";s:3:"int";s:8:"not null";b:1;}s:5:"genus";a:3:{s:4:"type";s:7:"varchar";s:6:"length";s:3:"255";s:8:"not null";b:1;}s:7:"species";a:3:{s:4:"type";s:7:"varchar";s:6:"length";s:3:"255";s:8:"not null";b:1;}s:11:"common_name";a:3:{s:4:"type";s:7:"varchar";s:6:"length";s:3:"255";s:8:"not null";b:0;}s:10:"num_stocks";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:9:"cvterm_id";a:3:{s:4:"size";s:3:"big";s:4:"type";s:3:"int";s:8:"not null";b:1;}s:10:"stock_type";a:3:{s:4:"type";s:7:"varchar";s:6:"length";s:3:"255";s:8:"not null";b:1;}}s:7:"indexes";a:3:{s:25:"organism_stock_count_idx1";a:1:{i:0;s:11:"organism_id";}s:25:"organism_stock_count_idx2";a:1:{i:0;s:9:"cvterm_id";}s:25:"organism_stock_count_idx3";a:1:{i:0;s:10:"stock_type";}}}', 0, 'chado');
INSERT INTO public.tripal_custom_tables VALUES (6, 'library_feature_count', 'a:4:{s:5:"table";s:21:"library_feature_count";s:11:"description";s:72:"Provides count of feature by type that are associated with all libraries";s:6:"fields";a:4:{s:10:"library_id";a:3:{s:4:"size";s:3:"big";s:4:"type";s:3:"int";s:8:"not null";b:1;}s:4:"name";a:3:{s:4:"type";s:7:"varchar";s:6:"length";i:255;s:8:"not null";b:1;}s:12:"num_features";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:12:"feature_type";a:3:{s:4:"type";s:7:"varchar";s:6:"length";i:255;s:8:"not null";b:1;}}s:7:"indexes";a:1:{s:26:"library_feature_count_idx1";a:1:{i:0;s:10:"library_id";}}}', 0, 'chado');
INSERT INTO public.tripal_custom_tables VALUES (7, 'organism_feature_count', 'a:4:{s:11:"description";s:51:"Stores the type and number of features per organism";s:5:"table";s:22:"organism_feature_count";s:6:"fields";a:7:{s:11:"organism_id";a:3:{s:4:"size";s:3:"big";s:4:"type";s:3:"int";s:8:"not null";b:1;}s:5:"genus";a:3:{s:4:"type";s:7:"varchar";s:6:"length";s:3:"255";s:8:"not null";b:1;}s:7:"species";a:3:{s:4:"type";s:7:"varchar";s:6:"length";s:3:"255";s:8:"not null";b:1;}s:11:"common_name";a:3:{s:4:"type";s:7:"varchar";s:6:"length";s:3:"255";s:8:"not null";b:0;}s:12:"num_features";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:9:"cvterm_id";a:3:{s:4:"size";s:3:"big";s:4:"type";s:3:"int";s:8:"not null";b:1;}s:12:"feature_type";a:3:{s:4:"type";s:7:"varchar";s:6:"length";s:3:"255";s:8:"not null";b:1;}}s:7:"indexes";a:3:{s:27:"organism_feature_count_idx1";a:1:{i:0;s:11:"organism_id";}s:27:"organism_feature_count_idx2";a:1:{i:0;s:9:"cvterm_id";}s:27:"organism_feature_count_idx3";a:1:{i:0;s:12:"feature_type";}}}', 0, 'chado');
INSERT INTO public.tripal_custom_tables VALUES (9, 'cv_root_mview', 'a:4:{s:5:"table";s:13:"cv_root_mview";s:11:"description";s:93:"A list of the root terms for all controlled vocabularies. This is needed for viewing CV trees";s:6:"fields";a:4:{s:4:"name";a:3:{s:4:"type";s:7:"varchar";s:6:"length";i:255;s:8:"not null";b:1;}s:9:"cvterm_id";a:3:{s:4:"size";s:3:"big";s:4:"type";s:3:"int";s:8:"not null";b:1;}s:5:"cv_id";a:3:{s:4:"size";s:3:"big";s:4:"type";s:3:"int";s:8:"not null";b:1;}s:7:"cv_name";a:3:{s:4:"type";s:7:"varchar";s:6:"length";i:255;s:8:"not null";b:1;}}s:7:"indexes";a:2:{s:19:"cv_root_mview_indx1";a:1:{i:0;s:9:"cvterm_id";}s:19:"cv_root_mview_indx2";a:1:{i:0;s:5:"cv_id";}}}', 1, 'chado');
INSERT INTO public.tripal_custom_tables VALUES (10, 'db2cv_mview', 'a:4:{s:5:"table";s:11:"db2cv_mview";s:11:"description";s:88:"A table for quick lookup of the vocabularies and the databases they are associated with.";s:6:"fields";a:5:{s:5:"cv_id";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:6:"cvname";a:3:{s:4:"type";s:7:"varchar";s:6:"length";s:3:"255";s:8:"not null";b:1;}s:5:"db_id";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}s:6:"dbname";a:3:{s:4:"type";s:7:"varchar";s:6:"length";s:3:"255";s:8:"not null";b:1;}s:9:"num_terms";a:2:{s:4:"type";s:3:"int";s:8:"not null";b:1;}}s:7:"indexes";a:4:{s:9:"cv_id_idx";a:1:{i:0;s:5:"cv_id";}s:10:"cvname_idx";a:1:{i:0;s:6:"cvname";}s:9:"db_id_idx";a:1:{i:0;s:5:"db_id";}s:10:"dbname_idx";a:1:{i:0;s:5:"db_id";}}}', 1, 'chado');


INSERT INTO public.tripal_cv_obo VALUES (1, 'Relationship Ontology (legacy)', '{tripal_chado}/files/legacy_ro.obo');
INSERT INTO public.tripal_cv_obo VALUES (2, 'The Gene Ontology (GO) knowledgebase is the world’s largest source of information on the functions of genes', 'http://purl.obolibrary.org/obo/go.obo');
INSERT INTO public.tripal_cv_obo VALUES (3, 'The Sequence Ontology', 'http://purl.obolibrary.org/obo/so.obo');
INSERT INTO public.tripal_cv_obo VALUES (4, 'A vocabulary of taxonomic ranks (species, family, phylum, etc)', 'http://purl.obolibrary.org/obo/taxrank.obo');
INSERT INTO public.tripal_cv_obo VALUES (5, 'Tripal Contact Ontology. A temporary ontology until a more formal appropriate ontology can be identified.', '{tripal_chado}/files/tcontact.obo');
INSERT INTO public.tripal_cv_obo VALUES (6, 'Tripal Publication Ontology. A temporary ontology until a more formal appropriate ontology can be identified.', '{tripal_chado}/files/tpub.obo');


INSERT INTO public.tripal_id_space_collection VALUES ('CO_010', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('dc', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('data', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('format', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('operation', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('topic', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('EFO', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('ERO', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('OBCS', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('OBI', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('OGI', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('IAO', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('null', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('local', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('SBO', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('SWO', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('PMID', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('UO', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('NCIT', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('NCBITaxon', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('rdfs', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('RO', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('GO', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('SO', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('TAXRANK', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('TCONTACT', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('TPUB', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('foaf', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('hydra', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('rdf', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('schema', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('sep', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('SIO', 'chado_id_space');
INSERT INTO public.tripal_id_space_collection VALUES ('synonym_type', 'chado_id_space');


INSERT INTO public.tripal_import VALUES (1, 1, 'chado_obo_loader', NULL, 'YToyOntzOjg6InJ1bl9hcmdzIjthOjI6e3M6Njoib2JvX2lkIjtzOjE6IjMiO3M6MTE6InNjaGVtYV9uYW1lIjtzOjU6ImNoYWRvIjt9czo1OiJmaWxlcyI7YTowOnt9fQ==', 1667003577);
INSERT INTO public.tripal_import VALUES (2, 1, 'chado_obo_loader', NULL, 'YToyOntzOjg6InJ1bl9hcmdzIjthOjI6e3M6Njoib2JvX2lkIjtzOjE6IjQiO3M6MTE6InNjaGVtYV9uYW1lIjtzOjU6ImNoYWRvIjt9czo1OiJmaWxlcyI7YTowOnt9fQ==', 1667003599);
INSERT INTO public.tripal_import VALUES (3, 1, 'chado_obo_loader', NULL, 'YToyOntzOjg6InJ1bl9hcmdzIjthOjI6e3M6Njoib2JvX2lkIjtzOjE6IjUiO3M6MTE6InNjaGVtYV9uYW1lIjtzOjU6ImNoYWRvIjt9czo1OiJmaWxlcyI7YTowOnt9fQ==', 1667003600);
INSERT INTO public.tripal_import VALUES (4, 1, 'chado_obo_loader', NULL, 'YToyOntzOjg6InJ1bl9hcmdzIjthOjI6e3M6Njoib2JvX2lkIjtzOjE6IjYiO3M6MTE6InNjaGVtYV9uYW1lIjtzOjU6ImNoYWRvIjt9czo1OiJmaWxlcyI7YTowOnt9fQ==', 1667003600);


INSERT INTO public.tripal_jobs VALUES (1, 1, 'Install Chado 1.3', 'tripal_chado', 'tripal_chado_install_chado', 'a:2:{i:0;s:5:"chado";i:1;s:3:"1.3";}', 100, 'Completed', 1667003549, 1667003558, 1667003562, NULL, NULL, 10, NULL, NULL, 'a:0:{}');
INSERT INTO public.tripal_jobs VALUES (2, 1, 'Prepare Chado', 'tripal_chado', 'tripal_chado_prepare_chado', 'a:1:{i:0;s:5:"chado";}', 100, 'Completed', 1667003569, 1667003574, 1667003613, NULL, NULL, 10, NULL, NULL, 'a:0:{}');


INSERT INTO public.tripal_mviews VALUES (1, 5, 'organism_stock_count', '
      SELECT
          O.organism_id, O.genus, O.species, O.common_name,
          count(S.stock_id) as num_stocks,
          CVT.cvterm_id, CVT.name as stock_type
       FROM organism O
          INNER JOIN stock S ON O.Organism_id = S.organism_id
          INNER JOIN cvterm CVT ON S.type_id = CVT.cvterm_id
       GROUP BY
          O.Organism_id, O.genus, O.species, O.common_name, CVT.cvterm_id, CVT.name
    ', NULL, NULL, 'Stores the type and number of stocks per organism');
INSERT INTO public.tripal_mviews VALUES (5, 9, 'cv_root_mview', '
      SELECT DISTINCT CVT.name, CVT.cvterm_id, CV.cv_id, CV.name
      FROM cvterm CVT
        LEFT JOIN cvterm_relationship CVTR ON CVT.cvterm_id = CVTR.subject_id
        INNER JOIN cvterm_relationship CVTR2 ON CVT.cvterm_id = CVTR2.object_id
      INNER JOIN cv CV on CV.cv_id = CVT.cv_id
      WHERE CVTR.subject_id is NULL and
        CVT.is_relationshiptype = 0 and CVT.is_obsolete = 0
    ', 1667003601, 'Populated with 9 rows', 'A list of the root terms for all controlled vocabularies. This is needed for viewing CV trees');
INSERT INTO public.tripal_mviews VALUES (2, 6, 'library_feature_count', '
      SELECT
        L.library_id, L.name,
        count(F.feature_id) as num_features,
        CVT.name as feature_type
      FROM library L
        INNER JOIN library_feature LF  ON LF.library_id = L.library_id
        INNER JOIN feature F           ON LF.feature_id = F.feature_id
        INNER JOIN cvterm CVT          ON F.type_id     = CVT.cvterm_id
      GROUP BY L.library_id, L.name, CVT.name
    ', NULL, NULL, 'Provides count of feature by type that are associated with all libraries');
INSERT INTO public.tripal_mviews VALUES (6, 10, 'db2cv_mview', '
      SELECT DISTINCT CV.cv_id, CV.name as cvname, DB.db_id, DB.name as dbname,
        COUNT(CVT.cvterm_id) as num_terms
      FROM cv CV
        INNER JOIN cvterm CVT on CVT.cv_id = CV.cv_id
        INNER JOIN dbxref DBX on DBX.dbxref_id = CVT.dbxref_id
        INNER JOIN db DB on DB.db_id = DBX.db_id
      WHERE CVT.is_relationshiptype = 0 and CVT.is_obsolete = 0
      GROUP BY CV.cv_id, CV.name, DB.db_id, DB.name
      ORDER BY DB.name
    ', 1667003601, 'Populated with 41 rows', 'A table for quick lookup of the vocabularies and the databases they are associated with.');
INSERT INTO public.tripal_mviews VALUES (3, 7, 'organism_feature_count', '
      SELECT
          O.organism_id, O.genus, O.species, O.common_name,
          count(F.feature_id) as num_features,
          CVT.cvterm_id, CVT.name as feature_type
       FROM organism O
          INNER JOIN feature F  ON O.Organism_id = F.organism_id
          INNER JOIN cvterm CVT ON F.type_id     = CVT.cvterm_id
       GROUP BY
          O.Organism_id, O.genus, O.species, O.common_name, CVT.cvterm_id, CVT.name
    ', NULL, NULL, 'Stores the type and number of features per organism');
INSERT INTO public.tripal_mviews VALUES (4, 8, 'analysis_organism', '
      SELECT DISTINCT A.analysis_id, O.organism_id
      FROM analysis A
        INNER JOIN analysisfeature AF ON A.analysis_id = AF.analysis_id
        INNER JOIN feature F ON AF.feature_id = F.feature_id
        INNER JOIN organism O ON O.organism_id = F.organism_id
    ', NULL, NULL, 'This view is for associating an organism (via it''s associated features) to an analysis.');

INSERT INTO public.tripal_vocabulary_collection VALUES ('germplasm_ontology', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('dc', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('EDAM', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('efo', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('ero', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('OBCS', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('obi', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('ogi', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('IAO', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('null', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('local', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('organism_property', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('analysis_property', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('tripal_phylogeny', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('feature_relationship', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('feature_property', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('contact_property', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('contact_type', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('tripal_contact', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('contact_relationship', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('featuremap_units', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('featurepos_property', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('featuremap_property', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('library_property', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('library_type', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('project_property', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('study_property', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('project_relationship', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('tripal_pub', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('pub_type', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('pub_property', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('pub_relationship', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('stock_relationship', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('stock_property', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('stock_type', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('tripal_analysis', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('nd_experiment_types', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('nd_geolocation_property', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('sbo', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('swo', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('PMID', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('uo', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('ncit', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('ncbitaxon', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('rdfs', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('ro', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('cellular_component', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('biological_process', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('molecular_function', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('sequence', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('taxonomic_rank', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('foaf', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('hydra', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('rdf', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('schema', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('sep', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('SIO', 'chado_vocabulary');
INSERT INTO public.tripal_vocabulary_collection VALUES ('synonym_type', 'chado_vocabulary');

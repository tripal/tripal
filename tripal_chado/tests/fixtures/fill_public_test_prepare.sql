SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: tripal_admin_notifications; Type: TABLE; Schema: public; Owner: drupal
--

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

--
-- Name: TABLE tripal_admin_notifications; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_admin_notifications IS 'This table is used for information describing administrative
     notifications. For example, when new fields are available.';


--
-- Name: COLUMN tripal_admin_notifications.details; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_admin_notifications.details IS 'Description and additional information relating to the notification.';


--
-- Name: COLUMN tripal_admin_notifications.title; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_admin_notifications.title IS 'Title of the notification.';


--
-- Name: COLUMN tripal_admin_notifications.actions; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_admin_notifications.actions IS 'Actions that can be performed on the notification,::text like disimissal or import.';


--
-- Name: COLUMN tripal_admin_notifications.submitter_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_admin_notifications.submitter_id IS 'A unique id that should be specific to the notification to ensure notifications are not duplicated.';


--
-- Name: COLUMN tripal_admin_notifications.enabled; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_admin_notifications.enabled IS 'Boolean indicating whether the notification is enabled or disabled (disabled will not be shown on the dashboard).';


--
-- Name: COLUMN tripal_admin_notifications.type; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_admin_notifications.type IS 'Type of the notification, relating to what tripal function the notification belongs to, IE Fields, Jobs, Vocabulary.';


--
-- Name: tripal_admin_notifications_note_id_seq; Type: SEQUENCE; Schema: public; Owner: drupal
--

CREATE SEQUENCE public.tripal_admin_notifications_note_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_admin_notifications_note_id_seq OWNER TO drupal;

--
-- Name: tripal_admin_notifications_note_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: drupal
--

ALTER SEQUENCE public.tripal_admin_notifications_note_id_seq OWNED BY public.tripal_admin_notifications.note_id;


--
-- Name: tripal_collection; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_collection (
    collection_id integer NOT NULL,
    collection_name character varying(1024) NOT NULL,
    description text,
    uid integer NOT NULL,
    create_date integer NOT NULL,
    CONSTRAINT tripal_collection_collection_id_check CHECK ((collection_id >= 0))
);


ALTER TABLE public.tripal_collection OWNER TO drupal;

--
-- Name: COLUMN tripal_collection.uid; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_collection.uid IS 'The user Id of the person who created the collection.';


--
-- Name: COLUMN tripal_collection.create_date; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_collection.create_date IS 'UNIX integer start time';


--
-- Name: tripal_collection_bundle; Type: TABLE; Schema: public; Owner: drupal
--

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

--
-- Name: COLUMN tripal_collection_bundle.ids; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_collection_bundle.ids IS 'An array of entity IDs.';


--
-- Name: COLUMN tripal_collection_bundle.fields; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_collection_bundle.fields IS 'An array of numeric field IDs.';


--
-- Name: COLUMN tripal_collection_bundle.site_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_collection_bundle.site_id IS 'The ID of the site from the Tripal Sites table.';


--
-- Name: tripal_collection_bundle_collection_bundle_id_seq; Type: SEQUENCE; Schema: public; Owner: drupal
--

CREATE SEQUENCE public.tripal_collection_bundle_collection_bundle_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_collection_bundle_collection_bundle_id_seq OWNER TO drupal;

--
-- Name: tripal_collection_bundle_collection_bundle_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: drupal
--

ALTER SEQUENCE public.tripal_collection_bundle_collection_bundle_id_seq OWNED BY public.tripal_collection_bundle.collection_bundle_id;


--
-- Name: tripal_collection_collection_id_seq; Type: SEQUENCE; Schema: public; Owner: drupal
--

CREATE SEQUENCE public.tripal_collection_collection_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_collection_collection_id_seq OWNER TO drupal;

--
-- Name: tripal_collection_collection_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: drupal
--

ALTER SEQUENCE public.tripal_collection_collection_id_seq OWNED BY public.tripal_collection.collection_id;


--
-- Name: tripal_custom_quota; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_custom_quota (
    uid bigint NOT NULL,
    custom_quota bigint NOT NULL,
    custom_expiration bigint NOT NULL
);


ALTER TABLE public.tripal_custom_quota OWNER TO drupal;

--
-- Name: tripal_custom_tables; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_custom_tables (
    table_id integer NOT NULL,
    table_name character varying(255) NOT NULL,
    schema text NOT NULL,
    hidden smallint DEFAULT 0,
    chado character varying(64) NOT NULL,
    CONSTRAINT tripal_custom_tables_table_id_check CHECK ((table_id >= 0))
);


ALTER TABLE public.tripal_custom_tables OWNER TO drupal;

--
-- Name: COLUMN tripal_custom_tables.hidden; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_custom_tables.hidden IS 'Set to true if this custom table is not for end-users to manage, but for the Tripal module.';


--
-- Name: COLUMN tripal_custom_tables.chado; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_custom_tables.chado IS 'The name of the Chado schema where this table exists.';


--
-- Name: tripal_custom_tables_table_id_seq; Type: SEQUENCE; Schema: public; Owner: drupal
--

CREATE SEQUENCE public.tripal_custom_tables_table_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_custom_tables_table_id_seq OWNER TO drupal;

--
-- Name: tripal_custom_tables_table_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: drupal
--

ALTER SEQUENCE public.tripal_custom_tables_table_id_seq OWNED BY public.tripal_custom_tables.table_id;


--
-- Name: tripal_cv_obo; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_cv_obo (
    obo_id integer NOT NULL,
    name character varying(255),
    path character varying(1024),
    CONSTRAINT tripal_cv_obo_obo_id_check CHECK ((obo_id >= 0))
);


ALTER TABLE public.tripal_cv_obo OWNER TO drupal;

--
-- Name: tripal_cv_obo_obo_id_seq; Type: SEQUENCE; Schema: public; Owner: drupal
--

CREATE SEQUENCE public.tripal_cv_obo_obo_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_cv_obo_obo_id_seq OWNER TO drupal;

--
-- Name: tripal_cv_obo_obo_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: drupal
--

ALTER SEQUENCE public.tripal_cv_obo_obo_id_seq OWNED BY public.tripal_cv_obo.obo_id;


--
-- Name: tripal_entity; Type: TABLE; Schema: public; Owner: drupal
--

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

--
-- Name: TABLE tripal_entity; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity IS 'The base table for tripal_entity entities.';


--
-- Name: COLUMN tripal_entity.type; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity.type IS 'The ID of the target entity.';


--
-- Name: COLUMN tripal_entity.uid; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity.uid IS 'The ID of the target entity.';


--
-- Name: tripal_entity__bio_data_10_schema_comment; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_10_schema_comment (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_10_schema_comment_value text,
    bio_data_10_schema_comment_record_id integer,
    CONSTRAINT tripal_entity__bio_data_10_schema_comment_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_10_schema_comment_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_10_schema_comment_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_10_schema_comment OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_10_schema_comment; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_10_schema_comment IS 'Data storage for tripal_entity field bio_data_10_schema_comment.';


--
-- Name: COLUMN tripal_entity__bio_data_10_schema_comment.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_10_schema_comment.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_10_schema_comment.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_10_schema_comment.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_10_schema_comment.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_10_schema_comment.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_10_schema_comment.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_10_schema_comment.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_10_schema_comment.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_10_schema_comment.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_10_schema_comment.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_10_schema_comment.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_10_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_10_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_10_schema_name_value character varying(255),
    bio_data_10_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_10_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_10_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_10_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_10_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_10_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_10_schema_name IS 'Data storage for tripal_entity field bio_data_10_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_10_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_10_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_10_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_10_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_10_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_10_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_10_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_10_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_10_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_10_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_10_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_10_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_11_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_11_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_11_schema_description_value text,
    bio_data_11_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_11_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_11_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_11_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_11_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_11_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_11_schema_description IS 'Data storage for tripal_entity field bio_data_11_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_11_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_11_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_11_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_11_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_11_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_11_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_11_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_11_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_11_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_11_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_11_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_11_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_11_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_11_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_11_schema_name_value character varying(255),
    bio_data_11_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_11_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_11_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_11_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_11_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_11_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_11_schema_name IS 'Data storage for tripal_entity field bio_data_11_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_11_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_11_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_11_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_11_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_11_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_11_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_11_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_11_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_11_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_11_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_11_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_11_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_12_data_0842; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_12_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_12_data_0842_value text,
    bio_data_12_data_0842_record_id integer,
    CONSTRAINT tripal_entity__bio_data_12_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_12_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_12_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_12_data_0842 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_12_data_0842; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_12_data_0842 IS 'Data storage for tripal_entity field bio_data_12_data_0842.';


--
-- Name: COLUMN tripal_entity__bio_data_12_data_0842.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_12_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_12_data_0842.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_12_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_12_data_0842.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_12_data_0842.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_12_data_0842.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_12_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_12_data_0842.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_12_data_0842.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_12_data_0842.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_12_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_12_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_12_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_12_schema_name_value character varying(255),
    bio_data_12_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_12_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_12_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_12_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_12_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_12_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_12_schema_name IS 'Data storage for tripal_entity field bio_data_12_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_12_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_12_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_12_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_12_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_12_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_12_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_12_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_12_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_12_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_12_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_12_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_12_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_13_data_1047; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_13_data_1047 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_13_data_1047_value text,
    bio_data_13_data_1047_record_id integer,
    CONSTRAINT tripal_entity__bio_data_13_data_1047_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_13_data_1047_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_13_data_1047_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_13_data_1047 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_13_data_1047; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_13_data_1047 IS 'Data storage for tripal_entity field bio_data_13_data_1047.';


--
-- Name: COLUMN tripal_entity__bio_data_13_data_1047.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_data_1047.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_13_data_1047.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_data_1047.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_13_data_1047.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_data_1047.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_13_data_1047.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_data_1047.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_13_data_1047.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_data_1047.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_13_data_1047.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_data_1047.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_13_iao_0000064; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_13_iao_0000064 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_13_iao_0000064_value character varying(255),
    bio_data_13_iao_0000064_record_id integer,
    CONSTRAINT tripal_entity__bio_data_13_iao_0000064_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_13_iao_0000064_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_13_iao_0000064_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_13_iao_0000064 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_13_iao_0000064; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_13_iao_0000064 IS 'Data storage for tripal_entity field bio_data_13_iao_0000064.';


--
-- Name: COLUMN tripal_entity__bio_data_13_iao_0000064.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_iao_0000064.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_13_iao_0000064.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_iao_0000064.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_13_iao_0000064.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_iao_0000064.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_13_iao_0000064.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_iao_0000064.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_13_iao_0000064.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_iao_0000064.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_13_iao_0000064.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_iao_0000064.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_13_iao_0000129; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_13_iao_0000129 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_13_iao_0000129_value character varying(255),
    bio_data_13_iao_0000129_record_id integer,
    CONSTRAINT tripal_entity__bio_data_13_iao_0000129_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_13_iao_0000129_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_13_iao_0000129_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_13_iao_0000129 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_13_iao_0000129; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_13_iao_0000129 IS 'Data storage for tripal_entity field bio_data_13_iao_0000129.';


--
-- Name: COLUMN tripal_entity__bio_data_13_iao_0000129.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_iao_0000129.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_13_iao_0000129.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_iao_0000129.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_13_iao_0000129.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_iao_0000129.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_13_iao_0000129.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_iao_0000129.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_13_iao_0000129.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_iao_0000129.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_13_iao_0000129.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_iao_0000129.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_13_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_13_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_13_schema_description_value text,
    bio_data_13_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_13_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_13_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_13_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_13_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_13_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_13_schema_description IS 'Data storage for tripal_entity field bio_data_13_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_13_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_13_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_13_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_13_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_13_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_13_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_13_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_13_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_13_schema_name_value character varying(255),
    bio_data_13_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_13_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_13_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_13_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_13_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_13_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_13_schema_name IS 'Data storage for tripal_entity field bio_data_13_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_13_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_13_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_13_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_13_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_13_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_13_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_13_swo_0000001; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_13_swo_0000001 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_13_swo_0000001_value character varying(255),
    bio_data_13_swo_0000001_record_id integer,
    CONSTRAINT tripal_entity__bio_data_13_swo_0000001_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_13_swo_0000001_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_13_swo_0000001_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_13_swo_0000001 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_13_swo_0000001; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_13_swo_0000001 IS 'Data storage for tripal_entity field bio_data_13_swo_0000001.';


--
-- Name: COLUMN tripal_entity__bio_data_13_swo_0000001.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_swo_0000001.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_13_swo_0000001.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_swo_0000001.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_13_swo_0000001.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_swo_0000001.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_13_swo_0000001.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_swo_0000001.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_13_swo_0000001.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_swo_0000001.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_13_swo_0000001.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_13_swo_0000001.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_14_data_1047; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_14_data_1047 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_14_data_1047_value text,
    bio_data_14_data_1047_record_id integer,
    CONSTRAINT tripal_entity__bio_data_14_data_1047_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_14_data_1047_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_14_data_1047_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_14_data_1047 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_14_data_1047; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_14_data_1047 IS 'Data storage for tripal_entity field bio_data_14_data_1047.';


--
-- Name: COLUMN tripal_entity__bio_data_14_data_1047.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_data_1047.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_14_data_1047.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_data_1047.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_14_data_1047.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_data_1047.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_14_data_1047.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_data_1047.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_14_data_1047.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_data_1047.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_14_data_1047.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_data_1047.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_14_iao_0000064; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_14_iao_0000064 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_14_iao_0000064_value character varying(255),
    bio_data_14_iao_0000064_record_id integer,
    CONSTRAINT tripal_entity__bio_data_14_iao_0000064_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_14_iao_0000064_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_14_iao_0000064_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_14_iao_0000064 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_14_iao_0000064; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_14_iao_0000064 IS 'Data storage for tripal_entity field bio_data_14_iao_0000064.';


--
-- Name: COLUMN tripal_entity__bio_data_14_iao_0000064.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_iao_0000064.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_14_iao_0000064.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_iao_0000064.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_14_iao_0000064.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_iao_0000064.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_14_iao_0000064.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_iao_0000064.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_14_iao_0000064.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_iao_0000064.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_14_iao_0000064.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_iao_0000064.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_14_iao_0000129; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_14_iao_0000129 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_14_iao_0000129_value character varying(255),
    bio_data_14_iao_0000129_record_id integer,
    CONSTRAINT tripal_entity__bio_data_14_iao_0000129_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_14_iao_0000129_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_14_iao_0000129_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_14_iao_0000129 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_14_iao_0000129; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_14_iao_0000129 IS 'Data storage for tripal_entity field bio_data_14_iao_0000129.';


--
-- Name: COLUMN tripal_entity__bio_data_14_iao_0000129.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_iao_0000129.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_14_iao_0000129.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_iao_0000129.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_14_iao_0000129.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_iao_0000129.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_14_iao_0000129.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_iao_0000129.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_14_iao_0000129.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_iao_0000129.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_14_iao_0000129.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_iao_0000129.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_14_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_14_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_14_schema_description_value text,
    bio_data_14_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_14_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_14_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_14_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_14_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_14_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_14_schema_description IS 'Data storage for tripal_entity field bio_data_14_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_14_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_14_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_14_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_14_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_14_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_14_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_14_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_14_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_14_schema_name_value character varying(255),
    bio_data_14_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_14_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_14_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_14_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_14_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_14_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_14_schema_name IS 'Data storage for tripal_entity field bio_data_14_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_14_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_14_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_14_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_14_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_14_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_14_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_14_swo_0000001; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_14_swo_0000001 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_14_swo_0000001_value character varying(255),
    bio_data_14_swo_0000001_record_id integer,
    CONSTRAINT tripal_entity__bio_data_14_swo_0000001_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_14_swo_0000001_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_14_swo_0000001_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_14_swo_0000001 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_14_swo_0000001; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_14_swo_0000001 IS 'Data storage for tripal_entity field bio_data_14_swo_0000001.';


--
-- Name: COLUMN tripal_entity__bio_data_14_swo_0000001.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_swo_0000001.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_14_swo_0000001.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_swo_0000001.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_14_swo_0000001.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_swo_0000001.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_14_swo_0000001.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_swo_0000001.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_14_swo_0000001.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_swo_0000001.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_14_swo_0000001.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_14_swo_0000001.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_15_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_15_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_15_schema_description_value text,
    bio_data_15_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_15_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_15_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_15_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_15_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_15_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_15_schema_description IS 'Data storage for tripal_entity field bio_data_15_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_15_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_15_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_15_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_15_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_15_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_15_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_15_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_15_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_15_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_15_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_15_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_15_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_15_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_15_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_15_schema_name_value character varying(255),
    bio_data_15_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_15_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_15_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_15_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_15_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_15_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_15_schema_name IS 'Data storage for tripal_entity field bio_data_15_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_15_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_15_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_15_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_15_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_15_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_15_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_15_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_15_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_15_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_15_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_15_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_15_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_16_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_16_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_16_schema_description_value text,
    bio_data_16_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_16_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_16_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_16_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_16_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_16_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_16_schema_description IS 'Data storage for tripal_entity field bio_data_16_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_16_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_16_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_16_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_16_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_16_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_16_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_16_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_16_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_16_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_16_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_16_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_16_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_16_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_16_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_16_schema_name_value character varying(255),
    bio_data_16_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_16_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_16_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_16_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_16_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_16_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_16_schema_name IS 'Data storage for tripal_entity field bio_data_16_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_16_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_16_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_16_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_16_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_16_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_16_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_16_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_16_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_16_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_16_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_16_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_16_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_17_data_0842; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_17_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_17_data_0842_value text,
    bio_data_17_data_0842_record_id integer,
    CONSTRAINT tripal_entity__bio_data_17_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_17_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_17_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_17_data_0842 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_17_data_0842; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_17_data_0842 IS 'Data storage for tripal_entity field bio_data_17_data_0842.';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_0842.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_0842.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_0842.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_0842.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_0842.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_0842.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_0842.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_0842.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_17_data_1249; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_17_data_1249 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_17_data_1249_value integer,
    bio_data_17_data_1249_record_id integer,
    CONSTRAINT tripal_entity__bio_data_17_data_1249_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_17_data_1249_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_17_data_1249_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_17_data_1249 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_17_data_1249; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_17_data_1249 IS 'Data storage for tripal_entity field bio_data_17_data_1249.';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_1249.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_1249.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_1249.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_1249.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_1249.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_1249.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_1249.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_1249.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_1249.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_1249.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_1249.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_1249.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_17_data_2044; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_17_data_2044 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_17_data_2044_value text,
    bio_data_17_data_2044_record_id integer,
    CONSTRAINT tripal_entity__bio_data_17_data_2044_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_17_data_2044_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_17_data_2044_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_17_data_2044 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_17_data_2044; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_17_data_2044 IS 'Data storage for tripal_entity field bio_data_17_data_2044.';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_2044.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_2044.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_2044.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_2044.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_2044.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_2044.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_2044.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_2044.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_2044.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_2044.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_17_data_2044.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_data_2044.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_17_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_17_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_17_schema_name_value character varying(255),
    bio_data_17_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_17_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_17_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_17_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_17_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_17_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_17_schema_name IS 'Data storage for tripal_entity field bio_data_17_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_17_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_17_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_17_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_17_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_17_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_17_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_17_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_18_data_0842; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_18_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_18_data_0842_value text,
    bio_data_18_data_0842_record_id integer,
    CONSTRAINT tripal_entity__bio_data_18_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_18_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_18_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_18_data_0842 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_18_data_0842; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_18_data_0842 IS 'Data storage for tripal_entity field bio_data_18_data_0842.';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_0842.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_0842.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_0842.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_0842.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_0842.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_0842.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_0842.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_0842.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_18_data_1249; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_18_data_1249 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_18_data_1249_value integer,
    bio_data_18_data_1249_record_id integer,
    CONSTRAINT tripal_entity__bio_data_18_data_1249_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_18_data_1249_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_18_data_1249_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_18_data_1249 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_18_data_1249; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_18_data_1249 IS 'Data storage for tripal_entity field bio_data_18_data_1249.';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_1249.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_1249.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_1249.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_1249.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_1249.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_1249.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_1249.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_1249.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_1249.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_1249.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_1249.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_1249.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_18_data_2044; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_18_data_2044 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_18_data_2044_value text,
    bio_data_18_data_2044_record_id integer,
    CONSTRAINT tripal_entity__bio_data_18_data_2044_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_18_data_2044_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_18_data_2044_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_18_data_2044 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_18_data_2044; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_18_data_2044 IS 'Data storage for tripal_entity field bio_data_18_data_2044.';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_2044.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_2044.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_2044.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_2044.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_2044.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_2044.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_2044.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_2044.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_2044.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_2044.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_18_data_2044.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_data_2044.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_18_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_18_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_18_schema_name_value character varying(255),
    bio_data_18_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_18_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_18_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_18_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_18_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_18_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_18_schema_name IS 'Data storage for tripal_entity field bio_data_18_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_18_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_18_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_18_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_18_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_18_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_18_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_18_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_19_data_0842; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_19_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_19_data_0842_value text,
    bio_data_19_data_0842_record_id integer,
    CONSTRAINT tripal_entity__bio_data_19_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_19_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_19_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_19_data_0842 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_19_data_0842; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_19_data_0842 IS 'Data storage for tripal_entity field bio_data_19_data_0842.';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_0842.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_0842.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_0842.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_0842.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_0842.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_0842.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_0842.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_0842.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_19_data_1249; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_19_data_1249 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_19_data_1249_value integer,
    bio_data_19_data_1249_record_id integer,
    CONSTRAINT tripal_entity__bio_data_19_data_1249_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_19_data_1249_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_19_data_1249_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_19_data_1249 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_19_data_1249; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_19_data_1249 IS 'Data storage for tripal_entity field bio_data_19_data_1249.';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_1249.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_1249.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_1249.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_1249.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_1249.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_1249.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_1249.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_1249.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_1249.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_1249.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_1249.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_1249.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_19_data_2044; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_19_data_2044 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_19_data_2044_value text,
    bio_data_19_data_2044_record_id integer,
    CONSTRAINT tripal_entity__bio_data_19_data_2044_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_19_data_2044_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_19_data_2044_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_19_data_2044 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_19_data_2044; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_19_data_2044 IS 'Data storage for tripal_entity field bio_data_19_data_2044.';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_2044.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_2044.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_2044.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_2044.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_2044.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_2044.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_2044.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_2044.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_2044.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_2044.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_19_data_2044.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_data_2044.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_19_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_19_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_19_schema_name_value character varying(255),
    bio_data_19_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_19_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_19_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_19_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_19_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_19_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_19_schema_name IS 'Data storage for tripal_entity field bio_data_19_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_19_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_19_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_19_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_19_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_19_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_19_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_19_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_1_local_abbreviation; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_1_local_abbreviation (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_1_local_abbreviation_value character varying(255),
    bio_data_1_local_abbreviation_record_id integer,
    CONSTRAINT tripal_entity__bio_data_1_local_abbreviation_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_1_local_abbreviation_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_1_local_abbreviation_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_1_local_abbreviation OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_1_local_abbreviation; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_1_local_abbreviation IS 'Data storage for tripal_entity field bio_data_1_local_abbreviation.';


--
-- Name: COLUMN tripal_entity__bio_data_1_local_abbreviation.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_local_abbreviation.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_1_local_abbreviation.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_local_abbreviation.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_1_local_abbreviation.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_local_abbreviation.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_1_local_abbreviation.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_local_abbreviation.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_1_local_abbreviation.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_local_abbreviation.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_1_local_abbreviation.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_local_abbreviation.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_1_ncbitaxon_common_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_1_ncbitaxon_common_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_1_ncbitaxon_common_name_value character varying(255),
    bio_data_1_ncbitaxon_common_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_1_ncbitaxon_common_na_revision_id_check CHECK ((revision_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_1_ncbitaxon_common_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_1_ncbitaxon_common_name_entity_id_check CHECK ((entity_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_1_ncbitaxon_common_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_1_ncbitaxon_common_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_1_ncbitaxon_common_name IS 'Data storage for tripal_entity field bio_data_1_ncbitaxon_common_name.';


--
-- Name: COLUMN tripal_entity__bio_data_1_ncbitaxon_common_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_ncbitaxon_common_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_1_ncbitaxon_common_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_ncbitaxon_common_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_1_ncbitaxon_common_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_ncbitaxon_common_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_1_ncbitaxon_common_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_ncbitaxon_common_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_1_ncbitaxon_common_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_ncbitaxon_common_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_1_ncbitaxon_common_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_ncbitaxon_common_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_1_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_1_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_1_schema_description_value text,
    bio_data_1_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_1_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_1_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_1_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_1_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_1_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_1_schema_description IS 'Data storage for tripal_entity field bio_data_1_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_1_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_1_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_1_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_1_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_1_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_1_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_1_taxrank_0000005; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_1_taxrank_0000005 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_1_taxrank_0000005_value character varying(255),
    bio_data_1_taxrank_0000005_record_id integer,
    CONSTRAINT tripal_entity__bio_data_1_taxrank_0000005_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_1_taxrank_0000005_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_1_taxrank_0000005_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_1_taxrank_0000005 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_1_taxrank_0000005; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_1_taxrank_0000005 IS 'Data storage for tripal_entity field bio_data_1_taxrank_0000005.';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000005.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000005.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000005.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000005.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000005.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000005.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000005.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000005.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000005.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000005.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000005.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000005.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_1_taxrank_0000006; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_1_taxrank_0000006 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_1_taxrank_0000006_value character varying(255),
    bio_data_1_taxrank_0000006_record_id integer,
    CONSTRAINT tripal_entity__bio_data_1_taxrank_0000006_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_1_taxrank_0000006_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_1_taxrank_0000006_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_1_taxrank_0000006 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_1_taxrank_0000006; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_1_taxrank_0000006 IS 'Data storage for tripal_entity field bio_data_1_taxrank_0000006.';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000006.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000006.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000006.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000006.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000006.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000006.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000006.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000006.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000006.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000006.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000006.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000006.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_1_taxrank_0000045; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_1_taxrank_0000045 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_1_taxrank_0000045_value character varying(1024),
    bio_data_1_taxrank_0000045_record_id integer,
    CONSTRAINT tripal_entity__bio_data_1_taxrank_0000045_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_1_taxrank_0000045_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_1_taxrank_0000045_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_1_taxrank_0000045 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_1_taxrank_0000045; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_1_taxrank_0000045 IS 'Data storage for tripal_entity field bio_data_1_taxrank_0000045.';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000045.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000045.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000045.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000045.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000045.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000045.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000045.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000045.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000045.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000045.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_1_taxrank_0000045.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_1_taxrank_0000045.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_20_data_0842; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_20_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_20_data_0842_value text,
    bio_data_20_data_0842_record_id integer,
    CONSTRAINT tripal_entity__bio_data_20_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_20_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_20_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_20_data_0842 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_20_data_0842; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_20_data_0842 IS 'Data storage for tripal_entity field bio_data_20_data_0842.';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_0842.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_0842.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_0842.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_0842.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_0842.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_0842.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_0842.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_0842.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_20_data_1249; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_20_data_1249 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_20_data_1249_value integer,
    bio_data_20_data_1249_record_id integer,
    CONSTRAINT tripal_entity__bio_data_20_data_1249_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_20_data_1249_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_20_data_1249_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_20_data_1249 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_20_data_1249; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_20_data_1249 IS 'Data storage for tripal_entity field bio_data_20_data_1249.';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_1249.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_1249.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_1249.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_1249.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_1249.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_1249.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_1249.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_1249.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_1249.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_1249.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_1249.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_1249.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_20_data_2044; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_20_data_2044 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_20_data_2044_value text,
    bio_data_20_data_2044_record_id integer,
    CONSTRAINT tripal_entity__bio_data_20_data_2044_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_20_data_2044_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_20_data_2044_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_20_data_2044 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_20_data_2044; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_20_data_2044 IS 'Data storage for tripal_entity field bio_data_20_data_2044.';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_2044.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_2044.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_2044.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_2044.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_2044.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_2044.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_2044.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_2044.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_2044.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_2044.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_20_data_2044.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_data_2044.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_20_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_20_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_20_schema_name_value character varying(255),
    bio_data_20_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_20_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_20_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_20_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_20_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_20_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_20_schema_name IS 'Data storage for tripal_entity field bio_data_20_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_20_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_20_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_20_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_20_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_20_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_20_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_20_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_21_data_0842; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_21_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_21_data_0842_value text,
    bio_data_21_data_0842_record_id integer,
    CONSTRAINT tripal_entity__bio_data_21_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_21_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_21_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_21_data_0842 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_21_data_0842; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_21_data_0842 IS 'Data storage for tripal_entity field bio_data_21_data_0842.';


--
-- Name: COLUMN tripal_entity__bio_data_21_data_0842.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_21_data_0842.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_21_data_0842.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_data_0842.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_21_data_0842.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_21_data_0842.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_data_0842.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_21_data_0842.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_21_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_21_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_21_schema_description_value text,
    bio_data_21_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_21_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_21_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_21_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_21_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_21_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_21_schema_description IS 'Data storage for tripal_entity field bio_data_21_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_21_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_21_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_21_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_21_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_21_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_21_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_21_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_21_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_21_schema_name_value character varying(255),
    bio_data_21_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_21_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_21_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_21_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_21_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_21_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_21_schema_name IS 'Data storage for tripal_entity field bio_data_21_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_21_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_21_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_21_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_21_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_21_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_21_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_21_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_22_data_0842; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_22_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_22_data_0842_value text,
    bio_data_22_data_0842_record_id integer,
    CONSTRAINT tripal_entity__bio_data_22_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_22_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_22_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_22_data_0842 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_22_data_0842; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_22_data_0842 IS 'Data storage for tripal_entity field bio_data_22_data_0842.';


--
-- Name: COLUMN tripal_entity__bio_data_22_data_0842.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_22_data_0842.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_22_data_0842.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_data_0842.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_22_data_0842.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_22_data_0842.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_data_0842.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_22_data_0842.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_22_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_22_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_22_schema_description_value text,
    bio_data_22_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_22_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_22_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_22_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_22_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_22_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_22_schema_description IS 'Data storage for tripal_entity field bio_data_22_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_22_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_22_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_22_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_22_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_22_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_22_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_22_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_22_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_22_schema_name_value character varying(255),
    bio_data_22_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_22_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_22_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_22_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_22_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_22_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_22_schema_name IS 'Data storage for tripal_entity field bio_data_22_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_22_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_22_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_22_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_22_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_22_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_22_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_22_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_23_data_0842; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_23_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_23_data_0842_value text,
    bio_data_23_data_0842_record_id integer,
    CONSTRAINT tripal_entity__bio_data_23_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_23_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_23_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_23_data_0842 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_23_data_0842; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_23_data_0842 IS 'Data storage for tripal_entity field bio_data_23_data_0842.';


--
-- Name: COLUMN tripal_entity__bio_data_23_data_0842.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_23_data_0842.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_23_data_0842.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_data_0842.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_23_data_0842.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_23_data_0842.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_data_0842.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_23_data_0842.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_23_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_23_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_23_schema_description_value text,
    bio_data_23_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_23_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_23_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_23_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_23_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_23_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_23_schema_description IS 'Data storage for tripal_entity field bio_data_23_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_23_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_23_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_23_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_23_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_23_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_23_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_23_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_23_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_23_schema_name_value character varying(255),
    bio_data_23_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_23_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_23_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_23_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_23_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_23_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_23_schema_name IS 'Data storage for tripal_entity field bio_data_23_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_23_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_23_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_23_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_23_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_23_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_23_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_23_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_24_data_0842; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_24_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_24_data_0842_value text,
    bio_data_24_data_0842_record_id integer,
    CONSTRAINT tripal_entity__bio_data_24_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_24_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_24_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_24_data_0842 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_24_data_0842; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_24_data_0842 IS 'Data storage for tripal_entity field bio_data_24_data_0842.';


--
-- Name: COLUMN tripal_entity__bio_data_24_data_0842.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_24_data_0842.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_24_data_0842.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_data_0842.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_24_data_0842.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_24_data_0842.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_data_0842.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_24_data_0842.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_24_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_24_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_24_schema_description_value text,
    bio_data_24_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_24_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_24_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_24_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_24_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_24_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_24_schema_description IS 'Data storage for tripal_entity field bio_data_24_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_24_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_24_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_24_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_24_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_24_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_24_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_24_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_24_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_24_schema_name_value character varying(255),
    bio_data_24_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_24_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_24_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_24_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_24_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_24_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_24_schema_name IS 'Data storage for tripal_entity field bio_data_24_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_24_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_24_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_24_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_24_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_24_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_24_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_24_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_25_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_25_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_25_schema_description_value text,
    bio_data_25_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_25_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_25_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_25_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_25_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_25_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_25_schema_description IS 'Data storage for tripal_entity field bio_data_25_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_25_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_25_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_25_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_25_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_25_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_25_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_25_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_25_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_25_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_25_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_25_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_25_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_25_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_25_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_25_schema_name_value text,
    bio_data_25_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_25_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_25_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_25_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_25_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_25_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_25_schema_name IS 'Data storage for tripal_entity field bio_data_25_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_25_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_25_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_25_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_25_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_25_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_25_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_25_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_25_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_25_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_25_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_25_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_25_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_27_iao_0000129; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_27_iao_0000129 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_27_iao_0000129_value text,
    bio_data_27_iao_0000129_record_id integer,
    CONSTRAINT tripal_entity__bio_data_27_iao_0000129_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_27_iao_0000129_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_27_iao_0000129_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_27_iao_0000129 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_27_iao_0000129; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_27_iao_0000129 IS 'Data storage for tripal_entity field bio_data_27_iao_0000129.';


--
-- Name: COLUMN tripal_entity__bio_data_27_iao_0000129.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_iao_0000129.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_27_iao_0000129.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_iao_0000129.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_27_iao_0000129.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_iao_0000129.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_27_iao_0000129.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_iao_0000129.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_27_iao_0000129.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_iao_0000129.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_27_iao_0000129.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_iao_0000129.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_27_local_array_dimensio; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_27_local_array_dimensio (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_27_local_array_dimensio_value text,
    bio_data_27_local_array_dimensio_record_id integer,
    CONSTRAINT tripal_entity__bio_data_27_local_array_dimens_revision_id_check CHECK ((revision_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_27_local_array_dimensio_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_27_local_array_dimensio_entity_id_check CHECK ((entity_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_27_local_array_dimensio OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_27_local_array_dimensio; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_27_local_array_dimensio IS 'Data storage for tripal_entity field bio_data_27_local_array_dimensio.';


--
-- Name: COLUMN tripal_entity__bio_data_27_local_array_dimensio.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_local_array_dimensio.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_27_local_array_dimensio.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_local_array_dimensio.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_27_local_array_dimensio.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_local_array_dimensio.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_27_local_array_dimensio.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_local_array_dimensio.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_27_local_array_dimensio.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_local_array_dimensio.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_27_local_array_dimensio.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_local_array_dimensio.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_27_local_element_dimens; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_27_local_element_dimens (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_27_local_element_dimens_value text,
    bio_data_27_local_element_dimens_record_id integer,
    CONSTRAINT tripal_entity__bio_data_27_local_element_dime_revision_id_check CHECK ((revision_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_27_local_element_dimens_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_27_local_element_dimens_entity_id_check CHECK ((entity_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_27_local_element_dimens OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_27_local_element_dimens; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_27_local_element_dimens IS 'Data storage for tripal_entity field bio_data_27_local_element_dimens.';


--
-- Name: COLUMN tripal_entity__bio_data_27_local_element_dimens.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_local_element_dimens.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_27_local_element_dimens.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_local_element_dimens.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_27_local_element_dimens.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_local_element_dimens.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_27_local_element_dimens.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_local_element_dimens.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_27_local_element_dimens.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_local_element_dimens.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_27_local_element_dimens.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_local_element_dimens.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_27_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_27_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_27_schema_description_value text,
    bio_data_27_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_27_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_27_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_27_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_27_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_27_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_27_schema_description IS 'Data storage for tripal_entity field bio_data_27_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_27_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_27_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_27_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_27_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_27_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_27_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_27_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_27_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_27_schema_name_value text,
    bio_data_27_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_27_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_27_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_27_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_27_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_27_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_27_schema_name IS 'Data storage for tripal_entity field bio_data_27_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_27_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_27_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_27_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_27_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_27_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_27_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_27_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_2_data_1047; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_2_data_1047 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_2_data_1047_value text,
    bio_data_2_data_1047_record_id integer,
    CONSTRAINT tripal_entity__bio_data_2_data_1047_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_2_data_1047_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_2_data_1047_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_2_data_1047 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_2_data_1047; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_2_data_1047 IS 'Data storage for tripal_entity field bio_data_2_data_1047.';


--
-- Name: COLUMN tripal_entity__bio_data_2_data_1047.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_data_1047.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_2_data_1047.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_data_1047.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_2_data_1047.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_data_1047.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_2_data_1047.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_data_1047.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_2_data_1047.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_data_1047.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_2_data_1047.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_data_1047.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_2_iao_0000064; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_2_iao_0000064 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_2_iao_0000064_value character varying(255),
    bio_data_2_iao_0000064_record_id integer,
    CONSTRAINT tripal_entity__bio_data_2_iao_0000064_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_2_iao_0000064_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_2_iao_0000064_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_2_iao_0000064 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_2_iao_0000064; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_2_iao_0000064 IS 'Data storage for tripal_entity field bio_data_2_iao_0000064.';


--
-- Name: COLUMN tripal_entity__bio_data_2_iao_0000064.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_iao_0000064.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_2_iao_0000064.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_iao_0000064.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_2_iao_0000064.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_iao_0000064.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_2_iao_0000064.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_iao_0000064.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_2_iao_0000064.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_iao_0000064.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_2_iao_0000064.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_iao_0000064.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_2_iao_0000129; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_2_iao_0000129 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_2_iao_0000129_value character varying(255),
    bio_data_2_iao_0000129_record_id integer,
    CONSTRAINT tripal_entity__bio_data_2_iao_0000129_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_2_iao_0000129_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_2_iao_0000129_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_2_iao_0000129 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_2_iao_0000129; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_2_iao_0000129 IS 'Data storage for tripal_entity field bio_data_2_iao_0000129.';


--
-- Name: COLUMN tripal_entity__bio_data_2_iao_0000129.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_iao_0000129.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_2_iao_0000129.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_iao_0000129.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_2_iao_0000129.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_iao_0000129.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_2_iao_0000129.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_iao_0000129.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_2_iao_0000129.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_iao_0000129.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_2_iao_0000129.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_iao_0000129.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_2_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_2_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_2_schema_description_value text,
    bio_data_2_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_2_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_2_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_2_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_2_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_2_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_2_schema_description IS 'Data storage for tripal_entity field bio_data_2_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_2_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_2_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_2_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_2_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_2_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_2_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_2_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_2_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_2_schema_name_value character varying(255),
    bio_data_2_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_2_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_2_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_2_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_2_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_2_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_2_schema_name IS 'Data storage for tripal_entity field bio_data_2_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_2_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_2_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_2_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_2_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_2_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_2_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_2_swo_0000001; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_2_swo_0000001 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_2_swo_0000001_value character varying(255),
    bio_data_2_swo_0000001_record_id integer,
    CONSTRAINT tripal_entity__bio_data_2_swo_0000001_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_2_swo_0000001_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_2_swo_0000001_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_2_swo_0000001 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_2_swo_0000001; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_2_swo_0000001 IS 'Data storage for tripal_entity field bio_data_2_swo_0000001.';


--
-- Name: COLUMN tripal_entity__bio_data_2_swo_0000001.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_swo_0000001.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_2_swo_0000001.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_swo_0000001.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_2_swo_0000001.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_swo_0000001.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_2_swo_0000001.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_swo_0000001.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_2_swo_0000001.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_swo_0000001.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_2_swo_0000001.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_2_swo_0000001.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_3_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_3_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_3_schema_description_value text,
    bio_data_3_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_3_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_3_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_3_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_3_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_3_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_3_schema_description IS 'Data storage for tripal_entity field bio_data_3_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_3_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_3_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_3_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_3_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_3_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_3_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_3_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_3_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_3_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_3_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_3_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_3_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_3_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_3_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_3_schema_name_value character varying(255),
    bio_data_3_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_3_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_3_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_3_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_3_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_3_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_3_schema_name IS 'Data storage for tripal_entity field bio_data_3_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_3_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_3_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_3_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_3_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_3_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_3_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_3_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_3_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_3_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_3_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_3_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_3_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_4_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_4_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_4_schema_description_value text,
    bio_data_4_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_4_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_4_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_4_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_4_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_4_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_4_schema_description IS 'Data storage for tripal_entity field bio_data_4_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_4_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_4_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_4_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_4_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_4_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_4_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_4_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_4_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_4_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_4_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_4_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_4_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_4_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_4_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_4_schema_name_value text,
    bio_data_4_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_4_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_4_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_4_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_4_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_4_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_4_schema_name IS 'Data storage for tripal_entity field bio_data_4_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_4_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_4_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_4_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_4_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_4_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_4_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_4_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_4_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_4_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_4_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_4_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_4_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_5_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_5_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_5_schema_description_value character varying(255),
    bio_data_5_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_5_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_5_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_5_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_5_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_5_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_5_schema_description IS 'Data storage for tripal_entity field bio_data_5_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_5_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_5_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_5_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_5_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_5_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_5_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_5_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_5_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_5_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_5_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_5_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_5_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_5_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_5_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_5_schema_name_value character varying(255),
    bio_data_5_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_5_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_5_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_5_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_5_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_5_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_5_schema_name IS 'Data storage for tripal_entity field bio_data_5_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_5_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_5_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_5_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_5_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_5_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_5_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_5_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_5_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_5_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_5_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_5_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_5_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_7_data_1047; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_7_data_1047 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_7_data_1047_value text,
    bio_data_7_data_1047_record_id integer,
    CONSTRAINT tripal_entity__bio_data_7_data_1047_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_7_data_1047_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_7_data_1047_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_7_data_1047 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_7_data_1047; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_7_data_1047 IS 'Data storage for tripal_entity field bio_data_7_data_1047.';


--
-- Name: COLUMN tripal_entity__bio_data_7_data_1047.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_data_1047.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_7_data_1047.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_data_1047.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_7_data_1047.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_data_1047.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_7_data_1047.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_data_1047.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_7_data_1047.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_data_1047.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_7_data_1047.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_data_1047.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_7_efo_0000548; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_7_efo_0000548 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_7_efo_0000548_value text,
    bio_data_7_efo_0000548_record_id integer,
    CONSTRAINT tripal_entity__bio_data_7_efo_0000548_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_7_efo_0000548_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_7_efo_0000548_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_7_efo_0000548 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_7_efo_0000548; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_7_efo_0000548 IS 'Data storage for tripal_entity field bio_data_7_efo_0000548.';


--
-- Name: COLUMN tripal_entity__bio_data_7_efo_0000548.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_efo_0000548.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_7_efo_0000548.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_efo_0000548.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_7_efo_0000548.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_efo_0000548.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_7_efo_0000548.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_efo_0000548.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_7_efo_0000548.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_efo_0000548.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_7_efo_0000548.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_efo_0000548.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_7_schema_description; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_7_schema_description (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_7_schema_description_value text,
    bio_data_7_schema_description_record_id integer,
    CONSTRAINT tripal_entity__bio_data_7_schema_description_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_7_schema_description_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_7_schema_description_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_7_schema_description OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_7_schema_description; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_7_schema_description IS 'Data storage for tripal_entity field bio_data_7_schema_description.';


--
-- Name: COLUMN tripal_entity__bio_data_7_schema_description.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_schema_description.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_7_schema_description.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_schema_description.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_7_schema_description.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_schema_description.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_7_schema_description.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_schema_description.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_7_schema_description.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_schema_description.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_7_schema_description.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_schema_description.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_7_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_7_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_7_schema_name_value text,
    bio_data_7_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_7_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_7_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_7_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_7_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_7_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_7_schema_name IS 'Data storage for tripal_entity field bio_data_7_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_7_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_7_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_7_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_7_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_7_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_7_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_7_swo_0000001; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_7_swo_0000001 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_7_swo_0000001_value text,
    bio_data_7_swo_0000001_record_id integer,
    CONSTRAINT tripal_entity__bio_data_7_swo_0000001_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_7_swo_0000001_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_7_swo_0000001_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_7_swo_0000001 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_7_swo_0000001; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_7_swo_0000001 IS 'Data storage for tripal_entity field bio_data_7_swo_0000001.';


--
-- Name: COLUMN tripal_entity__bio_data_7_swo_0000001.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_swo_0000001.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_7_swo_0000001.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_swo_0000001.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_7_swo_0000001.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_swo_0000001.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_7_swo_0000001.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_swo_0000001.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_7_swo_0000001.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_swo_0000001.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_7_swo_0000001.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_7_swo_0000001.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_8_data_0842; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_8_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_8_data_0842_value text,
    bio_data_8_data_0842_record_id integer,
    CONSTRAINT tripal_entity__bio_data_8_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_8_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_8_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_8_data_0842 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_8_data_0842; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_8_data_0842 IS 'Data storage for tripal_entity field bio_data_8_data_0842.';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_0842.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_0842.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_0842.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_0842.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_0842.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_0842.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_0842.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_0842.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_8_data_1249; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_8_data_1249 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_8_data_1249_value integer,
    bio_data_8_data_1249_record_id integer,
    CONSTRAINT tripal_entity__bio_data_8_data_1249_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_8_data_1249_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_8_data_1249_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_8_data_1249 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_8_data_1249; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_8_data_1249 IS 'Data storage for tripal_entity field bio_data_8_data_1249.';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_1249.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_1249.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_1249.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_1249.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_1249.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_1249.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_1249.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_1249.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_1249.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_1249.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_1249.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_1249.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_8_data_2044; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_8_data_2044 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_8_data_2044_value text,
    bio_data_8_data_2044_record_id integer,
    CONSTRAINT tripal_entity__bio_data_8_data_2044_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_8_data_2044_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_8_data_2044_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_8_data_2044 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_8_data_2044; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_8_data_2044 IS 'Data storage for tripal_entity field bio_data_8_data_2044.';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_2044.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_2044.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_2044.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_2044.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_2044.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_2044.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_2044.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_2044.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_2044.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_2044.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_8_data_2044.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_data_2044.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_8_obi_0100026; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_8_obi_0100026 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_8_obi_0100026_value integer,
    bio_data_8_obi_0100026_rdfs_label character varying(2558),
    "bio_data_8_obi_0100026_TAXRANK_0000005" character varying(255),
    "bio_data_8_obi_0100026_TAXRANK_0000006" character varying(255),
    "bio_data_8_obi_0100026_TAXRANK_0000045" character varying(1024),
    bio_data_8_obi_0100026_local_infraspecific_type integer,
    bio_data_8_obi_0100026_record_id integer,
    CONSTRAINT tripal_entity__bio_data_8_obi_0100026_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_8_obi_0100026_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_8_obi_0100026_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_8_obi_0100026 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_8_obi_0100026; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_8_obi_0100026 IS 'Data storage for tripal_entity field bio_data_8_obi_0100026.';


--
-- Name: COLUMN tripal_entity__bio_data_8_obi_0100026.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_obi_0100026.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_8_obi_0100026.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_obi_0100026.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_8_obi_0100026.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_obi_0100026.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_8_obi_0100026.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_obi_0100026.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_8_obi_0100026.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_obi_0100026.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_8_obi_0100026.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_obi_0100026.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_8_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_8_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_8_schema_name_value character varying(255),
    bio_data_8_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_8_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_8_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_8_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_8_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_8_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_8_schema_name IS 'Data storage for tripal_entity field bio_data_8_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_8_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_8_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_8_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_8_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_8_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_8_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_8_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_9_data_0842; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_9_data_0842 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_9_data_0842_value text,
    bio_data_9_data_0842_record_id integer,
    CONSTRAINT tripal_entity__bio_data_9_data_0842_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_9_data_0842_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_9_data_0842_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_9_data_0842 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_9_data_0842; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_9_data_0842 IS 'Data storage for tripal_entity field bio_data_9_data_0842.';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_0842.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_0842.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_0842.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_0842.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_0842.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_0842.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_0842.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_0842.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_0842.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_0842.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_0842.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_0842.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_9_data_1249; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_9_data_1249 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_9_data_1249_value integer,
    bio_data_9_data_1249_record_id integer,
    CONSTRAINT tripal_entity__bio_data_9_data_1249_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_9_data_1249_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_9_data_1249_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_9_data_1249 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_9_data_1249; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_9_data_1249 IS 'Data storage for tripal_entity field bio_data_9_data_1249.';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_1249.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_1249.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_1249.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_1249.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_1249.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_1249.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_1249.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_1249.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_1249.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_1249.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_1249.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_1249.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_9_data_2044; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_9_data_2044 (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_9_data_2044_value text,
    bio_data_9_data_2044_record_id integer,
    CONSTRAINT tripal_entity__bio_data_9_data_2044_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_9_data_2044_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_9_data_2044_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_9_data_2044 OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_9_data_2044; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_9_data_2044 IS 'Data storage for tripal_entity field bio_data_9_data_2044.';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_2044.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_2044.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_2044.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_2044.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_2044.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_2044.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_2044.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_2044.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_2044.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_2044.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_9_data_2044.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_data_2044.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__bio_data_9_schema_name; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__bio_data_9_schema_name (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    bio_data_9_schema_name_value character varying(255),
    bio_data_9_schema_name_record_id integer,
    CONSTRAINT tripal_entity__bio_data_9_schema_name_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__bio_data_9_schema_name_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__bio_data_9_schema_name_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__bio_data_9_schema_name OWNER TO drupal;

--
-- Name: TABLE tripal_entity__bio_data_9_schema_name; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__bio_data_9_schema_name IS 'Data storage for tripal_entity field bio_data_9_schema_name.';


--
-- Name: COLUMN tripal_entity__bio_data_9_schema_name.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_schema_name.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__bio_data_9_schema_name.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_schema_name.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__bio_data_9_schema_name.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_schema_name.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__bio_data_9_schema_name.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_schema_name.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__bio_data_9_schema_name.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_schema_name.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__bio_data_9_schema_name.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__bio_data_9_schema_name.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity__schema__additionaltype; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_entity__schema__additionaltype (
    bundle character varying(128) DEFAULT ''::character varying NOT NULL,
    deleted smallint DEFAULT 0 NOT NULL,
    entity_id bigint NOT NULL,
    revision_id bigint NOT NULL,
    langcode character varying(32) DEFAULT ''::character varying NOT NULL,
    delta bigint NOT NULL,
    schema__additionaltype_value integer,
    schema__additionaltype_record_id integer,
    CONSTRAINT tripal_entity__schema__additionaltype_delta_check CHECK ((delta >= 0)),
    CONSTRAINT tripal_entity__schema__additionaltype_entity_id_check CHECK ((entity_id >= 0)),
    CONSTRAINT tripal_entity__schema__additionaltype_revision_id_check CHECK ((revision_id >= 0))
);


ALTER TABLE public.tripal_entity__schema__additionaltype OWNER TO drupal;

--
-- Name: TABLE tripal_entity__schema__additionaltype; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_entity__schema__additionaltype IS 'Data storage for tripal_entity field schema__additionaltype.';


--
-- Name: COLUMN tripal_entity__schema__additionaltype.bundle; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__schema__additionaltype.bundle IS 'The field instance bundle to which this row belongs, used when deleting a field instance';


--
-- Name: COLUMN tripal_entity__schema__additionaltype.deleted; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__schema__additionaltype.deleted IS 'A boolean indicating whether this data item has been deleted';


--
-- Name: COLUMN tripal_entity__schema__additionaltype.entity_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__schema__additionaltype.entity_id IS 'The entity id this data is attached to';


--
-- Name: COLUMN tripal_entity__schema__additionaltype.revision_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__schema__additionaltype.revision_id IS 'The entity revision id this data is attached to, which for an unversioned entity type is the same as the entity id';


--
-- Name: COLUMN tripal_entity__schema__additionaltype.langcode; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__schema__additionaltype.langcode IS 'The language code for this data item.';


--
-- Name: COLUMN tripal_entity__schema__additionaltype.delta; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_entity__schema__additionaltype.delta IS 'The sequence number for this data item, used for multi-value fields';


--
-- Name: tripal_entity_id_seq; Type: SEQUENCE; Schema: public; Owner: drupal
--

CREATE SEQUENCE public.tripal_entity_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_entity_id_seq OWNER TO drupal;

--
-- Name: tripal_entity_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: drupal
--

ALTER SEQUENCE public.tripal_entity_id_seq OWNED BY public.tripal_entity.id;


--
-- Name: tripal_expiration_files; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_expiration_files (
    fid integer NOT NULL,
    expiration_date bigint NOT NULL
);


ALTER TABLE public.tripal_expiration_files OWNER TO drupal;

--
-- Name: tripal_id_space_collection; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_id_space_collection (
    name character varying(255) NOT NULL,
    plugin_id character varying(255) NOT NULL
);


ALTER TABLE public.tripal_id_space_collection OWNER TO drupal;

--
-- Name: tripal_import; Type: TABLE; Schema: public; Owner: drupal
--

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

--
-- Name: COLUMN tripal_import.uid; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_import.uid IS 'The Drupal userid of the submitee.';


--
-- Name: COLUMN tripal_import.fid; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_import.fid IS 'The file IDs of the to import. This only applies if the file was uploaded (i.e. not already on the server) and is mangaged by Drupal. Multiple fids are separated using a | character.';


--
-- Name: COLUMN tripal_import.arguments; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_import.arguments IS 'Holds a serialized PHP array containing the key/value paris that are used for arguments of the job.';


--
-- Name: COLUMN tripal_import.submit_date; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_import.submit_date IS 'UNIX integer submit time';


--
-- Name: tripal_import_import_id_seq; Type: SEQUENCE; Schema: public; Owner: drupal
--

CREATE SEQUENCE public.tripal_import_import_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_import_import_id_seq OWNER TO drupal;

--
-- Name: tripal_import_import_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: drupal
--

ALTER SEQUENCE public.tripal_import_import_id_seq OWNED BY public.tripal_import.import_id;


--
-- Name: tripal_jobs; Type: TABLE; Schema: public; Owner: drupal
--

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

--
-- Name: COLUMN tripal_jobs.uid; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_jobs.uid IS 'The Drupal userid of the submitee';


--
-- Name: COLUMN tripal_jobs.modulename; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_jobs.modulename IS 'The module name that provides the callback for this job';


--
-- Name: COLUMN tripal_jobs.progress; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_jobs.progress IS 'a value from 0 to 100 indicating percent complete';


--
-- Name: COLUMN tripal_jobs.submit_date; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_jobs.submit_date IS 'UNIX integer submit time';


--
-- Name: COLUMN tripal_jobs.start_time; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_jobs.start_time IS 'UNIX integer start time';


--
-- Name: COLUMN tripal_jobs.end_time; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_jobs.end_time IS 'UNIX integer end time';


--
-- Name: COLUMN tripal_jobs.pid; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_jobs.pid IS 'The process id for the job';


--
-- Name: COLUMN tripal_jobs.priority; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_jobs.priority IS 'The job priority';


--
-- Name: COLUMN tripal_jobs.mlock; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_jobs.mlock IS 'If set to 1 then all jobs for the module are held until this one finishes';


--
-- Name: COLUMN tripal_jobs.lock; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_jobs.lock IS 'If set to 1 then all jobs are held until this one finishes';


--
-- Name: COLUMN tripal_jobs.includes; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_jobs.includes IS 'A serialized array of file paths that should be included prior to executing the job.';


--
-- Name: tripal_jobs_job_id_seq; Type: SEQUENCE; Schema: public; Owner: drupal
--

CREATE SEQUENCE public.tripal_jobs_job_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_jobs_job_id_seq OWNER TO drupal;

--
-- Name: tripal_jobs_job_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: drupal
--

ALTER SEQUENCE public.tripal_jobs_job_id_seq OWNED BY public.tripal_jobs.job_id;


--
-- Name: tripal_mviews; Type: TABLE; Schema: public; Owner: drupal
--

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

--
-- Name: COLUMN tripal_mviews.table_id; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_mviews.table_id IS 'The custom table ID';


--
-- Name: COLUMN tripal_mviews.last_update; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON COLUMN public.tripal_mviews.last_update IS 'UNIX integer time';


--
-- Name: tripal_mviews_mview_id_seq; Type: SEQUENCE; Schema: public; Owner: drupal
--

CREATE SEQUENCE public.tripal_mviews_mview_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_mviews_mview_id_seq OWNER TO drupal;

--
-- Name: tripal_mviews_mview_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: drupal
--

ALTER SEQUENCE public.tripal_mviews_mview_id_seq OWNED BY public.tripal_mviews.mview_id;


--
-- Name: tripal_token_formats; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_token_formats (
    tripal_format_id integer NOT NULL,
    content_type character varying(255) NOT NULL,
    application character varying(255) NOT NULL,
    format text NOT NULL,
    tokens text NOT NULL,
    CONSTRAINT tripal_token_formats_tripal_format_id_check CHECK ((tripal_format_id >= 0))
);


ALTER TABLE public.tripal_token_formats OWNER TO drupal;

--
-- Name: tripal_token_formats_tripal_format_id_seq; Type: SEQUENCE; Schema: public; Owner: drupal
--

CREATE SEQUENCE public.tripal_token_formats_tripal_format_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_token_formats_tripal_format_id_seq OWNER TO drupal;

--
-- Name: tripal_token_formats_tripal_format_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: drupal
--

ALTER SEQUENCE public.tripal_token_formats_tripal_format_id_seq OWNED BY public.tripal_token_formats.tripal_format_id;


--
-- Name: tripal_variables; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_variables (
    variable_id integer NOT NULL,
    name character varying(255) NOT NULL,
    description text NOT NULL
);


ALTER TABLE public.tripal_variables OWNER TO drupal;

--
-- Name: TABLE tripal_variables; Type: COMMENT; Schema: public; Owner: drupal
--

COMMENT ON TABLE public.tripal_variables IS 'This table houses a list of unique variable names that can be used in the tripal_node_variables table.';


--
-- Name: tripal_variables_variable_id_seq; Type: SEQUENCE; Schema: public; Owner: drupal
--

CREATE SEQUENCE public.tripal_variables_variable_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.tripal_variables_variable_id_seq OWNER TO drupal;

--
-- Name: tripal_variables_variable_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: drupal
--

ALTER SEQUENCE public.tripal_variables_variable_id_seq OWNED BY public.tripal_variables.variable_id;


--
-- Name: tripal_vocabulary_collection; Type: TABLE; Schema: public; Owner: drupal
--

CREATE TABLE public.tripal_vocabulary_collection (
    name character varying(255) NOT NULL,
    plugin_id character varying(255) NOT NULL
);


ALTER TABLE public.tripal_vocabulary_collection OWNER TO drupal;
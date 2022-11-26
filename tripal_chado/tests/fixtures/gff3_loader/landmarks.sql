--
-- PostgreSQL database dump
--

-- Dumped from database version 10.22
-- Dumped by pg_dump version 10.22

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

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: feature; Type: TABLE; Schema: chado; Owner: drupal
--

CREATE TABLE chado.feature (
    feature_id bigint NOT NULL,
    dbxref_id bigint,
    organism_id bigint NOT NULL,
    name character varying(255),
    uniquename text NOT NULL,
    residues text,
    seqlen bigint,
    md5checksum character(32),
    type_id bigint NOT NULL,
    is_analysis boolean DEFAULT false NOT NULL,
    is_obsolete boolean DEFAULT false NOT NULL,
    timeaccessioned timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    timelastmodified timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);
ALTER TABLE ONLY chado.feature ALTER COLUMN residues SET STORAGE EXTERNAL;


ALTER TABLE chado.feature OWNER TO drupal;

--
-- Name: TABLE feature; Type: COMMENT; Schema: chado; Owner: drupal
--

COMMENT ON TABLE chado.feature IS 'A feature is a biological sequence or a
section of a biological sequence, or a collection of such
sections. Examples include genes, exons, transcripts, regulatory
regions, polypeptides, protein domains, chromosome sequences, sequence
variations, cross-genome match regions such as hits and HSPs and so
on; see the Sequence Ontology for more. The combination of
organism_id, uniquename and type_id should be unique.';


--
-- Name: COLUMN feature.dbxref_id; Type: COMMENT; Schema: chado; Owner: drupal
--

COMMENT ON COLUMN chado.feature.dbxref_id IS 'An optional primary public stable
identifier for this feature. Secondary identifiers and external
dbxrefs go in the table feature_dbxref.';


--
-- Name: COLUMN feature.organism_id; Type: COMMENT; Schema: chado; Owner: drupal
--

COMMENT ON COLUMN chado.feature.organism_id IS 'The organism to which this feature
belongs. This column is mandatory.';


--
-- Name: COLUMN feature.name; Type: COMMENT; Schema: chado; Owner: drupal
--

COMMENT ON COLUMN chado.feature.name IS 'The optional human-readable common name for
a feature, for display purposes.';


--
-- Name: COLUMN feature.uniquename; Type: COMMENT; Schema: chado; Owner: drupal
--

COMMENT ON COLUMN chado.feature.uniquename IS 'The unique name for a feature; may
not be necessarily be particularly human-readable, although this is
preferred. This name must be unique for this type of feature within
this organism.';


--
-- Name: COLUMN feature.residues; Type: COMMENT; Schema: chado; Owner: drupal
--

COMMENT ON COLUMN chado.feature.residues IS 'A sequence of alphabetic characters
representing biological residues (nucleic acids, amino acids). This
column does not need to be manifested for all features; it is optional
for features such as exons where the residues can be derived from the
featureloc. It is recommended that the value for this column be
manifested for features which may may non-contiguous sublocations (e.g.
transcripts), since derivation at query time is non-trivial. For
expressed sequence, the DNA sequence should be used rather than the
RNA sequence. The default storage method for the residues column is
EXTERNAL, which will store it uncompressed to make substring operations
faster.';


--
-- Name: COLUMN feature.seqlen; Type: COMMENT; Schema: chado; Owner: drupal
--

COMMENT ON COLUMN chado.feature.seqlen IS 'The length of the residue feature. See
column:residues. This column is partially redundant with the residues
column, and also with featureloc. This column is required because the
location may be unknown and the residue sequence may not be
manifested, yet it may be desirable to store and query the length of
the feature. The seqlen should always be manifested where the length
of the sequence is known.';


--
-- Name: COLUMN feature.md5checksum; Type: COMMENT; Schema: chado; Owner: drupal
--

COMMENT ON COLUMN chado.feature.md5checksum IS 'The 32-character checksum of the sequence,
calculated using the MD5 algorithm. This is practically guaranteed to
be unique for any feature. This column thus acts as a unique
identifier on the mathematical sequence.';


--
-- Name: COLUMN feature.type_id; Type: COMMENT; Schema: chado; Owner: drupal
--

COMMENT ON COLUMN chado.feature.type_id IS 'A required reference to a table:cvterm
giving the feature type. This will typically be a Sequence Ontology
identifier. This column is thus used to subclass the feature table.';


--
-- Name: COLUMN feature.is_analysis; Type: COMMENT; Schema: chado; Owner: drupal
--

COMMENT ON COLUMN chado.feature.is_analysis IS 'Boolean indicating whether this
feature is annotated or the result of an automated analysis. Analysis
results also use the companalysis module. Note that the dividing line
between analysis and annotation may be fuzzy, this should be determined on
a per-project basis in a consistent manner. One requirement is that
there should only be one non-analysis version of each wild-type gene
feature in a genome, whereas the same gene feature can be predicted
multiple times in different analyses.';


--
-- Name: COLUMN feature.is_obsolete; Type: COMMENT; Schema: chado; Owner: drupal
--

COMMENT ON COLUMN chado.feature.is_obsolete IS 'Boolean indicating whether this
feature has been obsoleted. Some chado instances may choose to simply
remove the feature altogether, others may choose to keep an obsolete
row in the table.';


--
-- Name: COLUMN feature.timeaccessioned; Type: COMMENT; Schema: chado; Owner: drupal
--

COMMENT ON COLUMN chado.feature.timeaccessioned IS 'For handling object
accession or modification timestamps (as opposed to database auditing data,
handled elsewhere). The expectation is that these fields would be
available to software interacting with chado.';


--
-- Name: COLUMN feature.timelastmodified; Type: COMMENT; Schema: chado; Owner: drupal
--

COMMENT ON COLUMN chado.feature.timelastmodified IS 'For handling object
accession or modification timestamps (as opposed to database auditing data,
handled elsewhere). The expectation is that these fields would be
available to software interacting with chado.';


--
-- Name: feature_feature_id_seq; Type: SEQUENCE; Schema: chado; Owner: drupal
--

CREATE SEQUENCE chado.feature_feature_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE chado.feature_feature_id_seq OWNER TO drupal;

--
-- Name: feature_feature_id_seq; Type: SEQUENCE OWNED BY; Schema: chado; Owner: drupal
--

ALTER SEQUENCE chado.feature_feature_id_seq OWNED BY chado.feature.feature_id;


--
-- Name: feature feature_id; Type: DEFAULT; Schema: chado; Owner: drupal
--

ALTER TABLE ONLY chado.feature ALTER COLUMN feature_id SET DEFAULT nextval('chado.feature_feature_id_seq'::regclass);


--
-- Data for Name: feature; Type: TABLE DATA; Schema: chado; Owner: drupal
--

INSERT INTO chado.feature VALUES (1, NULL, 1, 'Contig10036', 'Contig10036', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:39:55.810798', '2022-11-26 05:39:55.810798');
INSERT INTO chado.feature VALUES (2, NULL, 1, 'Contig1', 'Contig1', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:39:57.335594', '2022-11-26 05:39:57.335594');
INSERT INTO chado.feature VALUES (3, NULL, 1, 'Contig0', 'Contig0', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:39:59.809424', '2022-11-26 05:39:59.809424');
INSERT INTO chado.feature VALUES (4, NULL, 1, 'Contig100', 'Contig100', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:40:01.720026', '2022-11-26 05:40:01.720026');
INSERT INTO chado.feature VALUES (5, NULL, 1, 'Contig10022', 'Contig10022', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:40:03.67779', '2022-11-26 05:40:03.67779');
INSERT INTO chado.feature VALUES (6, NULL, 1, 'Contig10023', 'Contig10023', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:40:06.476017', '2022-11-26 05:40:06.476017');
INSERT INTO chado.feature VALUES (7, NULL, 1, 'Contig10035', 'Contig10035', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:13.941346', '2022-11-26 05:42:13.941346');
INSERT INTO chado.feature VALUES (8, NULL, 1, 'Contig1001', 'Contig1001', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:15.88952', '2022-11-26 05:42:15.88952');
INSERT INTO chado.feature VALUES (9, NULL, 1, 'Contig10012', 'Contig10012', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:17.575528', '2022-11-26 05:42:17.575528');
INSERT INTO chado.feature VALUES (10, NULL, 1, 'Contig1002', 'Contig1002', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:19.333469', '2022-11-26 05:42:19.333469');
INSERT INTO chado.feature VALUES (11, NULL, 1, 'Contig10026', 'Contig10026', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:20.99745', '2022-11-26 05:42:20.99745');
INSERT INTO chado.feature VALUES (12, NULL, 1, 'Contig10018', 'Contig10018', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:23.141713', '2022-11-26 05:42:23.141713');
INSERT INTO chado.feature VALUES (13, NULL, 1, 'Contig1003', 'Contig1003', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:24.721516', '2022-11-26 05:42:24.721516');
INSERT INTO chado.feature VALUES (14, NULL, 1, 'Contig10030', 'Contig10030', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:27.485629', '2022-11-26 05:42:27.485629');
INSERT INTO chado.feature VALUES (15, NULL, 1, 'Contig10', 'Contig10', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:29.815552', '2022-11-26 05:42:29.815552');
INSERT INTO chado.feature VALUES (16, NULL, 1, 'Contig10011', 'Contig10011', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:31.600361', '2022-11-26 05:42:31.600361');
INSERT INTO chado.feature VALUES (17, NULL, 1, 'Contig10005', 'Contig10005', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:34.168833', '2022-11-26 05:42:34.168833');
INSERT INTO chado.feature VALUES (18, NULL, 1, 'Contig10002', 'Contig10002', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:35.759462', '2022-11-26 05:42:35.759462');
INSERT INTO chado.feature VALUES (19, NULL, 1, 'Contig1000', 'Contig1000', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:37.428834', '2022-11-26 05:42:37.428834');
INSERT INTO chado.feature VALUES (20, NULL, 1, 'Contig10000', 'Contig10000', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:39.367891', '2022-11-26 05:42:39.367891');
INSERT INTO chado.feature VALUES (21, NULL, 1, '', '', '', 0, 'd41d8cd98f00b204e9800998ecf8427e', 474, false, false, '2022-11-26 05:42:42.145663', '2022-11-26 05:42:42.145663');


--
-- Name: feature_feature_id_seq; Type: SEQUENCE SET; Schema: chado; Owner: drupal
--

SELECT pg_catalog.setval('chado.feature_feature_id_seq', 21, true);


--
-- Name: feature feature_c1; Type: CONSTRAINT; Schema: chado; Owner: drupal
--

ALTER TABLE ONLY chado.feature
    ADD CONSTRAINT feature_c1 UNIQUE (organism_id, uniquename, type_id);


--
-- Name: feature feature_pkey; Type: CONSTRAINT; Schema: chado; Owner: drupal
--

ALTER TABLE ONLY chado.feature
    ADD CONSTRAINT feature_pkey PRIMARY KEY (feature_id);


--
-- Name: feature_idx1; Type: INDEX; Schema: chado; Owner: drupal
--

CREATE INDEX feature_idx1 ON chado.feature USING btree (dbxref_id);


--
-- Name: feature_idx1b; Type: INDEX; Schema: chado; Owner: drupal
--

CREATE INDEX feature_idx1b ON chado.feature USING btree (feature_id, dbxref_id) WHERE (dbxref_id IS NOT NULL);


--
-- Name: feature_idx2; Type: INDEX; Schema: chado; Owner: drupal
--

CREATE INDEX feature_idx2 ON chado.feature USING btree (organism_id);


--
-- Name: feature_idx3; Type: INDEX; Schema: chado; Owner: drupal
--

CREATE INDEX feature_idx3 ON chado.feature USING btree (type_id);


--
-- Name: feature_idx4; Type: INDEX; Schema: chado; Owner: drupal
--

CREATE INDEX feature_idx4 ON chado.feature USING btree (uniquename);


--
-- Name: feature_idx5; Type: INDEX; Schema: chado; Owner: drupal
--

CREATE INDEX feature_idx5 ON chado.feature USING btree (lower((name)::text));


--
-- Name: feature_name_ind1; Type: INDEX; Schema: chado; Owner: drupal
--

CREATE INDEX feature_name_ind1 ON chado.feature USING btree (name);


--
-- Name: feature feature_dbxref_id_fkey; Type: FK CONSTRAINT; Schema: chado; Owner: drupal
--

ALTER TABLE ONLY chado.feature
    ADD CONSTRAINT feature_dbxref_id_fkey FOREIGN KEY (dbxref_id) REFERENCES chado.dbxref(dbxref_id) ON DELETE SET NULL DEFERRABLE INITIALLY DEFERRED;


--
-- Name: feature feature_organism_id_fkey; Type: FK CONSTRAINT; Schema: chado; Owner: drupal
--

ALTER TABLE ONLY chado.feature
    ADD CONSTRAINT feature_organism_id_fkey FOREIGN KEY (organism_id) REFERENCES chado.organism(organism_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;


--
-- Name: feature feature_type_id_fkey; Type: FK CONSTRAINT; Schema: chado; Owner: drupal
--

ALTER TABLE ONLY chado.feature
    ADD CONSTRAINT feature_type_id_fkey FOREIGN KEY (type_id) REFERENCES chado.cvterm(cvterm_id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED;


--
-- PostgreSQL database dump complete
--


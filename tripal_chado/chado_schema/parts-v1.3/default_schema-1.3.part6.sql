SET search_path=so,chado,pg_catalog;
--- *** relation: cosmid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cloning vector that is a hybrid of lam ***
--- *** bda phages and a plasmid that can be pro ***
--- *** pagated as a plasmid or packaged as a ph ***
--- *** age,since they retain the lambda cos sit ***
--- *** es.                                      ***
--- ************************************************
---

CREATE VIEW cosmid AS
  SELECT
    feature_id AS cosmid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cosmid';

--- ************************************************
--- *** relation: phagemid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A plasmid which carries within its seque ***
--- *** nce a bacteriophage replication origin.  ***
--- *** When the host bacterium is infected with ***
--- ***  "helper" phage, a phagemid is replicate ***
--- *** d along with the phage DNA and packaged  ***
--- *** into phage capsids.                      ***
--- ************************************************
---

CREATE VIEW phagemid AS
  SELECT
    feature_id AS phagemid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'phagemid';

--- ************************************************
--- *** relation: fosmid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cloning vector that utilizes the E. co ***
--- *** li F factor.                             ***
--- ************************************************
---

CREATE VIEW fosmid AS
  SELECT
    feature_id AS fosmid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'fosmid';

--- ************************************************
--- *** relation: deletion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The point at which one or more contiguou ***
--- *** s nucleotides were excised.              ***
--- ************************************************
---

CREATE VIEW deletion AS
  SELECT
    feature_id AS deletion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'deletion';

--- ************************************************
--- *** relation: methylated_a ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A modified RNA base in which adenine has ***
--- ***  been methylated.                        ***
--- ************************************************
---

CREATE VIEW methylated_a AS
  SELECT
    feature_id AS methylated_a_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'methylated_A';

--- ************************************************
--- *** relation: splice_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Consensus region of primary transcript b ***
--- *** ordering junction of splicing. A region  ***
--- *** that overlaps exactly 2 base and adjacen ***
--- *** t_to splice_junction.                    ***
--- ************************************************
---

CREATE VIEW splice_site AS
  SELECT
    feature_id AS splice_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cis_splice_site' OR cvterm.name = 'trans_splice_site' OR cvterm.name = 'cryptic_splice_site' OR cvterm.name = 'five_prime_cis_splice_site' OR cvterm.name = 'three_prime_cis_splice_site' OR cvterm.name = 'recursive_splice_site' OR cvterm.name = 'canonical_five_prime_splice_site' OR cvterm.name = 'non_canonical_five_prime_splice_site' OR cvterm.name = 'canonical_three_prime_splice_site' OR cvterm.name = 'non_canonical_three_prime_splice_site' OR cvterm.name = 'trans_splice_acceptor_site' OR cvterm.name = 'trans_splice_donor_site' OR cvterm.name = 'SL1_acceptor_site' OR cvterm.name = 'SL2_acceptor_site' OR cvterm.name = 'SL3_acceptor_site' OR cvterm.name = 'SL4_acceptor_site' OR cvterm.name = 'SL5_acceptor_site' OR cvterm.name = 'SL6_acceptor_site' OR cvterm.name = 'SL7_acceptor_site' OR cvterm.name = 'SL8_acceptor_site' OR cvterm.name = 'SL9_acceptor_site' OR cvterm.name = 'SL10_accceptor_site' OR cvterm.name = 'SL11_acceptor_site' OR cvterm.name = 'SL12_acceptor_site' OR cvterm.name = 'splice_site';

--- ************************************************
--- *** relation: five_prime_cis_splice_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Intronic 2 bp region bordering the exon, ***
--- ***  at the 5' edge of the intron. A splice_ ***
--- *** site that is downstream_adjacent_to exon ***
--- ***  and starts intron.                      ***
--- ************************************************
---

CREATE VIEW five_prime_cis_splice_site AS
  SELECT
    feature_id AS five_prime_cis_splice_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'canonical_five_prime_splice_site' OR cvterm.name = 'non_canonical_five_prime_splice_site' OR cvterm.name = 'five_prime_cis_splice_site';

--- ************************************************
--- *** relation: three_prime_cis_splice_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Intronic 2 bp region bordering the exon, ***
--- ***  at the 3' edge of the intron. A splice_ ***
--- *** site that is upstream_adjacent_to exon a ***
--- *** nd finishes intron.                      ***
--- ************************************************
---

CREATE VIEW three_prime_cis_splice_site AS
  SELECT
    feature_id AS three_prime_cis_splice_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'canonical_three_prime_splice_site' OR cvterm.name = 'non_canonical_three_prime_splice_site' OR cvterm.name = 'three_prime_cis_splice_site';

--- ************************************************
--- *** relation: enhancer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cis-acting sequence that increases the ***
--- ***  utilization of (some) eukaryotic promot ***
--- *** ers, and can function in either orientat ***
--- *** ion and in any location (upstream or dow ***
--- *** nstream) relative to the promoter.       ***
--- ************************************************
---

CREATE VIEW enhancer AS
  SELECT
    feature_id AS enhancer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'enhancer_bound_by_factor' OR cvterm.name = 'shadow_enhancer' OR cvterm.name = 'enhancer';

--- ************************************************
--- *** relation: enhancer_bound_by_factor ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An enhancer bound by a factor.           ***
--- ************************************************
---

CREATE VIEW enhancer_bound_by_factor AS
  SELECT
    feature_id AS enhancer_bound_by_factor_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'enhancer_bound_by_factor';

--- ************************************************
--- *** relation: promoter ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A regulatory_region composed of the TSS( ***
--- *** s) and binding sites for TF_complexes of ***
--- ***  the basal transcription machinery.      ***
--- ************************************************
---

CREATE VIEW promoter AS
  SELECT
    feature_id AS promoter_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'bidirectional_promoter' OR cvterm.name = 'RNA_polymerase_promoter' OR cvterm.name = 'RNApol_I_promoter' OR cvterm.name = 'RNApol_II_promoter' OR cvterm.name = 'RNApol_III_promoter' OR cvterm.name = 'bacterial_RNApol_promoter' OR cvterm.name = 'Phage_RNA_Polymerase_Promoter' OR cvterm.name = 'RNApol_II_core_promoter' OR cvterm.name = 'RNApol_III_promoter_type_1' OR cvterm.name = 'RNApol_III_promoter_type_2' OR cvterm.name = 'RNApol_III_promoter_type_3' OR cvterm.name = 'bacterial_RNApol_promoter_sigma_70' OR cvterm.name = 'bacterial_RNApol_promoter_sigma54' OR cvterm.name = 'SP6_RNA_Polymerase_Promoter' OR cvterm.name = 'T3_RNA_Polymerase_Promoter' OR cvterm.name = 'T7_RNA_Polymerase_Promoter' OR cvterm.name = 'promoter';

--- ************************************************
--- *** relation: rnapol_i_promoter ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A DNA sequence in eukaryotic DNA to whic ***
--- *** h RNA polymerase I binds, to begin trans ***
--- *** cription.                                ***
--- ************************************************
---

CREATE VIEW rnapol_i_promoter AS
  SELECT
    feature_id AS rnapol_i_promoter_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNApol_I_promoter';

--- ************************************************
--- *** relation: rnapol_ii_promoter ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A DNA sequence in eukaryotic DNA to whic ***
--- *** h RNA polymerase II binds, to begin tran ***
--- *** scription.                               ***
--- ************************************************
---

CREATE VIEW rnapol_ii_promoter AS
  SELECT
    feature_id AS rnapol_ii_promoter_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNApol_II_core_promoter' OR cvterm.name = 'RNApol_II_promoter';

--- ************************************************
--- *** relation: rnapol_iii_promoter ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A DNA sequence in eukaryotic DNA to whic ***
--- *** h RNA polymerase III binds, to begin tra ***
--- *** nscription.                              ***
--- ************************************************
---

CREATE VIEW rnapol_iii_promoter AS
  SELECT
    feature_id AS rnapol_iii_promoter_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNApol_III_promoter_type_1' OR cvterm.name = 'RNApol_III_promoter_type_2' OR cvterm.name = 'RNApol_III_promoter_type_3' OR cvterm.name = 'RNApol_III_promoter';

--- ************************************************
--- *** relation: caat_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Part of a conserved sequence located abo ***
--- *** ut 75-bp upstream of the start point of  ***
--- *** eukaryotic transcription units which may ***
--- ***  be involved in RNA polymerase binding;  ***
--- *** consensus=GG(C|T)CAATCT.                 ***
--- ************************************************
---

CREATE VIEW caat_signal AS
  SELECT
    feature_id AS caat_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'CAAT_signal';

--- ************************************************
--- *** relation: gc_rich_promoter_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A conserved GC-rich region located upstr ***
--- *** eam of the start point of eukaryotic tra ***
--- *** nscription units which may occur in mult ***
--- *** iple copies or in either orientation; co ***
--- *** nsensus=GGGCGG.                          ***
--- ************************************************
---

CREATE VIEW gc_rich_promoter_region AS
  SELECT
    feature_id AS gc_rich_promoter_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'GC_rich_promoter_region';

--- ************************************************
--- *** relation: tata_box ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A conserved AT-rich septamer found about ***
--- ***  25-bp before the start point of many eu ***
--- *** karyotic RNA polymerase II transcript un ***
--- *** its; may be involved in positioning the  ***
--- *** enzyme for correct initiation; consensus ***
--- *** =TATA(A|T)A(A|T).                        ***
--- ************************************************
---

CREATE VIEW tata_box AS
  SELECT
    feature_id AS tata_box_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNA_polymerase_II_TATA_box' OR cvterm.name = 'RNA_polymerase_III_TATA_box' OR cvterm.name = 'TATA_box';

--- ************************************************
--- *** relation: minus_10_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A conserved region about 10-bp upstream  ***
--- *** of the start point of bacterial transcri ***
--- *** ption units which may be involved in bin ***
--- *** ding RNA polymerase; consensus=TAtAaT. T ***
--- *** his region is associated with sigma fact ***
--- *** or 70.                                   ***
--- ************************************************
---

CREATE VIEW minus_10_signal AS
  SELECT
    feature_id AS minus_10_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minus_10_signal';

--- ************************************************
--- *** relation: minus_35_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A conserved hexamer about 35-bp upstream ***
--- ***  of the start point of bacterial transcr ***
--- *** iption units; consensus=TTGACa or TGTTGA ***
--- *** CA. This region is associated with sigma ***
--- ***  factor 70.                              ***
--- ************************************************
---

CREATE VIEW minus_35_signal AS
  SELECT
    feature_id AS minus_35_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minus_35_signal';

--- ************************************************
--- *** relation: cross_genome_match ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A nucleotide match against a sequence fr ***
--- *** om another organism.                     ***
--- ************************************************
---

CREATE VIEW cross_genome_match AS
  SELECT
    feature_id AS cross_genome_match_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cross_genome_match';

--- ************************************************
--- *** relation: operon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A group of contiguous genes transcribed  ***
--- *** as a single (polycistronic) mRNA from a  ***
--- *** single regulatory region.                ***
--- ************************************************
---

CREATE VIEW operon AS
  SELECT
    feature_id AS operon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'operon';

--- ************************************************
--- *** relation: clone_insert_start ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The start of the clone insert.           ***
--- ************************************************
---

CREATE VIEW clone_insert_start AS
  SELECT
    feature_id AS clone_insert_start_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'clone_insert_start';

--- ************************************************
--- *** relation: retrotransposon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transposable element that is incorpora ***
--- *** ted into a chromosome by a mechanism tha ***
--- *** t requires reverse transcriptase.        ***
--- ************************************************
---

CREATE VIEW retrotransposon AS
  SELECT
    feature_id AS retrotransposon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'LTR_retrotransposon' OR cvterm.name = 'non_LTR_retrotransposon' OR cvterm.name = 'LINE_element' OR cvterm.name = 'SINE_element' OR cvterm.name = 'retrotransposon';

--- ************************************************
--- *** relation: translated_nucleotide_match ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A match against a translated sequence.   ***
--- ************************************************
---

CREATE VIEW translated_nucleotide_match AS
  SELECT
    feature_id AS translated_nucleotide_match_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'translated_nucleotide_match';

--- ************************************************
--- *** relation: dna_transposon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transposon where the mechanism of tran ***
--- *** sposition is via a DNA intermediate.     ***
--- ************************************************
---

CREATE VIEW dna_transposon AS
  SELECT
    feature_id AS dna_transposon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'terminal_inverted_repeat_element' OR cvterm.name = 'foldback_element' OR cvterm.name = 'conjugative_transposon' OR cvterm.name = 'helitron' OR cvterm.name = 'p_element' OR cvterm.name = 'MITE' OR cvterm.name = 'insertion_sequence' OR cvterm.name = 'polinton' OR cvterm.name = 'DNA_transposon';

--- ************************************************
--- *** relation: non_transcribed_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of the gene which is not transc ***
--- *** ribed.                                   ***
--- ************************************************
---

CREATE VIEW non_transcribed_region AS
  SELECT
    feature_id AS non_transcribed_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'non_transcribed_region';

--- ************************************************
--- *** relation: u2_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A major type of spliceosomal intron spli ***
--- *** ced by the U2 spliceosome, that includes ***
--- ***  U1, U2, U4/U6 and U5 snRNAs.            ***
--- ************************************************
---

CREATE VIEW u2_intron AS
  SELECT
    feature_id AS u2_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U2_intron';

--- ************************************************
--- *** relation: primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript that in its initial state r ***
--- *** equires modification to be functional.   ***
--- ************************************************
---

CREATE VIEW primary_transcript AS
  SELECT
    feature_id AS primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'protein_coding_primary_transcript' OR cvterm.name = 'nc_primary_transcript' OR cvterm.name = 'polycistronic_primary_transcript' OR cvterm.name = 'monocistronic_primary_transcript' OR cvterm.name = 'mini_exon_donor_RNA' OR cvterm.name = 'antisense_primary_transcript' OR cvterm.name = 'capped_primary_transcript' OR cvterm.name = 'pre_edited_mRNA' OR cvterm.name = 'scRNA_primary_transcript' OR cvterm.name = 'rRNA_primary_transcript' OR cvterm.name = 'tRNA_primary_transcript' OR cvterm.name = 'snRNA_primary_transcript' OR cvterm.name = 'snoRNA_primary_transcript' OR cvterm.name = 'tmRNA_primary_transcript' OR cvterm.name = 'SRP_RNA_primary_transcript' OR cvterm.name = 'miRNA_primary_transcript' OR cvterm.name = 'tasiRNA_primary_transcript' OR cvterm.name = 'rRNA_small_subunit_primary_transcript' OR cvterm.name = 'rRNA_large_subunit_primary_transcript' OR cvterm.name = 'alanine_tRNA_primary_transcript' OR cvterm.name = 'arginine_tRNA_primary_transcript' OR cvterm.name = 'asparagine_tRNA_primary_transcript' OR cvterm.name = 'aspartic_acid_tRNA_primary_transcript' OR cvterm.name = 'cysteine_tRNA_primary_transcript' OR cvterm.name = 'glutamic_acid_tRNA_primary_transcript' OR cvterm.name = 'glutamine_tRNA_primary_transcript' OR cvterm.name = 'glycine_tRNA_primary_transcript' OR cvterm.name = 'histidine_tRNA_primary_transcript' OR cvterm.name = 'isoleucine_tRNA_primary_transcript' OR cvterm.name = 'leucine_tRNA_primary_transcript' OR cvterm.name = 'lysine_tRNA_primary_transcript' OR cvterm.name = 'methionine_tRNA_primary_transcript' OR cvterm.name = 'phenylalanine_tRNA_primary_transcript' OR cvterm.name = 'proline_tRNA_primary_transcript' OR cvterm.name = 'serine_tRNA_primary_transcript' OR cvterm.name = 'threonine_tRNA_primary_transcript' OR cvterm.name = 'tryptophan_tRNA_primary_transcript' OR cvterm.name = 'tyrosine_tRNA_primary_transcript' OR cvterm.name = 'valine_tRNA_primary_transcript' OR cvterm.name = 'pyrrolysine_tRNA_primary_transcript' OR cvterm.name = 'selenocysteine_tRNA_primary_transcript' OR cvterm.name = 'methylation_guide_snoRNA_primary_transcript' OR cvterm.name = 'rRNA_cleavage_snoRNA_primary_transcript' OR cvterm.name = 'C_D_box_snoRNA_primary_transcript' OR cvterm.name = 'H_ACA_box_snoRNA_primary_transcript' OR cvterm.name = 'U14_snoRNA_primary_transcript' OR cvterm.name = 'stRNA_primary_transcript' OR cvterm.name = 'dicistronic_primary_transcript' OR cvterm.name = 'primary_transcript';

--- ************************************************
--- *** relation: ltr_retrotransposon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A retrotransposon flanked by long termin ***
--- *** al repeat sequences.                     ***
--- ************************************************
---

CREATE VIEW ltr_retrotransposon AS
  SELECT
    feature_id AS ltr_retrotransposon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'LTR_retrotransposon';

--- ************************************************
--- *** relation: intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a primary transcript that is ***
--- ***  transcribed, but removed from within th ***
--- *** e transcript by splicing together the se ***
--- *** quences (exons) on either side of it.    ***
--- ************************************************
---

CREATE VIEW intron AS
  SELECT
    feature_id AS intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_intron' OR cvterm.name = 'interior_intron' OR cvterm.name = 'three_prime_intron' OR cvterm.name = 'twintron' OR cvterm.name = 'UTR_intron' OR cvterm.name = 'autocatalytically_spliced_intron' OR cvterm.name = 'spliceosomal_intron' OR cvterm.name = 'mobile_intron' OR cvterm.name = 'endonuclease_spliced_intron' OR cvterm.name = 'five_prime_UTR_intron' OR cvterm.name = 'three_prime_UTR_intron' OR cvterm.name = 'group_I_intron' OR cvterm.name = 'group_II_intron' OR cvterm.name = 'group_III_intron' OR cvterm.name = 'group_IIA_intron' OR cvterm.name = 'group_IIB_intron' OR cvterm.name = 'U2_intron' OR cvterm.name = 'U12_intron' OR cvterm.name = 'archaeal_intron' OR cvterm.name = 'tRNA_intron' OR cvterm.name = 'intron';

--- ************************************************
--- *** relation: non_ltr_retrotransposon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A retrotransposon without long terminal  ***
--- *** repeat sequences.                        ***
--- ************************************************
---

CREATE VIEW non_ltr_retrotransposon AS
  SELECT
    feature_id AS non_ltr_retrotransposon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'LINE_element' OR cvterm.name = 'SINE_element' OR cvterm.name = 'non_LTR_retrotransposon';

--- ************************************************
--- *** relation: five_prime_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW five_prime_intron AS
  SELECT
    feature_id AS five_prime_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_intron';

--- ************************************************
--- *** relation: interior_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW interior_intron AS
  SELECT
    feature_id AS interior_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'interior_intron';

--- ************************************************
--- *** relation: three_prime_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW three_prime_intron AS
  SELECT
    feature_id AS three_prime_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_intron';

--- ************************************************
--- *** relation: rflp_fragment ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A DNA fragment used as a reagent to dete ***
--- *** ct the polymorphic genomic loci by hybri ***
--- *** dizing against the genomic DNA digested  ***
--- *** with a given restriction enzyme.         ***
--- ************************************************
---

CREATE VIEW rflp_fragment AS
  SELECT
    feature_id AS rflp_fragment_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RFLP_fragment';

--- ************************************************
--- *** relation: line_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A dispersed repeat family with many copi ***
--- *** es, each from 1 to 6 kb long. New elemen ***
--- *** ts are generated by retroposition of a t ***
--- *** ranscribed copy. Typically the LINE cont ***
--- *** ains 2 ORF's one of which is reverse tra ***
--- *** nscriptase, and 3'and 5' direct repeats. ***
--- ************************************************
---

CREATE VIEW line_element AS
  SELECT
    feature_id AS line_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'LINE_element';

--- ************************************************
--- *** relation: coding_exon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An exon whereby at least one base is par ***
--- *** t of a codon (here, 'codon' is inclusive ***
--- ***  of the stop_codon).                     ***
--- ************************************************
---

CREATE VIEW coding_exon AS
  SELECT
    feature_id AS coding_exon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'interior_coding_exon' OR cvterm.name = 'five_prime_coding_exon' OR cvterm.name = 'three_prime_coding_exon' OR cvterm.name = 'coding_exon';

--- ************************************************
--- *** relation: five_prime_coding_exon_coding_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The sequence of the five_prime_coding_ex ***
--- *** on that codes for protein.               ***
--- ************************************************
---

CREATE VIEW five_prime_coding_exon_coding_region AS
  SELECT
    feature_id AS five_prime_coding_exon_coding_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_coding_exon_coding_region';

--- ************************************************
--- *** relation: three_prime_coding_exon_coding_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The sequence of the three_prime_coding_e ***
--- *** xon that codes for protein.              ***
--- ************************************************
---

CREATE VIEW three_prime_coding_exon_coding_region AS
  SELECT
    feature_id AS three_prime_coding_exon_coding_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_coding_exon_coding_region';

--- ************************************************
--- *** relation: noncoding_exon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An exon that does not contain any codons ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW noncoding_exon AS
  SELECT
    feature_id AS noncoding_exon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_noncoding_exon' OR cvterm.name = 'five_prime_noncoding_exon' OR cvterm.name = 'noncoding_exon';

--- ************************************************
--- *** relation: translocation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of nucleotide sequence that has ***
--- ***  translocated to a new position.         ***
--- ************************************************
---

CREATE VIEW translocation AS
  SELECT
    feature_id AS translocation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'translocation';

--- ************************************************
--- *** relation: five_prime_coding_exon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The 5' most coding exon.                 ***
--- ************************************************
---

CREATE VIEW five_prime_coding_exon AS
  SELECT
    feature_id AS five_prime_coding_exon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_coding_exon';

--- ************************************************
--- *** relation: interior_exon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An exon that is bounded by 5' and 3' spl ***
--- *** ice sites.                               ***
--- ************************************************
---

CREATE VIEW interior_exon AS
  SELECT
    feature_id AS interior_exon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'interior_exon';

--- ************************************************
--- *** relation: three_prime_coding_exon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The coding exon that is most 3-prime on  ***
--- *** a given transcript.                      ***
--- ************************************************
---

CREATE VIEW three_prime_coding_exon AS
  SELECT
    feature_id AS three_prime_coding_exon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_coding_exon';

--- ************************************************
--- *** relation: utr ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Messenger RNA sequences that are untrans ***
--- *** lated and lie five prime or three prime  ***
--- *** to sequences which are translated.       ***
--- ************************************************
---

CREATE VIEW utr AS
  SELECT
    feature_id AS utr_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_UTR' OR cvterm.name = 'three_prime_UTR' OR cvterm.name = 'internal_UTR' OR cvterm.name = 'untranslated_region_polycistronic_mRNA' OR cvterm.name = 'UTR';

--- ************************************************
--- *** relation: five_prime_utr ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region at the 5' end of a mature trans ***
--- *** cript (preceding the initiation codon) t ***
--- *** hat is not translated into a protein.    ***
--- ************************************************
---

CREATE VIEW five_prime_utr AS
  SELECT
    feature_id AS five_prime_utr_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_UTR';

--- ************************************************
--- *** relation: three_prime_utr ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region at the 3' end of a mature trans ***
--- *** cript (following the stop codon) that is ***
--- ***  not translated into a protein.          ***
--- ************************************************
---

CREATE VIEW three_prime_utr AS
  SELECT
    feature_id AS three_prime_utr_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_UTR';

--- ************************************************
--- *** relation: sine_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A repetitive element, a few hundred base ***
--- ***  pairs long, that is dispersed throughou ***
--- *** t the genome. A common human SINE is the ***
--- ***  Alu element.                            ***
--- ************************************************
---

CREATE VIEW sine_element AS
  SELECT
    feature_id AS sine_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SINE_element';

--- ************************************************
--- *** relation: simple_sequence_length_variation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW simple_sequence_length_variation AS
  SELECT
    feature_id AS simple_sequence_length_variation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'simple_sequence_length_variation';

--- ************************************************
--- *** relation: terminal_inverted_repeat_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A DNA transposable element defined as ha ***
--- *** ving termini with perfect, or nearly per ***
--- *** fect short inverted repeats, generally 1 ***
--- *** 0 - 40 nucleotides long.                 ***
--- ************************************************
---

CREATE VIEW terminal_inverted_repeat_element AS
  SELECT
    feature_id AS terminal_inverted_repeat_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'MITE' OR cvterm.name = 'insertion_sequence' OR cvterm.name = 'polinton' OR cvterm.name = 'terminal_inverted_repeat_element';

--- ************************************************
--- *** relation: rrna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding a ribosoma ***
--- *** l RNA.                                   ***
--- ************************************************
---

CREATE VIEW rrna_primary_transcript AS
  SELECT
    feature_id AS rrna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_small_subunit_primary_transcript' OR cvterm.name = 'rRNA_large_subunit_primary_transcript' OR cvterm.name = 'rRNA_primary_transcript';

--- ************************************************
--- *** relation: trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding a transfer ***
--- ***  RNA (SO:0000253).                       ***
--- ************************************************
---

CREATE VIEW trna_primary_transcript AS
  SELECT
    feature_id AS trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'alanine_tRNA_primary_transcript' OR cvterm.name = 'arginine_tRNA_primary_transcript' OR cvterm.name = 'asparagine_tRNA_primary_transcript' OR cvterm.name = 'aspartic_acid_tRNA_primary_transcript' OR cvterm.name = 'cysteine_tRNA_primary_transcript' OR cvterm.name = 'glutamic_acid_tRNA_primary_transcript' OR cvterm.name = 'glutamine_tRNA_primary_transcript' OR cvterm.name = 'glycine_tRNA_primary_transcript' OR cvterm.name = 'histidine_tRNA_primary_transcript' OR cvterm.name = 'isoleucine_tRNA_primary_transcript' OR cvterm.name = 'leucine_tRNA_primary_transcript' OR cvterm.name = 'lysine_tRNA_primary_transcript' OR cvterm.name = 'methionine_tRNA_primary_transcript' OR cvterm.name = 'phenylalanine_tRNA_primary_transcript' OR cvterm.name = 'proline_tRNA_primary_transcript' OR cvterm.name = 'serine_tRNA_primary_transcript' OR cvterm.name = 'threonine_tRNA_primary_transcript' OR cvterm.name = 'tryptophan_tRNA_primary_transcript' OR cvterm.name = 'tyrosine_tRNA_primary_transcript' OR cvterm.name = 'valine_tRNA_primary_transcript' OR cvterm.name = 'pyrrolysine_tRNA_primary_transcript' OR cvterm.name = 'selenocysteine_tRNA_primary_transcript' OR cvterm.name = 'tRNA_primary_transcript';

--- ************************************************
--- *** relation: alanine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding alanyl tRN ***
--- *** A.                                       ***
--- ************************************************
---

CREATE VIEW alanine_trna_primary_transcript AS
  SELECT
    feature_id AS alanine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'alanine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: arg_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding arginyl tR ***
--- *** NA (SO:0000255).                         ***
--- ************************************************
---

CREATE VIEW arg_trna_primary_transcript AS
  SELECT
    feature_id AS arg_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'arginine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: asparagine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding asparaginy ***
--- *** l tRNA (SO:0000256).                     ***
--- ************************************************
---

CREATE VIEW asparagine_trna_primary_transcript AS
  SELECT
    feature_id AS asparagine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'asparagine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: aspartic_acid_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding aspartyl t ***
--- *** RNA (SO:0000257).                        ***
--- ************************************************
---

CREATE VIEW aspartic_acid_trna_primary_transcript AS
  SELECT
    feature_id AS aspartic_acid_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'aspartic_acid_tRNA_primary_transcript';

--- ************************************************
--- *** relation: cysteine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding cysteinyl  ***
--- *** tRNA (SO:0000258).                       ***

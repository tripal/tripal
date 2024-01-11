SET search_path=so,chado,pg_catalog;
--- *** relation: four_bp_start_codon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A non-canonical start codon with 4 base  ***
--- *** pairs.                                   ***
--- ************************************************
---

CREATE VIEW four_bp_start_codon AS
  SELECT
    feature_id AS four_bp_start_codon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'four_bp_start_codon';

--- ************************************************
--- *** relation: archaeal_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An intron characteristic of Archaeal tRN ***
--- *** A and rRNA genes, where intron transcrip ***
--- *** t generates a bulge-helix-bulge motif th ***
--- *** at is recognised by a splicing endoribon ***
--- *** uclease.                                 ***
--- ************************************************
---

CREATE VIEW archaeal_intron AS
  SELECT
    feature_id AS archaeal_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'archaeal_intron';

--- ************************************************
--- *** relation: trna_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An intron found in tRNA that is spliced  ***
--- *** via endonucleolytic cleavage and ligatio ***
--- *** n rather than transesterification.       ***
--- ************************************************
---

CREATE VIEW trna_intron AS
  SELECT
    feature_id AS trna_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tRNA_intron';

--- ************************************************
--- *** relation: ctg_start_codon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A non-canonical start codon of sequence  ***
--- *** CTG.                                     ***
--- ************************************************
---

CREATE VIEW ctg_start_codon AS
  SELECT
    feature_id AS ctg_start_codon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'CTG_start_codon';

--- ************************************************
--- *** relation: secis_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The incorporation of selenocysteine into ***
--- ***  a protein sequence is directed by an in ***
--- *** -frame UGA codon (usually a stop codon)  ***
--- *** within the coding region of the mRNA. Se ***
--- *** lenoprotein mRNAs contain a conserved se ***
--- *** condary structure in the 3' UTR that is  ***
--- *** required for the distinction of UGA stop ***
--- ***  from UGA selenocysteine. The selenocyst ***
--- *** eine insertion sequence (SECIS) is aroun ***
--- *** d 60 nt in length and adopts a hairpin s ***
--- *** tructure which is sufficiently well-defi ***
--- *** ned and conserved to act as a computatio ***
--- *** nal screen for selenoprotein genes.      ***
--- ************************************************
---

CREATE VIEW secis_element AS
  SELECT
    feature_id AS secis_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SECIS_element';

--- ************************************************
--- *** relation: retron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Sequence coding for a short, single-stra ***
--- *** nded, DNA sequence via a retrotransposed ***
--- ***  RNA intermediate; characteristic of som ***
--- *** e microbial genomes.                     ***
--- ************************************************
---

CREATE VIEW retron AS
  SELECT
    feature_id AS retron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'retron';

--- ************************************************
--- *** relation: three_prime_recoding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The recoding stimulatory signal located  ***
--- *** downstream of the recoding site.         ***
--- ************************************************
---

CREATE VIEW three_prime_recoding_site AS
  SELECT
    feature_id AS three_prime_recoding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_stem_loop_structure' OR cvterm.name = 'flanking_three_prime_quadruplet_recoding_signal' OR cvterm.name = 'three_prime_repeat_recoding_signal' OR cvterm.name = 'distant_three_prime_recoding_signal' OR cvterm.name = 'three_prime_recoding_site';

--- ************************************************
--- *** relation: three_prime_stem_loop_structure ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A recoding stimulatory region, the stem- ***
--- *** loop secondary structural element is dow ***
--- *** nstream of the redefined region.         ***
--- ************************************************
---

CREATE VIEW three_prime_stem_loop_structure AS
  SELECT
    feature_id AS three_prime_stem_loop_structure_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_stem_loop_structure';

--- ************************************************
--- *** relation: five_prime_recoding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The recoding stimulatory signal located  ***
--- *** upstream of the recoding site.           ***
--- ************************************************
---

CREATE VIEW five_prime_recoding_site AS
  SELECT
    feature_id AS five_prime_recoding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_recoding_site';

--- ************************************************
--- *** relation: flanking_three_prime_quadruplet_recoding_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Four base pair sequence immediately down ***
--- *** stream of the redefined region. The rede ***
--- *** fined region is a frameshift site. The q ***
--- *** uadruplet is 2 overlapping codons.       ***
--- ************************************************
---

CREATE VIEW flanking_three_prime_quadruplet_recoding_signal AS
  SELECT
    feature_id AS flanking_three_prime_quadruplet_recoding_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'flanking_three_prime_quadruplet_recoding_signal';

--- ************************************************
--- *** relation: uag_stop_codon_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A stop codon signal for a UAG stop codon ***
--- ***  redefinition.                           ***
--- ************************************************
---

CREATE VIEW uag_stop_codon_signal AS
  SELECT
    feature_id AS uag_stop_codon_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'UAG_stop_codon_signal';

--- ************************************************
--- *** relation: uaa_stop_codon_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A stop codon signal for a UAA stop codon ***
--- ***  redefinition.                           ***
--- ************************************************
---

CREATE VIEW uaa_stop_codon_signal AS
  SELECT
    feature_id AS uaa_stop_codon_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'UAA_stop_codon_signal';

--- ************************************************
--- *** relation: regulon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A group of genes, whether linked as a cl ***
--- *** uster or not, that respond to a common r ***
--- *** egulatory signal.                        ***
--- ************************************************
---

CREATE VIEW regulon AS
  SELECT
    feature_id AS regulon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'regulon';

--- ************************************************
--- *** relation: uga_stop_codon_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A stop codon signal for a UGA stop codon ***
--- ***  redefinition.                           ***
--- ************************************************
---

CREATE VIEW uga_stop_codon_signal AS
  SELECT
    feature_id AS uga_stop_codon_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'UGA_stop_codon_signal';

--- ************************************************
--- *** relation: three_prime_repeat_recoding_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A recoding stimulatory signal, downstrea ***
--- *** m sequence important for recoding that c ***
--- *** ontains repetitive elements.             ***
--- ************************************************
---

CREATE VIEW three_prime_repeat_recoding_signal AS
  SELECT
    feature_id AS three_prime_repeat_recoding_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_repeat_recoding_signal';

--- ************************************************
--- *** relation: distant_three_prime_recoding_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A recoding signal that is found many hun ***
--- *** dreds of nucleotides 3' of a redefined s ***
--- *** top codon.                               ***
--- ************************************************
---

CREATE VIEW distant_three_prime_recoding_signal AS
  SELECT
    feature_id AS distant_three_prime_recoding_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'distant_three_prime_recoding_signal';

--- ************************************************
--- *** relation: stop_codon_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A recoding stimulatory signal that is a  ***
--- *** stop codon and has effect on efficiency  ***
--- *** of recoding.                             ***
--- ************************************************
---

CREATE VIEW stop_codon_signal AS
  SELECT
    feature_id AS stop_codon_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'UAG_stop_codon_signal' OR cvterm.name = 'UAA_stop_codon_signal' OR cvterm.name = 'UGA_stop_codon_signal' OR cvterm.name = 'stop_codon_signal';

--- ************************************************
--- *** relation: databank_entry ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The sequence referred to by an entry in  ***
--- *** a databank such as Genbank or SwissProt. ***
--- ************************************************
---

CREATE VIEW databank_entry AS
  SELECT
    feature_id AS databank_entry_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'databank_entry';

--- ************************************************
--- *** relation: gene_segment ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene component region which acts as a  ***
--- *** recombinational unit of a gene whose fun ***
--- *** ctional form is generated through somati ***
--- *** c recombination.                         ***
--- ************************************************
---

CREATE VIEW gene_segment AS
  SELECT
    feature_id AS gene_segment_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pseudogenic_gene_segment' OR cvterm.name = 'gene_segment';

CREATE TABLE sequence_cv_lookup_table (sequence_cv_lookup_table_id serial not null, primary key(sequence_cv_lookup_table_id), original_cvterm_name varchar(1024), relation_name varchar(128));
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcription_variant','transcription_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('helitron','helitron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cleaved_initiator_methionine','cleaved_initiator_methionine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('epoxyqueuosine','epoxyqueuosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u4atac_snrna','u4atac_snrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('kinetoplast','kinetoplast');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('elongated_out_of_frame_polypeptide_n_terminal','elongated_out_of_frame_polypeptide_n_terminal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('shadow_enhancer','shadow_enhancer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered','engineered');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rna_polymerase_ii_tata_box','rna_polymerase_ii_tata_box');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('seven_aminomethyl_seven_deazaguanosine','seven_aminomethyl_seven_deazaguanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequence_motif','sequence_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('low_complexity','low_complexity');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('est_match','est_match');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_nonamer','v_nonamer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('d_dj_j_c_cluster','d_dj_j_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_21s','rrna_21s');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bound_by_factor','bound_by_factor');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_carboxymethyluridine','five_carboxymethyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dinucleotide_repeat_microsatellite_feature','dinucleotide_repeat_microsatellite_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_methylthio_n6_methyladenosine','two_methylthio_n6_methyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('trans_spliced_mrna','trans_spliced_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('g_to_c_transversion','g_to_c_transversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('heptamer_of_recombination_feature_of_vertebrate_immune_system_gene','heptamer_of_recombination_feature_of_vertebrate_im_sys_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('genotype','so_genotype');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cloned_region','cloned_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tmrna_coding_piece','tmrna_coding_piece');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rna_6s','rna_6s');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('x_element','x_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('minicircle','minicircle');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('grna_encoding','grna_encoding');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('endonuclease_spliced_intron','endonuclease_spliced_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('insertional_duplication','insertional_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('databank_entry','databank_entry');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('glycine','glycine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('variant_phenotype','variant_phenotype');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_cluster','v_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sl12_acceptor_site','sl12_acceptor_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_nickel_ion_contact_site','polypeptide_nickel_ion_contact_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('circular_single_stranded_rna_chromosome','circular_single_stranded_rna_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('wc_base_pair','wc_base_pair');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pcr_product','pcr_product');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('3_prime_utr_variant','three_prime_utr_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_three_amino_three_carboxypropyl_uridine','three_three_amino_three_carboxypropyl_uridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('site_specific_recombination_target_region','site_specific_recombination_target_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_polycistronic_transcript','gene_with_polycistronic_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rescue','rescue');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nuclease_hypersensitive_site','nuclease_hypersensitive_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('upstream_gene_variant','upstream_gene_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mirna_loop','mirna_loop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('double_stranded_cdna','double_stranded_cdna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_post_translational_processing_variant','polypeptide_post_translational_processing_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('2kb_upstream_variant','twokb_upstream_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('supported_by_domain_match','supported_by_domain_match');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('one_methylpseudouridine','one_methylpseudouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n_terminal_region','n_terminal_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('blunt_end_restriction_enzyme_cleavage_site','blunt_end_restriction_enzyme_cleavage_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('experimental_result_region','experimental_result_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('methionine_trna_primary_transcript','methionine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('utr','utr');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_terminal_residue','non_terminal_residue');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('member_of_regulon','member_of_regulon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('threonine_trna_primary_transcript','thr_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cds_supported_by_sequence_similarity_data','cds_supported_by_sequence_similarity_data');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_structural_region','polypeptide_structural_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('trna_gene','trna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_tungsten_ion_contact_site','polypeptide_tungsten_ion_contact_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_bulge_loop_six','beta_bulge_loop_six');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('d_dj_c_cluster','d_dj_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequence_location','sequence_location');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_nest_right_left_motif','polypeptide_nest_right_left_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('encodes_overlapping_polypeptides_different_start_and_stop','encodes_overlapping_polypeptides_different_start_and_stop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('leucoplast_gene','leucoplast_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('y_rna','y_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('trans_spliced_transcript','trans_spliced_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inverted','inverted');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('splicing_regulatory_region','splicing_regulatory_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('branch_site','branch_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_bulge_loop_five','beta_bulge_loop_five');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosome_breakpoint','chromosome_breakpoint');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequence_uncertainty','sequence_uncertainty');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n6_methyl_n6_threonylcarbamoyladenosine','n6_methyl_n6_threonylcarbamoyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_mrna_with_frameshift','gene_with_mrna_with_frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('compositionally_biased_region_of_peptide','compositionally_biased_region_of_peptide');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vj_j_c_cluster','vj_j_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pirna','pirna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('reverse_hoogsteen_base_pair','reverse_hoogsteen_base_pair');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tryptophanyl_trna','tryptophanyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polya_primed_cdna_clone','polya_primed_cdna_clone');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('leucoplast_chromosome','leucoplast_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('status','status');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ltr_retrotransposon','ltr_retrotransposon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rnase_p_rna','rnase_p_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('conjugative_transposon','conjugative_transposon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('improved_high_quality_draft','improved_high_quality_draft');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('copy_number_gain','copy_number_gain');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('linkage_group','linkage_group');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_trans_spliced_transcript','gene_with_trans_spliced_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sl8_acceptor_site','sl8_acceptor_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('peptide_coil','peptide_coil');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pyrrolysine_trna_primary_transcript','pyrrolysine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_vj_c_cluster','v_vj_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('phage_sequence','phage_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k79_methylation_site','h3k79_methylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recoded','recoded');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transposon_fragment','transposon_fragment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vj_c_cluster','vj_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('editing_domain','editing_domain');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_methylaminomethyluridine','five_methylaminomethyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('centromere_dna_element_ii','centromere_dna_element_ii');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('alteration_attribute','alteration_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_ltr_retrotransposon_polymeric_tract','non_ltr_retrotransposon_polymeric_tract');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transversion','transversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tryptophan','tryptophan');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recursive_splice_site','recursive_splice_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_fusion','polypeptide_fusion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('insulator_binding_site','insulator_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('increased_polyadenylation_variant','increased_polyadenylation_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('proline_trna_primary_transcript','proline_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('repeat_fragment','repeat_fragment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('blocked_reading_frame','blocked_reading_frame');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_cleavage_snorna_primary_transcript','rrna_cleavage_snorna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n6_isopentenyladenosine','n6_isopentenyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_arginine','modified_l_arginine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_conserved_motif','polypeptide_conserved_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('paracentric','paracentric');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('t3_rna_polymerase_promoter','t3_rna_polymerase_promoter');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inversion_derived_bipartite_duplication','inversion_derived_bipartite_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('trans_splice_acceptor_site','trans_splice_acceptor_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('a_box_type_2','a_box_type_2');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rre_rna','rre_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_prime_o_ribosyladenosine_phosphate','two_prime_o_riboA_phosphate');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pac_end','pac_end');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('extramembrane_polypeptide_region','extramembrane_polypeptide_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('copy_number_change','copy_number_change');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intein','intein');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('endosomal_localization_signal','endosomal_localization_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('twintron','twintron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('scrna_primary_transcript','scrna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_carboxymethylaminomethyl_two_prime_o_methyluridine','five_carboxymethylaminomethyl_two_prime_o_methyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('somatic_variant','somatic_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('duplication','duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tmrna_encoding','tmrna_encoding');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_cobalt_ion_contact_site','polypeptide_cobalt_ion_contact_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('flanked','flanked');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inversion','inversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ctg_start_codon','ctg_start_codon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tyrosine_trna_primary_transcript','tyrosine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('eukaryotic_terminator','eukaryotic_terminator');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('frt_flanked','frt_flanked');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('spliceosomal_intron_region','spliceosomal_intron_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('coding_region_of_exon','coding_region_of_exon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cloned_cdna_insert','cloned_cdna_insert');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('decreased_transcription_rate_variant','decreased_transcription_rate_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_vdj_c_cluster','v_vdj_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rnase_p_rna_gene','rnase_p_rna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('translationally_regulated','translationally_regulated');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('histidyl_trna','histidyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sonicate_fragment','sonicate_fragment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_recoded_mrna','gene_with_recoded_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_prime_o_methyluridine','two_prime_o_methyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cosmid','cosmid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('silenced_by_rna_interference','silenced_by_rna_interference');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_conservative_missense_codon','non_conservative_missense_codon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('snorna','snorna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mature_transcript','mature_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pseudouridylation_guide_snorna','pseudouridylation_guide_snorna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('c_gene','c_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('processed_transcript','processed_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('floxed_gene','floxed_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('spot_42_rna','spot_42_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cdna_clone','cdna_clone');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cryptic_splice_site','cryptic_splice_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pseudogenic_gene_segment','pseudogenic_gene_segment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_ltr','three_prime_ltr');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('group_ii_intron','group_ii_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rnase_mrp_rna_gene','rnase_mrp_rna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('structural_alteration','structural_alteration');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pna_oligo','pna_oligo');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('insertion_sequence','insertion_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('junction','junction');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('paralogous','paralogous');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tna','tna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_isopentenylaminomethyl_two_thiouridine','five_isopentenylaminomethyl_two_thiouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nested_tandem_repeat','nested_tandem_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('minus_1_frameshift','minus_1_frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_canonical_three_prime_splice_site','non_canonical_three_prime_splice_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_non_canonical_start_codon','gene_with_non_canonical_start_codon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pseudogenic_rrna','pseudogenic_rrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('serine_threonine_turn','serine_threonine_turn');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('j_gene','j_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k27_trimethylation_site','h3k27_trimethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('strna_primary_transcript','strna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('internal_eliminated_sequence','internal_eliminated_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('allelically_excluded_gene','allelically_excluded_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('qtl','qtl');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_est','three_prime_est');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bred_motif','bred_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('reverse','reverse');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mirna_encoding','mirna_encoding');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n2_n2_2_prime_o_trimethylguanosine','n2_n2_2_prime_o_trimethylguanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('translational_product_function_variant','translational_product_function_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('encodes_alternate_transcription_start_sites','encodes_alternate_transcription_start_sites');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_array','gene_array');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tetranucleotide_repeat_microsatellite_feature','tetranuc_repeat_microsat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_aminomethyl_two_thiouridine','five_aminomethyl_two_thiouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('monocistronic_primary_transcript','monocistronic_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('snv','snv');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('direct','direct');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mobile_genetic_element','mobile_genetic_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_ligand_contact','polypeptide_ligand_contact');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('biomaterial_region','biomaterial_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transposable_element_flanking_region','transposable_element_flanking_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('symmetric_rna_internal_loop','symmetric_rna_internal_loop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mrna_with_plus_1_frameshift','mrna_with_plus_1_frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcriptionally_regulated','transcriptionally_regulated');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_intron','five_prime_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vertebrate_immune_system_gene_recombination_feature','vertebrate_immune_system_gene_recombination_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_carboxyhydroxymethyl_uridine_methyl_ester','five_carboxyhydroxymethyl_uridine_methyl_ester');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosomal_transposition','chromosomal_transposition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('proplastid_gene','proplastid_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('serine_trna_primary_transcript','serine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('attp_site','attp_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('antisense','antisense');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('terminal_inverted_repeat_element','terminal_inverted_repeat_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('coiled_coil','coiled_coil');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_vdj_cluster','v_vdj_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('edited_transcript_by_a_to_i_substitution','edited_transcript_by_a_to_i_substitution');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('protein_coding_primary_transcript','protein_coding_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mite','mite');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cryptic_splice_site_variant','cryptic_splice_site_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('insertion','insertion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('secis_element','secis_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('maxicircle','maxicircle');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tss','tss');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pedigree_specific_variant','pedigree_specific_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cysteine','cysteine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ribothymidine','ribothymidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_adjacent_residues','non_adjacent_residues');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('histone_modification','histone_modification');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('internal_ribosome_entry_site','internal_ribosome_entry_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('outron','outron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_repeat','polypeptide_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('clone_insert_start','clone_insert_start');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('attr_site','attr_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dmv3_motif','dmv3_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('capped_mrna','capped_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequence_rearrangement_feature','sequence_rearrangement_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('apicoplast_chromosome','apicoplast_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_turn_type_six_a_two','beta_turn_type_six_a_two');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('invalidated','invalidated');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('valine','valine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('translationally_regulated_gene','translationally_regulated_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('amino_acid_insertion','amino_acid_insertion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('promoter_targeting_sequence','promoter_targeting_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polinton','polinton');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered_tag','engineered_tag');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_coding_exon_variant','non_coding_exon_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_methylcytidine','five_methylcytidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sl5_acceptor_site','sl5_acceptor_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('positively_autoregulated','positively_autoregulated');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pseudouridine','pseudouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('amplification_origin','amplification_origin');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('unoriented_insertional_duplication','unorient_insert_dup');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcriptionally_constitutive','transcriptionally_constitutive');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('extrachromosomal_mobile_genetic_element','extrachromosomal_mobile_genetic_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('variant_origin','variant_origin');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('utr_region','utr_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mirna','mirna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tyrosine','tyrosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inr1_motif','inr1_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h2b_ubiquitination_site','h2b_ubiquitination_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n6_acetyladenosine','n6_acetyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cis_splice_site','cis_splice_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('floxed','floxed');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_turn_right_handed_type_two','beta_turn_right_handed_type_two');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('utr_variant','utr_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('c_terminal_region','c_terminal_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcription_regulatory_region','transcription_regulatory_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_leucine','modified_l_leucine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_ltr_component','five_prime_ltr_component');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('histone_acylation_region','histone_acylation_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vdj_c_cluster','vdj_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosome_part','chromosome_part');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcriptional_cis_regulatory_region','transcriptional_cis_regulatory_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('phenylalanyl_trna','phenylalanyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('insertion_site','insertion_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gc_rich_promoter_region','gc_rich_promoter_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('overlapping_est_set','overlapping_est_set');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('asx_turn_right_handed_type_two','asx_turn_right_handed_type_two');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('anticodon_loop','anticodon_loop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dmv5_motif','dmv5_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sl1_acceptor_site','sl1_acceptor_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cds_region','cds_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('regulatory_region_variant','regulatory_region_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k9_dimethylation_site','h3k9_dimethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('stop_gained','stop_gained');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('telomerase_rna_gene','telomerase_rna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_dj_j_c_cluster','v_dj_j_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered_insert','engineered_insert');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recombinationally_inverted_gene','recombinationally_inverted_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('microarray_oligo','microarray_oligo');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cassette_array_member','cassette_array_member');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('plus_1_frameshift_variant','plus_1_frameshift_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u12_snrna','u12_snrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('supported_by_est_or_cdna','supported_by_est_or_cdna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('minus_10_signal','minus_10_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('clone_insert_end','clone_insert_end');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inr_motif','inr_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_stem_loop_structure','three_prime_stem_loop_structure');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rflp_fragment','rflp_fragment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('phage_rna_polymerase_promoter','phage_rna_polymerase_promoter');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pyrimidine_transition','pyrimidine_transition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intrinsically_unstructured_polypeptide_region','intrinsically_unstructured_polypeptide_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n2_2_prime_o_dimethylguanosine','n2_2_prime_o_dimethylguanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('exon_loss','exon_loss');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('archaeal_intron','archaeal_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('lna','lna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('exon_junction','exon_junction');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('t7_rna_polymerase_promoter','t7_rna_polymerase_promoter');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inverted_interchromosomal_transposition','invert_inter_transposition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('episome','episome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('uninverted_insertional_duplication','uninvert_insert_dup');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('free','free');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequence_difference','sequence_difference');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h4k5_acylation_site','h4k5_acylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_d_dj_c_cluster','v_d_dj_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequence_conflict','sequence_conflict');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nmd_transcript_variant','nmd_transcript_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tiling_path_clone','tiling_path_clone');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('group_iii_intron','group_iii_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_glycine','modified_glycine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequence_alteration','sequence_alteration');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polyploid','polyploid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mathematically_defined_repeat','mathematically_defined_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_silenced_by_dna_modification','gene_silenced_by_dna_modification');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_vj_j_cluster','v_vj_j_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('isoleucine_trna_primary_transcript','isoleucine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_small_subunit_primary_transcript','rrna_small_subunit_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ltr_component','ltr_component');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('plus_2_framshift','plus_2_framshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('translational_product_structure_variant','translational_product_structure_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('glutamic_acid_trna_primary_transcript','glutamic_acid_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_rearranged_at_dna_level','gene_rearranged_at_dna_level');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('edited_transcript','edited_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('invalidated_by_partial_processing','invalidated_by_partial_processing');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('increased_transcript_stability_variant','increased_transcript_stability_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequencing_primer','sequencing_primer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cleaved_for_gpi_anchor_region','cleaved_for_gpi_anchor_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_cysteine','modified_l_cysteine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_utr','five_prime_utr');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('restriction_enzyme_recognition_site','restriction_enzyme_recognition_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('frt_site','frt_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('terminal_inverted_repeat','terminal_inverted_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('centromere_dna_element_i','centromere_dna_element_i');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transition','transition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('deletion_junction','deletion_junction');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_turn_right_handed_type_one','beta_turn_right_handed_type_one');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_prime_o_ribosylguanosine_phosphate','two_prime_o_ribosylguanosine_phosphate');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_carbamoylmethyl_two_prime_o_methyluridine','five_cm_2_prime_o_methU');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('internal_transcribed_spacer_region','internal_transcribed_spacer_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dicistronic','dicistronic');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('supported_by_sequence_similarity','supported_by_sequence_similarity');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('reverse_primer','reverse_primer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u3_three_prime_ltr_region','u3_three_prime_ltr_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('glutamine_trna_primary_transcript','glutamine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rnapol_ii_promoter','rnapol_ii_promoter');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('overlapping','overlapping');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('alpha_beta_motif','alpha_beta_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered_transposable_element','engineered_transposable_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('forward_primer','forward_primer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('attctn_site','attctn_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_d_recombination_signal_sequence','five_prime_d_recombination_signal_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u6_snrna','u6_snrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recombinationally_rearranged_gene','recombinationally_rearranged_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n6_threonylcarbamoyladenosine','n6_threonylcarbamoyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_carbamoylmethyluridine','five_carbamoylmethyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cds_fragment','cds_fragment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('genome','genome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('increased_translational_product_level','increased_translational_product_level');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('translational_product_level_variant','translational_product_level_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('promoter','promoter');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('protein_coding_gene','protein_coding_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u5_snrna','u5_snrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('wybutosine','wybutosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('methylwyosine','methylwyosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('large_subunit_rrna','large_subunit_rrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosomally_aberrant_genome','chromosomally_aberrant_genome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n4_2_prime_o_dimethylcytidine','n4_2_prime_o_dimethylcytidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('c_to_t_transition','c_to_t_transition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bidirectional_promoter','bidirectional_promoter');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('validated_cdna_clone','validated_cdna_clone');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('k_turn_rna_motif','k_turn_rna_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcribed_fragment','transcribed_fragment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_ust','five_prime_ust');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_utr_intron','three_prime_utr_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('retrogene','retrogene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pyrimidine_to_purine_transversion','pyrimidine_to_purine_transversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sine_element','sine_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_rst','five_prime_rst');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('utr_intron','utr_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('interchromosomal_transposition','interchromosomal_transposition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rna_sequence_secondary_structure','rna_sequence_secondary_structure');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('complex_change_in_transcript','complex_change_in_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered_foreign_transposable_element','engineered_foreign_transposable_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ds_rna_viral_sequence','ds_rna_viral_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('fosmid','fosmid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('complex_substitution','complex_substitution');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('validated','validated');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u2_snrna','u2_snrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('duplication_attribute','duplication_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('caat_signal','caat_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('c_cluster','c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('consensus_region','consensus_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vertebrate_immune_system_gene_recombination_spacer','vertebrate_immune_system_gene_recombination_spacer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_restriction_enzyme_junction','three_prime_restriction_enzyme_junction');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_trap_construct','gene_trap_construct');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rna_aptamer','rna_aptamer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcriptionally_induced','transcriptionally_induced');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intrachromosomal','intrachromosomal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nuclear_localization_signal','nuclear_localization_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rescue_region','rescue_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inversion_site_part','inversion_site_part');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('plus_2_frameshift variant','plus_2_frameshift_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('origin_of_replication','origin_of_replication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('standard_draft','standard_draft');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k79_dimethylation_site','h3k79_dimethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rna_internal_loop','rna_internal_loop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ultracontig','ultracontig');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('peptidyl','peptidyl');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_region','polypeptide_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('epigenetically_modified_region','epigenetically_modified_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transgenic_insertion','transgenic_insertion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mirna_antiguide','mirna_antiguide');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rearranged_at_dna_level','rearranged_at_dna_level');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intergenic_variant','intergenic_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_spacer','v_spacer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('strand_attribute','strand_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('stop_lost','stop_lost');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('alternatively_spliced','alternatively_spliced');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_formyl_two_prime_o_methylcytidine','five_formyl_two_prime_o_methylcytidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('plasmid_location','plasmid_location');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('four_bp_start_codon','four_bp_start_codon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('increased_transcription_rate_variant','increased_transcription_rate_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recombinationally_rearranged','recombinationally_rearranged');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('complex_3d_structural_variant','complex_3d_structural_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chimeric_cdna_clone','chimeric_cdna_clone');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tasirna_primary_transcript','tasirna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_dicistronic_transcript','gene_with_dicistronic_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_ltr_component','three_prime_ltr_component');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('retron','retron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('autopolyploid','autopolyploid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('phenylalanine','phenylalanine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('translation_regulatory_region','translation_regulatory_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transit_peptide','transit_peptide');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('amino_acid_deletion','amino_acid_deletion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_28s','rrna_28s');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('one_two_prime_o_dimethylinosine','one_two_prime_o_dimethylinosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('threonine','threonine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('a_minor_rna_motif','a_minor_rna_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('j_cluster','j_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dce','dce');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('quantitative_variant','quantitative_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('lysosomal_localization_signal','lysosomal_localization_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('d_dj_cluster','d_dj_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosome_arm','chromosome_arm');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('kinetoplast_gene','kinetoplast_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('line_element','line_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('solo_ltr','solo_ltr');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('external_transcribed_spacer_region','external_transcribed_spacer_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_transcribed_region','non_transcribed_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mirna_stem','mirna_stem');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dj_j_c_cluster','dj_j_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('hyperploid','hyperploid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cryptic','cryptic');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k9_acetylation_site','h3k9_acetylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('alpha_helix','alpha_helix');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('fusion','fusion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vdj_j_cluster','vdj_j_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('isowyosine','isowyosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('paracentric_inversion','paracentric_inversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('homing_endonuclease_binding_site','homing_endonuclease_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tna_oligo','tna_oligo');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mini_gene','mini_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('restriction_fragment','restriction_fragment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('base_pair','base_pair');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inside_intron_antiparallel','inside_intron_antiparallel');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dna_binding_site','dna_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_cytidine','modified_cytidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('hydrophobic_region_of_peptide','hydrophobic_region_of_peptide');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polycistronic_primary_transcript','polycistronic_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_proline','modified_l_proline');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('overlapping_feature_set','overlapping_feature_set');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('asx_turn_left_handed_type_two','asx_turn_left_handed_type_two');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('interchromosomal_duplication','interchromosomal_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inframe_codon_loss','inframe_codon_loss');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('substitution','substitution');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('isoleucine','isoleucine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('functional_variant','functional_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_recoding_site','three_prime_recoding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcript_stability_variant','transcript_stability_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('5kb_upstream_variant','fivekb_upstream_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('terminator_of_type_2_rnapol_iii_promoter','terminator_of_type_2_rnapol_iii_promoter');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('glycine_trna_primary_transcript','glycine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intron_variant','intron_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('regional_centromere_outer_repeat_region','regional_centromere_outer_repeat_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('replication_regulatory_region','replication_regulatory_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mating_type_region','mating_type_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_heptamer','v_heptamer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dispersed_repeat','dispersed_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('primer','primer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_domain','polypeptide_domain');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('wild_type','wild_type');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('fusion_gene','fusion_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcript_function_variant','transcript_function_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_member_region','gene_member_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('arginyl_trna','arginyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('compensatory_transcript_secondary_structure_variant','compensatory_transcript_secondary_structure_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('methylated_base_feature','methylated_base_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('uninverted_intrachromosomal_transposition','uninvert_intra_transposition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('scrna_gene','scrna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_18s','rrna_18s');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rnapol_iii_promoter_type_1','rnapol_iii_promoter_type_1');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('point_mutation','point_mutation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pseudoknot','pseudoknot');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('g_quartet','g_quartet');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('schellmann_loop','schellmann_loop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_selenocysteine','modified_l_selenocysteine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pna','pna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_coding_exon','three_prime_coding_exon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('endogenous_retroviral_gene','endogenous_retroviral_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vertebrate_immunoglobulin_t_cell_receptor_segment','vertebrate_immunoglobulin_t_cell_receptor_segment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mrna_recoded_by_translational_bypass','mrna_recoded_by_translational_bypass');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered_foreign_region','engineered_foreign_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('snorna_encoding','snorna_encoding');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_est','five_prime_est');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('foldback_element','foldback_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('srp_rna_encoding','srp_rna_encoding');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('d_j_c_cluster','d_j_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dj_c_cluster','dj_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('histone_ubiqitination_site','histone_ubiqitination_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('complex_structural_alteration','complex_structural_alteration');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_encoding','rrna_encoding');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mrna_recoded_by_codon_redefinition','mrna_recoded_by_codon_redefinition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_methyluridine','five_methyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polya_sequence','polya_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('metabolic_island','metabolic_island');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('homologous','homologous');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('immature_peptide_region','immature_peptide_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h2bk5_monomethylation_site','h2bk5_monomethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequence_attribute','sequence_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sirna','sirna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dart_marker','dart_marker');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nucleotide_motif','nucleotide_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('plus_1_translationally_frameshifted','plus_1_translationally_frameshifted');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('trna_intron','trna_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_noncoding_exon','five_prime_noncoding_exon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dna_motif','dna_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_strand','beta_strand');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ds_oligo','ds_oligo');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('one_methyladenosine','one_methyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('oxys_rna','oxys_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('asx_motif','asx_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_hydroxyuridine','five_hydroxyuridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('coding_exon','coding_exon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('plus_1_translational_frameshift','plus_1_translational_frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_formylcytidine','five_formylcytidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k27_dimethylation_site','h3k27_dimethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('spliced_leader_rna','spliced_leader_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mitochondrial_chromosome','mitochondrial_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_fragment','gene_fragment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n2_7_2prirme_o_trimethylguanosine','n2_7_2prirme_o_trimethylguanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('frameshift','frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('propeptide_cleavage_site','propeptide_cleavage_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_methyldihydrouridine','five_methyldihydrouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('amino_acid','amino_acid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('translocation_breakpoint','translocation_breakpoint');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_5_8s','rrna_5_8s');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('helix_turn_helix','helix_turn_helix');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('simple_sequence_length_variation','simple_sequence_length_variation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('methionine','methionine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_loss_of_function_variant','polypeptide_loss_of_function_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transposable_element_gene','transposable_element_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('whole_genome_sequence_status','whole_genome_sequence_status');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('genomic_island','genomic_island');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_segment','gene_segment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('snrna_gene','snrna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered_region','engineered_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('common_variant','common_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cryptogene','cryptogene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_coding_exon_noncoding_region','three_prime_coding_exon_noncoding_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_silenced_by_rna_interference','gene_silenced_by_rna_interference');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('deficient_interchromosomal_transposition','d_interchr_transposition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('natural_variant_site','natural_variant_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('assembly','assembly');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('major_tss','major_tss');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('trna','trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('encodes_overlapping_peptides','encodes_overlapping_peptides');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nc_conserved_region','nc_conserved_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('locus_control_region','locus_control_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('s_gna_oligo','s_gna_oligo');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dna_chromosome','dna_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_turn_type_six_b','beta_turn_type_six_b');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('loss_of_heterozygosity','loss_of_heterozygosity');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered_gene','engineered_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('wobble_base_pair','wobble_base_pair');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_amino_acid_feature','modified_amino_acid_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('t_to_c_transition','t_to_c_transition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('translocaton_attribute','translocaton_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('apicoplast_sequence','apicoplast_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('terminal_codon_variant','terminal_codon_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('irlinv_site','irlinv_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('synthetic_sequence','synthetic_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('encodes_1_polypeptide','encodes_1_polypeptide');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('group_iia_intron','group_iia_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('telomere','telomere');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('interior_intron','interior_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('edited_mrna','edited_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('catmat_right_handed_three','catmat_right_handed_three');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tandem_duplication','tandem_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tmrna_gene','tmrna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pre_edited_region','pre_edited_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n6_hydroxynorvalylcarbamoyladenosine','n6_hydroxynorvalylcarbamoyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nucleomorphic_chromosome','nucleomorphic_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('fragmentary','fragmentary');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('single','single');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('binding_site','binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('seven_methylguanine','seven_methylguanine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('target_site_duplication','target_site_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vdj_gene','vdj_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bound_by_nucleic_acid','bound_by_nucleic_acid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('peptide_localization_signal','peptide_localization_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('catmat_right_handed_four','catmat_right_handed_four');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k27_acylation_site','h3k27_acylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('compound_chromosome','compound_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('coding_end','coding_end');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gap','gap');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ligand_binding_site','ligand_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('upstream_aug_codon','upstream_aug_codon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pseudogenic_transcript','pseudogenic_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('satellite_dna','satellite_dna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('assortment_derived_deficiency_plus_duplication','assortment_derived_deficiency_plus_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transposable_element','transposable_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('endogenous_retroviral_sequence','endogenous_retroviral_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('microsatellite','microsatellite');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('encodes_different_polypeptides_different_stop','encodes_different_polypeptides_different_stop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('primary_transcript','primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('consensus_mrna','consensus_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('membrane_peptide_loop','membrane_peptide_loop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('foreign','so_foreign');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rho_independent_bacterial_terminator','rho_independent_bacterial_terminator');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u_box','u_box');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_silenced_by_histone_deacetylation','gene_silenced_by_histone_deacetylation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vdj_j_c_cluster','vdj_j_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cpg_island','cpg_island');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('haplotype','haplotype');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_prime_o_methylinosine','two_prime_o_methylinosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dna','dna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('circular_double_stranded_rna_chromosome','circular_double_stranded_rna_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mature_protein_region','mature_protein_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('b_box','b_box');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_zinc_ion_contact_site','polypeptide_zinc_ion_contact_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_subarray_member','gene_subarray_member');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_cassette','gene_cassette');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('oric','oric');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('deletion_breakpoint','deletion_breakpoint');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('insertion_attribute','insertion_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mrna_with_plus_2_frameshift','mrna_with_plus_2_frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chip_seq_region','chip_seq_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('asx_turn_right_handed_type_one','asx_turn_right_handed_type_one');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcribed_cluster','transcribed_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tyrosyl_trna','tyrosyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('orthologous','orthologous');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('s_gna','s_gna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('canonical_three_prime_splice_site','canonical_three_prime_splice_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('noncoding_exon','noncoding_exon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('lethal_variant','lethal_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('minor_tss','minor_tss');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_two_prime_o_dimethylcytidine','five_two_prime_o_dimethylcytidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k36_trimethylation_site','h3k36_trimethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('macronuclear_chromosome','macronuclear_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('deficient_translocation','deficient_translocation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('read_pair','read_pair');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcript_with_translational_frameshift','transcript_with_translational_frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('finished_genome','finished_genome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rnapol_iii_promoter_type_3','rnapol_iii_promoter_type_3');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dna_transposon','dna_transposon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('orf','orf');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('right_handed_peptide_helix','right_handed_peptide_helix');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_nest_left_right_motif','polypeptide_nest_left_right_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('topology_attribute','topology_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mirtron','mirtron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_motif','polypeptide_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sl9_acceptor_site','sl9_acceptor_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('proplastid_sequence','proplastid_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('negatively_autoregulated_gene','negatively_autoregulated_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('retinoic_acid_responsive_element','retinoic_acid_responsive_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('c_d_box_snorna_encoding','c_d_box_snorna_encoding');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequence_assembly','sequence_assembly');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromoplast_gene','chromoplast_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dcaps_primer','dcaps_primer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_clip','five_prime_clip');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('golden_path','golden_path');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('splice_acceptor_variant','splice_acceptor_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('alanine','alanine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cleaved_peptide_region','cleaved_peptide_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_dj_j_cluster','v_dj_j_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pseudogenic_region','pseudogenic_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('terminator_codon_variant','terminator_codon_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('methylation_guide_snorna','methylation_guide_snorna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_vj_j_c_cluster','v_vj_j_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_canonical_start_codon','non_canonical_start_codon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_mrna_recoded_by_translational_bypass','gene_with_mrna_recoded_by_translational_bypass');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_turn_motif','polypeptide_turn_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('autocatalytically_spliced_intron','autocatalytically_spliced_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mobile','mobile');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tandem','tandem');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intron','intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('clip','clip');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dye_terminator_read','dye_terminator_read');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dmv4_motif','dmv4_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('au_rich_element','au_rich_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inversion_breakpoint','inversion_breakpoint');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dce_siii','dce_siii');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_recoding_site','five_prime_recoding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_protein_coding','non_protein_coding');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mobile_intron','mobile_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vertebrate_immunoglobulin_t_cell_receptor_rearranged_segment','vertebrate_immunoglobulin_t_cell_receptor_rearranged_segment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('st_turn_right_handed_type_one','st_turn_right_handed_type_one');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna','rrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inside_intron_parallel','inside_intron_parallel');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('spliceosomal_intron','spliceosomal_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('phagemid','phagemid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('editing_block','editing_block');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('fragment_assembly','fragment_assembly');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tmrna_acceptor_piece','tmrna_acceptor_piece');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_turn_type_six','beta_turn_type_six');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_rst','three_prime_rst');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cysteine_trna_primary_transcript','cysteine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('post_translationally_regulated_gene','post_translationally_regulated_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcriptionally_repressed','transcriptionally_repressed');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('crm','crm');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cointegrated_plasmid','cointegrated_plasmid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_sequencing_information','polypeptide_sequencing_information');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_d_spacer','three_prime_d_spacer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tiling_path_fragment','tiling_path_fragment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('natural','so_natural');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pi_helix','pi_helix');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('possible_base_call_error','possible_base_call_error');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_truncation','polypeptide_truncation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k14_acetylation_site','h3k14_acetylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('adaptive_island','adaptive_island');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('uridine_five_oxyacetic_acid','uridine_five_oxyacetic_acid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sl7_acceptor_site','sl7_acceptor_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('plus_2_translational_frameshift','plus_2_translational_frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('stop_retained_variant','stop_retained_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('homologous_region','homologous_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('500b_downstream_variant','fivehundred_b_downstream_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('internal_utr','internal_utr');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_cytoplasmic_polypeptide_region','non_cytoplasmic_polypeptide_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('experimental_feature','experimental_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nuclear_chromosome','nuclear_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('exemplar','exemplar');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rnapol_ii_core_promoter','rnapol_ii_core_promoter');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k9_methylation_site','h3k9_methylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('alanine_trna_primary_transcript','alanine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('assortment_derived_variation','assortment_derived_variation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n2_n2_dimethylguanosine','n2_n2_dimethylguanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rna_hook_turn','rna_hook_turn');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcribed_spacer_region','transcribed_spacer_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('plasmid_gene','plasmid_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u14_snorna','u14_snorna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('galactosyl_queuosine','galactosyl_queuosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cyanelle_gene','cyanelle_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('wild_type_rescue_gene','wild_type_rescue_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u12_intron','u12_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('aptamer','aptamer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recoded_mrna','recoded_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nested_transposon','nested_transposon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tf_binding_site_variant','tf_binding_site_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('macronuclear_sequence','macronuclear_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ust','ust');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('selenocysteine','selenocysteine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('elongated_out_of_frame_polypeptide_c_terminal','elongated_out_of_frame_polypeptide_c_terminal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_dicistronic_mrna','gene_with_dicistronic_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('match_part','match_part');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nucleomorphic_sequence','nucleomorphic_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('apicoplast_gene','apicoplast_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('regulon','regulon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('plasmid_vector','plasmid_vector');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_tryptophan','modified_l_tryptophan');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('free_chromosome_arm','free_chromosome_arm');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('srp_rna_primary_transcript','srp_rna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('asx_turn','asx_turn');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('anchor_binding_site','anchor_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_primary_transcript','rrna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('reading_frame','reading_frame');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k23_acylation site','h3k23_acylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('maternal_variant','maternal_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dhu_loop','dhu_loop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n4_acetylcytidine','n4_acetylcytidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('experimental_feature_attribute','experimental_feature_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('silenced_gene','silenced_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cloned_genomic_insert','cloned_genomic_insert');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intron_gain','intron_gain');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dna_sequence_secondary_structure','dna_sequence_secondary_structure');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cdna_match','cdna_match');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_flanking_region','five_prime_flanking_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pyrrolysyl_trna','pyrrolysyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_methylthio_n6_cis_hydroxyisopentenyl_adenosine','two_methylthio_n6_cis_hydroxyisopentenyl_adenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('repeat_component','repeat_component');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('one_methyl_three_three_amino_three_carboxypropyl_pseudouridine','one_methyl_3_3_amino_three_carboxypropyl_pseudouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rpra_rna','rpra_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nuclease_sensitive_site','nuclease_sensitive_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('conservative_amino_acid_substitution','conservative_amino_acid_substitution');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_coding_exon_noncoding_region','five_prime_coding_exon_noncoding_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rnapol_iii_promoter','rnapol_iii_promoter');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tryptophan_trna_primary_transcript','try_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('noncontiguous_finished','noncontiguous_finished');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('region','region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tf_binding_site','tf_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('attl_site','attl_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('natural_plasmid','natural_plasmid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('upd','upd');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('conservative_missense_codon','conservative_missense_codon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n6_n6_dimethyladenosine','n6_n6_dimethyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('laevosynaptic_chromosome','laevosynaptic_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosomal_structural_element','chromosomal_structural_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_cassette_array','gene_cassette_array');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vertebrate_immunoglobulin_t_cell_receptor_gene_cluster','vertebrate_immunoglobulin_t_cell_receptor_gene_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('canonical_five_prime_splice_site','canonical_five_prime_splice_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bound_by_protein','bound_by_protein');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sts_map','sts_map');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dnazyme','dnazyme');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('silent_mutation','silent_mutation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_d_j_cluster','v_d_j_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('distal_promoter_element','distal_promoter_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bipartite_duplication','bipartite_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('hydroxywybutosine','hydroxywybutosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dihydrouridine','dihydrouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_coding_exon_coding_region','five_prime_coding_exon_coding_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_turn_left_handed_type_one','beta_turn_left_handed_type_one');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k4_trimethylation','h3k4_trimethylation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recoded_codon','recoded_codon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('predicted','predicted');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('resolution_site','resolution_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('seven_cyano_seven_deazaguanosine','seven_cyano_seven_deazaguanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('disease_associated_variant','disease_associated_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('conformational_switch','conformational_switch');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('regulated','regulated');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inverted_repeat','inverted_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('t_to_a_transversion','t_to_a_transversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('attc_site','attc_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_methyladenosine','two_methyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cross_genome_match','cross_genome_match');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tandem_repeat','tandem_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('copy_number_loss','copy_number_loss');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('antisense_primary_transcript','antisense_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequence_collection','sequence_collection');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_polyadenylated_mrna','gene_with_polyadenylated_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rnapol_i_promoter','rnapol_i_promoter');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_methyluridine','three_methyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('start_codon','start_codon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('retrotransposon','retrotransposon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('elongated_in_frame_polypeptide_c_terminal','elongated_in_frame_polypeptide_c_terminal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_gene','v_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chloroplast_dna','chloroplast_dna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('negative_sense_ssrna_viral_sequence','negative_sense_ssrna_viral_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('primer_binding_site','primer_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('c_box','c_box');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('plasmid','plasmid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('biological_region','biological_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('g_to_a_transition','g_to_a_transition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_canonical_five_prime_splice_site','non_canonical_five_prime_splice_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('c_d_box_snorna_primary_transcript','c_d_box_snorna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('trna_region','trna_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n6_cis_hydroxyisopentenyl_adenosine','n6_cis_hydroxyisopentenyl_adenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chloroplast_sequence','chloroplast_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('variant_frequency','variant_frequency');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('exon_region','exon_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('r_five_prime_ltr_region','r_five_prime_ltr_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_j_c_cluster','v_j_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('r_three_prime_ltr_region','r_three_prime_ltr_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('snrna','snrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('one_methylinosine','one_methylinosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inframe_codon_gain','inframe_codon_gain');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('j_gene_recombination_feature','j_gene_recombination_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_structural_motif','polypeptide_structural_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('conserved_region','conserved_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sl3_acceptor_site','sl3_acceptor_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('remark','remark');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('fixed_variant','fixed_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_dna_contact','polypeptide_dna_contact');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('codon','codon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_23s','rrna_23s');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_gain_of_function_variant','polypeptide_gain_of_function_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mrna','mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('glycyl_trna','glycyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cyanelle_sequence','cyanelle_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cds_independently_known','cds_independently_known');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('insulator','insulator');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('positive_sense_ssrna_viral_sequence','positive_sense_ssrna_viral_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sticky_end_restriction_enzyme_cleavage_site','sticky_end_restriction_enzyme_cleavage_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('expressed_sequence_match','expressed_sequence_match');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('possible_assembly_error','possible_assembly_error');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u3_snorna','u3_snorna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_manganese_ion_contact_site','polypeptide_manganese_ion_contact_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h4k16_acylation_site','h4k16_acylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('g_to_t_transversion','g_to_t_transversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recombination_feature_of_rearranged_gene','recombination_feature_of_rearranged_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_coding_exon_coding_region','three_prime_coding_exon_coding_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tmrna_primary_transcript','tmrna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('single_stranded_cdna','single_stranded_cdna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('experimentally_determined','experimentally_determined');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pseudogenic_exon','pseudogenic_exon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u2_intron','u2_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosome','chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('encodes_alternately_spliced_transcripts','encodes_alternately_spliced_transcripts');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('aberrant_processed_transcript','aberrant_processed_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('complex_change_of_translational_product_variant','complex_change_of_translational_product_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gna','gna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dsra_rna','dsra_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intron_domain','intron_domain');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cds_predicted','cds_predicted');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_asparagine','modified_l_asparagine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inframe_variant','inframe_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_d_nonamer','five_prime_d_nonamer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sl2_acceptor_site','sl2_acceptor_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_localization_variant','polypeptide_localization_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dce_si','dce_si');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('snrna_primary_transcript','snrna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('translocation','translocation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k27_methylation_site','h3k27_methylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_phenylalanine','modified_l_phenylalanine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('lincrna','lincrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_valine','modified_l_valine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('yac','yac');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('signal_peptide','signal_peptide');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('r_ltr_region','r_ltr_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('srp_rna_gene','srp_rna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('elongated_polypeptide_n_terminal','elongated_polypeptide_n_terminal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recombination_hotspot','recombination_hotspot');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_dj_c_cluster','v_dj_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('viral_sequence','viral_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_terminal_inverted_repeat','five_prime_terminal_inverted_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_methoxycarbonylmethyl_two_thiouridine','five_mcm_2_thiouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('edited','edited');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('breu_motif','breu_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('coding_start','coding_start');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k9_monomethylation_site','h3k9_monomethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_utr','three_prime_utr');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dpe1_motif','dpe1_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_tyrosine','modified_l_tyrosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_d_j_c_cluster','v_d_j_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('silenced_by_histone_methylation','silenced_by_histone_methylation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('deficient_inversion','deficient_inversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('decreased_transcript_level_variant','decreased_transcript_level_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_thiouridine','two_thiouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polyadenylation_variant','polyadenylation_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rna_polymerase_iii_tata_box','rna_polymerase_iii_tata_box');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_thio_two_prime_o_methyluridine','two_thio_two_prime_o_methyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k18_acetylation_site','h3k18_acetylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('leucoplast_sequence','leucoplast_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cds','cds');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polya_signal_sequence','polya_signal_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('micronuclear_sequence','micronuclear_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('glutamyl_trna','glutamyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k27_monomethylation_site','h3k27_monomethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('minus_2_frameshift_variant','minus_2_frameshift_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('strna_gene','strna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('paternally_imprinted_gene','paternally_imprinted_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rna_chromosome','rna_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ndm3_motif','ndm3_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u1_snrna','u1_snrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_hydroxymethylcytidine','five_hydroxymethylcytidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recombination_feature','recombination_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('encodes_disjoint_polypeptides','encodes_disjoint_polypeptides');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('post_translationally_regulated','post_translationally_regulated');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered_fusion_gene','engineered_fusion_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_d_recombination_signal_sequence','three_prime_d_recombination_signal_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intermediate','intermediate');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_calcium_ion_contact_site','polypeptide_calcium_ion_contact_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('syntenic_region','syntenic_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('variant_collection','variant_collection');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cryptic_splice_donor','cryptic_splice_donor');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('assembly_error_correction','assembly_error_correction');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sugar_edge_base_pair','sugar_edge_base_pair');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered_foreign_gene','engineered_foreign_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k4_monomethylation_site','h3k4_monomethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n4_acetyl_2_prime_o_methylcytidine','n4_acetyl_2_prime_o_methylcytidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('predicted_by_ab_initio_computation','predicted_by_ab_initio_computation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_secondary_structure','polypeptide_secondary_structure');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ncrna_gene','ncrna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rna_junction_loop','rna_junction_loop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('haplotype_block','haplotype_block');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('oriv','oriv');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('peptide_collection','peptide_collection');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ambisense_ssrna_viral_sequence','ambisense_ssrna_viral_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('morpholino_oligo','morpholino_oligo');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('centromere','centromere');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('epigenetically_modified_gene','epigenetically_modified_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosomal_inversion','chromosomal_inversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('minus_35_signal','minus_35_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_two_prime_o_dimethyluridine','three_two_prime_o_dimethyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('four_thiouridine','four_thiouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcription_end_site','transcription_end_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pirna_gene','pirna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pre_mirna','pre_mirna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cysteinyl_trna','cysteinyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('catmat_left_handed_three','catmat_left_handed_three');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cryptic_splice_acceptor','cryptic_splice_acceptor');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('schellmann_loop_seven','schellmann_loop_seven');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_vdj_j_c_cluster','v_vdj_j_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transposable_element_insertion_site','transposable_element_insertion_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('translocation_element','translocation_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mirna_primary_transcript_region','mirna_primary_transcript_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('orphan_cds','orphan_cds');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('monocistronic_mrna','monocistronic_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('natural_transposable_element','natural_transposable_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('golden_path_fragment','golden_path_fragment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('lipoprotein_signal_peptide','lipoprotein_signal_peptide');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('arginine','arginine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('linear_double_stranded_rna_chromosome','linear_double_stranded_rna_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h4k8_acylation site','h4k8_acylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_large_subunit_primary_transcript','rrna_large_subunit_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('irrinv_site','irrinv_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('plastid_sequence','plastid_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('base_call_error_correction','base_call_error_correction');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('integrated_plasmid','integrated_plasmid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_methionine','modified_l_methionine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('linear_single_stranded_rna_chromosome','linear_single_stranded_rna_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromoplast_sequence','chromoplast_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('proximal_promoter_element','proximal_promoter_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('contig_read','contig_read');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('promoter_trap_construct','promoter_trap_construct');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('stop_codon_redefined_as_selenocysteine','stop_codon_redefined_as_selenocysteine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('seven_methylguanosine','seven_methylguanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gamma_turn','gamma_turn');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tmrna','tmrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('methionyl_trna','methionyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('synonymous_codon','synonymous_codon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cdna','cdna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sl4_acceptor_site','sl4_acceptor_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nuclease_binding_site','nuclease_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('uridine_five_oxyacetic_acid_methyl_ester','uridine_five_oxyacetic_acid_methyl_ester');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_race_clone','three_prime_race_clone');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('circular_double_stranded_dna_chromosome','circular_double_stranded_dna_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('consensus','consensus');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('positively_autoregulated_gene','positively_autoregulated_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tss_region','tss_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_histidine','modified_l_histidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('unitary_pseudogene','unitary_pseudogene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_metal_contact','polypeptide_metal_contact');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('integron','integron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('d_loop','d_loop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('decayed_exon','decayed_exon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recombination_signal_sequence','recombination_signal_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_inosine','modified_inosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_three_prime_overlap','three_prime_three_prime_overlap');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_j_cluster','v_j_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_five_prime_overlap','three_prime_five_prime_overlap');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recombination_regulatory_region','recombination_regulatory_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_bulge_loop','beta_bulge_loop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('restriction_enzyme_cleavage_junction','restriction_enzyme_cleavage_junction');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('blunt_end_restriction_enzyme_cleavage_junction','blunt_end_restriction_enzyme_cleavage_junction');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intergenic_region','intergenic_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dmv2_motif','dmv2_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intrachromosomal_mutation','intrachromosomal_mutation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('antisense_rna','antisense_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequence_feature','sequence_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n6_glycinylcarbamoyladenosine','n6_glycinylcarbamoyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gamma_turn_classic','gamma_turn_classic');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_cis_splice_site','three_prime_cis_splice_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rapd','rapd');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inverted_ring_chromosome','inverted_ring_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cca_tail','cca_tail');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('linear_double_stranded_dna_chromosome','linear_double_stranded_dna_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u5_five_prime_ltr_region','u5_five_prime_ltr_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bruno_response_element','bruno_response_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('t_to_g_transversion','t_to_g_transversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('c_to_a_transversion','c_to_a_transversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('macronucleus_destined_segment','macronucleus_destined_segment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('distant_three_prime_recoding_signal','distant_three_prime_recoding_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pre_edited_mrna','pre_edited_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('p_element','p_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pac','pac');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_fusion','gene_fusion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('base','base');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('codon_redefined','codon_redefined');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polycistronic_mrna','polycistronic_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('codon_variant','codon_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_methoxycarbonylmethyl_two_prime_o_methyluridine','five_methoxycarbonylmethyl_two_prime_o_methyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('match','match');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_stop_codon_read_through','gene_with_stop_codon_read_through');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('asparaginyl_trna','asparaginyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('threonyl_trna','threonyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u3_five_prime_ltr_region','u3_five_prime_ltr_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_ltr','five_prime_ltr');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vj_gene','vj_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rho_dependent_bacterial_terminator','rho_dependent_bacterial_terminator');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n2_methylguanosine','n2_methylguanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_flanking_region','three_prime_flanking_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('genomically_contaminated_cdna_clone','genomically_contaminated_cdna_clone');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('internal_guide_sequence','internal_guide_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mirna_target_site','mirna_target_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u5_three_prime_ltr_region','u5_three_prime_ltr_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('undermodified_hydroxywybutosine','undermodified_hydroxywybutosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('introgressed_chromosome_region','introgressed_chromosome_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('translationally_frameshifted','translationally_frameshifted');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('trans_spliced','trans_spliced');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('methylation_guide_snorna_primary_transcript','methylation_guide_snorna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('leucine','leucine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosomal_deletion','chromosomal_deletion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_isopentenylaminomethyl_uridine','five_isopentenylaminomethyl_uridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('stop_codon','stop_codon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_methylthio_n6_threonyl_carbamoyladenosine','two_methylthio_n6_threonyl_carbamoyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('decreased_polyadenylation_variant','decreased_polyadenylation_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('biochemical_region_of_peptide','biochemical_region_of_peptide');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('interband','interband');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dna_constraint_sequence','dna_constraint_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('clone_insert','clone_insert');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('snp','snp');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromoplast_chromosome','chromoplast_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_25s','rrna_25s');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tata_box','tata_box');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('plastid_gene','plastid_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('asx_turn_left_handed_type_one','asx_turn_left_handed_type_one');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_uridine','modified_uridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dce_sii','dce_sii');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intramembrane_polypeptide_region','intramembrane_polypeptide_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('lysyl_trna','lysyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rr_tract','rr_tract');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_primary_transcript_region','rrna_primary_transcript_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h4k20_monomethylation_site','h4k20_monomethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ds_dna_viral_sequence','ds_dna_viral_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('alternatively_spliced_transcript','alternatively_spliced_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_copper_ion_contact_site','polypeptide_copper_ion_contact_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('scrna_encoding','scrna_encoding');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosomal_duplication','chromosomal_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('clone','clone');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_isoleucine','modified_l_isoleucine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_subarray','gene_subarray');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('hetero_compound_chromosome','hetero_compound_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dicistronic_transcript','dicistronic_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inactive_ligand_binding_site','inactive_ligand_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('silenced_by_dna_methylation','silenced_by_dna_methylation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sl6_acceptor_site','sl6_acceptor_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('t_loop','t_loop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('targeting_vector','targeting_vector');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_thiocytidine','two_thiocytidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_serine','modified_l_serine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('srp_rna','srp_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_repeat_recoding_signal','three_prime_repeat_recoding_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rate_of_transcription_variant','rate_of_transcription_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_prime_o_methylguanosine','two_prime_o_methylguanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_binding_motif','polypeptide_binding_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recombinationally_rearranged_vertebrate_immune_system_gene','recombinationally_rearranged_vertebrate_immune_system_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('a_box','a_box');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('splicing_variant','splicing_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('one_methylguanosine','one_methylguanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_glutamine','modified_l_glutamine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequence_variant','sequence_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequence_length_variation','sequence_length_variation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('strna_encoding','strna_encoding');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('plus_1_frameshift','plus_1_frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('topologically_defined_region','topologically_defined_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('edited_cds','edited_cds');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_silenced_by_histone_modification','gene_silenced_by_histone_modification');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('e_box_motif','e_box_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('paternal_uniparental_disomy','paternal_uniparental_disomy');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('small_subunit_rrna','small_subunit_rrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dnasei_hypersensitive_site','dnasei_hypersensitive_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_d_dj_j_c_cluster','v_d_dj_j_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('compound_chromosome_arm','compound_chromosome_arm');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('score','score');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('feature_attribute','feature_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('protein_match','protein_match');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('downstream_gene_variant','downstream_gene_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sl10_accceptor_site','sl10_accceptor_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosome_variation','chromosome_variation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('serine_threonine_motif','serine_threonine_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('a_box_type_1','a_box_type_1');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('allelically_excluded','allelically_excluded');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('assortment_derived_aneuploid','assortment_derived_aneuploid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rare_variant','rare_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_isopentenylaminomethyl_two_prime_o_methyluridine','five_isopentenylaminomethyl_two_prime_o_methyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('regional_centromere_central_core','regional_centromere_central_core');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gna_oligo','gna_oligo');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nc_transcript_variant','nc_transcript_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('copy_number_variation','copy_number_variation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('silenced','silenced');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_methylcytidine','three_methylcytidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dexstrosynaptic_chromosome','dexstrosynaptic_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inverted_insertional_duplication','inverted_insertional_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rescue_mini_gene','rescue_mini_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('catmat_left_handed_four','catmat_left_handed_four');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('alternate_sequence_site','alternate_sequence_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_iron_ion_contact_site','polypeptide_iron_ion_contact_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('c_to_t_transition_at_pcpg_site','c_to_t_transition_at_pcpg_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_molybdenum_ion_contact_site','polypeptide_molybdenum_ion_contact_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('phenylalanine_trna_primary_transcript','phe_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('decreased_translational_product_level','decreased_translational_product_level');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h_aca_box_snorna_primary_transcript','h_aca_box_snorna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('r_gna','r_gna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('coding_sequence_variant','coding_sequence_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_partial_loss_of_function','polypeptide_partial_loss_of_function');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_restriction_enzyme_junction','five_prime_restriction_enzyme_junction');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intrachromosomal_transposition','intrachromosomal_transposition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('methylated_a','methylated_a');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_16s','rrna_16s');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('disease_causing_variant','disease_causing_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n4_methylcytidine','n4_methylcytidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('riboswitch','riboswitch');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('duplicated_pseudogene','duplicated_pseudogene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('assortment_derived_duplication','assortment_derived_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosomal_regulatory_element','chromosomal_regulatory_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_five_prime_overlap','five_prime_five_prime_overlap');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_function_variant','polypeptide_function_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ribozymic','ribozymic');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inversion_derived_bipartite_deficiency','inversion_derived_bipartite_deficiency');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('invalidated_by_genomic_contamination','invalidated_by_genomic_contamination');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('variant_genome','variant_genome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vj_j_cluster','vj_j_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosome_number_variation','chromosome_number_variation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_attribute','gene_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('uag_stop_codon_signal','uag_stop_codon_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nucleotide_match','nucleotide_match');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mature_mirna_variant','mature_mirna_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('st_turn_left_handed_type_two','st_turn_left_handed_type_two');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('epigenetically_modified','epigenetically_modified');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inversion_derived_deficiency_plus_duplication','inversion_derived_deficiency_plus_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_methylaminomethyl_two_selenouridine','five_methylaminomethyl_two_selenouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('aspartic_acid_trna_primary_transcript','aspartic_acid_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nuclear_mt_pseudogene','nuclear_mt_pseudogene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('exonic_splice_enhancer','exonic_splice_enhancer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u4_snrna','u4_snrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('csrb_rsmb_rna','csrb_rsmb_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('group_1_intron_homing_endonuclease_target_region','group_1_intron_homing_endonuclease_target_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('crispr','crispr');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('snorna_gene','snorna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('trans_splice_junction','trans_splice_junction');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('flanking_three_prime_quadruplet_recoding_signal','flanking_three_prime_quadruplet_recoding_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_vdj_j_cluster','v_vdj_j_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cassette_pseudogene','cassette_pseudogene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('incomplete_terminal_codon_variant','incomplete_terminal_codon_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('silenced_by_histone_modification','silenced_by_histone_modification');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('proviral_gene','proviral_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_carboxyhydroxymethyl_uridine','five_carboxyhydroxymethyl_uridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mt_gene','mt_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('single_stranded_rna_chromosome','single_stranded_rna_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recoding_stimulatory_region','recoding_stimulatory_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_taurinomethyluridine','five_taurinomethyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_threonine','modified_l_threonine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_d_dj_cluster','v_d_dj_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('synthetic_oligo','synthetic_oligo');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('purine_to_pyrimidine_transversion','purine_to_pyrimidine_transversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('editing_variant','editing_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('antiparallel_beta_strand','antiparallel_beta_strand');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('central_hydrophobic_region_of_signal_peptide','central_hydrophobic_region_of_signal_peptide');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('integrated_mobile_genetic_element','integrated_mobile_genetic_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('parallel_beta_strand','parallel_beta_strand');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_dj_cluster','v_dj_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dre_motif','dre_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_ltr_retrotransposon','non_ltr_retrotransposon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('r_gna_oligo','r_gna_oligo');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('autoregulated','autoregulated');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_lysine','modified_l_lysine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bac_end','bac_end');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pyrrolysine','pyrrolysine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('high_quality_draft','high_quality_draft');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('lysine','lysine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('elongated_polypeptide','elongated_polypeptide');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('unique_variant','unique_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('protein_protein_contact','protein_protein_contact');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inversion_attribute','inversion_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nucleotide_binding_site','nucleotide_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('splice_site','splice_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_synonymous_codon','non_synonymous_codon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('5kb_downstream_variant','fivekb_downstream_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosomal_translocation','chromosomal_translocation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('epitope','epitope');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('allele','allele');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n4_n4_2_prime_o_trimethylcytidine','n4_n4_2_prime_o_trimethylcytidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u5_ltr_region','u5_ltr_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('paired_end_fragment','paired_end_fragment');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rescue_gene','rescue_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transgenic_transposable_element','transgenic_transposable_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_conserved_region','polypeptide_conserved_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sts','sts');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('a_to_c_transversion','a_to_c_transversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('class_ii_rna','class_ii_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nonamer_of_recombination_feature_of_vertebrate_immune_system_gene','nonamer_of_recombination_feature_of_vertebrate_im_sys_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('unedited_region','unedited_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('lambda_vector','lambda_vector');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene','gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('alanyl_trna','alanyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('amino_acid_substitution','amino_acid_substitution');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('virtual_sequence','virtual_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('group_iib_intron','group_iib_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('retrotransposed','retrotransposed');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mrna_with_minus_2_frameshift','mrna_with_minus_2_frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polymer_attribute','polymer_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('autosynaptic_chromosome','autosynaptic_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('peptide_helix','peptide_helix');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('partially_processed_cdna_clone','partially_processed_cdna_clone');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rst_match','rst_match');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('paternally_imprinted','paternally_imprinted');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('predicted_gene','predicted_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('x_element_combinatorial_repeat','x_element_combinatorial_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('robertsonian_fusion','robertsonian_fusion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_prime_o_methylpseudouridine','two_prime_o_methylpseudouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pericentric_inversion','pericentric_inversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('aspartyl_trna','aspartyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('strna','strna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_intron','three_prime_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('linear','linear');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('j_nonamer','j_nonamer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('a_to_t_transversion','a_to_t_transversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('idna','idna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n2_n2_7_trimethylguanosine','n2_n2_7_trimethylguanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('complex_chromosomal_mutation','complex_chromosomal_mutation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inversion_derived_deficiency_plus_aneuploid','inversion_derived_deficiency_plus_aneuploid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k4_methylation_site','h3k4_methylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('asymmetric_rna_internal_loop','asymmetric_rna_internal_loop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('deletion','deletion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k79_monomethylation_site','h3k79_monomethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cyclic_translocation','cyclic_translocation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ars','ars');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('glutaminyl_trna','glutaminyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('allopolyploid','allopolyploid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('replicon','replicon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_prime_o_methylcytidine','two_prime_o_methylcytidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('regional_centromere','regional_centromere');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('st_turn_left_handed_type_one','st_turn_left_handed_type_one');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('paralogous_region','paralogous_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mature_transcript_region','mature_transcript_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mrna_with_frameshift','mrna_with_frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('reference_genome','reference_genome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('unoriented_interchromosomal_transposition','unoriented_interchromosomal_transposition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('d_dj_j_cluster','d_dj_j_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('maxicircle_gene','maxicircle_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('st_turn_right_handed_type_two','st_turn_right_handed_type_two');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('snrna_encoding','snrna_encoding');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('annotation_directed_improved_draft','annotation_directed_improved_draft');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_d_spacer','five_prime_d_spacer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('read','read');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('arginine_trna_primary_transcript','arg_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('oligo_u_tail','oligo_u_tail');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recoding_pseudoknot','recoding_pseudoknot');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_methylaminomethyl_two_thiouridine','five_mam_2_thiouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('monocistronic','monocistronic');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('3d_polypeptide_structure_variant','threed_polypeptide_structure_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transmembrane_polypeptide_region','transmembrane_polypeptide_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcript_processing_variant','transcript_processing_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vector_replicon','vector_replicon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('maternal_uniparental_disomy','maternal_uniparental_disomy');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pyrosequenced_read','pyrosequenced_read');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('splice_site_variant','splice_site_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_magnesium_ion_contact_site','polypeptide_magnesium_ion_contact_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polycistronic_transcript','polycistronic_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polya_site','polya_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosomal_variation_attribute','chromosomal_variation_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('free_duplication','free_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosome_structure_variation','chromosome_structure_variation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_rna_base_feature','modified_rna_base_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mutated_variant_site','mutated_variant_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gaga_motif','gaga_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('centromeric_repeat','centromeric_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_gene','rrna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('interchromosomal_mutation','interchromosomal_mutation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('prophage','prophage');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('syntenic','syntenic');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered_foreign_repetitive_element','engineered_foreign_repetitive_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('translated_nucleotide_match','translated_nucleotide_match');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('exon_variant','exon_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h_aca_box_snorna','h_aca_box_snorna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vault_rna','vault_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('orphan','orphan');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('linear_single_stranded_dna_chromosome','linear_single_stranded_dna_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('telomeric_repeat','telomeric_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('encodes_greater_than_1_polypeptide','encodes_greater_than_1_polypeptide');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('atti_site','atti_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_start_codon_cug','gene_with_start_codon_cug');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('silenced_by_histone_deacetylation','silenced_by_histone_deacetylation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('reagent','reagent');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosome_fission','chromosome_fission');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ct_gene','ct_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('capped_primary_transcript','capped_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('methylinosine','methylinosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('j_spacer','j_spacer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('glutamine','glutamine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_guanosine','modified_guanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n2_7_dimethylguanosine','n2_7_dimethylguanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k79_trimethylation_site','h3k79_trimethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_d_heptamer','three_prime_d_heptamer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('invalidated_cdna_clone','invalidated_cdna_clone');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('terminator','terminator');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('stem_loop','stem_loop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_utr_intron','five_prime_utr_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('unoriented_intrachromosomal_transposition','unoriented_intrachromosomal_transposition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosome_band','chromosome_band');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mini_exon_donor_rna','mini_exon_donor_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('aneuploid','aneuploid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_methyl_2_thiouridine','five_methyl_2_thiouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_silenced_by_dna_methylation','gene_silenced_by_dna_methylation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered_foreign_transposable_element_gene','engineered_foreign_transposable_element_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('processed_pseudogene','processed_pseudogene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('supercontig','supercontig');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('trna_encoding','trna_encoding');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('reciprocal_chromosomal_translocation','reciprocal_chromosomal_translocation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tasirna','tasirna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('hoogsteen_base_pair','hoogsteen_base_pair');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('regional_centromere_inner_repeat_region','regional_centromere_inner_repeat_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('yac_end','yac_end');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('purine_transition','purine_transition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('c_d_box_snorna','c_d_box_snorna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('snorna_primary_transcript','snorna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_vj_cluster','v_vj_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intronic_regulatory_region','intronic_regulatory_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u3_ltr_region','u3_ltr_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('attenuator','attenuator');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_noncoding_exon','three_prime_noncoding_exon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u14_snorna_primary_transcript','u14_snorna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('d_gene_recombination_feature','d_gene_recombination_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mte','mte');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gcvb_rna','gcvb_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rst','rst');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('operator','operator');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ring_chromosome','ring_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ndm2_motif','ndm2_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k4_dimethylation_site','h3k4_dimethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('selenocysteine_trna_primary_transcript','selenocysteine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('edited_transcript_feature','edited_transcript_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('stop_codon_redefined_as_pyrrolysine','stop_codon_redefined_as_pyrrolysine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('homo_compound_chromosome','homo_compound_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('foreign_gene','foreign_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('four_demethylwyosine','four_demethylwyosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('guide_rna','guide_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_methylpseudouridine','three_methylpseudouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inversion_derived_aneuploid_chromosome','inversion_derived_aneuploid_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('decreased_transcript_stability_variant','decreased_transcript_stability_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('lincrna_gene','lincrna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('specific_recombination_site','specific_recombination_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inosine','inosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('foreign_transposable_element','foreign_transposable_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('d_gene','d_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bipartite_inversion','bipartite_inversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered_plasmid','engineered_plasmid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_group_regulatory_region','gene_group_regulatory_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vd_gene','vd_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('regulatory_region','regulatory_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sl11_acceptor_site','sl11_acceptor_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('guide_rna_region','guide_rna_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_base','modified_base');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('elongated_polypeptide_c_terminal','elongated_polypeptide_c_terminal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_ten_helix','three_ten_helix');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('elongated_in_frame_polypeptide_n_terminal_elongation','elongated_in_frame_polypeptide_n_terminal_elongation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sarcin_like_rna_motif','sarcin_like_rna_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('minus_1_translationally_frameshifted','minus_1_translationally_frameshifted');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_alanine','modified_l_alanine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inversion_cum_translocation','inversion_cum_translocation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tag','tag');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('uninverted_interchromosomal_transposition','uninvert_inter_transposition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cryptic_gene','cryptic_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pericentric','pericentric');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transgenic','transgenic');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('genomic_clone','genomic_clone');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chromosome_breakage_sequence','chromosome_breakage_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('d_j_cluster','d_j_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_turn_type_six_a_one','beta_turn_type_six_a_one');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ribosome_entry_site','ribosome_entry_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('left_handed_peptide_helix','left_handed_peptide_helix');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dna_aptamer','dna_aptamer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('i_motif','i_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('deficient_intrachromosomal_transposition','d_intrachr_transposition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('single_stranded_dna_chromosome','single_stranded_dna_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('methylated_c','methylated_c');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('clone_end','clone_end');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ligation_based_read','ligation_based_read');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('expressed_sequence_assembly','expressed_sequence_assembly');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_two_prime_o_dimethyluridine','five_two_prime_o_dimethyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('histidine_trna_primary_transcript','histidine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('orthologous_region','orthologous_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('valine_trna_primary_transcript','valine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('operon_member','operon_member');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('missense_codon','missense_codon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bacterial_rnapol_promoter_sigma54','bacterial_rnapol_promoter_sigma54');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_group','gene_group');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('small_regulatory_ncrna','small_regulatory_ncrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intrachromosomal_duplication','intrachromosomal_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_conservative_amino_acid_substitution','non_conservative_amino_acid_substitution');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('uaa_stop_codon_signal','uaa_stop_codon_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k36_methylation_site','h3k36_methylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcript_variant','transcript_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('minus_2_frameshift','minus_2_frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('organelle_sequence','organelle_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('initiator_codon_change','initiator_codon_change');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cryptic_prophage','cryptic_prophage');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('micf_rna','micf_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('direct_tandem_duplication','direct_tandem_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('conserved','conserved');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('telomerase_rna','telomerase_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u6atac_snrna','u6atac_snrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('attb_site','attb_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_array_member','gene_array_member');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polyadenylated_mrna','polyadenylated_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('symbiosis_island','symbiosis_island');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polymorphic_variant','polymorphic_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('splice_junction','splice_junction');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('fingerprint_map','fingerprint_map');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('single_strand_restriction_enzyme_cleavage_site','single_strand_restriction_enzyme_cleavage_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('wyosine','wyosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('uga_stop_codon_signal','uga_stop_codon_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('minus_24_signal','minus_24_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cytoplasmic_polypeptide_region','cytoplasmic_polypeptide_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h4k_acylation_region','h4k_acylation_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('one_two_prime_o_dimethylguanosine','one_two_prime_o_dimethylguanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rh_map','rh_map');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('silenced_by_dna_modification','silenced_by_dna_modification');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inactive_catalytic_site','inactive_catalytic_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('anticodon','anticodon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('seven_deazaguanosine','seven_deazaguanosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('asparagine','asparagine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('probe','probe');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('defective_conjugative_transposon','defective_conjugative_transposon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('archaeosine','archaeosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('molecular_contact_region','molecular_contact_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nested_repeat','nested_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('membrane_structure','membrane_structure');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('contig_collection','contig_collection');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tetraloop','tetraloop');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('coding_conserved_region','coding_conserved_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('long_terminal_repeat','long_terminal_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vertebrate_immune_system_gene_recombination_signal_feature','vertebrate_immune_system_gene_recombination_signal_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('untranslated_region_polycistronic_mrna','untranslated_region_polycistronic_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('leucine_trna_primary_transcript','leucine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('high_identity_region','high_identity_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('circular_single_stranded_dna_chromosome','circular_single_stranded_dna_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nuclear_rim_localization_signal','nuclear_rim_localization_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('isoleucyl_trna','isoleucyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('insertion_breakpoint','insertion_breakpoint');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('5_prime_utr_variant','five_prime_utr_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transgene','transgene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mrna_region','mrna_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcript_attribute','transcript_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('serine_threonine_staple_motif','serine_threonine_staple_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('protein_coding','protein_coding');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('enhancer_bound_by_factor','enhancer_bound_by_factor');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('recoded_by_translational_bypass','recoded_by_translational_bypass');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('operon','operon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('monocistronic_transcript','monocistronic_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('reciprocal','reciprocal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polyadenylated','polyadenylated');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('unigene_cluster','unigene_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('vertebrate_immunoglobulin_t_cell_receptor_rearranged_gene_cluster','vertebrate_ig_t_cell_receptor_rearranged_gene_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_cassette_member','gene_cassette_member');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('stop_codon_read_through','stop_codon_read_through');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_transcript_with_translational_frameshift','gene_with_transcript_with_translational_frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('variant_quality','variant_quality');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mnp','mnp');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('glutamic_acid','glutamic_acid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('y_prime_element','y_prime_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_turn','beta_turn');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pathogenic_island','pathogenic_island');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ust_match','ust_match');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcript_bound_by_protein','transcript_bound_by_protein');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n6_methyladenosine','n6_methyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cyanelle_chromosome','cyanelle_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('orit','orit');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('maternally_imprinted','maternally_imprinted');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('chloroplast_chromosome','chloroplast_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('minicircle_gene','minicircle_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_catalytic_motif','polypeptide_catalytic_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rnapol_iii_promoter_type_2','rnapol_iii_promoter_type_2');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('no_output','no_output');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('interior_coding_exon','interior_coding_exon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_taurinomethyl_two_thiouridine','five_taurinomethyl_two_thiouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k9_trimethylation_site','h3k9_trimethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcript_secondary_structure_variant','transcript_secondary_structure_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide','polypeptide');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('splice_donor_5th_base_variant','splice_donor_5th_base_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polymerase_synthesis_read','polymerase_synthesis_read');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('enhancer_binding_site','enhancer_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nucleotide_to_protein_binding_site','nucleotide_to_protein_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_turn_left_handed_type_two','beta_turn_left_handed_type_two');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('invalidated_by_genomic_polya_primed_cdna','invalidated_by_genomic_polya_primed_cdna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_edited_transcript','gene_with_edited_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dmv1_motif','dmv1_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('minus_12_signal','minus_12_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('post_translationally_modified_region','post_translationally_modified_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('proline','proline');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('flanking_region','flanking_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_methylthio_n6_isopentenyladenosine','two_methylthio_n6_isopentenyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypyrimidine_tract','polypyrimidine_tract');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_methoxyuridine','five_methoxyuridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_to_gene_feature','gene_to_gene_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bac_cloned_genomic_insert','bac_cloned_genomic_insert');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('j_heptamer','j_heptamer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_ust','three_prime_ust');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n6_2_prime_o_dimethyladenosine','n6_2_prime_o_dimethyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('trans_splice_site','trans_splice_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('de_novo_variant','de_novo_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered_rescue_region','engineered_rescue_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nucleomorph_gene','nucleomorph_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mrna_attribute','mrna_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_three_prime_overlap','five_prime_three_prime_overlap');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('group_i_intron','group_i_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('d_cluster','d_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('open_chromatin_region','open_chromatin_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('genomic_dna','genomic_dna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inside_intron','inside_intron');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('hammerhead_ribozyme','hammerhead_ribozyme');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_d_heptamer','five_prime_d_heptamer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intronic_splice_enhancer','intronic_splice_enhancer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_adenosine','modified_adenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_carboxymethylaminomethyluridine','five_carboxymethylaminomethyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('repeat_region','repeat_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('repeat_unit','repeat_unit');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('double_stranded_dna_chromosome','double_stranded_dna_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('heritable_phenotypic_marker','heritable_phenotypic_marker');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('template_region','template_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('primary_transcript_region','primary_transcript_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mitochondrial_dna','mitochondrial_dna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcript_region','transcript_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('histone_acetylation_site','histone_acetylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ribozyme','ribozyme');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('matrix_attachment_site','matrix_attachment_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('imprinted','imprinted');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_sequence_variant','polypeptide_sequence_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('est','est');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rna_motif','rna_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_variation_site','polypeptide_variation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('class_i_rna','class_i_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('oligo','oligo');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('stop_codon_signal','stop_codon_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('hypoploid','hypoploid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('exemplar_mrna','exemplar_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('experimentally_defined_binding_region','experimentally_defined_binding_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('c_to_g_transversion','c_to_g_transversion');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('active_peptide','active_peptide');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mannosyl_queuosine','mannosyl_queuosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_stop_codon_redefined_as_pyrrolysine','gene_with_stop_codon_redefined_as_pyrrolysine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('queuosine','queuosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('lna_oligo','lna_oligo');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('independently_known','independently_known');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('proviral_region','proviral_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('capped','capped');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('minus_1_frameshift_variant','minus_1_frameshift_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('direction_attribute','direction_attribute');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('micronuclear_chromosome','micronuclear_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pseudogene_by_unequal_crossing_over','pseudogene_by_unequal_crossing_over');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('one_two_prime_o_dimethyladenosine','one_two_prime_o_dimethyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dpe_motif','dpe_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('frame_restoring_variant','frame_restoring_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('seryl_trna','seryl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('structural_variant','structural_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('regulatory_promoter_element','regulatory_promoter_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('integration_excision_site','integration_excision_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('noncoding_region_of_exon','noncoding_region_of_exon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rnase_mrp_rna','rnase_mrp_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nuclear_export_signal','nuclear_export_signal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_methoxycarbonylmethyluridine','five_methoxycarbonylmethyluridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('exon_of_single_exon_gene','exon_of_single_exon_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_dicistronic_primary_transcript','gene_with_dicistronic_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sequence_secondary_structure','sequence_secondary_structure');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bacterial_rnapol_promoter_sigma_70','bacterial_rnapol_promoter_sigma_70');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tiling_path','tiling_path');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nuclear_sequence','nuclear_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('contig','contig');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('quality_value','quality_value');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('kozak_sequence','kozak_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('population_specific_variant','population_specific_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('catalytic_residue','catalytic_residue');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inversion_site','inversion_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('aspartic_acid','aspartic_acid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dif_site','dif_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mirna_gene','mirna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('valyl_trna','valyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inverted_tandem_duplication','inverted_tandem_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cis_regulatory_frameshift_element','cis_regulatory_frameshift_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('minisatellite','minisatellite');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('assembly_component','assembly_component');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('low_complexity_region','low_complexity_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('morpholino_backbone','morpholino_backbone');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('designed_sequence','designed_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('n6_n6_2_prime_o_trimethyladenosine','n6_n6_2_prime_o_trimethyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rna_polymerase_promoter','rna_polymerase_promoter');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_gene_recombination_feature','v_gene_recombination_feature');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_carboxymethylaminomethyl_two_thiouridine','five_carboxymethylaminomethyl_two_thiouridine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('free_ring_duplication','free_ring_duplication');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('point_centromere','point_centromere');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dicistronic_mrna','dicistronic_mrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('interchromosomal','interchromosomal');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('uncharacterised_chromosomal_mutation','uncharacterised_chromosomal_mutation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_cis_splice_site','five_prime_cis_splice_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('octamer_motif','octamer_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('encodes_overlapping_peptides_different_start','encodes_overlapping_peptides_different_start');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ss_rna_viral_sequence','ss_rna_viral_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('indel','indel');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dicistronic_primary_transcript','dicistronic_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('protein_binding_site','protein_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polycistronic','polycistronic');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('asparagine_trna_primary_transcript','asparagine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('splice_enhancer','splice_enhancer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('aneuploid_chromosome','aneuploid_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('peroxywybutosine','peroxywybutosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_aspartic_acid','modified_l_aspartic_acid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('engineered_episome','engineered_episome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rnai_reagent','rnai_reagent');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rasirna','rasirna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('tmrna_region','tmrna_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('increased_transcript_level_variant','increased_transcript_level_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bacterial_rnapol_promoter','bacterial_rnapol_promoter');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mitochondrial_sequence','mitochondrial_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('trinucleotide_repeat_microsatellite_feature','trinuc_repeat_microsat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('post_translationally_regulated_by_protein_stability','post_translationally_regulated_by_protein_stability');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nc_primary_transcript','nc_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('iron_responsive_element','iron_responsive_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('v_d_dj_j_cluster','v_d_dj_j_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('forward','forward');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('proviral_location','proviral_location');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('histone_binding_site','histone_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('promoter_element','promoter_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pse_motif','pse_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_turn_type_eight','beta_turn_type_eight');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('double','double');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cds_supported_by_est_or_cdna_data','cds_supported_by_est_or_cdna_data');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('double_stranded_rna_chromosome','double_stranded_rna_chromosome');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('modified_l_glutamic_acid','modified_l_glutamic_acid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polypeptide_nest_motif','polypeptide_nest_motif');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('translational_frameshift','translational_frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_terminal_inverted_repeat','three_prime_terminal_inverted_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pseudogenic_trna','pseudogenic_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cap','cap');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_coding_exon','five_prime_coding_exon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('enzymatic','enzymatic');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('interior_exon','interior_exon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('genetic_marker','genetic_marker');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('a_to_g_transition','a_to_g_transition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_methylthio_n6_hydroxynorvalyl_carbamoyladenosine','two_methylthio_n6_hydroxynorvalyl_carbamoyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('isre','isre');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('maternally_imprinted_gene','maternally_imprinted_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('circular','circular');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h_pseudoknot','h_pseudoknot');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intermediate_element','intermediate_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcript','transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('pseudogene','pseudogene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('direct_repeat','direct_repeat');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bacterial_terminator','bacterial_terminator');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('diplotype','diplotype');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('metal_binding_site','metal_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dj_gene','dj_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('two_prime_o_methyladenosine','two_prime_o_methyladenosine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('splice_region_variant','splice_region_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('aspe_primer','aspe_primer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('restriction_enzyme_binding_site','restriction_enzyme_binding_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('bac','bac');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('dj_j_cluster','dj_j_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k36_dimethylation_site','h3k36_dimethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('trans_splice_donor_site','trans_splice_donor_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_variant','gene_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('conformational_change_variant','conformational_change_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h3k36_monomethylation_site','h3k36_monomethylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nucleic_acid','nucleic_acid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('invalidated_by_chimeric_cdna','invalidated_by_chimeric_cdna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('histidine','histidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_component_region','gene_component_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('primer_match','primer_match');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('trna_primary_transcript','trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('loxp_site','loxp_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('serine','serine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('propeptide','propeptide');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('five_prime_open_reading_frame','five_prime_open_reading_frame');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('schellmann_loop_six','schellmann_loop_six');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('shine_dalgarno_sequence','shine_dalgarno_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sp6_rna_polymerase_promoter','sp6_rna_polymerase_promoter');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('centromere_dna_element_iii','centromere_dna_element_iii');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('lysine_trna_primary_transcript','lysine_trna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('j_c_cluster','j_c_cluster');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('assortment_derived_deficiency','assortment_derived_deficiency');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mirna_primary_transcript','mirna_primary_transcript');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_5s','rrna_5s');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('leucyl_trna','leucyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inverted_intrachromosomal_transposition','invert_intra_transposition');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('enzymatic_rna','enzymatic_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('germline_variant','germline_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('negatively_autoregulated','negatively_autoregulated');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('benign_variant','benign_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('anchor_region','anchor_region');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('exon','exon');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_turn_type_six_a','beta_turn_type_six_a');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('random_sequence','random_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('partially_characterised_chromosomal_mutation','partially_characterised_chromosomal_mutation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rna','rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('core_promoter_element','core_promoter_element');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('restriction_enzyme_single_strand_overhang','restriction_enzyme_single_strand_overhang');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ss_oligo','ss_oligo');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('histone_methylation_site','histone_methylation_site');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('inversion_derived_duplication_plus_aneuploid','inversion_derived_duplication_plus_aneuploid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('transcript_bound_by_nucleic_acid','transcript_bound_by_nucleic_acid');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('intein_containing','intein_containing');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('h_aca_box_snorna_encoding','h_aca_box_snorna_encoding');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('rrna_cleavage_rna','rrna_cleavage_rna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('enhancer_trap_construct','enhancer_trap_construct');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gamma_turn_inverse','gamma_turn_inverse');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_d_nonamer','three_prime_d_nonamer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('paternal_variant','paternal_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('level_of_transcript_variant','level_of_transcript_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('selenocysteinyl_trna','selenocysteinyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_silenced_by_histone_methylation','gene_silenced_by_histone_methylation');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('u11_snrna','u11_snrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('scrna','scrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('gene_with_stop_codon_redefined_as_selenocysteine','gene_with_stop_codon_redefined_as_selenocysteine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('silencer','silencer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('sage_tag','sage_tag');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('mrna_with_minus_1_frameshift','mrna_with_minus_1_frameshift');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('nuclear_gene','nuclear_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('cds_supported_by_domain_match_data','cds_supported_by_domain_match_data');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('internal_shine_dalgarno_sequence','internal_shine_dalgarno_sequence');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('prolyl_trna','prolyl_trna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('lysidine','lysidine');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('beta_bulge','beta_bulge');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('post_translationally_regulated_by_protein_modification','post_translationally_regulated_by_protein_modification');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('splice_donor_variant','splice_donor_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('ncrna','ncrna');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('grna_gene','grna_gene');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('enhancer','enhancer');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('polymorphic_sequence_variant','polymorphic_sequence_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('three_prime_clip','three_prime_clip');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('frameshift_variant','frameshift_variant');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('insertional','insertional');
INSERT INTO sequence_cv_lookup_table (original_cvterm_name,relation_name) VALUES ('non_processed_pseudogene','non_processed_pseudogene');

CREATE INDEX sequence_cv_lookup_table_idx ON sequence_cv_lookup_table (original_cvterm_name);


SET search_path=chado,pg_catalog;
-- DEPENDENCY:
--  chado/modules/bridges/sofa-bridge.sql

-- The standard Chado pattern for protein coding genes
-- is a feature of type 'gene' with 'mRNA' features as parts
-- REQUIRES: 'mrna' view from so-bridge.sql
CREATE OR REPLACE VIEW protein_coding_gene AS
 SELECT
  DISTINCT gene.*
 FROM
  feature AS gene
  INNER JOIN feature_relationship AS fr ON (gene.feature_id=fr.object_id)
  INNER JOIN so.mrna ON (mrna.feature_id=fr.subject_id);


-- introns are implicit from surrounding exons
-- combines intron features with location and parent transcript
-- the same intron appearing in multiple transcripts will appear
-- multiple times
CREATE VIEW intron_combined_view AS
 SELECT
  x1.feature_id         AS exon1_id,
  x2.feature_id         AS exon2_id,
  CASE WHEN l1.strand=-1  THEN l2.fmax ELSE l1.fmax END AS fmin,
  CASE WHEN l1.strand=-1  THEN l1.fmin ELSE l2.fmin END AS fmax,
  l1.strand             AS strand,
  l1.srcfeature_id      AS srcfeature_id,
  r1.rank               AS intron_rank,
  r1.object_id          AS transcript_id
 FROM
 cvterm
  INNER JOIN 
   feature                AS x1    ON (x1.type_id=cvterm.cvterm_id)
    INNER JOIN
     feature_relationship AS r1    ON (x1.feature_id=r1.subject_id)
    INNER JOIN
     featureloc           AS l1    ON (x1.feature_id=l1.feature_id)
  INNER JOIN
   feature                AS x2    ON (x2.type_id=cvterm.cvterm_id)
    INNER JOIN
     feature_relationship AS r2    ON (x2.feature_id=r2.subject_id)
    INNER JOIN
     featureloc           AS l2    ON (x2.feature_id=l2.feature_id)
 WHERE
  cvterm.name='exon'            AND
  (r2.rank - r1.rank) = 1       AND
  r1.object_id=r2.object_id     AND
  l1.strand = l2.strand         AND
  l1.srcfeature_id = l2.srcfeature_id         AND
  l1.locgroup=0                 AND
  l2.locgroup=0;

-- intron locations. intron IDs are the (exon1,exon2) ID pair
-- this means that introns may be counted twice if the start of
-- the 5' exon or the end of the 3' exon vary
-- introns shared by transcripts will not appear twice
CREATE VIEW intronloc_view AS
 SELECT DISTINCT
  exon1_id,
  exon2_id,
  fmin,
  fmax,
  strand,
  srcfeature_id
 FROM intron_combined_view;
CREATE OR REPLACE FUNCTION store_feature 
(INT,INT,INT,INT,
 INT,INT,VARCHAR,VARCHAR,INT,BOOLEAN)
 RETURNS INT AS 
'DECLARE
  v_srcfeature_id       ALIAS FOR $1;
  v_fmin                ALIAS FOR $2;
  v_fmax                ALIAS FOR $3;
  v_strand              ALIAS FOR $4;
  v_dbxref_id           ALIAS FOR $5;
  v_organism_id         ALIAS FOR $6;
  v_name                ALIAS FOR $7;
  v_uniquename          ALIAS FOR $8;
  v_type_id             ALIAS FOR $9;
  v_is_analysis         ALIAS FOR $10;
  v_feature_id          INT;
  v_featureloc_id       INT;
 BEGIN
    IF v_dbxref_id IS NULL THEN
      SELECT INTO v_feature_id feature_id
      FROM feature
      WHERE uniquename=v_uniquename     AND
            organism_id=v_organism_id   AND
            type_id=v_type_id;
    ELSE
      SELECT INTO v_feature_id feature_id
      FROM feature
      WHERE dbxref_id=v_dbxref_id;
    END IF;
    IF NOT FOUND THEN
      INSERT INTO feature
       ( dbxref_id           ,
         organism_id         ,
         name                ,
         uniquename          ,
         type_id             ,
         is_analysis         )
        VALUES
        ( v_dbxref_id           ,
          v_organism_id         ,
          v_name                ,
          v_uniquename          ,
          v_type_id             ,
          v_is_analysis         );
      v_feature_id = currval(''feature_feature_id_seq'');
    ELSE
      UPDATE feature SET
        dbxref_id   =  v_dbxref_id           ,
        organism_id =  v_organism_id         ,
        name        =  v_name                ,
        uniquename  =  v_uniquename          ,
        type_id     =  v_type_id             ,
        is_analysis =  v_is_analysis
      WHERE
        feature_id=v_feature_id;
    END IF;
  PERFORM store_featureloc(v_feature_id,
                           v_srcfeature_id,
                           v_fmin,
                           v_fmax,
                           v_strand,
                           0,
                           0);
  RETURN v_feature_id;
 END;
' LANGUAGE 'plpgsql';


CREATE OR REPLACE FUNCTION store_featureloc
(INT,INT,INT,INT,INT,INT,INT)
 RETURNS INT AS 
'DECLARE
  v_feature_id          ALIAS FOR $1;
  v_srcfeature_id       ALIAS FOR $2;
  v_fmin                ALIAS FOR $3;
  v_fmax                ALIAS FOR $4;
  v_strand              ALIAS FOR $5;
  v_rank                ALIAS FOR $6;
  v_locgroup            ALIAS FOR $7;
  v_featureloc_id       INT;
 BEGIN
    IF v_feature_id IS NULL THEN RAISE EXCEPTION ''feature_id cannot be null'';
    END IF;
    SELECT INTO v_featureloc_id featureloc_id
      FROM featureloc
      WHERE feature_id=v_feature_id     AND
            rank=v_rank                 AND
            locgroup=v_locgroup;
    IF NOT FOUND THEN
      INSERT INTO featureloc
        ( feature_id,
          srcfeature_id,
          fmin,
          fmax,
          strand,
          rank,
          locgroup)
        VALUES
        (  v_feature_id,
           v_srcfeature_id,
           v_fmin,
           v_fmax,
           v_strand,
           v_rank,
           v_locgroup);
      v_featureloc_id = currval(''featureloc_featureloc_id_seq'');
    ELSE
      UPDATE featureloc SET
        feature_id    =  v_feature_id,
        srcfeature_id =  v_srcfeature_id,
        fmin          =  v_fmin,
        fmax          =  v_fmax,
        strand        =  v_strand,
        rank          =  v_rank,
        locgroup      =  v_locgroup
      WHERE
        featureloc_id=v_featureloc_id;
    END IF;
  RETURN v_featureloc_id;
 END;
' LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION store_feature_synonym
(INT,VARCHAR,INT,BOOLEAN,BOOLEAN,INT)
 RETURNS INT AS 
'DECLARE
  v_feature_id          ALIAS FOR $1;
  v_syn                 ALIAS FOR $2;
  v_type_id             ALIAS FOR $3;
  v_is_current          ALIAS FOR $4;
  v_is_internal         ALIAS FOR $5;
  v_pub_id              ALIAS FOR $6;
  v_synonym_id          INT;
  v_feature_synonym_id  INT;
 BEGIN
    IF v_feature_id IS NULL THEN RAISE EXCEPTION ''feature_id cannot be null'';
    END IF;
    SELECT INTO v_synonym_id synonym_id
      FROM synonym
      WHERE name=v_syn                  AND
            type_id=v_type_id;
    IF NOT FOUND THEN
      INSERT INTO synonym
        ( name,
          synonym_sgml,
          type_id)
        VALUES
        ( v_syn,
          v_syn,
          v_type_id);
      v_synonym_id = currval(''synonym_synonym_id_seq'');
    END IF;
    SELECT INTO v_feature_synonym_id feature_synonym_id
        FROM feature_synonym
        WHERE feature_id=v_feature_id   AND
              synonym_id=v_synonym_id   AND
              pub_id=v_pub_id;
    IF NOT FOUND THEN
      INSERT INTO feature_synonym
        ( feature_id,
          synonym_id,
          pub_id,
          is_current,
          is_internal)
        VALUES
        ( v_feature_id,
          v_synonym_id,
          v_pub_id,
          v_is_current,
          v_is_internal);
      v_feature_synonym_id = currval(''feature_synonym_feature_synonym_id_seq'');
    ELSE
      UPDATE feature_synonym
        SET is_current=v_is_current, is_internal=v_is_internal
        WHERE feature_synonym_id=v_feature_synonym_id;
    END IF;
  RETURN v_feature_synonym_id;
 END;
' LANGUAGE 'plpgsql';



-- dependency_on: [sequtil,sequence-cv-helper]

CREATE OR REPLACE FUNCTION subsequence(bigint,bigint,bigint,INT)
 RETURNS TEXT AS
 'SELECT 
  CASE WHEN $4<0 
   THEN reverse_complement(substring(srcf.residues,CAST(($2+1) as int),CAST(($3-$2) as int)))
   ELSE substring(residues,CAST(($2+1) as int),CAST(($3-$2) as int))
  END AS residues
  FROM feature AS srcf
  WHERE
   srcf.feature_id=$1'
LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION subsequence_by_featureloc(bigint)
 RETURNS TEXT AS
 'SELECT 
  CASE WHEN strand<0 
   THEN reverse_complement(substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int)))
   ELSE substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int))
  END AS residues
  FROM feature AS srcf
   INNER JOIN featureloc ON (srcf.feature_id=featureloc.srcfeature_id)
  WHERE
   featureloc_id=$1'
LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION subsequence_by_feature(bigint,INT,INT)
 RETURNS TEXT AS
 'SELECT 
  CASE WHEN strand<0 
   THEN reverse_complement(substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int)))
   ELSE substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int))
  END AS residues
  FROM feature AS srcf
   INNER JOIN featureloc ON (srcf.feature_id=featureloc.srcfeature_id)
  WHERE
   featureloc.feature_id=$1 AND
   featureloc.rank=$2 AND
   featureloc.locgroup=$3'
LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION subsequence_by_feature(bigint)
 RETURNS TEXT AS 'SELECT subsequence_by_feature($1,0,0)'
LANGUAGE 'sql';

-- based on subfeature sets:

-- constrained by feature_relationship.type_id
--   (allows user to construct queries that only get subsequences of
--    part_of subfeatures)

CREATE OR REPLACE FUNCTION subsequence_by_subfeatures(bigint,bigint,INT,INT)
 RETURNS TEXT AS '
DECLARE v_feature_id ALIAS FOR $1;
DECLARE v_rtype_id   ALIAS FOR $2;
DECLARE v_rank       ALIAS FOR $3;
DECLARE v_locgroup   ALIAS FOR $4;
DECLARE subseq       TEXT;
DECLARE seqrow       RECORD;
BEGIN 
  subseq = '''';
 FOR seqrow IN
   SELECT
    CASE WHEN strand<0 
     THEN reverse_complement(substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int)))
     ELSE substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int))
    END AS residues
    FROM feature AS srcf
     INNER JOIN featureloc ON (srcf.feature_id=featureloc.srcfeature_id)
     INNER JOIN feature_relationship AS fr
       ON (fr.subject_id=featureloc.feature_id)
    WHERE
     fr.object_id=v_feature_id AND
     fr.type_id=v_rtype_id AND
     featureloc.rank=v_rank AND
     featureloc.locgroup=v_locgroup
    ORDER BY fr.rank
  LOOP
   subseq = subseq  || seqrow.residues;
  END LOOP;
 RETURN subseq;
END
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION subsequence_by_subfeatures(bigint,bigint)
 RETURNS TEXT AS
 'SELECT subsequence_by_subfeatures($1,$2,0,0)'
LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION subsequence_by_subfeatures(bigint)
 RETURNS TEXT AS
'
SELECT subsequence_by_subfeatures($1,get_feature_relationship_type_id(''part_of''),0,0)
'
LANGUAGE 'sql';


-- constrained by subfeature.type_id (eg exons of a transcript)
CREATE OR REPLACE FUNCTION subsequence_by_typed_subfeatures(bigint,bigint,INT,INT)
 RETURNS TEXT AS '
DECLARE v_feature_id ALIAS FOR $1;
DECLARE v_ftype_id   ALIAS FOR $2;
DECLARE v_rank       ALIAS FOR $3;
DECLARE v_locgroup   ALIAS FOR $4;
DECLARE subseq       TEXT;
DECLARE seqrow       RECORD;
BEGIN 
  subseq = '''';
 FOR seqrow IN
   SELECT
    CASE WHEN strand<0 
     THEN reverse_complement(substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int)))
     ELSE substring(srcf.residues,CAST(fmin+1 as int),CAST((fmax-fmin) as int))
    END AS residues
  FROM feature AS srcf
   INNER JOIN featureloc ON (srcf.feature_id=featureloc.srcfeature_id)
   INNER JOIN feature AS subf ON (subf.feature_id=featureloc.feature_id)
   INNER JOIN feature_relationship AS fr ON (fr.subject_id=subf.feature_id)
  WHERE
     fr.object_id=v_feature_id AND
     subf.type_id=v_ftype_id AND
     featureloc.rank=v_rank AND
     featureloc.locgroup=v_locgroup
  ORDER BY fr.rank
   LOOP
   subseq = subseq  || seqrow.residues;
  END LOOP;
 RETURN subseq;
END
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION subsequence_by_typed_subfeatures(bigint,bigint)
 RETURNS TEXT AS
 'SELECT subsequence_by_typed_subfeatures($1,$2,0,0)'
LANGUAGE 'sql';

 


CREATE OR REPLACE FUNCTION feature_subalignments(bigint) RETURNS SETOF featureloc AS '
DECLARE
  return_data featureloc%ROWTYPE;
  f_id ALIAS FOR $1;
  feature_data feature%rowtype;
  featureloc_data featureloc%rowtype;

  s text;

  fmin bigint;
  slen bigint;
BEGIN
  --RAISE NOTICE ''feature_id is %'', featureloc_data.feature_id;
  SELECT INTO feature_data * FROM feature WHERE feature_id = f_id;

  FOR featureloc_data IN SELECT * FROM featureloc WHERE feature_id = f_id LOOP

    --RAISE NOTICE ''fmin is %'', featureloc_data.fmin;

    return_data.feature_id      = f_id;
    return_data.srcfeature_id   = featureloc_data.srcfeature_id;
    return_data.is_fmin_partial = featureloc_data.is_fmin_partial;
    return_data.is_fmax_partial = featureloc_data.is_fmax_partial;
    return_data.strand          = featureloc_data.strand;
    return_data.phase           = featureloc_data.phase;
    return_data.residue_info    = featureloc_data.residue_info;
    return_data.locgroup        = featureloc_data.locgroup;
    return_data.rank            = featureloc_data.rank;

    s = feature_data.residues;
    fmin = featureloc_data.fmin;
    slen = char_length(s);

    WHILE char_length(s) LOOP
      --RAISE NOTICE ''residues is %'', s;

      --trim off leading match
      s = trim(leading ''|ATCGNatcgn'' from s);
      --if leading match detected
      IF slen > char_length(s) THEN
        return_data.fmin = fmin;
        return_data.fmax = featureloc_data.fmin + (slen - char_length(s));

        --if the string started with a match, return it,
        --otherwise, trim the gaps first (ie do not return this iteration)
        RETURN NEXT return_data;
      END IF;

      --trim off leading gap
      s = trim(leading ''-'' from s);

      fmin = featureloc_data.fmin + (slen - char_length(s));
    END LOOP;
  END LOOP;

  RETURN;

END;
' LANGUAGE 'plpgsql';
CREATE SCHEMA frange;
SET search_path = frange,chado,pg_catalog;

CREATE TABLE featuregroup (
    featuregroup_id bigserial not null,
    primary key (featuregroup_id),

    subject_id bigint not null,
    foreign key (subject_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,

    object_id bigint not null,
    foreign key (object_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,

    group_id bigint not null,
    foreign key (group_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,

    srcfeature_id bigint null,
    foreign key (srcfeature_id) references feature (feature_id) on delete cascade INITIALLY DEFERRED,

    fmin bigint null,
    fmax bigint null,
    strand int null,
    is_root int not null default 0,

    constraint featuregroup_c1 unique (subject_id,object_id,group_id,srcfeature_id,fmin,fmax,strand)
);
CREATE INDEX featuregroup_idx1 ON featuregroup (subject_id);
CREATE INDEX featuregroup_idx2 ON featuregroup (object_id);
CREATE INDEX featuregroup_idx3 ON featuregroup (group_id);
CREATE INDEX featuregroup_idx4 ON featuregroup (srcfeature_id);
CREATE INDEX featuregroup_idx5 ON featuregroup (strand);
CREATE INDEX featuregroup_idx6 ON featuregroup (is_root);

CREATE OR REPLACE FUNCTION groupoverlaps(bigint, bigint, varchar) RETURNS setof featuregroup AS '
  SELECT g2.*
  FROM  featuregroup g1,
        featuregroup g2
  WHERE g1.is_root = 1
    AND ( g1.srcfeature_id = g2.srcfeature_id OR g2.srcfeature_id IS NULL )
    AND g1.group_id = g2.group_id
    AND g1.srcfeature_id = (SELECT feature_id FROM feature WHERE uniquename = $3)
    AND boxquery($1, $2) <@ boxrange(g1.fmin,g2.fmax)
' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION groupcontains(bigint, bigint, varchar) RETURNS setof featuregroup AS '
  SELECT *
  FROM groupoverlaps($1,$2,$3)
  WHERE fmin <= $1 AND fmax >= $2
' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION groupinside(bigint, bigint, varchar) RETURNS setof featuregroup AS '
  SELECT *
  FROM groupoverlaps($1,$2,$3)
  WHERE fmin >= $1 AND fmax <= $2
' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION groupidentical(bigint, bigint, varchar) RETURNS setof featuregroup AS '
  SELECT *
  FROM groupoverlaps($1,$2,$3)
  WHERE fmin = $1 AND fmax = $2
' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION groupoverlaps(bigint, bigint) RETURNS setof featuregroup AS '
  SELECT *
  FROM featuregroup
  WHERE is_root = 1
    AND boxquery($1, $2) <@ boxrange(fmin,fmax)
' LANGUAGE 'sql';

CREATE OR REPLACE FUNCTION groupoverlaps(_int8, _int8, _varchar) RETURNS setof featuregroup AS '
DECLARE
    mins alias for $1;
    maxs alias for $2;
    srcs alias for $3;
    f featuregroup%ROWTYPE;
    i int;
    s int;
BEGIN
    i := 1;
    FOR i in array_lower( mins, 1 ) .. array_upper( mins, 1 ) LOOP
        SELECT INTO s feature_id FROM feature WHERE uniquename = srcs[i];
        FOR f IN
            SELECT *
            FROM  featuregroup WHERE group_id IN (
                SELECT group_id FROM featuregroup
                WHERE (srcfeature_id = s OR srcfeature_id IS NULL)
                  AND group_id IN (
                      SELECT group_id FROM groupoverlaps( mins[i], maxs[i] )
                      WHERE  srcfeature_id = s
                  )
            )
        LOOP
            RETURN NEXT f;
        END LOOP;
    END LOOP;
    RETURN;
END;
' LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION groupcontains(_int8, _int8, _varchar) RETURNS setof featuregroup AS '
DECLARE
    mins alias for $1;
    maxs alias for $2;
    srcs alias for $3;
    f featuregroup%ROWTYPE;
    i int;
    s int;
BEGIN
    i := 1;
    FOR i in array_lower( mins, 1 ) .. array_upper( mins, 1 ) LOOP
        SELECT INTO s feature_id FROM feature WHERE uniquename = srcs[i];
        FOR f IN
            SELECT *
            FROM  featuregroup WHERE group_id IN (
                SELECT group_id FROM featuregroup
                WHERE (srcfeature_id = s OR srcfeature_id IS NULL)
                  AND fmin <= mins[i]
                  AND fmax >= maxs[i]
                  AND group_id IN (
                      SELECT group_id FROM groupoverlaps( mins[i], maxs[i] )
                      WHERE  srcfeature_id = s
                  )
            )
        LOOP
            RETURN NEXT f;
        END LOOP;
    END LOOP;
    RETURN;
END;
' LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION groupinside(_int8, _int8, _varchar) RETURNS setof featuregroup AS '
DECLARE
    mins alias for $1;
    maxs alias for $2;
    srcs alias for $3;
    f featuregroup%ROWTYPE;
    i int;
    s int;
BEGIN
    i := 1;
    FOR i in array_lower( mins, 1 ) .. array_upper( mins, 1 ) LOOP
        SELECT INTO s feature_id FROM feature WHERE uniquename = srcs[i];
        FOR f IN
            SELECT *
            FROM  featuregroup WHERE group_id IN (
                SELECT group_id FROM featuregroup
                WHERE (srcfeature_id = s OR srcfeature_id IS NULL)
                  AND fmin >= mins[i]
                  AND fmax <= maxs[i]
                  AND group_id IN (
                      SELECT group_id FROM groupoverlaps( mins[i], maxs[i] )
                      WHERE  srcfeature_id = s
                  )
            )
        LOOP
            RETURN NEXT f;
        END LOOP;
    END LOOP;
    RETURN;
END;
' LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION groupidentical(_int8, _int8, _varchar) RETURNS setof featuregroup AS '
DECLARE
    mins alias for $1;
    maxs alias for $2;
    srcs alias for $3;
    f featuregroup%ROWTYPE;
    i int;
    s int;
BEGIN
    i := 1;
    FOR i in array_lower( mins, 1 ) .. array_upper( mins, 1 ) LOOP
        SELECT INTO s feature_id FROM feature WHERE uniquename = srcs[i];
        FOR f IN
            SELECT *
            FROM  featuregroup WHERE group_id IN (
                SELECT group_id FROM featuregroup
                WHERE (srcfeature_id = s OR srcfeature_id IS NULL)
                  AND fmin = mins[i]
                  AND fmax = maxs[i]
                  AND group_id IN (
                      SELECT group_id FROM groupoverlaps( mins[i], maxs[i] )
                      WHERE  srcfeature_id = s
                  )
            )
        LOOP
            RETURN NEXT f;
        END LOOP;
    END LOOP;
    RETURN;
END;
' LANGUAGE 'plpgsql';

--functional index that depends on the above functions
CREATE INDEX bingroup_boxrange ON featuregroup USING gist (boxrange(fmin, fmax)) WHERE is_root = 1;

CREATE OR REPLACE FUNCTION _fill_featuregroup(bigint, bigint) RETURNS INTEGER AS '
DECLARE
    groupid alias for $1;
    parentid alias for $2;
    g featuregroup%ROWTYPE;
BEGIN
    FOR g IN
        SELECT DISTINCT 0, fr.subject_id, fr.object_id, groupid, fl.srcfeature_id, fl.fmin, fl.fmax, fl.strand, 0
        FROM  feature_relationship AS fr,
              featureloc AS fl
        WHERE fr.object_id = parentid
          AND fr.subject_id = fl.feature_id
    LOOP
        INSERT INTO featuregroup
            (subject_id, object_id, group_id, srcfeature_id, fmin, fmax, strand, is_root)
        VALUES
            (g.subject_id, g.object_id, g.group_id, g.srcfeature_id, g.fmin, g.fmax, g.strand, 0);
        PERFORM _fill_featuregroup(groupid,g.subject_id);
    END LOOP;
    RETURN 1;
END;
' LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION fill_featuregroup() RETURNS INTEGER AS '
DECLARE
    p featuregroup%ROWTYPE;
    l featureloc%ROWTYPE;
    isa bigint;
    -- c int;  the c variable isnt used
BEGIN
    TRUNCATE featuregroup;
    SELECT INTO isa cvterm_id FROM cvterm WHERE (name = ''isa'' OR name = ''is_a'');

    -- Recursion is the biggest performance killer for this function.
    -- We can dodge the first round of recursion using the "fr1 / GROUP BY" approach.
    -- Luckily, most feature graphs are only 2 levels deep, so most recursion is
    -- avoidable.

    RAISE NOTICE ''Loading root and singleton features.'';
    FOR p IN
        SELECT DISTINCT 0, f.feature_id, f.feature_id, f.feature_id, srcfeature_id, fmin, fmax, strand, 1
        FROM feature AS f
        LEFT JOIN feature_relationship ON (f.feature_id = object_id)
        LEFT JOIN featureloc           ON (f.feature_id = featureloc.feature_id)
        WHERE f.feature_id NOT IN ( SELECT subject_id FROM feature_relationship )
          AND srcfeature_id IS NOT NULL
    LOOP
        INSERT INTO featuregroup
            (subject_id, object_id, group_id, srcfeature_id, fmin, fmax, strand, is_root)
        VALUES
            (p.object_id, p.object_id, p.object_id, p.srcfeature_id, p.fmin, p.fmax, p.strand, 1);
    END LOOP;

    RAISE NOTICE ''Loading child features.  If your database contains grandchild'';
    RAISE NOTICE ''features, they will be loaded recursively and may take a long time.'';

    FOR p IN
        SELECT DISTINCT 0, fr0.subject_id, fr0.object_id, fr0.object_id, fl.srcfeature_id, fl.fmin, fl.fmax, fl.strand, count(fr1.subject_id)
        FROM  feature_relationship AS fr0
        LEFT JOIN feature_relationship AS fr1 ON ( fr0.subject_id = fr1.object_id),
        featureloc AS fl
        WHERE fr0.subject_id = fl.feature_id
          AND fr0.object_id IN (
                  SELECT f.feature_id
                  FROM feature AS f
                  LEFT JOIN feature_relationship ON (f.feature_id = object_id)
                  LEFT JOIN featureloc           ON (f.feature_id = featureloc.feature_id)
                  WHERE f.feature_id NOT IN ( SELECT subject_id FROM feature_relationship )
                    AND f.feature_id     IN ( SELECT object_id  FROM feature_relationship )
                    AND srcfeature_id IS NOT NULL
              )
        GROUP BY fr0.subject_id, fr0.object_id, fl.srcfeature_id, fl.fmin, fl.fmax, fl.strand
    LOOP
        INSERT INTO featuregroup
            (subject_id, object_id, group_id, srcfeature_id, fmin, fmax, strand, is_root)
        VALUES
            (p.subject_id, p.object_id, p.object_id, p.srcfeature_id, p.fmin, p.fmax, p.strand, 0);
        IF ( p.is_root > 0 ) THEN
            PERFORM _fill_featuregroup(p.subject_id,p.subject_id);
        END IF;
    END LOOP;

    RETURN 1;
END;   
' LANGUAGE 'plpgsql';

SET search_path = chado,pg_catalog;
--- create ontology that has instantiated located_sequence_feature part of SO
--- way as it is written, the function can not be execute more than once in one connection
--- when you get error like ERROR:  relation with OID NNNNN does not exist
--- as this is not meant to execute >1 times in one session so it should never happen
--- except at testing and test failed
--- disconnect and try again, in other words, it can NOT be executed >1 time in one connection
--- if using EXECUTE, we can avoid this problem but code is hard to write and read (lots of ', escape char)

--NOTE: private, don't call directly as relying on having temp table tmpcvtr

--DROP TYPE soi_type CASCADE;
CREATE TYPE soi_type AS (
    type_id bigint,
    subject_id bigint,
    object_id bigint
);

CREATE OR REPLACE FUNCTION _fill_cvtermpath4soinode(BIGINT, BIGINT, BIGINT, BIGINT, INTEGER) RETURNS INTEGER AS
'
DECLARE
    origin alias for $1;
    child_id alias for $2;
    cvid alias for $3;
    typeid alias for $4;
    depth alias for $5;
    cterm soi_type%ROWTYPE;
    exist_c int;

BEGIN

    --RAISE NOTICE ''depth=% o=%, root=%, cv=%, t=%'', depth,origin,child_id,cvid,typeid;
    SELECT INTO exist_c count(*) FROM cvtermpath WHERE cv_id = cvid AND object_id = origin AND subject_id = child_id AND pathdistance = depth;
    --- longest path
    IF (exist_c > 0) THEN
        UPDATE cvtermpath SET pathdistance = depth WHERE cv_id = cvid AND object_id = origin AND subject_id = child_id;
    ELSE
        INSERT INTO cvtermpath (object_id, subject_id, cv_id, type_id, pathdistance) VALUES(origin, child_id, cvid, typeid, depth);
    END IF;

    FOR cterm IN SELECT tmp_type AS type_id, subject_id FROM tmpcvtr WHERE object_id = child_id LOOP
        PERFORM _fill_cvtermpath4soinode(origin, cterm.subject_id, cvid, cterm.type_id, depth+1);
    END LOOP;
    RETURN 1;
END;
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION _fill_cvtermpath4soi(BIGINT, BIGINT) RETURNS INTEGER AS
'
DECLARE
    rootid alias for $1;
    cvid alias for $2;
    ttype bigint;
    cterm soi_type%ROWTYPE;

BEGIN
    
    SELECT INTO ttype cvterm_id FROM cvterm WHERE name = ''isa'';
    --RAISE NOTICE ''got ttype %'',ttype;
    PERFORM _fill_cvtermpath4soinode(rootid, rootid, cvid, ttype, 0);
    FOR cterm IN SELECT tmp_type AS type_id, subject_id FROM tmpcvtr WHERE object_id = rootid LOOP
        PERFORM _fill_cvtermpath4soi(cterm.subject_id, cvid);
    END LOOP;
    RETURN 1;
END;   
'
LANGUAGE 'plpgsql';

--- use tmpcvtr to temp store soi (virtural ontology)
--- using tmp tables is faster than using recursive function to create feature type relationship
--- since it gets feature type rel set by set instead of one by one
--- and getting feature type rel is very expensive
--- call _fillcvtermpath4soi to create path for the virtual ontology

CREATE OR REPLACE FUNCTION create_soi() RETURNS INTEGER AS
'
DECLARE
    parent soi_type%ROWTYPE;
    isa_id cvterm.cvterm_id%TYPE;
    soi_term TEXT := ''soi'';
    soi_def TEXT := ''ontology of SO feature instantiated in database'';
    soi_cvid bigint;
    soiterm_id bigint;
    pcount INTEGER;
    count INTEGER := 0;
    cquery TEXT;
BEGIN

    SELECT INTO isa_id cvterm_id FROM cvterm WHERE name = ''isa'';

    SELECT INTO soi_cvid cv_id FROM cv WHERE name = soi_term;
    IF (soi_cvid > 0) THEN
        DELETE FROM cvtermpath WHERE cv_id = soi_cvid;
        DELETE FROM cvterm WHERE cv_id = soi_cvid;
    ELSE
        INSERT INTO cv (name, definition) VALUES(soi_term, soi_def);
    END IF;
    SELECT INTO soi_cvid cv_id FROM cv WHERE name = soi_term;
    INSERT INTO cvterm (name, cv_id) VALUES(soi_term, soi_cvid);
    SELECT INTO soiterm_id cvterm_id FROM cvterm WHERE name = soi_term;

    CREATE TEMP TABLE tmpcvtr (tmp_type BIGINT, type_id bigint, subject_id bigint, object_id bigint);
    CREATE UNIQUE INDEX u_tmpcvtr ON tmpcvtr(subject_id, object_id);

    INSERT INTO tmpcvtr (tmp_type, type_id, subject_id, object_id)
        SELECT DISTINCT isa_id, soiterm_id, f.type_id, soiterm_id FROM feature f, cvterm t
        WHERE f.type_id = t.cvterm_id AND f.type_id > 0;
    EXECUTE ''select * from tmpcvtr where type_id = '' || soiterm_id || '';'';
    get diagnostics pcount = row_count;
    raise notice ''all types in feature %'',pcount;
--- do it hard way, delete any child feature type from above (NOT IN clause did not work)
    FOR parent IN SELECT DISTINCT 0, t.cvterm_id, 0 FROM feature c, feature_relationship fr, cvterm t
            WHERE t.cvterm_id = c.type_id AND c.feature_id = fr.subject_id LOOP
        DELETE FROM tmpcvtr WHERE type_id = soiterm_id and object_id = soiterm_id
            AND subject_id = parent.subject_id;
    END LOOP;
    EXECUTE ''select * from tmpcvtr where type_id = '' || soiterm_id || '';'';
    get diagnostics pcount = row_count;
    raise notice ''all types in feature after delete child %'',pcount;

    --- create feature type relationship (store in tmpcvtr)
    CREATE TEMP TABLE tmproot (cv_id bigint not null, cvterm_id bigint not null, status INTEGER DEFAULT 0);
    cquery := ''SELECT * FROM tmproot tmp WHERE tmp.status = 0;'';
    ---temp use tmpcvtr to hold instantiated SO relationship for speed
    ---use soterm_id as type_id, will delete from tmpcvtr
    ---us tmproot for this as well
    INSERT INTO tmproot (cv_id, cvterm_id, status) SELECT DISTINCT soi_cvid, c.subject_id, 0 FROM tmpcvtr c
        WHERE c.object_id = soiterm_id;
    EXECUTE cquery;
    GET DIAGNOSTICS pcount = ROW_COUNT;
    WHILE (pcount > 0) LOOP
        RAISE NOTICE ''num child temp (to be inserted) in tmpcvtr: %'',pcount;
        INSERT INTO tmpcvtr (tmp_type, type_id, subject_id, object_id)
            SELECT DISTINCT fr.type_id, soiterm_id, c.type_id, p.cvterm_id FROM feature c, feature_relationship fr,
            tmproot p, feature pf, cvterm t WHERE c.feature_id = fr.subject_id AND fr.object_id = pf.feature_id
            AND p.cvterm_id = pf.type_id AND t.cvterm_id = c.type_id AND p.status = 0;
        UPDATE tmproot SET status = 1 WHERE status = 0;
        INSERT INTO tmproot (cv_id, cvterm_id, status)
            SELECT DISTINCT soi_cvid, c.type_id, 0 FROM feature c, feature_relationship fr,
            tmproot tmp, feature p, cvterm t WHERE c.feature_id = fr.subject_id AND fr.object_id = p.feature_id
            AND tmp.cvterm_id = p.type_id AND t.cvterm_id = c.type_id AND tmp.status = 1;
        UPDATE tmproot SET status = 2 WHERE status = 1;
        EXECUTE cquery;
        GET DIAGNOSTICS pcount = ROW_COUNT; 
    END LOOP;
    DELETE FROM tmproot;

    ---get transitive closure for soi
    PERFORM _fill_cvtermpath4soi(soiterm_id, soi_cvid);

    DROP TABLE tmpcvtr;
    DROP TABLE tmproot;

    RETURN 1;
END;
'
LANGUAGE 'plpgsql';

---bad precedence: change customed type name
---drop here to remove old function
--DROP TYPE feature_by_cvt_type CASCADE;
--DROP TYPE fxgsfids_type CASCADE;

--DROP TYPE feature_by_fx_type CASCADE;
CREATE TYPE feature_by_fx_type AS (
    feature_id bigint,
    depth INT
);

CREATE OR REPLACE FUNCTION get_sub_feature_ids(text) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    sql alias for $1;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN
    FOR myrc IN EXECUTE sql LOOP
        FOR myrc2 IN SELECT * FROM get_sub_feature_ids(myrc.feature_id) LOOP
            RETURN NEXT myrc2;
        END LOOP;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_up_feature_ids(text) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    sql alias for $1;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN
    FOR myrc IN EXECUTE sql LOOP
        FOR myrc2 IN SELECT * FROM get_up_feature_ids(myrc.feature_id) LOOP
            RETURN NEXT myrc2;
        END LOOP;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_feature_ids(text) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    sql alias for $1;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;
    myrc3 feature_by_fx_type%ROWTYPE;

BEGIN

    FOR myrc IN EXECUTE sql LOOP
        RETURN NEXT myrc;
        FOR myrc2 IN SELECT * FROM get_up_feature_ids(myrc.feature_id) LOOP
            RETURN NEXT myrc2;
        END LOOP;
        FOR myrc3 IN SELECT * FROM get_sub_feature_ids(myrc.feature_id) LOOP
            RETURN NEXT myrc3;
        END LOOP;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';


CREATE OR REPLACE FUNCTION get_sub_feature_ids(bigint) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    root alias for $1;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN
    FOR myrc IN SELECT DISTINCT subject_id AS feature_id FROM feature_relationship WHERE object_id = root LOOP
        RETURN NEXT myrc;
        FOR myrc2 IN SELECT * FROM get_sub_feature_ids(myrc.feature_id) LOOP
            RETURN NEXT myrc2;
        END LOOP;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_up_feature_ids(bigint) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    leaf alias for $1;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;
BEGIN
    FOR myrc IN SELECT DISTINCT object_id AS feature_id FROM feature_relationship WHERE subject_id = leaf LOOP
        RETURN NEXT myrc;
        FOR myrc2 IN SELECT * FROM get_up_feature_ids(myrc.feature_id) LOOP
            RETURN NEXT myrc2;
        END LOOP;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_sub_feature_ids(bigint, integer) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    root alias for $1;
    depth alias for $2;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN
    FOR myrc IN SELECT DISTINCT subject_id AS feature_id, depth FROM feature_relationship WHERE object_id = root LOOP
        RETURN NEXT myrc;
        FOR myrc2 IN SELECT * FROM get_sub_feature_ids(myrc.feature_id,depth+1) LOOP
            RETURN NEXT myrc2;
        END LOOP;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

--- depth is reversed and meanless when union with results from get_sub_feature_ids
CREATE OR REPLACE FUNCTION get_up_feature_ids(bigint, integer) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    leaf alias for $1;
    depth alias for $2;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;
BEGIN
    FOR myrc IN SELECT DISTINCT object_id AS feature_id, depth FROM feature_relationship WHERE subject_id = leaf LOOP
        RETURN NEXT myrc;
        FOR myrc2 IN SELECT * FROM get_up_feature_ids(myrc.feature_id,depth+1) LOOP
            RETURN NEXT myrc2;
        END LOOP;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

--- children feature ids only (not include itself--parent) for SO type and range (src)
CREATE OR REPLACE FUNCTION get_sub_feature_ids_by_type_src(cvterm.name%TYPE,feature.uniquename%TYPE,char(1)) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    gtype alias for $1;
    src alias for $2;
    is_an alias for $3;
    query text;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT f.feature_id FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id)
        INNER join featureloc fl
        ON (f.feature_id = fl.feature_id) INNER join feature src ON (src.feature_id = fl.srcfeature_id)
        WHERE t.name = '' || quote_literal(gtype) || '' AND src.uniquename = '' || quote_literal(src)
        || '' AND f.is_analysis = '' || quote_literal(is_an) || '';'';
 
    IF (STRPOS(gtype, ''%'') > 0) THEN
        query := ''SELECT DISTINCT f.feature_id FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id)
             INNER join featureloc fl
            ON (f.feature_id = fl.feature_id) INNER join feature src ON (src.feature_id = fl.srcfeature_id)
            WHERE t.name like '' || quote_literal(gtype) || '' AND src.uniquename = '' || quote_literal(src)
            || '' AND f.is_analysis = '' || quote_literal(is_an) || '';'';
    END IF;
    FOR myrc IN SELECT * FROM get_sub_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

--- by SO type, usefull for tRNA, ncRNA, etc
CREATE OR REPLACE FUNCTION get_feature_ids_by_type(cvterm.name%TYPE, char(1)) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    gtype alias for $1;
    is_an alias for $2;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT f.feature_id 
        FROM feature f, cvterm t WHERE t.cvterm_id = f.type_id AND t.name = '' || quote_literal(gtype) ||
        '' AND f.is_analysis = '' || quote_literal(is_an) || '';'';
    IF (STRPOS(gtype, ''%'') > 0) THEN
        query := ''SELECT DISTINCT f.feature_id 
            FROM feature f, cvterm t WHERE t.cvterm_id = f.type_id AND t.name like ''
            || quote_literal(gtype) || '' AND f.is_analysis = '' || quote_literal(is_an) || '';'';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_feature_ids_by_type_src(cvterm.name%TYPE, feature.uniquename%TYPE, char(1)) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    gtype alias for $1;
    src alias for $2;
    is_an alias for $3;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT f.feature_id 
        FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id) INNER join featureloc fl
        ON (f.feature_id = fl.feature_id) INNER join feature src ON (src.feature_id = fl.srcfeature_id)
        WHERE t.name = '' || quote_literal(gtype) || '' AND src.uniquename = '' || quote_literal(src)
        || '' AND f.is_analysis = '' || quote_literal(is_an) || '';'';
 
    IF (STRPOS(gtype, ''%'') > 0) THEN
        query := ''SELECT DISTINCT f.feature_id 
            FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id) INNER join featureloc fl
            ON (f.feature_id = fl.feature_id) INNER join feature src ON (src.feature_id = fl.srcfeature_id)
            WHERE t.name like '' || quote_literal(gtype) || '' AND src.uniquename = '' || quote_literal(src)
            || '' AND f.is_analysis = '' || quote_literal(is_an) || '';'';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_feature_ids_by_type_name(cvterm.name%TYPE, feature.uniquename%TYPE, char(1)) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    gtype alias for $1;
    name alias for $2;
    is_an alias for $3;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT f.feature_id 
        FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id)
        WHERE t.name = '' || quote_literal(gtype) || '' AND (f.uniquename = '' || quote_literal(name)
        || '' OR f.name = '' || quote_literal(name) || '') AND f.is_analysis = '' || quote_literal(is_an) || '';'';
 
    IF (STRPOS(name, ''%'') > 0) THEN
        query := ''SELECT DISTINCT f.feature_id 
            FROM feature f INNER join cvterm t ON (f.type_id = t.cvterm_id)
            WHERE t.name = '' || quote_literal(gtype) || '' AND (f.uniquename like '' || quote_literal(name)
            || '' OR f.name like '' || quote_literal(name) || '') AND f.is_analysis = '' || quote_literal(is_an) || '';'';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

--- get all feature ids (including children) for feature that has an ontology term (say GO function)
CREATE OR REPLACE FUNCTION get_feature_ids_by_ont(cv.name%TYPE,cvterm.name%TYPE) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    aspect alias for $1;
    term alias for $2;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT fcvt.feature_id 
        FROM feature_cvterm fcvt, cv, cvterm t WHERE cv.cv_id = t.cv_id AND
        t.cvterm_id = fcvt.cvterm_id AND cv.name = '' || quote_literal(aspect) ||
        '' AND t.name = '' || quote_literal(term) || '';'';
    IF (STRPOS(term, ''%'') > 0) THEN
        query := ''SELECT DISTINCT fcvt.feature_id 
            FROM feature_cvterm fcvt, cv, cvterm t WHERE cv.cv_id = t.cv_id AND
            t.cvterm_id = fcvt.cvterm_id AND cv.name = '' || quote_literal(aspect) ||
            '' AND t.name like '' || quote_literal(term) || '';'';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION get_feature_ids_by_ont_root(cv.name%TYPE,cvterm.name%TYPE) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    aspect alias for $1;
    term alias for $2;
    query TEXT;
    subquery TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    subquery := ''SELECT t.cvterm_id FROM cv, cvterm t WHERE cv.cv_id = t.cv_id 
        AND cv.name = '' || quote_literal(aspect) || '' AND t.name = '' || quote_literal(term) || '';'';
    IF (STRPOS(term, ''%'') > 0) THEN
        subquery := ''SELECT t.cvterm_id FROM cv, cvterm t WHERE cv.cv_id = t.cv_id 
            AND cv.name = '' || quote_literal(aspect) || '' AND t.name like '' || quote_literal(term) || '';'';
    END IF;
    query := ''SELECT DISTINCT fcvt.feature_id 
        FROM feature_cvterm fcvt INNER JOIN (SELECT cvterm_id FROM get_it_sub_cvterm_ids('' || quote_literal(subquery) || '')) AS ont ON (fcvt.cvterm_id = ont.cvterm_id);'';

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

--- get all feature ids (including children) for feature with the property (type, val)
CREATE OR REPLACE FUNCTION get_feature_ids_by_property(cvterm.name%TYPE,varchar) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    p_type alias for $1;
    p_val alias for $2;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT fprop.feature_id 
        FROM featureprop fprop, cvterm t WHERE t.cvterm_id = fprop.type_id AND t.name = '' ||
        quote_literal(p_type) || '' AND fprop.value = '' || quote_literal(p_val) || '';'';
    IF (STRPOS(p_val, ''%'') > 0) THEN
        query := ''SELECT DISTINCT fprop.feature_id 
            FROM featureprop fprop, cvterm t WHERE t.cvterm_id = fprop.type_id AND t.name = '' ||
            quote_literal(p_type) || '' AND fprop.value like '' || quote_literal(p_val) || '';'';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';

--- get all feature ids (including children) for feature with the property val
CREATE OR REPLACE FUNCTION get_feature_ids_by_propval(varchar) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    p_val alias for $1;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type%ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT fprop.feature_id 
        FROM featureprop fprop WHERE fprop.value = '' || quote_literal(p_val) || '';'';
    IF (STRPOS(p_val, ''%'') > 0) THEN
        query := ''SELECT DISTINCT fprop.feature_id 
            FROM featureprop fprop WHERE fprop.value like '' || quote_literal(p_val) || '';'';
    END IF;

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';


---4 args: ptype, ctype, count, operator (valid SQL number comparison operator), and is_analysis 
---get feature ids for any node with type = ptype whose child node type = ctype
---and child node feature count comparing (using operator) to ccount
CREATE OR REPLACE FUNCTION get_feature_ids_by_child_count(cvterm.name%TYPE, cvterm.name%TYPE, INTEGER, varchar, char(1)) RETURNS SETOF feature_by_fx_type AS
'
DECLARE
    ptype alias for $1;
    ctype alias for $2;
    ccount alias for $3;
    operator alias for $4;
    is_an alias for $5;
    query TEXT;
    myrc feature_by_fx_type%ROWTYPE;
    myrc2 feature_by_fx_type %ROWTYPE;

BEGIN

    query := ''SELECT DISTINCT f.feature_id
        FROM feature f INNER join (select count(*) as c, p.feature_id FROM feature p
        INNER join cvterm pt ON (p.type_id = pt.cvterm_id) INNER join feature_relationship fr
        ON (p.feature_id = fr.object_id) INNER join feature c ON (c.feature_id = fr.subject_id)
        INNER join cvterm ct ON (c.type_id = ct.cvterm_id)
        WHERE pt.name = '' || quote_literal(ptype) || '' AND ct.name = '' || quote_literal(ctype)
        || '' AND p.is_analysis = '' || quote_literal(is_an) || '' group by p.feature_id) as cq
        ON (cq.feature_id = f.feature_id) WHERE cq.c '' || operator || ccount || '';'';
    ---RAISE NOTICE ''%'', query; 

    FOR myrc IN SELECT * FROM get_feature_ids(query) LOOP
        RETURN NEXT myrc;
    END LOOP;
    RETURN;
END;
'
LANGUAGE 'plpgsql';
-- $Id: companalysis.sql,v 1.37 2007-03-23 15:18:02 scottcain Exp $
-- ==========================================

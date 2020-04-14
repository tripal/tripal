SET search_path=so,chado,pg_catalog;
--- ***                                          ***
--- *** A gene that rescues.                     ***
--- ************************************************
---

CREATE VIEW wild_type_rescue_gene AS
  SELECT
    feature_id AS wild_type_rescue_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'wild_type_rescue_gene';

--- ************************************************
--- *** relation: mitochondrial_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome originating in a mitochondr ***
--- *** ia.                                      ***
--- ************************************************
---

CREATE VIEW mitochondrial_chromosome AS
  SELECT
    feature_id AS mitochondrial_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mitochondrial_chromosome';

--- ************************************************
--- *** relation: chloroplast_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome originating in a chloroplas ***
--- *** t.                                       ***
--- ************************************************
---

CREATE VIEW chloroplast_chromosome AS
  SELECT
    feature_id AS chloroplast_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chloroplast_chromosome';

--- ************************************************
--- *** relation: chromoplast_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome originating in a chromoplas ***
--- *** t.                                       ***
--- ************************************************
---

CREATE VIEW chromoplast_chromosome AS
  SELECT
    feature_id AS chromoplast_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chromoplast_chromosome';

--- ************************************************
--- *** relation: cyanelle_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome originating in a cyanelle.  ***
--- ************************************************
---

CREATE VIEW cyanelle_chromosome AS
  SELECT
    feature_id AS cyanelle_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cyanelle_chromosome';

--- ************************************************
--- *** relation: leucoplast_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome with origin in a leucoplast ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW leucoplast_chromosome AS
  SELECT
    feature_id AS leucoplast_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'leucoplast_chromosome';

--- ************************************************
--- *** relation: macronuclear_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome originating in a macronucle ***
--- *** us.                                      ***
--- ************************************************
---

CREATE VIEW macronuclear_chromosome AS
  SELECT
    feature_id AS macronuclear_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'macronuclear_chromosome';

--- ************************************************
--- *** relation: micronuclear_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome originating in a micronucle ***
--- *** us.                                      ***
--- ************************************************
---

CREATE VIEW micronuclear_chromosome AS
  SELECT
    feature_id AS micronuclear_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'micronuclear_chromosome';

--- ************************************************
--- *** relation: nuclear_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome originating in a nucleus.   ***
--- ************************************************
---

CREATE VIEW nuclear_chromosome AS
  SELECT
    feature_id AS nuclear_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nuclear_chromosome';

--- ************************************************
--- *** relation: nucleomorphic_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome originating in a nucleomorp ***
--- *** h.                                       ***
--- ************************************************
---

CREATE VIEW nucleomorphic_chromosome AS
  SELECT
    feature_id AS nucleomorphic_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nucleomorphic_chromosome';

--- ************************************************
--- *** relation: chromosome_part ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a chromosome.                ***
--- ************************************************
---

CREATE VIEW chromosome_part AS
  SELECT
    feature_id AS chromosome_part_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chromosome_arm' OR cvterm.name = 'chromosome_band' OR cvterm.name = 'interband' OR cvterm.name = 'chromosomal_regulatory_element' OR cvterm.name = 'chromosomal_structural_element' OR cvterm.name = 'introgressed_chromosome_region' OR cvterm.name = 'matrix_attachment_site' OR cvterm.name = 'centromere' OR cvterm.name = 'telomere' OR cvterm.name = 'point_centromere' OR cvterm.name = 'regional_centromere' OR cvterm.name = 'chromosome_part';

--- ************************************************
--- *** relation: gene_member_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a gene.                      ***
--- ************************************************
---

CREATE VIEW gene_member_region AS
  SELECT
    feature_id AS gene_member_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transcript' OR cvterm.name = 'regulatory_region' OR cvterm.name = 'polycistronic_transcript' OR cvterm.name = 'transcript_with_translational_frameshift' OR cvterm.name = 'primary_transcript' OR cvterm.name = 'mature_transcript' OR cvterm.name = 'transcript_bound_by_nucleic_acid' OR cvterm.name = 'transcript_bound_by_protein' OR cvterm.name = 'enzymatic_RNA' OR cvterm.name = 'trans_spliced_transcript' OR cvterm.name = 'monocistronic_transcript' OR cvterm.name = 'aberrant_processed_transcript' OR cvterm.name = 'edited_transcript' OR cvterm.name = 'processed_transcript' OR cvterm.name = 'alternatively_spliced_transcript' OR cvterm.name = 'dicistronic_transcript' OR cvterm.name = 'polycistronic_primary_transcript' OR cvterm.name = 'polycistronic_mRNA' OR cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'dicistronic_primary_transcript' OR cvterm.name = 'dicistronic_primary_transcript' OR cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'protein_coding_primary_transcript' OR cvterm.name = 'nc_primary_transcript' OR cvterm.name = 'polycistronic_primary_transcript' OR cvterm.name = 'monocistronic_primary_transcript' OR cvterm.name = 'mini_exon_donor_RNA' OR cvterm.name = 'antisense_primary_transcript' OR cvterm.name = 'capped_primary_transcript' OR cvterm.name = 'pre_edited_mRNA' OR cvterm.name = 'scRNA_primary_transcript' OR cvterm.name = 'rRNA_primary_transcript' OR cvterm.name = 'tRNA_primary_transcript' OR cvterm.name = 'snRNA_primary_transcript' OR cvterm.name = 'snoRNA_primary_transcript' OR cvterm.name = 'tmRNA_primary_transcript' OR cvterm.name = 'SRP_RNA_primary_transcript' OR cvterm.name = 'miRNA_primary_transcript' OR cvterm.name = 'tasiRNA_primary_transcript' OR cvterm.name = 'rRNA_small_subunit_primary_transcript' OR cvterm.name = 'rRNA_large_subunit_primary_transcript' OR cvterm.name = 'alanine_tRNA_primary_transcript' OR cvterm.name = 'arginine_tRNA_primary_transcript' OR cvterm.name = 'asparagine_tRNA_primary_transcript' OR cvterm.name = 'aspartic_acid_tRNA_primary_transcript' OR cvterm.name = 'cysteine_tRNA_primary_transcript' OR cvterm.name = 'glutamic_acid_tRNA_primary_transcript' OR cvterm.name = 'glutamine_tRNA_primary_transcript' OR cvterm.name = 'glycine_tRNA_primary_transcript' OR cvterm.name = 'histidine_tRNA_primary_transcript' OR cvterm.name = 'isoleucine_tRNA_primary_transcript' OR cvterm.name = 'leucine_tRNA_primary_transcript' OR cvterm.name = 'lysine_tRNA_primary_transcript' OR cvterm.name = 'methionine_tRNA_primary_transcript' OR cvterm.name = 'phenylalanine_tRNA_primary_transcript' OR cvterm.name = 'proline_tRNA_primary_transcript' OR cvterm.name = 'serine_tRNA_primary_transcript' OR cvterm.name = 'threonine_tRNA_primary_transcript' OR cvterm.name = 'tryptophan_tRNA_primary_transcript' OR cvterm.name = 'tyrosine_tRNA_primary_transcript' OR cvterm.name = 'valine_tRNA_primary_transcript' OR cvterm.name = 'pyrrolysine_tRNA_primary_transcript' OR cvterm.name = 'selenocysteine_tRNA_primary_transcript' OR cvterm.name = 'methylation_guide_snoRNA_primary_transcript' OR cvterm.name = 'rRNA_cleavage_snoRNA_primary_transcript' OR cvterm.name = 'C_D_box_snoRNA_primary_transcript' OR cvterm.name = 'H_ACA_box_snoRNA_primary_transcript' OR cvterm.name = 'U14_snoRNA_primary_transcript' OR cvterm.name = 'stRNA_primary_transcript' OR cvterm.name = 'dicistronic_primary_transcript' OR cvterm.name = 'mRNA' OR cvterm.name = 'ncRNA' OR cvterm.name = 'mRNA_with_frameshift' OR cvterm.name = 'monocistronic_mRNA' OR cvterm.name = 'polycistronic_mRNA' OR cvterm.name = 'exemplar_mRNA' OR cvterm.name = 'capped_mRNA' OR cvterm.name = 'polyadenylated_mRNA' OR cvterm.name = 'trans_spliced_mRNA' OR cvterm.name = 'edited_mRNA' OR cvterm.name = 'consensus_mRNA' OR cvterm.name = 'recoded_mRNA' OR cvterm.name = 'mRNA_with_minus_1_frameshift' OR cvterm.name = 'mRNA_with_plus_1_frameshift' OR cvterm.name = 'mRNA_with_plus_2_frameshift' OR cvterm.name = 'mRNA_with_minus_2_frameshift' OR cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'mRNA_recoded_by_translational_bypass' OR cvterm.name = 'mRNA_recoded_by_codon_redefinition' OR cvterm.name = 'scRNA' OR cvterm.name = 'rRNA' OR cvterm.name = 'tRNA' OR cvterm.name = 'snRNA' OR cvterm.name = 'snoRNA' OR cvterm.name = 'small_regulatory_ncRNA' OR cvterm.name = 'RNase_MRP_RNA' OR cvterm.name = 'RNase_P_RNA' OR cvterm.name = 'telomerase_RNA' OR cvterm.name = 'vault_RNA' OR cvterm.name = 'Y_RNA' OR cvterm.name = 'rasiRNA' OR cvterm.name = 'SRP_RNA' OR cvterm.name = 'guide_RNA' OR cvterm.name = 'antisense_RNA' OR cvterm.name = 'siRNA' OR cvterm.name = 'stRNA' OR cvterm.name = 'class_II_RNA' OR cvterm.name = 'class_I_RNA' OR cvterm.name = 'piRNA' OR cvterm.name = 'lincRNA' OR cvterm.name = 'tasiRNA' OR cvterm.name = 'rRNA_cleavage_RNA' OR cvterm.name = 'small_subunit_rRNA' OR cvterm.name = 'large_subunit_rRNA' OR cvterm.name = 'rRNA_18S' OR cvterm.name = 'rRNA_16S' OR cvterm.name = 'rRNA_5_8S' OR cvterm.name = 'rRNA_5S' OR cvterm.name = 'rRNA_28S' OR cvterm.name = 'rRNA_23S' OR cvterm.name = 'rRNA_25S' OR cvterm.name = 'rRNA_21S' OR cvterm.name = 'alanyl_tRNA' OR cvterm.name = 'asparaginyl_tRNA' OR cvterm.name = 'aspartyl_tRNA' OR cvterm.name = 'cysteinyl_tRNA' OR cvterm.name = 'glutaminyl_tRNA' OR cvterm.name = 'glutamyl_tRNA' OR cvterm.name = 'glycyl_tRNA' OR cvterm.name = 'histidyl_tRNA' OR cvterm.name = 'isoleucyl_tRNA' OR cvterm.name = 'leucyl_tRNA' OR cvterm.name = 'lysyl_tRNA' OR cvterm.name = 'methionyl_tRNA' OR cvterm.name = 'phenylalanyl_tRNA' OR cvterm.name = 'prolyl_tRNA' OR cvterm.name = 'seryl_tRNA' OR cvterm.name = 'threonyl_tRNA' OR cvterm.name = 'tryptophanyl_tRNA' OR cvterm.name = 'tyrosyl_tRNA' OR cvterm.name = 'valyl_tRNA' OR cvterm.name = 'pyrrolysyl_tRNA' OR cvterm.name = 'arginyl_tRNA' OR cvterm.name = 'selenocysteinyl_tRNA' OR cvterm.name = 'U1_snRNA' OR cvterm.name = 'U2_snRNA' OR cvterm.name = 'U4_snRNA' OR cvterm.name = 'U4atac_snRNA' OR cvterm.name = 'U5_snRNA' OR cvterm.name = 'U6_snRNA' OR cvterm.name = 'U6atac_snRNA' OR cvterm.name = 'U11_snRNA' OR cvterm.name = 'U12_snRNA' OR cvterm.name = 'C_D_box_snoRNA' OR cvterm.name = 'H_ACA_box_snoRNA' OR cvterm.name = 'U14_snoRNA' OR cvterm.name = 'U3_snoRNA' OR cvterm.name = 'methylation_guide_snoRNA' OR cvterm.name = 'pseudouridylation_guide_snoRNA' OR cvterm.name = 'miRNA' OR cvterm.name = 'RNA_6S' OR cvterm.name = 'CsrB_RsmB_RNA' OR cvterm.name = 'DsrA_RNA' OR cvterm.name = 'OxyS_RNA' OR cvterm.name = 'RprA_RNA' OR cvterm.name = 'RRE_RNA' OR cvterm.name = 'spot_42_RNA' OR cvterm.name = 'tmRNA' OR cvterm.name = 'GcvB_RNA' OR cvterm.name = 'MicF_RNA' OR cvterm.name = 'ribozyme' OR cvterm.name = 'trans_spliced_mRNA' OR cvterm.name = 'monocistronic_primary_transcript' OR cvterm.name = 'monocistronic_mRNA' OR cvterm.name = 'edited_transcript_by_A_to_I_substitution' OR cvterm.name = 'edited_mRNA' OR cvterm.name = 'transcription_regulatory_region' OR cvterm.name = 'translation_regulatory_region' OR cvterm.name = 'recombination_regulatory_region' OR cvterm.name = 'replication_regulatory_region' OR cvterm.name = 'terminator' OR cvterm.name = 'TF_binding_site' OR cvterm.name = 'polyA_signal_sequence' OR cvterm.name = 'gene_group_regulatory_region' OR cvterm.name = 'transcriptional_cis_regulatory_region' OR cvterm.name = 'splicing_regulatory_region' OR cvterm.name = 'cis_regulatory_frameshift_element' OR cvterm.name = 'intronic_regulatory_region' OR cvterm.name = 'bacterial_terminator' OR cvterm.name = 'eukaryotic_terminator' OR cvterm.name = 'rho_dependent_bacterial_terminator' OR cvterm.name = 'rho_independent_bacterial_terminator' OR cvterm.name = 'terminator_of_type_2_RNApol_III_promoter' OR cvterm.name = 'operator' OR cvterm.name = 'bacterial_RNApol_promoter' OR cvterm.name = 'bacterial_terminator' OR cvterm.name = 'bacterial_RNApol_promoter_sigma_70' OR cvterm.name = 'bacterial_RNApol_promoter_sigma54' OR cvterm.name = 'rho_dependent_bacterial_terminator' OR cvterm.name = 'rho_independent_bacterial_terminator' OR cvterm.name = 'promoter' OR cvterm.name = 'insulator' OR cvterm.name = 'CRM' OR cvterm.name = 'promoter_targeting_sequence' OR cvterm.name = 'ISRE' OR cvterm.name = 'bidirectional_promoter' OR cvterm.name = 'RNA_polymerase_promoter' OR cvterm.name = 'RNApol_I_promoter' OR cvterm.name = 'RNApol_II_promoter' OR cvterm.name = 'RNApol_III_promoter' OR cvterm.name = 'bacterial_RNApol_promoter' OR cvterm.name = 'Phage_RNA_Polymerase_Promoter' OR cvterm.name = 'RNApol_II_core_promoter' OR cvterm.name = 'RNApol_III_promoter_type_1' OR cvterm.name = 'RNApol_III_promoter_type_2' OR cvterm.name = 'RNApol_III_promoter_type_3' OR cvterm.name = 'bacterial_RNApol_promoter_sigma_70' OR cvterm.name = 'bacterial_RNApol_promoter_sigma54' OR cvterm.name = 'SP6_RNA_Polymerase_Promoter' OR cvterm.name = 'T3_RNA_Polymerase_Promoter' OR cvterm.name = 'T7_RNA_Polymerase_Promoter' OR cvterm.name = 'locus_control_region' OR cvterm.name = 'enhancer' OR cvterm.name = 'silencer' OR cvterm.name = 'enhancer_bound_by_factor' OR cvterm.name = 'shadow_enhancer' OR cvterm.name = 'splice_enhancer' OR cvterm.name = 'intronic_splice_enhancer' OR cvterm.name = 'exonic_splice_enhancer' OR cvterm.name = 'attenuator' OR cvterm.name = 'gene_member_region';

--- ************************************************
--- *** relation: transcript_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a transcript.                ***
--- ************************************************
---

CREATE VIEW transcript_region AS
  SELECT
    feature_id AS transcript_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'exon' OR cvterm.name = 'edited_transcript_feature' OR cvterm.name = 'mature_transcript_region' OR cvterm.name = 'primary_transcript_region' OR cvterm.name = 'exon_region' OR cvterm.name = 'anchor_binding_site' OR cvterm.name = 'coding_exon' OR cvterm.name = 'noncoding_exon' OR cvterm.name = 'interior_exon' OR cvterm.name = 'exon_of_single_exon_gene' OR cvterm.name = 'interior_coding_exon' OR cvterm.name = 'five_prime_coding_exon' OR cvterm.name = 'three_prime_coding_exon' OR cvterm.name = 'three_prime_noncoding_exon' OR cvterm.name = 'five_prime_noncoding_exon' OR cvterm.name = 'pre_edited_region' OR cvterm.name = 'editing_block' OR cvterm.name = 'editing_domain' OR cvterm.name = 'unedited_region' OR cvterm.name = 'mRNA_region' OR cvterm.name = 'tmRNA_region' OR cvterm.name = 'guide_RNA_region' OR cvterm.name = 'tRNA_region' OR cvterm.name = 'riboswitch' OR cvterm.name = 'ribosome_entry_site' OR cvterm.name = 'UTR' OR cvterm.name = 'CDS' OR cvterm.name = 'five_prime_open_reading_frame' OR cvterm.name = 'UTR_region' OR cvterm.name = 'CDS_region' OR cvterm.name = 'translational_frameshift' OR cvterm.name = 'recoding_stimulatory_region' OR cvterm.name = 'internal_ribosome_entry_site' OR cvterm.name = 'Shine_Dalgarno_sequence' OR cvterm.name = 'kozak_sequence' OR cvterm.name = 'internal_Shine_Dalgarno_sequence' OR cvterm.name = 'five_prime_UTR' OR cvterm.name = 'three_prime_UTR' OR cvterm.name = 'internal_UTR' OR cvterm.name = 'untranslated_region_polycistronic_mRNA' OR cvterm.name = 'edited_CDS' OR cvterm.name = 'CDS_fragment' OR cvterm.name = 'CDS_independently_known' OR cvterm.name = 'CDS_predicted' OR cvterm.name = 'orphan_CDS' OR cvterm.name = 'CDS_supported_by_sequence_similarity_data' OR cvterm.name = 'CDS_supported_by_domain_match_data' OR cvterm.name = 'CDS_supported_by_EST_or_cDNA_data' OR cvterm.name = 'upstream_AUG_codon' OR cvterm.name = 'AU_rich_element' OR cvterm.name = 'Bruno_response_element' OR cvterm.name = 'iron_responsive_element' OR cvterm.name = 'coding_start' OR cvterm.name = 'coding_end' OR cvterm.name = 'codon' OR cvterm.name = 'recoded_codon' OR cvterm.name = 'start_codon' OR cvterm.name = 'stop_codon' OR cvterm.name = 'stop_codon_read_through' OR cvterm.name = 'stop_codon_redefined_as_pyrrolysine' OR cvterm.name = 'stop_codon_redefined_as_selenocysteine' OR cvterm.name = 'non_canonical_start_codon' OR cvterm.name = 'four_bp_start_codon' OR cvterm.name = 'CTG_start_codon' OR cvterm.name = 'plus_1_translational_frameshift' OR cvterm.name = 'plus_2_translational_frameshift' OR cvterm.name = 'internal_Shine_Dalgarno_sequence' OR cvterm.name = 'SECIS_element' OR cvterm.name = 'three_prime_recoding_site' OR cvterm.name = 'five_prime_recoding_site' OR cvterm.name = 'stop_codon_signal' OR cvterm.name = 'three_prime_stem_loop_structure' OR cvterm.name = 'flanking_three_prime_quadruplet_recoding_signal' OR cvterm.name = 'three_prime_repeat_recoding_signal' OR cvterm.name = 'distant_three_prime_recoding_signal' OR cvterm.name = 'UAG_stop_codon_signal' OR cvterm.name = 'UAA_stop_codon_signal' OR cvterm.name = 'UGA_stop_codon_signal' OR cvterm.name = 'tmRNA_coding_piece' OR cvterm.name = 'tmRNA_acceptor_piece' OR cvterm.name = 'anchor_region' OR cvterm.name = 'template_region' OR cvterm.name = 'anticodon_loop' OR cvterm.name = 'anticodon' OR cvterm.name = 'CCA_tail' OR cvterm.name = 'DHU_loop' OR cvterm.name = 'T_loop' OR cvterm.name = 'splice_site' OR cvterm.name = 'intron' OR cvterm.name = 'clip' OR cvterm.name = 'TSS' OR cvterm.name = 'transcription_end_site' OR cvterm.name = 'spliced_leader_RNA' OR cvterm.name = 'rRNA_primary_transcript_region' OR cvterm.name = 'spliceosomal_intron_region' OR cvterm.name = 'intron_domain' OR cvterm.name = 'miRNA_primary_transcript_region' OR cvterm.name = 'outron' OR cvterm.name = 'cis_splice_site' OR cvterm.name = 'trans_splice_site' OR cvterm.name = 'cryptic_splice_site' OR cvterm.name = 'five_prime_cis_splice_site' OR cvterm.name = 'three_prime_cis_splice_site' OR cvterm.name = 'recursive_splice_site' OR cvterm.name = 'canonical_five_prime_splice_site' OR cvterm.name = 'non_canonical_five_prime_splice_site' OR cvterm.name = 'canonical_three_prime_splice_site' OR cvterm.name = 'non_canonical_three_prime_splice_site' OR cvterm.name = 'trans_splice_acceptor_site' OR cvterm.name = 'trans_splice_donor_site' OR cvterm.name = 'SL1_acceptor_site' OR cvterm.name = 'SL2_acceptor_site' OR cvterm.name = 'SL3_acceptor_site' OR cvterm.name = 'SL4_acceptor_site' OR cvterm.name = 'SL5_acceptor_site' OR cvterm.name = 'SL6_acceptor_site' OR cvterm.name = 'SL7_acceptor_site' OR cvterm.name = 'SL8_acceptor_site' OR cvterm.name = 'SL9_acceptor_site' OR cvterm.name = 'SL10_accceptor_site' OR cvterm.name = 'SL11_acceptor_site' OR cvterm.name = 'SL12_acceptor_site' OR cvterm.name = 'five_prime_intron' OR cvterm.name = 'interior_intron' OR cvterm.name = 'three_prime_intron' OR cvterm.name = 'twintron' OR cvterm.name = 'UTR_intron' OR cvterm.name = 'autocatalytically_spliced_intron' OR cvterm.name = 'spliceosomal_intron' OR cvterm.name = 'mobile_intron' OR cvterm.name = 'endonuclease_spliced_intron' OR cvterm.name = 'five_prime_UTR_intron' OR cvterm.name = 'three_prime_UTR_intron' OR cvterm.name = 'group_I_intron' OR cvterm.name = 'group_II_intron' OR cvterm.name = 'group_III_intron' OR cvterm.name = 'group_IIA_intron' OR cvterm.name = 'group_IIB_intron' OR cvterm.name = 'U2_intron' OR cvterm.name = 'U12_intron' OR cvterm.name = 'archaeal_intron' OR cvterm.name = 'tRNA_intron' OR cvterm.name = 'five_prime_clip' OR cvterm.name = 'three_prime_clip' OR cvterm.name = 'major_TSS' OR cvterm.name = 'minor_TSS' OR cvterm.name = 'transcribed_spacer_region' OR cvterm.name = 'internal_transcribed_spacer_region' OR cvterm.name = 'external_transcribed_spacer_region' OR cvterm.name = 'intronic_splice_enhancer' OR cvterm.name = 'branch_site' OR cvterm.name = 'polypyrimidine_tract' OR cvterm.name = 'internal_guide_sequence' OR cvterm.name = 'mirtron' OR cvterm.name = 'pre_miRNA' OR cvterm.name = 'miRNA_stem' OR cvterm.name = 'miRNA_loop' OR cvterm.name = 'miRNA_antiguide' OR cvterm.name = 'noncoding_region_of_exon' OR cvterm.name = 'coding_region_of_exon' OR cvterm.name = 'three_prime_coding_exon_noncoding_region' OR cvterm.name = 'five_prime_coding_exon_noncoding_region' OR cvterm.name = 'five_prime_coding_exon_coding_region' OR cvterm.name = 'three_prime_coding_exon_coding_region' OR cvterm.name = 'transcript_region';

--- ************************************************
--- *** relation: mature_transcript_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a mature transcript.         ***
--- ************************************************
---

CREATE VIEW mature_transcript_region AS
  SELECT
    feature_id AS mature_transcript_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mRNA_region' OR cvterm.name = 'tmRNA_region' OR cvterm.name = 'guide_RNA_region' OR cvterm.name = 'tRNA_region' OR cvterm.name = 'riboswitch' OR cvterm.name = 'ribosome_entry_site' OR cvterm.name = 'UTR' OR cvterm.name = 'CDS' OR cvterm.name = 'five_prime_open_reading_frame' OR cvterm.name = 'UTR_region' OR cvterm.name = 'CDS_region' OR cvterm.name = 'translational_frameshift' OR cvterm.name = 'recoding_stimulatory_region' OR cvterm.name = 'internal_ribosome_entry_site' OR cvterm.name = 'Shine_Dalgarno_sequence' OR cvterm.name = 'kozak_sequence' OR cvterm.name = 'internal_Shine_Dalgarno_sequence' OR cvterm.name = 'five_prime_UTR' OR cvterm.name = 'three_prime_UTR' OR cvterm.name = 'internal_UTR' OR cvterm.name = 'untranslated_region_polycistronic_mRNA' OR cvterm.name = 'edited_CDS' OR cvterm.name = 'CDS_fragment' OR cvterm.name = 'CDS_independently_known' OR cvterm.name = 'CDS_predicted' OR cvterm.name = 'orphan_CDS' OR cvterm.name = 'CDS_supported_by_sequence_similarity_data' OR cvterm.name = 'CDS_supported_by_domain_match_data' OR cvterm.name = 'CDS_supported_by_EST_or_cDNA_data' OR cvterm.name = 'upstream_AUG_codon' OR cvterm.name = 'AU_rich_element' OR cvterm.name = 'Bruno_response_element' OR cvterm.name = 'iron_responsive_element' OR cvterm.name = 'coding_start' OR cvterm.name = 'coding_end' OR cvterm.name = 'codon' OR cvterm.name = 'recoded_codon' OR cvterm.name = 'start_codon' OR cvterm.name = 'stop_codon' OR cvterm.name = 'stop_codon_read_through' OR cvterm.name = 'stop_codon_redefined_as_pyrrolysine' OR cvterm.name = 'stop_codon_redefined_as_selenocysteine' OR cvterm.name = 'non_canonical_start_codon' OR cvterm.name = 'four_bp_start_codon' OR cvterm.name = 'CTG_start_codon' OR cvterm.name = 'plus_1_translational_frameshift' OR cvterm.name = 'plus_2_translational_frameshift' OR cvterm.name = 'internal_Shine_Dalgarno_sequence' OR cvterm.name = 'SECIS_element' OR cvterm.name = 'three_prime_recoding_site' OR cvterm.name = 'five_prime_recoding_site' OR cvterm.name = 'stop_codon_signal' OR cvterm.name = 'three_prime_stem_loop_structure' OR cvterm.name = 'flanking_three_prime_quadruplet_recoding_signal' OR cvterm.name = 'three_prime_repeat_recoding_signal' OR cvterm.name = 'distant_three_prime_recoding_signal' OR cvterm.name = 'UAG_stop_codon_signal' OR cvterm.name = 'UAA_stop_codon_signal' OR cvterm.name = 'UGA_stop_codon_signal' OR cvterm.name = 'tmRNA_coding_piece' OR cvterm.name = 'tmRNA_acceptor_piece' OR cvterm.name = 'anchor_region' OR cvterm.name = 'template_region' OR cvterm.name = 'anticodon_loop' OR cvterm.name = 'anticodon' OR cvterm.name = 'CCA_tail' OR cvterm.name = 'DHU_loop' OR cvterm.name = 'T_loop' OR cvterm.name = 'mature_transcript_region';

--- ************************************************
--- *** relation: primary_transcript_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A part of a primary transcript.          ***
--- ************************************************
---

CREATE VIEW primary_transcript_region AS
  SELECT
    feature_id AS primary_transcript_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'splice_site' OR cvterm.name = 'intron' OR cvterm.name = 'clip' OR cvterm.name = 'TSS' OR cvterm.name = 'transcription_end_site' OR cvterm.name = 'spliced_leader_RNA' OR cvterm.name = 'rRNA_primary_transcript_region' OR cvterm.name = 'spliceosomal_intron_region' OR cvterm.name = 'intron_domain' OR cvterm.name = 'miRNA_primary_transcript_region' OR cvterm.name = 'outron' OR cvterm.name = 'cis_splice_site' OR cvterm.name = 'trans_splice_site' OR cvterm.name = 'cryptic_splice_site' OR cvterm.name = 'five_prime_cis_splice_site' OR cvterm.name = 'three_prime_cis_splice_site' OR cvterm.name = 'recursive_splice_site' OR cvterm.name = 'canonical_five_prime_splice_site' OR cvterm.name = 'non_canonical_five_prime_splice_site' OR cvterm.name = 'canonical_three_prime_splice_site' OR cvterm.name = 'non_canonical_three_prime_splice_site' OR cvterm.name = 'trans_splice_acceptor_site' OR cvterm.name = 'trans_splice_donor_site' OR cvterm.name = 'SL1_acceptor_site' OR cvterm.name = 'SL2_acceptor_site' OR cvterm.name = 'SL3_acceptor_site' OR cvterm.name = 'SL4_acceptor_site' OR cvterm.name = 'SL5_acceptor_site' OR cvterm.name = 'SL6_acceptor_site' OR cvterm.name = 'SL7_acceptor_site' OR cvterm.name = 'SL8_acceptor_site' OR cvterm.name = 'SL9_acceptor_site' OR cvterm.name = 'SL10_accceptor_site' OR cvterm.name = 'SL11_acceptor_site' OR cvterm.name = 'SL12_acceptor_site' OR cvterm.name = 'five_prime_intron' OR cvterm.name = 'interior_intron' OR cvterm.name = 'three_prime_intron' OR cvterm.name = 'twintron' OR cvterm.name = 'UTR_intron' OR cvterm.name = 'autocatalytically_spliced_intron' OR cvterm.name = 'spliceosomal_intron' OR cvterm.name = 'mobile_intron' OR cvterm.name = 'endonuclease_spliced_intron' OR cvterm.name = 'five_prime_UTR_intron' OR cvterm.name = 'three_prime_UTR_intron' OR cvterm.name = 'group_I_intron' OR cvterm.name = 'group_II_intron' OR cvterm.name = 'group_III_intron' OR cvterm.name = 'group_IIA_intron' OR cvterm.name = 'group_IIB_intron' OR cvterm.name = 'U2_intron' OR cvterm.name = 'U12_intron' OR cvterm.name = 'archaeal_intron' OR cvterm.name = 'tRNA_intron' OR cvterm.name = 'five_prime_clip' OR cvterm.name = 'three_prime_clip' OR cvterm.name = 'major_TSS' OR cvterm.name = 'minor_TSS' OR cvterm.name = 'transcribed_spacer_region' OR cvterm.name = 'internal_transcribed_spacer_region' OR cvterm.name = 'external_transcribed_spacer_region' OR cvterm.name = 'intronic_splice_enhancer' OR cvterm.name = 'branch_site' OR cvterm.name = 'polypyrimidine_tract' OR cvterm.name = 'internal_guide_sequence' OR cvterm.name = 'mirtron' OR cvterm.name = 'pre_miRNA' OR cvterm.name = 'miRNA_stem' OR cvterm.name = 'miRNA_loop' OR cvterm.name = 'miRNA_antiguide' OR cvterm.name = 'primary_transcript_region';

--- ************************************************
--- *** relation: mrna_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of an mRNA.                     ***
--- ************************************************
---

CREATE VIEW mrna_region AS
  SELECT
    feature_id AS mrna_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'riboswitch' OR cvterm.name = 'ribosome_entry_site' OR cvterm.name = 'UTR' OR cvterm.name = 'CDS' OR cvterm.name = 'five_prime_open_reading_frame' OR cvterm.name = 'UTR_region' OR cvterm.name = 'CDS_region' OR cvterm.name = 'translational_frameshift' OR cvterm.name = 'recoding_stimulatory_region' OR cvterm.name = 'internal_ribosome_entry_site' OR cvterm.name = 'Shine_Dalgarno_sequence' OR cvterm.name = 'kozak_sequence' OR cvterm.name = 'internal_Shine_Dalgarno_sequence' OR cvterm.name = 'five_prime_UTR' OR cvterm.name = 'three_prime_UTR' OR cvterm.name = 'internal_UTR' OR cvterm.name = 'untranslated_region_polycistronic_mRNA' OR cvterm.name = 'edited_CDS' OR cvterm.name = 'CDS_fragment' OR cvterm.name = 'CDS_independently_known' OR cvterm.name = 'CDS_predicted' OR cvterm.name = 'orphan_CDS' OR cvterm.name = 'CDS_supported_by_sequence_similarity_data' OR cvterm.name = 'CDS_supported_by_domain_match_data' OR cvterm.name = 'CDS_supported_by_EST_or_cDNA_data' OR cvterm.name = 'upstream_AUG_codon' OR cvterm.name = 'AU_rich_element' OR cvterm.name = 'Bruno_response_element' OR cvterm.name = 'iron_responsive_element' OR cvterm.name = 'coding_start' OR cvterm.name = 'coding_end' OR cvterm.name = 'codon' OR cvterm.name = 'recoded_codon' OR cvterm.name = 'start_codon' OR cvterm.name = 'stop_codon' OR cvterm.name = 'stop_codon_read_through' OR cvterm.name = 'stop_codon_redefined_as_pyrrolysine' OR cvterm.name = 'stop_codon_redefined_as_selenocysteine' OR cvterm.name = 'non_canonical_start_codon' OR cvterm.name = 'four_bp_start_codon' OR cvterm.name = 'CTG_start_codon' OR cvterm.name = 'plus_1_translational_frameshift' OR cvterm.name = 'plus_2_translational_frameshift' OR cvterm.name = 'internal_Shine_Dalgarno_sequence' OR cvterm.name = 'SECIS_element' OR cvterm.name = 'three_prime_recoding_site' OR cvterm.name = 'five_prime_recoding_site' OR cvterm.name = 'stop_codon_signal' OR cvterm.name = 'three_prime_stem_loop_structure' OR cvterm.name = 'flanking_three_prime_quadruplet_recoding_signal' OR cvterm.name = 'three_prime_repeat_recoding_signal' OR cvterm.name = 'distant_three_prime_recoding_signal' OR cvterm.name = 'UAG_stop_codon_signal' OR cvterm.name = 'UAA_stop_codon_signal' OR cvterm.name = 'UGA_stop_codon_signal' OR cvterm.name = 'mRNA_region';

--- ************************************************
--- *** relation: utr_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of UTR.                         ***
--- ************************************************
---

CREATE VIEW utr_region AS
  SELECT
    feature_id AS utr_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'upstream_AUG_codon' OR cvterm.name = 'AU_rich_element' OR cvterm.name = 'Bruno_response_element' OR cvterm.name = 'iron_responsive_element' OR cvterm.name = 'UTR_region';

--- ************************************************
--- *** relation: rrna_primary_transcript_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of an rRNA primary transcript.  ***
--- ************************************************
---

CREATE VIEW rrna_primary_transcript_region AS
  SELECT
    feature_id AS rrna_primary_transcript_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transcribed_spacer_region' OR cvterm.name = 'internal_transcribed_spacer_region' OR cvterm.name = 'external_transcribed_spacer_region' OR cvterm.name = 'rRNA_primary_transcript_region';

--- ************************************************
--- *** relation: polypeptide_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Biological sequence region that can be a ***
--- *** ssigned to a specific subsequence of a p ***
--- *** olypeptide.                              ***
--- ************************************************
---

CREATE VIEW polypeptide_region AS
  SELECT
    feature_id AS polypeptide_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mature_protein_region' OR cvterm.name = 'immature_peptide_region' OR cvterm.name = 'compositionally_biased_region_of_peptide' OR cvterm.name = 'polypeptide_structural_region' OR cvterm.name = 'polypeptide_variation_site' OR cvterm.name = 'peptide_localization_signal' OR cvterm.name = 'cleaved_peptide_region' OR cvterm.name = 'hydrophobic_region_of_peptide' OR cvterm.name = 'polypeptide_conserved_region' OR cvterm.name = 'active_peptide' OR cvterm.name = 'polypeptide_domain' OR cvterm.name = 'membrane_structure' OR cvterm.name = 'extramembrane_polypeptide_region' OR cvterm.name = 'intramembrane_polypeptide_region' OR cvterm.name = 'polypeptide_secondary_structure' OR cvterm.name = 'polypeptide_structural_motif' OR cvterm.name = 'intrinsically_unstructured_polypeptide_region' OR cvterm.name = 'cytoplasmic_polypeptide_region' OR cvterm.name = 'non_cytoplasmic_polypeptide_region' OR cvterm.name = 'membrane_peptide_loop' OR cvterm.name = 'transmembrane_polypeptide_region' OR cvterm.name = 'asx_motif' OR cvterm.name = 'beta_bulge' OR cvterm.name = 'beta_bulge_loop' OR cvterm.name = 'beta_strand' OR cvterm.name = 'peptide_helix' OR cvterm.name = 'polypeptide_nest_motif' OR cvterm.name = 'schellmann_loop' OR cvterm.name = 'serine_threonine_motif' OR cvterm.name = 'serine_threonine_staple_motif' OR cvterm.name = 'polypeptide_turn_motif' OR cvterm.name = 'catmat_left_handed_three' OR cvterm.name = 'catmat_left_handed_four' OR cvterm.name = 'catmat_right_handed_three' OR cvterm.name = 'catmat_right_handed_four' OR cvterm.name = 'alpha_beta_motif' OR cvterm.name = 'peptide_coil' OR cvterm.name = 'beta_bulge_loop_five' OR cvterm.name = 'beta_bulge_loop_six' OR cvterm.name = 'antiparallel_beta_strand' OR cvterm.name = 'parallel_beta_strand' OR cvterm.name = 'left_handed_peptide_helix' OR cvterm.name = 'right_handed_peptide_helix' OR cvterm.name = 'alpha_helix' OR cvterm.name = 'pi_helix' OR cvterm.name = 'three_ten_helix' OR cvterm.name = 'polypeptide_nest_left_right_motif' OR cvterm.name = 'polypeptide_nest_right_left_motif' OR cvterm.name = 'schellmann_loop_seven' OR cvterm.name = 'schellmann_loop_six' OR cvterm.name = 'asx_turn' OR cvterm.name = 'beta_turn' OR cvterm.name = 'gamma_turn' OR cvterm.name = 'serine_threonine_turn' OR cvterm.name = 'asx_turn_left_handed_type_one' OR cvterm.name = 'asx_turn_left_handed_type_two' OR cvterm.name = 'asx_turn_right_handed_type_two' OR cvterm.name = 'asx_turn_right_handed_type_one' OR cvterm.name = 'beta_turn_left_handed_type_one' OR cvterm.name = 'beta_turn_left_handed_type_two' OR cvterm.name = 'beta_turn_right_handed_type_one' OR cvterm.name = 'beta_turn_right_handed_type_two' OR cvterm.name = 'beta_turn_type_six' OR cvterm.name = 'beta_turn_type_eight' OR cvterm.name = 'beta_turn_type_six_a' OR cvterm.name = 'beta_turn_type_six_b' OR cvterm.name = 'beta_turn_type_six_a_one' OR cvterm.name = 'beta_turn_type_six_a_two' OR cvterm.name = 'gamma_turn_classic' OR cvterm.name = 'gamma_turn_inverse' OR cvterm.name = 'st_turn_left_handed_type_one' OR cvterm.name = 'st_turn_left_handed_type_two' OR cvterm.name = 'st_turn_right_handed_type_one' OR cvterm.name = 'st_turn_right_handed_type_two' OR cvterm.name = 'coiled_coil' OR cvterm.name = 'helix_turn_helix' OR cvterm.name = 'natural_variant_site' OR cvterm.name = 'mutated_variant_site' OR cvterm.name = 'alternate_sequence_site' OR cvterm.name = 'signal_peptide' OR cvterm.name = 'transit_peptide' OR cvterm.name = 'nuclear_localization_signal' OR cvterm.name = 'endosomal_localization_signal' OR cvterm.name = 'lysosomal_localization_signal' OR cvterm.name = 'nuclear_export_signal' OR cvterm.name = 'nuclear_rim_localization_signal' OR cvterm.name = 'cleaved_initiator_methionine' OR cvterm.name = 'intein' OR cvterm.name = 'propeptide_cleavage_site' OR cvterm.name = 'propeptide' OR cvterm.name = 'cleaved_for_gpi_anchor_region' OR cvterm.name = 'lipoprotein_signal_peptide' OR cvterm.name = 'n_terminal_region' OR cvterm.name = 'c_terminal_region' OR cvterm.name = 'central_hydrophobic_region_of_signal_peptide' OR cvterm.name = 'polypeptide_domain' OR cvterm.name = 'polypeptide_motif' OR cvterm.name = 'polypeptide_repeat' OR cvterm.name = 'biochemical_region_of_peptide' OR cvterm.name = 'polypeptide_conserved_motif' OR cvterm.name = 'post_translationally_modified_region' OR cvterm.name = 'conformational_switch' OR cvterm.name = 'molecular_contact_region' OR cvterm.name = 'polypeptide_binding_motif' OR cvterm.name = 'polypeptide_catalytic_motif' OR cvterm.name = 'histone_modification' OR cvterm.name = 'histone_methylation_site' OR cvterm.name = 'histone_acetylation_site' OR cvterm.name = 'histone_ubiqitination_site' OR cvterm.name = 'histone_acylation_region' OR cvterm.name = 'H4K20_monomethylation_site' OR cvterm.name = 'H2BK5_monomethylation_site' OR cvterm.name = 'H3K27_methylation_site' OR cvterm.name = 'H3K36_methylation_site' OR cvterm.name = 'H3K4_methylation_site' OR cvterm.name = 'H3K79_methylation_site' OR cvterm.name = 'H3K9_methylation_site' OR cvterm.name = 'H3K27_monomethylation_site' OR cvterm.name = 'H3K27_trimethylation_site' OR cvterm.name = 'H3K27_dimethylation_site' OR cvterm.name = 'H3K36_monomethylation_site' OR cvterm.name = 'H3K36_dimethylation_site' OR cvterm.name = 'H3K36_trimethylation_site' OR cvterm.name = 'H3K4_monomethylation_site' OR cvterm.name = 'H3K4_trimethylation' OR cvterm.name = 'H3K4_dimethylation_site' OR cvterm.name = 'H3K79_monomethylation_site' OR cvterm.name = 'H3K79_dimethylation_site' OR cvterm.name = 'H3K79_trimethylation_site' OR cvterm.name = 'H3K9_trimethylation_site' OR cvterm.name = 'H3K9_monomethylation_site' OR cvterm.name = 'H3K9_dimethylation_site' OR cvterm.name = 'H3K9_acetylation_site' OR cvterm.name = 'H3K14_acetylation_site' OR cvterm.name = 'H3K18_acetylation_site' OR cvterm.name = 'H3K23_acylation site' OR cvterm.name = 'H3K27_acylation_site' OR cvterm.name = 'H4K16_acylation_site' OR cvterm.name = 'H4K5_acylation_site' OR cvterm.name = 'H4K8_acylation site' OR cvterm.name = 'H2B_ubiquitination_site' OR cvterm.name = 'H4K_acylation_region' OR cvterm.name = 'polypeptide_metal_contact' OR cvterm.name = 'protein_protein_contact' OR cvterm.name = 'polypeptide_ligand_contact' OR cvterm.name = 'polypeptide_DNA_contact' OR cvterm.name = 'polypeptide_calcium_ion_contact_site' OR cvterm.name = 'polypeptide_cobalt_ion_contact_site' OR cvterm.name = 'polypeptide_copper_ion_contact_site' OR cvterm.name = 'polypeptide_iron_ion_contact_site' OR cvterm.name = 'polypeptide_magnesium_ion_contact_site' OR cvterm.name = 'polypeptide_manganese_ion_contact_site' OR cvterm.name = 'polypeptide_molybdenum_ion_contact_site' OR cvterm.name = 'polypeptide_nickel_ion_contact_site' OR cvterm.name = 'polypeptide_tungsten_ion_contact_site' OR cvterm.name = 'polypeptide_zinc_ion_contact_site' OR cvterm.name = 'polypeptide_region';

--- ************************************************
--- *** relation: repeat_component ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a repeated sequence.         ***
--- ************************************************
---

CREATE VIEW repeat_component AS
  SELECT
    feature_id AS repeat_component_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'non_LTR_retrotransposon_polymeric_tract' OR cvterm.name = 'LTR_component' OR cvterm.name = 'repeat_fragment' OR cvterm.name = 'transposon_fragment' OR cvterm.name = 'U5_LTR_region' OR cvterm.name = 'R_LTR_region' OR cvterm.name = 'U3_LTR_region' OR cvterm.name = 'three_prime_LTR_component' OR cvterm.name = 'five_prime_LTR_component' OR cvterm.name = 'U5_five_prime_LTR_region' OR cvterm.name = 'R_five_prime_LTR_region' OR cvterm.name = 'U3_five_prime_LTR_region' OR cvterm.name = 'R_three_prime_LTR_region' OR cvterm.name = 'U3_three_prime_LTR_region' OR cvterm.name = 'U5_three_prime_LTR_region' OR cvterm.name = 'R_five_prime_LTR_region' OR cvterm.name = 'U5_five_prime_LTR_region' OR cvterm.name = 'U3_five_prime_LTR_region' OR cvterm.name = 'repeat_component';

--- ************************************************
--- *** relation: spliceosomal_intron_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region within an intron.               ***
--- ************************************************
---

CREATE VIEW spliceosomal_intron_region AS
  SELECT
    feature_id AS spliceosomal_intron_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'intronic_splice_enhancer' OR cvterm.name = 'branch_site' OR cvterm.name = 'polypyrimidine_tract' OR cvterm.name = 'spliceosomal_intron_region';

--- ************************************************
--- *** relation: gene_component_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW gene_component_region AS
  SELECT
    feature_id AS gene_component_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'non_transcribed_region' OR cvterm.name = 'gene_fragment' OR cvterm.name = 'TSS_region' OR cvterm.name = 'gene_segment' OR cvterm.name = 'pseudogenic_gene_segment' OR cvterm.name = 'gene_component_region';

--- ************************************************
--- *** relation: tmrna_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a tmRNA.                     ***
--- ************************************************
---

CREATE VIEW tmrna_region AS
  SELECT
    feature_id AS tmrna_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tmRNA_coding_piece' OR cvterm.name = 'tmRNA_acceptor_piece' OR cvterm.name = 'tmRNA_region';

--- ************************************************
--- *** relation: ltr_component ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW ltr_component AS
  SELECT
    feature_id AS ltr_component_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U5_LTR_region' OR cvterm.name = 'R_LTR_region' OR cvterm.name = 'U3_LTR_region' OR cvterm.name = 'three_prime_LTR_component' OR cvterm.name = 'five_prime_LTR_component' OR cvterm.name = 'U5_five_prime_LTR_region' OR cvterm.name = 'R_five_prime_LTR_region' OR cvterm.name = 'U3_five_prime_LTR_region' OR cvterm.name = 'R_three_prime_LTR_region' OR cvterm.name = 'U3_three_prime_LTR_region' OR cvterm.name = 'U5_three_prime_LTR_region' OR cvterm.name = 'R_five_prime_LTR_region' OR cvterm.name = 'U5_five_prime_LTR_region' OR cvterm.name = 'U3_five_prime_LTR_region' OR cvterm.name = 'LTR_component';

--- ************************************************
--- *** relation: three_prime_ltr_component ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW three_prime_ltr_component AS
  SELECT
    feature_id AS three_prime_ltr_component_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'R_three_prime_LTR_region' OR cvterm.name = 'U3_three_prime_LTR_region' OR cvterm.name = 'U5_three_prime_LTR_region' OR cvterm.name = 'three_prime_LTR_component';

--- ************************************************
--- *** relation: five_prime_ltr_component ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW five_prime_ltr_component AS
  SELECT
    feature_id AS five_prime_ltr_component_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'R_five_prime_LTR_region' OR cvterm.name = 'U5_five_prime_LTR_region' OR cvterm.name = 'U3_five_prime_LTR_region' OR cvterm.name = 'five_prime_LTR_component';

--- ************************************************
--- *** relation: cds_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a CDS.                       ***
--- ************************************************
---

CREATE VIEW cds_region AS
  SELECT
    feature_id AS cds_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'coding_start' OR cvterm.name = 'coding_end' OR cvterm.name = 'codon' OR cvterm.name = 'recoded_codon' OR cvterm.name = 'start_codon' OR cvterm.name = 'stop_codon' OR cvterm.name = 'stop_codon_read_through' OR cvterm.name = 'stop_codon_redefined_as_pyrrolysine' OR cvterm.name = 'stop_codon_redefined_as_selenocysteine' OR cvterm.name = 'non_canonical_start_codon' OR cvterm.name = 'four_bp_start_codon' OR cvterm.name = 'CTG_start_codon' OR cvterm.name = 'CDS_region';

--- ************************************************
--- *** relation: exon_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of an exon.                     ***
--- ************************************************
---

CREATE VIEW exon_region AS
  SELECT
    feature_id AS exon_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'noncoding_region_of_exon' OR cvterm.name = 'coding_region_of_exon' OR cvterm.name = 'three_prime_coding_exon_noncoding_region' OR cvterm.name = 'five_prime_coding_exon_noncoding_region' OR cvterm.name = 'five_prime_coding_exon_coding_region' OR cvterm.name = 'three_prime_coding_exon_coding_region' OR cvterm.name = 'exon_region';

--- ************************************************
--- *** relation: homologous_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region that is homologous to another r ***
--- *** egion.                                   ***
--- ************************************************
---

CREATE VIEW homologous_region AS
  SELECT
    feature_id AS homologous_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'paralogous_region' OR cvterm.name = 'orthologous_region' OR cvterm.name = 'homologous_region';

--- ************************************************
--- *** relation: paralogous_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A homologous_region that is paralogous t ***
--- *** o another region.                        ***
--- ************************************************
---

CREATE VIEW paralogous_region AS
  SELECT
    feature_id AS paralogous_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'paralogous_region';

--- ************************************************
--- *** relation: orthologous_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A homologous_region that is orthologous  ***
--- *** to another region.                       ***
--- ************************************************
---

CREATE VIEW orthologous_region AS
  SELECT
    feature_id AS orthologous_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'orthologous_region';

--- ************************************************
--- *** relation: conserved ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW conserved AS
  SELECT
    feature_id AS conserved_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'homologous' OR cvterm.name = 'syntenic' OR cvterm.name = 'orthologous' OR cvterm.name = 'paralogous' OR cvterm.name = 'conserved';

--- ************************************************
--- *** relation: homologous ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Similarity due to common ancestry.       ***
--- ************************************************
---

CREATE VIEW homologous AS
  SELECT
    feature_id AS homologous_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'orthologous' OR cvterm.name = 'paralogous' OR cvterm.name = 'homologous';

--- ************************************************
--- *** relation: orthologous ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a kind of homolo ***
--- *** gy where divergence occured after a spec ***
--- *** iation event.                            ***
--- ************************************************
---

CREATE VIEW orthologous AS
  SELECT
    feature_id AS orthologous_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'orthologous';

--- ************************************************
--- *** relation: paralogous ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a kind of homolo ***
--- *** gy where divergence occurred after a dup ***
--- *** lication event.                          ***
--- ************************************************
---

CREATE VIEW paralogous AS
  SELECT
    feature_id AS paralogous_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'paralogous';

--- ************************************************
--- *** relation: syntenic ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Attribute describing sequence regions oc ***
--- *** curring in same order on chromosome of d ***
--- *** ifferent species.                        ***
--- ************************************************
---

CREATE VIEW syntenic AS
  SELECT
    feature_id AS syntenic_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'syntenic';

--- ************************************************
--- *** relation: capped_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript that is capped.     ***
--- ************************************************
---

CREATE VIEW capped_primary_transcript AS
  SELECT
    feature_id AS capped_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'capped_primary_transcript';

--- ************************************************
--- *** relation: capped_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An mRNA that is capped.                  ***
--- ************************************************
---

CREATE VIEW capped_mrna AS
  SELECT
    feature_id AS capped_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'capped_mRNA';

--- ************************************************
--- *** relation: mrna_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing an mRNA feature. ***
--- ************************************************
---

CREATE VIEW mrna_attribute AS
  SELECT
    feature_id AS mrna_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polyadenylated' OR cvterm.name = 'exemplar' OR cvterm.name = 'frameshift' OR cvterm.name = 'recoded' OR cvterm.name = 'minus_1_frameshift' OR cvterm.name = 'minus_2_frameshift' OR cvterm.name = 'plus_1_frameshift' OR cvterm.name = 'plus_2_framshift' OR cvterm.name = 'codon_redefined' OR cvterm.name = 'recoded_by_translational_bypass' OR cvterm.name = 'translationally_frameshifted' OR cvterm.name = 'minus_1_translationally_frameshifted' OR cvterm.name = 'plus_1_translationally_frameshifted' OR cvterm.name = 'mRNA_attribute';

--- ************************************************
--- *** relation: exemplar ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence is re ***
--- *** presentative of a class of similar seque ***
--- *** nces.                                    ***
--- ************************************************
---

CREATE VIEW exemplar AS
  SELECT
    feature_id AS exemplar_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'exemplar';

--- ************************************************
--- *** relation: frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence that  ***
--- *** contains a mutation involving the deleti ***
--- *** on or insertion of one or more bases, wh ***
--- *** ere this number is not divisible by 3.   ***
--- ************************************************
---

CREATE VIEW frameshift AS
  SELECT
    feature_id AS frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minus_1_frameshift' OR cvterm.name = 'minus_2_frameshift' OR cvterm.name = 'plus_1_frameshift' OR cvterm.name = 'plus_2_framshift' OR cvterm.name = 'frameshift';

--- ************************************************
--- *** relation: minus_1_frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A frameshift caused by deleting one base ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW minus_1_frameshift AS
  SELECT
    feature_id AS minus_1_frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minus_1_frameshift';

--- ************************************************
--- *** relation: minus_2_frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A frameshift caused by deleting two base ***
--- *** s.                                       ***
--- ************************************************
---

CREATE VIEW minus_2_frameshift AS
  SELECT
    feature_id AS minus_2_frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minus_2_frameshift';

--- ************************************************
--- *** relation: plus_1_frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A frameshift caused by inserting one bas ***
--- *** e.                                       ***
--- ************************************************
---

CREATE VIEW plus_1_frameshift AS
  SELECT
    feature_id AS plus_1_frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'plus_1_frameshift';

--- ************************************************
--- *** relation: plus_2_framshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A frameshift caused by inserting two bas ***
--- *** es.                                      ***
--- ************************************************
---

CREATE VIEW plus_2_framshift AS
  SELECT
    feature_id AS plus_2_framshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'plus_2_framshift';

--- ************************************************
--- *** relation: trans_spliced ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing transcript seque ***
--- *** nce that is created by splicing exons fr ***
--- *** om diferent genes.                       ***
--- ************************************************
---

CREATE VIEW trans_spliced AS
  SELECT
    feature_id AS trans_spliced_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'trans_spliced';

--- ************************************************
--- *** relation: polyadenylated_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An mRNA that is polyadenylated.          ***
--- ************************************************
---

CREATE VIEW polyadenylated_mrna AS
  SELECT
    feature_id AS polyadenylated_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polyadenylated_mRNA';

--- ************************************************
--- *** relation: trans_spliced_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An mRNA that is trans-spliced.           ***
--- ************************************************
---

CREATE VIEW trans_spliced_mrna AS
  SELECT
    feature_id AS trans_spliced_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'trans_spliced_mRNA';

--- ************************************************
--- *** relation: edited_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript that is edited.             ***
--- ************************************************
---

CREATE VIEW edited_transcript AS
  SELECT
    feature_id AS edited_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'edited_transcript_by_A_to_I_substitution' OR cvterm.name = 'edited_mRNA' OR cvterm.name = 'edited_transcript';

--- ************************************************
--- *** relation: edited_transcript_by_a_to_i_substitution ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript that has been edited by A t ***
--- *** o I substitution.                        ***
--- ************************************************
---

CREATE VIEW edited_transcript_by_a_to_i_substitution AS
  SELECT
    feature_id AS edited_transcript_by_a_to_i_substitution_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'edited_transcript_by_A_to_I_substitution';

--- ************************************************
--- *** relation: bound_by_protein ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence that  ***
--- *** is bound by a protein.                   ***
--- ************************************************
---

CREATE VIEW bound_by_protein AS
  SELECT
    feature_id AS bound_by_protein_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'bound_by_protein';

--- ************************************************
--- *** relation: bound_by_nucleic_acid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence that  ***
--- *** is bound by a nucleic acid.              ***
--- ************************************************
---

CREATE VIEW bound_by_nucleic_acid AS
  SELECT
    feature_id AS bound_by_nucleic_acid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'bound_by_nucleic_acid';

--- ************************************************
--- *** relation: alternatively_spliced ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a situation wher ***
--- *** e a gene may encode for more than 1 tran ***
--- *** script.                                  ***
--- ************************************************
---

CREATE VIEW alternatively_spliced AS
  SELECT
    feature_id AS alternatively_spliced_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'alternatively_spliced';

--- ************************************************
--- *** relation: monocistronic ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence that  ***
--- *** contains the code for one gene product.  ***
--- ************************************************
---

CREATE VIEW monocistronic AS
  SELECT
    feature_id AS monocistronic_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'monocistronic';

--- ************************************************
--- *** relation: dicistronic ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence that  ***
--- *** contains the code for two gene products. ***
--- ************************************************
---

CREATE VIEW dicistronic AS
  SELECT
    feature_id AS dicistronic_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dicistronic';

--- ************************************************
--- *** relation: polycistronic ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence that  ***
--- *** contains the code for more than one gene ***
--- ***  product.                                ***
--- ************************************************
---

CREATE VIEW polycistronic AS
  SELECT
    feature_id AS polycistronic_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dicistronic' OR cvterm.name = 'polycistronic';

--- ************************************************
--- *** relation: recoded ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing an mRNA sequence ***
--- ***  that has been reprogrammed at translati ***
--- *** on, causing localized alterations.       ***
--- ************************************************
---

CREATE VIEW recoded AS
  SELECT
    feature_id AS recoded_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'codon_redefined' OR cvterm.name = 'recoded_by_translational_bypass' OR cvterm.name = 'translationally_frameshifted' OR cvterm.name = 'minus_1_translationally_frameshifted' OR cvterm.name = 'plus_1_translationally_frameshifted' OR cvterm.name = 'recoded';

--- ************************************************
--- *** relation: codon_redefined ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing the alteration o ***
--- *** f codon meaning.                         ***
--- ************************************************
---

CREATE VIEW codon_redefined AS
  SELECT
    feature_id AS codon_redefined_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'codon_redefined';

--- ************************************************
--- *** relation: stop_codon_read_through ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A stop codon redefined to be a new amino ***
--- ***  acid.                                   ***
--- ************************************************
---

CREATE VIEW stop_codon_read_through AS
  SELECT
    feature_id AS stop_codon_read_through_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stop_codon_redefined_as_pyrrolysine' OR cvterm.name = 'stop_codon_redefined_as_selenocysteine' OR cvterm.name = 'stop_codon_read_through';

--- ************************************************
--- *** relation: stop_codon_redefined_as_pyrrolysine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A stop codon redefined to be the new ami ***
--- *** no acid, pyrrolysine.                    ***
--- ************************************************
---

CREATE VIEW stop_codon_redefined_as_pyrrolysine AS
  SELECT
    feature_id AS stop_codon_redefined_as_pyrrolysine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stop_codon_redefined_as_pyrrolysine';

--- ************************************************
--- *** relation: stop_codon_redefined_as_selenocysteine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A stop codon redefined to be the new ami ***
--- *** no acid, selenocysteine.                 ***

SET search_path=so,chado,pg_catalog;
--- *** endogenous transcript of a miRNA gene. M ***
--- *** icro RNAs are produced from precursor mo ***
--- *** lecules (SO:0000647) that can form local ***
--- ***  hairpin structures, which ordinarily ar ***
--- *** e processed (via the Dicer pathway) such ***
--- ***  that a single miRNA molecule accumulate ***
--- *** s from one arm of a hairpin precursor mo ***
--- *** lecule. Micro RNAs may trigger the cleav ***
--- *** age of their target molecules or act as  ***
--- *** translational repressors.                ***
--- ************************************************
---

CREATE VIEW mirna AS
  SELECT
    feature_id AS mirna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'miRNA';

--- ************************************************
--- *** relation: bound_by_factor ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence that  ***
--- *** is bound by another molecule.            ***
--- ************************************************
---

CREATE VIEW bound_by_factor AS
  SELECT
    feature_id AS bound_by_factor_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'bound_by_protein' OR cvterm.name = 'bound_by_nucleic_acid' OR cvterm.name = 'bound_by_factor';

--- ************************************************
--- *** relation: transcript_bound_by_nucleic_acid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript that is bound by a nucleic  ***
--- *** acid.                                    ***
--- ************************************************
---

CREATE VIEW transcript_bound_by_nucleic_acid AS
  SELECT
    feature_id AS transcript_bound_by_nucleic_acid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transcript_bound_by_nucleic_acid';

--- ************************************************
--- *** relation: transcript_bound_by_protein ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript that is bound by a protein. ***
--- ************************************************
---

CREATE VIEW transcript_bound_by_protein AS
  SELECT
    feature_id AS transcript_bound_by_protein_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transcript_bound_by_protein';

--- ************************************************
--- *** relation: engineered_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is engineered.               ***
--- ************************************************
---

CREATE VIEW engineered_gene AS
  SELECT
    feature_id AS engineered_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_foreign_gene' OR cvterm.name = 'engineered_fusion_gene' OR cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'engineered_gene';

--- ************************************************
--- *** relation: engineered_foreign_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is engineered and foreign.   ***
--- ************************************************
---

CREATE VIEW engineered_foreign_gene AS
  SELECT
    feature_id AS engineered_foreign_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'engineered_foreign_gene';

--- ************************************************
--- *** relation: mrna_with_minus_1_frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An mRNA with a minus 1 frameshift.       ***
--- ************************************************
---

CREATE VIEW mrna_with_minus_1_frameshift AS
  SELECT
    feature_id AS mrna_with_minus_1_frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mRNA_with_minus_1_frameshift';

--- ************************************************
--- *** relation: engineered_foreign_transposable_element_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transposable_element that is engineere ***
--- *** d and foreign.                           ***
--- ************************************************
---

CREATE VIEW engineered_foreign_transposable_element_gene AS
  SELECT
    feature_id AS engineered_foreign_transposable_element_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_foreign_transposable_element_gene';

--- ************************************************
--- *** relation: foreign_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is foreign.                  ***
--- ************************************************
---

CREATE VIEW foreign_gene AS
  SELECT
    feature_id AS foreign_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_foreign_gene' OR cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'foreign_gene';

--- ************************************************
--- *** relation: long_terminal_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence directly repeated at both end ***
--- *** s of a defined sequence, of the sort typ ***
--- *** ically found in retroviruses.            ***
--- ************************************************
---

CREATE VIEW long_terminal_repeat AS
  SELECT
    feature_id AS long_terminal_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_LTR' OR cvterm.name = 'three_prime_LTR' OR cvterm.name = 'solo_LTR' OR cvterm.name = 'long_terminal_repeat';

--- ************************************************
--- *** relation: fusion_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is a fusion.                 ***
--- ************************************************
---

CREATE VIEW fusion_gene AS
  SELECT
    feature_id AS fusion_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_fusion_gene' OR cvterm.name = 'fusion_gene';

--- ************************************************
--- *** relation: engineered_fusion_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A fusion gene that is engineered.        ***
--- ************************************************
---

CREATE VIEW engineered_fusion_gene AS
  SELECT
    feature_id AS engineered_fusion_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_fusion_gene';

--- ************************************************
--- *** relation: microsatellite ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A repeat_region containing repeat_units  ***
--- *** (2 to 4 bp) that is repeated multiple ti ***
--- *** mes in tandem.                           ***
--- ************************************************
---

CREATE VIEW microsatellite AS
  SELECT
    feature_id AS microsatellite_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dinucleotide_repeat_microsatellite_feature' OR cvterm.name = 'trinucleotide_repeat_microsatellite_feature' OR cvterm.name = 'tetranucleotide_repeat_microsatellite_feature' OR cvterm.name = 'microsatellite';

--- ************************************************
--- *** relation: dinucleotide_repeat_microsatellite_feature ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW dinucleotide_repeat_microsatellite_feature AS
  SELECT
    feature_id AS dinucleotide_repeat_microsatellite_feature_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dinucleotide_repeat_microsatellite_feature';

--- ************************************************
--- *** relation: trinuc_repeat_microsat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW trinuc_repeat_microsat AS
  SELECT
    feature_id AS trinuc_repeat_microsat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'trinucleotide_repeat_microsatellite_feature';

--- ************************************************
--- *** relation: engineered_foreign_repetitive_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A repetitive element that is engineered  ***
--- *** and foreign.                             ***
--- ************************************************
---

CREATE VIEW engineered_foreign_repetitive_element AS
  SELECT
    feature_id AS engineered_foreign_repetitive_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_foreign_repetitive_element';

--- ************************************************
--- *** relation: inverted_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The sequence is complementarily repeated ***
--- ***  on the opposite strand. It is a palindr ***
--- *** ome, and it may, or may not be hyphenate ***
--- *** d. Examples: GCTGATCAGC, or GCTGA-----TC ***
--- *** AGC.                                     ***
--- ************************************************
---

CREATE VIEW inverted_repeat AS
  SELECT
    feature_id AS inverted_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'terminal_inverted_repeat' OR cvterm.name = 'five_prime_terminal_inverted_repeat' OR cvterm.name = 'three_prime_terminal_inverted_repeat' OR cvterm.name = 'inverted_repeat';

--- ************************************************
--- *** relation: u12_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A type of spliceosomal intron spliced by ***
--- ***  the U12 spliceosome, that includes U11, ***
--- ***  U12, U4atac/U6atac and U5 snRNAs.       ***
--- ************************************************
---

CREATE VIEW u12_intron AS
  SELECT
    feature_id AS u12_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U12_intron';

--- ************************************************
--- *** relation: origin_of_replication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The origin of replication; starting site ***
--- ***  for duplication of a nucleic acid molec ***
--- *** ule to give two identical copies.        ***
--- ************************************************
---

CREATE VIEW origin_of_replication AS
  SELECT
    feature_id AS origin_of_replication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'D_loop' OR cvterm.name = 'ARS' OR cvterm.name = 'oriT' OR cvterm.name = 'amplification_origin' OR cvterm.name = 'oriV' OR cvterm.name = 'oriC' OR cvterm.name = 'origin_of_replication';

--- ************************************************
--- *** relation: d_loop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Displacement loop; a region within mitoc ***
--- *** hondrial DNA in which a short stretch of ***
--- ***  RNA is paired with one strand of DNA, d ***
--- *** isplacing the original partner DNA stran ***
--- *** d in this region; also used to describe  ***
--- *** the displacement of a region of one stra ***
--- *** nd of duplex DNA by a single stranded in ***
--- *** vader in the reaction catalyzed by RecA  ***
--- *** protein.                                 ***
--- ************************************************
---

CREATE VIEW d_loop AS
  SELECT
    feature_id AS d_loop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'D_loop';

--- ************************************************
--- *** relation: recombination_feature ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW recombination_feature AS
  SELECT
    feature_id AS recombination_feature_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'recombination_hotspot' OR cvterm.name = 'haplotype_block' OR cvterm.name = 'sequence_rearrangement_feature' OR cvterm.name = 'iDNA' OR cvterm.name = 'specific_recombination_site' OR cvterm.name = 'chromosome_breakage_sequence' OR cvterm.name = 'internal_eliminated_sequence' OR cvterm.name = 'macronucleus_destined_segment' OR cvterm.name = 'recombination_feature_of_rearranged_gene' OR cvterm.name = 'site_specific_recombination_target_region' OR cvterm.name = 'recombination_signal_sequence' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_feature' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_segment' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_gene_cluster' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_spacer' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_rearranged_segment' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_rearranged_gene_cluster' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_signal_feature' OR cvterm.name = 'D_gene' OR cvterm.name = 'V_gene' OR cvterm.name = 'J_gene' OR cvterm.name = 'C_gene' OR cvterm.name = 'D_J_C_cluster' OR cvterm.name = 'J_C_cluster' OR cvterm.name = 'J_cluster' OR cvterm.name = 'V_cluster' OR cvterm.name = 'V_J_cluster' OR cvterm.name = 'V_J_C_cluster' OR cvterm.name = 'C_cluster' OR cvterm.name = 'D_cluster' OR cvterm.name = 'D_J_cluster' OR cvterm.name = 'three_prime_D_spacer' OR cvterm.name = 'five_prime_D_spacer' OR cvterm.name = 'J_spacer' OR cvterm.name = 'V_spacer' OR cvterm.name = 'VD_gene' OR cvterm.name = 'DJ_gene' OR cvterm.name = 'VDJ_gene' OR cvterm.name = 'VJ_gene' OR cvterm.name = 'DJ_J_cluster' OR cvterm.name = 'VDJ_J_C_cluster' OR cvterm.name = 'VDJ_J_cluster' OR cvterm.name = 'VJ_C_cluster' OR cvterm.name = 'VJ_J_C_cluster' OR cvterm.name = 'VJ_J_cluster' OR cvterm.name = 'D_DJ_C_cluster' OR cvterm.name = 'D_DJ_cluster' OR cvterm.name = 'D_DJ_J_C_cluster' OR cvterm.name = 'D_DJ_J_cluster' OR cvterm.name = 'V_DJ_cluster' OR cvterm.name = 'V_DJ_J_cluster' OR cvterm.name = 'V_VDJ_C_cluster' OR cvterm.name = 'V_VDJ_cluster' OR cvterm.name = 'V_VDJ_J_cluster' OR cvterm.name = 'V_VJ_C_cluster' OR cvterm.name = 'V_VJ_cluster' OR cvterm.name = 'V_VJ_J_cluster' OR cvterm.name = 'V_D_DJ_C_cluster' OR cvterm.name = 'V_D_DJ_cluster' OR cvterm.name = 'V_D_DJ_J_C_cluster' OR cvterm.name = 'V_D_DJ_J_cluster' OR cvterm.name = 'V_D_J_C_cluster' OR cvterm.name = 'V_D_J_cluster' OR cvterm.name = 'DJ_C_cluster' OR cvterm.name = 'DJ_J_C_cluster' OR cvterm.name = 'VDJ_C_cluster' OR cvterm.name = 'V_DJ_C_cluster' OR cvterm.name = 'V_DJ_J_C_cluster' OR cvterm.name = 'V_VDJ_J_C_cluster' OR cvterm.name = 'V_VJ_J_C_cluster' OR cvterm.name = 'J_gene_recombination_feature' OR cvterm.name = 'D_gene_recombination_feature' OR cvterm.name = 'V_gene_recombination_feature' OR cvterm.name = 'heptamer_of_recombination_feature_of_vertebrate_immune_system_gene' OR cvterm.name = 'nonamer_of_recombination_feature_of_vertebrate_immune_system_gene' OR cvterm.name = 'five_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_heptamer' OR cvterm.name = 'five_prime_D_heptamer' OR cvterm.name = 'J_heptamer' OR cvterm.name = 'V_heptamer' OR cvterm.name = 'three_prime_D_nonamer' OR cvterm.name = 'five_prime_D_nonamer' OR cvterm.name = 'J_nonamer' OR cvterm.name = 'V_nonamer' OR cvterm.name = 'integration_excision_site' OR cvterm.name = 'resolution_site' OR cvterm.name = 'inversion_site' OR cvterm.name = 'inversion_site_part' OR cvterm.name = 'attI_site' OR cvterm.name = 'attP_site' OR cvterm.name = 'attB_site' OR cvterm.name = 'attL_site' OR cvterm.name = 'attR_site' OR cvterm.name = 'attC_site' OR cvterm.name = 'attCtn_site' OR cvterm.name = 'loxP_site' OR cvterm.name = 'dif_site' OR cvterm.name = 'FRT_site' OR cvterm.name = 'IRLinv_site' OR cvterm.name = 'IRRinv_site' OR cvterm.name = 'recombination_feature';

--- ************************************************
--- *** relation: specific_recombination_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW specific_recombination_site AS
  SELECT
    feature_id AS specific_recombination_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'recombination_feature_of_rearranged_gene' OR cvterm.name = 'site_specific_recombination_target_region' OR cvterm.name = 'recombination_signal_sequence' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_feature' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_segment' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_gene_cluster' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_spacer' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_rearranged_segment' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_rearranged_gene_cluster' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_signal_feature' OR cvterm.name = 'D_gene' OR cvterm.name = 'V_gene' OR cvterm.name = 'J_gene' OR cvterm.name = 'C_gene' OR cvterm.name = 'D_J_C_cluster' OR cvterm.name = 'J_C_cluster' OR cvterm.name = 'J_cluster' OR cvterm.name = 'V_cluster' OR cvterm.name = 'V_J_cluster' OR cvterm.name = 'V_J_C_cluster' OR cvterm.name = 'C_cluster' OR cvterm.name = 'D_cluster' OR cvterm.name = 'D_J_cluster' OR cvterm.name = 'three_prime_D_spacer' OR cvterm.name = 'five_prime_D_spacer' OR cvterm.name = 'J_spacer' OR cvterm.name = 'V_spacer' OR cvterm.name = 'VD_gene' OR cvterm.name = 'DJ_gene' OR cvterm.name = 'VDJ_gene' OR cvterm.name = 'VJ_gene' OR cvterm.name = 'DJ_J_cluster' OR cvterm.name = 'VDJ_J_C_cluster' OR cvterm.name = 'VDJ_J_cluster' OR cvterm.name = 'VJ_C_cluster' OR cvterm.name = 'VJ_J_C_cluster' OR cvterm.name = 'VJ_J_cluster' OR cvterm.name = 'D_DJ_C_cluster' OR cvterm.name = 'D_DJ_cluster' OR cvterm.name = 'D_DJ_J_C_cluster' OR cvterm.name = 'D_DJ_J_cluster' OR cvterm.name = 'V_DJ_cluster' OR cvterm.name = 'V_DJ_J_cluster' OR cvterm.name = 'V_VDJ_C_cluster' OR cvterm.name = 'V_VDJ_cluster' OR cvterm.name = 'V_VDJ_J_cluster' OR cvterm.name = 'V_VJ_C_cluster' OR cvterm.name = 'V_VJ_cluster' OR cvterm.name = 'V_VJ_J_cluster' OR cvterm.name = 'V_D_DJ_C_cluster' OR cvterm.name = 'V_D_DJ_cluster' OR cvterm.name = 'V_D_DJ_J_C_cluster' OR cvterm.name = 'V_D_DJ_J_cluster' OR cvterm.name = 'V_D_J_C_cluster' OR cvterm.name = 'V_D_J_cluster' OR cvterm.name = 'DJ_C_cluster' OR cvterm.name = 'DJ_J_C_cluster' OR cvterm.name = 'VDJ_C_cluster' OR cvterm.name = 'V_DJ_C_cluster' OR cvterm.name = 'V_DJ_J_C_cluster' OR cvterm.name = 'V_VDJ_J_C_cluster' OR cvterm.name = 'V_VJ_J_C_cluster' OR cvterm.name = 'J_gene_recombination_feature' OR cvterm.name = 'D_gene_recombination_feature' OR cvterm.name = 'V_gene_recombination_feature' OR cvterm.name = 'heptamer_of_recombination_feature_of_vertebrate_immune_system_gene' OR cvterm.name = 'nonamer_of_recombination_feature_of_vertebrate_immune_system_gene' OR cvterm.name = 'five_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_heptamer' OR cvterm.name = 'five_prime_D_heptamer' OR cvterm.name = 'J_heptamer' OR cvterm.name = 'V_heptamer' OR cvterm.name = 'three_prime_D_nonamer' OR cvterm.name = 'five_prime_D_nonamer' OR cvterm.name = 'J_nonamer' OR cvterm.name = 'V_nonamer' OR cvterm.name = 'integration_excision_site' OR cvterm.name = 'resolution_site' OR cvterm.name = 'inversion_site' OR cvterm.name = 'inversion_site_part' OR cvterm.name = 'attI_site' OR cvterm.name = 'attP_site' OR cvterm.name = 'attB_site' OR cvterm.name = 'attL_site' OR cvterm.name = 'attR_site' OR cvterm.name = 'attC_site' OR cvterm.name = 'attCtn_site' OR cvterm.name = 'loxP_site' OR cvterm.name = 'dif_site' OR cvterm.name = 'FRT_site' OR cvterm.name = 'IRLinv_site' OR cvterm.name = 'IRRinv_site' OR cvterm.name = 'specific_recombination_site';

--- ************************************************
--- *** relation: recombination_feature_of_rearranged_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW recombination_feature_of_rearranged_gene AS
  SELECT
    feature_id AS recombination_feature_of_rearranged_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'vertebrate_immune_system_gene_recombination_feature' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_segment' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_gene_cluster' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_spacer' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_rearranged_segment' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_rearranged_gene_cluster' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_signal_feature' OR cvterm.name = 'D_gene' OR cvterm.name = 'V_gene' OR cvterm.name = 'J_gene' OR cvterm.name = 'C_gene' OR cvterm.name = 'D_J_C_cluster' OR cvterm.name = 'J_C_cluster' OR cvterm.name = 'J_cluster' OR cvterm.name = 'V_cluster' OR cvterm.name = 'V_J_cluster' OR cvterm.name = 'V_J_C_cluster' OR cvterm.name = 'C_cluster' OR cvterm.name = 'D_cluster' OR cvterm.name = 'D_J_cluster' OR cvterm.name = 'three_prime_D_spacer' OR cvterm.name = 'five_prime_D_spacer' OR cvterm.name = 'J_spacer' OR cvterm.name = 'V_spacer' OR cvterm.name = 'VD_gene' OR cvterm.name = 'DJ_gene' OR cvterm.name = 'VDJ_gene' OR cvterm.name = 'VJ_gene' OR cvterm.name = 'DJ_J_cluster' OR cvterm.name = 'VDJ_J_C_cluster' OR cvterm.name = 'VDJ_J_cluster' OR cvterm.name = 'VJ_C_cluster' OR cvterm.name = 'VJ_J_C_cluster' OR cvterm.name = 'VJ_J_cluster' OR cvterm.name = 'D_DJ_C_cluster' OR cvterm.name = 'D_DJ_cluster' OR cvterm.name = 'D_DJ_J_C_cluster' OR cvterm.name = 'D_DJ_J_cluster' OR cvterm.name = 'V_DJ_cluster' OR cvterm.name = 'V_DJ_J_cluster' OR cvterm.name = 'V_VDJ_C_cluster' OR cvterm.name = 'V_VDJ_cluster' OR cvterm.name = 'V_VDJ_J_cluster' OR cvterm.name = 'V_VJ_C_cluster' OR cvterm.name = 'V_VJ_cluster' OR cvterm.name = 'V_VJ_J_cluster' OR cvterm.name = 'V_D_DJ_C_cluster' OR cvterm.name = 'V_D_DJ_cluster' OR cvterm.name = 'V_D_DJ_J_C_cluster' OR cvterm.name = 'V_D_DJ_J_cluster' OR cvterm.name = 'V_D_J_C_cluster' OR cvterm.name = 'V_D_J_cluster' OR cvterm.name = 'DJ_C_cluster' OR cvterm.name = 'DJ_J_C_cluster' OR cvterm.name = 'VDJ_C_cluster' OR cvterm.name = 'V_DJ_C_cluster' OR cvterm.name = 'V_DJ_J_C_cluster' OR cvterm.name = 'V_VDJ_J_C_cluster' OR cvterm.name = 'V_VJ_J_C_cluster' OR cvterm.name = 'J_gene_recombination_feature' OR cvterm.name = 'D_gene_recombination_feature' OR cvterm.name = 'V_gene_recombination_feature' OR cvterm.name = 'heptamer_of_recombination_feature_of_vertebrate_immune_system_gene' OR cvterm.name = 'nonamer_of_recombination_feature_of_vertebrate_immune_system_gene' OR cvterm.name = 'five_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_heptamer' OR cvterm.name = 'five_prime_D_heptamer' OR cvterm.name = 'J_heptamer' OR cvterm.name = 'V_heptamer' OR cvterm.name = 'three_prime_D_nonamer' OR cvterm.name = 'five_prime_D_nonamer' OR cvterm.name = 'J_nonamer' OR cvterm.name = 'V_nonamer' OR cvterm.name = 'recombination_feature_of_rearranged_gene';

--- ************************************************
--- *** relation: vertebrate_immune_system_gene_recombination_feature ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW vertebrate_immune_system_gene_recombination_feature AS
  SELECT
    feature_id AS vertebrate_immune_system_gene_recombination_feature_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_segment' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_gene_cluster' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_spacer' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_rearranged_segment' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_rearranged_gene_cluster' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_signal_feature' OR cvterm.name = 'D_gene' OR cvterm.name = 'V_gene' OR cvterm.name = 'J_gene' OR cvterm.name = 'C_gene' OR cvterm.name = 'D_J_C_cluster' OR cvterm.name = 'J_C_cluster' OR cvterm.name = 'J_cluster' OR cvterm.name = 'V_cluster' OR cvterm.name = 'V_J_cluster' OR cvterm.name = 'V_J_C_cluster' OR cvterm.name = 'C_cluster' OR cvterm.name = 'D_cluster' OR cvterm.name = 'D_J_cluster' OR cvterm.name = 'three_prime_D_spacer' OR cvterm.name = 'five_prime_D_spacer' OR cvterm.name = 'J_spacer' OR cvterm.name = 'V_spacer' OR cvterm.name = 'VD_gene' OR cvterm.name = 'DJ_gene' OR cvterm.name = 'VDJ_gene' OR cvterm.name = 'VJ_gene' OR cvterm.name = 'DJ_J_cluster' OR cvterm.name = 'VDJ_J_C_cluster' OR cvterm.name = 'VDJ_J_cluster' OR cvterm.name = 'VJ_C_cluster' OR cvterm.name = 'VJ_J_C_cluster' OR cvterm.name = 'VJ_J_cluster' OR cvterm.name = 'D_DJ_C_cluster' OR cvterm.name = 'D_DJ_cluster' OR cvterm.name = 'D_DJ_J_C_cluster' OR cvterm.name = 'D_DJ_J_cluster' OR cvterm.name = 'V_DJ_cluster' OR cvterm.name = 'V_DJ_J_cluster' OR cvterm.name = 'V_VDJ_C_cluster' OR cvterm.name = 'V_VDJ_cluster' OR cvterm.name = 'V_VDJ_J_cluster' OR cvterm.name = 'V_VJ_C_cluster' OR cvterm.name = 'V_VJ_cluster' OR cvterm.name = 'V_VJ_J_cluster' OR cvterm.name = 'V_D_DJ_C_cluster' OR cvterm.name = 'V_D_DJ_cluster' OR cvterm.name = 'V_D_DJ_J_C_cluster' OR cvterm.name = 'V_D_DJ_J_cluster' OR cvterm.name = 'V_D_J_C_cluster' OR cvterm.name = 'V_D_J_cluster' OR cvterm.name = 'DJ_C_cluster' OR cvterm.name = 'DJ_J_C_cluster' OR cvterm.name = 'VDJ_C_cluster' OR cvterm.name = 'V_DJ_C_cluster' OR cvterm.name = 'V_DJ_J_C_cluster' OR cvterm.name = 'V_VDJ_J_C_cluster' OR cvterm.name = 'V_VJ_J_C_cluster' OR cvterm.name = 'J_gene_recombination_feature' OR cvterm.name = 'D_gene_recombination_feature' OR cvterm.name = 'V_gene_recombination_feature' OR cvterm.name = 'heptamer_of_recombination_feature_of_vertebrate_immune_system_gene' OR cvterm.name = 'nonamer_of_recombination_feature_of_vertebrate_immune_system_gene' OR cvterm.name = 'five_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_heptamer' OR cvterm.name = 'five_prime_D_heptamer' OR cvterm.name = 'J_heptamer' OR cvterm.name = 'V_heptamer' OR cvterm.name = 'three_prime_D_nonamer' OR cvterm.name = 'five_prime_D_nonamer' OR cvterm.name = 'J_nonamer' OR cvterm.name = 'V_nonamer' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_feature';

--- ************************************************
--- *** relation: j_gene_recombination_feature ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Recombination signal including J-heptame ***
--- *** r, J-spacer and J-nonamer in 5' of J-reg ***
--- *** ion of a J-gene or J-sequence.           ***
--- ************************************************
---

CREATE VIEW j_gene_recombination_feature AS
  SELECT
    feature_id AS j_gene_recombination_feature_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'J_gene_recombination_feature';

--- ************************************************
--- *** relation: clip ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Part of the primary transcript that is c ***
--- *** lipped off during processing.            ***
--- ************************************************
---

CREATE VIEW clip AS
  SELECT
    feature_id AS clip_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_clip' OR cvterm.name = 'three_prime_clip' OR cvterm.name = 'clip';

--- ************************************************
--- *** relation: modified_base ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A modified nucleotide, i.e. a nucleotide ***
--- ***  other than A, T, C. G.                  ***
--- ************************************************
---

CREATE VIEW modified_base AS
  SELECT
    feature_id AS modified_base_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'methylated_base_feature' OR cvterm.name = 'methylated_C' OR cvterm.name = 'methylated_A' OR cvterm.name = 'modified_base';

--- ************************************************
--- *** relation: methylated_base_feature ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A nucleotide modified by methylation.    ***
--- ************************************************
---

CREATE VIEW methylated_base_feature AS
  SELECT
    feature_id AS methylated_base_feature_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'methylated_C' OR cvterm.name = 'methylated_A' OR cvterm.name = 'methylated_base_feature';

--- ************************************************
--- *** relation: cpg_island ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Regions of a few hundred to a few thousa ***
--- *** nd bases in vertebrate genomes that are  ***
--- *** relatively GC and CpG rich; they are typ ***
--- *** ically unmethylated and often found near ***
--- ***  the 5' ends of genes.                   ***
--- ************************************************
---

CREATE VIEW cpg_island AS
  SELECT
    feature_id AS cpg_island_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'CpG_island';

--- ************************************************
--- *** relation: experimentally_determined ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Attribute to describe a feature that has ***
--- ***  been experimentally verified.           ***
--- ************************************************
---

CREATE VIEW experimentally_determined AS
  SELECT
    feature_id AS experimentally_determined_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'experimentally_determined';

--- ************************************************
--- *** relation: stem_loop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A double-helical region of nucleic acid  ***
--- *** formed by base-pairing between adjacent  ***
--- *** (inverted) complementary sequences.      ***
--- ************************************************
---

CREATE VIEW stem_loop AS
  SELECT
    feature_id AS stem_loop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tetraloop' OR cvterm.name = 'stem_loop';

--- ************************************************
--- *** relation: direct_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A repeat where the same sequence is repe ***
--- *** ated in the same direction. Example: GCT ***
--- *** GA-----GCTGA.                            ***
--- ************************************************
---

CREATE VIEW direct_repeat AS
  SELECT
    feature_id AS direct_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'target_site_duplication' OR cvterm.name = 'CRISPR' OR cvterm.name = 'direct_repeat';

--- ************************************************
--- *** relation: tss ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The first base where RNA polymerase begi ***
--- *** ns to synthesize the RNA transcript.     ***
--- ************************************************
---

CREATE VIEW tss AS
  SELECT
    feature_id AS tss_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'major_TSS' OR cvterm.name = 'minor_TSS' OR cvterm.name = 'TSS';

--- ************************************************
--- *** relation: cds ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A contiguous sequence which begins with, ***
--- ***  and includes, a start codon and ends wi ***
--- *** th, and includes, a stop codon.          ***
--- ************************************************
---

CREATE VIEW cds AS
  SELECT
    feature_id AS cds_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'edited_CDS' OR cvterm.name = 'CDS_fragment' OR cvterm.name = 'CDS_independently_known' OR cvterm.name = 'CDS_predicted' OR cvterm.name = 'orphan_CDS' OR cvterm.name = 'CDS_supported_by_sequence_similarity_data' OR cvterm.name = 'CDS_supported_by_domain_match_data' OR cvterm.name = 'CDS_supported_by_EST_or_cDNA_data' OR cvterm.name = 'CDS';

--- ************************************************
--- *** relation: cdna_clone ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Complementary DNA; A piece of DNA copied ***
--- ***  from an mRNA and spliced into a vector  ***
--- *** for propagation in a suitable host.      ***
--- ************************************************
---

CREATE VIEW cdna_clone AS
  SELECT
    feature_id AS cdna_clone_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'validated_cDNA_clone' OR cvterm.name = 'invalidated_cDNA_clone' OR cvterm.name = 'three_prime_RACE_clone' OR cvterm.name = 'chimeric_cDNA_clone' OR cvterm.name = 'genomically_contaminated_cDNA_clone' OR cvterm.name = 'polyA_primed_cDNA_clone' OR cvterm.name = 'partially_processed_cDNA_clone' OR cvterm.name = 'cDNA_clone';

--- ************************************************
--- *** relation: start_codon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** First codon to be translated by a riboso ***
--- *** me.                                      ***
--- ************************************************
---

CREATE VIEW start_codon AS
  SELECT
    feature_id AS start_codon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'non_canonical_start_codon' OR cvterm.name = 'four_bp_start_codon' OR cvterm.name = 'CTG_start_codon' OR cvterm.name = 'start_codon';

--- ************************************************
--- *** relation: stop_codon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** In mRNA, a set of three nucleotides that ***
--- ***  indicates the end of information for pr ***
--- *** otein synthesis.                         ***
--- ************************************************
---

CREATE VIEW stop_codon AS
  SELECT
    feature_id AS stop_codon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stop_codon';

--- ************************************************
--- *** relation: intronic_splice_enhancer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Sequences within the intron that modulat ***
--- *** e splice site selection for some introns ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW intronic_splice_enhancer AS
  SELECT
    feature_id AS intronic_splice_enhancer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'intronic_splice_enhancer';

--- ************************************************
--- *** relation: mrna_with_plus_1_frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An mRNA with a plus 1 frameshift.        ***
--- ************************************************
---

CREATE VIEW mrna_with_plus_1_frameshift AS
  SELECT
    feature_id AS mrna_with_plus_1_frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mRNA_with_plus_1_frameshift';

--- ************************************************
--- *** relation: nuclease_hypersensitive_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW nuclease_hypersensitive_site AS
  SELECT
    feature_id AS nuclease_hypersensitive_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DNAseI_hypersensitive_site' OR cvterm.name = 'nuclease_hypersensitive_site';

--- ************************************************
--- *** relation: coding_start ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The first base to be translated into pro ***
--- *** tein.                                    ***
--- ************************************************
---

CREATE VIEW coding_start AS
  SELECT
    feature_id AS coding_start_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'coding_start';

--- ************************************************
--- *** relation: tag ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A nucleotide sequence that may be used t ***
--- *** o identify a larger sequence.            ***
--- ************************************************
---

CREATE VIEW tag AS
  SELECT
    feature_id AS tag_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SAGE_tag' OR cvterm.name = 'STS' OR cvterm.name = 'EST' OR cvterm.name = 'engineered_tag' OR cvterm.name = 'five_prime_EST' OR cvterm.name = 'three_prime_EST' OR cvterm.name = 'UST' OR cvterm.name = 'RST' OR cvterm.name = 'three_prime_UST' OR cvterm.name = 'five_prime_UST' OR cvterm.name = 'three_prime_RST' OR cvterm.name = 'five_prime_RST' OR cvterm.name = 'tag';

--- ************************************************
--- *** relation: rrna_large_subunit_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding a large ri ***
--- *** bosomal subunit RNA.                     ***
--- ************************************************
---

CREATE VIEW rrna_large_subunit_primary_transcript AS
  SELECT
    feature_id AS rrna_large_subunit_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_large_subunit_primary_transcript';

--- ************************************************
--- *** relation: sage_tag ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A short diagnostic sequence tag, serial  ***
--- *** analysis of gene expression (SAGE), that ***
--- ***  allows the quantitative and simultaneou ***
--- *** s analysis of a large number of transcri ***
--- *** pts.                                     ***
--- ************************************************
---

CREATE VIEW sage_tag AS
  SELECT
    feature_id AS sage_tag_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SAGE_tag';

--- ************************************************
--- *** relation: coding_end ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The last base to be translated into prot ***
--- *** ein. It does not include the stop codon. ***
--- ************************************************
---

CREATE VIEW coding_end AS
  SELECT
    feature_id AS coding_end_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'coding_end';

--- ************************************************
--- *** relation: microarray_oligo ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW microarray_oligo AS
  SELECT
    feature_id AS microarray_oligo_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'microarray_oligo';

--- ************************************************
--- *** relation: mrna_with_plus_2_frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An mRNA with a plus 2 frameshift.        ***
--- ************************************************
---

CREATE VIEW mrna_with_plus_2_frameshift AS
  SELECT
    feature_id AS mrna_with_plus_2_frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mRNA_with_plus_2_frameshift';

--- ************************************************
--- *** relation: conserved_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Region of sequence similarity by descent ***
--- ***  from a common ancestor.                 ***
--- ************************************************
---

CREATE VIEW conserved_region AS
  SELECT
    feature_id AS conserved_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'coding_conserved_region' OR cvterm.name = 'nc_conserved_region' OR cvterm.name = 'RR_tract' OR cvterm.name = 'homologous_region' OR cvterm.name = 'centromere_DNA_Element_I' OR cvterm.name = 'centromere_DNA_Element_II' OR cvterm.name = 'centromere_DNA_Element_III' OR cvterm.name = 'X_element' OR cvterm.name = 'U_box' OR cvterm.name = 'regional_centromere_central_core' OR cvterm.name = 'syntenic_region' OR cvterm.name = 'paralogous_region' OR cvterm.name = 'orthologous_region' OR cvterm.name = 'conserved_region';

--- ************************************************
--- *** relation: sts ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Short (typically a few hundred base pair ***
--- *** s) DNA sequence that has a single occurr ***
--- *** ence in a genome and whose location and  ***
--- *** base sequence are known.                 ***
--- ************************************************
---

CREATE VIEW sts AS
  SELECT
    feature_id AS sts_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'STS';

--- ************************************************
--- *** relation: coding_conserved_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Coding region of sequence similarity by  ***
--- *** descent from a common ancestor.          ***
--- ************************************************
---

CREATE VIEW coding_conserved_region AS
  SELECT
    feature_id AS coding_conserved_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'coding_conserved_region';

--- ************************************************
--- *** relation: exon_junction ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The boundary between two exons in a proc ***
--- *** essed transcript.                        ***
--- ************************************************
---

CREATE VIEW exon_junction AS
  SELECT
    feature_id AS exon_junction_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'exon_junction';

--- ************************************************
--- *** relation: nc_conserved_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Non-coding region of sequence similarity ***
--- ***  by descent from a common ancestor.      ***
--- ************************************************
---

CREATE VIEW nc_conserved_region AS
  SELECT
    feature_id AS nc_conserved_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nc_conserved_region';

--- ************************************************
--- *** relation: mrna_with_minus_2_frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A mRNA with a minus 2 frameshift.        ***
--- ************************************************
---

CREATE VIEW mrna_with_minus_2_frameshift AS
  SELECT
    feature_id AS mrna_with_minus_2_frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mRNA_with_minus_2_frameshift';

--- ************************************************
--- *** relation: pseudogene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence that closely resembles a know ***
--- *** n functional gene, at another locus with ***
--- *** in a genome, that is non-functional as a ***
--- ***  consequence of (usually several) mutati ***
--- *** ons that prevent either its transcriptio ***
--- *** n or translation (or both). In general,  ***
--- *** pseudogenes result from either reverse t ***
--- *** ranscription of a transcript of their "n ***
--- *** ormal" paralog (SO:0000043) (in which ca ***
--- *** se the pseudogene typically lacks intron ***
--- *** s and includes a poly(A) tail) or from r ***
--- *** ecombination (SO:0000044) (in which case ***
--- ***  the pseudogene is typically a tandem du ***
--- *** plication of its "normal" paralog).      ***
--- ************************************************
---

CREATE VIEW pseudogene AS
  SELECT
    feature_id AS pseudogene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'processed_pseudogene' OR cvterm.name = 'non_processed_pseudogene' OR cvterm.name = 'pseudogene_by_unequal_crossing_over' OR cvterm.name = 'nuclear_mt_pseudogene' OR cvterm.name = 'cassette_pseudogene' OR cvterm.name = 'duplicated_pseudogene' OR cvterm.name = 'unitary_pseudogene' OR cvterm.name = 'pseudogene';

--- ************************************************
--- *** relation: rnai_reagent ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A double stranded RNA duplex, at least 2 ***
--- *** 0bp long, used experimentally to inhibit ***
--- ***  gene function by RNA interference.      ***
--- ************************************************
---

CREATE VIEW rnai_reagent AS
  SELECT
    feature_id AS rnai_reagent_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNAi_reagent';

--- ************************************************
--- *** relation: mite ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A highly repetitive and short (100-500 b ***
--- *** ase pair) transposable element with term ***
--- *** inal inverted repeats (TIR) and target s ***
--- *** ite duplication (TSD). MITEs do not enco ***
--- *** de proteins.                             ***
--- ************************************************
---

CREATE VIEW mite AS
  SELECT
    feature_id AS mite_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'MITE';

--- ************************************************
--- *** relation: recombination_hotspot ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region in a genome which promotes reco ***
--- *** mbination.                               ***
--- ************************************************
---

CREATE VIEW recombination_hotspot AS
  SELECT
    feature_id AS recombination_hotspot_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'recombination_hotspot';

--- ************************************************
--- *** relation: chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a nucleic ac ***
--- *** id molecule which controls its own repli ***
--- *** cation through the interaction of specif ***
--- *** ic proteins at one or more origins of re ***
--- *** plication.                               ***
--- ************************************************

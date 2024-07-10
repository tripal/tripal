SET search_path=so,chado,pg_catalog;
--- *** relation: promoter_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW promoter_element AS
  SELECT
    feature_id AS promoter_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'GC_rich_promoter_region' OR cvterm.name = 'DMv4_motif' OR cvterm.name = 'DMv5_motif' OR cvterm.name = 'DMv3_motif' OR cvterm.name = 'DMv2_motif' OR cvterm.name = 'DPE1_motif' OR cvterm.name = 'DMv1_motif' OR cvterm.name = 'NDM2_motif' OR cvterm.name = 'NDM3_motif' OR cvterm.name = 'core_promoter_element' OR cvterm.name = 'regulatory_promoter_element' OR cvterm.name = 'INR_motif' OR cvterm.name = 'DPE_motif' OR cvterm.name = 'BREu_motif' OR cvterm.name = 'TATA_box' OR cvterm.name = 'A_box' OR cvterm.name = 'B_box' OR cvterm.name = 'C_box' OR cvterm.name = 'MTE' OR cvterm.name = 'BREd_motif' OR cvterm.name = 'DCE' OR cvterm.name = 'intermediate_element' OR cvterm.name = 'RNA_polymerase_II_TATA_box' OR cvterm.name = 'RNA_polymerase_III_TATA_box' OR cvterm.name = 'A_box_type_1' OR cvterm.name = 'A_box_type_2' OR cvterm.name = 'proximal_promoter_element' OR cvterm.name = 'distal_promoter_element' OR cvterm.name = 'promoter_element';

--- ************************************************
--- *** relation: core_promoter_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW core_promoter_element AS
  SELECT
    feature_id AS core_promoter_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'INR_motif' OR cvterm.name = 'DPE_motif' OR cvterm.name = 'BREu_motif' OR cvterm.name = 'TATA_box' OR cvterm.name = 'A_box' OR cvterm.name = 'B_box' OR cvterm.name = 'C_box' OR cvterm.name = 'MTE' OR cvterm.name = 'BREd_motif' OR cvterm.name = 'DCE' OR cvterm.name = 'intermediate_element' OR cvterm.name = 'RNA_polymerase_II_TATA_box' OR cvterm.name = 'RNA_polymerase_III_TATA_box' OR cvterm.name = 'A_box_type_1' OR cvterm.name = 'A_box_type_2' OR cvterm.name = 'core_promoter_element';

--- ************************************************
--- *** relation: rna_polymerase_ii_tata_box ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A TATA box core promoter of a gene trans ***
--- *** cribed by RNA polymerase II.             ***
--- ************************************************
---

CREATE VIEW rna_polymerase_ii_tata_box AS
  SELECT
    feature_id AS rna_polymerase_ii_tata_box_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNA_polymerase_II_TATA_box';

--- ************************************************
--- *** relation: rna_polymerase_iii_tata_box ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A TATA box core promoter of a gene trans ***
--- *** cribed by RNA polymerase III.            ***
--- ************************************************
---

CREATE VIEW rna_polymerase_iii_tata_box AS
  SELECT
    feature_id AS rna_polymerase_iii_tata_box_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNA_polymerase_III_TATA_box';

--- ************************************************
--- *** relation: bred_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A core TRNA polymerase II promoter eleme ***
--- *** nt with consensus (G/A)T(T/G/A)(T/A)(G/T ***
--- *** )(T/G)(T/G).                             ***
--- ************************************************
---

CREATE VIEW bred_motif AS
  SELECT
    feature_id AS bred_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'BREd_motif';

--- ************************************************
--- *** relation: dce ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A discontinuous core element of RNA poly ***
--- *** merase II transcribed genes, situated do ***
--- *** wnstream of the TSS. It is composed of t ***
--- *** hree sub elements: SI, SII and SIII.     ***
--- ************************************************
---

CREATE VIEW dce AS
  SELECT
    feature_id AS dce_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DCE';

--- ************************************************
--- *** relation: dce_si ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sub element of the DCE core promoter e ***
--- *** lement, with consensus sequence CTTC.    ***
--- ************************************************
---

CREATE VIEW dce_si AS
  SELECT
    feature_id AS dce_si_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DCE_SI';

--- ************************************************
--- *** relation: dce_sii ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sub element of the DCE core promoter e ***
--- *** lement with consensus sequence CTGT.     ***
--- ************************************************
---

CREATE VIEW dce_sii AS
  SELECT
    feature_id AS dce_sii_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DCE_SII';

--- ************************************************
--- *** relation: dce_siii ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sub element of the DCE core promoter e ***
--- *** lement with consensus sequence AGC.      ***
--- ************************************************
---

CREATE VIEW dce_siii AS
  SELECT
    feature_id AS dce_siii_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DCE_SIII';

--- ************************************************
--- *** relation: proximal_promoter_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW proximal_promoter_element AS
  SELECT
    feature_id AS proximal_promoter_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'proximal_promoter_element';

--- ************************************************
--- *** relation: rnapol_ii_core_promoter ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The minimal portion of the promoter requ ***
--- *** ired to properly initiate transcription  ***
--- *** in RNA polymerase II transcribed genes.  ***
--- ************************************************
---

CREATE VIEW rnapol_ii_core_promoter AS
  SELECT
    feature_id AS rnapol_ii_core_promoter_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNApol_II_core_promoter';

--- ************************************************
--- *** relation: distal_promoter_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW distal_promoter_element AS
  SELECT
    feature_id AS distal_promoter_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'distal_promoter_element';

--- ************************************************
--- *** relation: bacterial_rnapol_promoter_sigma_70 ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW bacterial_rnapol_promoter_sigma_70 AS
  SELECT
    feature_id AS bacterial_rnapol_promoter_sigma_70_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'bacterial_RNApol_promoter_sigma_70';

--- ************************************************
--- *** relation: bacterial_rnapol_promoter_sigma54 ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW bacterial_rnapol_promoter_sigma54 AS
  SELECT
    feature_id AS bacterial_rnapol_promoter_sigma54_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'bacterial_RNApol_promoter_sigma54';

--- ************************************************
--- *** relation: minus_12_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A conserved region about 12-bp upstream  ***
--- *** of the start point of bacterial transcri ***
--- *** ption units, involved with sigma factor  ***
--- *** 54.                                      ***
--- ************************************************
---

CREATE VIEW minus_12_signal AS
  SELECT
    feature_id AS minus_12_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minus_12_signal';

--- ************************************************
--- *** relation: minus_24_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A conserved region about 12-bp upstream  ***
--- *** of the start point of bacterial transcri ***
--- *** ption units, involved with sigma factor  ***
--- *** 54.                                      ***
--- ************************************************
---

CREATE VIEW minus_24_signal AS
  SELECT
    feature_id AS minus_24_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minus_24_signal';

--- ************************************************
--- *** relation: a_box_type_1 ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An A box within an RNA polymerase III ty ***
--- *** pe 1 promoter.                           ***
--- ************************************************
---

CREATE VIEW a_box_type_1 AS
  SELECT
    feature_id AS a_box_type_1_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'A_box_type_1';

--- ************************************************
--- *** relation: a_box_type_2 ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An A box within an RNA polymerase III ty ***
--- *** pe 2 promoter.                           ***
--- ************************************************
---

CREATE VIEW a_box_type_2 AS
  SELECT
    feature_id AS a_box_type_2_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'A_box_type_2';

--- ************************************************
--- *** relation: intermediate_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A core promoter region of RNA polymerase ***
--- ***  III type 1 promoters.                   ***
--- ************************************************
---

CREATE VIEW intermediate_element AS
  SELECT
    feature_id AS intermediate_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'intermediate_element';

--- ************************************************
--- *** relation: regulatory_promoter_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A promoter element that is not part of t ***
--- *** he core promoter, but provides the promo ***
--- *** ter with a specific regulatory region.   ***
--- ************************************************
---

CREATE VIEW regulatory_promoter_element AS
  SELECT
    feature_id AS regulatory_promoter_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'proximal_promoter_element' OR cvterm.name = 'distal_promoter_element' OR cvterm.name = 'regulatory_promoter_element';

--- ************************************************
--- *** relation: transcription_regulatory_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A regulatory region that is involved in  ***
--- *** the control of the process of transcript ***
--- *** ion.                                     ***
--- ************************************************
---

CREATE VIEW transcription_regulatory_region AS
  SELECT
    feature_id AS transcription_regulatory_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'terminator' OR cvterm.name = 'TF_binding_site' OR cvterm.name = 'polyA_signal_sequence' OR cvterm.name = 'gene_group_regulatory_region' OR cvterm.name = 'transcriptional_cis_regulatory_region' OR cvterm.name = 'splicing_regulatory_region' OR cvterm.name = 'cis_regulatory_frameshift_element' OR cvterm.name = 'intronic_regulatory_region' OR cvterm.name = 'bacterial_terminator' OR cvterm.name = 'eukaryotic_terminator' OR cvterm.name = 'rho_dependent_bacterial_terminator' OR cvterm.name = 'rho_independent_bacterial_terminator' OR cvterm.name = 'terminator_of_type_2_RNApol_III_promoter' OR cvterm.name = 'operator' OR cvterm.name = 'bacterial_RNApol_promoter' OR cvterm.name = 'bacterial_terminator' OR cvterm.name = 'bacterial_RNApol_promoter_sigma_70' OR cvterm.name = 'bacterial_RNApol_promoter_sigma54' OR cvterm.name = 'rho_dependent_bacterial_terminator' OR cvterm.name = 'rho_independent_bacterial_terminator' OR cvterm.name = 'promoter' OR cvterm.name = 'insulator' OR cvterm.name = 'CRM' OR cvterm.name = 'promoter_targeting_sequence' OR cvterm.name = 'ISRE' OR cvterm.name = 'bidirectional_promoter' OR cvterm.name = 'RNA_polymerase_promoter' OR cvterm.name = 'RNApol_I_promoter' OR cvterm.name = 'RNApol_II_promoter' OR cvterm.name = 'RNApol_III_promoter' OR cvterm.name = 'bacterial_RNApol_promoter' OR cvterm.name = 'Phage_RNA_Polymerase_Promoter' OR cvterm.name = 'RNApol_II_core_promoter' OR cvterm.name = 'RNApol_III_promoter_type_1' OR cvterm.name = 'RNApol_III_promoter_type_2' OR cvterm.name = 'RNApol_III_promoter_type_3' OR cvterm.name = 'bacterial_RNApol_promoter_sigma_70' OR cvterm.name = 'bacterial_RNApol_promoter_sigma54' OR cvterm.name = 'SP6_RNA_Polymerase_Promoter' OR cvterm.name = 'T3_RNA_Polymerase_Promoter' OR cvterm.name = 'T7_RNA_Polymerase_Promoter' OR cvterm.name = 'locus_control_region' OR cvterm.name = 'enhancer' OR cvterm.name = 'silencer' OR cvterm.name = 'enhancer_bound_by_factor' OR cvterm.name = 'shadow_enhancer' OR cvterm.name = 'splice_enhancer' OR cvterm.name = 'intronic_splice_enhancer' OR cvterm.name = 'exonic_splice_enhancer' OR cvterm.name = 'transcription_regulatory_region';

--- ************************************************
--- *** relation: translation_regulatory_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A regulatory region that is involved in  ***
--- *** the control of the process of translatio ***
--- *** n.                                       ***
--- ************************************************
---

CREATE VIEW translation_regulatory_region AS
  SELECT
    feature_id AS translation_regulatory_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'attenuator' OR cvterm.name = 'translation_regulatory_region';

--- ************************************************
--- *** relation: recombination_regulatory_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A regulatory region that is involved in  ***
--- *** the control of the process of recombinat ***
--- *** ion.                                     ***
--- ************************************************
---

CREATE VIEW recombination_regulatory_region AS
  SELECT
    feature_id AS recombination_regulatory_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'recombination_regulatory_region';

--- ************************************************
--- *** relation: replication_regulatory_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A regulatory region that is involved in  ***
--- *** the control of the process of nucleotide ***
--- ***  replication.                            ***
--- ************************************************
---

CREATE VIEW replication_regulatory_region AS
  SELECT
    feature_id AS replication_regulatory_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'replication_regulatory_region';

--- ************************************************
--- *** relation: sequence_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence motif is a nucleotide or amin ***
--- *** o-acid sequence pattern that may have bi ***
--- *** ological significance.                   ***
--- ************************************************
---

CREATE VIEW sequence_motif AS
  SELECT
    feature_id AS sequence_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nucleotide_motif' OR cvterm.name = 'DNA_motif' OR cvterm.name = 'RNA_motif' OR cvterm.name = 'PSE_motif' OR cvterm.name = 'CAAT_signal' OR cvterm.name = 'minus_10_signal' OR cvterm.name = 'minus_35_signal' OR cvterm.name = 'DRE_motif' OR cvterm.name = 'E_box_motif' OR cvterm.name = 'INR1_motif' OR cvterm.name = 'GAGA_motif' OR cvterm.name = 'octamer_motif' OR cvterm.name = 'retinoic_acid_responsive_element' OR cvterm.name = 'promoter_element' OR cvterm.name = 'DCE_SI' OR cvterm.name = 'DCE_SII' OR cvterm.name = 'DCE_SIII' OR cvterm.name = 'minus_12_signal' OR cvterm.name = 'minus_24_signal' OR cvterm.name = 'GC_rich_promoter_region' OR cvterm.name = 'DMv4_motif' OR cvterm.name = 'DMv5_motif' OR cvterm.name = 'DMv3_motif' OR cvterm.name = 'DMv2_motif' OR cvterm.name = 'DPE1_motif' OR cvterm.name = 'DMv1_motif' OR cvterm.name = 'NDM2_motif' OR cvterm.name = 'NDM3_motif' OR cvterm.name = 'core_promoter_element' OR cvterm.name = 'regulatory_promoter_element' OR cvterm.name = 'INR_motif' OR cvterm.name = 'DPE_motif' OR cvterm.name = 'BREu_motif' OR cvterm.name = 'TATA_box' OR cvterm.name = 'A_box' OR cvterm.name = 'B_box' OR cvterm.name = 'C_box' OR cvterm.name = 'MTE' OR cvterm.name = 'BREd_motif' OR cvterm.name = 'DCE' OR cvterm.name = 'intermediate_element' OR cvterm.name = 'RNA_polymerase_II_TATA_box' OR cvterm.name = 'RNA_polymerase_III_TATA_box' OR cvterm.name = 'A_box_type_1' OR cvterm.name = 'A_box_type_2' OR cvterm.name = 'proximal_promoter_element' OR cvterm.name = 'distal_promoter_element' OR cvterm.name = 'RNA_internal_loop' OR cvterm.name = 'A_minor_RNA_motif' OR cvterm.name = 'RNA_junction_loop' OR cvterm.name = 'hammerhead_ribozyme' OR cvterm.name = 'asymmetric_RNA_internal_loop' OR cvterm.name = 'symmetric_RNA_internal_loop' OR cvterm.name = 'K_turn_RNA_motif' OR cvterm.name = 'sarcin_like_RNA_motif' OR cvterm.name = 'RNA_hook_turn' OR cvterm.name = 'sequence_motif';

--- ************************************************
--- *** relation: experimental_feature_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute of an experimentally derive ***
--- *** d feature.                               ***
--- ************************************************
---

CREATE VIEW experimental_feature_attribute AS
  SELECT
    feature_id AS experimental_feature_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'score' OR cvterm.name = 'quality_value' OR cvterm.name = 'experimental_feature_attribute';

--- ************************************************
--- *** relation: score ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The score of an experimentally derived f ***
--- *** eature such as a p-value.                ***
--- ************************************************
---

CREATE VIEW score AS
  SELECT
    feature_id AS score_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'score';

--- ************************************************
--- *** relation: quality_value ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An experimental feature attribute that d ***
--- *** efines the quality of the feature in a q ***
--- *** uantitative way, such as a phred quality ***
--- ***  score.                                  ***
--- ************************************************
---

CREATE VIEW quality_value AS
  SELECT
    feature_id AS quality_value_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'quality_value';

--- ************************************************
--- *** relation: restriction_enzyme_recognition_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The nucleotide region (usually a palindr ***
--- *** ome) that is recognized by a restriction ***
--- ***  enzyme. This may or may not be equal to ***
--- ***  the restriction enzyme binding site.    ***
--- ************************************************
---

CREATE VIEW restriction_enzyme_recognition_site AS
  SELECT
    feature_id AS restriction_enzyme_recognition_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'blunt_end_restriction_enzyme_cleavage_site' OR cvterm.name = 'sticky_end_restriction_enzyme_cleavage_site' OR cvterm.name = 'restriction_enzyme_recognition_site';

--- ************************************************
--- *** relation: restriction_enzyme_cleavage_junction ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The boundary at which a restriction enzy ***
--- *** me breaks the nucleotide sequence.       ***
--- ************************************************
---

CREATE VIEW restriction_enzyme_cleavage_junction AS
  SELECT
    feature_id AS restriction_enzyme_cleavage_junction_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'blunt_end_restriction_enzyme_cleavage_junction' OR cvterm.name = 'single_strand_restriction_enzyme_cleavage_site' OR cvterm.name = 'five_prime_restriction_enzyme_junction' OR cvterm.name = 'three_prime_restriction_enzyme_junction' OR cvterm.name = 'restriction_enzyme_cleavage_junction';

--- ************************************************
--- *** relation: five_prime_restriction_enzyme_junction ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The restriction enzyme cleavage junction ***
--- ***  on the 5' strand of the nucleotide sequ ***
--- *** ence.                                    ***
--- ************************************************
---

CREATE VIEW five_prime_restriction_enzyme_junction AS
  SELECT
    feature_id AS five_prime_restriction_enzyme_junction_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_restriction_enzyme_junction';

--- ************************************************
--- *** relation: three_prime_restriction_enzyme_junction ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW three_prime_restriction_enzyme_junction AS
  SELECT
    feature_id AS three_prime_restriction_enzyme_junction_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_restriction_enzyme_junction';

--- ************************************************
--- *** relation: blunt_end_restriction_enzyme_cleavage_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW blunt_end_restriction_enzyme_cleavage_site AS
  SELECT
    feature_id AS blunt_end_restriction_enzyme_cleavage_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'blunt_end_restriction_enzyme_cleavage_site';

--- ************************************************
--- *** relation: sticky_end_restriction_enzyme_cleavage_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW sticky_end_restriction_enzyme_cleavage_site AS
  SELECT
    feature_id AS sticky_end_restriction_enzyme_cleavage_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'sticky_end_restriction_enzyme_cleavage_site';

--- ************************************************
--- *** relation: blunt_end_restriction_enzyme_cleavage_junction ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A restriction enzyme cleavage site where ***
--- ***  both strands are cut at the same positi ***
--- *** on.                                      ***
--- ************************************************
---

CREATE VIEW blunt_end_restriction_enzyme_cleavage_junction AS
  SELECT
    feature_id AS blunt_end_restriction_enzyme_cleavage_junction_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'blunt_end_restriction_enzyme_cleavage_junction';

--- ************************************************
--- *** relation: single_strand_restriction_enzyme_cleavage_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A restriction enzyme cleavage site where ***
--- *** by only one strand is cut.               ***
--- ************************************************
---

CREATE VIEW single_strand_restriction_enzyme_cleavage_site AS
  SELECT
    feature_id AS single_strand_restriction_enzyme_cleavage_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_restriction_enzyme_junction' OR cvterm.name = 'three_prime_restriction_enzyme_junction' OR cvterm.name = 'single_strand_restriction_enzyme_cleavage_site';

--- ************************************************
--- *** relation: restriction_enzyme_single_strand_overhang ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A terminal region of DNA sequence where  ***
--- *** the end of the region is not blunt ended ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW restriction_enzyme_single_strand_overhang AS
  SELECT
    feature_id AS restriction_enzyme_single_strand_overhang_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'restriction_enzyme_single_strand_overhang';

--- ************************************************
--- *** relation: experimentally_defined_binding_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region that has been implicated in bin ***
--- *** ding although the exact coordinates of b ***
--- *** inding may be unknown.                   ***
--- ************************************************
---

CREATE VIEW experimentally_defined_binding_region AS
  SELECT
    feature_id AS experimentally_defined_binding_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'CHiP_seq_region' OR cvterm.name = 'experimentally_defined_binding_region';

--- ************************************************
--- *** relation: chip_seq_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of sequence identified by CHiP  ***
--- *** seq technology to contain a protein bind ***
--- *** ing site.                                ***
--- ************************************************
---

CREATE VIEW chip_seq_region AS
  SELECT
    feature_id AS chip_seq_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'CHiP_seq_region';

--- ************************************************
--- *** relation: aspe_primer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** "A primer containing an SNV at the 3' en ***
--- *** d for accurate genotyping.               ***
--- ************************************************
---

CREATE VIEW aspe_primer AS
  SELECT
    feature_id AS aspe_primer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ASPE_primer';

--- ************************************************
--- *** relation: dcaps_primer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primer with one or more mis-matches to ***
--- ***  the DNA template corresponding to a pos ***
--- *** ition within a restriction enzyme recogn ***
--- *** ition site.                              ***
--- ************************************************
---

CREATE VIEW dcaps_primer AS
  SELECT
    feature_id AS dcaps_primer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dCAPS_primer';

--- ************************************************
--- *** relation: histone_modification ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Histone modification is a post translati ***
--- *** onally modified region whereby residues  ***
--- *** of the histone protein are modified by m ***
--- *** ethylation, acetylation, phosphorylation ***
--- *** , ubiquitination, sumoylation, citrullin ***
--- *** ation, or ADP-ribosylation.              ***
--- ************************************************
---

CREATE VIEW histone_modification AS
  SELECT
    feature_id AS histone_modification_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'histone_methylation_site' OR cvterm.name = 'histone_acetylation_site' OR cvterm.name = 'histone_ubiqitination_site' OR cvterm.name = 'histone_acylation_region' OR cvterm.name = 'H4K20_monomethylation_site' OR cvterm.name = 'H2BK5_monomethylation_site' OR cvterm.name = 'H3K27_methylation_site' OR cvterm.name = 'H3K36_methylation_site' OR cvterm.name = 'H3K4_methylation_site' OR cvterm.name = 'H3K79_methylation_site' OR cvterm.name = 'H3K9_methylation_site' OR cvterm.name = 'H3K27_monomethylation_site' OR cvterm.name = 'H3K27_trimethylation_site' OR cvterm.name = 'H3K27_dimethylation_site' OR cvterm.name = 'H3K36_monomethylation_site' OR cvterm.name = 'H3K36_dimethylation_site' OR cvterm.name = 'H3K36_trimethylation_site' OR cvterm.name = 'H3K4_monomethylation_site' OR cvterm.name = 'H3K4_trimethylation' OR cvterm.name = 'H3K4_dimethylation_site' OR cvterm.name = 'H3K79_monomethylation_site' OR cvterm.name = 'H3K79_dimethylation_site' OR cvterm.name = 'H3K79_trimethylation_site' OR cvterm.name = 'H3K9_trimethylation_site' OR cvterm.name = 'H3K9_monomethylation_site' OR cvterm.name = 'H3K9_dimethylation_site' OR cvterm.name = 'H3K9_acetylation_site' OR cvterm.name = 'H3K14_acetylation_site' OR cvterm.name = 'H3K18_acetylation_site' OR cvterm.name = 'H3K23_acylation site' OR cvterm.name = 'H3K27_acylation_site' OR cvterm.name = 'H4K16_acylation_site' OR cvterm.name = 'H4K5_acylation_site' OR cvterm.name = 'H4K8_acylation site' OR cvterm.name = 'H2B_ubiquitination_site' OR cvterm.name = 'H4K_acylation_region' OR cvterm.name = 'histone_modification';

--- ************************************************
--- *** relation: histone_methylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A histone modification site where the mo ***
--- *** dification is the methylation of the res ***
--- *** idue.                                    ***
--- ************************************************
---

CREATE VIEW histone_methylation_site AS
  SELECT
    feature_id AS histone_methylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H4K20_monomethylation_site' OR cvterm.name = 'H2BK5_monomethylation_site' OR cvterm.name = 'H3K27_methylation_site' OR cvterm.name = 'H3K36_methylation_site' OR cvterm.name = 'H3K4_methylation_site' OR cvterm.name = 'H3K79_methylation_site' OR cvterm.name = 'H3K9_methylation_site' OR cvterm.name = 'H3K27_monomethylation_site' OR cvterm.name = 'H3K27_trimethylation_site' OR cvterm.name = 'H3K27_dimethylation_site' OR cvterm.name = 'H3K36_monomethylation_site' OR cvterm.name = 'H3K36_dimethylation_site' OR cvterm.name = 'H3K36_trimethylation_site' OR cvterm.name = 'H3K4_monomethylation_site' OR cvterm.name = 'H3K4_trimethylation' OR cvterm.name = 'H3K4_dimethylation_site' OR cvterm.name = 'H3K79_monomethylation_site' OR cvterm.name = 'H3K79_dimethylation_site' OR cvterm.name = 'H3K79_trimethylation_site' OR cvterm.name = 'H3K9_trimethylation_site' OR cvterm.name = 'H3K9_monomethylation_site' OR cvterm.name = 'H3K9_dimethylation_site' OR cvterm.name = 'histone_methylation_site';

--- ************************************************
--- *** relation: histone_acetylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A histone modification where the modific ***
--- *** ation is the acylation of the residue.   ***
--- ************************************************
---

CREATE VIEW histone_acetylation_site AS
  SELECT
    feature_id AS histone_acetylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K9_acetylation_site' OR cvterm.name = 'H3K14_acetylation_site' OR cvterm.name = 'H3K18_acetylation_site' OR cvterm.name = 'H3K23_acylation site' OR cvterm.name = 'H3K27_acylation_site' OR cvterm.name = 'H4K16_acylation_site' OR cvterm.name = 'H4K5_acylation_site' OR cvterm.name = 'H4K8_acylation site' OR cvterm.name = 'histone_acetylation_site';

--- ************************************************
--- *** relation: h3k9_acetylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 9th residue (a lysine), from th ***
--- *** e start of the H3 histone protein is acy ***
--- *** lated.                                   ***
--- ************************************************
---

CREATE VIEW h3k9_acetylation_site AS
  SELECT
    feature_id AS h3k9_acetylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K9_acetylation_site';

--- ************************************************
--- *** relation: h3k14_acetylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 14th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is ac ***
--- *** ylated.                                  ***
--- ************************************************
---

CREATE VIEW h3k14_acetylation_site AS
  SELECT
    feature_id AS h3k14_acetylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K14_acetylation_site';

--- ************************************************
--- *** relation: h3k4_monomethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification, whereby  ***
--- *** the 4th residue (a lysine), from the sta ***
--- *** rt of the H3 protein is mono-methylated. ***
--- ************************************************
---

CREATE VIEW h3k4_monomethylation_site AS
  SELECT
    feature_id AS h3k4_monomethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K4_monomethylation_site';

--- ************************************************
--- *** relation: h3k4_trimethylation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 4th residue (a lysine), from th ***
--- *** e start of the H3 protein is tri-methyla ***
--- *** ted.                                     ***
--- ************************************************
---

CREATE VIEW h3k4_trimethylation AS
  SELECT
    feature_id AS h3k4_trimethylation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K4_trimethylation';

--- ************************************************
--- *** relation: h3k9_trimethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 9th residue (a lysine), from th ***
--- *** e start of the H3 histone protein is tri ***
--- *** -methylated.                             ***
--- ************************************************
---

CREATE VIEW h3k9_trimethylation_site AS
  SELECT
    feature_id AS h3k9_trimethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K9_trimethylation_site';

--- ************************************************
--- *** relation: h3k27_monomethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 27th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is mo ***
--- *** no-methylated.                           ***
--- ************************************************
---

CREATE VIEW h3k27_monomethylation_site AS
  SELECT
    feature_id AS h3k27_monomethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K27_monomethylation_site';

--- ************************************************
--- *** relation: h3k27_trimethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 27th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is tr ***
--- *** i-methylated.                            ***
--- ************************************************
---

CREATE VIEW h3k27_trimethylation_site AS
  SELECT
    feature_id AS h3k27_trimethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K27_trimethylation_site';

--- ************************************************
--- *** relation: h3k79_monomethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 79th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is mo ***
--- *** no- methylated.                          ***
--- ************************************************
---

CREATE VIEW h3k79_monomethylation_site AS
  SELECT
    feature_id AS h3k79_monomethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K79_monomethylation_site';

--- ************************************************
--- *** relation: h3k79_dimethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 79th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is di ***
--- *** -methylated.                             ***
--- ************************************************
---

CREATE VIEW h3k79_dimethylation_site AS
  SELECT
    feature_id AS h3k79_dimethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K79_dimethylation_site';

--- ************************************************
--- *** relation: h3k79_trimethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 79th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is tr ***
--- *** i-methylated.                            ***
--- ************************************************
---

CREATE VIEW h3k79_trimethylation_site AS
  SELECT
    feature_id AS h3k79_trimethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K79_trimethylation_site';

--- ************************************************
--- *** relation: h4k20_monomethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 20th residue (a lysine), from t ***
--- *** he start of the H34histone protein is mo ***
--- *** no-methylated.                           ***
--- ************************************************
---

CREATE VIEW h4k20_monomethylation_site AS
  SELECT
    feature_id AS h4k20_monomethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H4K20_monomethylation_site';

--- ************************************************
--- *** relation: h2bk5_monomethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 5th residue (a lysine), from th ***
--- *** e start of the H2B protein is methylated ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW h2bk5_monomethylation_site AS
  SELECT
    feature_id AS h2bk5_monomethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H2BK5_monomethylation_site';

--- ************************************************
--- *** relation: isre ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An ISRE is a transcriptional cis regulat ***
--- *** ory region, containing the consensus reg ***
--- *** ion: YAGTTTC(A/T)YTTTYCC, responsible fo ***
--- *** r increased transcription via interferon ***
--- ***  binding.                                ***
--- ************************************************
---

CREATE VIEW isre AS
  SELECT
    feature_id AS isre_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ISRE';

--- ************************************************

SET search_path=so,chado,pg_catalog;
--- ************************************************
---

CREATE VIEW stop_codon_redefined_as_selenocysteine AS
  SELECT
    feature_id AS stop_codon_redefined_as_selenocysteine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stop_codon_redefined_as_selenocysteine';

--- ************************************************
--- *** relation: recoded_by_translational_bypass ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Recoded mRNA where a block of nucleotide ***
--- *** s is not translated.                     ***
--- ************************************************
---

CREATE VIEW recoded_by_translational_bypass AS
  SELECT
    feature_id AS recoded_by_translational_bypass_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'recoded_by_translational_bypass';

--- ************************************************
--- *** relation: translationally_frameshifted ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Recoding by frameshifting a particular s ***
--- *** ite.                                     ***
--- ************************************************
---

CREATE VIEW translationally_frameshifted AS
  SELECT
    feature_id AS translationally_frameshifted_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minus_1_translationally_frameshifted' OR cvterm.name = 'plus_1_translationally_frameshifted' OR cvterm.name = 'translationally_frameshifted';

--- ************************************************
--- *** relation: maternally_imprinted_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is maternally_imprinted.     ***
--- ************************************************
---

CREATE VIEW maternally_imprinted_gene AS
  SELECT
    feature_id AS maternally_imprinted_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'maternally_imprinted_gene';

--- ************************************************
--- *** relation: paternally_imprinted_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is paternally imprinted.     ***
--- ************************************************
---

CREATE VIEW paternally_imprinted_gene AS
  SELECT
    feature_id AS paternally_imprinted_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'paternally_imprinted_gene';

--- ************************************************
--- *** relation: post_translationally_regulated_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is post translationally regu ***
--- *** lated.                                   ***
--- ************************************************
---

CREATE VIEW post_translationally_regulated_gene AS
  SELECT
    feature_id AS post_translationally_regulated_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'post_translationally_regulated_gene';

--- ************************************************
--- *** relation: negatively_autoregulated_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is negatively autoreguated.  ***
--- ************************************************
---

CREATE VIEW negatively_autoregulated_gene AS
  SELECT
    feature_id AS negatively_autoregulated_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'negatively_autoregulated_gene';

--- ************************************************
--- *** relation: positively_autoregulated_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is positively autoregulated. ***
--- ************************************************
---

CREATE VIEW positively_autoregulated_gene AS
  SELECT
    feature_id AS positively_autoregulated_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'positively_autoregulated_gene';

--- ************************************************
--- *** relation: silenced ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing an epigenetic pr ***
--- *** ocess where a gene is inactivated at tra ***
--- *** nscriptional or translational level.     ***
--- ************************************************
---

CREATE VIEW silenced AS
  SELECT
    feature_id AS silenced_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'silenced_by_DNA_modification' OR cvterm.name = 'silenced_by_RNA_interference' OR cvterm.name = 'silenced_by_histone_modification' OR cvterm.name = 'silenced_by_DNA_methylation' OR cvterm.name = 'silenced_by_histone_methylation' OR cvterm.name = 'silenced_by_histone_deacetylation' OR cvterm.name = 'silenced';

--- ************************************************
--- *** relation: silenced_by_dna_modification ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing an epigenetic pr ***
--- *** ocess where a gene is inactivated by DNA ***
--- ***  modifications, resulting in repression  ***
--- *** of transcription.                        ***
--- ************************************************
---

CREATE VIEW silenced_by_dna_modification AS
  SELECT
    feature_id AS silenced_by_dna_modification_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'silenced_by_DNA_methylation' OR cvterm.name = 'silenced_by_DNA_modification';

--- ************************************************
--- *** relation: silenced_by_dna_methylation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing an epigenetic pr ***
--- *** ocess where a gene is inactivated by DNA ***
--- ***  methylation, resulting in repression of ***
--- ***  transcription.                          ***
--- ************************************************
---

CREATE VIEW silenced_by_dna_methylation AS
  SELECT
    feature_id AS silenced_by_dna_methylation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'silenced_by_DNA_methylation';

--- ************************************************
--- *** relation: translationally_regulated_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is translationally regulated ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW translationally_regulated_gene AS
  SELECT
    feature_id AS translationally_regulated_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'translationally_regulated_gene';

--- ************************************************
--- *** relation: allelically_excluded_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is allelically_excluded.     ***
--- ************************************************
---

CREATE VIEW allelically_excluded_gene AS
  SELECT
    feature_id AS allelically_excluded_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'allelically_excluded_gene';

--- ************************************************
--- *** relation: epigenetically_modified_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is epigenetically modified.  ***
--- ************************************************
---

CREATE VIEW epigenetically_modified_gene AS
  SELECT
    feature_id AS epigenetically_modified_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_rearranged_at_DNA_level' OR cvterm.name = 'maternally_imprinted_gene' OR cvterm.name = 'paternally_imprinted_gene' OR cvterm.name = 'allelically_excluded_gene' OR cvterm.name = 'epigenetically_modified_gene';

--- ************************************************
--- *** relation: transgene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transgene is a gene that has been tran ***
--- *** sferred naturally or by any of a number  ***
--- *** of genetic engineering techniques from o ***
--- *** ne organism to another.                  ***
--- ************************************************
---

CREATE VIEW transgene AS
  SELECT
    feature_id AS transgene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'floxed_gene' OR cvterm.name = 'transgene';

--- ************************************************
--- *** relation: endogenous_retroviral_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW endogenous_retroviral_sequence AS
  SELECT
    feature_id AS endogenous_retroviral_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'endogenous_retroviral_sequence';

--- ************************************************
--- *** relation: rearranged_at_dna_level ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe the sequence of ***
--- ***  a feature, where the DNA is rearranged. ***
--- ************************************************
---

CREATE VIEW rearranged_at_dna_level AS
  SELECT
    feature_id AS rearranged_at_dna_level_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rearranged_at_DNA_level';

--- ************************************************
--- *** relation: status ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing the status of a  ***
--- *** feature, based on the available evidence ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW status AS
  SELECT
    feature_id AS status_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'fragmentary' OR cvterm.name = 'predicted' OR cvterm.name = 'validated' OR cvterm.name = 'invalidated' OR cvterm.name = 'independently_known' OR cvterm.name = 'consensus' OR cvterm.name = 'low_complexity' OR cvterm.name = 'whole_genome_sequence_status' OR cvterm.name = 'supported_by_sequence_similarity' OR cvterm.name = 'orphan' OR cvterm.name = 'predicted_by_ab_initio_computation' OR cvterm.name = 'supported_by_domain_match' OR cvterm.name = 'supported_by_EST_or_cDNA' OR cvterm.name = 'experimentally_determined' OR cvterm.name = 'invalidated_by_chimeric_cDNA' OR cvterm.name = 'invalidated_by_genomic_contamination' OR cvterm.name = 'invalidated_by_genomic_polyA_primed_cDNA' OR cvterm.name = 'invalidated_by_partial_processing' OR cvterm.name = 'standard_draft' OR cvterm.name = 'high_quality_draft' OR cvterm.name = 'improved_high_quality_draft' OR cvterm.name = 'annotation_directed_improved_draft' OR cvterm.name = 'noncontiguous_finished' OR cvterm.name = 'finished_genome' OR cvterm.name = 'status';

--- ************************************************
--- *** relation: independently_known ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Attribute to describe a feature that is  ***
--- *** independently known - not predicted.     ***
--- ************************************************
---

CREATE VIEW independently_known AS
  SELECT
    feature_id AS independently_known_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'independently_known';

--- ************************************************
--- *** relation: supported_by_sequence_similarity ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a feature that  ***
--- *** has been predicted using sequence simila ***
--- *** rity techniques.                         ***
--- ************************************************
---

CREATE VIEW supported_by_sequence_similarity AS
  SELECT
    feature_id AS supported_by_sequence_similarity_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'supported_by_domain_match' OR cvterm.name = 'supported_by_EST_or_cDNA' OR cvterm.name = 'supported_by_sequence_similarity';

--- ************************************************
--- *** relation: supported_by_domain_match ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a feature that  ***
--- *** has been predicted using sequence simila ***
--- *** rity of a known domain.                  ***
--- ************************************************
---

CREATE VIEW supported_by_domain_match AS
  SELECT
    feature_id AS supported_by_domain_match_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'supported_by_domain_match';

--- ************************************************
--- *** relation: supported_by_est_or_cdna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a feature that  ***
--- *** has been predicted using sequence simila ***
--- *** rity to EST or cDNA data.                ***
--- ************************************************
---

CREATE VIEW supported_by_est_or_cdna AS
  SELECT
    feature_id AS supported_by_est_or_cdna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'supported_by_EST_or_cDNA';

--- ************************************************
--- *** relation: orphan ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW orphan AS
  SELECT
    feature_id AS orphan_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'orphan';

--- ************************************************
--- *** relation: predicted_by_ab_initio_computation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a feature that i ***
--- *** s predicted by a computer program that d ***
--- *** id not rely on sequence similarity.      ***
--- ************************************************
---

CREATE VIEW predicted_by_ab_initio_computation AS
  SELECT
    feature_id AS predicted_by_ab_initio_computation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'predicted_by_ab_initio_computation';

--- ************************************************
--- *** relation: asx_turn ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of three consecutive residues an ***
--- *** d one H-bond in which: residue(i) is Asp ***
--- *** artate or Asparagine (Asx), the side-cha ***
--- *** in O of residue(i) is H-bonded to the ma ***
--- *** in-chain NH of residue(i+2).             ***
--- ************************************************
---

CREATE VIEW asx_turn AS
  SELECT
    feature_id AS asx_turn_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'asx_turn_left_handed_type_one' OR cvterm.name = 'asx_turn_left_handed_type_two' OR cvterm.name = 'asx_turn_right_handed_type_two' OR cvterm.name = 'asx_turn_right_handed_type_one' OR cvterm.name = 'asx_turn';

--- ************************************************
--- *** relation: cloned_cdna_insert ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A clone insert made from cDNA.           ***
--- ************************************************
---

CREATE VIEW cloned_cdna_insert AS
  SELECT
    feature_id AS cloned_cdna_insert_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cloned_cDNA_insert';

--- ************************************************
--- *** relation: cloned_genomic_insert ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A clone insert made from genomic DNA.    ***
--- ************************************************
---

CREATE VIEW cloned_genomic_insert AS
  SELECT
    feature_id AS cloned_genomic_insert_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'BAC_cloned_genomic_insert' OR cvterm.name = 'cloned_genomic_insert';

--- ************************************************
--- *** relation: engineered_insert ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A clone insert that is engineered.       ***
--- ************************************************
---

CREATE VIEW engineered_insert AS
  SELECT
    feature_id AS engineered_insert_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_insert';

--- ************************************************
--- *** relation: edited_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An mRNA that is edited.                  ***
--- ************************************************
---

CREATE VIEW edited_mrna AS
  SELECT
    feature_id AS edited_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'edited_mRNA';

--- ************************************************
--- *** relation: guide_rna_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of guide RNA.                   ***
--- ************************************************
---

CREATE VIEW guide_rna_region AS
  SELECT
    feature_id AS guide_rna_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'anchor_region' OR cvterm.name = 'template_region' OR cvterm.name = 'guide_RNA_region';

--- ************************************************
--- *** relation: anchor_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a guide_RNA that base-pairs  ***
--- *** to a target mRNA.                        ***
--- ************************************************
---

CREATE VIEW anchor_region AS
  SELECT
    feature_id AS anchor_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'anchor_region';

--- ************************************************
--- *** relation: pre_edited_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW pre_edited_mrna AS
  SELECT
    feature_id AS pre_edited_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pre_edited_mRNA';

--- ************************************************
--- *** relation: intermediate ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a feature betwe ***
--- *** en stages of processing.                 ***
--- ************************************************
---

CREATE VIEW intermediate AS
  SELECT
    feature_id AS intermediate_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'intermediate';

--- ************************************************
--- *** relation: mirna_target_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A miRNA target site is a binding site wh ***
--- *** ere the molecule is a micro RNA.         ***
--- ************************************************
---

CREATE VIEW mirna_target_site AS
  SELECT
    feature_id AS mirna_target_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'miRNA_target_site';

--- ************************************************
--- *** relation: edited_cds ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A CDS that is edited.                    ***
--- ************************************************
---

CREATE VIEW edited_cds AS
  SELECT
    feature_id AS edited_cds_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'edited_CDS';

--- ************************************************
--- *** relation: vertebrate_immunoglobulin_t_cell_receptor_rearranged_segment ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW vertebrate_immunoglobulin_t_cell_receptor_rearranged_segment AS
  SELECT
    feature_id AS vertebrate_immunoglobulin_t_cell_receptor_rearranged_segment_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'VD_gene' OR cvterm.name = 'DJ_gene' OR cvterm.name = 'VDJ_gene' OR cvterm.name = 'VJ_gene' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_rearranged_segment';

--- ************************************************
--- *** relation: vertebrate_ig_t_cell_receptor_rearranged_gene_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW vertebrate_ig_t_cell_receptor_rearranged_gene_cluster AS
  SELECT
    feature_id AS vertebrate_ig_t_cell_receptor_rearranged_gene_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DJ_J_cluster' OR cvterm.name = 'VDJ_J_C_cluster' OR cvterm.name = 'VDJ_J_cluster' OR cvterm.name = 'VJ_C_cluster' OR cvterm.name = 'VJ_J_C_cluster' OR cvterm.name = 'VJ_J_cluster' OR cvterm.name = 'D_DJ_C_cluster' OR cvterm.name = 'D_DJ_cluster' OR cvterm.name = 'D_DJ_J_C_cluster' OR cvterm.name = 'D_DJ_J_cluster' OR cvterm.name = 'V_DJ_cluster' OR cvterm.name = 'V_DJ_J_cluster' OR cvterm.name = 'V_VDJ_C_cluster' OR cvterm.name = 'V_VDJ_cluster' OR cvterm.name = 'V_VDJ_J_cluster' OR cvterm.name = 'V_VJ_C_cluster' OR cvterm.name = 'V_VJ_cluster' OR cvterm.name = 'V_VJ_J_cluster' OR cvterm.name = 'V_D_DJ_C_cluster' OR cvterm.name = 'V_D_DJ_cluster' OR cvterm.name = 'V_D_DJ_J_C_cluster' OR cvterm.name = 'V_D_DJ_J_cluster' OR cvterm.name = 'V_D_J_C_cluster' OR cvterm.name = 'V_D_J_cluster' OR cvterm.name = 'DJ_C_cluster' OR cvterm.name = 'DJ_J_C_cluster' OR cvterm.name = 'VDJ_C_cluster' OR cvterm.name = 'V_DJ_C_cluster' OR cvterm.name = 'V_DJ_J_C_cluster' OR cvterm.name = 'V_VDJ_J_C_cluster' OR cvterm.name = 'V_VJ_J_C_cluster' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_rearranged_gene_cluster';

--- ************************************************
--- *** relation: vertebrate_immune_system_gene_recombination_signal_feature ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW vertebrate_immune_system_gene_recombination_signal_feature AS
  SELECT
    feature_id AS vertebrate_immune_system_gene_recombination_signal_feature_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'J_gene_recombination_feature' OR cvterm.name = 'D_gene_recombination_feature' OR cvterm.name = 'V_gene_recombination_feature' OR cvterm.name = 'heptamer_of_recombination_feature_of_vertebrate_immune_system_gene' OR cvterm.name = 'nonamer_of_recombination_feature_of_vertebrate_immune_system_gene' OR cvterm.name = 'five_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_heptamer' OR cvterm.name = 'five_prime_D_heptamer' OR cvterm.name = 'J_heptamer' OR cvterm.name = 'V_heptamer' OR cvterm.name = 'three_prime_D_nonamer' OR cvterm.name = 'five_prime_D_nonamer' OR cvterm.name = 'J_nonamer' OR cvterm.name = 'V_nonamer' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_signal_feature';

--- ************************************************
--- *** relation: recombinationally_rearranged ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW recombinationally_rearranged AS
  SELECT
    feature_id AS recombinationally_rearranged_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'recombinationally_rearranged';

--- ************************************************
--- *** relation: recombinationally_rearranged_vertebrate_immune_system_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A recombinationally rearranged gene of t ***
--- *** he vertebrate immune system.             ***
--- ************************************************
---

CREATE VIEW recombinationally_rearranged_vertebrate_immune_system_gene AS
  SELECT
    feature_id AS recombinationally_rearranged_vertebrate_immune_system_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'recombinationally_rearranged_vertebrate_immune_system_gene';

--- ************************************************
--- *** relation: attp_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An integration/excision site of a phage  ***
--- *** chromosome at which a recombinase acts t ***
--- *** o insert the phage DNA at a cognate inte ***
--- *** gration/excision site on a bacterial chr ***
--- *** omosome.                                 ***
--- ************************************************
---

CREATE VIEW attp_site AS
  SELECT
    feature_id AS attp_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'attP_site';

--- ************************************************
--- *** relation: attb_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An integration/excision site of a bacter ***
--- *** ial chromosome at which a recombinase ac ***
--- *** ts to insert foreign DNA containing a co ***
--- *** gnate integration/excision site.         ***
--- ************************************************
---

CREATE VIEW attb_site AS
  SELECT
    feature_id AS attb_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'attB_site';

--- ************************************************
--- *** relation: attl_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region that results from recombination ***
--- ***  between attP_site and attB_site, compos ***
--- *** ed of the 5' portion of attB_site and th ***
--- *** e 3' portion of attP_site.               ***
--- ************************************************
---

CREATE VIEW attl_site AS
  SELECT
    feature_id AS attl_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'attL_site';

--- ************************************************
--- *** relation: attr_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region that results from recombination ***
--- ***  between attP_site and attB_site, compos ***
--- *** ed of the 5' portion of attP_site and th ***
--- *** e 3' portion of attB_site.               ***
--- ************************************************
---

CREATE VIEW attr_site AS
  SELECT
    feature_id AS attr_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'attR_site';

--- ************************************************
--- *** relation: integration_excision_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region specifically recognised by a re ***
--- *** combinase, which inserts or removes anot ***
--- *** her region marked by a distinct cognate  ***
--- *** integration/excision site.               ***
--- ************************************************
---

CREATE VIEW integration_excision_site AS
  SELECT
    feature_id AS integration_excision_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'attI_site' OR cvterm.name = 'attP_site' OR cvterm.name = 'attB_site' OR cvterm.name = 'attL_site' OR cvterm.name = 'attR_site' OR cvterm.name = 'attC_site' OR cvterm.name = 'attCtn_site' OR cvterm.name = 'integration_excision_site';

--- ************************************************
--- *** relation: resolution_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region specifically recognised by a re ***
--- *** combinase, which separates a physically  ***
--- *** contiguous circle of DNA into two physic ***
--- *** ally separate circles.                   ***
--- ************************************************
---

CREATE VIEW resolution_site AS
  SELECT
    feature_id AS resolution_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'loxP_site' OR cvterm.name = 'dif_site' OR cvterm.name = 'resolution_site';

--- ************************************************
--- *** relation: inversion_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region specifically recognised by a re ***
--- *** combinase, which inverts the region flan ***
--- *** ked by a pair of sites.                  ***
--- ************************************************
---

CREATE VIEW inversion_site AS
  SELECT
    feature_id AS inversion_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'FRT_site' OR cvterm.name = 'inversion_site';

--- ************************************************
--- *** relation: dif_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A site at which replicated bacterial cir ***
--- *** cular chromosomes are decatenated by sit ***
--- *** e specific resolvase.                    ***
--- ************************************************
---

CREATE VIEW dif_site AS
  SELECT
    feature_id AS dif_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dif_site';

--- ************************************************
--- *** relation: attc_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attC site is a sequence required for  ***
--- *** the integration of a DNA of an integron. ***
--- ************************************************
---

CREATE VIEW attc_site AS
  SELECT
    feature_id AS attc_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'attC_site';

--- ************************************************
--- *** relation: eukaryotic_terminator ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW eukaryotic_terminator AS
  SELECT
    feature_id AS eukaryotic_terminator_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'terminator_of_type_2_RNApol_III_promoter' OR cvterm.name = 'eukaryotic_terminator';

--- ************************************************
--- *** relation: oriv ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An origin of vegetative replication in p ***
--- *** lasmids and phages.                      ***
--- ************************************************
---

CREATE VIEW oriv AS
  SELECT
    feature_id AS oriv_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'oriV';

--- ************************************************
--- *** relation: oric ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An origin of bacterial chromosome replic ***
--- *** ation.                                   ***
--- ************************************************
---

CREATE VIEW oric AS
  SELECT
    feature_id AS oric_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'oriC';

--- ************************************************
--- *** relation: dna_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a self-repli ***
--- *** cating, DNA molecule.                    ***
--- ************************************************
---

CREATE VIEW dna_chromosome AS
  SELECT
    feature_id AS dna_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'double_stranded_DNA_chromosome' OR cvterm.name = 'single_stranded_DNA_chromosome' OR cvterm.name = 'linear_double_stranded_DNA_chromosome' OR cvterm.name = 'circular_double_stranded_DNA_chromosome' OR cvterm.name = 'linear_single_stranded_DNA_chromosome' OR cvterm.name = 'circular_single_stranded_DNA_chromosome' OR cvterm.name = 'DNA_chromosome';

--- ************************************************
--- *** relation: double_stranded_dna_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a self-repli ***
--- *** cating, double-stranded DNA molecule.    ***
--- ************************************************
---

CREATE VIEW double_stranded_dna_chromosome AS
  SELECT
    feature_id AS double_stranded_dna_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'linear_double_stranded_DNA_chromosome' OR cvterm.name = 'circular_double_stranded_DNA_chromosome' OR cvterm.name = 'double_stranded_DNA_chromosome';

--- ************************************************
--- *** relation: single_stranded_dna_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a self-repli ***
--- *** cating, single-stranded DNA molecule.    ***
--- ************************************************
---

CREATE VIEW single_stranded_dna_chromosome AS
  SELECT
    feature_id AS single_stranded_dna_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'linear_single_stranded_DNA_chromosome' OR cvterm.name = 'circular_single_stranded_DNA_chromosome' OR cvterm.name = 'single_stranded_DNA_chromosome';

--- ************************************************
--- *** relation: linear_double_stranded_dna_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a self-repli ***
--- *** cating, double-stranded, linear DNA mole ***
--- *** cule.                                    ***
--- ************************************************
---

CREATE VIEW linear_double_stranded_dna_chromosome AS
  SELECT
    feature_id AS linear_double_stranded_dna_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'linear_double_stranded_DNA_chromosome';

--- ************************************************
--- *** relation: circular_double_stranded_dna_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a self-repli ***
--- *** cating, double-stranded, circular DNA mo ***
--- *** lecule.                                  ***
--- ************************************************
---

CREATE VIEW circular_double_stranded_dna_chromosome AS
  SELECT
    feature_id AS circular_double_stranded_dna_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'circular_double_stranded_DNA_chromosome';

--- ************************************************
--- *** relation: linear_single_stranded_dna_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a self-repli ***
--- *** cating, single-stranded, linear DNA mole ***
--- *** cule.                                    ***
--- ************************************************
---

CREATE VIEW linear_single_stranded_dna_chromosome AS
  SELECT
    feature_id AS linear_single_stranded_dna_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'linear_single_stranded_DNA_chromosome';

--- ************************************************
--- *** relation: circular_single_stranded_dna_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a self-repli ***
--- *** cating, single-stranded, circular DNA mo ***
--- *** lecule.                                  ***
--- ************************************************
---

CREATE VIEW circular_single_stranded_dna_chromosome AS
  SELECT
    feature_id AS circular_single_stranded_dna_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'circular_single_stranded_DNA_chromosome';

--- ************************************************

SET search_path=so,chado,pg_catalog;
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A genome is the sum of genetic material  ***
--- *** within a cell or virion.                 ***
--- ************************************************
---

CREATE VIEW genome AS
  SELECT
    feature_id AS genome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'kinetoplast' OR cvterm.name = 'reference_genome' OR cvterm.name = 'variant_genome' OR cvterm.name = 'chromosomally_aberrant_genome' OR cvterm.name = 'genome';

--- ************************************************
--- *** relation: so_genotype ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A genotype is a variant genome, complete ***
--- ***  or incomplete.                          ***
--- ************************************************
---

CREATE VIEW so_genotype AS
  SELECT
    feature_id AS so_genotype_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'genotype';

--- ************************************************
--- *** relation: diplotype ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A diplotype is a pair of haplotypes from ***
--- ***  a given individual. It is a genotype wh ***
--- *** ere the phase is known.                  ***
--- ************************************************
---

CREATE VIEW diplotype AS
  SELECT
    feature_id AS diplotype_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'diplotype';

--- ************************************************
--- *** relation: direction_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW direction_attribute AS
  SELECT
    feature_id AS direction_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'forward' OR cvterm.name = 'reverse' OR cvterm.name = 'direction_attribute';

--- ************************************************
--- *** relation: forward ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Forward is an attribute of the feature,  ***
--- *** where the feature is in the 5' to 3' dir ***
--- *** ection.                                  ***
--- ************************************************
---

CREATE VIEW forward AS
  SELECT
    feature_id AS forward_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'forward';

--- ************************************************
--- *** relation: reverse ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Reverse is an attribute of the feature,  ***
--- *** where the feature is in the 3' to 5' dir ***
--- *** ection. Again could be applied to primer ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW reverse AS
  SELECT
    feature_id AS reverse_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'reverse';

--- ************************************************
--- *** relation: mitochondrial_dna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW mitochondrial_dna AS
  SELECT
    feature_id AS mitochondrial_dna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mitochondrial_DNA';

--- ************************************************
--- *** relation: chloroplast_dna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW chloroplast_dna AS
  SELECT
    feature_id AS chloroplast_dna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chloroplast_DNA';

--- ************************************************
--- *** relation: mirtron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A de-branched intron which mimics the st ***
--- *** ructure of pre-miRNA and enters the miRN ***
--- *** A processing pathway without Drosha medi ***
--- *** ated cleavage.                           ***
--- ************************************************
---

CREATE VIEW mirtron AS
  SELECT
    feature_id AS mirtron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mirtron';

--- ************************************************
--- *** relation: pirna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A small non coding RNA, part of a silenc ***
--- *** ing system that prevents the spreading o ***
--- *** f selfish genetic elements.              ***
--- ************************************************
---

CREATE VIEW pirna AS
  SELECT
    feature_id AS pirna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'piRNA';

--- ************************************************
--- *** relation: arginyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has an arginine ant ***
--- *** icodon, and a 3' arginine binding region ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW arginyl_trna AS
  SELECT
    feature_id AS arginyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'arginyl_tRNA';

--- ************************************************
--- *** relation: mobile_genetic_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A nucleotide region with either intra-ge ***
--- *** nome or intracellular moblity, of varyin ***
--- *** g length, which often carry the informat ***
--- *** ion necessary for transfer and recombina ***
--- *** tion with the host genome.               ***
--- ************************************************
---

CREATE VIEW mobile_genetic_element AS
  SELECT
    feature_id AS mobile_genetic_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mobile_intron' OR cvterm.name = 'extrachromosomal_mobile_genetic_element' OR cvterm.name = 'integrated_mobile_genetic_element' OR cvterm.name = 'natural_transposable_element' OR cvterm.name = 'viral_sequence' OR cvterm.name = 'natural_plasmid' OR cvterm.name = 'phage_sequence' OR cvterm.name = 'ds_RNA_viral_sequence' OR cvterm.name = 'ds_DNA_viral_sequence' OR cvterm.name = 'ss_RNA_viral_sequence' OR cvterm.name = 'negative_sense_ssRNA_viral_sequence' OR cvterm.name = 'positive_sense_ssRNA_viral_sequence' OR cvterm.name = 'ambisense_ssRNA_viral_sequence' OR cvterm.name = 'transposable_element' OR cvterm.name = 'proviral_region' OR cvterm.name = 'integron' OR cvterm.name = 'genomic_island' OR cvterm.name = 'integrated_plasmid' OR cvterm.name = 'cointegrated_plasmid' OR cvterm.name = 'retrotransposon' OR cvterm.name = 'DNA_transposon' OR cvterm.name = 'foreign_transposable_element' OR cvterm.name = 'transgenic_transposable_element' OR cvterm.name = 'natural_transposable_element' OR cvterm.name = 'engineered_transposable_element' OR cvterm.name = 'nested_transposon' OR cvterm.name = 'LTR_retrotransposon' OR cvterm.name = 'non_LTR_retrotransposon' OR cvterm.name = 'LINE_element' OR cvterm.name = 'SINE_element' OR cvterm.name = 'terminal_inverted_repeat_element' OR cvterm.name = 'foldback_element' OR cvterm.name = 'conjugative_transposon' OR cvterm.name = 'helitron' OR cvterm.name = 'p_element' OR cvterm.name = 'MITE' OR cvterm.name = 'insertion_sequence' OR cvterm.name = 'polinton' OR cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'prophage' OR cvterm.name = 'pathogenic_island' OR cvterm.name = 'metabolic_island' OR cvterm.name = 'adaptive_island' OR cvterm.name = 'symbiosis_island' OR cvterm.name = 'cryptic_prophage' OR cvterm.name = 'defective_conjugative_transposon' OR cvterm.name = 'mobile_genetic_element';

--- ************************************************
--- *** relation: extrachromosomal_mobile_genetic_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An MGE that is not integrated into the h ***
--- *** ost chromosome.                          ***
--- ************************************************
---

CREATE VIEW extrachromosomal_mobile_genetic_element AS
  SELECT
    feature_id AS extrachromosomal_mobile_genetic_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'natural_transposable_element' OR cvterm.name = 'viral_sequence' OR cvterm.name = 'natural_plasmid' OR cvterm.name = 'phage_sequence' OR cvterm.name = 'ds_RNA_viral_sequence' OR cvterm.name = 'ds_DNA_viral_sequence' OR cvterm.name = 'ss_RNA_viral_sequence' OR cvterm.name = 'negative_sense_ssRNA_viral_sequence' OR cvterm.name = 'positive_sense_ssRNA_viral_sequence' OR cvterm.name = 'ambisense_ssRNA_viral_sequence' OR cvterm.name = 'extrachromosomal_mobile_genetic_element';

--- ************************************************
--- *** relation: integrated_mobile_genetic_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An MGE that is integrated into the host  ***
--- *** chromosome.                              ***
--- ************************************************
---

CREATE VIEW integrated_mobile_genetic_element AS
  SELECT
    feature_id AS integrated_mobile_genetic_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transposable_element' OR cvterm.name = 'proviral_region' OR cvterm.name = 'integron' OR cvterm.name = 'genomic_island' OR cvterm.name = 'integrated_plasmid' OR cvterm.name = 'cointegrated_plasmid' OR cvterm.name = 'retrotransposon' OR cvterm.name = 'DNA_transposon' OR cvterm.name = 'foreign_transposable_element' OR cvterm.name = 'transgenic_transposable_element' OR cvterm.name = 'natural_transposable_element' OR cvterm.name = 'engineered_transposable_element' OR cvterm.name = 'nested_transposon' OR cvterm.name = 'LTR_retrotransposon' OR cvterm.name = 'non_LTR_retrotransposon' OR cvterm.name = 'LINE_element' OR cvterm.name = 'SINE_element' OR cvterm.name = 'terminal_inverted_repeat_element' OR cvterm.name = 'foldback_element' OR cvterm.name = 'conjugative_transposon' OR cvterm.name = 'helitron' OR cvterm.name = 'p_element' OR cvterm.name = 'MITE' OR cvterm.name = 'insertion_sequence' OR cvterm.name = 'polinton' OR cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'prophage' OR cvterm.name = 'pathogenic_island' OR cvterm.name = 'metabolic_island' OR cvterm.name = 'adaptive_island' OR cvterm.name = 'symbiosis_island' OR cvterm.name = 'cryptic_prophage' OR cvterm.name = 'defective_conjugative_transposon' OR cvterm.name = 'integrated_mobile_genetic_element';

--- ************************************************
--- *** relation: integrated_plasmid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A plasmid sequence that is integrated wi ***
--- *** thin the host chromosome.                ***
--- ************************************************
---

CREATE VIEW integrated_plasmid AS
  SELECT
    feature_id AS integrated_plasmid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'integrated_plasmid';

--- ************************************************
--- *** relation: viral_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The region of nucleotide sequence of a v ***
--- *** irus, a submicroscopic particle that rep ***
--- *** licates by infecting a host cell.        ***
--- ************************************************
---

CREATE VIEW viral_sequence AS
  SELECT
    feature_id AS viral_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'phage_sequence' OR cvterm.name = 'ds_RNA_viral_sequence' OR cvterm.name = 'ds_DNA_viral_sequence' OR cvterm.name = 'ss_RNA_viral_sequence' OR cvterm.name = 'negative_sense_ssRNA_viral_sequence' OR cvterm.name = 'positive_sense_ssRNA_viral_sequence' OR cvterm.name = 'ambisense_ssRNA_viral_sequence' OR cvterm.name = 'viral_sequence';

--- ************************************************
--- *** relation: phage_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The nucleotide sequence of a virus that  ***
--- *** infects bacteria.                        ***
--- ************************************************
---

CREATE VIEW phage_sequence AS
  SELECT
    feature_id AS phage_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'phage_sequence';

--- ************************************************
--- *** relation: attctn_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attachment site located on a conjugat ***
--- *** ive transposon and used for site-specifi ***
--- *** c integration of a conjugative transposo ***
--- *** n.                                       ***
--- ************************************************
---

CREATE VIEW attctn_site AS
  SELECT
    feature_id AS attctn_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'attCtn_site';

--- ************************************************
--- *** relation: nuclear_mt_pseudogene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A nuclear pseudogene of either coding or ***
--- ***  non-coding mitochondria derived sequenc ***
--- *** e.                                       ***
--- ************************************************
---

CREATE VIEW nuclear_mt_pseudogene AS
  SELECT
    feature_id AS nuclear_mt_pseudogene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nuclear_mt_pseudogene';

--- ************************************************
--- *** relation: cointegrated_plasmid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A MGE region consisting of two fused pla ***
--- *** smids resulting from a replicative trans ***
--- *** position event.                          ***
--- ************************************************
---

CREATE VIEW cointegrated_plasmid AS
  SELECT
    feature_id AS cointegrated_plasmid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cointegrated_plasmid';

--- ************************************************
--- *** relation: irlinv_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Component of the inversion site located  ***
--- *** at the left of a region susceptible to s ***
--- *** ite-specific inversion.                  ***
--- ************************************************
---

CREATE VIEW irlinv_site AS
  SELECT
    feature_id AS irlinv_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'IRLinv_site';

--- ************************************************
--- *** relation: irrinv_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Component of the inversion site located  ***
--- *** at the right of a region susceptible to  ***
--- *** site-specific inversion.                 ***
--- ************************************************
---

CREATE VIEW irrinv_site AS
  SELECT
    feature_id AS irrinv_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'IRRinv_site';

--- ************************************************
--- *** relation: inversion_site_part ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region located within an inversion sit ***
--- *** e.                                       ***
--- ************************************************
---

CREATE VIEW inversion_site_part AS
  SELECT
    feature_id AS inversion_site_part_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'IRLinv_site' OR cvterm.name = 'IRRinv_site' OR cvterm.name = 'inversion_site_part';

--- ************************************************
--- *** relation: defective_conjugative_transposon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An island that contains genes for integr ***
--- *** ation/excision and the gene and site for ***
--- ***  the initiation of intercellular transfe ***
--- *** r by conjugation. It can be complemented ***
--- ***  for transfer by a conjugative transposo ***
--- *** n.                                       ***
--- ************************************************
---

CREATE VIEW defective_conjugative_transposon AS
  SELECT
    feature_id AS defective_conjugative_transposon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'defective_conjugative_transposon';

--- ************************************************
--- *** relation: repeat_fragment ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A portion of a repeat, interrupted by th ***
--- *** e insertion of another element.          ***
--- ************************************************
---

CREATE VIEW repeat_fragment AS
  SELECT
    feature_id AS repeat_fragment_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'repeat_fragment';

--- ************************************************
--- *** relation: transposon_fragment ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A portion of a transposon, interrupted b ***
--- *** y the insertion of another element.      ***
--- ************************************************
---

CREATE VIEW transposon_fragment AS
  SELECT
    feature_id AS transposon_fragment_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transposon_fragment';

--- ************************************************
--- *** relation: transcriptional_cis_regulatory_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A regulatory_region that modulates the t ***
--- *** ranscription of a gene or genes.         ***
--- ************************************************
---

CREATE VIEW transcriptional_cis_regulatory_region AS
  SELECT
    feature_id AS transcriptional_cis_regulatory_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'promoter' OR cvterm.name = 'insulator' OR cvterm.name = 'CRM' OR cvterm.name = 'promoter_targeting_sequence' OR cvterm.name = 'ISRE' OR cvterm.name = 'bidirectional_promoter' OR cvterm.name = 'RNA_polymerase_promoter' OR cvterm.name = 'RNApol_I_promoter' OR cvterm.name = 'RNApol_II_promoter' OR cvterm.name = 'RNApol_III_promoter' OR cvterm.name = 'bacterial_RNApol_promoter' OR cvterm.name = 'Phage_RNA_Polymerase_Promoter' OR cvterm.name = 'RNApol_II_core_promoter' OR cvterm.name = 'RNApol_III_promoter_type_1' OR cvterm.name = 'RNApol_III_promoter_type_2' OR cvterm.name = 'RNApol_III_promoter_type_3' OR cvterm.name = 'bacterial_RNApol_promoter_sigma_70' OR cvterm.name = 'bacterial_RNApol_promoter_sigma54' OR cvterm.name = 'SP6_RNA_Polymerase_Promoter' OR cvterm.name = 'T3_RNA_Polymerase_Promoter' OR cvterm.name = 'T7_RNA_Polymerase_Promoter' OR cvterm.name = 'locus_control_region' OR cvterm.name = 'enhancer' OR cvterm.name = 'silencer' OR cvterm.name = 'enhancer_bound_by_factor' OR cvterm.name = 'shadow_enhancer' OR cvterm.name = 'transcriptional_cis_regulatory_region';

--- ************************************************
--- *** relation: splicing_regulatory_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A regulatory_region that modulates splic ***
--- *** ing.                                     ***
--- ************************************************
---

CREATE VIEW splicing_regulatory_region AS
  SELECT
    feature_id AS splicing_regulatory_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'splice_enhancer' OR cvterm.name = 'intronic_splice_enhancer' OR cvterm.name = 'exonic_splice_enhancer' OR cvterm.name = 'splicing_regulatory_region';

--- ************************************************
--- *** relation: promoter_targeting_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcriptional_cis_regulatory_region  ***
--- *** that restricts the activity of a CRM to  ***
--- *** a single promoter and which functions on ***
--- *** ly when both itself and an insulator are ***
--- ***  located between the CRM and the promote ***
--- *** r.                                       ***
--- ************************************************
---

CREATE VIEW promoter_targeting_sequence AS
  SELECT
    feature_id AS promoter_targeting_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'promoter_targeting_sequence';

--- ************************************************
--- *** relation: sequence_alteration ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence_alteration is a sequence_feat ***
--- *** ure whose extent is the deviation from a ***
--- *** nother sequence.                         ***
--- ************************************************
---

CREATE VIEW sequence_alteration AS
  SELECT
    feature_id AS sequence_alteration_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'deletion' OR cvterm.name = 'translocation' OR cvterm.name = 'insertion' OR cvterm.name = 'copy_number_variation' OR cvterm.name = 'UPD' OR cvterm.name = 'structural_alteration' OR cvterm.name = 'substitution' OR cvterm.name = 'indel' OR cvterm.name = 'inversion' OR cvterm.name = 'transgenic_insertion' OR cvterm.name = 'duplication' OR cvterm.name = 'tandem_duplication' OR cvterm.name = 'direct_tandem_duplication' OR cvterm.name = 'inverted_tandem_duplication' OR cvterm.name = 'copy_number_gain' OR cvterm.name = 'copy_number_loss' OR cvterm.name = 'maternal_uniparental_disomy' OR cvterm.name = 'paternal_uniparental_disomy' OR cvterm.name = 'complex_structural_alteration' OR cvterm.name = 'sequence_length_variation' OR cvterm.name = 'MNP' OR cvterm.name = 'SNV' OR cvterm.name = 'complex_substitution' OR cvterm.name = 'simple_sequence_length_variation' OR cvterm.name = 'SNP' OR cvterm.name = 'point_mutation' OR cvterm.name = 'transition' OR cvterm.name = 'transversion' OR cvterm.name = 'pyrimidine_transition' OR cvterm.name = 'purine_transition' OR cvterm.name = 'C_to_T_transition' OR cvterm.name = 'T_to_C_transition' OR cvterm.name = 'C_to_T_transition_at_pCpG_site' OR cvterm.name = 'A_to_G_transition' OR cvterm.name = 'G_to_A_transition' OR cvterm.name = 'pyrimidine_to_purine_transversion' OR cvterm.name = 'purine_to_pyrimidine_transversion' OR cvterm.name = 'C_to_A_transversion' OR cvterm.name = 'C_to_G_transversion' OR cvterm.name = 'T_to_A_transversion' OR cvterm.name = 'T_to_G_transversion' OR cvterm.name = 'A_to_C_transversion' OR cvterm.name = 'A_to_T_transversion' OR cvterm.name = 'G_to_C_transversion' OR cvterm.name = 'G_to_T_transversion' OR cvterm.name = 'sequence_alteration';

--- ************************************************
--- *** relation: sequence_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence_variant is a non exact copy o ***
--- *** f a sequence_feature or genome exhibitin ***
--- *** g one or more sequence_alteration.       ***
--- ************************************************
---

CREATE VIEW sequence_variant AS
  SELECT
    feature_id AS sequence_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'functional_variant' OR cvterm.name = 'structural_variant' OR cvterm.name = 'loss_of_heterozygosity' OR cvterm.name = 'transcript_function_variant' OR cvterm.name = 'translational_product_function_variant' OR cvterm.name = 'level_of_transcript_variant' OR cvterm.name = 'transcript_processing_variant' OR cvterm.name = 'transcript_stability_variant' OR cvterm.name = 'transcription_variant' OR cvterm.name = 'decreased_transcript_level_variant' OR cvterm.name = 'increased_transcript_level_variant' OR cvterm.name = 'editing_variant' OR cvterm.name = 'polyadenylation_variant' OR cvterm.name = 'increased_polyadenylation_variant' OR cvterm.name = 'decreased_polyadenylation_variant' OR cvterm.name = 'decreased_transcript_stability_variant' OR cvterm.name = 'increased_transcript_stability_variant' OR cvterm.name = 'rate_of_transcription_variant' OR cvterm.name = 'increased_transcription_rate_variant' OR cvterm.name = 'decreased_transcription_rate_variant' OR cvterm.name = 'translational_product_level_variant' OR cvterm.name = 'polypeptide_function_variant' OR cvterm.name = 'decreased_translational_product_level' OR cvterm.name = 'increased_translational_product_level' OR cvterm.name = 'polypeptide_gain_of_function_variant' OR cvterm.name = 'polypeptide_localization_variant' OR cvterm.name = 'polypeptide_loss_of_function_variant' OR cvterm.name = 'polypeptide_post_translational_processing_variant' OR cvterm.name = 'inactive_ligand_binding_site' OR cvterm.name = 'polypeptide_partial_loss_of_function' OR cvterm.name = 'inactive_catalytic_site' OR cvterm.name = 'silent_mutation' OR cvterm.name = 'copy_number_change' OR cvterm.name = 'gene_variant' OR cvterm.name = 'regulatory_region_variant' OR cvterm.name = 'intergenic_variant' OR cvterm.name = 'upstream_gene_variant' OR cvterm.name = 'downstream_gene_variant' OR cvterm.name = 'gene_fusion' OR cvterm.name = 'splicing_variant' OR cvterm.name = 'transcript_variant' OR cvterm.name = 'translational_product_structure_variant' OR cvterm.name = 'cryptic_splice_site_variant' OR cvterm.name = 'exon_loss' OR cvterm.name = 'intron_gain' OR cvterm.name = 'splice_region_variant' OR cvterm.name = 'cryptic_splice_acceptor' OR cvterm.name = 'cryptic_splice_donor' OR cvterm.name = 'complex_change_in_transcript' OR cvterm.name = 'transcript_secondary_structure_variant' OR cvterm.name = 'nc_transcript_variant' OR cvterm.name = 'NMD_transcript_variant' OR cvterm.name = 'UTR_variant' OR cvterm.name = 'intron_variant' OR cvterm.name = 'exon_variant' OR cvterm.name = 'compensatory_transcript_secondary_structure_variant' OR cvterm.name = 'mature_miRNA_variant' OR cvterm.name = '5_prime_UTR_variant' OR cvterm.name = '3_prime_UTR_variant' OR cvterm.name = 'splice_site_variant' OR cvterm.name = 'splice_acceptor_variant' OR cvterm.name = 'splice_donor_variant' OR cvterm.name = 'splice_donor_5th_base_variant' OR cvterm.name = 'coding_sequence_variant' OR cvterm.name = 'non_coding_exon_variant' OR cvterm.name = 'codon_variant' OR cvterm.name = 'frameshift_variant' OR cvterm.name = 'inframe_variant' OR cvterm.name = 'initiator_codon_change' OR cvterm.name = 'non_synonymous_codon' OR cvterm.name = 'synonymous_codon' OR cvterm.name = 'terminal_codon_variant' OR cvterm.name = 'stop_gained' OR cvterm.name = 'missense_codon' OR cvterm.name = 'conservative_missense_codon' OR cvterm.name = 'non_conservative_missense_codon' OR cvterm.name = 'terminator_codon_variant' OR cvterm.name = 'incomplete_terminal_codon_variant' OR cvterm.name = 'stop_retained_variant' OR cvterm.name = 'stop_lost' OR cvterm.name = 'frame_restoring_variant' OR cvterm.name = 'minus_1_frameshift_variant' OR cvterm.name = 'minus_2_frameshift_variant' OR cvterm.name = 'plus_1_frameshift_variant' OR cvterm.name = 'plus_2_frameshift variant' OR cvterm.name = 'inframe_codon_gain' OR cvterm.name = 'inframe_codon_loss' OR cvterm.name = '3D_polypeptide_structure_variant' OR cvterm.name = 'complex_change_of_translational_product_variant' OR cvterm.name = 'polypeptide_sequence_variant' OR cvterm.name = 'complex_3D_structural_variant' OR cvterm.name = 'conformational_change_variant' OR cvterm.name = 'amino_acid_deletion' OR cvterm.name = 'amino_acid_insertion' OR cvterm.name = 'amino_acid_substitution' OR cvterm.name = 'elongated_polypeptide' OR cvterm.name = 'polypeptide_fusion' OR cvterm.name = 'polypeptide_truncation' OR cvterm.name = 'conservative_amino_acid_substitution' OR cvterm.name = 'non_conservative_amino_acid_substitution' OR cvterm.name = 'elongated_polypeptide_C_terminal' OR cvterm.name = 'elongated_polypeptide_N_terminal' OR cvterm.name = 'elongated_in_frame_polypeptide_C_terminal' OR cvterm.name = 'elongated_out_of_frame_polypeptide_C_terminal' OR cvterm.name = 'elongated_in_frame_polypeptide_N_terminal_elongation' OR cvterm.name = 'elongated_out_of_frame_polypeptide_N_terminal' OR cvterm.name = 'TF_binding_site_variant' OR cvterm.name = '5KB_upstream_variant' OR cvterm.name = '2KB_upstream_variant' OR cvterm.name = '5KB_downstream_variant' OR cvterm.name = '500B_downstream_variant' OR cvterm.name = 'sequence_variant';

--- ************************************************
--- *** relation: propeptide_cleavage_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The propeptide_cleavage_site is the argi ***
--- *** nine/lysine boundary on a propeptide whe ***
--- *** re cleavage occurs.                      ***
--- ************************************************
---

CREATE VIEW propeptide_cleavage_site AS
  SELECT
    feature_id AS propeptide_cleavage_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'propeptide_cleavage_site';

--- ************************************************
--- *** relation: propeptide ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Part of a peptide chain which is cleaved ***
--- ***  off during the formation of the mature  ***
--- *** protein.                                 ***
--- ************************************************
---

CREATE VIEW propeptide AS
  SELECT
    feature_id AS propeptide_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'propeptide';

--- ************************************************
--- *** relation: immature_peptide_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An immature_peptide_region is the extent ***
--- ***  of the peptide after it has been transl ***
--- *** ated and before any processing occurs.   ***
--- ************************************************
---

CREATE VIEW immature_peptide_region AS
  SELECT
    feature_id AS immature_peptide_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'immature_peptide_region';

--- ************************************************
--- *** relation: active_peptide ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Active peptides are proteins which are b ***
--- *** iologically active, released from a prec ***
--- *** ursor molecule.                          ***
--- ************************************************
---

CREATE VIEW active_peptide AS
  SELECT
    feature_id AS active_peptide_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'active_peptide';

--- ************************************************
--- *** relation: compositionally_biased_region_of_peptide ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Polypeptide region that is rich in a par ***
--- *** ticular amino acid or homopolymeric and  ***
--- *** greater than three residues in length.   ***
--- ************************************************
---

CREATE VIEW compositionally_biased_region_of_peptide AS
  SELECT
    feature_id AS compositionally_biased_region_of_peptide_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'compositionally_biased_region_of_peptide';

--- ************************************************
--- *** relation: polypeptide_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence motif is a short (up to 20 am ***
--- *** ino acids) region of biological interest ***
--- *** . Such motifs, although they are too sho ***
--- *** rt to constitute functional domains, sha ***
--- *** re sequence similarities and are conserv ***
--- *** ed in different proteins. They display a ***
--- ***  common function (protein-binding, subce ***
--- *** llular location etc.).                   ***
--- ************************************************
---

CREATE VIEW polypeptide_motif AS
  SELECT
    feature_id AS polypeptide_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'biochemical_region_of_peptide' OR cvterm.name = 'polypeptide_conserved_motif' OR cvterm.name = 'post_translationally_modified_region' OR cvterm.name = 'conformational_switch' OR cvterm.name = 'molecular_contact_region' OR cvterm.name = 'polypeptide_binding_motif' OR cvterm.name = 'polypeptide_catalytic_motif' OR cvterm.name = 'histone_modification' OR cvterm.name = 'histone_methylation_site' OR cvterm.name = 'histone_acetylation_site' OR cvterm.name = 'histone_ubiqitination_site' OR cvterm.name = 'histone_acylation_region' OR cvterm.name = 'H4K20_monomethylation_site' OR cvterm.name = 'H2BK5_monomethylation_site' OR cvterm.name = 'H3K27_methylation_site' OR cvterm.name = 'H3K36_methylation_site' OR cvterm.name = 'H3K4_methylation_site' OR cvterm.name = 'H3K79_methylation_site' OR cvterm.name = 'H3K9_methylation_site' OR cvterm.name = 'H3K27_monomethylation_site' OR cvterm.name = 'H3K27_trimethylation_site' OR cvterm.name = 'H3K27_dimethylation_site' OR cvterm.name = 'H3K36_monomethylation_site' OR cvterm.name = 'H3K36_dimethylation_site' OR cvterm.name = 'H3K36_trimethylation_site' OR cvterm.name = 'H3K4_monomethylation_site' OR cvterm.name = 'H3K4_trimethylation' OR cvterm.name = 'H3K4_dimethylation_site' OR cvterm.name = 'H3K79_monomethylation_site' OR cvterm.name = 'H3K79_dimethylation_site' OR cvterm.name = 'H3K79_trimethylation_site' OR cvterm.name = 'H3K9_trimethylation_site' OR cvterm.name = 'H3K9_monomethylation_site' OR cvterm.name = 'H3K9_dimethylation_site' OR cvterm.name = 'H3K9_acetylation_site' OR cvterm.name = 'H3K14_acetylation_site' OR cvterm.name = 'H3K18_acetylation_site' OR cvterm.name = 'H3K23_acylation site' OR cvterm.name = 'H3K27_acylation_site' OR cvterm.name = 'H4K16_acylation_site' OR cvterm.name = 'H4K5_acylation_site' OR cvterm.name = 'H4K8_acylation site' OR cvterm.name = 'H2B_ubiquitination_site' OR cvterm.name = 'H4K_acylation_region' OR cvterm.name = 'polypeptide_metal_contact' OR cvterm.name = 'protein_protein_contact' OR cvterm.name = 'polypeptide_ligand_contact' OR cvterm.name = 'polypeptide_DNA_contact' OR cvterm.name = 'polypeptide_calcium_ion_contact_site' OR cvterm.name = 'polypeptide_cobalt_ion_contact_site' OR cvterm.name = 'polypeptide_copper_ion_contact_site' OR cvterm.name = 'polypeptide_iron_ion_contact_site' OR cvterm.name = 'polypeptide_magnesium_ion_contact_site' OR cvterm.name = 'polypeptide_manganese_ion_contact_site' OR cvterm.name = 'polypeptide_molybdenum_ion_contact_site' OR cvterm.name = 'polypeptide_nickel_ion_contact_site' OR cvterm.name = 'polypeptide_tungsten_ion_contact_site' OR cvterm.name = 'polypeptide_zinc_ion_contact_site' OR cvterm.name = 'polypeptide_motif';

--- ************************************************
--- *** relation: polypeptide_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A polypeptide_repeat is a single copy of ***
--- ***  an internal sequence repetition.        ***
--- ************************************************
---

CREATE VIEW polypeptide_repeat AS
  SELECT
    feature_id AS polypeptide_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_repeat';

--- ************************************************
--- *** relation: polypeptide_structural_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Region of polypeptide with a given struc ***
--- *** tural property.                          ***
--- ************************************************
---

CREATE VIEW polypeptide_structural_region AS
  SELECT
    feature_id AS polypeptide_structural_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_domain' OR cvterm.name = 'membrane_structure' OR cvterm.name = 'extramembrane_polypeptide_region' OR cvterm.name = 'intramembrane_polypeptide_region' OR cvterm.name = 'polypeptide_secondary_structure' OR cvterm.name = 'polypeptide_structural_motif' OR cvterm.name = 'intrinsically_unstructured_polypeptide_region' OR cvterm.name = 'cytoplasmic_polypeptide_region' OR cvterm.name = 'non_cytoplasmic_polypeptide_region' OR cvterm.name = 'membrane_peptide_loop' OR cvterm.name = 'transmembrane_polypeptide_region' OR cvterm.name = 'asx_motif' OR cvterm.name = 'beta_bulge' OR cvterm.name = 'beta_bulge_loop' OR cvterm.name = 'beta_strand' OR cvterm.name = 'peptide_helix' OR cvterm.name = 'polypeptide_nest_motif' OR cvterm.name = 'schellmann_loop' OR cvterm.name = 'serine_threonine_motif' OR cvterm.name = 'serine_threonine_staple_motif' OR cvterm.name = 'polypeptide_turn_motif' OR cvterm.name = 'catmat_left_handed_three' OR cvterm.name = 'catmat_left_handed_four' OR cvterm.name = 'catmat_right_handed_three' OR cvterm.name = 'catmat_right_handed_four' OR cvterm.name = 'alpha_beta_motif' OR cvterm.name = 'peptide_coil' OR cvterm.name = 'beta_bulge_loop_five' OR cvterm.name = 'beta_bulge_loop_six' OR cvterm.name = 'antiparallel_beta_strand' OR cvterm.name = 'parallel_beta_strand' OR cvterm.name = 'left_handed_peptide_helix' OR cvterm.name = 'right_handed_peptide_helix' OR cvterm.name = 'alpha_helix' OR cvterm.name = 'pi_helix' OR cvterm.name = 'three_ten_helix' OR cvterm.name = 'polypeptide_nest_left_right_motif' OR cvterm.name = 'polypeptide_nest_right_left_motif' OR cvterm.name = 'schellmann_loop_seven' OR cvterm.name = 'schellmann_loop_six' OR cvterm.name = 'asx_turn' OR cvterm.name = 'beta_turn' OR cvterm.name = 'gamma_turn' OR cvterm.name = 'serine_threonine_turn' OR cvterm.name = 'asx_turn_left_handed_type_one' OR cvterm.name = 'asx_turn_left_handed_type_two' OR cvterm.name = 'asx_turn_right_handed_type_two' OR cvterm.name = 'asx_turn_right_handed_type_one' OR cvterm.name = 'beta_turn_left_handed_type_one' OR cvterm.name = 'beta_turn_left_handed_type_two' OR cvterm.name = 'beta_turn_right_handed_type_one' OR cvterm.name = 'beta_turn_right_handed_type_two' OR cvterm.name = 'beta_turn_type_six' OR cvterm.name = 'beta_turn_type_eight' OR cvterm.name = 'beta_turn_type_six_a' OR cvterm.name = 'beta_turn_type_six_b' OR cvterm.name = 'beta_turn_type_six_a_one' OR cvterm.name = 'beta_turn_type_six_a_two' OR cvterm.name = 'gamma_turn_classic' OR cvterm.name = 'gamma_turn_inverse' OR cvterm.name = 'st_turn_left_handed_type_one' OR cvterm.name = 'st_turn_left_handed_type_two' OR cvterm.name = 'st_turn_right_handed_type_one' OR cvterm.name = 'st_turn_right_handed_type_two' OR cvterm.name = 'coiled_coil' OR cvterm.name = 'helix_turn_helix' OR cvterm.name = 'polypeptide_structural_region';

--- ************************************************
--- *** relation: membrane_structure ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Arrangement of the polypeptide with resp ***
--- *** ect to the lipid bilayer.                ***
--- ************************************************
---

CREATE VIEW membrane_structure AS
  SELECT
    feature_id AS membrane_structure_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'membrane_structure';

--- ************************************************
--- *** relation: extramembrane_polypeptide_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Polypeptide region that is localized out ***
--- *** side of a lipid bilayer.                 ***
--- ************************************************
---

CREATE VIEW extramembrane_polypeptide_region AS
  SELECT
    feature_id AS extramembrane_polypeptide_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cytoplasmic_polypeptide_region' OR cvterm.name = 'non_cytoplasmic_polypeptide_region' OR cvterm.name = 'extramembrane_polypeptide_region';

--- ************************************************
--- *** relation: cytoplasmic_polypeptide_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Polypeptide region that is localized ins ***
--- *** ide the cytoplasm.                       ***
--- ************************************************
---

CREATE VIEW cytoplasmic_polypeptide_region AS
  SELECT
    feature_id AS cytoplasmic_polypeptide_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cytoplasmic_polypeptide_region';

--- ************************************************
--- *** relation: non_cytoplasmic_polypeptide_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Polypeptide region that is localized out ***
--- *** side of a lipid bilayer and outside of t ***
--- *** he cytoplasm.                            ***
--- ************************************************
---

CREATE VIEW non_cytoplasmic_polypeptide_region AS
  SELECT
    feature_id AS non_cytoplasmic_polypeptide_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'non_cytoplasmic_polypeptide_region';

--- ************************************************
--- *** relation: intramembrane_polypeptide_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Polypeptide region present in the lipid  ***
--- *** bilayer.                                 ***
--- ************************************************
---

CREATE VIEW intramembrane_polypeptide_region AS
  SELECT
    feature_id AS intramembrane_polypeptide_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'membrane_peptide_loop' OR cvterm.name = 'transmembrane_polypeptide_region' OR cvterm.name = 'intramembrane_polypeptide_region';

--- ************************************************
--- *** relation: membrane_peptide_loop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Polypeptide region localized within the  ***
--- *** lipid bilayer where both ends traverse t ***
--- *** he same membrane.                        ***
--- ************************************************
---

CREATE VIEW membrane_peptide_loop AS
  SELECT
    feature_id AS membrane_peptide_loop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'membrane_peptide_loop';

--- ************************************************
--- *** relation: transmembrane_polypeptide_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Polypeptide region traversing the lipid  ***
--- *** bilayer.                                 ***
--- ************************************************
---

CREATE VIEW transmembrane_polypeptide_region AS
  SELECT
    feature_id AS transmembrane_polypeptide_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transmembrane_polypeptide_region';

--- ************************************************
--- *** relation: polypeptide_secondary_structure ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of peptide with secondary struc ***
--- *** ture has hydrogen bonding along the pept ***
--- *** ide chain that causes a defined conforma ***
--- *** tion of the chain.                       ***
--- ************************************************
---

CREATE VIEW polypeptide_secondary_structure AS
  SELECT
    feature_id AS polypeptide_secondary_structure_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'asx_motif' OR cvterm.name = 'beta_bulge' OR cvterm.name = 'beta_bulge_loop' OR cvterm.name = 'beta_strand' OR cvterm.name = 'peptide_helix' OR cvterm.name = 'polypeptide_nest_motif' OR cvterm.name = 'schellmann_loop' OR cvterm.name = 'serine_threonine_motif' OR cvterm.name = 'serine_threonine_staple_motif' OR cvterm.name = 'polypeptide_turn_motif' OR cvterm.name = 'catmat_left_handed_three' OR cvterm.name = 'catmat_left_handed_four' OR cvterm.name = 'catmat_right_handed_three' OR cvterm.name = 'catmat_right_handed_four' OR cvterm.name = 'alpha_beta_motif' OR cvterm.name = 'peptide_coil' OR cvterm.name = 'beta_bulge_loop_five' OR cvterm.name = 'beta_bulge_loop_six' OR cvterm.name = 'antiparallel_beta_strand' OR cvterm.name = 'parallel_beta_strand' OR cvterm.name = 'left_handed_peptide_helix' OR cvterm.name = 'right_handed_peptide_helix' OR cvterm.name = 'alpha_helix' OR cvterm.name = 'pi_helix' OR cvterm.name = 'three_ten_helix' OR cvterm.name = 'polypeptide_nest_left_right_motif' OR cvterm.name = 'polypeptide_nest_right_left_motif' OR cvterm.name = 'schellmann_loop_seven' OR cvterm.name = 'schellmann_loop_six' OR cvterm.name = 'asx_turn' OR cvterm.name = 'beta_turn' OR cvterm.name = 'gamma_turn' OR cvterm.name = 'serine_threonine_turn' OR cvterm.name = 'asx_turn_left_handed_type_one' OR cvterm.name = 'asx_turn_left_handed_type_two' OR cvterm.name = 'asx_turn_right_handed_type_two' OR cvterm.name = 'asx_turn_right_handed_type_one' OR cvterm.name = 'beta_turn_left_handed_type_one' OR cvterm.name = 'beta_turn_left_handed_type_two' OR cvterm.name = 'beta_turn_right_handed_type_one' OR cvterm.name = 'beta_turn_right_handed_type_two' OR cvterm.name = 'beta_turn_type_six' OR cvterm.name = 'beta_turn_type_eight' OR cvterm.name = 'beta_turn_type_six_a' OR cvterm.name = 'beta_turn_type_six_b' OR cvterm.name = 'beta_turn_type_six_a_one' OR cvterm.name = 'beta_turn_type_six_a_two' OR cvterm.name = 'gamma_turn_classic' OR cvterm.name = 'gamma_turn_inverse' OR cvterm.name = 'st_turn_left_handed_type_one' OR cvterm.name = 'st_turn_left_handed_type_two' OR cvterm.name = 'st_turn_right_handed_type_one' OR cvterm.name = 'st_turn_right_handed_type_two' OR cvterm.name = 'polypeptide_secondary_structure';

--- ************************************************
--- *** relation: polypeptide_structural_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Motif is a three-dimensional structural  ***
--- *** element within the chain, which appears  ***
--- *** also in a variety of other molecules. Un ***
--- *** like a domain, a motif does not need to  ***
--- *** form a stable globular unit.             ***
--- ************************************************
---

CREATE VIEW polypeptide_structural_motif AS
  SELECT
    feature_id AS polypeptide_structural_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'coiled_coil' OR cvterm.name = 'helix_turn_helix' OR cvterm.name = 'polypeptide_structural_motif';

--- ************************************************
--- *** relation: coiled_coil ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A coiled coil is a structural motif in p ***
--- *** roteins, in which alpha-helices are coil ***
--- *** ed together like the strands of a rope.  ***
--- ************************************************
---

CREATE VIEW coiled_coil AS
  SELECT
    feature_id AS coiled_coil_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'coiled_coil';

--- ************************************************
--- *** relation: helix_turn_helix ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif comprising two helices separated ***
--- ***  by a turn.                              ***
--- ************************************************
---

CREATE VIEW helix_turn_helix AS
  SELECT
    feature_id AS helix_turn_helix_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'helix_turn_helix';

--- ************************************************
--- *** relation: polypeptide_sequencing_information ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Incompatibility in the sequence due to s ***
--- *** ome experimental problem.                ***
--- ************************************************
---

CREATE VIEW polypeptide_sequencing_information AS
  SELECT
    feature_id AS polypeptide_sequencing_information_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'non_adjacent_residues' OR cvterm.name = 'non_terminal_residue' OR cvterm.name = 'sequence_conflict' OR cvterm.name = 'sequence_uncertainty' OR cvterm.name = 'contig_collection' OR cvterm.name = 'polypeptide_sequencing_information';

--- ************************************************
--- *** relation: non_adjacent_residues ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Indicates that two consecutive residues  ***
--- *** in a fragment sequence are not consecuti ***
--- *** ve in the full-length protein and that t ***
--- *** here are a number of unsequenced residue ***
--- *** s between them.                          ***
--- ************************************************
---

CREATE VIEW non_adjacent_residues AS
  SELECT
    feature_id AS non_adjacent_residues_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'non_adjacent_residues';

--- ************************************************
--- *** relation: non_terminal_residue ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The residue at an extremity of the seque ***
--- *** nce is not the terminal residue.         ***
--- ************************************************
---

CREATE VIEW non_terminal_residue AS
  SELECT
    feature_id AS non_terminal_residue_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'non_terminal_residue';

--- ************************************************
--- *** relation: sequence_conflict ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Different sources report differing seque ***
--- *** nces.                                    ***
--- ************************************************
---

CREATE VIEW sequence_conflict AS
  SELECT
    feature_id AS sequence_conflict_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'contig_collection' OR cvterm.name = 'sequence_conflict';

--- ************************************************
--- *** relation: sequence_uncertainty ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Describes the positions in a sequence wh ***
--- *** ere the authors are unsure about the seq ***
--- *** uence assignment.                        ***
--- ************************************************
---

CREATE VIEW sequence_uncertainty AS
  SELECT
    feature_id AS sequence_uncertainty_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'sequence_uncertainty';

--- ************************************************
--- *** relation: post_translationally_modified_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region where a transformation occurs i ***
--- *** n a protein after it has been synthesize ***
--- *** d. This which may regulate, stabilize, c ***
--- *** rosslink or introduce new chemical funct ***
--- *** ionalities in the protein.               ***
--- ************************************************
---

CREATE VIEW post_translationally_modified_region AS
  SELECT
    feature_id AS post_translationally_modified_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'histone_modification' OR cvterm.name = 'histone_methylation_site' OR cvterm.name = 'histone_acetylation_site' OR cvterm.name = 'histone_ubiqitination_site' OR cvterm.name = 'histone_acylation_region' OR cvterm.name = 'H4K20_monomethylation_site' OR cvterm.name = 'H2BK5_monomethylation_site' OR cvterm.name = 'H3K27_methylation_site' OR cvterm.name = 'H3K36_methylation_site' OR cvterm.name = 'H3K4_methylation_site' OR cvterm.name = 'H3K79_methylation_site' OR cvterm.name = 'H3K9_methylation_site' OR cvterm.name = 'H3K27_monomethylation_site' OR cvterm.name = 'H3K27_trimethylation_site' OR cvterm.name = 'H3K27_dimethylation_site' OR cvterm.name = 'H3K36_monomethylation_site' OR cvterm.name = 'H3K36_dimethylation_site' OR cvterm.name = 'H3K36_trimethylation_site' OR cvterm.name = 'H3K4_monomethylation_site' OR cvterm.name = 'H3K4_trimethylation' OR cvterm.name = 'H3K4_dimethylation_site' OR cvterm.name = 'H3K79_monomethylation_site' OR cvterm.name = 'H3K79_dimethylation_site' OR cvterm.name = 'H3K79_trimethylation_site' OR cvterm.name = 'H3K9_trimethylation_site' OR cvterm.name = 'H3K9_monomethylation_site' OR cvterm.name = 'H3K9_dimethylation_site' OR cvterm.name = 'H3K9_acetylation_site' OR cvterm.name = 'H3K14_acetylation_site' OR cvterm.name = 'H3K18_acetylation_site' OR cvterm.name = 'H3K23_acylation site' OR cvterm.name = 'H3K27_acylation_site' OR cvterm.name = 'H4K16_acylation_site' OR cvterm.name = 'H4K5_acylation_site' OR cvterm.name = 'H4K8_acylation site' OR cvterm.name = 'H2B_ubiquitination_site' OR cvterm.name = 'H4K_acylation_region' OR cvterm.name = 'post_translationally_modified_region';

--- ************************************************
--- *** relation: polypeptide_metal_contact ***
--- *** relation type: VIEW                      ***

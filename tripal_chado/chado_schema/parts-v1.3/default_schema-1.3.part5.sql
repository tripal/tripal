SET search_path=so,chado,pg_catalog;
--- *** relation: cyanelle_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene from cyanelle sequence.           ***
--- ************************************************
---

CREATE VIEW cyanelle_gene AS
  SELECT
    feature_id AS cyanelle_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cyanelle_gene';

--- ************************************************
--- *** relation: leucoplast_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A plastid gene from leucoplast sequence. ***
--- ************************************************
---

CREATE VIEW leucoplast_gene AS
  SELECT
    feature_id AS leucoplast_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'leucoplast_gene';

--- ************************************************
--- *** relation: proplastid_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene from proplastid sequence.         ***
--- ************************************************
---

CREATE VIEW proplastid_gene AS
  SELECT
    feature_id AS proplastid_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'proplastid_gene';

--- ************************************************
--- *** relation: nucleomorph_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene from nucleomorph sequence.        ***
--- ************************************************
---

CREATE VIEW nucleomorph_gene AS
  SELECT
    feature_id AS nucleomorph_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nucleomorph_gene';

--- ************************************************
--- *** relation: plasmid_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene from plasmid sequence.            ***
--- ************************************************
---

CREATE VIEW plasmid_gene AS
  SELECT
    feature_id AS plasmid_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'plasmid_gene';

--- ************************************************
--- *** relation: proviral_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene from proviral sequence.           ***
--- ************************************************
---

CREATE VIEW proviral_gene AS
  SELECT
    feature_id AS proviral_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'endogenous_retroviral_gene' OR cvterm.name = 'proviral_gene';

--- ************************************************
--- *** relation: endogenous_retroviral_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A proviral gene with origin endogenous r ***
--- *** etrovirus.                               ***
--- ************************************************
---

CREATE VIEW endogenous_retroviral_gene AS
  SELECT
    feature_id AS endogenous_retroviral_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'endogenous_retroviral_gene';

--- ************************************************
--- *** relation: transposable_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transposon or insertion sequence. An e ***
--- *** lement that can insert in a variety of D ***
--- *** NA sequences.                            ***
--- ************************************************
---

CREATE VIEW transposable_element AS
  SELECT
    feature_id AS transposable_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'retrotransposon' OR cvterm.name = 'DNA_transposon' OR cvterm.name = 'foreign_transposable_element' OR cvterm.name = 'transgenic_transposable_element' OR cvterm.name = 'natural_transposable_element' OR cvterm.name = 'engineered_transposable_element' OR cvterm.name = 'nested_transposon' OR cvterm.name = 'LTR_retrotransposon' OR cvterm.name = 'non_LTR_retrotransposon' OR cvterm.name = 'LINE_element' OR cvterm.name = 'SINE_element' OR cvterm.name = 'terminal_inverted_repeat_element' OR cvterm.name = 'foldback_element' OR cvterm.name = 'conjugative_transposon' OR cvterm.name = 'helitron' OR cvterm.name = 'p_element' OR cvterm.name = 'MITE' OR cvterm.name = 'insertion_sequence' OR cvterm.name = 'polinton' OR cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'transposable_element';

--- ************************************************
--- *** relation: expressed_sequence_match ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A match to an EST or cDNA sequence.      ***
--- ************************************************
---

CREATE VIEW expressed_sequence_match AS
  SELECT
    feature_id AS expressed_sequence_match_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'EST_match' OR cvterm.name = 'cDNA_match' OR cvterm.name = 'UST_match' OR cvterm.name = 'RST_match' OR cvterm.name = 'expressed_sequence_match';

--- ************************************************
--- *** relation: clone_insert_end ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The end of the clone insert.             ***
--- ************************************************
---

CREATE VIEW clone_insert_end AS
  SELECT
    feature_id AS clone_insert_end_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'clone_insert_end';

--- ************************************************
--- *** relation: polypeptide ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence of amino acids linked by pept ***
--- *** ide bonds which may lack appreciable ter ***
--- *** tiary structure and may not be liable to ***
--- ***  irreversible denaturation.              ***
--- ************************************************
---

CREATE VIEW polypeptide AS
  SELECT
    feature_id AS polypeptide_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide';

--- ************************************************
--- *** relation: chromosome_arm ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of the chromosome between the c ***
--- *** entromere and the telomere. Human chromo ***
--- *** somes have two arms, the p arm (short) a ***
--- *** nd the q arm (long) which are separated  ***
--- *** from each other by the centromere.       ***
--- ************************************************
---

CREATE VIEW chromosome_arm AS
  SELECT
    feature_id AS chromosome_arm_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chromosome_arm';

--- ************************************************
--- *** relation: sequencing_primer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW sequencing_primer AS
  SELECT
    feature_id AS sequencing_primer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'sequencing_primer';

--- ************************************************
--- *** relation: mrna_with_frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An mRNA with a frameshift.               ***
--- ************************************************
---

CREATE VIEW mrna_with_frameshift AS
  SELECT
    feature_id AS mrna_with_frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mRNA_with_minus_1_frameshift' OR cvterm.name = 'mRNA_with_plus_1_frameshift' OR cvterm.name = 'mRNA_with_plus_2_frameshift' OR cvterm.name = 'mRNA_with_minus_2_frameshift' OR cvterm.name = 'mRNA_with_frameshift';

--- ************************************************
--- *** relation: sequence_feature ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An extent of biological sequence.        ***
--- ************************************************
---

CREATE VIEW sequence_feature AS
  SELECT
    feature_id AS sequence_feature_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'region' OR cvterm.name = 'junction' OR cvterm.name = 'sequence_alteration' OR cvterm.name = 'biomaterial_region' OR cvterm.name = 'experimental_feature' OR cvterm.name = 'biological_region' OR cvterm.name = 'topologically_defined_region' OR cvterm.name = 'reagent' OR cvterm.name = 'engineered_region' OR cvterm.name = 'PCR_product' OR cvterm.name = 'clone' OR cvterm.name = 'rescue_region' OR cvterm.name = 'oligo' OR cvterm.name = 'clone_insert' OR cvterm.name = 'cloned_region' OR cvterm.name = 'databank_entry' OR cvterm.name = 'RAPD' OR cvterm.name = 'genomic_clone' OR cvterm.name = 'cDNA_clone' OR cvterm.name = 'tiling_path_clone' OR cvterm.name = 'validated_cDNA_clone' OR cvterm.name = 'invalidated_cDNA_clone' OR cvterm.name = 'three_prime_RACE_clone' OR cvterm.name = 'chimeric_cDNA_clone' OR cvterm.name = 'genomically_contaminated_cDNA_clone' OR cvterm.name = 'polyA_primed_cDNA_clone' OR cvterm.name = 'partially_processed_cDNA_clone' OR cvterm.name = 'engineered_rescue_region' OR cvterm.name = 'aptamer' OR cvterm.name = 'probe' OR cvterm.name = 'tag' OR cvterm.name = 'ss_oligo' OR cvterm.name = 'ds_oligo' OR cvterm.name = 'DNAzyme' OR cvterm.name = 'synthetic_oligo' OR cvterm.name = 'DNA_aptamer' OR cvterm.name = 'RNA_aptamer' OR cvterm.name = 'microarray_oligo' OR cvterm.name = 'SAGE_tag' OR cvterm.name = 'STS' OR cvterm.name = 'EST' OR cvterm.name = 'engineered_tag' OR cvterm.name = 'five_prime_EST' OR cvterm.name = 'three_prime_EST' OR cvterm.name = 'UST' OR cvterm.name = 'RST' OR cvterm.name = 'three_prime_UST' OR cvterm.name = 'five_prime_UST' OR cvterm.name = 'three_prime_RST' OR cvterm.name = 'five_prime_RST' OR cvterm.name = 'primer' OR cvterm.name = 'sequencing_primer' OR cvterm.name = 'forward_primer' OR cvterm.name = 'reverse_primer' OR cvterm.name = 'ASPE_primer' OR cvterm.name = 'dCAPS_primer' OR cvterm.name = 'RNAi_reagent' OR cvterm.name = 'DNA_constraint_sequence' OR cvterm.name = 'morpholino_oligo' OR cvterm.name = 'PNA_oligo' OR cvterm.name = 'LNA_oligo' OR cvterm.name = 'TNA_oligo' OR cvterm.name = 'GNA_oligo' OR cvterm.name = 'R_GNA_oligo' OR cvterm.name = 'S_GNA_oligo' OR cvterm.name = 'cloned_cDNA_insert' OR cvterm.name = 'cloned_genomic_insert' OR cvterm.name = 'engineered_insert' OR cvterm.name = 'BAC_cloned_genomic_insert' OR cvterm.name = 'engineered_gene' OR cvterm.name = 'engineered_plasmid' OR cvterm.name = 'engineered_rescue_region' OR cvterm.name = 'engineered_transposable_element' OR cvterm.name = 'engineered_foreign_region' OR cvterm.name = 'engineered_tag' OR cvterm.name = 'engineered_insert' OR cvterm.name = 'targeting_vector' OR cvterm.name = 'engineered_foreign_gene' OR cvterm.name = 'engineered_fusion_gene' OR cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'engineered_episome' OR cvterm.name = 'gene_trap_construct' OR cvterm.name = 'promoter_trap_construct' OR cvterm.name = 'enhancer_trap_construct' OR cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'engineered_foreign_gene' OR cvterm.name = 'engineered_foreign_repetitive_element' OR cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'match_part' OR cvterm.name = 'assembly_component' OR cvterm.name = 'conserved_region' OR cvterm.name = 'match' OR cvterm.name = 'remark' OR cvterm.name = 'reading_frame' OR cvterm.name = 'consensus_region' OR cvterm.name = 'low_complexity_region' OR cvterm.name = 'assembly' OR cvterm.name = 'transcribed_fragment' OR cvterm.name = 'transcribed_cluster' OR cvterm.name = 'high_identity_region' OR cvterm.name = 'mathematically_defined_repeat' OR cvterm.name = 'experimentally_defined_binding_region' OR cvterm.name = 'contig' OR cvterm.name = 'read' OR cvterm.name = 'restriction_fragment' OR cvterm.name = 'golden_path_fragment' OR cvterm.name = 'tiling_path_fragment' OR cvterm.name = 'gap' OR cvterm.name = 'sonicate_fragment' OR cvterm.name = 'paired_end_fragment' OR cvterm.name = 'read_pair' OR cvterm.name = 'contig_read' OR cvterm.name = 'BAC_end' OR cvterm.name = 'dye_terminator_read' OR cvterm.name = 'pyrosequenced_read' OR cvterm.name = 'ligation_based_read' OR cvterm.name = 'polymerase_synthesis_read' OR cvterm.name = 'PAC_end' OR cvterm.name = 'YAC_end' OR cvterm.name = 'clone_end' OR cvterm.name = 'RFLP_fragment' OR cvterm.name = 'tiling_path_clone' OR cvterm.name = 'coding_conserved_region' OR cvterm.name = 'nc_conserved_region' OR cvterm.name = 'RR_tract' OR cvterm.name = 'homologous_region' OR cvterm.name = 'centromere_DNA_Element_I' OR cvterm.name = 'centromere_DNA_Element_II' OR cvterm.name = 'centromere_DNA_Element_III' OR cvterm.name = 'X_element' OR cvterm.name = 'U_box' OR cvterm.name = 'regional_centromere_central_core' OR cvterm.name = 'syntenic_region' OR cvterm.name = 'paralogous_region' OR cvterm.name = 'orthologous_region' OR cvterm.name = 'nucleotide_match' OR cvterm.name = 'protein_match' OR cvterm.name = 'expressed_sequence_match' OR cvterm.name = 'cross_genome_match' OR cvterm.name = 'translated_nucleotide_match' OR cvterm.name = 'primer_match' OR cvterm.name = 'EST_match' OR cvterm.name = 'cDNA_match' OR cvterm.name = 'UST_match' OR cvterm.name = 'RST_match' OR cvterm.name = 'sequence_difference' OR cvterm.name = 'experimental_result_region' OR cvterm.name = 'polypeptide_sequencing_information' OR cvterm.name = 'possible_base_call_error' OR cvterm.name = 'possible_assembly_error' OR cvterm.name = 'assembly_error_correction' OR cvterm.name = 'base_call_error_correction' OR cvterm.name = 'overlapping_feature_set' OR cvterm.name = 'no_output' OR cvterm.name = 'overlapping_EST_set' OR cvterm.name = 'non_adjacent_residues' OR cvterm.name = 'non_terminal_residue' OR cvterm.name = 'sequence_conflict' OR cvterm.name = 'sequence_uncertainty' OR cvterm.name = 'contig_collection' OR cvterm.name = 'ORF' OR cvterm.name = 'blocked_reading_frame' OR cvterm.name = 'mini_gene' OR cvterm.name = 'rescue_mini_gene' OR cvterm.name = 'consensus_mRNA' OR cvterm.name = 'sequence_assembly' OR cvterm.name = 'fragment_assembly' OR cvterm.name = 'supercontig' OR cvterm.name = 'contig' OR cvterm.name = 'tiling_path' OR cvterm.name = 'virtual_sequence' OR cvterm.name = 'golden_path' OR cvterm.name = 'ultracontig' OR cvterm.name = 'expressed_sequence_assembly' OR cvterm.name = 'fingerprint_map' OR cvterm.name = 'STS_map' OR cvterm.name = 'RH_map' OR cvterm.name = 'unigene_cluster' OR cvterm.name = 'CHiP_seq_region' OR cvterm.name = 'sequence_secondary_structure' OR cvterm.name = 'linkage_group' OR cvterm.name = 'polypeptide' OR cvterm.name = 'deletion' OR cvterm.name = 'origin_of_replication' OR cvterm.name = 'recombination_feature' OR cvterm.name = 'CpG_island' OR cvterm.name = 'pseudogene' OR cvterm.name = 'binding_site' OR cvterm.name = 'pseudogenic_region' OR cvterm.name = 'cap' OR cvterm.name = 'intergenic_region' OR cvterm.name = 'oligo_U_tail' OR cvterm.name = 'polyA_sequence' OR cvterm.name = 'repeat_region' OR cvterm.name = 'insertion' OR cvterm.name = 'gene' OR cvterm.name = 'repeat_unit' OR cvterm.name = 'QTL' OR cvterm.name = 'chromosome_part' OR cvterm.name = 'gene_member_region' OR cvterm.name = 'transcript_region' OR cvterm.name = 'polypeptide_region' OR cvterm.name = 'gene_component_region' OR cvterm.name = 'mobile_genetic_element' OR cvterm.name = 'replicon' OR cvterm.name = 'base' OR cvterm.name = 'amino_acid' OR cvterm.name = 'genetic_marker' OR cvterm.name = 'sequence_motif' OR cvterm.name = 'restriction_enzyme_recognition_site' OR cvterm.name = 'restriction_enzyme_single_strand_overhang' OR cvterm.name = 'epigenetically_modified_region' OR cvterm.name = 'open_chromatin_region' OR cvterm.name = 'gene_group' OR cvterm.name = 'substitution' OR cvterm.name = 'inversion' OR cvterm.name = 'retron' OR cvterm.name = 'G_quartet' OR cvterm.name = 'base_pair' OR cvterm.name = 'RNA_sequence_secondary_structure' OR cvterm.name = 'DNA_sequence_secondary_structure' OR cvterm.name = 'pseudoknot' OR cvterm.name = 'WC_base_pair' OR cvterm.name = 'sugar_edge_base_pair' OR cvterm.name = 'Hoogsteen_base_pair' OR cvterm.name = 'reverse_Hoogsteen_base_pair' OR cvterm.name = 'wobble_base_pair' OR cvterm.name = 'stem_loop' OR cvterm.name = 'tetraloop' OR cvterm.name = 'i_motif' OR cvterm.name = 'recoding_pseudoknot' OR cvterm.name = 'H_pseudoknot' OR cvterm.name = 'D_loop' OR cvterm.name = 'ARS' OR cvterm.name = 'oriT' OR cvterm.name = 'amplification_origin' OR cvterm.name = 'oriV' OR cvterm.name = 'oriC' OR cvterm.name = 'recombination_hotspot' OR cvterm.name = 'haplotype_block' OR cvterm.name = 'sequence_rearrangement_feature' OR cvterm.name = 'iDNA' OR cvterm.name = 'specific_recombination_site' OR cvterm.name = 'chromosome_breakage_sequence' OR cvterm.name = 'internal_eliminated_sequence' OR cvterm.name = 'macronucleus_destined_segment' OR cvterm.name = 'recombination_feature_of_rearranged_gene' OR cvterm.name = 'site_specific_recombination_target_region' OR cvterm.name = 'recombination_signal_sequence' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_feature' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_segment' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_gene_cluster' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_spacer' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_rearranged_segment' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_rearranged_gene_cluster' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_signal_feature' OR cvterm.name = 'D_gene' OR cvterm.name = 'V_gene' OR cvterm.name = 'J_gene' OR cvterm.name = 'C_gene' OR cvterm.name = 'D_J_C_cluster' OR cvterm.name = 'J_C_cluster' OR cvterm.name = 'J_cluster' OR cvterm.name = 'V_cluster' OR cvterm.name = 'V_J_cluster' OR cvterm.name = 'V_J_C_cluster' OR cvterm.name = 'C_cluster' OR cvterm.name = 'D_cluster' OR cvterm.name = 'D_J_cluster' OR cvterm.name = 'three_prime_D_spacer' OR cvterm.name = 'five_prime_D_spacer' OR cvterm.name = 'J_spacer' OR cvterm.name = 'V_spacer' OR cvterm.name = 'VD_gene' OR cvterm.name = 'DJ_gene' OR cvterm.name = 'VDJ_gene' OR cvterm.name = 'VJ_gene' OR cvterm.name = 'DJ_J_cluster' OR cvterm.name = 'VDJ_J_C_cluster' OR cvterm.name = 'VDJ_J_cluster' OR cvterm.name = 'VJ_C_cluster' OR cvterm.name = 'VJ_J_C_cluster' OR cvterm.name = 'VJ_J_cluster' OR cvterm.name = 'D_DJ_C_cluster' OR cvterm.name = 'D_DJ_cluster' OR cvterm.name = 'D_DJ_J_C_cluster' OR cvterm.name = 'D_DJ_J_cluster' OR cvterm.name = 'V_DJ_cluster' OR cvterm.name = 'V_DJ_J_cluster' OR cvterm.name = 'V_VDJ_C_cluster' OR cvterm.name = 'V_VDJ_cluster' OR cvterm.name = 'V_VDJ_J_cluster' OR cvterm.name = 'V_VJ_C_cluster' OR cvterm.name = 'V_VJ_cluster' OR cvterm.name = 'V_VJ_J_cluster' OR cvterm.name = 'V_D_DJ_C_cluster' OR cvterm.name = 'V_D_DJ_cluster' OR cvterm.name = 'V_D_DJ_J_C_cluster' OR cvterm.name = 'V_D_DJ_J_cluster' OR cvterm.name = 'V_D_J_C_cluster' OR cvterm.name = 'V_D_J_cluster' OR cvterm.name = 'DJ_C_cluster' OR cvterm.name = 'DJ_J_C_cluster' OR cvterm.name = 'VDJ_C_cluster' OR cvterm.name = 'V_DJ_C_cluster' OR cvterm.name = 'V_DJ_J_C_cluster' OR cvterm.name = 'V_VDJ_J_C_cluster' OR cvterm.name = 'V_VJ_J_C_cluster' OR cvterm.name = 'J_gene_recombination_feature' OR cvterm.name = 'D_gene_recombination_feature' OR cvterm.name = 'V_gene_recombination_feature' OR cvterm.name = 'heptamer_of_recombination_feature_of_vertebrate_immune_system_gene' OR cvterm.name = 'nonamer_of_recombination_feature_of_vertebrate_immune_system_gene' OR cvterm.name = 'five_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_heptamer' OR cvterm.name = 'five_prime_D_heptamer' OR cvterm.name = 'J_heptamer' OR cvterm.name = 'V_heptamer' OR cvterm.name = 'three_prime_D_nonamer' OR cvterm.name = 'five_prime_D_nonamer' OR cvterm.name = 'J_nonamer' OR cvterm.name = 'V_nonamer' OR cvterm.name = 'integration_excision_site' OR cvterm.name = 'resolution_site' OR cvterm.name = 'inversion_site' OR cvterm.name = 'inversion_site_part' OR cvterm.name = 'attI_site' OR cvterm.name = 'attP_site' OR cvterm.name = 'attB_site' OR cvterm.name = 'attL_site' OR cvterm.name = 'attR_site' OR cvterm.name = 'attC_site' OR cvterm.name = 'attCtn_site' OR cvterm.name = 'loxP_site' OR cvterm.name = 'dif_site' OR cvterm.name = 'FRT_site' OR cvterm.name = 'IRLinv_site' OR cvterm.name = 'IRRinv_site' OR cvterm.name = 'processed_pseudogene' OR cvterm.name = 'non_processed_pseudogene' OR cvterm.name = 'pseudogene_by_unequal_crossing_over' OR cvterm.name = 'nuclear_mt_pseudogene' OR cvterm.name = 'cassette_pseudogene' OR cvterm.name = 'duplicated_pseudogene' OR cvterm.name = 'unitary_pseudogene' OR cvterm.name = 'protein_binding_site' OR cvterm.name = 'epitope' OR cvterm.name = 'nucleotide_binding_site' OR cvterm.name = 'metal_binding_site' OR cvterm.name = 'ligand_binding_site' OR cvterm.name = 'protein_protein_contact' OR cvterm.name = 'nucleotide_to_protein_binding_site' OR cvterm.name = 'nuclease_binding_site' OR cvterm.name = 'TF_binding_site' OR cvterm.name = 'histone_binding_site' OR cvterm.name = 'insulator_binding_site' OR cvterm.name = 'enhancer_binding_site' OR cvterm.name = 'restriction_enzyme_binding_site' OR cvterm.name = 'nuclease_sensitive_site' OR cvterm.name = 'homing_endonuclease_binding_site' OR cvterm.name = 'nuclease_hypersensitive_site' OR cvterm.name = 'group_1_intron_homing_endonuclease_target_region' OR cvterm.name = 'DNAseI_hypersensitive_site' OR cvterm.name = 'miRNA_target_site' OR cvterm.name = 'DNA_binding_site' OR cvterm.name = 'primer_binding_site' OR cvterm.name = 'polypeptide_DNA_contact' OR cvterm.name = 'polypeptide_metal_contact' OR cvterm.name = 'polypeptide_calcium_ion_contact_site' OR cvterm.name = 'polypeptide_cobalt_ion_contact_site' OR cvterm.name = 'polypeptide_copper_ion_contact_site' OR cvterm.name = 'polypeptide_iron_ion_contact_site' OR cvterm.name = 'polypeptide_magnesium_ion_contact_site' OR cvterm.name = 'polypeptide_manganese_ion_contact_site' OR cvterm.name = 'polypeptide_molybdenum_ion_contact_site' OR cvterm.name = 'polypeptide_nickel_ion_contact_site' OR cvterm.name = 'polypeptide_tungsten_ion_contact_site' OR cvterm.name = 'polypeptide_zinc_ion_contact_site' OR cvterm.name = 'polypeptide_ligand_contact' OR cvterm.name = 'decayed_exon' OR cvterm.name = 'pseudogenic_exon' OR cvterm.name = 'pseudogenic_transcript' OR cvterm.name = 'pseudogenic_rRNA' OR cvterm.name = 'pseudogenic_tRNA' OR cvterm.name = 'long_terminal_repeat' OR cvterm.name = 'engineered_foreign_repetitive_element' OR cvterm.name = 'inverted_repeat' OR cvterm.name = 'direct_repeat' OR cvterm.name = 'non_LTR_retrotransposon_polymeric_tract' OR cvterm.name = 'dispersed_repeat' OR cvterm.name = 'tandem_repeat' OR cvterm.name = 'X_element_combinatorial_repeat' OR cvterm.name = 'Y_prime_element' OR cvterm.name = 'telomeric_repeat' OR cvterm.name = 'nested_repeat' OR cvterm.name = 'centromeric_repeat' OR cvterm.name = 'five_prime_LTR' OR cvterm.name = 'three_prime_LTR' OR cvterm.name = 'solo_LTR' OR cvterm.name = 'terminal_inverted_repeat' OR cvterm.name = 'five_prime_terminal_inverted_repeat' OR cvterm.name = 'three_prime_terminal_inverted_repeat' OR cvterm.name = 'target_site_duplication' OR cvterm.name = 'CRISPR' OR cvterm.name = 'satellite_DNA' OR cvterm.name = 'microsatellite' OR cvterm.name = 'minisatellite' OR cvterm.name = 'dinucleotide_repeat_microsatellite_feature' OR cvterm.name = 'trinucleotide_repeat_microsatellite_feature' OR cvterm.name = 'tetranucleotide_repeat_microsatellite_feature' OR cvterm.name = 'nested_tandem_repeat' OR cvterm.name = 'regional_centromere_inner_repeat_region' OR cvterm.name = 'regional_centromere_outer_repeat_region' OR cvterm.name = 'transgenic_insertion' OR cvterm.name = 'duplication' OR cvterm.name = 'tandem_duplication' OR cvterm.name = 'direct_tandem_duplication' OR cvterm.name = 'inverted_tandem_duplication' OR cvterm.name = 'nuclear_gene' OR cvterm.name = 'mt_gene' OR cvterm.name = 'plastid_gene' OR cvterm.name = 'nucleomorph_gene' OR cvterm.name = 'plasmid_gene' OR cvterm.name = 'proviral_gene' OR cvterm.name = 'transposable_element_gene' OR cvterm.name = 'silenced_gene' OR cvterm.name = 'engineered_gene' OR cvterm.name = 'foreign_gene' OR cvterm.name = 'fusion_gene' OR cvterm.name = 'recombinationally_rearranged_gene' OR cvterm.name = 'gene_with_trans_spliced_transcript' OR cvterm.name = 'gene_with_polycistronic_transcript' OR cvterm.name = 'rescue_gene' OR cvterm.name = 'post_translationally_regulated_gene' OR cvterm.name = 'negatively_autoregulated_gene' OR cvterm.name = 'positively_autoregulated_gene' OR cvterm.name = 'translationally_regulated_gene' OR cvterm.name = 'epigenetically_modified_gene' OR cvterm.name = 'transgene' OR cvterm.name = 'predicted_gene' OR cvterm.name = 'protein_coding_gene' OR cvterm.name = 'retrogene' OR cvterm.name = 'ncRNA_gene' OR cvterm.name = 'cryptic_gene' OR cvterm.name = 'gene_with_non_canonical_start_codon' OR cvterm.name = 'gene_cassette' OR cvterm.name = 'kinetoplast_gene' OR cvterm.name = 'maxicircle_gene' OR cvterm.name = 'minicircle_gene' OR cvterm.name = 'cryptogene' OR cvterm.name = 'apicoplast_gene' OR cvterm.name = 'ct_gene' OR cvterm.name = 'chromoplast_gene' OR cvterm.name = 'cyanelle_gene' OR cvterm.name = 'leucoplast_gene' OR cvterm.name = 'proplastid_gene' OR cvterm.name = 'endogenous_retroviral_gene' OR cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'gene_silenced_by_DNA_modification' OR cvterm.name = 'gene_silenced_by_RNA_interference' OR cvterm.name = 'gene_silenced_by_histone_modification' OR cvterm.name = 'gene_silenced_by_DNA_methylation' OR cvterm.name = 'gene_silenced_by_histone_methylation' OR cvterm.name = 'gene_silenced_by_histone_deacetylation' OR cvterm.name = 'engineered_foreign_gene' OR cvterm.name = 'engineered_fusion_gene' OR cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'engineered_foreign_gene' OR cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'engineered_fusion_gene' OR cvterm.name = 'recombinationally_inverted_gene' OR cvterm.name = 'recombinationally_rearranged_vertebrate_immune_system_gene' OR cvterm.name = 'gene_with_dicistronic_transcript' OR cvterm.name = 'gene_with_dicistronic_primary_transcript' OR cvterm.name = 'gene_with_dicistronic_mRNA' OR cvterm.name = 'wild_type_rescue_gene' OR cvterm.name = 'gene_rearranged_at_DNA_level' OR cvterm.name = 'maternally_imprinted_gene' OR cvterm.name = 'paternally_imprinted_gene' OR cvterm.name = 'allelically_excluded_gene' OR cvterm.name = 'floxed_gene' OR cvterm.name = 'gene_with_polyadenylated_mRNA' OR cvterm.name = 'gene_with_mRNA_with_frameshift' OR cvterm.name = 'gene_with_edited_transcript' OR cvterm.name = 'gene_with_recoded_mRNA' OR cvterm.name = 'gene_with_stop_codon_read_through' OR cvterm.name = 'gene_with_mRNA_recoded_by_translational_bypass' OR cvterm.name = 'gene_with_transcript_with_translational_frameshift' OR cvterm.name = 'gene_with_stop_codon_redefined_as_pyrrolysine' OR cvterm.name = 'gene_with_stop_codon_redefined_as_selenocysteine' OR cvterm.name = 'gRNA_gene' OR cvterm.name = 'miRNA_gene' OR cvterm.name = 'scRNA_gene' OR cvterm.name = 'snoRNA_gene' OR cvterm.name = 'snRNA_gene' OR cvterm.name = 'SRP_RNA_gene' OR cvterm.name = 'stRNA_gene' OR cvterm.name = 'tmRNA_gene' OR cvterm.name = 'tRNA_gene' OR cvterm.name = 'rRNA_gene' OR cvterm.name = 'piRNA_gene' OR cvterm.name = 'RNase_P_RNA_gene' OR cvterm.name = 'RNase_MRP_RNA_gene' OR cvterm.name = 'lincRNA_gene' OR cvterm.name = 'telomerase_RNA_gene' OR cvterm.name = 'cryptogene' OR cvterm.name = 'gene_with_start_codon_CUG' OR cvterm.name = 'chromosome_arm' OR cvterm.name = 'chromosome_band' OR cvterm.name = 'interband' OR cvterm.name = 'chromosomal_regulatory_element' OR cvterm.name = 'chromosomal_structural_element' OR cvterm.name = 'introgressed_chromosome_region' OR cvterm.name = 'matrix_attachment_site' OR cvterm.name = 'centromere' OR cvterm.name = 'telomere' OR cvterm.name = 'point_centromere' OR cvterm.name = 'regional_centromere' OR cvterm.name = 'transcript' OR cvterm.name = 'regulatory_region' OR cvterm.name = 'polycistronic_transcript' OR cvterm.name = 'transcript_with_translational_frameshift' OR cvterm.name = 'primary_transcript' OR cvterm.name = 'mature_transcript' OR cvterm.name = 'transcript_bound_by_nucleic_acid' OR cvterm.name = 'transcript_bound_by_protein' OR cvterm.name = 'enzymatic_RNA' OR cvterm.name = 'trans_spliced_transcript' OR cvterm.name = 'monocistronic_transcript' OR cvterm.name = 'aberrant_processed_transcript' OR cvterm.name = 'edited_transcript' OR cvterm.name = 'processed_transcript' OR cvterm.name = 'alternatively_spliced_transcript' OR cvterm.name = 'dicistronic_transcript' OR cvterm.name = 'polycistronic_primary_transcript' OR cvterm.name = 'polycistronic_mRNA' OR cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'dicistronic_primary_transcript' OR cvterm.name = 'dicistronic_primary_transcript' OR cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'protein_coding_primary_transcript' OR cvterm.name = 'nc_primary_transcript' OR cvterm.name = 'polycistronic_primary_transcript' OR cvterm.name = 'monocistronic_primary_transcript' OR cvterm.name = 'mini_exon_donor_RNA' OR cvterm.name = 'antisense_primary_transcript' OR cvterm.name = 'capped_primary_transcript' OR cvterm.name = 'pre_edited_mRNA' OR cvterm.name = 'scRNA_primary_transcript' OR cvterm.name = 'rRNA_primary_transcript' OR cvterm.name = 'tRNA_primary_transcript' OR cvterm.name = 'snRNA_primary_transcript' OR cvterm.name = 'snoRNA_primary_transcript' OR cvterm.name = 'tmRNA_primary_transcript' OR cvterm.name = 'SRP_RNA_primary_transcript' OR cvterm.name = 'miRNA_primary_transcript' OR cvterm.name = 'tasiRNA_primary_transcript' OR cvterm.name = 'rRNA_small_subunit_primary_transcript' OR cvterm.name = 'rRNA_large_subunit_primary_transcript' OR cvterm.name = 'alanine_tRNA_primary_transcript' OR cvterm.name = 'arginine_tRNA_primary_transcript' OR cvterm.name = 'asparagine_tRNA_primary_transcript' OR cvterm.name = 'aspartic_acid_tRNA_primary_transcript' OR cvterm.name = 'cysteine_tRNA_primary_transcript' OR cvterm.name = 'glutamic_acid_tRNA_primary_transcript' OR cvterm.name = 'glutamine_tRNA_primary_transcript' OR cvterm.name = 'glycine_tRNA_primary_transcript' OR cvterm.name = 'histidine_tRNA_primary_transcript' OR cvterm.name = 'isoleucine_tRNA_primary_transcript' OR cvterm.name = 'leucine_tRNA_primary_transcript' OR cvterm.name = 'lysine_tRNA_primary_transcript' OR cvterm.name = 'methionine_tRNA_primary_transcript' OR cvterm.name = 'phenylalanine_tRNA_primary_transcript' OR cvterm.name = 'proline_tRNA_primary_transcript' OR cvterm.name = 'serine_tRNA_primary_transcript' OR cvterm.name = 'threonine_tRNA_primary_transcript' OR cvterm.name = 'tryptophan_tRNA_primary_transcript' OR cvterm.name = 'tyrosine_tRNA_primary_transcript' OR cvterm.name = 'valine_tRNA_primary_transcript' OR cvterm.name = 'pyrrolysine_tRNA_primary_transcript' OR cvterm.name = 'selenocysteine_tRNA_primary_transcript' OR cvterm.name = 'methylation_guide_snoRNA_primary_transcript' OR cvterm.name = 'rRNA_cleavage_snoRNA_primary_transcript' OR cvterm.name = 'C_D_box_snoRNA_primary_transcript' OR cvterm.name = 'H_ACA_box_snoRNA_primary_transcript' OR cvterm.name = 'U14_snoRNA_primary_transcript' OR cvterm.name = 'stRNA_primary_transcript' OR cvterm.name = 'dicistronic_primary_transcript' OR cvterm.name = 'mRNA' OR cvterm.name = 'ncRNA' OR cvterm.name = 'mRNA_with_frameshift' OR cvterm.name = 'monocistronic_mRNA' OR cvterm.name = 'polycistronic_mRNA' OR cvterm.name = 'exemplar_mRNA' OR cvterm.name = 'capped_mRNA' OR cvterm.name = 'polyadenylated_mRNA' OR cvterm.name = 'trans_spliced_mRNA' OR cvterm.name = 'edited_mRNA' OR cvterm.name = 'consensus_mRNA' OR cvterm.name = 'recoded_mRNA' OR cvterm.name = 'mRNA_with_minus_1_frameshift' OR cvterm.name = 'mRNA_with_plus_1_frameshift' OR cvterm.name = 'mRNA_with_plus_2_frameshift' OR cvterm.name = 'mRNA_with_minus_2_frameshift' OR cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'mRNA_recoded_by_translational_bypass' OR cvterm.name = 'mRNA_recoded_by_codon_redefinition' OR cvterm.name = 'scRNA' OR cvterm.name = 'rRNA' OR cvterm.name = 'tRNA' OR cvterm.name = 'snRNA' OR cvterm.name = 'snoRNA' OR cvterm.name = 'small_regulatory_ncRNA' OR cvterm.name = 'RNase_MRP_RNA' OR cvterm.name = 'RNase_P_RNA' OR cvterm.name = 'telomerase_RNA' OR cvterm.name = 'vault_RNA' OR cvterm.name = 'Y_RNA' OR cvterm.name = 'rasiRNA' OR cvterm.name = 'SRP_RNA' OR cvterm.name = 'guide_RNA' OR cvterm.name = 'antisense_RNA' OR cvterm.name = 'siRNA' OR cvterm.name = 'stRNA' OR cvterm.name = 'class_II_RNA' OR cvterm.name = 'class_I_RNA' OR cvterm.name = 'piRNA' OR cvterm.name = 'lincRNA' OR cvterm.name = 'tasiRNA' OR cvterm.name = 'rRNA_cleavage_RNA' OR cvterm.name = 'small_subunit_rRNA' OR cvterm.name = 'large_subunit_rRNA' OR cvterm.name = 'rRNA_18S' OR cvterm.name = 'rRNA_16S' OR cvterm.name = 'rRNA_5_8S' OR cvterm.name = 'rRNA_5S' OR cvterm.name = 'rRNA_28S' OR cvterm.name = 'rRNA_23S' OR cvterm.name = 'rRNA_25S' OR cvterm.name = 'rRNA_21S' OR cvterm.name = 'alanyl_tRNA' OR cvterm.name = 'asparaginyl_tRNA' OR cvterm.name = 'aspartyl_tRNA' OR cvterm.name = 'cysteinyl_tRNA' OR cvterm.name = 'glutaminyl_tRNA' OR cvterm.name = 'glutamyl_tRNA' OR cvterm.name = 'glycyl_tRNA' OR cvterm.name = 'histidyl_tRNA' OR cvterm.name = 'isoleucyl_tRNA' OR cvterm.name = 'leucyl_tRNA' OR cvterm.name = 'lysyl_tRNA' OR cvterm.name = 'methionyl_tRNA' OR cvterm.name = 'phenylalanyl_tRNA' OR cvterm.name = 'prolyl_tRNA' OR cvterm.name = 'seryl_tRNA' OR cvterm.name = 'threonyl_tRNA' OR cvterm.name = 'tryptophanyl_tRNA' OR cvterm.name = 'tyrosyl_tRNA' OR cvterm.name = 'valyl_tRNA' OR cvterm.name = 'pyrrolysyl_tRNA' OR cvterm.name = 'arginyl_tRNA' OR cvterm.name = 'selenocysteinyl_tRNA' OR cvterm.name = 'U1_snRNA' OR cvterm.name = 'U2_snRNA' OR cvterm.name = 'U4_snRNA' OR cvterm.name = 'U4atac_snRNA' OR cvterm.name = 'U5_snRNA' OR cvterm.name = 'U6_snRNA' OR cvterm.name = 'U6atac_snRNA' OR cvterm.name = 'U11_snRNA' OR cvterm.name = 'U12_snRNA' OR cvterm.name = 'C_D_box_snoRNA' OR cvterm.name = 'H_ACA_box_snoRNA' OR cvterm.name = 'U14_snoRNA' OR cvterm.name = 'U3_snoRNA' OR cvterm.name = 'methylation_guide_snoRNA' OR cvterm.name = 'pseudouridylation_guide_snoRNA' OR cvterm.name = 'miRNA' OR cvterm.name = 'RNA_6S' OR cvterm.name = 'CsrB_RsmB_RNA' OR cvterm.name = 'DsrA_RNA' OR cvterm.name = 'OxyS_RNA' OR cvterm.name = 'RprA_RNA' OR cvterm.name = 'RRE_RNA' OR cvterm.name = 'spot_42_RNA' OR cvterm.name = 'tmRNA' OR cvterm.name = 'GcvB_RNA' OR cvterm.name = 'MicF_RNA' OR cvterm.name = 'ribozyme' OR cvterm.name = 'trans_spliced_mRNA' OR cvterm.name = 'monocistronic_primary_transcript' OR cvterm.name = 'monocistronic_mRNA' OR cvterm.name = 'edited_transcript_by_A_to_I_substitution' OR cvterm.name = 'edited_mRNA' OR cvterm.name = 'transcription_regulatory_region' OR cvterm.name = 'translation_regulatory_region' OR cvterm.name = 'recombination_regulatory_region' OR cvterm.name = 'replication_regulatory_region' OR cvterm.name = 'terminator' OR cvterm.name = 'TF_binding_site' OR cvterm.name = 'polyA_signal_sequence' OR cvterm.name = 'gene_group_regulatory_region' OR cvterm.name = 'transcriptional_cis_regulatory_region' OR cvterm.name = 'splicing_regulatory_region' OR cvterm.name = 'cis_regulatory_frameshift_element' OR cvterm.name = 'intronic_regulatory_region' OR cvterm.name = 'bacterial_terminator' OR cvterm.name = 'eukaryotic_terminator' OR cvterm.name = 'rho_dependent_bacterial_terminator' OR cvterm.name = 'rho_independent_bacterial_terminator' OR cvterm.name = 'terminator_of_type_2_RNApol_III_promoter' OR cvterm.name = 'operator' OR cvterm.name = 'bacterial_RNApol_promoter' OR cvterm.name = 'bacterial_terminator' OR cvterm.name = 'bacterial_RNApol_promoter_sigma_70' OR cvterm.name = 'bacterial_RNApol_promoter_sigma54' OR cvterm.name = 'rho_dependent_bacterial_terminator' OR cvterm.name = 'rho_independent_bacterial_terminator' OR cvterm.name = 'promoter' OR cvterm.name = 'insulator' OR cvterm.name = 'CRM' OR cvterm.name = 'promoter_targeting_sequence' OR cvterm.name = 'ISRE' OR cvterm.name = 'bidirectional_promoter' OR cvterm.name = 'RNA_polymerase_promoter' OR cvterm.name = 'RNApol_I_promoter' OR cvterm.name = 'RNApol_II_promoter' OR cvterm.name = 'RNApol_III_promoter' OR cvterm.name = 'bacterial_RNApol_promoter' OR cvterm.name = 'Phage_RNA_Polymerase_Promoter' OR cvterm.name = 'RNApol_II_core_promoter' OR cvterm.name = 'RNApol_III_promoter_type_1' OR cvterm.name = 'RNApol_III_promoter_type_2' OR cvterm.name = 'RNApol_III_promoter_type_3' OR cvterm.name = 'bacterial_RNApol_promoter_sigma_70' OR cvterm.name = 'bacterial_RNApol_promoter_sigma54' OR cvterm.name = 'SP6_RNA_Polymerase_Promoter' OR cvterm.name = 'T3_RNA_Polymerase_Promoter' OR cvterm.name = 'T7_RNA_Polymerase_Promoter' OR cvterm.name = 'locus_control_region' OR cvterm.name = 'enhancer' OR cvterm.name = 'silencer' OR cvterm.name = 'enhancer_bound_by_factor' OR cvterm.name = 'shadow_enhancer' OR cvterm.name = 'splice_enhancer' OR cvterm.name = 'intronic_splice_enhancer' OR cvterm.name = 'exonic_splice_enhancer' OR cvterm.name = 'attenuator' OR cvterm.name = 'exon' OR cvterm.name = 'edited_transcript_feature' OR cvterm.name = 'mature_transcript_region' OR cvterm.name = 'primary_transcript_region' OR cvterm.name = 'exon_region' OR cvterm.name = 'anchor_binding_site' OR cvterm.name = 'coding_exon' OR cvterm.name = 'noncoding_exon' OR cvterm.name = 'interior_exon' OR cvterm.name = 'exon_of_single_exon_gene' OR cvterm.name = 'interior_coding_exon' OR cvterm.name = 'five_prime_coding_exon' OR cvterm.name = 'three_prime_coding_exon' OR cvterm.name = 'three_prime_noncoding_exon' OR cvterm.name = 'five_prime_noncoding_exon' OR cvterm.name = 'pre_edited_region' OR cvterm.name = 'editing_block' OR cvterm.name = 'editing_domain' OR cvterm.name = 'unedited_region' OR cvterm.name = 'mRNA_region' OR cvterm.name = 'tmRNA_region' OR cvterm.name = 'guide_RNA_region' OR cvterm.name = 'tRNA_region' OR cvterm.name = 'riboswitch' OR cvterm.name = 'ribosome_entry_site' OR cvterm.name = 'UTR' OR cvterm.name = 'CDS' OR cvterm.name = 'five_prime_open_reading_frame' OR cvterm.name = 'UTR_region' OR cvterm.name = 'CDS_region' OR cvterm.name = 'translational_frameshift' OR cvterm.name = 'recoding_stimulatory_region' OR cvterm.name = 'internal_ribosome_entry_site' OR cvterm.name = 'Shine_Dalgarno_sequence' OR cvterm.name = 'kozak_sequence' OR cvterm.name = 'internal_Shine_Dalgarno_sequence' OR cvterm.name = 'five_prime_UTR' OR cvterm.name = 'three_prime_UTR' OR cvterm.name = 'internal_UTR' OR cvterm.name = 'untranslated_region_polycistronic_mRNA' OR cvterm.name = 'edited_CDS' OR cvterm.name = 'CDS_fragment' OR cvterm.name = 'CDS_independently_known' OR cvterm.name = 'CDS_predicted' OR cvterm.name = 'orphan_CDS' OR cvterm.name = 'CDS_supported_by_sequence_similarity_data' OR cvterm.name = 'CDS_supported_by_domain_match_data' OR cvterm.name = 'CDS_supported_by_EST_or_cDNA_data' OR cvterm.name = 'upstream_AUG_codon' OR cvterm.name = 'AU_rich_element' OR cvterm.name = 'Bruno_response_element' OR cvterm.name = 'iron_responsive_element' OR cvterm.name = 'coding_start' OR cvterm.name = 'coding_end' OR cvterm.name = 'codon' OR cvterm.name = 'recoded_codon' OR cvterm.name = 'start_codon' OR cvterm.name = 'stop_codon' OR cvterm.name = 'stop_codon_read_through' OR cvterm.name = 'stop_codon_redefined_as_pyrrolysine' OR cvterm.name = 'stop_codon_redefined_as_selenocysteine' OR cvterm.name = 'non_canonical_start_codon' OR cvterm.name = 'four_bp_start_codon' OR cvterm.name = 'CTG_start_codon' OR cvterm.name = 'plus_1_translational_frameshift' OR cvterm.name = 'plus_2_translational_frameshift' OR cvterm.name = 'internal_Shine_Dalgarno_sequence' OR cvterm.name = 'SECIS_element' OR cvterm.name = 'three_prime_recoding_site' OR cvterm.name = 'five_prime_recoding_site' OR cvterm.name = 'stop_codon_signal' OR cvterm.name = 'three_prime_stem_loop_structure' OR cvterm.name = 'flanking_three_prime_quadruplet_recoding_signal' OR cvterm.name = 'three_prime_repeat_recoding_signal' OR cvterm.name = 'distant_three_prime_recoding_signal' OR cvterm.name = 'UAG_stop_codon_signal' OR cvterm.name = 'UAA_stop_codon_signal' OR cvterm.name = 'UGA_stop_codon_signal' OR cvterm.name = 'tmRNA_coding_piece' OR cvterm.name = 'tmRNA_acceptor_piece' OR cvterm.name = 'anchor_region' OR cvterm.name = 'template_region' OR cvterm.name = 'anticodon_loop' OR cvterm.name = 'anticodon' OR cvterm.name = 'CCA_tail' OR cvterm.name = 'DHU_loop' OR cvterm.name = 'T_loop' OR cvterm.name = 'splice_site' OR cvterm.name = 'intron' OR cvterm.name = 'clip' OR cvterm.name = 'TSS' OR cvterm.name = 'transcription_end_site' OR cvterm.name = 'spliced_leader_RNA' OR cvterm.name = 'rRNA_primary_transcript_region' OR cvterm.name = 'spliceosomal_intron_region' OR cvterm.name = 'intron_domain' OR cvterm.name = 'miRNA_primary_transcript_region' OR cvterm.name = 'outron' OR cvterm.name = 'cis_splice_site' OR cvterm.name = 'trans_splice_site' OR cvterm.name = 'cryptic_splice_site' OR cvterm.name = 'five_prime_cis_splice_site' OR cvterm.name = 'three_prime_cis_splice_site' OR cvterm.name = 'recursive_splice_site' OR cvterm.name = 'canonical_five_prime_splice_site' OR cvterm.name = 'non_canonical_five_prime_splice_site' OR cvterm.name = 'canonical_three_prime_splice_site' OR cvterm.name = 'non_canonical_three_prime_splice_site' OR cvterm.name = 'trans_splice_acceptor_site' OR cvterm.name = 'trans_splice_donor_site' OR cvterm.name = 'SL1_acceptor_site' OR cvterm.name = 'SL2_acceptor_site' OR cvterm.name = 'SL3_acceptor_site' OR cvterm.name = 'SL4_acceptor_site' OR cvterm.name = 'SL5_acceptor_site' OR cvterm.name = 'SL6_acceptor_site' OR cvterm.name = 'SL7_acceptor_site' OR cvterm.name = 'SL8_acceptor_site' OR cvterm.name = 'SL9_acceptor_site' OR cvterm.name = 'SL10_accceptor_site' OR cvterm.name = 'SL11_acceptor_site' OR cvterm.name = 'SL12_acceptor_site' OR cvterm.name = 'five_prime_intron' OR cvterm.name = 'interior_intron' OR cvterm.name = 'three_prime_intron' OR cvterm.name = 'twintron' OR cvterm.name = 'UTR_intron' OR cvterm.name = 'autocatalytically_spliced_intron' OR cvterm.name = 'spliceosomal_intron' OR cvterm.name = 'mobile_intron' OR cvterm.name = 'endonuclease_spliced_intron' OR cvterm.name = 'five_prime_UTR_intron' OR cvterm.name = 'three_prime_UTR_intron' OR cvterm.name = 'group_I_intron' OR cvterm.name = 'group_II_intron' OR cvterm.name = 'group_III_intron' OR cvterm.name = 'group_IIA_intron' OR cvterm.name = 'group_IIB_intron' OR cvterm.name = 'U2_intron' OR cvterm.name = 'U12_intron' OR cvterm.name = 'archaeal_intron' OR cvterm.name = 'tRNA_intron' OR cvterm.name = 'five_prime_clip' OR cvterm.name = 'three_prime_clip' OR cvterm.name = 'major_TSS' OR cvterm.name = 'minor_TSS' OR cvterm.name = 'transcribed_spacer_region' OR cvterm.name = 'internal_transcribed_spacer_region' OR cvterm.name = 'external_transcribed_spacer_region' OR cvterm.name = 'intronic_splice_enhancer' OR cvterm.name = 'branch_site' OR cvterm.name = 'polypyrimidine_tract' OR cvterm.name = 'internal_guide_sequence' OR cvterm.name = 'mirtron' OR cvterm.name = 'pre_miRNA' OR cvterm.name = 'miRNA_stem' OR cvterm.name = 'miRNA_loop' OR cvterm.name = 'miRNA_antiguide' OR cvterm.name = 'noncoding_region_of_exon' OR cvterm.name = 'coding_region_of_exon' OR cvterm.name = 'three_prime_coding_exon_noncoding_region' OR cvterm.name = 'five_prime_coding_exon_noncoding_region' OR cvterm.name = 'five_prime_coding_exon_coding_region' OR cvterm.name = 'three_prime_coding_exon_coding_region' OR cvterm.name = 'mature_protein_region' OR cvterm.name = 'immature_peptide_region' OR cvterm.name = 'compositionally_biased_region_of_peptide' OR cvterm.name = 'polypeptide_structural_region' OR cvterm.name = 'polypeptide_variation_site' OR cvterm.name = 'peptide_localization_signal' OR cvterm.name = 'cleaved_peptide_region' OR cvterm.name = 'hydrophobic_region_of_peptide' OR cvterm.name = 'polypeptide_conserved_region' OR cvterm.name = 'active_peptide' OR cvterm.name = 'polypeptide_domain' OR cvterm.name = 'membrane_structure' OR cvterm.name = 'extramembrane_polypeptide_region' OR cvterm.name = 'intramembrane_polypeptide_region' OR cvterm.name = 'polypeptide_secondary_structure' OR cvterm.name = 'polypeptide_structural_motif' OR cvterm.name = 'intrinsically_unstructured_polypeptide_region' OR cvterm.name = 'cytoplasmic_polypeptide_region' OR cvterm.name = 'non_cytoplasmic_polypeptide_region' OR cvterm.name = 'membrane_peptide_loop' OR cvterm.name = 'transmembrane_polypeptide_region' OR cvterm.name = 'asx_motif' OR cvterm.name = 'beta_bulge' OR cvterm.name = 'beta_bulge_loop' OR cvterm.name = 'beta_strand' OR cvterm.name = 'peptide_helix' OR cvterm.name = 'polypeptide_nest_motif' OR cvterm.name = 'schellmann_loop' OR cvterm.name = 'serine_threonine_motif' OR cvterm.name = 'serine_threonine_staple_motif' OR cvterm.name = 'polypeptide_turn_motif' OR cvterm.name = 'catmat_left_handed_three' OR cvterm.name = 'catmat_left_handed_four' OR cvterm.name = 'catmat_right_handed_three' OR cvterm.name = 'catmat_right_handed_four' OR cvterm.name = 'alpha_beta_motif' OR cvterm.name = 'peptide_coil' OR cvterm.name = 'beta_bulge_loop_five' OR cvterm.name = 'beta_bulge_loop_six' OR cvterm.name = 'antiparallel_beta_strand' OR cvterm.name = 'parallel_beta_strand' OR cvterm.name = 'left_handed_peptide_helix' OR cvterm.name = 'right_handed_peptide_helix' OR cvterm.name = 'alpha_helix' OR cvterm.name = 'pi_helix' OR cvterm.name = 'three_ten_helix' OR cvterm.name = 'polypeptide_nest_left_right_motif' OR cvterm.name = 'polypeptide_nest_right_left_motif' OR cvterm.name = 'schellmann_loop_seven' OR cvterm.name = 'schellmann_loop_six' OR cvterm.name = 'asx_turn' OR cvterm.name = 'beta_turn' OR cvterm.name = 'gamma_turn' OR cvterm.name = 'serine_threonine_turn' OR cvterm.name = 'asx_turn_left_handed_type_one' OR cvterm.name = 'asx_turn_left_handed_type_two' OR cvterm.name = 'asx_turn_right_handed_type_two' OR cvterm.name = 'asx_turn_right_handed_type_one' OR cvterm.name = 'beta_turn_left_handed_type_one' OR cvterm.name = 'beta_turn_left_handed_type_two' OR cvterm.name = 'beta_turn_right_handed_type_one' OR cvterm.name = 'beta_turn_right_handed_type_two' OR cvterm.name = 'beta_turn_type_six' OR cvterm.name = 'beta_turn_type_eight' OR cvterm.name = 'beta_turn_type_six_a' OR cvterm.name = 'beta_turn_type_six_b' OR cvterm.name = 'beta_turn_type_six_a_one' OR cvterm.name = 'beta_turn_type_six_a_two' OR cvterm.name = 'gamma_turn_classic' OR cvterm.name = 'gamma_turn_inverse' OR cvterm.name = 'st_turn_left_handed_type_one' OR cvterm.name = 'st_turn_left_handed_type_two' OR cvterm.name = 'st_turn_right_handed_type_one' OR cvterm.name = 'st_turn_right_handed_type_two' OR cvterm.name = 'coiled_coil' OR cvterm.name = 'helix_turn_helix' OR cvterm.name = 'natural_variant_site' OR cvterm.name = 'mutated_variant_site' OR cvterm.name = 'alternate_sequence_site' OR cvterm.name = 'signal_peptide' OR cvterm.name = 'transit_peptide' OR cvterm.name = 'nuclear_localization_signal' OR cvterm.name = 'endosomal_localization_signal' OR cvterm.name = 'lysosomal_localization_signal' OR cvterm.name = 'nuclear_export_signal' OR cvterm.name = 'nuclear_rim_localization_signal' OR cvterm.name = 'cleaved_initiator_methionine' OR cvterm.name = 'intein' OR cvterm.name = 'propeptide_cleavage_site' OR cvterm.name = 'propeptide' OR cvterm.name = 'cleaved_for_gpi_anchor_region' OR cvterm.name = 'lipoprotein_signal_peptide' OR cvterm.name = 'n_terminal_region' OR cvterm.name = 'c_terminal_region' OR cvterm.name = 'central_hydrophobic_region_of_signal_peptide' OR cvterm.name = 'polypeptide_domain' OR cvterm.name = 'polypeptide_motif' OR cvterm.name = 'polypeptide_repeat' OR cvterm.name = 'biochemical_region_of_peptide' OR cvterm.name = 'polypeptide_conserved_motif' OR cvterm.name = 'post_translationally_modified_region' OR cvterm.name = 'conformational_switch' OR cvterm.name = 'molecular_contact_region' OR cvterm.name = 'polypeptide_binding_motif' OR cvterm.name = 'polypeptide_catalytic_motif' OR cvterm.name = 'histone_modification' OR cvterm.name = 'histone_methylation_site' OR cvterm.name = 'histone_acetylation_site' OR cvterm.name = 'histone_ubiqitination_site' OR cvterm.name = 'histone_acylation_region' OR cvterm.name = 'H4K20_monomethylation_site' OR cvterm.name = 'H2BK5_monomethylation_site' OR cvterm.name = 'H3K27_methylation_site' OR cvterm.name = 'H3K36_methylation_site' OR cvterm.name = 'H3K4_methylation_site' OR cvterm.name = 'H3K79_methylation_site' OR cvterm.name = 'H3K9_methylation_site' OR cvterm.name = 'H3K27_monomethylation_site' OR cvterm.name = 'H3K27_trimethylation_site' OR cvterm.name = 'H3K27_dimethylation_site' OR cvterm.name = 'H3K36_monomethylation_site' OR cvterm.name = 'H3K36_dimethylation_site' OR cvterm.name = 'H3K36_trimethylation_site' OR cvterm.name = 'H3K4_monomethylation_site' OR cvterm.name = 'H3K4_trimethylation' OR cvterm.name = 'H3K4_dimethylation_site' OR cvterm.name = 'H3K79_monomethylation_site' OR cvterm.name = 'H3K79_dimethylation_site' OR cvterm.name = 'H3K79_trimethylation_site' OR cvterm.name = 'H3K9_trimethylation_site' OR cvterm.name = 'H3K9_monomethylation_site' OR cvterm.name = 'H3K9_dimethylation_site' OR cvterm.name = 'H3K9_acetylation_site' OR cvterm.name = 'H3K14_acetylation_site' OR cvterm.name = 'H3K18_acetylation_site' OR cvterm.name = 'H3K23_acylation site' OR cvterm.name = 'H3K27_acylation_site' OR cvterm.name = 'H4K16_acylation_site' OR cvterm.name = 'H4K5_acylation_site' OR cvterm.name = 'H4K8_acylation site' OR cvterm.name = 'H2B_ubiquitination_site' OR cvterm.name = 'H4K_acylation_region' OR cvterm.name = 'polypeptide_metal_contact' OR cvterm.name = 'protein_protein_contact' OR cvterm.name = 'polypeptide_ligand_contact' OR cvterm.name = 'polypeptide_DNA_contact' OR cvterm.name = 'polypeptide_calcium_ion_contact_site' OR cvterm.name = 'polypeptide_cobalt_ion_contact_site' OR cvterm.name = 'polypeptide_copper_ion_contact_site' OR cvterm.name = 'polypeptide_iron_ion_contact_site' OR cvterm.name = 'polypeptide_magnesium_ion_contact_site' OR cvterm.name = 'polypeptide_manganese_ion_contact_site' OR cvterm.name = 'polypeptide_molybdenum_ion_contact_site' OR cvterm.name = 'polypeptide_nickel_ion_contact_site' OR cvterm.name = 'polypeptide_tungsten_ion_contact_site' OR cvterm.name = 'polypeptide_zinc_ion_contact_site' OR cvterm.name = 'non_transcribed_region' OR cvterm.name = 'gene_fragment' OR cvterm.name = 'TSS_region' OR cvterm.name = 'gene_segment' OR cvterm.name = 'pseudogenic_gene_segment' OR cvterm.name = 'mobile_intron' OR cvterm.name = 'extrachromosomal_mobile_genetic_element' OR cvterm.name = 'integrated_mobile_genetic_element' OR cvterm.name = 'natural_transposable_element' OR cvterm.name = 'viral_sequence' OR cvterm.name = 'natural_plasmid' OR cvterm.name = 'phage_sequence' OR cvterm.name = 'ds_RNA_viral_sequence' OR cvterm.name = 'ds_DNA_viral_sequence' OR cvterm.name = 'ss_RNA_viral_sequence' OR cvterm.name = 'negative_sense_ssRNA_viral_sequence' OR cvterm.name = 'positive_sense_ssRNA_viral_sequence' OR cvterm.name = 'ambisense_ssRNA_viral_sequence' OR cvterm.name = 'transposable_element' OR cvterm.name = 'proviral_region' OR cvterm.name = 'integron' OR cvterm.name = 'genomic_island' OR cvterm.name = 'integrated_plasmid' OR cvterm.name = 'cointegrated_plasmid' OR cvterm.name = 'retrotransposon' OR cvterm.name = 'DNA_transposon' OR cvterm.name = 'foreign_transposable_element' OR cvterm.name = 'transgenic_transposable_element' OR cvterm.name = 'natural_transposable_element' OR cvterm.name = 'engineered_transposable_element' OR cvterm.name = 'nested_transposon' OR cvterm.name = 'LTR_retrotransposon' OR cvterm.name = 'non_LTR_retrotransposon' OR cvterm.name = 'LINE_element' OR cvterm.name = 'SINE_element' OR cvterm.name = 'terminal_inverted_repeat_element' OR cvterm.name = 'foldback_element' OR cvterm.name = 'conjugative_transposon' OR cvterm.name = 'helitron' OR cvterm.name = 'p_element' OR cvterm.name = 'MITE' OR cvterm.name = 'insertion_sequence' OR cvterm.name = 'polinton' OR cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'prophage' OR cvterm.name = 'pathogenic_island' OR cvterm.name = 'metabolic_island' OR cvterm.name = 'adaptive_island' OR cvterm.name = 'symbiosis_island' OR cvterm.name = 'cryptic_prophage' OR cvterm.name = 'defective_conjugative_transposon' OR cvterm.name = 'plasmid' OR cvterm.name = 'chromosome' OR cvterm.name = 'vector_replicon' OR cvterm.name = 'maxicircle' OR cvterm.name = 'minicircle' OR cvterm.name = 'viral_sequence' OR cvterm.name = 'engineered_plasmid' OR cvterm.name = 'episome' OR cvterm.name = 'natural_plasmid' OR cvterm.name = 'engineered_episome' OR cvterm.name = 'gene_trap_construct' OR cvterm.name = 'promoter_trap_construct' OR cvterm.name = 'enhancer_trap_construct' OR cvterm.name = 'engineered_episome' OR cvterm.name = 'mitochondrial_chromosome' OR cvterm.name = 'chloroplast_chromosome' OR cvterm.name = 'chromoplast_chromosome' OR cvterm.name = 'cyanelle_chromosome' OR cvterm.name = 'leucoplast_chromosome' OR cvterm.name = 'macronuclear_chromosome' OR cvterm.name = 'micronuclear_chromosome' OR cvterm.name = 'nuclear_chromosome' OR cvterm.name = 'nucleomorphic_chromosome' OR cvterm.name = 'DNA_chromosome' OR cvterm.name = 'RNA_chromosome' OR cvterm.name = 'apicoplast_chromosome' OR cvterm.name = 'double_stranded_DNA_chromosome' OR cvterm.name = 'single_stranded_DNA_chromosome' OR cvterm.name = 'linear_double_stranded_DNA_chromosome' OR cvterm.name = 'circular_double_stranded_DNA_chromosome' OR cvterm.name = 'linear_single_stranded_DNA_chromosome' OR cvterm.name = 'circular_single_stranded_DNA_chromosome' OR cvterm.name = 'single_stranded_RNA_chromosome' OR cvterm.name = 'double_stranded_RNA_chromosome' OR cvterm.name = 'linear_single_stranded_RNA_chromosome' OR cvterm.name = 'circular_single_stranded_RNA_chromosome' OR cvterm.name = 'linear_double_stranded_RNA_chromosome' OR cvterm.name = 'circular_double_stranded_RNA_chromosome' OR cvterm.name = 'YAC' OR cvterm.name = 'BAC' OR cvterm.name = 'PAC' OR cvterm.name = 'cosmid' OR cvterm.name = 'phagemid' OR cvterm.name = 'fosmid' OR cvterm.name = 'lambda_vector' OR cvterm.name = 'plasmid_vector' OR cvterm.name = 'targeting_vector' OR cvterm.name = 'phage_sequence' OR cvterm.name = 'ds_RNA_viral_sequence' OR cvterm.name = 'ds_DNA_viral_sequence' OR cvterm.name = 'ss_RNA_viral_sequence' OR cvterm.name = 'negative_sense_ssRNA_viral_sequence' OR cvterm.name = 'positive_sense_ssRNA_viral_sequence' OR cvterm.name = 'ambisense_ssRNA_viral_sequence' OR cvterm.name = 'modified_RNA_base_feature' OR cvterm.name = 'inosine' OR cvterm.name = 'seven_methylguanine' OR cvterm.name = 'ribothymidine' OR cvterm.name = 'modified_adenosine' OR cvterm.name = 'modified_cytidine' OR cvterm.name = 'modified_guanosine' OR cvterm.name = 'modified_uridine' OR cvterm.name = 'modified_inosine' OR cvterm.name = 'methylinosine' OR cvterm.name = 'one_methylinosine' OR cvterm.name = 'one_two_prime_O_dimethylinosine' OR cvterm.name = 'two_prime_O_methylinosine' OR cvterm.name = 'one_methyladenosine' OR cvterm.name = 'two_methyladenosine' OR cvterm.name = 'N6_methyladenosine' OR cvterm.name = 'two_prime_O_methyladenosine' OR cvterm.name = 'two_methylthio_N6_methyladenosine' OR cvterm.name = 'N6_isopentenyladenosine' OR cvterm.name = 'two_methylthio_N6_isopentenyladenosine' OR cvterm.name = 'N6_cis_hydroxyisopentenyl_adenosine' OR cvterm.name = 'two_methylthio_N6_cis_hydroxyisopentenyl_adenosine' OR cvterm.name = 'N6_glycinylcarbamoyladenosine' OR cvterm.name = 'N6_threonylcarbamoyladenosine' OR cvterm.name = 'two_methylthio_N6_threonyl_carbamoyladenosine' OR cvterm.name = 'N6_methyl_N6_threonylcarbamoyladenosine' OR cvterm.name = 'N6_hydroxynorvalylcarbamoyladenosine' OR cvterm.name = 'two_methylthio_N6_hydroxynorvalyl_carbamoyladenosine' OR cvterm.name = 'two_prime_O_ribosyladenosine_phosphate' OR cvterm.name = 'N6_N6_dimethyladenosine' OR cvterm.name = 'N6_2_prime_O_dimethyladenosine' OR cvterm.name = 'N6_N6_2_prime_O_trimethyladenosine' OR cvterm.name = 'one_two_prime_O_dimethyladenosine' OR cvterm.name = 'N6_acetyladenosine' OR cvterm.name = 'three_methylcytidine' OR cvterm.name = 'five_methylcytidine' OR cvterm.name = 'two_prime_O_methylcytidine' OR cvterm.name = 'two_thiocytidine' OR cvterm.name = 'N4_acetylcytidine' OR cvterm.name = 'five_formylcytidine' OR cvterm.name = 'five_two_prime_O_dimethylcytidine' OR cvterm.name = 'N4_acetyl_2_prime_O_methylcytidine' OR cvterm.name = 'lysidine' OR cvterm.name = 'N4_methylcytidine' OR cvterm.name = 'N4_2_prime_O_dimethylcytidine' OR cvterm.name = 'five_hydroxymethylcytidine' OR cvterm.name = 'five_formyl_two_prime_O_methylcytidine' OR cvterm.name = 'N4_N4_2_prime_O_trimethylcytidine' OR cvterm.name = 'seven_deazaguanosine' OR cvterm.name = 'one_methylguanosine' OR cvterm.name = 'N2_methylguanosine' OR cvterm.name = 'seven_methylguanosine' OR cvterm.name = 'two_prime_O_methylguanosine' OR cvterm.name = 'N2_N2_dimethylguanosine' OR cvterm.name = 'N2_2_prime_O_dimethylguanosine' OR cvterm.name = 'N2_N2_2_prime_O_trimethylguanosine' OR cvterm.name = 'two_prime_O_ribosylguanosine_phosphate' OR cvterm.name = 'wybutosine' OR cvterm.name = 'peroxywybutosine' OR cvterm.name = 'hydroxywybutosine' OR cvterm.name = 'undermodified_hydroxywybutosine' OR cvterm.name = 'wyosine' OR cvterm.name = 'methylwyosine' OR cvterm.name = 'N2_7_dimethylguanosine' OR cvterm.name = 'N2_N2_7_trimethylguanosine' OR cvterm.name = 'one_two_prime_O_dimethylguanosine' OR cvterm.name = 'four_demethylwyosine' OR cvterm.name = 'isowyosine' OR cvterm.name = 'N2_7_2prirme_O_trimethylguanosine' OR cvterm.name = 'queuosine' OR cvterm.name = 'epoxyqueuosine' OR cvterm.name = 'galactosyl_queuosine' OR cvterm.name = 'mannosyl_queuosine' OR cvterm.name = 'seven_cyano_seven_deazaguanosine' OR cvterm.name = 'seven_aminomethyl_seven_deazaguanosine' OR cvterm.name = 'archaeosine' OR cvterm.name = 'dihydrouridine' OR cvterm.name = 'pseudouridine' OR cvterm.name = 'five_methyluridine' OR cvterm.name = 'two_prime_O_methyluridine' OR cvterm.name = 'five_two_prime_O_dimethyluridine' OR cvterm.name = 'one_methylpseudouridine' OR cvterm.name = 'two_prime_O_methylpseudouridine' OR cvterm.name = 'two_thiouridine' OR cvterm.name = 'four_thiouridine' OR cvterm.name = 'five_methyl_2_thiouridine' OR cvterm.name = 'two_thio_two_prime_O_methyluridine' OR cvterm.name = 'three_three_amino_three_carboxypropyl_uridine' OR cvterm.name = 'five_hydroxyuridine' OR cvterm.name = 'five_methoxyuridine' OR cvterm.name = 'uridine_five_oxyacetic_acid' OR cvterm.name = 'uridine_five_oxyacetic_acid_methyl_ester' OR cvterm.name = 'five_carboxyhydroxymethyl_uridine' OR cvterm.name = 'five_carboxyhydroxymethyl_uridine_methyl_ester' OR cvterm.name = 'five_methoxycarbonylmethyluridine' OR cvterm.name = 'five_methoxycarbonylmethyl_two_prime_O_methyluridine' OR cvterm.name = 'five_methoxycarbonylmethyl_two_thiouridine' OR cvterm.name = 'five_aminomethyl_two_thiouridine' OR cvterm.name = 'five_methylaminomethyluridine' OR cvterm.name = 'five_methylaminomethyl_two_thiouridine' OR cvterm.name = 'five_methylaminomethyl_two_selenouridine' OR cvterm.name = 'five_carbamoylmethyluridine' OR cvterm.name = 'five_carbamoylmethyl_two_prime_O_methyluridine' OR cvterm.name = 'five_carboxymethylaminomethyluridine' OR cvterm.name = 'five_carboxymethylaminomethyl_two_prime_O_methyluridine' OR cvterm.name = 'five_carboxymethylaminomethyl_two_thiouridine' OR cvterm.name = 'three_methyluridine' OR cvterm.name = 'one_methyl_three_three_amino_three_carboxypropyl_pseudouridine' OR cvterm.name = 'five_carboxymethyluridine' OR cvterm.name = 'three_two_prime_O_dimethyluridine' OR cvterm.name = 'five_methyldihydrouridine' OR cvterm.name = 'three_methylpseudouridine' OR cvterm.name = 'five_taurinomethyluridine' OR cvterm.name = 'five_taurinomethyl_two_thiouridine' OR cvterm.name = 'five_isopentenylaminomethyl_uridine' OR cvterm.name = 'five_isopentenylaminomethyl_two_thiouridine' OR cvterm.name = 'five_isopentenylaminomethyl_two_prime_O_methyluridine' OR cvterm.name = 'catalytic_residue' OR cvterm.name = 'modified_amino_acid_feature' OR cvterm.name = 'alanine' OR cvterm.name = 'valine' OR cvterm.name = 'leucine' OR cvterm.name = 'isoleucine' OR cvterm.name = 'proline' OR cvterm.name = 'tryptophan' OR cvterm.name = 'phenylalanine' OR cvterm.name = 'methionine' OR cvterm.name = 'glycine' OR cvterm.name = 'serine' OR cvterm.name = 'threonine' OR cvterm.name = 'tyrosine' OR cvterm.name = 'cysteine' OR cvterm.name = 'glutamine' OR cvterm.name = 'asparagine' OR cvterm.name = 'lysine' OR cvterm.name = 'arginine' OR cvterm.name = 'histidine' OR cvterm.name = 'aspartic_acid' OR cvterm.name = 'glutamic_acid' OR cvterm.name = 'selenocysteine' OR cvterm.name = 'pyrrolysine' OR cvterm.name = 'modified_glycine' OR cvterm.name = 'modified_L_alanine' OR cvterm.name = 'modified_L_asparagine' OR cvterm.name = 'modified_L_aspartic_acid' OR cvterm.name = 'modified_L_cysteine' OR cvterm.name = 'modified_L_glutamic_acid' OR cvterm.name = 'modified_L_threonine' OR cvterm.name = 'modified_L_tryptophan' OR cvterm.name = 'modified_L_glutamine' OR cvterm.name = 'modified_L_methionine' OR cvterm.name = 'modified_L_isoleucine' OR cvterm.name = 'modified_L_phenylalanine' OR cvterm.name = 'modified_L_histidine' OR cvterm.name = 'modified_L_serine' OR cvterm.name = 'modified_L_lysine' OR cvterm.name = 'modified_L_leucine' OR cvterm.name = 'modified_L_selenocysteine' OR cvterm.name = 'modified_L_valine' OR cvterm.name = 'modified_L_proline' OR cvterm.name = 'modified_L_tyrosine' OR cvterm.name = 'modified_L_arginine' OR cvterm.name = 'heritable_phenotypic_marker' OR cvterm.name = 'DArT_marker' OR cvterm.name = 'nucleotide_motif' OR cvterm.name = 'DNA_motif' OR cvterm.name = 'RNA_motif' OR cvterm.name = 'PSE_motif' OR cvterm.name = 'CAAT_signal' OR cvterm.name = 'minus_10_signal' OR cvterm.name = 'minus_35_signal' OR cvterm.name = 'DRE_motif' OR cvterm.name = 'E_box_motif' OR cvterm.name = 'INR1_motif' OR cvterm.name = 'GAGA_motif' OR cvterm.name = 'octamer_motif' OR cvterm.name = 'retinoic_acid_responsive_element' OR cvterm.name = 'promoter_element' OR cvterm.name = 'DCE_SI' OR cvterm.name = 'DCE_SII' OR cvterm.name = 'DCE_SIII' OR cvterm.name = 'minus_12_signal' OR cvterm.name = 'minus_24_signal' OR cvterm.name = 'GC_rich_promoter_region' OR cvterm.name = 'DMv4_motif' OR cvterm.name = 'DMv5_motif' OR cvterm.name = 'DMv3_motif' OR cvterm.name = 'DMv2_motif' OR cvterm.name = 'DPE1_motif' OR cvterm.name = 'DMv1_motif' OR cvterm.name = 'NDM2_motif' OR cvterm.name = 'NDM3_motif' OR cvterm.name = 'core_promoter_element' OR cvterm.name = 'regulatory_promoter_element' OR cvterm.name = 'INR_motif' OR cvterm.name = 'DPE_motif' OR cvterm.name = 'BREu_motif' OR cvterm.name = 'TATA_box' OR cvterm.name = 'A_box' OR cvterm.name = 'B_box' OR cvterm.name = 'C_box' OR cvterm.name = 'MTE' OR cvterm.name = 'BREd_motif' OR cvterm.name = 'DCE' OR cvterm.name = 'intermediate_element' OR cvterm.name = 'RNA_polymerase_II_TATA_box' OR cvterm.name = 'RNA_polymerase_III_TATA_box' OR cvterm.name = 'A_box_type_1' OR cvterm.name = 'A_box_type_2' OR cvterm.name = 'proximal_promoter_element' OR cvterm.name = 'distal_promoter_element' OR cvterm.name = 'RNA_internal_loop' OR cvterm.name = 'A_minor_RNA_motif' OR cvterm.name = 'RNA_junction_loop' OR cvterm.name = 'hammerhead_ribozyme' OR cvterm.name = 'asymmetric_RNA_internal_loop' OR cvterm.name = 'symmetric_RNA_internal_loop' OR cvterm.name = 'K_turn_RNA_motif' OR cvterm.name = 'sarcin_like_RNA_motif' OR cvterm.name = 'RNA_hook_turn' OR cvterm.name = 'blunt_end_restriction_enzyme_cleavage_site' OR cvterm.name = 'sticky_end_restriction_enzyme_cleavage_site' OR cvterm.name = 'modified_base' OR cvterm.name = 'epigenetically_modified_gene' OR cvterm.name = 'histone_modification' OR cvterm.name = 'methylated_base_feature' OR cvterm.name = 'methylated_C' OR cvterm.name = 'methylated_A' OR cvterm.name = 'gene_rearranged_at_DNA_level' OR cvterm.name = 'maternally_imprinted_gene' OR cvterm.name = 'paternally_imprinted_gene' OR cvterm.name = 'allelically_excluded_gene' OR cvterm.name = 'histone_methylation_site' OR cvterm.name = 'histone_acetylation_site' OR cvterm.name = 'histone_ubiqitination_site' OR cvterm.name = 'histone_acylation_region' OR cvterm.name = 'H4K20_monomethylation_site' OR cvterm.name = 'H2BK5_monomethylation_site' OR cvterm.name = 'H3K27_methylation_site' OR cvterm.name = 'H3K36_methylation_site' OR cvterm.name = 'H3K4_methylation_site' OR cvterm.name = 'H3K79_methylation_site' OR cvterm.name = 'H3K9_methylation_site' OR cvterm.name = 'H3K27_monomethylation_site' OR cvterm.name = 'H3K27_trimethylation_site' OR cvterm.name = 'H3K27_dimethylation_site' OR cvterm.name = 'H3K36_monomethylation_site' OR cvterm.name = 'H3K36_dimethylation_site' OR cvterm.name = 'H3K36_trimethylation_site' OR cvterm.name = 'H3K4_monomethylation_site' OR cvterm.name = 'H3K4_trimethylation' OR cvterm.name = 'H3K4_dimethylation_site' OR cvterm.name = 'H3K79_monomethylation_site' OR cvterm.name = 'H3K79_dimethylation_site' OR cvterm.name = 'H3K79_trimethylation_site' OR cvterm.name = 'H3K9_trimethylation_site' OR cvterm.name = 'H3K9_monomethylation_site' OR cvterm.name = 'H3K9_dimethylation_site' OR cvterm.name = 'H3K9_acetylation_site' OR cvterm.name = 'H3K14_acetylation_site' OR cvterm.name = 'H3K18_acetylation_site' OR cvterm.name = 'H3K23_acylation site' OR cvterm.name = 'H3K27_acylation_site' OR cvterm.name = 'H4K16_acylation_site' OR cvterm.name = 'H4K5_acylation_site' OR cvterm.name = 'H4K8_acylation site' OR cvterm.name = 'H2B_ubiquitination_site' OR cvterm.name = 'H4K_acylation_region' OR cvterm.name = 'operon' OR cvterm.name = 'mating_type_region' OR cvterm.name = 'gene_array' OR cvterm.name = 'gene_subarray' OR cvterm.name = 'gene_cassette_array' OR cvterm.name = 'regulon' OR cvterm.name = 'sequence_length_variation' OR cvterm.name = 'MNP' OR cvterm.name = 'SNV' OR cvterm.name = 'complex_substitution' OR cvterm.name = 'simple_sequence_length_variation' OR cvterm.name = 'SNP' OR cvterm.name = 'point_mutation' OR cvterm.name = 'transition' OR cvterm.name = 'transversion' OR cvterm.name = 'pyrimidine_transition' OR cvterm.name = 'purine_transition' OR cvterm.name = 'C_to_T_transition' OR cvterm.name = 'T_to_C_transition' OR cvterm.name = 'C_to_T_transition_at_pCpG_site' OR cvterm.name = 'A_to_G_transition' OR cvterm.name = 'G_to_A_transition' OR cvterm.name = 'pyrimidine_to_purine_transversion' OR cvterm.name = 'purine_to_pyrimidine_transversion' OR cvterm.name = 'C_to_A_transversion' OR cvterm.name = 'C_to_G_transversion' OR cvterm.name = 'T_to_A_transversion' OR cvterm.name = 'T_to_G_transversion' OR cvterm.name = 'A_to_C_transversion' OR cvterm.name = 'A_to_T_transversion' OR cvterm.name = 'G_to_C_transversion' OR cvterm.name = 'G_to_T_transversion' OR cvterm.name = 'flanking_region' OR cvterm.name = 'repeat_component' OR cvterm.name = 'transposable_element_flanking_region' OR cvterm.name = 'five_prime_flanking_region' OR cvterm.name = 'three_prime_flanking_region' OR cvterm.name = 'non_LTR_retrotransposon_polymeric_tract' OR cvterm.name = 'LTR_component' OR cvterm.name = 'repeat_fragment' OR cvterm.name = 'transposon_fragment' OR cvterm.name = 'U5_LTR_region' OR cvterm.name = 'R_LTR_region' OR cvterm.name = 'U3_LTR_region' OR cvterm.name = 'three_prime_LTR_component' OR cvterm.name = 'five_prime_LTR_component' OR cvterm.name = 'U5_five_prime_LTR_region' OR cvterm.name = 'R_five_prime_LTR_region' OR cvterm.name = 'U3_five_prime_LTR_region' OR cvterm.name = 'R_three_prime_LTR_region' OR cvterm.name = 'U3_three_prime_LTR_region' OR cvterm.name = 'U5_three_prime_LTR_region' OR cvterm.name = 'R_five_prime_LTR_region' OR cvterm.name = 'U5_five_prime_LTR_region' OR cvterm.name = 'U3_five_prime_LTR_region' OR cvterm.name = 'clone_insert_end' OR cvterm.name = 'clone_insert_start' OR cvterm.name = 'exon_junction' OR cvterm.name = 'insertion_site' OR cvterm.name = 'polyA_site' OR cvterm.name = 'deletion_junction' OR cvterm.name = 'chromosome_breakpoint' OR cvterm.name = 'splice_junction' OR cvterm.name = 'trans_splice_junction' OR cvterm.name = 'restriction_enzyme_cleavage_junction' OR cvterm.name = 'transposable_element_insertion_site' OR cvterm.name = 'inversion_breakpoint' OR cvterm.name = 'translocation_breakpoint' OR cvterm.name = 'insertion_breakpoint' OR cvterm.name = 'deletion_breakpoint' OR cvterm.name = 'blunt_end_restriction_enzyme_cleavage_junction' OR cvterm.name = 'single_strand_restriction_enzyme_cleavage_site' OR cvterm.name = 'five_prime_restriction_enzyme_junction' OR cvterm.name = 'three_prime_restriction_enzyme_junction' OR cvterm.name = 'deletion' OR cvterm.name = 'translocation' OR cvterm.name = 'insertion' OR cvterm.name = 'copy_number_variation' OR cvterm.name = 'UPD' OR cvterm.name = 'structural_alteration' OR cvterm.name = 'substitution' OR cvterm.name = 'indel' OR cvterm.name = 'inversion' OR cvterm.name = 'transgenic_insertion' OR cvterm.name = 'duplication' OR cvterm.name = 'tandem_duplication' OR cvterm.name = 'direct_tandem_duplication' OR cvterm.name = 'inverted_tandem_duplication' OR cvterm.name = 'copy_number_gain' OR cvterm.name = 'copy_number_loss' OR cvterm.name = 'maternal_uniparental_disomy' OR cvterm.name = 'paternal_uniparental_disomy' OR cvterm.name = 'complex_structural_alteration' OR cvterm.name = 'sequence_length_variation' OR cvterm.name = 'MNP' OR cvterm.name = 'SNV' OR cvterm.name = 'complex_substitution' OR cvterm.name = 'simple_sequence_length_variation' OR cvterm.name = 'SNP' OR cvterm.name = 'point_mutation' OR cvterm.name = 'transition' OR cvterm.name = 'transversion' OR cvterm.name = 'pyrimidine_transition' OR cvterm.name = 'purine_transition' OR cvterm.name = 'C_to_T_transition' OR cvterm.name = 'T_to_C_transition' OR cvterm.name = 'C_to_T_transition_at_pCpG_site' OR cvterm.name = 'A_to_G_transition' OR cvterm.name = 'G_to_A_transition' OR cvterm.name = 'pyrimidine_to_purine_transversion' OR cvterm.name = 'purine_to_pyrimidine_transversion' OR cvterm.name = 'C_to_A_transversion' OR cvterm.name = 'C_to_G_transversion' OR cvterm.name = 'T_to_A_transversion' OR cvterm.name = 'T_to_G_transversion' OR cvterm.name = 'A_to_C_transversion' OR cvterm.name = 'A_to_T_transversion' OR cvterm.name = 'G_to_C_transversion' OR cvterm.name = 'G_to_T_transversion' OR cvterm.name = 'sequence_feature';

--- ************************************************
--- *** relation: transposable_element_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene encoded within a transposable ele ***
--- *** ment. For example gag, int, env and pol  ***
--- *** are the transposable element genes of th ***
--- *** e TY element in yeast.                   ***
--- ************************************************
---

CREATE VIEW transposable_element_gene AS
  SELECT
    feature_id AS transposable_element_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'transposable_element_gene';

--- ************************************************
--- *** relation: primer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An oligo to which new deoxyribonucleotid ***
--- *** es can be added by DNA polymerase.       ***
--- ************************************************
---

CREATE VIEW primer AS
  SELECT
    feature_id AS primer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'sequencing_primer' OR cvterm.name = 'forward_primer' OR cvterm.name = 'reverse_primer' OR cvterm.name = 'ASPE_primer' OR cvterm.name = 'dCAPS_primer' OR cvterm.name = 'primer';

--- ************************************************
--- *** relation: proviral_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A viral sequence which has integrated in ***
--- *** to a host genome.                        ***
--- ************************************************
---

CREATE VIEW proviral_region AS
  SELECT
    feature_id AS proviral_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'prophage' OR cvterm.name = 'proviral_region';

--- ************************************************
--- *** relation: methylated_c ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A methylated deoxy-cytosine.             ***
--- ************************************************
---

CREATE VIEW methylated_c AS
  SELECT
    feature_id AS methylated_c_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'methylated_C';

--- ************************************************
--- *** relation: edited ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence that  ***
--- *** is modified by editing.                  ***
--- ************************************************
---

CREATE VIEW edited AS
  SELECT
    feature_id AS edited_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'edited';

--- ************************************************
--- *** relation: transcript_with_translational_frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript with a translational frames ***
--- *** hift.                                    ***
--- ************************************************
---

CREATE VIEW transcript_with_translational_frameshift AS
  SELECT
    feature_id AS transcript_with_translational_frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transcript_with_translational_frameshift';

--- ************************************************
--- *** relation: regulated ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a sequence that ***
--- ***  is regulated.                           ***
--- ************************************************
---

CREATE VIEW regulated AS
  SELECT
    feature_id AS regulated_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transcriptionally_regulated' OR cvterm.name = 'post_translationally_regulated' OR cvterm.name = 'translationally_regulated' OR cvterm.name = 'imprinted' OR cvterm.name = 'transcriptionally_constitutive' OR cvterm.name = 'transcriptionally_induced' OR cvterm.name = 'transcriptionally_repressed' OR cvterm.name = 'autoregulated' OR cvterm.name = 'positively_autoregulated' OR cvterm.name = 'negatively_autoregulated' OR cvterm.name = 'silenced' OR cvterm.name = 'silenced_by_DNA_modification' OR cvterm.name = 'silenced_by_RNA_interference' OR cvterm.name = 'silenced_by_histone_modification' OR cvterm.name = 'silenced_by_DNA_methylation' OR cvterm.name = 'silenced_by_histone_methylation' OR cvterm.name = 'silenced_by_histone_deacetylation' OR cvterm.name = 'negatively_autoregulated' OR cvterm.name = 'positively_autoregulated' OR cvterm.name = 'post_translationally_regulated_by_protein_stability' OR cvterm.name = 'post_translationally_regulated_by_protein_modification' OR cvterm.name = 'maternally_imprinted' OR cvterm.name = 'paternally_imprinted' OR cvterm.name = 'regulated';

--- ************************************************
--- *** relation: protein_coding_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript that, at least in p ***
--- *** art, encodes one or more proteins.       ***
--- ************************************************
---

CREATE VIEW protein_coding_primary_transcript AS
  SELECT
    feature_id AS protein_coding_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pre_edited_mRNA' OR cvterm.name = 'protein_coding_primary_transcript';

--- ************************************************
--- *** relation: forward_primer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A single stranded oligo used for polymer ***
--- *** ase chain reaction.                      ***
--- ************************************************
---

CREATE VIEW forward_primer AS
  SELECT
    feature_id AS forward_primer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'forward_primer';

--- ************************************************
--- *** relation: rna_sequence_secondary_structure ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A folded RNA sequence.                   ***
--- ************************************************
---

CREATE VIEW rna_sequence_secondary_structure AS
  SELECT
    feature_id AS rna_sequence_secondary_structure_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stem_loop' OR cvterm.name = 'tetraloop' OR cvterm.name = 'RNA_sequence_secondary_structure';

--- ************************************************
--- *** relation: transcriptionally_regulated ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a gene that is r ***
--- *** egulated at transcription.               ***
--- ************************************************
---

CREATE VIEW transcriptionally_regulated AS
  SELECT
    feature_id AS transcriptionally_regulated_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transcriptionally_constitutive' OR cvterm.name = 'transcriptionally_induced' OR cvterm.name = 'transcriptionally_repressed' OR cvterm.name = 'autoregulated' OR cvterm.name = 'positively_autoregulated' OR cvterm.name = 'negatively_autoregulated' OR cvterm.name = 'silenced' OR cvterm.name = 'silenced_by_DNA_modification' OR cvterm.name = 'silenced_by_RNA_interference' OR cvterm.name = 'silenced_by_histone_modification' OR cvterm.name = 'silenced_by_DNA_methylation' OR cvterm.name = 'silenced_by_histone_methylation' OR cvterm.name = 'silenced_by_histone_deacetylation' OR cvterm.name = 'negatively_autoregulated' OR cvterm.name = 'positively_autoregulated' OR cvterm.name = 'transcriptionally_regulated';

--- ************************************************
--- *** relation: transcriptionally_constitutive ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Expressed in relatively constant amounts ***
--- ***  without regard to cellular environmenta ***
--- *** l conditions such as the concentration o ***
--- *** f a particular substrate.                ***
--- ************************************************
---

CREATE VIEW transcriptionally_constitutive AS
  SELECT
    feature_id AS transcriptionally_constitutive_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transcriptionally_constitutive';

--- ************************************************
--- *** relation: transcriptionally_induced ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An inducer molecule is required for tran ***
--- *** scription to occur.                      ***
--- ************************************************
---

CREATE VIEW transcriptionally_induced AS
  SELECT
    feature_id AS transcriptionally_induced_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'positively_autoregulated' OR cvterm.name = 'transcriptionally_induced';

--- ************************************************
--- *** relation: transcriptionally_repressed ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A repressor molecule is required for tra ***
--- *** nscription to stop.                      ***
--- ************************************************
---

CREATE VIEW transcriptionally_repressed AS
  SELECT
    feature_id AS transcriptionally_repressed_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'negatively_autoregulated' OR cvterm.name = 'silenced' OR cvterm.name = 'silenced_by_DNA_modification' OR cvterm.name = 'silenced_by_RNA_interference' OR cvterm.name = 'silenced_by_histone_modification' OR cvterm.name = 'silenced_by_DNA_methylation' OR cvterm.name = 'silenced_by_histone_methylation' OR cvterm.name = 'silenced_by_histone_deacetylation' OR cvterm.name = 'transcriptionally_repressed';

--- ************************************************
--- *** relation: silenced_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is silenced.                 ***
--- ************************************************
---

CREATE VIEW silenced_gene AS
  SELECT
    feature_id AS silenced_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_silenced_by_DNA_modification' OR cvterm.name = 'gene_silenced_by_RNA_interference' OR cvterm.name = 'gene_silenced_by_histone_modification' OR cvterm.name = 'gene_silenced_by_DNA_methylation' OR cvterm.name = 'gene_silenced_by_histone_methylation' OR cvterm.name = 'gene_silenced_by_histone_deacetylation' OR cvterm.name = 'silenced_gene';

--- ************************************************
--- *** relation: gene_silenced_by_dna_modification ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is silenced by DNA modificat ***
--- *** ion.                                     ***
--- ************************************************
---

CREATE VIEW gene_silenced_by_dna_modification AS
  SELECT
    feature_id AS gene_silenced_by_dna_modification_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_silenced_by_DNA_methylation' OR cvterm.name = 'gene_silenced_by_DNA_modification';

--- ************************************************
--- *** relation: gene_silenced_by_dna_methylation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is silenced by DNA methylati ***
--- *** on.                                      ***
--- ************************************************
---

CREATE VIEW gene_silenced_by_dna_methylation AS
  SELECT
    feature_id AS gene_silenced_by_dna_methylation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_silenced_by_DNA_methylation';

--- ************************************************
--- *** relation: post_translationally_regulated ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a gene that is r ***
--- *** egulated after it has been translated.   ***
--- ************************************************
---

CREATE VIEW post_translationally_regulated AS
  SELECT
    feature_id AS post_translationally_regulated_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'post_translationally_regulated_by_protein_stability' OR cvterm.name = 'post_translationally_regulated_by_protein_modification' OR cvterm.name = 'post_translationally_regulated';

--- ************************************************
--- *** relation: translationally_regulated ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a gene that is r ***
--- *** egulated as it is translated.            ***
--- ************************************************
---

CREATE VIEW translationally_regulated AS
  SELECT
    feature_id AS translationally_regulated_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'translationally_regulated';

--- ************************************************
--- *** relation: reverse_primer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A single stranded oligo used for polymer ***
--- *** ase chain reaction.                      ***
--- ************************************************
---

CREATE VIEW reverse_primer AS
  SELECT
    feature_id AS reverse_primer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'reverse_primer';

--- ************************************************
--- *** relation: epigenetically_modified ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** This attribute describes a gene where he ***
--- *** ritable changes other than those in the  ***
--- *** DNA sequence occur. These changes includ ***
--- *** e: modification to the DNA (such as DNA  ***
--- *** methylation, the covalent modification o ***
--- *** f cytosine), and post-translational modi ***
--- *** fication of histones.                    ***
--- ************************************************
---

CREATE VIEW epigenetically_modified AS
  SELECT
    feature_id AS epigenetically_modified_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'imprinted' OR cvterm.name = 'allelically_excluded' OR cvterm.name = 'rearranged_at_DNA_level' OR cvterm.name = 'maternally_imprinted' OR cvterm.name = 'paternally_imprinted' OR cvterm.name = 'epigenetically_modified';

--- ************************************************
--- *** relation: imprinted ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Imprinted genes are epigenetically modif ***
--- *** ied genes that are expressed monoallelic ***
--- *** ally according to their parent of origin ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW imprinted AS
  SELECT
    feature_id AS imprinted_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'maternally_imprinted' OR cvterm.name = 'paternally_imprinted' OR cvterm.name = 'imprinted';

--- ************************************************
--- *** relation: maternally_imprinted ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The maternal copy of the gene is modifie ***
--- *** d, rendering it transcriptionally silent ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW maternally_imprinted AS
  SELECT
    feature_id AS maternally_imprinted_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'maternally_imprinted';

--- ************************************************
--- *** relation: paternally_imprinted ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The paternal copy of the gene is modifie ***
--- *** d, rendering it transcriptionally silent ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW paternally_imprinted AS
  SELECT
    feature_id AS paternally_imprinted_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'paternally_imprinted';

--- ************************************************
--- *** relation: allelically_excluded ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Allelic exclusion is a process occurring ***
--- ***  in diploid organisms, where a gene is i ***
--- *** nactivated and not expressed in that cel ***
--- *** l.                                       ***
--- ************************************************
---

CREATE VIEW allelically_excluded AS
  SELECT
    feature_id AS allelically_excluded_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'allelically_excluded';

--- ************************************************
--- *** relation: gene_rearranged_at_dna_level ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An epigenetically modified gene, rearran ***
--- *** ged at the DNA level.                    ***
--- ************************************************
---

CREATE VIEW gene_rearranged_at_dna_level AS
  SELECT
    feature_id AS gene_rearranged_at_dna_level_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_rearranged_at_DNA_level';

--- ************************************************
--- *** relation: ribosome_entry_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Region in mRNA where ribosome assembles. ***
--- ************************************************
---

CREATE VIEW ribosome_entry_site AS
  SELECT
    feature_id AS ribosome_entry_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'internal_ribosome_entry_site' OR cvterm.name = 'Shine_Dalgarno_sequence' OR cvterm.name = 'kozak_sequence' OR cvterm.name = 'internal_Shine_Dalgarno_sequence' OR cvterm.name = 'ribosome_entry_site';

--- ************************************************
--- *** relation: attenuator ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence segment located within the fi ***
--- *** ve prime end of an mRNA that causes prem ***
--- *** ature termination of translation.        ***
--- ************************************************
---

CREATE VIEW attenuator AS
  SELECT
    feature_id AS attenuator_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'attenuator';

--- ************************************************
--- *** relation: terminator ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The sequence of DNA located either at th ***
--- *** e end of the transcript that causes RNA  ***
--- *** polymerase to terminate transcription.   ***
--- ************************************************
---

CREATE VIEW terminator AS
  SELECT
    feature_id AS terminator_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'bacterial_terminator' OR cvterm.name = 'eukaryotic_terminator' OR cvterm.name = 'rho_dependent_bacterial_terminator' OR cvterm.name = 'rho_independent_bacterial_terminator' OR cvterm.name = 'terminator_of_type_2_RNApol_III_promoter' OR cvterm.name = 'terminator';

--- ************************************************
--- *** relation: dna_sequence_secondary_structure ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A folded DNA sequence.                   ***
--- ************************************************
---

CREATE VIEW dna_sequence_secondary_structure AS
  SELECT
    feature_id AS dna_sequence_secondary_structure_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'i_motif' OR cvterm.name = 'DNA_sequence_secondary_structure';

--- ************************************************
--- *** relation: assembly_component ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of known length which may be us ***
--- *** ed to manufacture a longer region.       ***
--- ************************************************
---

CREATE VIEW assembly_component AS
  SELECT
    feature_id AS assembly_component_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'contig' OR cvterm.name = 'read' OR cvterm.name = 'restriction_fragment' OR cvterm.name = 'golden_path_fragment' OR cvterm.name = 'tiling_path_fragment' OR cvterm.name = 'gap' OR cvterm.name = 'sonicate_fragment' OR cvterm.name = 'paired_end_fragment' OR cvterm.name = 'read_pair' OR cvterm.name = 'contig_read' OR cvterm.name = 'BAC_end' OR cvterm.name = 'dye_terminator_read' OR cvterm.name = 'pyrosequenced_read' OR cvterm.name = 'ligation_based_read' OR cvterm.name = 'polymerase_synthesis_read' OR cvterm.name = 'PAC_end' OR cvterm.name = 'YAC_end' OR cvterm.name = 'clone_end' OR cvterm.name = 'RFLP_fragment' OR cvterm.name = 'tiling_path_clone' OR cvterm.name = 'assembly_component';

--- ************************************************
--- *** relation: recoded_codon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A codon that has been redefined at trans ***
--- *** lation. The redefinition may be as a res ***
--- *** ult of translational bypass, translation ***
--- *** al frameshifting or stop codon readthrou ***
--- *** gh.                                      ***
--- ************************************************
---

CREATE VIEW recoded_codon AS
  SELECT
    feature_id AS recoded_codon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stop_codon_read_through' OR cvterm.name = 'stop_codon_redefined_as_pyrrolysine' OR cvterm.name = 'stop_codon_redefined_as_selenocysteine' OR cvterm.name = 'recoded_codon';

--- ************************************************
--- *** relation: capped ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing when a sequence, ***
--- ***  usually an mRNA is capped by the additi ***
--- *** on of a modified guanine nucleotide at t ***
--- *** he 5' end.                               ***
--- ************************************************
---

CREATE VIEW capped AS
  SELECT
    feature_id AS capped_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'capped';

--- ************************************************
--- *** relation: exon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of the transcript sequence with ***
--- *** in a gene which is not removed from the  ***
--- *** primary RNA transcript by RNA splicing.  ***
--- ************************************************
---

CREATE VIEW exon AS
  SELECT
    feature_id AS exon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'coding_exon' OR cvterm.name = 'noncoding_exon' OR cvterm.name = 'interior_exon' OR cvterm.name = 'exon_of_single_exon_gene' OR cvterm.name = 'interior_coding_exon' OR cvterm.name = 'five_prime_coding_exon' OR cvterm.name = 'three_prime_coding_exon' OR cvterm.name = 'three_prime_noncoding_exon' OR cvterm.name = 'five_prime_noncoding_exon' OR cvterm.name = 'exon';

--- ************************************************
--- *** relation: supercontig ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** One or more contigs that have been order ***
--- *** ed and oriented using end-read informati ***
--- *** on. Contains gaps that are filled with N ***
--- *** 's.                                      ***
--- ************************************************
---

CREATE VIEW supercontig AS
  SELECT
    feature_id AS supercontig_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'supercontig';

--- ************************************************
--- *** relation: contig ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A contiguous sequence derived from seque ***
--- *** nce assembly. Has no gaps, but may conta ***
--- *** in N's from unavailable bases.           ***
--- ************************************************
---

CREATE VIEW contig AS
  SELECT
    feature_id AS contig_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'contig';

--- ************************************************
--- *** relation: read ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence obtained from a single sequen ***
--- *** cing experiment. Typically a read is pro ***
--- *** duced when a base calling program interp ***
--- *** rets information from a chromatogram tra ***
--- *** ce file produced from a sequencing machi ***
--- *** ne.                                      ***
--- ************************************************
---

CREATE VIEW read AS
  SELECT
    feature_id AS read_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'read_pair' OR cvterm.name = 'contig_read' OR cvterm.name = 'BAC_end' OR cvterm.name = 'dye_terminator_read' OR cvterm.name = 'pyrosequenced_read' OR cvterm.name = 'ligation_based_read' OR cvterm.name = 'polymerase_synthesis_read' OR cvterm.name = 'PAC_end' OR cvterm.name = 'YAC_end' OR cvterm.name = 'clone_end' OR cvterm.name = 'read';

--- ************************************************
--- *** relation: clone ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A piece of DNA that has been inserted in ***
--- ***  a vector so that it can be propagated i ***
--- *** n a host bacterium or some other organis ***
--- *** m.                                       ***
--- ************************************************
---

CREATE VIEW clone AS
  SELECT
    feature_id AS clone_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'genomic_clone' OR cvterm.name = 'cDNA_clone' OR cvterm.name = 'tiling_path_clone' OR cvterm.name = 'validated_cDNA_clone' OR cvterm.name = 'invalidated_cDNA_clone' OR cvterm.name = 'three_prime_RACE_clone' OR cvterm.name = 'chimeric_cDNA_clone' OR cvterm.name = 'genomically_contaminated_cDNA_clone' OR cvterm.name = 'polyA_primed_cDNA_clone' OR cvterm.name = 'partially_processed_cDNA_clone' OR cvterm.name = 'clone';

--- ************************************************
--- *** relation: yac ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Yeast Artificial Chromosome, a vector co ***
--- *** nstructed from the telomeric, centromeri ***
--- *** c, and replication origin sequences need ***
--- *** ed for replication in yeast cells.       ***
--- ************************************************
---

CREATE VIEW yac AS
  SELECT
    feature_id AS yac_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'YAC';

--- ************************************************
--- *** relation: bac ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Bacterial Artificial Chromosome, a cloni ***
--- *** ng vector that can be propagated as mini ***
--- *** -chromosomes in a bacterial host.        ***
--- ************************************************
---

CREATE VIEW bac AS
  SELECT
    feature_id AS bac_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'BAC';

--- ************************************************
--- *** relation: pac ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The P1-derived artificial chromosome are ***
--- ***  DNA constructs that are derived from th ***
--- *** e DNA of P1 bacteriophage. They can carr ***
--- *** y large amounts (about 100-300 kilobases ***
--- *** ) of other sequences for a variety of bi ***
--- *** oengineering purposes. It is one type of ***
--- ***  vector used to clone DNA fragments (100 ***
--- *** - to 300-kb insert size; average, 150 kb ***
--- *** ) in Escherichia coli cells.             ***
--- ************************************************
---

CREATE VIEW pac AS
  SELECT
    feature_id AS pac_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'PAC';

--- ************************************************
--- *** relation: plasmid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A self replicating, using the hosts cell ***
--- *** ular machinery, often circular nucleic a ***
--- *** cid molecule that is distinct from a chr ***
--- *** omosome in the organism.                 ***
--- ************************************************
---

CREATE VIEW plasmid AS
  SELECT
    feature_id AS plasmid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_plasmid' OR cvterm.name = 'episome' OR cvterm.name = 'natural_plasmid' OR cvterm.name = 'engineered_episome' OR cvterm.name = 'gene_trap_construct' OR cvterm.name = 'promoter_trap_construct' OR cvterm.name = 'enhancer_trap_construct' OR cvterm.name = 'engineered_episome' OR cvterm.name = 'plasmid';

--- ************************************************

SET search_path=so,chado,pg_catalog;
--- ************************************************
---

CREATE VIEW cysteine_trna_primary_transcript AS
  SELECT
    feature_id AS cysteine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cysteine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: glutamic_acid_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding glutaminyl ***
--- ***  tRNA (SO:0000260).                      ***
--- ************************************************
---

CREATE VIEW glutamic_acid_trna_primary_transcript AS
  SELECT
    feature_id AS glutamic_acid_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'glutamic_acid_tRNA_primary_transcript';

--- ************************************************
--- *** relation: glutamine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding glutamyl t ***
--- *** RNA (SO:0000260).                        ***
--- ************************************************
---

CREATE VIEW glutamine_trna_primary_transcript AS
  SELECT
    feature_id AS glutamine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'glutamine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: glycine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding glycyl tRN ***
--- *** A (SO:0000263).                          ***
--- ************************************************
---

CREATE VIEW glycine_trna_primary_transcript AS
  SELECT
    feature_id AS glycine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'glycine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: histidine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding histidyl t ***
--- *** RNA (SO:0000262).                        ***
--- ************************************************
---

CREATE VIEW histidine_trna_primary_transcript AS
  SELECT
    feature_id AS histidine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'histidine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: isoleucine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding isoleucyl  ***
--- *** tRNA (SO:0000263).                       ***
--- ************************************************
---

CREATE VIEW isoleucine_trna_primary_transcript AS
  SELECT
    feature_id AS isoleucine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'isoleucine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: leucine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding leucyl tRN ***
--- *** A (SO:0000264).                          ***
--- ************************************************
---

CREATE VIEW leucine_trna_primary_transcript AS
  SELECT
    feature_id AS leucine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'leucine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: lysine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding lysyl tRNA ***
--- ***  (SO:0000265).                           ***
--- ************************************************
---

CREATE VIEW lysine_trna_primary_transcript AS
  SELECT
    feature_id AS lysine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'lysine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: methionine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding methionyl  ***
--- *** tRNA (SO:0000266).                       ***
--- ************************************************
---

CREATE VIEW methionine_trna_primary_transcript AS
  SELECT
    feature_id AS methionine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'methionine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: phe_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding phenylalan ***
--- *** yl tRNA (SO:0000267).                    ***
--- ************************************************
---

CREATE VIEW phe_trna_primary_transcript AS
  SELECT
    feature_id AS phe_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'phenylalanine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: proline_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding prolyl tRN ***
--- *** A (SO:0000268).                          ***
--- ************************************************
---

CREATE VIEW proline_trna_primary_transcript AS
  SELECT
    feature_id AS proline_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'proline_tRNA_primary_transcript';

--- ************************************************
--- *** relation: serine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding seryl tRNA ***
--- ***  (SO:000269).                            ***
--- ************************************************
---

CREATE VIEW serine_trna_primary_transcript AS
  SELECT
    feature_id AS serine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'serine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: thr_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding threonyl t ***
--- *** RNA (SO:000270).                         ***
--- ************************************************
---

CREATE VIEW thr_trna_primary_transcript AS
  SELECT
    feature_id AS thr_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'threonine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: try_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding tryptophan ***
--- *** yl tRNA (SO:000271).                     ***
--- ************************************************
---

CREATE VIEW try_trna_primary_transcript AS
  SELECT
    feature_id AS try_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tryptophan_tRNA_primary_transcript';

--- ************************************************
--- *** relation: tyrosine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding tyrosyl tR ***
--- *** NA (SO:000272).                          ***
--- ************************************************
---

CREATE VIEW tyrosine_trna_primary_transcript AS
  SELECT
    feature_id AS tyrosine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tyrosine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: valine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding valyl tRNA ***
--- ***  (SO:000273).                            ***
--- ************************************************
---

CREATE VIEW valine_trna_primary_transcript AS
  SELECT
    feature_id AS valine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'valine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: snrna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding a small nu ***
--- *** clear RNA (SO:0000274).                  ***
--- ************************************************
---

CREATE VIEW snrna_primary_transcript AS
  SELECT
    feature_id AS snrna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'snRNA_primary_transcript';

--- ************************************************
--- *** relation: snorna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding a small nu ***
--- *** cleolar mRNA (SO:0000275).               ***
--- ************************************************
---

CREATE VIEW snorna_primary_transcript AS
  SELECT
    feature_id AS snorna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'methylation_guide_snoRNA_primary_transcript' OR cvterm.name = 'rRNA_cleavage_snoRNA_primary_transcript' OR cvterm.name = 'C_D_box_snoRNA_primary_transcript' OR cvterm.name = 'H_ACA_box_snoRNA_primary_transcript' OR cvterm.name = 'U14_snoRNA_primary_transcript' OR cvterm.name = 'snoRNA_primary_transcript';

--- ************************************************
--- *** relation: mature_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript which has undergone the nec ***
--- *** essary modifications, if any, for its fu ***
--- *** nction. In eukaryotes this includes, for ***
--- ***  example, processing of introns, cleavag ***
--- *** e, base modification, and modifications  ***
--- *** to the 5' and/or the 3' ends, other than ***
--- ***  addition of bases. In bacteria function ***
--- *** al mRNAs are usually not modified.       ***
--- ************************************************
---

CREATE VIEW mature_transcript AS
  SELECT
    feature_id AS mature_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mRNA' OR cvterm.name = 'ncRNA' OR cvterm.name = 'mRNA_with_frameshift' OR cvterm.name = 'monocistronic_mRNA' OR cvterm.name = 'polycistronic_mRNA' OR cvterm.name = 'exemplar_mRNA' OR cvterm.name = 'capped_mRNA' OR cvterm.name = 'polyadenylated_mRNA' OR cvterm.name = 'trans_spliced_mRNA' OR cvterm.name = 'edited_mRNA' OR cvterm.name = 'consensus_mRNA' OR cvterm.name = 'recoded_mRNA' OR cvterm.name = 'mRNA_with_minus_1_frameshift' OR cvterm.name = 'mRNA_with_plus_1_frameshift' OR cvterm.name = 'mRNA_with_plus_2_frameshift' OR cvterm.name = 'mRNA_with_minus_2_frameshift' OR cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'mRNA_recoded_by_translational_bypass' OR cvterm.name = 'mRNA_recoded_by_codon_redefinition' OR cvterm.name = 'scRNA' OR cvterm.name = 'rRNA' OR cvterm.name = 'tRNA' OR cvterm.name = 'snRNA' OR cvterm.name = 'snoRNA' OR cvterm.name = 'small_regulatory_ncRNA' OR cvterm.name = 'RNase_MRP_RNA' OR cvterm.name = 'RNase_P_RNA' OR cvterm.name = 'telomerase_RNA' OR cvterm.name = 'vault_RNA' OR cvterm.name = 'Y_RNA' OR cvterm.name = 'rasiRNA' OR cvterm.name = 'SRP_RNA' OR cvterm.name = 'guide_RNA' OR cvterm.name = 'antisense_RNA' OR cvterm.name = 'siRNA' OR cvterm.name = 'stRNA' OR cvterm.name = 'class_II_RNA' OR cvterm.name = 'class_I_RNA' OR cvterm.name = 'piRNA' OR cvterm.name = 'lincRNA' OR cvterm.name = 'tasiRNA' OR cvterm.name = 'rRNA_cleavage_RNA' OR cvterm.name = 'small_subunit_rRNA' OR cvterm.name = 'large_subunit_rRNA' OR cvterm.name = 'rRNA_18S' OR cvterm.name = 'rRNA_16S' OR cvterm.name = 'rRNA_5_8S' OR cvterm.name = 'rRNA_5S' OR cvterm.name = 'rRNA_28S' OR cvterm.name = 'rRNA_23S' OR cvterm.name = 'rRNA_25S' OR cvterm.name = 'rRNA_21S' OR cvterm.name = 'alanyl_tRNA' OR cvterm.name = 'asparaginyl_tRNA' OR cvterm.name = 'aspartyl_tRNA' OR cvterm.name = 'cysteinyl_tRNA' OR cvterm.name = 'glutaminyl_tRNA' OR cvterm.name = 'glutamyl_tRNA' OR cvterm.name = 'glycyl_tRNA' OR cvterm.name = 'histidyl_tRNA' OR cvterm.name = 'isoleucyl_tRNA' OR cvterm.name = 'leucyl_tRNA' OR cvterm.name = 'lysyl_tRNA' OR cvterm.name = 'methionyl_tRNA' OR cvterm.name = 'phenylalanyl_tRNA' OR cvterm.name = 'prolyl_tRNA' OR cvterm.name = 'seryl_tRNA' OR cvterm.name = 'threonyl_tRNA' OR cvterm.name = 'tryptophanyl_tRNA' OR cvterm.name = 'tyrosyl_tRNA' OR cvterm.name = 'valyl_tRNA' OR cvterm.name = 'pyrrolysyl_tRNA' OR cvterm.name = 'arginyl_tRNA' OR cvterm.name = 'selenocysteinyl_tRNA' OR cvterm.name = 'U1_snRNA' OR cvterm.name = 'U2_snRNA' OR cvterm.name = 'U4_snRNA' OR cvterm.name = 'U4atac_snRNA' OR cvterm.name = 'U5_snRNA' OR cvterm.name = 'U6_snRNA' OR cvterm.name = 'U6atac_snRNA' OR cvterm.name = 'U11_snRNA' OR cvterm.name = 'U12_snRNA' OR cvterm.name = 'C_D_box_snoRNA' OR cvterm.name = 'H_ACA_box_snoRNA' OR cvterm.name = 'U14_snoRNA' OR cvterm.name = 'U3_snoRNA' OR cvterm.name = 'methylation_guide_snoRNA' OR cvterm.name = 'pseudouridylation_guide_snoRNA' OR cvterm.name = 'miRNA' OR cvterm.name = 'RNA_6S' OR cvterm.name = 'CsrB_RsmB_RNA' OR cvterm.name = 'DsrA_RNA' OR cvterm.name = 'OxyS_RNA' OR cvterm.name = 'RprA_RNA' OR cvterm.name = 'RRE_RNA' OR cvterm.name = 'spot_42_RNA' OR cvterm.name = 'tmRNA' OR cvterm.name = 'GcvB_RNA' OR cvterm.name = 'MicF_RNA' OR cvterm.name = 'mature_transcript';

--- ************************************************
--- *** relation: mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Messenger RNA is the intermediate molecu ***
--- *** le between DNA and protein. It includes  ***
--- *** UTR and coding sequences. It does not co ***
--- *** ntain introns.                           ***
--- ************************************************
---

CREATE VIEW mrna AS
  SELECT
    feature_id AS mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mRNA_with_frameshift' OR cvterm.name = 'monocistronic_mRNA' OR cvterm.name = 'polycistronic_mRNA' OR cvterm.name = 'exemplar_mRNA' OR cvterm.name = 'capped_mRNA' OR cvterm.name = 'polyadenylated_mRNA' OR cvterm.name = 'trans_spliced_mRNA' OR cvterm.name = 'edited_mRNA' OR cvterm.name = 'consensus_mRNA' OR cvterm.name = 'recoded_mRNA' OR cvterm.name = 'mRNA_with_minus_1_frameshift' OR cvterm.name = 'mRNA_with_plus_1_frameshift' OR cvterm.name = 'mRNA_with_plus_2_frameshift' OR cvterm.name = 'mRNA_with_minus_2_frameshift' OR cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'mRNA_recoded_by_translational_bypass' OR cvterm.name = 'mRNA_recoded_by_codon_redefinition' OR cvterm.name = 'mRNA';

--- ************************************************
--- *** relation: tf_binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a nucleotide molecule that b ***
--- *** inds a Transcription Factor or Transcrip ***
--- *** tion Factor complex [GO:0005667].        ***
--- ************************************************
---

CREATE VIEW tf_binding_site AS
  SELECT
    feature_id AS tf_binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'TF_binding_site';

--- ************************************************
--- *** relation: orf ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The in-frame interval between the stop c ***
--- *** odons of a reading frame which when read ***
--- ***  as sequential triplets, has the potenti ***
--- *** al of encoding a sequential string of am ***
--- *** ino acids. TER(NNN)nTER.                 ***
--- ************************************************
---

CREATE VIEW orf AS
  SELECT
    feature_id AS orf_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mini_gene' OR cvterm.name = 'rescue_mini_gene' OR cvterm.name = 'ORF';

--- ************************************************
--- *** relation: transcript_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW transcript_attribute AS
  SELECT
    feature_id AS transcript_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'edited' OR cvterm.name = 'capped' OR cvterm.name = 'mRNA_attribute' OR cvterm.name = 'trans_spliced' OR cvterm.name = 'alternatively_spliced' OR cvterm.name = 'monocistronic' OR cvterm.name = 'polycistronic' OR cvterm.name = 'polyadenylated' OR cvterm.name = 'exemplar' OR cvterm.name = 'frameshift' OR cvterm.name = 'recoded' OR cvterm.name = 'minus_1_frameshift' OR cvterm.name = 'minus_2_frameshift' OR cvterm.name = 'plus_1_frameshift' OR cvterm.name = 'plus_2_framshift' OR cvterm.name = 'codon_redefined' OR cvterm.name = 'recoded_by_translational_bypass' OR cvterm.name = 'translationally_frameshifted' OR cvterm.name = 'minus_1_translationally_frameshifted' OR cvterm.name = 'plus_1_translationally_frameshifted' OR cvterm.name = 'dicistronic' OR cvterm.name = 'transcript_attribute';

--- ************************************************
--- *** relation: foldback_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transposable element with extensive se ***
--- *** condary structure, characterized by larg ***
--- *** e modular imperfect long inverted repeat ***
--- *** s.                                       ***
--- ************************************************
---

CREATE VIEW foldback_element AS
  SELECT
    feature_id AS foldback_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'foldback_element';

--- ************************************************
--- *** relation: flanking_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The sequences extending on either side o ***
--- *** f a specific region.                     ***
--- ************************************************
---

CREATE VIEW flanking_region AS
  SELECT
    feature_id AS flanking_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transposable_element_flanking_region' OR cvterm.name = 'five_prime_flanking_region' OR cvterm.name = 'three_prime_flanking_region' OR cvterm.name = 'flanking_region';

--- ************************************************
--- *** relation: chromosome_variation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW chromosome_variation AS
  SELECT
    feature_id AS chromosome_variation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'assortment_derived_variation' OR cvterm.name = 'chromosome_number_variation' OR cvterm.name = 'chromosome_structure_variation' OR cvterm.name = 'assortment_derived_duplication' OR cvterm.name = 'assortment_derived_deficiency_plus_duplication' OR cvterm.name = 'assortment_derived_deficiency' OR cvterm.name = 'assortment_derived_aneuploid' OR cvterm.name = 'aneuploid' OR cvterm.name = 'polyploid' OR cvterm.name = 'hyperploid' OR cvterm.name = 'hypoploid' OR cvterm.name = 'autopolyploid' OR cvterm.name = 'allopolyploid' OR cvterm.name = 'free_chromosome_arm' OR cvterm.name = 'chromosomal_transposition' OR cvterm.name = 'aneuploid_chromosome' OR cvterm.name = 'intrachromosomal_mutation' OR cvterm.name = 'interchromosomal_mutation' OR cvterm.name = 'chromosomal_duplication' OR cvterm.name = 'compound_chromosome' OR cvterm.name = 'autosynaptic_chromosome' OR cvterm.name = 'complex_chromosomal_mutation' OR cvterm.name = 'uncharacterised_chromosomal_mutation' OR cvterm.name = 'intrachromosomal_transposition' OR cvterm.name = 'interchromosomal_transposition' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'uninverted_intrachromosomal_transposition' OR cvterm.name = 'unoriented_intrachromosomal_transposition' OR cvterm.name = 'deficient_interchromosomal_transposition' OR cvterm.name = 'inverted_interchromosomal_transposition' OR cvterm.name = 'uninverted_interchromosomal_transposition' OR cvterm.name = 'unoriented_interchromosomal_transposition' OR cvterm.name = 'inversion_derived_aneuploid_chromosome' OR cvterm.name = 'chromosomal_deletion' OR cvterm.name = 'chromosomal_inversion' OR cvterm.name = 'intrachromosomal_duplication' OR cvterm.name = 'ring_chromosome' OR cvterm.name = 'chromosome_fission' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inversion_derived_bipartite_deficiency' OR cvterm.name = 'inversion_derived_deficiency_plus_duplication' OR cvterm.name = 'inversion_derived_deficiency_plus_aneuploid' OR cvterm.name = 'deficient_translocation' OR cvterm.name = 'deficient_inversion' OR cvterm.name = 'inverted_ring_chromosome' OR cvterm.name = 'pericentric_inversion' OR cvterm.name = 'paracentric_inversion' OR cvterm.name = 'inversion_cum_translocation' OR cvterm.name = 'bipartite_inversion' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'deficient_inversion' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'inversion_derived_deficiency_plus_duplication' OR cvterm.name = 'inversion_derived_bipartite_duplication' OR cvterm.name = 'inversion_derived_duplication_plus_aneuploid' OR cvterm.name = 'intrachromosomal_transposition' OR cvterm.name = 'bipartite_duplication' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'uninverted_intrachromosomal_transposition' OR cvterm.name = 'unoriented_intrachromosomal_transposition' OR cvterm.name = 'inverted_ring_chromosome' OR cvterm.name = 'free_ring_duplication' OR cvterm.name = 'chromosomal_translocation' OR cvterm.name = 'bipartite_duplication' OR cvterm.name = 'interchromosomal_transposition' OR cvterm.name = 'translocation_element' OR cvterm.name = 'Robertsonian_fusion' OR cvterm.name = 'reciprocal_chromosomal_translocation' OR cvterm.name = 'deficient_translocation' OR cvterm.name = 'inversion_cum_translocation' OR cvterm.name = 'cyclic_translocation' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'deficient_interchromosomal_transposition' OR cvterm.name = 'inverted_interchromosomal_transposition' OR cvterm.name = 'uninverted_interchromosomal_transposition' OR cvterm.name = 'unoriented_interchromosomal_transposition' OR cvterm.name = 'interchromosomal_duplication' OR cvterm.name = 'intrachromosomal_duplication' OR cvterm.name = 'free_duplication' OR cvterm.name = 'insertional_duplication' OR cvterm.name = 'inversion_derived_deficiency_plus_duplication' OR cvterm.name = 'inversion_derived_bipartite_duplication' OR cvterm.name = 'inversion_derived_duplication_plus_aneuploid' OR cvterm.name = 'intrachromosomal_transposition' OR cvterm.name = 'bipartite_duplication' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'uninverted_intrachromosomal_transposition' OR cvterm.name = 'unoriented_intrachromosomal_transposition' OR cvterm.name = 'free_ring_duplication' OR cvterm.name = 'uninverted_insertional_duplication' OR cvterm.name = 'inverted_insertional_duplication' OR cvterm.name = 'unoriented_insertional_duplication' OR cvterm.name = 'compound_chromosome_arm' OR cvterm.name = 'homo_compound_chromosome' OR cvterm.name = 'hetero_compound_chromosome' OR cvterm.name = 'dexstrosynaptic_chromosome' OR cvterm.name = 'laevosynaptic_chromosome' OR cvterm.name = 'partially_characterised_chromosomal_mutation' OR cvterm.name = 'chromosome_variation';

--- ************************************************
--- *** relation: internal_utr ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A UTR bordered by the terminal and initi ***
--- *** al codons of two CDSs in a polycistronic ***
--- ***  transcript. Every UTR is either 5', 3'  ***
--- *** or internal.                             ***
--- ************************************************
---

CREATE VIEW internal_utr AS
  SELECT
    feature_id AS internal_utr_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'internal_UTR';

--- ************************************************
--- *** relation: untranslated_region_polycistronic_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The untranslated sequence separating the ***
--- ***  'cistrons' of multicistronic mRNA.      ***
--- ************************************************
---

CREATE VIEW untranslated_region_polycistronic_mrna AS
  SELECT
    feature_id AS untranslated_region_polycistronic_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'untranslated_region_polycistronic_mRNA';

--- ************************************************
--- *** relation: internal_ribosome_entry_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Sequence element that recruits a ribosom ***
--- *** al subunit to internal mRNA for translat ***
--- *** ion initiation.                          ***
--- ************************************************
---

CREATE VIEW internal_ribosome_entry_site AS
  SELECT
    feature_id AS internal_ribosome_entry_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'internal_Shine_Dalgarno_sequence' OR cvterm.name = 'internal_ribosome_entry_site';

--- ************************************************
--- *** relation: polyadenylated ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A attribute describing the addition of a ***
--- ***  poly A tail to the 3' end of a mRNA mol ***
--- *** ecule.                                   ***
--- ************************************************
---

CREATE VIEW polyadenylated AS
  SELECT
    feature_id AS polyadenylated_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polyadenylated';

--- ************************************************
--- *** relation: sequence_length_variation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW sequence_length_variation AS
  SELECT
    feature_id AS sequence_length_variation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'simple_sequence_length_variation' OR cvterm.name = 'sequence_length_variation';

--- ************************************************
--- *** relation: modified_rna_base_feature ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A post_transcriptionally modified base.  ***
--- ************************************************
---

CREATE VIEW modified_rna_base_feature AS
  SELECT
    feature_id AS modified_rna_base_feature_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inosine' OR cvterm.name = 'seven_methylguanine' OR cvterm.name = 'ribothymidine' OR cvterm.name = 'modified_adenosine' OR cvterm.name = 'modified_cytidine' OR cvterm.name = 'modified_guanosine' OR cvterm.name = 'modified_uridine' OR cvterm.name = 'modified_inosine' OR cvterm.name = 'methylinosine' OR cvterm.name = 'one_methylinosine' OR cvterm.name = 'one_two_prime_O_dimethylinosine' OR cvterm.name = 'two_prime_O_methylinosine' OR cvterm.name = 'one_methyladenosine' OR cvterm.name = 'two_methyladenosine' OR cvterm.name = 'N6_methyladenosine' OR cvterm.name = 'two_prime_O_methyladenosine' OR cvterm.name = 'two_methylthio_N6_methyladenosine' OR cvterm.name = 'N6_isopentenyladenosine' OR cvterm.name = 'two_methylthio_N6_isopentenyladenosine' OR cvterm.name = 'N6_cis_hydroxyisopentenyl_adenosine' OR cvterm.name = 'two_methylthio_N6_cis_hydroxyisopentenyl_adenosine' OR cvterm.name = 'N6_glycinylcarbamoyladenosine' OR cvterm.name = 'N6_threonylcarbamoyladenosine' OR cvterm.name = 'two_methylthio_N6_threonyl_carbamoyladenosine' OR cvterm.name = 'N6_methyl_N6_threonylcarbamoyladenosine' OR cvterm.name = 'N6_hydroxynorvalylcarbamoyladenosine' OR cvterm.name = 'two_methylthio_N6_hydroxynorvalyl_carbamoyladenosine' OR cvterm.name = 'two_prime_O_ribosyladenosine_phosphate' OR cvterm.name = 'N6_N6_dimethyladenosine' OR cvterm.name = 'N6_2_prime_O_dimethyladenosine' OR cvterm.name = 'N6_N6_2_prime_O_trimethyladenosine' OR cvterm.name = 'one_two_prime_O_dimethyladenosine' OR cvterm.name = 'N6_acetyladenosine' OR cvterm.name = 'three_methylcytidine' OR cvterm.name = 'five_methylcytidine' OR cvterm.name = 'two_prime_O_methylcytidine' OR cvterm.name = 'two_thiocytidine' OR cvterm.name = 'N4_acetylcytidine' OR cvterm.name = 'five_formylcytidine' OR cvterm.name = 'five_two_prime_O_dimethylcytidine' OR cvterm.name = 'N4_acetyl_2_prime_O_methylcytidine' OR cvterm.name = 'lysidine' OR cvterm.name = 'N4_methylcytidine' OR cvterm.name = 'N4_2_prime_O_dimethylcytidine' OR cvterm.name = 'five_hydroxymethylcytidine' OR cvterm.name = 'five_formyl_two_prime_O_methylcytidine' OR cvterm.name = 'N4_N4_2_prime_O_trimethylcytidine' OR cvterm.name = 'seven_deazaguanosine' OR cvterm.name = 'one_methylguanosine' OR cvterm.name = 'N2_methylguanosine' OR cvterm.name = 'seven_methylguanosine' OR cvterm.name = 'two_prime_O_methylguanosine' OR cvterm.name = 'N2_N2_dimethylguanosine' OR cvterm.name = 'N2_2_prime_O_dimethylguanosine' OR cvterm.name = 'N2_N2_2_prime_O_trimethylguanosine' OR cvterm.name = 'two_prime_O_ribosylguanosine_phosphate' OR cvterm.name = 'wybutosine' OR cvterm.name = 'peroxywybutosine' OR cvterm.name = 'hydroxywybutosine' OR cvterm.name = 'undermodified_hydroxywybutosine' OR cvterm.name = 'wyosine' OR cvterm.name = 'methylwyosine' OR cvterm.name = 'N2_7_dimethylguanosine' OR cvterm.name = 'N2_N2_7_trimethylguanosine' OR cvterm.name = 'one_two_prime_O_dimethylguanosine' OR cvterm.name = 'four_demethylwyosine' OR cvterm.name = 'isowyosine' OR cvterm.name = 'N2_7_2prirme_O_trimethylguanosine' OR cvterm.name = 'queuosine' OR cvterm.name = 'epoxyqueuosine' OR cvterm.name = 'galactosyl_queuosine' OR cvterm.name = 'mannosyl_queuosine' OR cvterm.name = 'seven_cyano_seven_deazaguanosine' OR cvterm.name = 'seven_aminomethyl_seven_deazaguanosine' OR cvterm.name = 'archaeosine' OR cvterm.name = 'dihydrouridine' OR cvterm.name = 'pseudouridine' OR cvterm.name = 'five_methyluridine' OR cvterm.name = 'two_prime_O_methyluridine' OR cvterm.name = 'five_two_prime_O_dimethyluridine' OR cvterm.name = 'one_methylpseudouridine' OR cvterm.name = 'two_prime_O_methylpseudouridine' OR cvterm.name = 'two_thiouridine' OR cvterm.name = 'four_thiouridine' OR cvterm.name = 'five_methyl_2_thiouridine' OR cvterm.name = 'two_thio_two_prime_O_methyluridine' OR cvterm.name = 'three_three_amino_three_carboxypropyl_uridine' OR cvterm.name = 'five_hydroxyuridine' OR cvterm.name = 'five_methoxyuridine' OR cvterm.name = 'uridine_five_oxyacetic_acid' OR cvterm.name = 'uridine_five_oxyacetic_acid_methyl_ester' OR cvterm.name = 'five_carboxyhydroxymethyl_uridine' OR cvterm.name = 'five_carboxyhydroxymethyl_uridine_methyl_ester' OR cvterm.name = 'five_methoxycarbonylmethyluridine' OR cvterm.name = 'five_methoxycarbonylmethyl_two_prime_O_methyluridine' OR cvterm.name = 'five_methoxycarbonylmethyl_two_thiouridine' OR cvterm.name = 'five_aminomethyl_two_thiouridine' OR cvterm.name = 'five_methylaminomethyluridine' OR cvterm.name = 'five_methylaminomethyl_two_thiouridine' OR cvterm.name = 'five_methylaminomethyl_two_selenouridine' OR cvterm.name = 'five_carbamoylmethyluridine' OR cvterm.name = 'five_carbamoylmethyl_two_prime_O_methyluridine' OR cvterm.name = 'five_carboxymethylaminomethyluridine' OR cvterm.name = 'five_carboxymethylaminomethyl_two_prime_O_methyluridine' OR cvterm.name = 'five_carboxymethylaminomethyl_two_thiouridine' OR cvterm.name = 'three_methyluridine' OR cvterm.name = 'one_methyl_three_three_amino_three_carboxypropyl_pseudouridine' OR cvterm.name = 'five_carboxymethyluridine' OR cvterm.name = 'three_two_prime_O_dimethyluridine' OR cvterm.name = 'five_methyldihydrouridine' OR cvterm.name = 'three_methylpseudouridine' OR cvterm.name = 'five_taurinomethyluridine' OR cvterm.name = 'five_taurinomethyl_two_thiouridine' OR cvterm.name = 'five_isopentenylaminomethyl_uridine' OR cvterm.name = 'five_isopentenylaminomethyl_two_thiouridine' OR cvterm.name = 'five_isopentenylaminomethyl_two_prime_O_methyluridine' OR cvterm.name = 'modified_RNA_base_feature';

--- ************************************************
--- *** relation: rrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** RNA that comprises part of a ribosome, a ***
--- *** nd that can provide both structural scaf ***
--- *** folding and catalytic activity.          ***
--- ************************************************
---

CREATE VIEW rrna AS
  SELECT
    feature_id AS rrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'small_subunit_rRNA' OR cvterm.name = 'large_subunit_rRNA' OR cvterm.name = 'rRNA_18S' OR cvterm.name = 'rRNA_16S' OR cvterm.name = 'rRNA_5_8S' OR cvterm.name = 'rRNA_5S' OR cvterm.name = 'rRNA_28S' OR cvterm.name = 'rRNA_23S' OR cvterm.name = 'rRNA_25S' OR cvterm.name = 'rRNA_21S' OR cvterm.name = 'rRNA';

--- ************************************************
--- *** relation: trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Transfer RNA (tRNA) molecules are approx ***
--- *** imately 80 nucleotides in length. Their  ***
--- *** secondary structure includes four short  ***
--- *** double-helical elements and three loops  ***
--- *** (D, anti-codon, and T loops). Further hy ***
--- *** drogen bonds mediate the characteristic  ***
--- *** L-shaped molecular structure. Transfer R ***
--- *** NAs have two regions of fundamental func ***
--- *** tional importance: the anti-codon, which ***
--- ***  is responsible for specific mRNA codon  ***
--- *** recognition, and the 3' end, to which th ***
--- *** e tRNA's corresponding amino acid is att ***
--- *** ached (by aminoacyl-tRNA synthetases). T ***
--- *** ransfer RNAs cope with the degeneracy of ***
--- ***  the genetic code in two manners: having ***
--- ***  more than one tRNA (with a specific ant ***
--- *** i-codon) for a particular amino acid; an ***
--- *** d 'wobble' base-pairing, i.e. permitting ***
--- ***  non-standard base-pairing at the 3rd an ***
--- *** ti-codon position.                       ***
--- ************************************************
---

CREATE VIEW trna AS
  SELECT
    feature_id AS trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'alanyl_tRNA' OR cvterm.name = 'asparaginyl_tRNA' OR cvterm.name = 'aspartyl_tRNA' OR cvterm.name = 'cysteinyl_tRNA' OR cvterm.name = 'glutaminyl_tRNA' OR cvterm.name = 'glutamyl_tRNA' OR cvterm.name = 'glycyl_tRNA' OR cvterm.name = 'histidyl_tRNA' OR cvterm.name = 'isoleucyl_tRNA' OR cvterm.name = 'leucyl_tRNA' OR cvterm.name = 'lysyl_tRNA' OR cvterm.name = 'methionyl_tRNA' OR cvterm.name = 'phenylalanyl_tRNA' OR cvterm.name = 'prolyl_tRNA' OR cvterm.name = 'seryl_tRNA' OR cvterm.name = 'threonyl_tRNA' OR cvterm.name = 'tryptophanyl_tRNA' OR cvterm.name = 'tyrosyl_tRNA' OR cvterm.name = 'valyl_tRNA' OR cvterm.name = 'pyrrolysyl_tRNA' OR cvterm.name = 'arginyl_tRNA' OR cvterm.name = 'selenocysteinyl_tRNA' OR cvterm.name = 'tRNA';

--- ************************************************
--- *** relation: alanyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has an alanine anti ***
--- *** codon, and a 3' alanine binding region.  ***
--- ************************************************
---

CREATE VIEW alanyl_trna AS
  SELECT
    feature_id AS alanyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'alanyl_tRNA';

--- ************************************************
--- *** relation: rrna_small_subunit_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding a small ri ***
--- *** bosomal subunit RNA.                     ***
--- ************************************************
---

CREATE VIEW rrna_small_subunit_primary_transcript AS
  SELECT
    feature_id AS rrna_small_subunit_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_small_subunit_primary_transcript';

--- ************************************************
--- *** relation: asparaginyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has an asparagine a ***
--- *** nticodon, and a 3' asparagine binding re ***
--- *** gion.                                    ***
--- ************************************************
---

CREATE VIEW asparaginyl_trna AS
  SELECT
    feature_id AS asparaginyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'asparaginyl_tRNA';

--- ************************************************
--- *** relation: aspartyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has an aspartic aci ***
--- *** d anticodon, and a 3' aspartic acid bind ***
--- *** ing region.                              ***
--- ************************************************
---

CREATE VIEW aspartyl_trna AS
  SELECT
    feature_id AS aspartyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'aspartyl_tRNA';

--- ************************************************
--- *** relation: cysteinyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a cysteine anti ***
--- *** codon, and a 3' cysteine binding region. ***
--- ************************************************
---

CREATE VIEW cysteinyl_trna AS
  SELECT
    feature_id AS cysteinyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cysteinyl_tRNA';

--- ************************************************
--- *** relation: glutaminyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a glutamine ant ***
--- *** icodon, and a 3' glutamine binding regio ***
--- *** n.                                       ***
--- ************************************************
---

CREATE VIEW glutaminyl_trna AS
  SELECT
    feature_id AS glutaminyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'glutaminyl_tRNA';

--- ************************************************
--- *** relation: glutamyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a glutamic acid ***
--- ***  anticodon, and a 3' glutamic acid bindi ***
--- *** ng region.                               ***
--- ************************************************
---

CREATE VIEW glutamyl_trna AS
  SELECT
    feature_id AS glutamyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'glutamyl_tRNA';

--- ************************************************
--- *** relation: glycyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a glycine antic ***
--- *** odon, and a 3' glycine binding region.   ***
--- ************************************************
---

CREATE VIEW glycyl_trna AS
  SELECT
    feature_id AS glycyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'glycyl_tRNA';

--- ************************************************
--- *** relation: histidyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a histidine ant ***
--- *** icodon, and a 3' histidine binding regio ***
--- *** n.                                       ***
--- ************************************************
---

CREATE VIEW histidyl_trna AS
  SELECT
    feature_id AS histidyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'histidyl_tRNA';

--- ************************************************
--- *** relation: isoleucyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has an isoleucine a ***
--- *** nticodon, and a 3' isoleucine binding re ***
--- *** gion.                                    ***
--- ************************************************
---

CREATE VIEW isoleucyl_trna AS
  SELECT
    feature_id AS isoleucyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'isoleucyl_tRNA';

--- ************************************************
--- *** relation: leucyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a leucine antic ***
--- *** odon, and a 3' leucine binding region.   ***
--- ************************************************
---

CREATE VIEW leucyl_trna AS
  SELECT
    feature_id AS leucyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'leucyl_tRNA';

--- ************************************************
--- *** relation: lysyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a lysine antico ***
--- *** don, and a 3' lysine binding region.     ***
--- ************************************************
---

CREATE VIEW lysyl_trna AS
  SELECT
    feature_id AS lysyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'lysyl_tRNA';

--- ************************************************
--- *** relation: methionyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a methionine an ***
--- *** ticodon, and a 3' methionine binding reg ***
--- *** ion.                                     ***
--- ************************************************
---

CREATE VIEW methionyl_trna AS
  SELECT
    feature_id AS methionyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'methionyl_tRNA';

--- ************************************************
--- *** relation: phenylalanyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a phenylalanine ***
--- ***  anticodon, and a 3' phenylalanine bindi ***
--- *** ng region.                               ***
--- ************************************************
---

CREATE VIEW phenylalanyl_trna AS
  SELECT
    feature_id AS phenylalanyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'phenylalanyl_tRNA';

--- ************************************************
--- *** relation: prolyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a proline antic ***
--- *** odon, and a 3' proline binding region.   ***
--- ************************************************
---

CREATE VIEW prolyl_trna AS
  SELECT
    feature_id AS prolyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'prolyl_tRNA';

--- ************************************************
--- *** relation: seryl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a serine antico ***
--- *** don, and a 3' serine binding region.     ***
--- ************************************************
---

CREATE VIEW seryl_trna AS
  SELECT
    feature_id AS seryl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'seryl_tRNA';

--- ************************************************
--- *** relation: threonyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a threonine ant ***
--- *** icodon, and a 3' threonine binding regio ***
--- *** n.                                       ***
--- ************************************************
---

CREATE VIEW threonyl_trna AS
  SELECT
    feature_id AS threonyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'threonyl_tRNA';

--- ************************************************
--- *** relation: tryptophanyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a tryptophan an ***
--- *** ticodon, and a 3' tryptophan binding reg ***
--- *** ion.                                     ***
--- ************************************************
---

CREATE VIEW tryptophanyl_trna AS
  SELECT
    feature_id AS tryptophanyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tryptophanyl_tRNA';

--- ************************************************
--- *** relation: tyrosyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a tyrosine anti ***
--- *** codon, and a 3' tyrosine binding region. ***
--- ************************************************
---

CREATE VIEW tyrosyl_trna AS
  SELECT
    feature_id AS tyrosyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tyrosyl_tRNA';

--- ************************************************
--- *** relation: valyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a valine antico ***
--- *** don, and a 3' valine binding region.     ***
--- ************************************************
---

CREATE VIEW valyl_trna AS
  SELECT
    feature_id AS valyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'valyl_tRNA';

--- ************************************************
--- *** relation: snrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A small nuclear RNA molecule involved in ***
--- ***  pre-mRNA splicing and processing.       ***
--- ************************************************
---

CREATE VIEW snrna AS
  SELECT
    feature_id AS snrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U1_snRNA' OR cvterm.name = 'U2_snRNA' OR cvterm.name = 'U4_snRNA' OR cvterm.name = 'U4atac_snRNA' OR cvterm.name = 'U5_snRNA' OR cvterm.name = 'U6_snRNA' OR cvterm.name = 'U6atac_snRNA' OR cvterm.name = 'U11_snRNA' OR cvterm.name = 'U12_snRNA' OR cvterm.name = 'snRNA';

--- ************************************************
--- *** relation: snorna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A snoRNA (small nucleolar RNA) is any on ***
--- *** e of a class of small RNAs that are asso ***
--- *** ciated with the eukaryotic nucleus as co ***
--- *** mponents of small nucleolar ribonucleopr ***
--- *** oteins. They participate in the processi ***
--- *** ng or modifications of many RNAs, mostly ***
--- ***  ribosomal RNAs (rRNAs) though snoRNAs a ***
--- *** re also known to target other classes of ***
--- ***  RNA, including spliceosomal RNAs, tRNAs ***
--- *** , and mRNAs via a stretch of sequence th ***
--- *** at is complementary to a sequence in the ***
--- ***  targeted RNA.                           ***
--- ************************************************
---

CREATE VIEW snorna AS
  SELECT
    feature_id AS snorna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'C_D_box_snoRNA' OR cvterm.name = 'H_ACA_box_snoRNA' OR cvterm.name = 'U14_snoRNA' OR cvterm.name = 'U3_snoRNA' OR cvterm.name = 'methylation_guide_snoRNA' OR cvterm.name = 'pseudouridylation_guide_snoRNA' OR cvterm.name = 'snoRNA';

--- ************************************************
--- *** relation: mirna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Small, ~22-nt, RNA molecule that is the  ***

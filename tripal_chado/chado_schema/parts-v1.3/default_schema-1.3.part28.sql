SET search_path=so,chado,pg_catalog;
--- *** g platforms, and is assembled into conti ***
--- *** gs. Genome sequence of this quality may  ***
--- *** harbour regions of poor quality and can  ***
--- *** be relatively incomplete.                ***
--- ************************************************
---

CREATE VIEW standard_draft AS
  SELECT
    feature_id AS standard_draft_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'standard_draft';

--- ************************************************
--- *** relation: high_quality_draft ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The status of a whole genome sequence, w ***
--- *** here overall coverage represents at leas ***
--- *** t 90 percent of the genome.              ***
--- ************************************************
---

CREATE VIEW high_quality_draft AS
  SELECT
    feature_id AS high_quality_draft_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'high_quality_draft';

--- ************************************************
--- *** relation: improved_high_quality_draft ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The status of a whole genome sequence, w ***
--- *** here additional work has been performed, ***
--- ***  using either manual or automated method ***
--- *** s, such as gap resolution.               ***
--- ************************************************
---

CREATE VIEW improved_high_quality_draft AS
  SELECT
    feature_id AS improved_high_quality_draft_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'improved_high_quality_draft';

--- ************************************************
--- *** relation: annotation_directed_improved_draft ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The status of a whole genome sequence,wh ***
--- *** ere annotation, and verification of codi ***
--- *** ng regions has occurred.                 ***
--- ************************************************
---

CREATE VIEW annotation_directed_improved_draft AS
  SELECT
    feature_id AS annotation_directed_improved_draft_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'annotation_directed_improved_draft';

--- ************************************************
--- *** relation: noncontiguous_finished ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The status of a whole genome sequence, w ***
--- *** here the assembly is high quality, closu ***
--- *** re approaches have been successful for m ***
--- *** ost gaps, misassemblies and low quality  ***
--- *** regions.                                 ***
--- ************************************************
---

CREATE VIEW noncontiguous_finished AS
  SELECT
    feature_id AS noncontiguous_finished_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'noncontiguous_finished';

--- ************************************************
--- *** relation: finished_genome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The status of a whole genome sequence, w ***
--- *** ith less than 1 error per 100,000 base p ***
--- *** airs.                                    ***
--- ************************************************
---

CREATE VIEW finished_genome AS
  SELECT
    feature_id AS finished_genome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'finished_genome';

--- ************************************************
--- *** relation: intronic_regulatory_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A regulatory region that is part of an i ***
--- *** ntron.                                   ***
--- ************************************************
---

CREATE VIEW intronic_regulatory_region AS
  SELECT
    feature_id AS intronic_regulatory_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'intronic_regulatory_region';

--- ************************************************
--- *** relation: centromere_dna_element_i ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A centromere DNA Element I (CDEI) is a c ***
--- *** onserved region, part of the centromere, ***
--- ***  consisting of a consensus region compos ***
--- *** ed of 8-11bp which enables binding by th ***
--- *** e centromere binding factor 1(Cbf1p).    ***
--- ************************************************
---

CREATE VIEW centromere_dna_element_i AS
  SELECT
    feature_id AS centromere_dna_element_i_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'centromere_DNA_Element_I';

--- ************************************************
--- *** relation: centromere_dna_element_ii ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A centromere DNA Element II (CDEII) is p ***
--- *** art a conserved region of the centromere ***
--- *** , consisting of a consensus region that  ***
--- *** is AT-rich and ~ 75-100 bp in length.    ***
--- ************************************************
---

CREATE VIEW centromere_dna_element_ii AS
  SELECT
    feature_id AS centromere_dna_element_ii_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'centromere_DNA_Element_II';

--- ************************************************
--- *** relation: centromere_dna_element_iii ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A centromere DNA Element I (CDEI) is a c ***
--- *** onserved region, part of the centromere, ***
--- ***  consisting of a consensus region that c ***
--- *** onsists of a 25-bp which enables binding ***
--- ***  by the centromere DNA binding factor 3  ***
--- *** (CBF3) complex.                          ***
--- ************************************************
---

CREATE VIEW centromere_dna_element_iii AS
  SELECT
    feature_id AS centromere_dna_element_iii_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'centromere_DNA_Element_III';

--- ************************************************
--- *** relation: telomeric_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The telomeric repeat is a repeat region, ***
--- ***  part of the chromosome, which in yeast, ***
--- ***  is a G-rich terminal sequence of the fo ***
--- *** rm (TG(1-3))n or more precisely ((TG)(1- ***
--- *** 6)TG(2-3))n.                             ***
--- ************************************************
---

CREATE VIEW telomeric_repeat AS
  SELECT
    feature_id AS telomeric_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'telomeric_repeat';

--- ************************************************
--- *** relation: x_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The X element is a conserved region, of  ***
--- *** the telomere, of ~475 bp that contains a ***
--- *** n ARS sequence and in most cases an Abf1 ***
--- *** p binding site.                          ***
--- ************************************************
---

CREATE VIEW x_element AS
  SELECT
    feature_id AS x_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'X_element';

--- ************************************************
--- *** relation: yac_end ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of sequence from the end of a Y ***
--- *** AC clone that may provide a highly speci ***
--- *** fic marker.                              ***
--- ************************************************
---

CREATE VIEW yac_end AS
  SELECT
    feature_id AS yac_end_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'YAC_end';

--- ************************************************
--- *** relation: whole_genome_sequence_status ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The status of whole genome sequence.     ***
--- ************************************************
---

CREATE VIEW whole_genome_sequence_status AS
  SELECT
    feature_id AS whole_genome_sequence_status_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'standard_draft' OR cvterm.name = 'high_quality_draft' OR cvterm.name = 'improved_high_quality_draft' OR cvterm.name = 'annotation_directed_improved_draft' OR cvterm.name = 'noncontiguous_finished' OR cvterm.name = 'finished_genome' OR cvterm.name = 'whole_genome_sequence_status';

--- ************************************************
--- *** relation: heritable_phenotypic_marker ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A biological_region characterized as a s ***
--- *** ingle heritable trait in a phenotype scr ***
--- *** een. The heritable phenotype may be mapp ***
--- *** ed to a chromosome but generally has not ***
--- ***  been characterized to a specific gene l ***
--- *** ocus.                                    ***
--- ************************************************
---

CREATE VIEW heritable_phenotypic_marker AS
  SELECT
    feature_id AS heritable_phenotypic_marker_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'heritable_phenotypic_marker';

--- ************************************************
--- *** relation: peptide_collection ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A collection of peptide sequences.       ***
--- ************************************************
---

CREATE VIEW peptide_collection AS
  SELECT
    feature_id AS peptide_collection_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'peptide_collection';

--- ************************************************
--- *** relation: high_identity_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An experimental feature with high sequen ***
--- *** ce identity to another sequence.         ***
--- ************************************************
---

CREATE VIEW high_identity_region AS
  SELECT
    feature_id AS high_identity_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'high_identity_region';

--- ************************************************
--- *** relation: processed_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript for which no open reading f ***
--- *** rame has been identified and for which n ***
--- *** o other function has been determined.    ***
--- ************************************************
---

CREATE VIEW processed_transcript AS
  SELECT
    feature_id AS processed_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'processed_transcript';

--- ************************************************
--- *** relation: assortment_derived_variation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome variation derived from an e ***
--- *** vent during meiosis.                     ***
--- ************************************************
---

CREATE VIEW assortment_derived_variation AS
  SELECT
    feature_id AS assortment_derived_variation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'assortment_derived_duplication' OR cvterm.name = 'assortment_derived_deficiency_plus_duplication' OR cvterm.name = 'assortment_derived_deficiency' OR cvterm.name = 'assortment_derived_aneuploid' OR cvterm.name = 'assortment_derived_variation';

--- ************************************************
--- *** relation: reference_genome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A collection of sequences (often chromos ***
--- *** omes) taken as the standard for a given  ***
--- *** organism and genome assembly.            ***
--- ************************************************
---

CREATE VIEW reference_genome AS
  SELECT
    feature_id AS reference_genome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'reference_genome';

--- ************************************************
--- *** relation: variant_genome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A collection of sequences (often chromos ***
--- *** omes) of an individual.                  ***
--- ************************************************
---

CREATE VIEW variant_genome AS
  SELECT
    feature_id AS variant_genome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chromosomally_aberrant_genome' OR cvterm.name = 'variant_genome';

--- ************************************************
--- *** relation: variant_collection ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A collection of one or more sequences of ***
--- ***  an individual.                          ***
--- ************************************************
---

CREATE VIEW variant_collection AS
  SELECT
    feature_id AS variant_collection_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chromosome_variation' OR cvterm.name = 'allele' OR cvterm.name = 'haplotype' OR cvterm.name = 'genotype' OR cvterm.name = 'diplotype' OR cvterm.name = 'assortment_derived_variation' OR cvterm.name = 'chromosome_number_variation' OR cvterm.name = 'chromosome_structure_variation' OR cvterm.name = 'assortment_derived_duplication' OR cvterm.name = 'assortment_derived_deficiency_plus_duplication' OR cvterm.name = 'assortment_derived_deficiency' OR cvterm.name = 'assortment_derived_aneuploid' OR cvterm.name = 'aneuploid' OR cvterm.name = 'polyploid' OR cvterm.name = 'hyperploid' OR cvterm.name = 'hypoploid' OR cvterm.name = 'autopolyploid' OR cvterm.name = 'allopolyploid' OR cvterm.name = 'free_chromosome_arm' OR cvterm.name = 'chromosomal_transposition' OR cvterm.name = 'aneuploid_chromosome' OR cvterm.name = 'intrachromosomal_mutation' OR cvterm.name = 'interchromosomal_mutation' OR cvterm.name = 'chromosomal_duplication' OR cvterm.name = 'compound_chromosome' OR cvterm.name = 'autosynaptic_chromosome' OR cvterm.name = 'complex_chromosomal_mutation' OR cvterm.name = 'uncharacterised_chromosomal_mutation' OR cvterm.name = 'intrachromosomal_transposition' OR cvterm.name = 'interchromosomal_transposition' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'uninverted_intrachromosomal_transposition' OR cvterm.name = 'unoriented_intrachromosomal_transposition' OR cvterm.name = 'deficient_interchromosomal_transposition' OR cvterm.name = 'inverted_interchromosomal_transposition' OR cvterm.name = 'uninverted_interchromosomal_transposition' OR cvterm.name = 'unoriented_interchromosomal_transposition' OR cvterm.name = 'inversion_derived_aneuploid_chromosome' OR cvterm.name = 'chromosomal_deletion' OR cvterm.name = 'chromosomal_inversion' OR cvterm.name = 'intrachromosomal_duplication' OR cvterm.name = 'ring_chromosome' OR cvterm.name = 'chromosome_fission' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inversion_derived_bipartite_deficiency' OR cvterm.name = 'inversion_derived_deficiency_plus_duplication' OR cvterm.name = 'inversion_derived_deficiency_plus_aneuploid' OR cvterm.name = 'deficient_translocation' OR cvterm.name = 'deficient_inversion' OR cvterm.name = 'inverted_ring_chromosome' OR cvterm.name = 'pericentric_inversion' OR cvterm.name = 'paracentric_inversion' OR cvterm.name = 'inversion_cum_translocation' OR cvterm.name = 'bipartite_inversion' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'deficient_inversion' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'inversion_derived_deficiency_plus_duplication' OR cvterm.name = 'inversion_derived_bipartite_duplication' OR cvterm.name = 'inversion_derived_duplication_plus_aneuploid' OR cvterm.name = 'intrachromosomal_transposition' OR cvterm.name = 'bipartite_duplication' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'uninverted_intrachromosomal_transposition' OR cvterm.name = 'unoriented_intrachromosomal_transposition' OR cvterm.name = 'inverted_ring_chromosome' OR cvterm.name = 'free_ring_duplication' OR cvterm.name = 'chromosomal_translocation' OR cvterm.name = 'bipartite_duplication' OR cvterm.name = 'interchromosomal_transposition' OR cvterm.name = 'translocation_element' OR cvterm.name = 'Robertsonian_fusion' OR cvterm.name = 'reciprocal_chromosomal_translocation' OR cvterm.name = 'deficient_translocation' OR cvterm.name = 'inversion_cum_translocation' OR cvterm.name = 'cyclic_translocation' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'deficient_interchromosomal_transposition' OR cvterm.name = 'inverted_interchromosomal_transposition' OR cvterm.name = 'uninverted_interchromosomal_transposition' OR cvterm.name = 'unoriented_interchromosomal_transposition' OR cvterm.name = 'interchromosomal_duplication' OR cvterm.name = 'intrachromosomal_duplication' OR cvterm.name = 'free_duplication' OR cvterm.name = 'insertional_duplication' OR cvterm.name = 'inversion_derived_deficiency_plus_duplication' OR cvterm.name = 'inversion_derived_bipartite_duplication' OR cvterm.name = 'inversion_derived_duplication_plus_aneuploid' OR cvterm.name = 'intrachromosomal_transposition' OR cvterm.name = 'bipartite_duplication' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'uninverted_intrachromosomal_transposition' OR cvterm.name = 'unoriented_intrachromosomal_transposition' OR cvterm.name = 'free_ring_duplication' OR cvterm.name = 'uninverted_insertional_duplication' OR cvterm.name = 'inverted_insertional_duplication' OR cvterm.name = 'unoriented_insertional_duplication' OR cvterm.name = 'compound_chromosome_arm' OR cvterm.name = 'homo_compound_chromosome' OR cvterm.name = 'hetero_compound_chromosome' OR cvterm.name = 'dexstrosynaptic_chromosome' OR cvterm.name = 'laevosynaptic_chromosome' OR cvterm.name = 'partially_characterised_chromosomal_mutation' OR cvterm.name = 'polymorphic_sequence_variant' OR cvterm.name = 'variant_collection';

--- ************************************************
--- *** relation: alteration_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW alteration_attribute AS
  SELECT
    feature_id AS alteration_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chromosomal_variation_attribute' OR cvterm.name = 'insertion_attribute' OR cvterm.name = 'inversion_attribute' OR cvterm.name = 'translocaton_attribute' OR cvterm.name = 'duplication_attribute' OR cvterm.name = 'intrachromosomal' OR cvterm.name = 'interchromosomal' OR cvterm.name = 'tandem' OR cvterm.name = 'direct' OR cvterm.name = 'inverted' OR cvterm.name = 'pericentric' OR cvterm.name = 'paracentric' OR cvterm.name = 'reciprocal' OR cvterm.name = 'insertional' OR cvterm.name = 'free' OR cvterm.name = 'alteration_attribute';

--- ************************************************
--- *** relation: chromosomal_variation_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW chromosomal_variation_attribute AS
  SELECT
    feature_id AS chromosomal_variation_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'intrachromosomal' OR cvterm.name = 'interchromosomal' OR cvterm.name = 'chromosomal_variation_attribute';

--- ************************************************
--- *** relation: intrachromosomal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW intrachromosomal AS
  SELECT
    feature_id AS intrachromosomal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'intrachromosomal';

--- ************************************************
--- *** relation: interchromosomal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW interchromosomal AS
  SELECT
    feature_id AS interchromosomal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'interchromosomal';

--- ************************************************
--- *** relation: insertion_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A quality of a chromosomal insertion,.   ***
--- ************************************************
---

CREATE VIEW insertion_attribute AS
  SELECT
    feature_id AS insertion_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tandem' OR cvterm.name = 'direct' OR cvterm.name = 'inverted' OR cvterm.name = 'insertion_attribute';

--- ************************************************
--- *** relation: tandem ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW tandem AS
  SELECT
    feature_id AS tandem_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tandem';

--- ************************************************
--- *** relation: direct ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A quality of an insertion where the inse ***
--- *** rt is not in a cytologically inverted or ***
--- *** ientation.                               ***
--- ************************************************
---

CREATE VIEW direct AS
  SELECT
    feature_id AS direct_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'direct';

--- ************************************************
--- *** relation: inverted ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A quality of an insertion where the inse ***
--- *** rt is in a cytologically inverted orient ***
--- *** ation.                                   ***
--- ************************************************
---

CREATE VIEW inverted AS
  SELECT
    feature_id AS inverted_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inverted';

--- ************************************************
--- *** relation: free ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The quality of a duplication where the n ***
--- *** ew region exists independently of the or ***
--- *** iginal.                                  ***
--- ************************************************
---

CREATE VIEW free AS
  SELECT
    feature_id AS free_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'free';

--- ************************************************
--- *** relation: inversion_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW inversion_attribute AS
  SELECT
    feature_id AS inversion_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pericentric' OR cvterm.name = 'paracentric' OR cvterm.name = 'inversion_attribute';

--- ************************************************
--- *** relation: pericentric ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW pericentric AS
  SELECT
    feature_id AS pericentric_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pericentric';

--- ************************************************
--- *** relation: paracentric ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW paracentric AS
  SELECT
    feature_id AS paracentric_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'paracentric';

--- ************************************************
--- *** relation: translocaton_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW translocaton_attribute AS
  SELECT
    feature_id AS translocaton_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'reciprocal' OR cvterm.name = 'insertional' OR cvterm.name = 'translocaton_attribute';

--- ************************************************
--- *** relation: reciprocal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW reciprocal AS
  SELECT
    feature_id AS reciprocal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'reciprocal';

--- ************************************************
--- *** relation: insertional ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW insertional AS
  SELECT
    feature_id AS insertional_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'insertional';

--- ************************************************
--- *** relation: duplication_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW duplication_attribute AS
  SELECT
    feature_id AS duplication_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'free' OR cvterm.name = 'duplication_attribute';

--- ************************************************
--- *** relation: chromosomally_aberrant_genome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW chromosomally_aberrant_genome AS
  SELECT
    feature_id AS chromosomally_aberrant_genome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chromosomally_aberrant_genome';

--- ************************************************
--- *** relation: assembly_error_correction ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of sequence where the final nuc ***
--- *** leotide assignment differs from the orig ***
--- *** inal assembly due to an improvement that ***
--- ***  replaces a mistake.                     ***
--- ************************************************
---

CREATE VIEW assembly_error_correction AS
  SELECT
    feature_id AS assembly_error_correction_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'assembly_error_correction';

--- ************************************************
--- *** relation: base_call_error_correction ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of sequence where the final nuc ***
--- *** leotide assignment is different from tha ***
--- *** t given by the base caller due to an imp ***
--- *** rovement that replaces a mistake.        ***
--- ************************************************
---

CREATE VIEW base_call_error_correction AS
  SELECT
    feature_id AS base_call_error_correction_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'base_call_error_correction';

--- ************************************************
--- *** relation: peptide_localization_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of peptide sequence used to tar ***
--- *** get the polypeptide molecule to a specif ***
--- *** ic organelle.                            ***
--- ************************************************
---

CREATE VIEW peptide_localization_signal AS
  SELECT
    feature_id AS peptide_localization_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'signal_peptide' OR cvterm.name = 'transit_peptide' OR cvterm.name = 'nuclear_localization_signal' OR cvterm.name = 'endosomal_localization_signal' OR cvterm.name = 'lysosomal_localization_signal' OR cvterm.name = 'nuclear_export_signal' OR cvterm.name = 'nuclear_rim_localization_signal' OR cvterm.name = 'peptide_localization_signal';

--- ************************************************
--- *** relation: nuclear_localization_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A polypeptide region that targets a poly ***
--- *** peptide to the nucleus.                  ***
--- ************************************************
---

CREATE VIEW nuclear_localization_signal AS
  SELECT
    feature_id AS nuclear_localization_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nuclear_localization_signal';

--- ************************************************
--- *** relation: endosomal_localization_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A polypeptide region that targets a poly ***
--- *** peptide to the endosome.                 ***
--- ************************************************
---

CREATE VIEW endosomal_localization_signal AS
  SELECT
    feature_id AS endosomal_localization_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'endosomal_localization_signal';

--- ************************************************
--- *** relation: lysosomal_localization_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A polypeptide region that targets a poly ***
--- *** peptide to the lysosome.                 ***
--- ************************************************
---

CREATE VIEW lysosomal_localization_signal AS
  SELECT
    feature_id AS lysosomal_localization_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'lysosomal_localization_signal';

--- ************************************************
--- *** relation: nuclear_export_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A polypeptide region that targets a poly ***
--- *** peptide to he cytoplasm.                 ***
--- ************************************************
---

CREATE VIEW nuclear_export_signal AS
  SELECT
    feature_id AS nuclear_export_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nuclear_export_signal';

--- ************************************************
--- *** relation: recombination_signal_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region recognized by a recombinase.    ***
--- ************************************************
---

CREATE VIEW recombination_signal_sequence AS
  SELECT
    feature_id AS recombination_signal_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'recombination_signal_sequence';

--- ************************************************
--- *** relation: cryptic_splice_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A splice site that is in part of the tra ***
--- *** nscript not normally spliced. They occur ***
--- ***  via mutation or transcriptional error.  ***
--- ************************************************
---

CREATE VIEW cryptic_splice_site AS
  SELECT
    feature_id AS cryptic_splice_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cryptic_splice_site';

--- ************************************************
--- *** relation: nuclear_rim_localization_signal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A polypeptide region that targets a poly ***
--- *** peptide to the nuclear rim.              ***
--- ************************************************
---

CREATE VIEW nuclear_rim_localization_signal AS
  SELECT
    feature_id AS nuclear_rim_localization_signal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nuclear_rim_localization_signal';

--- ************************************************
--- *** relation: p_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A P_element is a DNA transposon responsi ***
--- *** ble for hybrid dysgenesis.               ***
--- ************************************************
---

CREATE VIEW p_element AS
  SELECT
    feature_id AS p_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'p_element';

--- ************************************************
--- *** relation: functional_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant in which the function ***
--- ***  of a gene product is altered with respe ***
--- *** ct to a reference.                       ***
--- ************************************************
---

CREATE VIEW functional_variant AS
  SELECT
    feature_id AS functional_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transcript_function_variant' OR cvterm.name = 'translational_product_function_variant' OR cvterm.name = 'level_of_transcript_variant' OR cvterm.name = 'transcript_processing_variant' OR cvterm.name = 'transcript_stability_variant' OR cvterm.name = 'transcription_variant' OR cvterm.name = 'decreased_transcript_level_variant' OR cvterm.name = 'increased_transcript_level_variant' OR cvterm.name = 'editing_variant' OR cvterm.name = 'polyadenylation_variant' OR cvterm.name = 'increased_polyadenylation_variant' OR cvterm.name = 'decreased_polyadenylation_variant' OR cvterm.name = 'decreased_transcript_stability_variant' OR cvterm.name = 'increased_transcript_stability_variant' OR cvterm.name = 'rate_of_transcription_variant' OR cvterm.name = 'increased_transcription_rate_variant' OR cvterm.name = 'decreased_transcription_rate_variant' OR cvterm.name = 'translational_product_level_variant' OR cvterm.name = 'polypeptide_function_variant' OR cvterm.name = 'decreased_translational_product_level' OR cvterm.name = 'increased_translational_product_level' OR cvterm.name = 'polypeptide_gain_of_function_variant' OR cvterm.name = 'polypeptide_localization_variant' OR cvterm.name = 'polypeptide_loss_of_function_variant' OR cvterm.name = 'polypeptide_post_translational_processing_variant' OR cvterm.name = 'inactive_ligand_binding_site' OR cvterm.name = 'polypeptide_partial_loss_of_function' OR cvterm.name = 'inactive_catalytic_site' OR cvterm.name = 'functional_variant';

--- ************************************************
--- *** relation: structural_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that changes one or m ***
--- *** ore sequence features.                   ***
--- ************************************************
---

CREATE VIEW structural_variant AS
  SELECT
    feature_id AS structural_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'silent_mutation' OR cvterm.name = 'copy_number_change' OR cvterm.name = 'gene_variant' OR cvterm.name = 'regulatory_region_variant' OR cvterm.name = 'intergenic_variant' OR cvterm.name = 'upstream_gene_variant' OR cvterm.name = 'downstream_gene_variant' OR cvterm.name = 'gene_fusion' OR cvterm.name = 'splicing_variant' OR cvterm.name = 'transcript_variant' OR cvterm.name = 'translational_product_structure_variant' OR cvterm.name = 'cryptic_splice_site_variant' OR cvterm.name = 'exon_loss' OR cvterm.name = 'intron_gain' OR cvterm.name = 'splice_region_variant' OR cvterm.name = 'cryptic_splice_acceptor' OR cvterm.name = 'cryptic_splice_donor' OR cvterm.name = 'complex_change_in_transcript' OR cvterm.name = 'transcript_secondary_structure_variant' OR cvterm.name = 'nc_transcript_variant' OR cvterm.name = 'NMD_transcript_variant' OR cvterm.name = 'UTR_variant' OR cvterm.name = 'intron_variant' OR cvterm.name = 'exon_variant' OR cvterm.name = 'compensatory_transcript_secondary_structure_variant' OR cvterm.name = 'mature_miRNA_variant' OR cvterm.name = '5_prime_UTR_variant' OR cvterm.name = '3_prime_UTR_variant' OR cvterm.name = 'splice_site_variant' OR cvterm.name = 'splice_acceptor_variant' OR cvterm.name = 'splice_donor_variant' OR cvterm.name = 'splice_donor_5th_base_variant' OR cvterm.name = 'coding_sequence_variant' OR cvterm.name = 'non_coding_exon_variant' OR cvterm.name = 'codon_variant' OR cvterm.name = 'frameshift_variant' OR cvterm.name = 'inframe_variant' OR cvterm.name = 'initiator_codon_change' OR cvterm.name = 'non_synonymous_codon' OR cvterm.name = 'synonymous_codon' OR cvterm.name = 'terminal_codon_variant' OR cvterm.name = 'stop_gained' OR cvterm.name = 'missense_codon' OR cvterm.name = 'conservative_missense_codon' OR cvterm.name = 'non_conservative_missense_codon' OR cvterm.name = 'terminator_codon_variant' OR cvterm.name = 'incomplete_terminal_codon_variant' OR cvterm.name = 'stop_retained_variant' OR cvterm.name = 'stop_lost' OR cvterm.name = 'frame_restoring_variant' OR cvterm.name = 'minus_1_frameshift_variant' OR cvterm.name = 'minus_2_frameshift_variant' OR cvterm.name = 'plus_1_frameshift_variant' OR cvterm.name = 'plus_2_frameshift variant' OR cvterm.name = 'inframe_codon_gain' OR cvterm.name = 'inframe_codon_loss' OR cvterm.name = '3D_polypeptide_structure_variant' OR cvterm.name = 'complex_change_of_translational_product_variant' OR cvterm.name = 'polypeptide_sequence_variant' OR cvterm.name = 'complex_3D_structural_variant' OR cvterm.name = 'conformational_change_variant' OR cvterm.name = 'amino_acid_deletion' OR cvterm.name = 'amino_acid_insertion' OR cvterm.name = 'amino_acid_substitution' OR cvterm.name = 'elongated_polypeptide' OR cvterm.name = 'polypeptide_fusion' OR cvterm.name = 'polypeptide_truncation' OR cvterm.name = 'conservative_amino_acid_substitution' OR cvterm.name = 'non_conservative_amino_acid_substitution' OR cvterm.name = 'elongated_polypeptide_C_terminal' OR cvterm.name = 'elongated_polypeptide_N_terminal' OR cvterm.name = 'elongated_in_frame_polypeptide_C_terminal' OR cvterm.name = 'elongated_out_of_frame_polypeptide_C_terminal' OR cvterm.name = 'elongated_in_frame_polypeptide_N_terminal_elongation' OR cvterm.name = 'elongated_out_of_frame_polypeptide_N_terminal' OR cvterm.name = 'TF_binding_site_variant' OR cvterm.name = '5KB_upstream_variant' OR cvterm.name = '2KB_upstream_variant' OR cvterm.name = '5KB_downstream_variant' OR cvterm.name = '500B_downstream_variant' OR cvterm.name = 'structural_variant';

--- ************************************************
--- *** relation: transcript_function_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant which alters the func ***
--- *** tioning of a transcript with respect to  ***
--- *** a reference sequence.                    ***
--- ************************************************
---

CREATE VIEW transcript_function_variant AS
  SELECT
    feature_id AS transcript_function_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'level_of_transcript_variant' OR cvterm.name = 'transcript_processing_variant' OR cvterm.name = 'transcript_stability_variant' OR cvterm.name = 'transcription_variant' OR cvterm.name = 'decreased_transcript_level_variant' OR cvterm.name = 'increased_transcript_level_variant' OR cvterm.name = 'editing_variant' OR cvterm.name = 'polyadenylation_variant' OR cvterm.name = 'increased_polyadenylation_variant' OR cvterm.name = 'decreased_polyadenylation_variant' OR cvterm.name = 'decreased_transcript_stability_variant' OR cvterm.name = 'increased_transcript_stability_variant' OR cvterm.name = 'rate_of_transcription_variant' OR cvterm.name = 'increased_transcription_rate_variant' OR cvterm.name = 'decreased_transcription_rate_variant' OR cvterm.name = 'transcript_function_variant';

--- ************************************************
--- *** relation: translational_product_function_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that affects the func ***
--- *** tioning of a translational product with  ***
--- *** respect to a reference sequence.         ***
--- ************************************************
---

CREATE VIEW translational_product_function_variant AS
  SELECT
    feature_id AS translational_product_function_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'translational_product_level_variant' OR cvterm.name = 'polypeptide_function_variant' OR cvterm.name = 'decreased_translational_product_level' OR cvterm.name = 'increased_translational_product_level' OR cvterm.name = 'polypeptide_gain_of_function_variant' OR cvterm.name = 'polypeptide_localization_variant' OR cvterm.name = 'polypeptide_loss_of_function_variant' OR cvterm.name = 'polypeptide_post_translational_processing_variant' OR cvterm.name = 'inactive_ligand_binding_site' OR cvterm.name = 'polypeptide_partial_loss_of_function' OR cvterm.name = 'inactive_catalytic_site' OR cvterm.name = 'translational_product_function_variant';

--- ************************************************
--- *** relation: level_of_transcript_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant which alters the leve ***
--- *** l of a transcript.                       ***
--- ************************************************
---

CREATE VIEW level_of_transcript_variant AS
  SELECT
    feature_id AS level_of_transcript_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'decreased_transcript_level_variant' OR cvterm.name = 'increased_transcript_level_variant' OR cvterm.name = 'level_of_transcript_variant';

--- ************************************************
--- *** relation: decreased_transcript_level_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that increases the le ***
--- *** vel of mature, spliced and processed RNA ***
--- ***  with respect to a reference sequence.   ***
--- ************************************************
---

CREATE VIEW decreased_transcript_level_variant AS
  SELECT
    feature_id AS decreased_transcript_level_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'decreased_transcript_level_variant';

--- ************************************************
--- *** relation: increased_transcript_level_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that increases the le ***
--- *** vel of mature, spliced and processed RNA ***
--- ***  with respect to a reference sequence.   ***
--- ************************************************
---

CREATE VIEW increased_transcript_level_variant AS
  SELECT
    feature_id AS increased_transcript_level_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'increased_transcript_level_variant';

--- ************************************************
--- *** relation: transcript_processing_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that affects the post ***
--- ***  transcriptional processing of a transcr ***
--- *** ipt with respect to a reference sequence ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW transcript_processing_variant AS
  SELECT
    feature_id AS transcript_processing_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'editing_variant' OR cvterm.name = 'polyadenylation_variant' OR cvterm.name = 'increased_polyadenylation_variant' OR cvterm.name = 'decreased_polyadenylation_variant' OR cvterm.name = 'transcript_processing_variant';

--- ************************************************
--- *** relation: editing_variant ***

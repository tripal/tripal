SET search_path=so,chado,pg_catalog;
--- *** relation: cyanelle_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW cyanelle_sequence AS
  SELECT
    feature_id AS cyanelle_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cyanelle_sequence';

--- ************************************************
--- *** relation: leucoplast_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW leucoplast_sequence AS
  SELECT
    feature_id AS leucoplast_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'leucoplast_sequence';

--- ************************************************
--- *** relation: proplastid_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW proplastid_sequence AS
  SELECT
    feature_id AS proplastid_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'proplastid_sequence';

--- ************************************************
--- *** relation: plasmid_location ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW plasmid_location AS
  SELECT
    feature_id AS plasmid_location_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'plasmid_location';

--- ************************************************
--- *** relation: amplification_origin ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An origin_of_replication that is used fo ***
--- *** r the amplification of a chromosomal nuc ***
--- *** leic acid sequence.                      ***
--- ************************************************
---

CREATE VIEW amplification_origin AS
  SELECT
    feature_id AS amplification_origin_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'amplification_origin';

--- ************************************************
--- *** relation: proviral_location ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW proviral_location AS
  SELECT
    feature_id AS proviral_location_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'endogenous_retroviral_sequence' OR cvterm.name = 'proviral_location';

--- ************************************************
--- *** relation: gene_group_regulatory_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW gene_group_regulatory_region AS
  SELECT
    feature_id AS gene_group_regulatory_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'operator' OR cvterm.name = 'bacterial_RNApol_promoter' OR cvterm.name = 'bacterial_terminator' OR cvterm.name = 'bacterial_RNApol_promoter_sigma_70' OR cvterm.name = 'bacterial_RNApol_promoter_sigma54' OR cvterm.name = 'rho_dependent_bacterial_terminator' OR cvterm.name = 'rho_independent_bacterial_terminator' OR cvterm.name = 'gene_group_regulatory_region';

--- ************************************************
--- *** relation: clone_insert ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The region of sequence that has been ins ***
--- *** erted and is being propagated by the clo ***
--- *** ne.                                      ***
--- ************************************************
---

CREATE VIEW clone_insert AS
  SELECT
    feature_id AS clone_insert_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cloned_cDNA_insert' OR cvterm.name = 'cloned_genomic_insert' OR cvterm.name = 'engineered_insert' OR cvterm.name = 'BAC_cloned_genomic_insert' OR cvterm.name = 'clone_insert';

--- ************************************************
--- *** relation: lambda_vector ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The lambda bacteriophage is the vector f ***
--- *** or the linear lambda clone. The genes in ***
--- *** volved in the lysogenic pathway are remo ***
--- *** ved from the from the viral DNA. Up to 2 ***
--- *** 5 kb of foreign DNA can then be inserted ***
--- ***  into the lambda genome.                 ***
--- ************************************************
---

CREATE VIEW lambda_vector AS
  SELECT
    feature_id AS lambda_vector_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'lambda_vector';

--- ************************************************
--- *** relation: plasmid_vector ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW plasmid_vector AS
  SELECT
    feature_id AS plasmid_vector_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'plasmid_vector';

--- ************************************************
--- *** relation: cdna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** DNA synthesized by reverse transcriptase ***
--- ***  using RNA as a template.                ***
--- ************************************************
---

CREATE VIEW cdna AS
  SELECT
    feature_id AS cdna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'single_stranded_cDNA' OR cvterm.name = 'double_stranded_cDNA' OR cvterm.name = 'cDNA';

--- ************************************************
--- *** relation: single_stranded_cdna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW single_stranded_cdna AS
  SELECT
    feature_id AS single_stranded_cdna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'single_stranded_cDNA';

--- ************************************************
--- *** relation: double_stranded_cdna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW double_stranded_cdna AS
  SELECT
    feature_id AS double_stranded_cdna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'double_stranded_cDNA';

--- ************************************************
--- *** relation: pyrrolysyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a pyrrolysine a ***
--- *** nticodon, and a 3' pyrrolysine binding r ***
--- *** egion.                                   ***
--- ************************************************
---

CREATE VIEW pyrrolysyl_trna AS
  SELECT
    feature_id AS pyrrolysyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pyrrolysyl_tRNA';

--- ************************************************
--- *** relation: episome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A plasmid that may integrate with a chro ***
--- *** mosome.                                  ***
--- ************************************************
---

CREATE VIEW episome AS
  SELECT
    feature_id AS episome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_episome' OR cvterm.name = 'episome';

--- ************************************************
--- *** relation: tmrna_coding_piece ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The region of a two-piece tmRNA that bea ***
--- *** rs the reading frame encoding the proteo ***
--- *** lysis tag. The tmRNA gene undergoes circ ***
--- *** ular permutation in some groups of bacte ***
--- *** ria. Processing of the transcripts from  ***
--- *** such a gene leaves the mature tmRNA in t ***
--- *** wo pieces, base-paired together.         ***
--- ************************************************
---

CREATE VIEW tmrna_coding_piece AS
  SELECT
    feature_id AS tmrna_coding_piece_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tmRNA_coding_piece';

--- ************************************************
--- *** relation: tmrna_acceptor_piece ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The acceptor region of a two-piece tmRNA ***
--- ***  that when mature is charged at its 3' e ***
--- *** nd with alanine. The tmRNA gene undergoe ***
--- *** s circular permutation in some groups of ***
--- ***  bacteria; processing of the transcripts ***
--- ***  from such a gene leaves the mature tmRN ***
--- *** A in two pieces, base-paired together.   ***
--- ************************************************
---

CREATE VIEW tmrna_acceptor_piece AS
  SELECT
    feature_id AS tmrna_acceptor_piece_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tmRNA_acceptor_piece';

--- ************************************************
--- *** relation: qtl ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A quantitative trait locus (QTL) is a po ***
--- *** lymorphic locus which contains alleles t ***
--- *** hat differentially affect the expression ***
--- ***  of a continuously distributed phenotypi ***
--- *** c trait. Usually it is a marker describe ***
--- *** d by statistical association to quantita ***
--- *** tive variation in the particular phenoty ***
--- *** pic trait that is thought to be controll ***
--- *** ed by the cumulative action of alleles a ***
--- *** t multiple loci.                         ***
--- ************************************************
---

CREATE VIEW qtl AS
  SELECT
    feature_id AS qtl_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'QTL';

--- ************************************************
--- *** relation: genomic_island ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A genomic island is an integrated mobile ***
--- ***  genetic element, characterized by size  ***
--- *** (over 10 Kb). It that has features that  ***
--- *** suggest a foreign origin. These can incl ***
--- *** ude nucleotide distribution (oligonucleo ***
--- *** tides signature, CG content etc.) that d ***
--- *** iffers from the bulk of the chromosome a ***
--- *** nd/or genes suggesting DNA mobility.     ***
--- ************************************************
---

CREATE VIEW genomic_island AS
  SELECT
    feature_id AS genomic_island_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pathogenic_island' OR cvterm.name = 'metabolic_island' OR cvterm.name = 'adaptive_island' OR cvterm.name = 'symbiosis_island' OR cvterm.name = 'cryptic_prophage' OR cvterm.name = 'defective_conjugative_transposon' OR cvterm.name = 'genomic_island';

--- ************************************************
--- *** relation: pathogenic_island ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Mobile genetic elements that contribute  ***
--- *** to rapid changes in virulence potential. ***
--- ***  They are present on the genomes of path ***
--- *** ogenic strains but absent from the genom ***
--- *** es of non pathogenic members of the same ***
--- ***  or related species.                     ***
--- ************************************************
---

CREATE VIEW pathogenic_island AS
  SELECT
    feature_id AS pathogenic_island_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pathogenic_island';

--- ************************************************
--- *** relation: metabolic_island ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transmissible element containing genes ***
--- ***  involved in metabolism, analogous to th ***
--- *** e pathogenicity islands of gram negative ***
--- ***  bacteria.                               ***
--- ************************************************
---

CREATE VIEW metabolic_island AS
  SELECT
    feature_id AS metabolic_island_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'metabolic_island';

--- ************************************************
--- *** relation: adaptive_island ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An adaptive island is a genomic island t ***
--- *** hat provides an adaptive advantage to th ***
--- *** e host.                                  ***
--- ************************************************
---

CREATE VIEW adaptive_island AS
  SELECT
    feature_id AS adaptive_island_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'adaptive_island';

--- ************************************************
--- *** relation: symbiosis_island ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transmissible element containing genes ***
--- ***  involved in symbiosis, analogous to the ***
--- ***  pathogenicity islands of gram negative  ***
--- *** bacteria.                                ***
--- ************************************************
---

CREATE VIEW symbiosis_island AS
  SELECT
    feature_id AS symbiosis_island_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'symbiosis_island';

--- ************************************************
--- *** relation: pseudogenic_rrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A non functional descendent of an rRNA.  ***
--- ************************************************
---

CREATE VIEW pseudogenic_rrna AS
  SELECT
    feature_id AS pseudogenic_rrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pseudogenic_rRNA';

--- ************************************************
--- *** relation: pseudogenic_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A non functional descendent of a tRNA.   ***
--- ************************************************
---

CREATE VIEW pseudogenic_trna AS
  SELECT
    feature_id AS pseudogenic_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pseudogenic_tRNA';

--- ************************************************
--- *** relation: engineered_episome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An episome that is engineered.           ***
--- ************************************************
---

CREATE VIEW engineered_episome AS
  SELECT
    feature_id AS engineered_episome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_episome';

--- ************************************************
--- *** relation: transgenic ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Attribute describing sequence that has b ***
--- *** een integrated with foreign sequence.    ***
--- ************************************************
---

CREATE VIEW transgenic AS
  SELECT
    feature_id AS transgenic_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transgenic';

--- ************************************************
--- *** relation: so_natural ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a feature that o ***
--- *** ccurs in nature.                         ***
--- ************************************************
---

CREATE VIEW so_natural AS
  SELECT
    feature_id AS so_natural_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'natural';

--- ************************************************
--- *** relation: engineered ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a region that w ***
--- *** as modified in vitro.                    ***
--- ************************************************
---

CREATE VIEW engineered AS
  SELECT
    feature_id AS engineered_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered';

--- ************************************************
--- *** relation: so_foreign ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a region from a ***
--- *** nother species.                          ***
--- ************************************************
---

CREATE VIEW so_foreign AS
  SELECT
    feature_id AS so_foreign_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'foreign';

--- ************************************************
--- *** relation: cloned_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW cloned_region AS
  SELECT
    feature_id AS cloned_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cloned_region';

--- ************************************************
--- *** relation: validated ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a feature that  ***
--- *** has been proven.                         ***
--- ************************************************
---

CREATE VIEW validated AS
  SELECT
    feature_id AS validated_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'experimentally_determined' OR cvterm.name = 'validated';

--- ************************************************
--- *** relation: invalidated ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a feature that i ***
--- *** s invalidated.                           ***
--- ************************************************
---

CREATE VIEW invalidated AS
  SELECT
    feature_id AS invalidated_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'invalidated_by_chimeric_cDNA' OR cvterm.name = 'invalidated_by_genomic_contamination' OR cvterm.name = 'invalidated_by_genomic_polyA_primed_cDNA' OR cvterm.name = 'invalidated_by_partial_processing' OR cvterm.name = 'invalidated';

--- ************************************************
--- *** relation: engineered_rescue_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A rescue region that is engineered.      ***
--- ************************************************
---

CREATE VIEW engineered_rescue_region AS
  SELECT
    feature_id AS engineered_rescue_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_rescue_region';

--- ************************************************
--- *** relation: rescue_mini_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A mini_gene that rescues.                ***
--- ************************************************
---

CREATE VIEW rescue_mini_gene AS
  SELECT
    feature_id AS rescue_mini_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rescue_mini_gene';

--- ************************************************
--- *** relation: transgenic_transposable_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** TE that has been modified in vitro, incl ***
--- *** uding insertion of DNA derived from a so ***
--- *** urce other than the originating TE.      ***
--- ************************************************
---

CREATE VIEW transgenic_transposable_element AS
  SELECT
    feature_id AS transgenic_transposable_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transgenic_transposable_element';

--- ************************************************
--- *** relation: natural_transposable_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** TE that exists (or existed) in nature.   ***
--- ************************************************
---

CREATE VIEW natural_transposable_element AS
  SELECT
    feature_id AS natural_transposable_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'natural_transposable_element';

--- ************************************************
--- *** relation: engineered_transposable_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** TE that has been modified by manipulatio ***
--- *** ns in vitro.                             ***
--- ************************************************
---

CREATE VIEW engineered_transposable_element AS
  SELECT
    feature_id AS engineered_transposable_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'engineered_transposable_element';

--- ************************************************
--- *** relation: engineered_foreign_transposable_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transposable_element that is engineere ***
--- *** d and foreign.                           ***
--- ************************************************
---

CREATE VIEW engineered_foreign_transposable_element AS
  SELECT
    feature_id AS engineered_foreign_transposable_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_foreign_transposable_element';

--- ************************************************
--- *** relation: assortment_derived_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A multi-chromosome duplication aberratio ***
--- *** n generated by reassortment of other abe ***
--- *** rration components.                      ***
--- ************************************************
---

CREATE VIEW assortment_derived_duplication AS
  SELECT
    feature_id AS assortment_derived_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'assortment_derived_duplication';

--- ************************************************
--- *** relation: assortment_derived_deficiency_plus_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A multi-chromosome aberration generated  ***
--- *** by reassortment of other aberration comp ***
--- *** onents; presumed to have a deficiency an ***
--- *** d a duplication.                         ***
--- ************************************************
---

CREATE VIEW assortment_derived_deficiency_plus_duplication AS
  SELECT
    feature_id AS assortment_derived_deficiency_plus_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'assortment_derived_deficiency_plus_duplication';

--- ************************************************
--- *** relation: assortment_derived_deficiency ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A multi-chromosome deficiency aberration ***
--- ***  generated by reassortment of other aber ***
--- *** ration components.                       ***
--- ************************************************
---

CREATE VIEW assortment_derived_deficiency AS
  SELECT
    feature_id AS assortment_derived_deficiency_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'assortment_derived_deficiency';

--- ************************************************
--- *** relation: assortment_derived_aneuploid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A multi-chromosome aberration generated  ***
--- *** by reassortment of other aberration comp ***
--- *** onents; presumed to have a deficiency or ***
--- ***  a duplication.                          ***
--- ************************************************
---

CREATE VIEW assortment_derived_aneuploid AS
  SELECT
    feature_id AS assortment_derived_aneuploid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'assortment_derived_aneuploid';

--- ************************************************
--- *** relation: engineered_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region that is engineered.             ***
--- ************************************************
---

CREATE VIEW engineered_region AS
  SELECT
    feature_id AS engineered_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_gene' OR cvterm.name = 'engineered_plasmid' OR cvterm.name = 'engineered_rescue_region' OR cvterm.name = 'engineered_transposable_element' OR cvterm.name = 'engineered_foreign_region' OR cvterm.name = 'engineered_tag' OR cvterm.name = 'engineered_insert' OR cvterm.name = 'targeting_vector' OR cvterm.name = 'engineered_foreign_gene' OR cvterm.name = 'engineered_fusion_gene' OR cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'engineered_episome' OR cvterm.name = 'gene_trap_construct' OR cvterm.name = 'promoter_trap_construct' OR cvterm.name = 'enhancer_trap_construct' OR cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'engineered_foreign_gene' OR cvterm.name = 'engineered_foreign_repetitive_element' OR cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'engineered_region';

--- ************************************************
--- *** relation: engineered_foreign_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region that is engineered and foreign. ***
--- ************************************************
---

CREATE VIEW engineered_foreign_region AS
  SELECT
    feature_id AS engineered_foreign_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_foreign_gene' OR cvterm.name = 'engineered_foreign_repetitive_element' OR cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'engineered_foreign_region';

--- ************************************************
--- *** relation: fusion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW fusion AS
  SELECT
    feature_id AS fusion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'fusion';

--- ************************************************
--- *** relation: engineered_tag ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tag that is engineered.                ***
--- ************************************************
---

CREATE VIEW engineered_tag AS
  SELECT
    feature_id AS engineered_tag_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_tag';

--- ************************************************
--- *** relation: validated_cdna_clone ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cDNA clone that has been validated.    ***
--- ************************************************
---

CREATE VIEW validated_cdna_clone AS
  SELECT
    feature_id AS validated_cdna_clone_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'validated_cDNA_clone';

--- ************************************************
--- *** relation: invalidated_cdna_clone ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cDNA clone that is invalid.            ***
--- ************************************************
---

CREATE VIEW invalidated_cdna_clone AS
  SELECT
    feature_id AS invalidated_cdna_clone_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chimeric_cDNA_clone' OR cvterm.name = 'genomically_contaminated_cDNA_clone' OR cvterm.name = 'polyA_primed_cDNA_clone' OR cvterm.name = 'partially_processed_cDNA_clone' OR cvterm.name = 'invalidated_cDNA_clone';

--- ************************************************
--- *** relation: chimeric_cdna_clone ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cDNA clone invalidated because it is c ***
--- *** himeric.                                 ***
--- ************************************************
---

CREATE VIEW chimeric_cdna_clone AS
  SELECT
    feature_id AS chimeric_cdna_clone_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chimeric_cDNA_clone';

--- ************************************************
--- *** relation: genomically_contaminated_cdna_clone ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cDNA clone invalidated by genomic cont ***
--- *** amination.                               ***
--- ************************************************
---

CREATE VIEW genomically_contaminated_cdna_clone AS
  SELECT
    feature_id AS genomically_contaminated_cdna_clone_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'genomically_contaminated_cDNA_clone';

--- ************************************************
--- *** relation: polya_primed_cdna_clone ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cDNA clone invalidated by polyA primin ***
--- *** g.                                       ***
--- ************************************************
---

CREATE VIEW polya_primed_cdna_clone AS
  SELECT
    feature_id AS polya_primed_cdna_clone_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polyA_primed_cDNA_clone';

--- ************************************************
--- *** relation: partially_processed_cdna_clone ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cDNA invalidated clone by partial proc ***
--- *** essing.                                  ***
--- ************************************************
---

CREATE VIEW partially_processed_cdna_clone AS
  SELECT
    feature_id AS partially_processed_cdna_clone_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'partially_processed_cDNA_clone';

--- ************************************************
--- *** relation: rescue ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a region's abili ***
--- *** ty, when introduced to a mutant organism ***
--- *** , to re-establish (rescue) a phenotype.  ***
--- ************************************************
---

CREATE VIEW rescue AS
  SELECT
    feature_id AS rescue_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rescue';

--- ************************************************
--- *** relation: mini_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** By definition, minigenes are short open- ***
--- *** reading frames (ORF), usually encoding a ***
--- *** pproximately 9 to 20 amino acids, which  ***
--- *** are expressed in vivo (as distinct from  ***
--- *** being synthesized as peptide or protein  ***
--- *** ex vivo and subsequently injected). The  ***
--- *** in vivo synthesis confers a distinct adv ***
--- *** antage: the expressed sequences can ente ***
--- *** r both antigen presentation pathways, MH ***
--- *** C I (inducing CD8+ T- cells, which are u ***
--- *** sually cytotoxic T-lymphocytes (CTL)) an ***
--- *** d MHC II (inducing CD4+ T-cells, usually ***
--- ***  'T-helpers' (Th)); and can encounter B- ***
--- *** cells, inducing antibody responses. Thre ***
--- *** e main vector approaches have been used  ***
--- *** to deliver minigenes: viral vectors, bac ***
--- *** terial vectors and plasmid DNA.          ***
--- ************************************************
---

CREATE VIEW mini_gene AS
  SELECT
    feature_id AS mini_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rescue_mini_gene' OR cvterm.name = 'mini_gene';

--- ************************************************
--- *** relation: rescue_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that rescues.                     ***
--- ************************************************
---

CREATE VIEW rescue_gene AS
  SELECT
    feature_id AS rescue_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'wild_type_rescue_gene' OR cvterm.name = 'rescue_gene';

--- ************************************************
--- *** relation: wild_type ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing sequence with th ***
--- *** e genotype found in nature and/or standa ***
--- *** rd laboratory stock.                     ***
--- ************************************************
---

CREATE VIEW wild_type AS
  SELECT
    feature_id AS wild_type_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'wild_type';

--- ************************************************
--- *** relation: wild_type_rescue_gene ***
--- *** relation type: VIEW                      ***

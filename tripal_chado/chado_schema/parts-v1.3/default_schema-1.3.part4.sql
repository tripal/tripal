SET search_path=so,chado,pg_catalog;
--- *** relation: linkage_group ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A group of loci that can be grouped in a ***
--- ***  linear order representing the different ***
--- ***  degrees of linkage among the genes conc ***
--- *** erned.                                   ***
--- ************************************************
---

CREATE VIEW linkage_group AS
  SELECT
    feature_id AS linkage_group_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'linkage_group';

--- ************************************************
--- *** relation: rna_internal_loop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of double stranded RNA where th ***
--- *** e bases do not conform to WC base pairin ***
--- *** g. The loop is closed on both sides by c ***
--- *** anonical base pairing. If the interrupti ***
--- *** on to base pairing occurs on one strand  ***
--- *** only, it is known as a bulge.            ***
--- ************************************************
---

CREATE VIEW rna_internal_loop AS
  SELECT
    feature_id AS rna_internal_loop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'asymmetric_RNA_internal_loop' OR cvterm.name = 'symmetric_RNA_internal_loop' OR cvterm.name = 'K_turn_RNA_motif' OR cvterm.name = 'sarcin_like_RNA_motif' OR cvterm.name = 'RNA_internal_loop';

--- ************************************************
--- *** relation: asymmetric_rna_internal_loop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An internal RNA loop where one of the st ***
--- *** rands includes more bases than the corre ***
--- *** sponding region on the other strand.     ***
--- ************************************************
---

CREATE VIEW asymmetric_rna_internal_loop AS
  SELECT
    feature_id AS asymmetric_rna_internal_loop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'K_turn_RNA_motif' OR cvterm.name = 'sarcin_like_RNA_motif' OR cvterm.name = 'asymmetric_RNA_internal_loop';

--- ************************************************
--- *** relation: a_minor_rna_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region forming a motif, composed of ad ***
--- *** enines, where the minor groove edges are ***
--- ***  inserted into the minor groove of anoth ***
--- *** er helix.                                ***
--- ************************************************
---

CREATE VIEW a_minor_rna_motif AS
  SELECT
    feature_id AS a_minor_rna_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'A_minor_RNA_motif';

--- ************************************************
--- *** relation: k_turn_rna_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The kink turn (K-turn) is an RNA structu ***
--- *** ral motif that creates a sharp (~120 deg ***
--- *** ree) bend between two continuous helices ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW k_turn_rna_motif AS
  SELECT
    feature_id AS k_turn_rna_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'K_turn_RNA_motif';

--- ************************************************
--- *** relation: sarcin_like_rna_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A loop in ribosomal RNA containing the s ***
--- *** ites of attack for ricin and sarcin.     ***
--- ************************************************
---

CREATE VIEW sarcin_like_rna_motif AS
  SELECT
    feature_id AS sarcin_like_rna_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'sarcin_like_RNA_motif';

--- ************************************************
--- *** relation: symmetric_rna_internal_loop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An internal RNA loop where the extent of ***
--- ***  the loop on both stands is the same siz ***
--- *** e.                                       ***
--- ************************************************
---

CREATE VIEW symmetric_rna_internal_loop AS
  SELECT
    feature_id AS symmetric_rna_internal_loop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'symmetric_RNA_internal_loop';

--- ************************************************
--- *** relation: rna_junction_loop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW rna_junction_loop AS
  SELECT
    feature_id AS rna_junction_loop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNA_hook_turn' OR cvterm.name = 'RNA_junction_loop';

--- ************************************************
--- *** relation: rna_hook_turn ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW rna_hook_turn AS
  SELECT
    feature_id AS rna_hook_turn_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNA_hook_turn';

--- ************************************************
--- *** relation: base_pair ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW base_pair AS
  SELECT
    feature_id AS base_pair_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'WC_base_pair' OR cvterm.name = 'sugar_edge_base_pair' OR cvterm.name = 'Hoogsteen_base_pair' OR cvterm.name = 'reverse_Hoogsteen_base_pair' OR cvterm.name = 'wobble_base_pair' OR cvterm.name = 'base_pair';

--- ************************************************
--- *** relation: wc_base_pair ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The canonical base pair, where two bases ***
--- ***  interact via WC edges, with glycosidic  ***
--- *** bonds oriented cis relative to the axis  ***
--- *** of orientation.                          ***
--- ************************************************
---

CREATE VIEW wc_base_pair AS
  SELECT
    feature_id AS wc_base_pair_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'WC_base_pair';

--- ************************************************
--- *** relation: sugar_edge_base_pair ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A type of non-canonical base-pairing.    ***
--- ************************************************
---

CREATE VIEW sugar_edge_base_pair AS
  SELECT
    feature_id AS sugar_edge_base_pair_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'sugar_edge_base_pair';

--- ************************************************
--- *** relation: aptamer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** DNA or RNA molecules that have been sele ***
--- *** cted from random pools based on their ab ***
--- *** ility to bind other molecules.           ***
--- ************************************************
---

CREATE VIEW aptamer AS
  SELECT
    feature_id AS aptamer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DNA_aptamer' OR cvterm.name = 'RNA_aptamer' OR cvterm.name = 'aptamer';

--- ************************************************
--- *** relation: dna_aptamer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** DNA molecules that have been selected fr ***
--- *** om random pools based on their ability t ***
--- *** o bind other molecules.                  ***
--- ************************************************
---

CREATE VIEW dna_aptamer AS
  SELECT
    feature_id AS dna_aptamer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DNA_aptamer';

--- ************************************************
--- *** relation: rna_aptamer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** RNA molecules that have been selected fr ***
--- *** om random pools based on their ability t ***
--- *** o bind other molecules.                  ***
--- ************************************************
---

CREATE VIEW rna_aptamer AS
  SELECT
    feature_id AS rna_aptamer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNA_aptamer';

--- ************************************************
--- *** relation: morpholino_oligo ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Morpholino oligos are synthesized from f ***
--- *** our different Morpholino subunits, each  ***
--- *** of which contains one of the four geneti ***
--- *** c bases (A, C, G, T) linked to a 6-membe ***
--- *** red morpholine ring. Eighteen to 25 subu ***
--- *** nits of these four subunit types are joi ***
--- *** ned in a specific order by non-ionic pho ***
--- *** sphorodiamidate intersubunit linkages to ***
--- ***  give a Morpholino.                      ***
--- ************************************************
---

CREATE VIEW morpholino_oligo AS
  SELECT
    feature_id AS morpholino_oligo_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'morpholino_oligo';

--- ************************************************
--- *** relation: riboswitch ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A riboswitch is a part of an mRNA that c ***
--- *** an act as a direct sensor of small molec ***
--- *** ules to control their own expression. A  ***
--- *** riboswitch is a cis element in the 5' en ***
--- *** d of an mRNA, that acts as a direct sens ***
--- *** or of metabolites.                       ***
--- ************************************************
---

CREATE VIEW riboswitch AS
  SELECT
    feature_id AS riboswitch_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'riboswitch';

--- ************************************************
--- *** relation: matrix_attachment_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A DNA region that is required for the bi ***
--- *** nding of chromatin to the nuclear matrix ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW matrix_attachment_site AS
  SELECT
    feature_id AS matrix_attachment_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'matrix_attachment_site';

--- ************************************************
--- *** relation: locus_control_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A DNA region that includes DNAse hyperse ***
--- *** nsitive sites located 5' to a gene that  ***
--- *** confers the high-level, position-indepen ***
--- *** dent, and copy number-dependent expressi ***
--- *** on to that gene.                         ***
--- ************************************************
---

CREATE VIEW locus_control_region AS
  SELECT
    feature_id AS locus_control_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'locus_control_region';

--- ************************************************
--- *** relation: match_part ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A part of a match, for example an hsp fr ***
--- *** om blast is a match_part.                ***
--- ************************************************
---

CREATE VIEW match_part AS
  SELECT
    feature_id AS match_part_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'match_part';

--- ************************************************
--- *** relation: genomic_clone ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A clone of a DNA region of a genome.     ***
--- ************************************************
---

CREATE VIEW genomic_clone AS
  SELECT
    feature_id AS genomic_clone_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'genomic_clone';

--- ************************************************
--- *** relation: processed_pseudogene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A pseudogene where by an mRNA was retrot ***
--- *** ransposed. The mRNA sequence is transcri ***
--- *** bed back into the genome, lacking intron ***
--- *** s and promoters, but often including a p ***
--- *** olyA tail.                               ***
--- ************************************************
---

CREATE VIEW processed_pseudogene AS
  SELECT
    feature_id AS processed_pseudogene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'processed_pseudogene';

--- ************************************************
--- *** relation: pseudogene_by_unequal_crossing_over ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A pseudogene caused by unequal crossing  ***
--- *** over at recombination.                   ***
--- ************************************************
---

CREATE VIEW pseudogene_by_unequal_crossing_over AS
  SELECT
    feature_id AS pseudogene_by_unequal_crossing_over_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pseudogene_by_unequal_crossing_over';

--- ************************************************
--- *** relation: probe ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A DNA sequence used experimentally to de ***
--- *** tect the presence or absence of a comple ***
--- *** mentary nucleic acid.                    ***
--- ************************************************
---

CREATE VIEW probe AS
  SELECT
    feature_id AS probe_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'microarray_oligo' OR cvterm.name = 'probe';

--- ************************************************
--- *** relation: aneuploid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of chromosome variation where the ***
--- ***  chromosome complement is not an exact m ***
--- *** ultiple of the haploid number.           ***
--- ************************************************
---

CREATE VIEW aneuploid AS
  SELECT
    feature_id AS aneuploid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'hyperploid' OR cvterm.name = 'hypoploid' OR cvterm.name = 'aneuploid';

--- ************************************************
--- *** relation: hyperploid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of chromosome variation where the ***
--- ***  chromosome complement is not an exact m ***
--- *** ultiple of the haploid number as extra c ***
--- *** hromosomes are present.                  ***
--- ************************************************
---

CREATE VIEW hyperploid AS
  SELECT
    feature_id AS hyperploid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'hyperploid';

--- ************************************************
--- *** relation: hypoploid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of chromosome variation where the ***
--- ***  chromosome complement is not an exact m ***
--- *** ultiple of the haploid number as some ch ***
--- *** romosomes are missing.                   ***
--- ************************************************
---

CREATE VIEW hypoploid AS
  SELECT
    feature_id AS hypoploid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'hypoploid';

--- ************************************************
--- *** relation: operator ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A regulatory element of an operon to whi ***
--- *** ch activators or repressors bind thereby ***
--- ***  effecting translation of genes in that  ***
--- *** operon.                                  ***
--- ************************************************
---

CREATE VIEW operator AS
  SELECT
    feature_id AS operator_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'operator';

--- ************************************************
--- *** relation: nuclease_binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, of a nucleotide mol ***
--- *** ecule, that interacts selectively and no ***
--- *** n-covalently with polypeptide residues o ***
--- *** f a nuclease.                            ***
--- ************************************************
---

CREATE VIEW nuclease_binding_site AS
  SELECT
    feature_id AS nuclease_binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'restriction_enzyme_binding_site' OR cvterm.name = 'nuclease_sensitive_site' OR cvterm.name = 'homing_endonuclease_binding_site' OR cvterm.name = 'nuclease_hypersensitive_site' OR cvterm.name = 'group_1_intron_homing_endonuclease_target_region' OR cvterm.name = 'DNAseI_hypersensitive_site' OR cvterm.name = 'nuclease_binding_site';

--- ************************************************
--- *** relation: compound_chromosome_arm ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW compound_chromosome_arm AS
  SELECT
    feature_id AS compound_chromosome_arm_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'compound_chromosome_arm';

--- ************************************************
--- *** relation: restriction_enzyme_binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the nucleotide m ***
--- *** olecule, interacts selectively and non-c ***
--- *** ovalently with polypeptide residues of a ***
--- ***  restriction enzyme.                     ***
--- ************************************************
---

CREATE VIEW restriction_enzyme_binding_site AS
  SELECT
    feature_id AS restriction_enzyme_binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'restriction_enzyme_binding_site';

--- ************************************************
--- *** relation: d_intrachr_transposition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An intrachromosomal transposition whereb ***
--- *** y a translocation in which one of the fo ***
--- *** ur broken ends loses a segment before re ***
--- *** -joining.                                ***
--- ************************************************
---

CREATE VIEW d_intrachr_transposition AS
  SELECT
    feature_id AS d_intrachr_transposition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'deficient_intrachromosomal_transposition';

--- ************************************************
--- *** relation: d_interchr_transposition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An interchromosomal transposition whereb ***
--- *** y a translocation in which one of the fo ***
--- *** ur broken ends loses a segment before re ***
--- *** -joining.                                ***
--- ************************************************
---

CREATE VIEW d_interchr_transposition AS
  SELECT
    feature_id AS d_interchr_transposition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'deficient_interchromosomal_transposition';

--- ************************************************
--- *** relation: free_chromosome_arm ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome structure variation whereby ***
--- ***  an arm exists as an individual chromoso ***
--- *** me element.                              ***
--- ************************************************
---

CREATE VIEW free_chromosome_arm AS
  SELECT
    feature_id AS free_chromosome_arm_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'free_chromosome_arm';

--- ************************************************
--- *** relation: gene_to_gene_feature ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW gene_to_gene_feature AS
  SELECT
    feature_id AS gene_to_gene_feature_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'overlapping' OR cvterm.name = 'inside_intron' OR cvterm.name = 'five_prime_three_prime_overlap' OR cvterm.name = 'five_prime_five_prime_overlap' OR cvterm.name = 'three_prime_three_prime_overlap' OR cvterm.name = 'three_prime_five_prime_overlap' OR cvterm.name = 'antisense' OR cvterm.name = 'inside_intron_antiparallel' OR cvterm.name = 'inside_intron_parallel' OR cvterm.name = 'gene_to_gene_feature';

--- ************************************************
--- *** relation: overlapping ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a gene that has  ***
--- *** a sequence that overlaps the sequence of ***
--- ***  another gene.                           ***
--- ************************************************
---

CREATE VIEW overlapping AS
  SELECT
    feature_id AS overlapping_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inside_intron' OR cvterm.name = 'five_prime_three_prime_overlap' OR cvterm.name = 'five_prime_five_prime_overlap' OR cvterm.name = 'three_prime_three_prime_overlap' OR cvterm.name = 'three_prime_five_prime_overlap' OR cvterm.name = 'antisense' OR cvterm.name = 'inside_intron_antiparallel' OR cvterm.name = 'inside_intron_parallel' OR cvterm.name = 'overlapping';

--- ************************************************
--- *** relation: inside_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a gene when it  ***
--- *** is located within the intron of another  ***
--- *** gene.                                    ***
--- ************************************************
---

CREATE VIEW inside_intron AS
  SELECT
    feature_id AS inside_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inside_intron_antiparallel' OR cvterm.name = 'inside_intron_parallel' OR cvterm.name = 'inside_intron';

--- ************************************************
--- *** relation: inside_intron_antiparallel ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a gene when it  ***
--- *** is located within the intron of another  ***
--- *** gene and on the opposite strand.         ***
--- ************************************************
---

CREATE VIEW inside_intron_antiparallel AS
  SELECT
    feature_id AS inside_intron_antiparallel_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inside_intron_antiparallel';

--- ************************************************
--- *** relation: inside_intron_parallel ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a gene when it  ***
--- *** is located within the intron of another  ***
--- *** gene and on the same strand.             ***
--- ************************************************
---

CREATE VIEW inside_intron_parallel AS
  SELECT
    feature_id AS inside_intron_parallel_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inside_intron_parallel';

--- ************************************************
--- *** relation: five_prime_three_prime_overlap ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a gene when the ***
--- ***  five prime region overlaps with another ***
--- ***  gene's 3' region.                       ***
--- ************************************************
---

CREATE VIEW five_prime_three_prime_overlap AS
  SELECT
    feature_id AS five_prime_three_prime_overlap_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_three_prime_overlap';

--- ************************************************
--- *** relation: five_prime_five_prime_overlap ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a gene when the ***
--- ***  five prime region overlaps with another ***
--- ***  gene's five prime region.               ***
--- ************************************************
---

CREATE VIEW five_prime_five_prime_overlap AS
  SELECT
    feature_id AS five_prime_five_prime_overlap_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_five_prime_overlap';

--- ************************************************
--- *** relation: three_prime_three_prime_overlap ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a gene when the ***
--- ***  3' region overlaps with another gene's  ***
--- *** 3' region.                               ***
--- ************************************************
---

CREATE VIEW three_prime_three_prime_overlap AS
  SELECT
    feature_id AS three_prime_three_prime_overlap_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_three_prime_overlap';

--- ************************************************
--- *** relation: three_prime_five_prime_overlap ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a gene when the ***
--- ***  3' region overlaps with another gene's  ***
--- *** 5' region.                               ***
--- ************************************************
---

CREATE VIEW three_prime_five_prime_overlap AS
  SELECT
    feature_id AS three_prime_five_prime_overlap_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_five_prime_overlap';

--- ************************************************
--- *** relation: antisense ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region sequence that is complementary  ***
--- *** to a sequence of messenger RNA.          ***
--- ************************************************
---

CREATE VIEW antisense AS
  SELECT
    feature_id AS antisense_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'antisense';

--- ************************************************
--- *** relation: polycistronic_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript that is polycistronic.      ***
--- ************************************************
---

CREATE VIEW polycistronic_transcript AS
  SELECT
    feature_id AS polycistronic_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dicistronic_transcript' OR cvterm.name = 'polycistronic_primary_transcript' OR cvterm.name = 'polycistronic_mRNA' OR cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'dicistronic_primary_transcript' OR cvterm.name = 'dicistronic_primary_transcript' OR cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'polycistronic_transcript';

--- ************************************************
--- *** relation: dicistronic_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript that is dicistronic.        ***
--- ************************************************
---

CREATE VIEW dicistronic_transcript AS
  SELECT
    feature_id AS dicistronic_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'dicistronic_primary_transcript' OR cvterm.name = 'dicistronic_transcript';

--- ************************************************
--- *** relation: operon_member ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW operon_member AS
  SELECT
    feature_id AS operon_member_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'operon_member';

--- ************************************************
--- *** relation: gene_array_member ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW gene_array_member AS
  SELECT
    feature_id AS gene_array_member_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'operon_member' OR cvterm.name = 'gene_cassette_member' OR cvterm.name = 'gene_subarray_member' OR cvterm.name = 'member_of_regulon' OR cvterm.name = 'cassette_array_member' OR cvterm.name = 'gene_array_member';

--- ************************************************
--- *** relation: macronuclear_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW macronuclear_sequence AS
  SELECT
    feature_id AS macronuclear_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'macronuclear_sequence';

--- ************************************************
--- *** relation: micronuclear_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW micronuclear_sequence AS
  SELECT
    feature_id AS micronuclear_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'micronuclear_sequence';

--- ************************************************
--- *** relation: nuclear_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene from nuclear sequence.            ***
--- ************************************************
---

CREATE VIEW nuclear_gene AS
  SELECT
    feature_id AS nuclear_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nuclear_gene';

--- ************************************************
--- *** relation: mt_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene located in mitochondrial sequence ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW mt_gene AS
  SELECT
    feature_id AS mt_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'kinetoplast_gene' OR cvterm.name = 'maxicircle_gene' OR cvterm.name = 'minicircle_gene' OR cvterm.name = 'cryptogene' OR cvterm.name = 'mt_gene';

--- ************************************************
--- *** relation: kinetoplast_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene located in kinetoplast sequence.  ***
--- ************************************************
---

CREATE VIEW kinetoplast_gene AS
  SELECT
    feature_id AS kinetoplast_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'maxicircle_gene' OR cvterm.name = 'minicircle_gene' OR cvterm.name = 'cryptogene' OR cvterm.name = 'kinetoplast_gene';

--- ************************************************
--- *** relation: plastid_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene from plastid sequence.            ***
--- ************************************************
---

CREATE VIEW plastid_gene AS
  SELECT
    feature_id AS plastid_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'apicoplast_gene' OR cvterm.name = 'ct_gene' OR cvterm.name = 'chromoplast_gene' OR cvterm.name = 'cyanelle_gene' OR cvterm.name = 'leucoplast_gene' OR cvterm.name = 'proplastid_gene' OR cvterm.name = 'plastid_gene';

--- ************************************************
--- *** relation: apicoplast_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene from apicoplast sequence.         ***
--- ************************************************
---

CREATE VIEW apicoplast_gene AS
  SELECT
    feature_id AS apicoplast_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'apicoplast_gene';

--- ************************************************
--- *** relation: ct_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene from chloroplast sequence.        ***
--- ************************************************
---

CREATE VIEW ct_gene AS
  SELECT
    feature_id AS ct_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ct_gene';

--- ************************************************
--- *** relation: chromoplast_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene from chromoplast_sequence.        ***
--- ************************************************
---

CREATE VIEW chromoplast_gene AS
  SELECT
    feature_id AS chromoplast_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chromoplast_gene';

--- ************************************************

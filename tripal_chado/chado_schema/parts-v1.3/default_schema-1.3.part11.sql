SET search_path=so,chado,pg_catalog;
--- *** relation: chromosomal_transposition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome structure variant whereby a ***
--- ***  region of a chromosome has been transfe ***
--- *** rred to another position. Among interchr ***
--- *** omosomal rearrangements, the term transp ***
--- *** osition is reserved for that class in wh ***
--- *** ich the telomeres of the chromosomes inv ***
--- *** olved are coupled (that is to say, form  ***
--- *** the two ends of a single DNA molecule) a ***
--- *** s in wild-type.                          ***
--- ************************************************
---

CREATE VIEW chromosomal_transposition AS
  SELECT
    feature_id AS chromosomal_transposition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'intrachromosomal_transposition' OR cvterm.name = 'interchromosomal_transposition' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'uninverted_intrachromosomal_transposition' OR cvterm.name = 'unoriented_intrachromosomal_transposition' OR cvterm.name = 'deficient_interchromosomal_transposition' OR cvterm.name = 'inverted_interchromosomal_transposition' OR cvterm.name = 'uninverted_interchromosomal_transposition' OR cvterm.name = 'unoriented_interchromosomal_transposition' OR cvterm.name = 'chromosomal_transposition';

--- ************************************************
--- *** relation: rasirna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A 17-28-nt, small interfering RNA derive ***
--- *** d from transcripts of repetitive element ***
--- *** s.                                       ***
--- ************************************************
---

CREATE VIEW rasirna AS
  SELECT
    feature_id AS rasirna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rasiRNA';

--- ************************************************
--- *** relation: gene_with_mrna_with_frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that encodes an mRNA with a frame ***
--- *** shift.                                   ***
--- ************************************************
---

CREATE VIEW gene_with_mrna_with_frameshift AS
  SELECT
    feature_id AS gene_with_mrna_with_frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_mRNA_with_frameshift';

--- ************************************************
--- *** relation: recombinationally_rearranged_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is recombinationally rearran ***
--- *** ged.                                     ***
--- ************************************************
---

CREATE VIEW recombinationally_rearranged_gene AS
  SELECT
    feature_id AS recombinationally_rearranged_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'recombinationally_inverted_gene' OR cvterm.name = 'recombinationally_rearranged_vertebrate_immune_system_gene' OR cvterm.name = 'recombinationally_rearranged_gene';

--- ************************************************
--- *** relation: interchromosomal_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome duplication involving an in ***
--- *** sertion from another chromosome.         ***
--- ************************************************
---

CREATE VIEW interchromosomal_duplication AS
  SELECT
    feature_id AS interchromosomal_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'interchromosomal_duplication';

--- ************************************************
--- *** relation: d_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Germline genomic DNA including D-region  ***
--- *** with 5' UTR and 3' UTR, also designated  ***
--- *** as D-segment.                            ***
--- ************************************************
---

CREATE VIEW d_gene AS
  SELECT
    feature_id AS d_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'D_gene';

--- ************************************************
--- *** relation: gene_with_trans_spliced_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene with a transcript that is trans-s ***
--- *** pliced.                                  ***
--- ************************************************
---

CREATE VIEW gene_with_trans_spliced_transcript AS
  SELECT
    feature_id AS gene_with_trans_spliced_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_trans_spliced_transcript';

--- ************************************************
--- *** relation: vertebrate_immunoglobulin_t_cell_receptor_segment ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW vertebrate_immunoglobulin_t_cell_receptor_segment AS
  SELECT
    feature_id AS vertebrate_immunoglobulin_t_cell_receptor_segment_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'D_gene' OR cvterm.name = 'V_gene' OR cvterm.name = 'J_gene' OR cvterm.name = 'C_gene' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_segment';

--- ************************************************
--- *** relation: inversion_derived_bipartite_deficiency ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosomal deletion whereby a chromos ***
--- *** ome generated by recombination between t ***
--- *** wo inversions; has a deficiency at each  ***
--- *** end of the inversion.                    ***
--- ************************************************
---

CREATE VIEW inversion_derived_bipartite_deficiency AS
  SELECT
    feature_id AS inversion_derived_bipartite_deficiency_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inversion_derived_bipartite_deficiency';

--- ************************************************
--- *** relation: pseudogenic_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A non-functional descendent of a functio ***
--- *** nal entity.                              ***
--- ************************************************
---

CREATE VIEW pseudogenic_region AS
  SELECT
    feature_id AS pseudogenic_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'decayed_exon' OR cvterm.name = 'pseudogenic_exon' OR cvterm.name = 'pseudogenic_transcript' OR cvterm.name = 'pseudogenic_rRNA' OR cvterm.name = 'pseudogenic_tRNA' OR cvterm.name = 'pseudogenic_region';

--- ************************************************
--- *** relation: encodes_alternately_spliced_transcripts ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that encodes more than one transc ***
--- *** ript.                                    ***
--- ************************************************
---

CREATE VIEW encodes_alternately_spliced_transcripts AS
  SELECT
    feature_id AS encodes_alternately_spliced_transcripts_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'encodes_1_polypeptide' OR cvterm.name = 'encodes_greater_than_1_polypeptide' OR cvterm.name = 'encodes_disjoint_polypeptides' OR cvterm.name = 'encodes_overlapping_peptides' OR cvterm.name = 'encodes_different_polypeptides_different_stop' OR cvterm.name = 'encodes_overlapping_peptides_different_start' OR cvterm.name = 'encodes_overlapping_polypeptides_different_start_and_stop' OR cvterm.name = 'encodes_alternately_spliced_transcripts';

--- ************************************************
--- *** relation: decayed_exon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A non-functional descendant of an exon.  ***
--- ************************************************
---

CREATE VIEW decayed_exon AS
  SELECT
    feature_id AS decayed_exon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'decayed_exon';

--- ************************************************
--- *** relation: inversion_derived_deficiency_plus_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome deletion whereby a chromoso ***
--- *** me is generated by recombination between ***
--- ***  two inversions; there is a deficiency a ***
--- *** t one end of the inversion and a duplica ***
--- *** tion at the other end of the inversion.  ***
--- ************************************************
---

CREATE VIEW inversion_derived_deficiency_plus_duplication AS
  SELECT
    feature_id AS inversion_derived_deficiency_plus_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inversion_derived_deficiency_plus_duplication';

--- ************************************************
--- *** relation: v_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Germline genomic DNA including L-part1,  ***
--- *** V-intron and V-exon, with the 5' UTR and ***
--- ***  3' UTR.                                 ***
--- ************************************************
---

CREATE VIEW v_gene AS
  SELECT
    feature_id AS v_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'V_gene';

--- ************************************************
--- *** relation: post_translationally_regulated_by_protein_stability ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a gene sequence  ***
--- *** where the resulting protein is regulated ***
--- ***  by the stability of the resulting prote ***
--- *** in.                                      ***
--- ************************************************
---

CREATE VIEW post_translationally_regulated_by_protein_stability AS
  SELECT
    feature_id AS post_translationally_regulated_by_protein_stability_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'post_translationally_regulated_by_protein_stability';

--- ************************************************
--- *** relation: golden_path_fragment ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** One of the pieces of sequence that make  ***
--- *** up a golden path.                        ***
--- ************************************************
---

CREATE VIEW golden_path_fragment AS
  SELECT
    feature_id AS golden_path_fragment_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'golden_path_fragment';

--- ************************************************
--- *** relation: post_translationally_regulated_by_protein_modification ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a gene sequence  ***
--- *** where the resulting protein is modified  ***
--- *** to regulate it.                          ***
--- ************************************************
---

CREATE VIEW post_translationally_regulated_by_protein_modification AS
  SELECT
    feature_id AS post_translationally_regulated_by_protein_modification_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'post_translationally_regulated_by_protein_modification';

--- ************************************************
--- *** relation: j_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Germline genomic DNA of an immunoglobuli ***
--- *** n/T-cell receptor gene including J-regio ***
--- *** n with 5' UTR (SO:0000204) and 3' UTR (S ***
--- *** O:0000205), also designated as J-segment ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW j_gene AS
  SELECT
    feature_id AS j_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'J_gene';

--- ************************************************
--- *** relation: autoregulated ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The gene product is involved in its own  ***
--- *** transcriptional regulation.              ***
--- ************************************************
---

CREATE VIEW autoregulated AS
  SELECT
    feature_id AS autoregulated_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'negatively_autoregulated' OR cvterm.name = 'positively_autoregulated' OR cvterm.name = 'autoregulated';

--- ************************************************
--- *** relation: tiling_path ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A set of regions which overlap with mini ***
--- *** mal polymorphism to form a linear sequen ***
--- *** ce.                                      ***
--- ************************************************
---

CREATE VIEW tiling_path AS
  SELECT
    feature_id AS tiling_path_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tiling_path';

--- ************************************************
--- *** relation: negatively_autoregulated ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The gene product is involved in its own  ***
--- *** transcriptional regulation where it decr ***
--- *** eases transcription.                     ***
--- ************************************************
---

CREATE VIEW negatively_autoregulated AS
  SELECT
    feature_id AS negatively_autoregulated_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'negatively_autoregulated';

--- ************************************************
--- *** relation: tiling_path_fragment ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A piece of sequence that makes up a tili ***
--- *** ng_path (SO:0000472).                    ***
--- ************************************************
---

CREATE VIEW tiling_path_fragment AS
  SELECT
    feature_id AS tiling_path_fragment_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tiling_path_clone' OR cvterm.name = 'tiling_path_fragment';

--- ************************************************
--- *** relation: positively_autoregulated ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The gene product is involved in its own  ***
--- *** transcriptional regulation, where it inc ***
--- *** reases transcription.                    ***
--- ************************************************
---

CREATE VIEW positively_autoregulated AS
  SELECT
    feature_id AS positively_autoregulated_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'positively_autoregulated';

--- ************************************************
--- *** relation: contig_read ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A DNA sequencer read which is part of a  ***
--- *** contig.                                  ***
--- ************************************************
---

CREATE VIEW contig_read AS
  SELECT
    feature_id AS contig_read_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'contig_read';

--- ************************************************
--- *** relation: c_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic DNA of immunoglobulin/T-cell rec ***
--- *** eptor gene including C-region (and intro ***
--- *** ns if present) with 5' UTR (SO:0000204)  ***
--- *** and 3' UTR (SO:0000205).                 ***
--- ************************************************
---

CREATE VIEW c_gene AS
  SELECT
    feature_id AS c_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'C_gene';

--- ************************************************
--- *** relation: trans_spliced_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript that is trans-spliced.      ***
--- ************************************************
---

CREATE VIEW trans_spliced_transcript AS
  SELECT
    feature_id AS trans_spliced_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'trans_spliced_mRNA' OR cvterm.name = 'trans_spliced_transcript';

--- ************************************************
--- *** relation: tiling_path_clone ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A clone which is part of a tiling path.  ***
--- *** A tiling path is a set of sequencing sub ***
--- *** strates, typically clones, which have be ***
--- *** en selected in order to efficiently cove ***
--- *** r a region of the genome in preparation  ***
--- *** for sequencing and assembly.             ***
--- ************************************************
---

CREATE VIEW tiling_path_clone AS
  SELECT
    feature_id AS tiling_path_clone_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tiling_path_clone';

--- ************************************************
--- *** relation: terminal_inverted_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An inverted repeat (SO:0000294) occurrin ***
--- *** g at the termini of a DNA transposon.    ***
--- ************************************************
---

CREATE VIEW terminal_inverted_repeat AS
  SELECT
    feature_id AS terminal_inverted_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_terminal_inverted_repeat' OR cvterm.name = 'three_prime_terminal_inverted_repeat' OR cvterm.name = 'terminal_inverted_repeat';

--- ************************************************
--- *** relation: vertebrate_immunoglobulin_t_cell_receptor_gene_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW vertebrate_immunoglobulin_t_cell_receptor_gene_cluster AS
  SELECT
    feature_id AS vertebrate_immunoglobulin_t_cell_receptor_gene_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'D_J_C_cluster' OR cvterm.name = 'J_C_cluster' OR cvterm.name = 'J_cluster' OR cvterm.name = 'V_cluster' OR cvterm.name = 'V_J_cluster' OR cvterm.name = 'V_J_C_cluster' OR cvterm.name = 'C_cluster' OR cvterm.name = 'D_cluster' OR cvterm.name = 'D_J_cluster' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_gene_cluster';

--- ************************************************
--- *** relation: nc_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript that is never trans ***
--- *** lated into a protein.                    ***
--- ************************************************
---

CREATE VIEW nc_primary_transcript AS
  SELECT
    feature_id AS nc_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'scRNA_primary_transcript' OR cvterm.name = 'rRNA_primary_transcript' OR cvterm.name = 'tRNA_primary_transcript' OR cvterm.name = 'snRNA_primary_transcript' OR cvterm.name = 'snoRNA_primary_transcript' OR cvterm.name = 'tmRNA_primary_transcript' OR cvterm.name = 'SRP_RNA_primary_transcript' OR cvterm.name = 'miRNA_primary_transcript' OR cvterm.name = 'tasiRNA_primary_transcript' OR cvterm.name = 'rRNA_small_subunit_primary_transcript' OR cvterm.name = 'rRNA_large_subunit_primary_transcript' OR cvterm.name = 'alanine_tRNA_primary_transcript' OR cvterm.name = 'arginine_tRNA_primary_transcript' OR cvterm.name = 'asparagine_tRNA_primary_transcript' OR cvterm.name = 'aspartic_acid_tRNA_primary_transcript' OR cvterm.name = 'cysteine_tRNA_primary_transcript' OR cvterm.name = 'glutamic_acid_tRNA_primary_transcript' OR cvterm.name = 'glutamine_tRNA_primary_transcript' OR cvterm.name = 'glycine_tRNA_primary_transcript' OR cvterm.name = 'histidine_tRNA_primary_transcript' OR cvterm.name = 'isoleucine_tRNA_primary_transcript' OR cvterm.name = 'leucine_tRNA_primary_transcript' OR cvterm.name = 'lysine_tRNA_primary_transcript' OR cvterm.name = 'methionine_tRNA_primary_transcript' OR cvterm.name = 'phenylalanine_tRNA_primary_transcript' OR cvterm.name = 'proline_tRNA_primary_transcript' OR cvterm.name = 'serine_tRNA_primary_transcript' OR cvterm.name = 'threonine_tRNA_primary_transcript' OR cvterm.name = 'tryptophan_tRNA_primary_transcript' OR cvterm.name = 'tyrosine_tRNA_primary_transcript' OR cvterm.name = 'valine_tRNA_primary_transcript' OR cvterm.name = 'pyrrolysine_tRNA_primary_transcript' OR cvterm.name = 'selenocysteine_tRNA_primary_transcript' OR cvterm.name = 'methylation_guide_snoRNA_primary_transcript' OR cvterm.name = 'rRNA_cleavage_snoRNA_primary_transcript' OR cvterm.name = 'C_D_box_snoRNA_primary_transcript' OR cvterm.name = 'H_ACA_box_snoRNA_primary_transcript' OR cvterm.name = 'U14_snoRNA_primary_transcript' OR cvterm.name = 'stRNA_primary_transcript' OR cvterm.name = 'nc_primary_transcript';

--- ************************************************
--- *** relation: three_prime_coding_exon_noncoding_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The sequence of the 3' exon that is not  ***
--- *** coding.                                  ***
--- ************************************************
---

CREATE VIEW three_prime_coding_exon_noncoding_region AS
  SELECT
    feature_id AS three_prime_coding_exon_noncoding_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_coding_exon_noncoding_region';

--- ************************************************
--- *** relation: dj_j_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic DNA of immunoglobulin/T-cell rec ***
--- *** eptor gene in rearranged configuration i ***
--- *** ncluding at least one DJ-gene, and one J ***
--- *** -gene.                                   ***
--- ************************************************
---

CREATE VIEW dj_j_cluster AS
  SELECT
    feature_id AS dj_j_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DJ_J_cluster';

--- ************************************************
--- *** relation: five_prime_coding_exon_noncoding_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The sequence of the 5' exon preceding th ***
--- *** e start codon.                           ***
--- ************************************************
---

CREATE VIEW five_prime_coding_exon_noncoding_region AS
  SELECT
    feature_id AS five_prime_coding_exon_noncoding_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_coding_exon_noncoding_region';

--- ************************************************
--- *** relation: vdj_j_c_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic DNA of immunoglobulin/T-cell rec ***
--- *** eptor gene in rearranged configuration i ***
--- *** ncluding at least one VDJ-gene, one J-ge ***
--- *** ne and one C-gene.                       ***
--- ************************************************
---

CREATE VIEW vdj_j_c_cluster AS
  SELECT
    feature_id AS vdj_j_c_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'VDJ_J_C_cluster';

--- ************************************************
--- *** relation: vdj_j_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic DNA of immunoglobulin/T-cell rec ***
--- *** eptor gene in rearranged configuration i ***
--- *** ncluding at least one VDJ-gene and one J ***
--- *** -gene.                                   ***
--- ************************************************
---

CREATE VIEW vdj_j_cluster AS
  SELECT
    feature_id AS vdj_j_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'VDJ_J_cluster';

--- ************************************************
--- *** relation: vj_c_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic DNA of immunoglobulin/T-cell rec ***
--- *** eptor gene in rearranged configuration i ***
--- *** ncluding at least one VJ-gene and one C- ***
--- *** gene.                                    ***
--- ************************************************
---

CREATE VIEW vj_c_cluster AS
  SELECT
    feature_id AS vj_c_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'VJ_C_cluster';

--- ************************************************
--- *** relation: vj_j_c_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic DNA of immunoglobulin/T-cell rec ***
--- *** eptor gene in rearranged configuration i ***
--- *** ncluding at least one VJ-gene, one J-gen ***
--- *** e and one C-gene.                        ***
--- ************************************************
---

CREATE VIEW vj_j_c_cluster AS
  SELECT
    feature_id AS vj_j_c_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'VJ_J_C_cluster';

--- ************************************************
--- *** relation: vj_j_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic DNA of immunoglobulin/T-cell rec ***
--- *** eptor gene in rearranged configuration i ***
--- *** ncluding at least one VJ-gene and one J- ***
--- *** gene.                                    ***
--- ************************************************
---

CREATE VIEW vj_j_cluster AS
  SELECT
    feature_id AS vj_j_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'VJ_J_cluster';

--- ************************************************
--- *** relation: d_gene_recombination_feature ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW d_gene_recombination_feature AS
  SELECT
    feature_id AS d_gene_recombination_feature_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_recombination_signal_sequence' OR cvterm.name = 'D_gene_recombination_feature';

--- ************************************************
--- *** relation: three_prime_d_heptamer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 7 nucleotide recombination site like CAC ***
--- *** AGTG, part of a 3' D-recombination signa ***
--- *** l sequence of an immunoglobulin/T-cell r ***
--- *** eceptor gene.                            ***
--- ************************************************
---

CREATE VIEW three_prime_d_heptamer AS
  SELECT
    feature_id AS three_prime_d_heptamer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_D_heptamer';

--- ************************************************
--- *** relation: three_prime_d_nonamer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A 9 nucleotide recombination site (e.g.  ***
--- *** ACAAAAACC), part of a 3' D-recombination ***
--- ***  signal sequence of an immunoglobulin/T- ***
--- *** cell receptor gene.                      ***
--- ************************************************
---

CREATE VIEW three_prime_d_nonamer AS
  SELECT
    feature_id AS three_prime_d_nonamer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_D_nonamer';

--- ************************************************
--- *** relation: three_prime_d_spacer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A 12 or 23 nucleotide spacer between the ***
--- ***  3'D-HEPTAMER and 3'D-NONAMER of a 3'D-R ***
--- *** S.                                       ***
--- ************************************************
---

CREATE VIEW three_prime_d_spacer AS
  SELECT
    feature_id AS three_prime_d_spacer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_D_spacer';

--- ************************************************
--- *** relation: five_prime_d_heptamer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 7 nucleotide recombination site (e.g. CA ***
--- *** CTGTG), part of a 5' D-recombination sig ***
--- *** nal sequence (SO:0000556) of an immunogl ***
--- *** obulin/T-cell receptor gene.             ***
--- ************************************************
---

CREATE VIEW five_prime_d_heptamer AS
  SELECT
    feature_id AS five_prime_d_heptamer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_D_heptamer';

--- ************************************************
--- *** relation: five_prime_d_nonamer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 9 nucleotide recombination site (e.g. GG ***
--- *** TTTTTGT), part of a five_prime_D-recombi ***
--- *** nation signal sequence (SO:0000556) of a ***
--- *** n immunoglobulin/T-cell receptor gene.   ***
--- ************************************************
---

CREATE VIEW five_prime_d_nonamer AS
  SELECT
    feature_id AS five_prime_d_nonamer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_D_nonamer';

--- ************************************************
--- *** relation: five_prime_d_spacer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 12 or 23 nucleotide spacer between the 5 ***
--- *** ' D-heptamer (SO:0000496) and 5' D-nonam ***
--- *** er (SO:0000497) of a 5' D-recombination  ***
--- *** signal sequence (SO:0000556) of an immun ***
--- *** oglobulin/T-cell receptor gene.          ***
--- ************************************************
---

CREATE VIEW five_prime_d_spacer AS
  SELECT
    feature_id AS five_prime_d_spacer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_D_spacer';

--- ************************************************
--- *** relation: virtual_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A continuous piece of sequence similar t ***
--- *** o the 'virtual contig' concept of the En ***
--- *** sembl database.                          ***
--- ************************************************
---

CREATE VIEW virtual_sequence AS
  SELECT
    feature_id AS virtual_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'virtual_sequence';

--- ************************************************
--- *** relation: hoogsteen_base_pair ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A type of non-canonical base-pairing. Th ***
--- *** is is less energetically favourable than ***
--- ***  watson crick base pairing. Hoogsteen GC ***
--- ***  base pairs only have two hydrogen bonds ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW hoogsteen_base_pair AS
  SELECT
    feature_id AS hoogsteen_base_pair_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'Hoogsteen_base_pair';

--- ************************************************
--- *** relation: reverse_hoogsteen_base_pair ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A type of non-canonical base-pairing.    ***
--- ************************************************
---

CREATE VIEW reverse_hoogsteen_base_pair AS
  SELECT
    feature_id AS reverse_hoogsteen_base_pair_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'reverse_Hoogsteen_base_pair';

--- ************************************************
--- *** relation: d_dj_c_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic DNA of immunoglobulin/T-cell rec ***
--- *** eptor gene in rearranged configuration i ***
--- *** ncluding at least one D-gene, one DJ-gen ***
--- *** e and one C-gene.                        ***
--- ************************************************
---

CREATE VIEW d_dj_c_cluster AS
  SELECT
    feature_id AS d_dj_c_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'D_DJ_C_cluster';

--- ************************************************
--- *** relation: d_dj_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic DNA of immunoglobulin/T-cell rec ***
--- *** eptor gene in rearranged configuration i ***
--- *** ncluding at least one D-gene and one DJ- ***
--- *** gene.                                    ***
--- ************************************************
---

CREATE VIEW d_dj_cluster AS
  SELECT
    feature_id AS d_dj_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'D_DJ_cluster';

--- ************************************************
--- *** relation: d_dj_j_c_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic DNA of immunoglobulin/T-cell rec ***
--- *** eptor gene in rearranged configuration i ***
--- *** ncluding at least one D-gene, one DJ-gen ***
--- *** e, one J-gene and one C-gene.            ***
--- ************************************************
---

CREATE VIEW d_dj_j_c_cluster AS
  SELECT
    feature_id AS d_dj_j_c_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'D_DJ_J_C_cluster';

--- ************************************************
--- *** relation: pseudogenic_exon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A non functional descendant of an exon,  ***
--- *** part of a pseudogene.                    ***
--- ************************************************
---

CREATE VIEW pseudogenic_exon AS
  SELECT
    feature_id AS pseudogenic_exon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pseudogenic_exon';

--- ************************************************
--- *** relation: d_dj_j_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic DNA of immunoglobulin/T-cell rec ***
--- *** eptor gene in rearranged configuration i ***
--- *** ncluding at least one D-gene, one DJ-gen ***
--- *** e, and one J-gene.                       ***
--- ************************************************
---

CREATE VIEW d_dj_j_cluster AS
  SELECT
    feature_id AS d_dj_j_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'D_DJ_J_cluster';

--- ************************************************
--- *** relation: d_j_c_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic DNA of immunoglobulin/T-cell rec ***
--- *** eptor gene in germline configuration inc ***
--- *** luding at least one D-gene, one J-gene a ***
--- *** nd one C-gene.                           ***
--- ************************************************
---

CREATE VIEW d_j_c_cluster AS
  SELECT
    feature_id AS d_j_c_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'D_J_C_cluster';

--- ************************************************
--- *** relation: vd_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic DNA of immunoglobulin/T-cell rec ***
--- *** eptor gene in partially rearranged genom ***
--- *** ic DNA including L-part1, V-intron and V ***
--- *** -D-exon, with the 5' UTR (SO:0000204) an ***
--- *** d 3' UTR (SO:0000205).                   ***
--- ************************************************
---

CREATE VIEW vd_gene AS
  SELECT
    feature_id AS vd_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'VD_gene';

--- ************************************************
--- *** relation: j_c_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic DNA of immunoglobulin/T-cell rec ***

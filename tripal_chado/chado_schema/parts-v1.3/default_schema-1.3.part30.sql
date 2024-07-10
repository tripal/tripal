SET search_path=so,chado,pg_catalog;
--- ***                                          ***
--- ************************************************
---

CREATE VIEW complex_change_of_translational_product_variant AS
  SELECT
    feature_id AS complex_change_of_translational_product_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'complex_change_of_translational_product_variant';

--- ************************************************
--- *** relation: polypeptide_sequence_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant with in the CDS that  ***
--- *** causes a change in the resulting polypep ***
--- *** tide sequence.                           ***
--- ************************************************
---

CREATE VIEW polypeptide_sequence_variant AS
  SELECT
    feature_id AS polypeptide_sequence_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'amino_acid_deletion' OR cvterm.name = 'amino_acid_insertion' OR cvterm.name = 'amino_acid_substitution' OR cvterm.name = 'elongated_polypeptide' OR cvterm.name = 'polypeptide_fusion' OR cvterm.name = 'polypeptide_truncation' OR cvterm.name = 'conservative_amino_acid_substitution' OR cvterm.name = 'non_conservative_amino_acid_substitution' OR cvterm.name = 'elongated_polypeptide_C_terminal' OR cvterm.name = 'elongated_polypeptide_N_terminal' OR cvterm.name = 'elongated_in_frame_polypeptide_C_terminal' OR cvterm.name = 'elongated_out_of_frame_polypeptide_C_terminal' OR cvterm.name = 'elongated_in_frame_polypeptide_N_terminal_elongation' OR cvterm.name = 'elongated_out_of_frame_polypeptide_N_terminal' OR cvterm.name = 'polypeptide_sequence_variant';

--- ************************************************
--- *** relation: amino_acid_deletion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant within a CDS resultin ***
--- *** g in the loss of an amino acid from the  ***
--- *** resulting polypeptide.                   ***
--- ************************************************
---

CREATE VIEW amino_acid_deletion AS
  SELECT
    feature_id AS amino_acid_deletion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'amino_acid_deletion';

--- ************************************************
--- *** relation: amino_acid_insertion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant within a CDS resultin ***
--- *** g in the gain of an amino acid to the re ***
--- *** sulting polypeptide.                     ***
--- ************************************************
---

CREATE VIEW amino_acid_insertion AS
  SELECT
    feature_id AS amino_acid_insertion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'amino_acid_insertion';

--- ************************************************
--- *** relation: amino_acid_substitution ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant of a codon resulting  ***
--- *** in the substitution of one amino acid fo ***
--- *** r another in the resulting polypeptide.  ***
--- ************************************************
---

CREATE VIEW amino_acid_substitution AS
  SELECT
    feature_id AS amino_acid_substitution_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'conservative_amino_acid_substitution' OR cvterm.name = 'non_conservative_amino_acid_substitution' OR cvterm.name = 'amino_acid_substitution';

--- ************************************************
--- *** relation: conservative_amino_acid_substitution ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant of a codon causing th ***
--- *** e substitution of a similar amino acid f ***
--- *** or another in the resulting polypeptide. ***
--- ************************************************
---

CREATE VIEW conservative_amino_acid_substitution AS
  SELECT
    feature_id AS conservative_amino_acid_substitution_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'conservative_amino_acid_substitution';

--- ************************************************
--- *** relation: non_conservative_amino_acid_substitution ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant of a codon causing th ***
--- *** e substitution of a non conservative ami ***
--- *** no acid for another in the resulting pol ***
--- *** ypeptide.                                ***
--- ************************************************
---

CREATE VIEW non_conservative_amino_acid_substitution AS
  SELECT
    feature_id AS non_conservative_amino_acid_substitution_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'non_conservative_amino_acid_substitution';

--- ************************************************
--- *** relation: elongated_polypeptide ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant with in the CDS that  ***
--- *** causes elongation of the resulting polyp ***
--- *** eptide sequence.                         ***
--- ************************************************
---

CREATE VIEW elongated_polypeptide AS
  SELECT
    feature_id AS elongated_polypeptide_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'elongated_polypeptide_C_terminal' OR cvterm.name = 'elongated_polypeptide_N_terminal' OR cvterm.name = 'elongated_in_frame_polypeptide_C_terminal' OR cvterm.name = 'elongated_out_of_frame_polypeptide_C_terminal' OR cvterm.name = 'elongated_in_frame_polypeptide_N_terminal_elongation' OR cvterm.name = 'elongated_out_of_frame_polypeptide_N_terminal' OR cvterm.name = 'elongated_polypeptide';

--- ************************************************
--- *** relation: elongated_polypeptide_c_terminal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant with in the CDS that  ***
--- *** causes elongation of the resulting polyp ***
--- *** eptide sequence at the C terminus.       ***
--- ************************************************
---

CREATE VIEW elongated_polypeptide_c_terminal AS
  SELECT
    feature_id AS elongated_polypeptide_c_terminal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'elongated_in_frame_polypeptide_C_terminal' OR cvterm.name = 'elongated_out_of_frame_polypeptide_C_terminal' OR cvterm.name = 'elongated_polypeptide_C_terminal';

--- ************************************************
--- *** relation: elongated_polypeptide_n_terminal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant with in the CDS that  ***
--- *** causes elongation of the resulting polyp ***
--- *** eptide sequence at the N terminus.       ***
--- ************************************************
---

CREATE VIEW elongated_polypeptide_n_terminal AS
  SELECT
    feature_id AS elongated_polypeptide_n_terminal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'elongated_in_frame_polypeptide_N_terminal_elongation' OR cvterm.name = 'elongated_out_of_frame_polypeptide_N_terminal' OR cvterm.name = 'elongated_polypeptide_N_terminal';

--- ************************************************
--- *** relation: elongated_in_frame_polypeptide_c_terminal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant with in the CDS that  ***
--- *** causes in frame elongation of the result ***
--- *** ing polypeptide sequence at the C termin ***
--- *** us.                                      ***
--- ************************************************
---

CREATE VIEW elongated_in_frame_polypeptide_c_terminal AS
  SELECT
    feature_id AS elongated_in_frame_polypeptide_c_terminal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'elongated_in_frame_polypeptide_C_terminal';

--- ************************************************
--- *** relation: elongated_out_of_frame_polypeptide_c_terminal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant with in the CDS that  ***
--- *** causes out of frame elongation of the re ***
--- *** sulting polypeptide sequence at the C te ***
--- *** rminus.                                  ***
--- ************************************************
---

CREATE VIEW elongated_out_of_frame_polypeptide_c_terminal AS
  SELECT
    feature_id AS elongated_out_of_frame_polypeptide_c_terminal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'elongated_out_of_frame_polypeptide_C_terminal';

--- ************************************************
--- *** relation: elongated_in_frame_polypeptide_n_terminal_elongation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant with in the CDS that  ***
--- *** causes in frame elongation of the result ***
--- *** ing polypeptide sequence at the N termin ***
--- *** us.                                      ***
--- ************************************************
---

CREATE VIEW elongated_in_frame_polypeptide_n_terminal_elongation AS
  SELECT
    feature_id AS elongated_in_frame_polypeptide_n_terminal_elongation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'elongated_in_frame_polypeptide_N_terminal_elongation';

--- ************************************************
--- *** relation: elongated_out_of_frame_polypeptide_n_terminal ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant with in the CDS that  ***
--- *** causes out of frame elongation of the re ***
--- *** sulting polypeptide sequence at the N te ***
--- *** rminus.                                  ***
--- ************************************************
---

CREATE VIEW elongated_out_of_frame_polypeptide_n_terminal AS
  SELECT
    feature_id AS elongated_out_of_frame_polypeptide_n_terminal_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'elongated_out_of_frame_polypeptide_N_terminal';

--- ************************************************
--- *** relation: polypeptide_fusion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that causes a fusion  ***
--- *** of two polypeptide sequences.            ***
--- ************************************************
---

CREATE VIEW polypeptide_fusion AS
  SELECT
    feature_id AS polypeptide_fusion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_fusion';

--- ************************************************
--- *** relation: polypeptide_truncation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant of the CD that causes ***
--- ***  a truncation of the resulting polypepti ***
--- *** de.                                      ***
--- ************************************************
---

CREATE VIEW polypeptide_truncation AS
  SELECT
    feature_id AS polypeptide_truncation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_truncation';

--- ************************************************
--- *** relation: inactive_catalytic_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that causes the inact ***
--- *** ivation of a catalytic site with respect ***
--- ***  to a reference sequence.                ***
--- ************************************************
---

CREATE VIEW inactive_catalytic_site AS
  SELECT
    feature_id AS inactive_catalytic_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inactive_catalytic_site';

--- ************************************************
--- *** relation: nc_transcript_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript variant of a non coding RNA ***
--- ***  gene.                                   ***
--- ************************************************
---

CREATE VIEW nc_transcript_variant AS
  SELECT
    feature_id AS nc_transcript_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mature_miRNA_variant' OR cvterm.name = 'nc_transcript_variant';

--- ************************************************
--- *** relation: mature_mirna_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript variant located with the se ***
--- *** quence of the mature miRNA.              ***
--- ************************************************
---

CREATE VIEW mature_mirna_variant AS
  SELECT
    feature_id AS mature_mirna_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mature_miRNA_variant';

--- ************************************************
--- *** relation: nmd_transcript_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A variant in a transcript that is the ta ***
--- *** rget of NMD.                             ***
--- ************************************************
---

CREATE VIEW nmd_transcript_variant AS
  SELECT
    feature_id AS nmd_transcript_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'NMD_transcript_variant';

--- ************************************************
--- *** relation: utr_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript variant that is located wit ***
--- *** hin the UTR.                             ***
--- ************************************************
---

CREATE VIEW utr_variant AS
  SELECT
    feature_id AS utr_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = '5_prime_UTR_variant' OR cvterm.name = '3_prime_UTR_variant' OR cvterm.name = 'UTR_variant';

--- ************************************************
--- *** relation: five_prime_utr_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A UTR variant of the 5' UTR.             ***
--- ************************************************
---

CREATE VIEW five_prime_utr_variant AS
  SELECT
    feature_id AS five_prime_utr_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = '5_prime_UTR_variant';

--- ************************************************
--- *** relation: three_prime_utr_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A UTR variant of the 3' UTR.             ***
--- ************************************************
---

CREATE VIEW three_prime_utr_variant AS
  SELECT
    feature_id AS three_prime_utr_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = '3_prime_UTR_variant';

--- ************************************************
--- *** relation: terminal_codon_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A codon variant that changes at least on ***
--- *** e base of the last codon of the transcri ***
--- *** pt.                                      ***
--- ************************************************
---

CREATE VIEW terminal_codon_variant AS
  SELECT
    feature_id AS terminal_codon_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'terminator_codon_variant' OR cvterm.name = 'incomplete_terminal_codon_variant' OR cvterm.name = 'stop_retained_variant' OR cvterm.name = 'stop_lost' OR cvterm.name = 'terminal_codon_variant';

--- ************************************************
--- *** relation: incomplete_terminal_codon_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant where at least one ba ***
--- *** se of the final codon of an incompletely ***
--- ***  annotated transcript is changed.        ***
--- ************************************************
---

CREATE VIEW incomplete_terminal_codon_variant AS
  SELECT
    feature_id AS incomplete_terminal_codon_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'incomplete_terminal_codon_variant';

--- ************************************************
--- *** relation: intron_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript variant occurring within an ***
--- ***  intron.                                 ***
--- ************************************************
---

CREATE VIEW intron_variant AS
  SELECT
    feature_id AS intron_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'splice_site_variant' OR cvterm.name = 'splice_acceptor_variant' OR cvterm.name = 'splice_donor_variant' OR cvterm.name = 'splice_donor_5th_base_variant' OR cvterm.name = 'intron_variant';

--- ************************************************
--- *** relation: intergenic_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant located in the interg ***
--- *** enic region, between genes.              ***
--- ************************************************
---

CREATE VIEW intergenic_variant AS
  SELECT
    feature_id AS intergenic_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'intergenic_variant';

--- ************************************************
--- *** relation: splice_site_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that changes the firs ***
--- *** t two or last two bases of an intron, or ***
--- ***  the 5th base from the start of the intr ***
--- *** on in the orientation of the transcript. ***
--- ************************************************
---

CREATE VIEW splice_site_variant AS
  SELECT
    feature_id AS splice_site_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'splice_acceptor_variant' OR cvterm.name = 'splice_donor_variant' OR cvterm.name = 'splice_donor_5th_base_variant' OR cvterm.name = 'splice_site_variant';

--- ************************************************
--- *** relation: splice_region_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant in which a change has ***
--- ***  occurred within the region of the splic ***
--- *** e site, either within 1-3 bases of the e ***
--- *** xon or 3-8 bases of the intron.          ***
--- ************************************************
---

CREATE VIEW splice_region_variant AS
  SELECT
    feature_id AS splice_region_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'splice_region_variant';

--- ************************************************
--- *** relation: upstream_gene_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant located 5' of a gene. ***
--- ************************************************
---

CREATE VIEW upstream_gene_variant AS
  SELECT
    feature_id AS upstream_gene_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = '5KB_upstream_variant' OR cvterm.name = '2KB_upstream_variant' OR cvterm.name = 'upstream_gene_variant';

--- ************************************************
--- *** relation: downstream_gene_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant located 3' of a gene. ***
--- ************************************************
---

CREATE VIEW downstream_gene_variant AS
  SELECT
    feature_id AS downstream_gene_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = '5KB_downstream_variant' OR cvterm.name = '500B_downstream_variant' OR cvterm.name = 'downstream_gene_variant';

--- ************************************************
--- *** relation: fivekb_downstream_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant located within 5 KB o ***
--- *** f the end of a gene.                     ***
--- ************************************************
---

CREATE VIEW fivekb_downstream_variant AS
  SELECT
    feature_id AS fivekb_downstream_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = '500B_downstream_variant' OR cvterm.name = '5KB_downstream_variant';

--- ************************************************
--- *** relation: fivehundred_b_downstream_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant located within a half ***
--- ***  KB of the end of a gene.                ***
--- ************************************************
---

CREATE VIEW fivehundred_b_downstream_variant AS
  SELECT
    feature_id AS fivehundred_b_downstream_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = '500B_downstream_variant';

--- ************************************************
--- *** relation: fivekb_upstream_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant located within 5KB 5' ***
--- ***  of a gene.                              ***
--- ************************************************
---

CREATE VIEW fivekb_upstream_variant AS
  SELECT
    feature_id AS fivekb_upstream_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = '2KB_upstream_variant' OR cvterm.name = '5KB_upstream_variant';

--- ************************************************
--- *** relation: twokb_upstream_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant located within 2KB 5' ***
--- ***  of a gene.                              ***
--- ************************************************
---

CREATE VIEW twokb_upstream_variant AS
  SELECT
    feature_id AS twokb_upstream_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = '2KB_upstream_variant';

--- ************************************************
--- *** relation: rrna_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that encodes for ribosomal RNA.   ***
--- ************************************************
---

CREATE VIEW rrna_gene AS
  SELECT
    feature_id AS rrna_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_gene';

--- ************************************************
--- *** relation: pirna_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that encodes for an piwi associat ***
--- *** ed RNA.                                  ***
--- ************************************************
---

CREATE VIEW pirna_gene AS
  SELECT
    feature_id AS pirna_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'piRNA_gene';

--- ************************************************
--- *** relation: rnase_p_rna_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that encodes an RNase P RNA.      ***
--- ************************************************
---

CREATE VIEW rnase_p_rna_gene AS
  SELECT
    feature_id AS rnase_p_rna_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNase_P_RNA_gene';

--- ************************************************
--- *** relation: rnase_mrp_rna_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that encodes a RNase_MRP_RNA.     ***
--- ************************************************
---

CREATE VIEW rnase_mrp_rna_gene AS
  SELECT
    feature_id AS rnase_mrp_rna_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNase_MRP_RNA_gene';

--- ************************************************
--- *** relation: lincrna_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that encodes large intervening no ***
--- *** n-coding RNA.                            ***
--- ************************************************
---

CREATE VIEW lincrna_gene AS
  SELECT
    feature_id AS lincrna_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'lincRNA_gene';

--- ************************************************
--- *** relation: mathematically_defined_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A mathematically defined repeat (MDR) is ***
--- ***  a experimental feature that is determin ***
--- *** ed by querying overlapping oligomers of  ***
--- *** length k against a database of shotgun s ***
--- *** equence data and identifying regions in  ***
--- *** the query sequence that exceed a statist ***
--- *** ically determined threshold of repetitiv ***
--- *** eness.                                   ***
--- ************************************************
---

CREATE VIEW mathematically_defined_repeat AS
  SELECT
    feature_id AS mathematically_defined_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mathematically_defined_repeat';

--- ************************************************
--- *** relation: telomerase_rna_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A telomerase RNA gene is a non coding RN ***
--- *** A gene the RNA product of which is a com ***
--- *** ponent of telomerase.                    ***
--- ************************************************
---

CREATE VIEW telomerase_rna_gene AS
  SELECT
    feature_id AS telomerase_rna_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'telomerase_RNA_gene';

--- ************************************************
--- *** relation: targeting_vector ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An engineered vector that is able to tak ***
--- *** e part in homologous recombination in a  ***
--- *** host with the intent of introducing site ***
--- ***  specific genomic modifications.         ***
--- ************************************************
---

CREATE VIEW targeting_vector AS
  SELECT
    feature_id AS targeting_vector_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'targeting_vector';

--- ************************************************
--- *** relation: genetic_marker ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A measurable sequence feature that varie ***
--- *** s within a population.                   ***
--- ************************************************
---

CREATE VIEW genetic_marker AS
  SELECT
    feature_id AS genetic_marker_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'heritable_phenotypic_marker' OR cvterm.name = 'DArT_marker' OR cvterm.name = 'genetic_marker';

--- ************************************************
--- *** relation: dart_marker ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A genetic marker, discovered using Diver ***
--- *** sity Arrays Technology (DArT) technology ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW dart_marker AS
  SELECT
    feature_id AS dart_marker_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DArT_marker';

--- ************************************************
--- *** relation: kozak_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of ribosome entry site, specific  ***
--- *** to Eukaryotic organisms that overlaps pa ***
--- *** rt of both 5' UTR and CDS sequence.      ***
--- ************************************************
---

CREATE VIEW kozak_sequence AS
  SELECT
    feature_id AS kozak_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'kozak_sequence';

--- ************************************************
--- *** relation: nested_transposon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transposon that is disrupted by the in ***
--- *** sertion of another element.              ***
--- ************************************************
---

CREATE VIEW nested_transposon AS
  SELECT
    feature_id AS nested_transposon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nested_transposon';

--- ************************************************
--- *** relation: nested_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A repeat that is disrupted by the insert ***
--- *** ion of another element.                  ***
--- ************************************************
---

CREATE VIEW nested_repeat AS
  SELECT
    feature_id AS nested_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nested_tandem_repeat' OR cvterm.name = 'nested_repeat';

--- ************************************************
--- *** relation: inframe_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant which does not cause  ***
--- *** a disruption of the translational readin ***
--- *** g frame.                                 ***
--- ************************************************
---

CREATE VIEW inframe_variant AS
  SELECT
    feature_id AS inframe_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inframe_codon_gain' OR cvterm.name = 'inframe_codon_loss' OR cvterm.name = 'inframe_variant';

--- ************************************************
--- *** relation: inframe_codon_gain ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant which gains a codon,  ***
--- *** and does not cause a disruption of the t ***
--- *** ranslational reading frame.              ***
--- ************************************************
---

CREATE VIEW inframe_codon_gain AS
  SELECT
    feature_id AS inframe_codon_gain_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inframe_codon_gain';

--- ************************************************
--- *** relation: inframe_codon_loss ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant which loses a codon,  ***
--- *** and does not cause a disruption of the t ***
--- *** ranslational reading frame.              ***
--- ************************************************
---

CREATE VIEW inframe_codon_loss AS
  SELECT
    feature_id AS inframe_codon_loss_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inframe_codon_loss';

--- ************************************************
--- *** relation: retinoic_acid_responsive_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcription factor binding site of v ***
--- *** ariable direct repeats of the sequence P ***
--- *** uGGTCA spaced by five nucleotides (DR5)  ***
--- *** found in the promoters of retinoic acid- ***
--- *** responsive genes, to which retinoic acid ***
--- ***  receptors bind.                         ***
--- ************************************************
---

CREATE VIEW retinoic_acid_responsive_element AS
  SELECT
    feature_id AS retinoic_acid_responsive_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'retinoic_acid_responsive_element';

--- ************************************************
--- *** relation: nucleotide_to_protein_binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the nucleotide m ***
--- *** olecule, interacts selectively and non-c ***
--- *** ovalently with polypeptide residues.     ***
--- ************************************************
---

CREATE VIEW nucleotide_to_protein_binding_site AS
  SELECT
    feature_id AS nucleotide_to_protein_binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nuclease_binding_site' OR cvterm.name = 'TF_binding_site' OR cvterm.name = 'histone_binding_site' OR cvterm.name = 'insulator_binding_site' OR cvterm.name = 'enhancer_binding_site' OR cvterm.name = 'restriction_enzyme_binding_site' OR cvterm.name = 'nuclease_sensitive_site' OR cvterm.name = 'homing_endonuclease_binding_site' OR cvterm.name = 'nuclease_hypersensitive_site' OR cvterm.name = 'group_1_intron_homing_endonuclease_target_region' OR cvterm.name = 'DNAseI_hypersensitive_site' OR cvterm.name = 'nucleotide_to_protein_binding_site';

--- ************************************************
--- *** relation: nucleotide_binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the molecule, in ***
--- *** teracts selectively and non-covalently w ***
--- *** ith nucleotide residues.                 ***
--- ************************************************
---

CREATE VIEW nucleotide_binding_site AS
  SELECT
    feature_id AS nucleotide_binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'miRNA_target_site' OR cvterm.name = 'DNA_binding_site' OR cvterm.name = 'primer_binding_site' OR cvterm.name = 'polypeptide_DNA_contact' OR cvterm.name = 'nucleotide_binding_site';

--- ************************************************
--- *** relation: metal_binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the molecule, in ***
--- *** teracts selectively and non-covalently w ***
--- *** ith metal ions.                          ***
--- ************************************************
---

CREATE VIEW metal_binding_site AS
  SELECT
    feature_id AS metal_binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_metal_contact' OR cvterm.name = 'polypeptide_calcium_ion_contact_site' OR cvterm.name = 'polypeptide_cobalt_ion_contact_site' OR cvterm.name = 'polypeptide_copper_ion_contact_site' OR cvterm.name = 'polypeptide_iron_ion_contact_site' OR cvterm.name = 'polypeptide_magnesium_ion_contact_site' OR cvterm.name = 'polypeptide_manganese_ion_contact_site' OR cvterm.name = 'polypeptide_molybdenum_ion_contact_site' OR cvterm.name = 'polypeptide_nickel_ion_contact_site' OR cvterm.name = 'polypeptide_tungsten_ion_contact_site' OR cvterm.name = 'polypeptide_zinc_ion_contact_site' OR cvterm.name = 'metal_binding_site';

--- ************************************************
--- *** relation: ligand_binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the molecule, in ***
--- *** teracts selectively and non-covalently w ***
--- *** ith a small molecule such as a drug, or  ***
--- *** hormone.                                 ***
--- ************************************************
---

CREATE VIEW ligand_binding_site AS
  SELECT
    feature_id AS ligand_binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_ligand_contact' OR cvterm.name = 'ligand_binding_site';

--- ************************************************
--- *** relation: nested_tandem_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An NTR is a nested repeat of two distinc ***
--- *** t tandem motifs interspersed with each o ***
--- *** ther.                                    ***
--- ************************************************
---

CREATE VIEW nested_tandem_repeat AS
  SELECT
    feature_id AS nested_tandem_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nested_tandem_repeat';

--- ************************************************

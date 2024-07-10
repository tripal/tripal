SET search_path=so,chado,pg_catalog;
--- *** relation: tna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence consi ***
--- *** sting of nucleobases attached to a repea ***
--- *** ting unit made of threose rings connecte ***
--- *** d to a phosphate backbone.               ***
--- ************************************************
---

CREATE VIEW tna AS
  SELECT
    feature_id AS tna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'TNA';

--- ************************************************
--- *** relation: tna_oligo ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An oligo composed of TNA residues.       ***
--- ************************************************
---

CREATE VIEW tna_oligo AS
  SELECT
    feature_id AS tna_oligo_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'TNA_oligo';

--- ************************************************
--- *** relation: gna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence consi ***
--- *** sting of nucleobases attached to a repea ***
--- *** ting unit made of an acyclic three-carbo ***
--- *** n propylene glycol connected to a phosph ***
--- *** ate backbone. It has two enantiomeric fo ***
--- *** rms, (R)-GNA and (S)-GNA.                ***
--- ************************************************
---

CREATE VIEW gna AS
  SELECT
    feature_id AS gna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'R_GNA' OR cvterm.name = 'S_GNA' OR cvterm.name = 'GNA';

--- ************************************************
--- *** relation: gna_oligo ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An oligo composed of GNA residues.       ***
--- ************************************************
---

CREATE VIEW gna_oligo AS
  SELECT
    feature_id AS gna_oligo_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'R_GNA_oligo' OR cvterm.name = 'S_GNA_oligo' OR cvterm.name = 'GNA_oligo';

--- ************************************************
--- *** relation: r_gna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a GNA sequence i ***
--- *** n the (R)-GNA enantiomer.                ***
--- ************************************************
---

CREATE VIEW r_gna AS
  SELECT
    feature_id AS r_gna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'R_GNA';

--- ************************************************
--- *** relation: r_gna_oligo ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An oligo composed of (R)-GNA residues.   ***
--- ************************************************
---

CREATE VIEW r_gna_oligo AS
  SELECT
    feature_id AS r_gna_oligo_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'R_GNA_oligo';

--- ************************************************
--- *** relation: s_gna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a GNA sequence i ***
--- *** n the (S)-GNA enantiomer.                ***
--- ************************************************
---

CREATE VIEW s_gna AS
  SELECT
    feature_id AS s_gna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'S_GNA';

--- ************************************************
--- *** relation: s_gna_oligo ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An oligo composed of (S)-GNA residues.   ***
--- ************************************************
---

CREATE VIEW s_gna_oligo AS
  SELECT
    feature_id AS s_gna_oligo_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'S_GNA_oligo';

--- ************************************************
--- *** relation: ds_dna_viral_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A ds_DNA_viral_sequence is a viral_seque ***
--- *** nce that is the sequence of a virus that ***
--- ***  exists as double stranded DNA.          ***
--- ************************************************
---

CREATE VIEW ds_dna_viral_sequence AS
  SELECT
    feature_id AS ds_dna_viral_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ds_DNA_viral_sequence';

--- ************************************************
--- *** relation: ss_rna_viral_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A ss_RNA_viral_sequence is a viral_seque ***
--- *** nce that is the sequence of a virus that ***
--- ***  exists as single stranded RNA.          ***
--- ************************************************
---

CREATE VIEW ss_rna_viral_sequence AS
  SELECT
    feature_id AS ss_rna_viral_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'negative_sense_ssRNA_viral_sequence' OR cvterm.name = 'positive_sense_ssRNA_viral_sequence' OR cvterm.name = 'ambisense_ssRNA_viral_sequence' OR cvterm.name = 'ss_RNA_viral_sequence';

--- ************************************************
--- *** relation: negative_sense_ssrna_viral_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A negative_sense_RNA_viral_sequence is a ***
--- ***  ss_RNA_viral_sequence that is the seque ***
--- *** nce of a single stranded RNA virus that  ***
--- *** is complementary to mRNA and must be con ***
--- *** verted to positive sense RNA by RNA poly ***
--- *** merase before translation.               ***
--- ************************************************
---

CREATE VIEW negative_sense_ssrna_viral_sequence AS
  SELECT
    feature_id AS negative_sense_ssrna_viral_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'negative_sense_ssRNA_viral_sequence';

--- ************************************************
--- *** relation: positive_sense_ssrna_viral_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A positive_sense_RNA_viral_sequence is a ***
--- ***  ss_RNA_viral_sequence that is the seque ***
--- *** nce of a single stranded RNA virus that  ***
--- *** can be immediately translated by the hos ***
--- *** t.                                       ***
--- ************************************************
---

CREATE VIEW positive_sense_ssrna_viral_sequence AS
  SELECT
    feature_id AS positive_sense_ssrna_viral_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'positive_sense_ssRNA_viral_sequence';

--- ************************************************
--- *** relation: ambisense_ssrna_viral_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A ambisense_RNA_virus is a ss_RNA_viral_ ***
--- *** sequence that is the sequence of a singl ***
--- *** e stranded RNA virus with both messenger ***
--- ***  and anti messenger polarity.            ***
--- ************************************************
---

CREATE VIEW ambisense_ssrna_viral_sequence AS
  SELECT
    feature_id AS ambisense_ssrna_viral_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ambisense_ssRNA_viral_sequence';

--- ************************************************
--- *** relation: rna_polymerase_promoter ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region (DNA) to which RNA polymerase b ***
--- *** inds, to begin transcription.            ***
--- ************************************************
---

CREATE VIEW rna_polymerase_promoter AS
  SELECT
    feature_id AS rna_polymerase_promoter_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNApol_I_promoter' OR cvterm.name = 'RNApol_II_promoter' OR cvterm.name = 'RNApol_III_promoter' OR cvterm.name = 'bacterial_RNApol_promoter' OR cvterm.name = 'Phage_RNA_Polymerase_Promoter' OR cvterm.name = 'RNApol_II_core_promoter' OR cvterm.name = 'RNApol_III_promoter_type_1' OR cvterm.name = 'RNApol_III_promoter_type_2' OR cvterm.name = 'RNApol_III_promoter_type_3' OR cvterm.name = 'bacterial_RNApol_promoter_sigma_70' OR cvterm.name = 'bacterial_RNApol_promoter_sigma54' OR cvterm.name = 'SP6_RNA_Polymerase_Promoter' OR cvterm.name = 'T3_RNA_Polymerase_Promoter' OR cvterm.name = 'T7_RNA_Polymerase_Promoter' OR cvterm.name = 'RNA_polymerase_promoter';

--- ************************************************
--- *** relation: phage_rna_polymerase_promoter ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region (DNA) to which Bacteriophage RN ***
--- *** A polymerase binds, to begin transcripti ***
--- *** on.                                      ***
--- ************************************************
---

CREATE VIEW phage_rna_polymerase_promoter AS
  SELECT
    feature_id AS phage_rna_polymerase_promoter_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SP6_RNA_Polymerase_Promoter' OR cvterm.name = 'T3_RNA_Polymerase_Promoter' OR cvterm.name = 'T7_RNA_Polymerase_Promoter' OR cvterm.name = 'Phage_RNA_Polymerase_Promoter';

--- ************************************************
--- *** relation: sp6_rna_polymerase_promoter ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region (DNA) to which the SP6 RNA poly ***
--- *** merase binds, to begin transcription.    ***
--- ************************************************
---

CREATE VIEW sp6_rna_polymerase_promoter AS
  SELECT
    feature_id AS sp6_rna_polymerase_promoter_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SP6_RNA_Polymerase_Promoter';

--- ************************************************
--- *** relation: t3_rna_polymerase_promoter ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A DNA sequence to which the T3 RNA polym ***
--- *** erase binds, to begin transcription.     ***
--- ************************************************
---

CREATE VIEW t3_rna_polymerase_promoter AS
  SELECT
    feature_id AS t3_rna_polymerase_promoter_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'T3_RNA_Polymerase_Promoter';

--- ************************************************
--- *** relation: t7_rna_polymerase_promoter ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region (DNA) to which the T7 RNA polym ***
--- *** erase binds, to begin transcription.     ***
--- ************************************************
---

CREATE VIEW t7_rna_polymerase_promoter AS
  SELECT
    feature_id AS t7_rna_polymerase_promoter_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'T7_RNA_Polymerase_Promoter';

--- ************************************************
--- *** relation: five_prime_est ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An EST read from the 5' end of a transcr ***
--- *** ipt that usually codes for a protein. Th ***
--- *** ese regions tend to be conserved across  ***
--- *** species and do not change much within a  ***
--- *** gene family.                             ***
--- ************************************************
---

CREATE VIEW five_prime_est AS
  SELECT
    feature_id AS five_prime_est_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_EST';

--- ************************************************
--- *** relation: three_prime_est ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An EST read from the 3' end of a transcr ***
--- *** ipt. They are more likely to fall within ***
--- ***  non-coding, or untranslated regions(UTR ***
--- *** s).                                      ***
--- ************************************************
---

CREATE VIEW three_prime_est AS
  SELECT
    feature_id AS three_prime_est_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_EST';

--- ************************************************
--- *** relation: translational_frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The region of mRNA (not divisible by 3 b ***
--- *** ases) that is skipped during the process ***
--- ***  of translational frameshifting (GO:0006 ***
--- *** 452), causing the reading frame to be di ***
--- *** fferent.                                 ***
--- ************************************************
---

CREATE VIEW translational_frameshift AS
  SELECT
    feature_id AS translational_frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'plus_1_translational_frameshift' OR cvterm.name = 'plus_2_translational_frameshift' OR cvterm.name = 'translational_frameshift';

--- ************************************************
--- *** relation: plus_1_translational_frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The region of mRNA 1 base long that is s ***
--- *** kipped during the process of translation ***
--- *** al frameshifting (GO:0006452), causing t ***
--- *** he reading frame to be different.        ***
--- ************************************************
---

CREATE VIEW plus_1_translational_frameshift AS
  SELECT
    feature_id AS plus_1_translational_frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'plus_1_translational_frameshift';

--- ************************************************
--- *** relation: plus_2_translational_frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The region of mRNA 2 bases long that is  ***
--- *** skipped during the process of translatio ***
--- *** nal frameshifting (GO:0006452), causing  ***
--- *** the reading frame to be different.       ***
--- ************************************************
---

CREATE VIEW plus_2_translational_frameshift AS
  SELECT
    feature_id AS plus_2_translational_frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'plus_2_translational_frameshift';

--- ************************************************
--- *** relation: group_iii_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Group III introns are introns found in t ***
--- *** he mRNA of the plastids of euglenoid pro ***
--- *** tists. They are spliced by a two step tr ***
--- *** ansesterification with bulged adenosine  ***
--- *** as initiating nucleophile.               ***
--- ************************************************
---

CREATE VIEW group_iii_intron AS
  SELECT
    feature_id AS group_iii_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'group_III_intron';

--- ************************************************
--- *** relation: noncoding_region_of_exon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The maximal intersection of exon and UTR ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW noncoding_region_of_exon AS
  SELECT
    feature_id AS noncoding_region_of_exon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_coding_exon_noncoding_region' OR cvterm.name = 'five_prime_coding_exon_noncoding_region' OR cvterm.name = 'noncoding_region_of_exon';

--- ************************************************
--- *** relation: coding_region_of_exon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The region of an exon that encodes for p ***
--- *** rotein sequence.                         ***
--- ************************************************
---

CREATE VIEW coding_region_of_exon AS
  SELECT
    feature_id AS coding_region_of_exon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_coding_exon_coding_region' OR cvterm.name = 'three_prime_coding_exon_coding_region' OR cvterm.name = 'coding_region_of_exon';

--- ************************************************
--- *** relation: endonuclease_spliced_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An intron that spliced via endonucleolyt ***
--- *** ic cleavage and ligation rather than tra ***
--- *** nsesterification.                        ***
--- ************************************************
---

CREATE VIEW endonuclease_spliced_intron AS
  SELECT
    feature_id AS endonuclease_spliced_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'archaeal_intron' OR cvterm.name = 'tRNA_intron' OR cvterm.name = 'endonuclease_spliced_intron';

--- ************************************************
--- *** relation: protein_coding_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW protein_coding_gene AS
  SELECT
    feature_id AS protein_coding_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_polyadenylated_mRNA' OR cvterm.name = 'gene_with_mRNA_with_frameshift' OR cvterm.name = 'gene_with_edited_transcript' OR cvterm.name = 'gene_with_recoded_mRNA' OR cvterm.name = 'gene_with_stop_codon_read_through' OR cvterm.name = 'gene_with_mRNA_recoded_by_translational_bypass' OR cvterm.name = 'gene_with_transcript_with_translational_frameshift' OR cvterm.name = 'gene_with_stop_codon_redefined_as_pyrrolysine' OR cvterm.name = 'gene_with_stop_codon_redefined_as_selenocysteine' OR cvterm.name = 'protein_coding_gene';

--- ************************************************
--- *** relation: transgenic_insertion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An insertion that derives from another o ***
--- *** rganism, via the use of recombinant DNA  ***
--- *** technology.                              ***
--- ************************************************
---

CREATE VIEW transgenic_insertion AS
  SELECT
    feature_id AS transgenic_insertion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transgenic_insertion';

--- ************************************************
--- *** relation: retrogene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW retrogene AS
  SELECT
    feature_id AS retrogene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'retrogene';

--- ************************************************
--- *** relation: silenced_by_rna_interference ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing an epigenetic pr ***
--- *** ocess where a gene is inactivated by RNA ***
--- ***  interference.                           ***
--- ************************************************
---

CREATE VIEW silenced_by_rna_interference AS
  SELECT
    feature_id AS silenced_by_rna_interference_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'silenced_by_RNA_interference';

--- ************************************************
--- *** relation: silenced_by_histone_modification ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing an epigenetic pr ***
--- *** ocess where a gene is inactivated by his ***
--- *** tone modification.                       ***
--- ************************************************
---

CREATE VIEW silenced_by_histone_modification AS
  SELECT
    feature_id AS silenced_by_histone_modification_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'silenced_by_histone_methylation' OR cvterm.name = 'silenced_by_histone_deacetylation' OR cvterm.name = 'silenced_by_histone_modification';

--- ************************************************
--- *** relation: silenced_by_histone_methylation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing an epigenetic pr ***
--- *** ocess where a gene is inactivated by his ***
--- *** tone methylation.                        ***
--- ************************************************
---

CREATE VIEW silenced_by_histone_methylation AS
  SELECT
    feature_id AS silenced_by_histone_methylation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'silenced_by_histone_methylation';

--- ************************************************
--- *** relation: silenced_by_histone_deacetylation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing an epigenetic pr ***
--- *** ocess where a gene is inactivated by his ***
--- *** tone deacetylation.                      ***
--- ************************************************
---

CREATE VIEW silenced_by_histone_deacetylation AS
  SELECT
    feature_id AS silenced_by_histone_deacetylation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'silenced_by_histone_deacetylation';

--- ************************************************
--- *** relation: gene_silenced_by_rna_interference ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is silenced by RNA interfere ***
--- *** nce.                                     ***
--- ************************************************
---

CREATE VIEW gene_silenced_by_rna_interference AS
  SELECT
    feature_id AS gene_silenced_by_rna_interference_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_silenced_by_RNA_interference';

--- ************************************************
--- *** relation: gene_silenced_by_histone_modification ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is silenced by histone modif ***
--- *** ication.                                 ***
--- ************************************************
---

CREATE VIEW gene_silenced_by_histone_modification AS
  SELECT
    feature_id AS gene_silenced_by_histone_modification_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_silenced_by_histone_methylation' OR cvterm.name = 'gene_silenced_by_histone_deacetylation' OR cvterm.name = 'gene_silenced_by_histone_modification';

--- ************************************************
--- *** relation: gene_silenced_by_histone_methylation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is silenced by histone methy ***
--- *** lation.                                  ***
--- ************************************************
---

CREATE VIEW gene_silenced_by_histone_methylation AS
  SELECT
    feature_id AS gene_silenced_by_histone_methylation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_silenced_by_histone_methylation';

--- ************************************************
--- *** relation: gene_silenced_by_histone_deacetylation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is silenced by histone deace ***
--- *** tylation.                                ***
--- ************************************************
---

CREATE VIEW gene_silenced_by_histone_deacetylation AS
  SELECT
    feature_id AS gene_silenced_by_histone_deacetylation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_silenced_by_histone_deacetylation';

--- ************************************************
--- *** relation: dihydrouridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A modified RNA base in which the 5,6-dih ***
--- *** ydrouracil is bound to the ribose ring.  ***
--- ************************************************
---

CREATE VIEW dihydrouridine AS
  SELECT
    feature_id AS dihydrouridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dihydrouridine';

--- ************************************************
--- *** relation: pseudouridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A modified RNA base in which the 5- posi ***
--- *** tion of the uracil is bound to the ribos ***
--- *** e ring instead of the 4- position.       ***
--- ************************************************
---

CREATE VIEW pseudouridine AS
  SELECT
    feature_id AS pseudouridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pseudouridine';

--- ************************************************
--- *** relation: inosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A modified RNA base in which hypoxanthin ***
--- *** e is bound to the ribose ring.           ***
--- ************************************************
---

CREATE VIEW inosine AS
  SELECT
    feature_id AS inosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'modified_inosine' OR cvterm.name = 'methylinosine' OR cvterm.name = 'one_methylinosine' OR cvterm.name = 'one_two_prime_O_dimethylinosine' OR cvterm.name = 'two_prime_O_methylinosine' OR cvterm.name = 'inosine';

--- ************************************************
--- *** relation: seven_methylguanine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A modified RNA base in which guanine is  ***
--- *** methylated at the 7- position.           ***
--- ************************************************
---

CREATE VIEW seven_methylguanine AS
  SELECT
    feature_id AS seven_methylguanine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'seven_methylguanine';

--- ************************************************
--- *** relation: ribothymidine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A modified RNA base in which thymine is  ***
--- *** bound to the ribose ring.                ***
--- ************************************************
---

CREATE VIEW ribothymidine AS
  SELECT
    feature_id AS ribothymidine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ribothymidine';

--- ************************************************
--- *** relation: methylinosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A modified RNA base in which methylhypox ***
--- *** anthine is bound to the ribose ring.     ***
--- ************************************************
---

CREATE VIEW methylinosine AS
  SELECT
    feature_id AS methylinosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'methylinosine';

--- ************************************************
--- *** relation: mobile ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a feature that h ***
--- *** as either intra-genome or intracellular  ***
--- *** mobility.                                ***
--- ************************************************
---

CREATE VIEW mobile AS
  SELECT
    feature_id AS mobile_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mobile';

--- ************************************************
--- *** relation: replicon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region containing at least one unique  ***
--- *** origin of replication and a unique termi ***
--- *** nation site.                             ***
--- ************************************************
---

CREATE VIEW replicon AS
  SELECT
    feature_id AS replicon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'plasmid' OR cvterm.name = 'chromosome' OR cvterm.name = 'vector_replicon' OR cvterm.name = 'maxicircle' OR cvterm.name = 'minicircle' OR cvterm.name = 'viral_sequence' OR cvterm.name = 'engineered_plasmid' OR cvterm.name = 'episome' OR cvterm.name = 'natural_plasmid' OR cvterm.name = 'engineered_episome' OR cvterm.name = 'gene_trap_construct' OR cvterm.name = 'promoter_trap_construct' OR cvterm.name = 'enhancer_trap_construct' OR cvterm.name = 'engineered_episome' OR cvterm.name = 'mitochondrial_chromosome' OR cvterm.name = 'chloroplast_chromosome' OR cvterm.name = 'chromoplast_chromosome' OR cvterm.name = 'cyanelle_chromosome' OR cvterm.name = 'leucoplast_chromosome' OR cvterm.name = 'macronuclear_chromosome' OR cvterm.name = 'micronuclear_chromosome' OR cvterm.name = 'nuclear_chromosome' OR cvterm.name = 'nucleomorphic_chromosome' OR cvterm.name = 'DNA_chromosome' OR cvterm.name = 'RNA_chromosome' OR cvterm.name = 'apicoplast_chromosome' OR cvterm.name = 'double_stranded_DNA_chromosome' OR cvterm.name = 'single_stranded_DNA_chromosome' OR cvterm.name = 'linear_double_stranded_DNA_chromosome' OR cvterm.name = 'circular_double_stranded_DNA_chromosome' OR cvterm.name = 'linear_single_stranded_DNA_chromosome' OR cvterm.name = 'circular_single_stranded_DNA_chromosome' OR cvterm.name = 'single_stranded_RNA_chromosome' OR cvterm.name = 'double_stranded_RNA_chromosome' OR cvterm.name = 'linear_single_stranded_RNA_chromosome' OR cvterm.name = 'circular_single_stranded_RNA_chromosome' OR cvterm.name = 'linear_double_stranded_RNA_chromosome' OR cvterm.name = 'circular_double_stranded_RNA_chromosome' OR cvterm.name = 'YAC' OR cvterm.name = 'BAC' OR cvterm.name = 'PAC' OR cvterm.name = 'cosmid' OR cvterm.name = 'phagemid' OR cvterm.name = 'fosmid' OR cvterm.name = 'lambda_vector' OR cvterm.name = 'plasmid_vector' OR cvterm.name = 'targeting_vector' OR cvterm.name = 'phage_sequence' OR cvterm.name = 'ds_RNA_viral_sequence' OR cvterm.name = 'ds_DNA_viral_sequence' OR cvterm.name = 'ss_RNA_viral_sequence' OR cvterm.name = 'negative_sense_ssRNA_viral_sequence' OR cvterm.name = 'positive_sense_ssRNA_viral_sequence' OR cvterm.name = 'ambisense_ssRNA_viral_sequence' OR cvterm.name = 'replicon';

--- ************************************************
--- *** relation: base ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A base is a sequence feature that corres ***
--- *** ponds to a single unit of a nucleotide p ***
--- *** olymer.                                  ***
--- ************************************************
---

CREATE VIEW base AS
  SELECT
    feature_id AS base_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'modified_RNA_base_feature' OR cvterm.name = 'inosine' OR cvterm.name = 'seven_methylguanine' OR cvterm.name = 'ribothymidine' OR cvterm.name = 'modified_adenosine' OR cvterm.name = 'modified_cytidine' OR cvterm.name = 'modified_guanosine' OR cvterm.name = 'modified_uridine' OR cvterm.name = 'modified_inosine' OR cvterm.name = 'methylinosine' OR cvterm.name = 'one_methylinosine' OR cvterm.name = 'one_two_prime_O_dimethylinosine' OR cvterm.name = 'two_prime_O_methylinosine' OR cvterm.name = 'one_methyladenosine' OR cvterm.name = 'two_methyladenosine' OR cvterm.name = 'N6_methyladenosine' OR cvterm.name = 'two_prime_O_methyladenosine' OR cvterm.name = 'two_methylthio_N6_methyladenosine' OR cvterm.name = 'N6_isopentenyladenosine' OR cvterm.name = 'two_methylthio_N6_isopentenyladenosine' OR cvterm.name = 'N6_cis_hydroxyisopentenyl_adenosine' OR cvterm.name = 'two_methylthio_N6_cis_hydroxyisopentenyl_adenosine' OR cvterm.name = 'N6_glycinylcarbamoyladenosine' OR cvterm.name = 'N6_threonylcarbamoyladenosine' OR cvterm.name = 'two_methylthio_N6_threonyl_carbamoyladenosine' OR cvterm.name = 'N6_methyl_N6_threonylcarbamoyladenosine' OR cvterm.name = 'N6_hydroxynorvalylcarbamoyladenosine' OR cvterm.name = 'two_methylthio_N6_hydroxynorvalyl_carbamoyladenosine' OR cvterm.name = 'two_prime_O_ribosyladenosine_phosphate' OR cvterm.name = 'N6_N6_dimethyladenosine' OR cvterm.name = 'N6_2_prime_O_dimethyladenosine' OR cvterm.name = 'N6_N6_2_prime_O_trimethyladenosine' OR cvterm.name = 'one_two_prime_O_dimethyladenosine' OR cvterm.name = 'N6_acetyladenosine' OR cvterm.name = 'three_methylcytidine' OR cvterm.name = 'five_methylcytidine' OR cvterm.name = 'two_prime_O_methylcytidine' OR cvterm.name = 'two_thiocytidine' OR cvterm.name = 'N4_acetylcytidine' OR cvterm.name = 'five_formylcytidine' OR cvterm.name = 'five_two_prime_O_dimethylcytidine' OR cvterm.name = 'N4_acetyl_2_prime_O_methylcytidine' OR cvterm.name = 'lysidine' OR cvterm.name = 'N4_methylcytidine' OR cvterm.name = 'N4_2_prime_O_dimethylcytidine' OR cvterm.name = 'five_hydroxymethylcytidine' OR cvterm.name = 'five_formyl_two_prime_O_methylcytidine' OR cvterm.name = 'N4_N4_2_prime_O_trimethylcytidine' OR cvterm.name = 'seven_deazaguanosine' OR cvterm.name = 'one_methylguanosine' OR cvterm.name = 'N2_methylguanosine' OR cvterm.name = 'seven_methylguanosine' OR cvterm.name = 'two_prime_O_methylguanosine' OR cvterm.name = 'N2_N2_dimethylguanosine' OR cvterm.name = 'N2_2_prime_O_dimethylguanosine' OR cvterm.name = 'N2_N2_2_prime_O_trimethylguanosine' OR cvterm.name = 'two_prime_O_ribosylguanosine_phosphate' OR cvterm.name = 'wybutosine' OR cvterm.name = 'peroxywybutosine' OR cvterm.name = 'hydroxywybutosine' OR cvterm.name = 'undermodified_hydroxywybutosine' OR cvterm.name = 'wyosine' OR cvterm.name = 'methylwyosine' OR cvterm.name = 'N2_7_dimethylguanosine' OR cvterm.name = 'N2_N2_7_trimethylguanosine' OR cvterm.name = 'one_two_prime_O_dimethylguanosine' OR cvterm.name = 'four_demethylwyosine' OR cvterm.name = 'isowyosine' OR cvterm.name = 'N2_7_2prirme_O_trimethylguanosine' OR cvterm.name = 'queuosine' OR cvterm.name = 'epoxyqueuosine' OR cvterm.name = 'galactosyl_queuosine' OR cvterm.name = 'mannosyl_queuosine' OR cvterm.name = 'seven_cyano_seven_deazaguanosine' OR cvterm.name = 'seven_aminomethyl_seven_deazaguanosine' OR cvterm.name = 'archaeosine' OR cvterm.name = 'dihydrouridine' OR cvterm.name = 'pseudouridine' OR cvterm.name = 'five_methyluridine' OR cvterm.name = 'two_prime_O_methyluridine' OR cvterm.name = 'five_two_prime_O_dimethyluridine' OR cvterm.name = 'one_methylpseudouridine' OR cvterm.name = 'two_prime_O_methylpseudouridine' OR cvterm.name = 'two_thiouridine' OR cvterm.name = 'four_thiouridine' OR cvterm.name = 'five_methyl_2_thiouridine' OR cvterm.name = 'two_thio_two_prime_O_methyluridine' OR cvterm.name = 'three_three_amino_three_carboxypropyl_uridine' OR cvterm.name = 'five_hydroxyuridine' OR cvterm.name = 'five_methoxyuridine' OR cvterm.name = 'uridine_five_oxyacetic_acid' OR cvterm.name = 'uridine_five_oxyacetic_acid_methyl_ester' OR cvterm.name = 'five_carboxyhydroxymethyl_uridine' OR cvterm.name = 'five_carboxyhydroxymethyl_uridine_methyl_ester' OR cvterm.name = 'five_methoxycarbonylmethyluridine' OR cvterm.name = 'five_methoxycarbonylmethyl_two_prime_O_methyluridine' OR cvterm.name = 'five_methoxycarbonylmethyl_two_thiouridine' OR cvterm.name = 'five_aminomethyl_two_thiouridine' OR cvterm.name = 'five_methylaminomethyluridine' OR cvterm.name = 'five_methylaminomethyl_two_thiouridine' OR cvterm.name = 'five_methylaminomethyl_two_selenouridine' OR cvterm.name = 'five_carbamoylmethyluridine' OR cvterm.name = 'five_carbamoylmethyl_two_prime_O_methyluridine' OR cvterm.name = 'five_carboxymethylaminomethyluridine' OR cvterm.name = 'five_carboxymethylaminomethyl_two_prime_O_methyluridine' OR cvterm.name = 'five_carboxymethylaminomethyl_two_thiouridine' OR cvterm.name = 'three_methyluridine' OR cvterm.name = 'one_methyl_three_three_amino_three_carboxypropyl_pseudouridine' OR cvterm.name = 'five_carboxymethyluridine' OR cvterm.name = 'three_two_prime_O_dimethyluridine' OR cvterm.name = 'five_methyldihydrouridine' OR cvterm.name = 'three_methylpseudouridine' OR cvterm.name = 'five_taurinomethyluridine' OR cvterm.name = 'five_taurinomethyl_two_thiouridine' OR cvterm.name = 'five_isopentenylaminomethyl_uridine' OR cvterm.name = 'five_isopentenylaminomethyl_two_thiouridine' OR cvterm.name = 'five_isopentenylaminomethyl_two_prime_O_methyluridine' OR cvterm.name = 'base';

--- ************************************************
--- *** relation: amino_acid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence feature that corresponds to a ***
--- ***  single amino acid residue in a polypept ***
--- *** ide.                                     ***
--- ************************************************
---

CREATE VIEW amino_acid AS
  SELECT
    feature_id AS amino_acid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'catalytic_residue' OR cvterm.name = 'modified_amino_acid_feature' OR cvterm.name = 'alanine' OR cvterm.name = 'valine' OR cvterm.name = 'leucine' OR cvterm.name = 'isoleucine' OR cvterm.name = 'proline' OR cvterm.name = 'tryptophan' OR cvterm.name = 'phenylalanine' OR cvterm.name = 'methionine' OR cvterm.name = 'glycine' OR cvterm.name = 'serine' OR cvterm.name = 'threonine' OR cvterm.name = 'tyrosine' OR cvterm.name = 'cysteine' OR cvterm.name = 'glutamine' OR cvterm.name = 'asparagine' OR cvterm.name = 'lysine' OR cvterm.name = 'arginine' OR cvterm.name = 'histidine' OR cvterm.name = 'aspartic_acid' OR cvterm.name = 'glutamic_acid' OR cvterm.name = 'selenocysteine' OR cvterm.name = 'pyrrolysine' OR cvterm.name = 'modified_glycine' OR cvterm.name = 'modified_L_alanine' OR cvterm.name = 'modified_L_asparagine' OR cvterm.name = 'modified_L_aspartic_acid' OR cvterm.name = 'modified_L_cysteine' OR cvterm.name = 'modified_L_glutamic_acid' OR cvterm.name = 'modified_L_threonine' OR cvterm.name = 'modified_L_tryptophan' OR cvterm.name = 'modified_L_glutamine' OR cvterm.name = 'modified_L_methionine' OR cvterm.name = 'modified_L_isoleucine' OR cvterm.name = 'modified_L_phenylalanine' OR cvterm.name = 'modified_L_histidine' OR cvterm.name = 'modified_L_serine' OR cvterm.name = 'modified_L_lysine' OR cvterm.name = 'modified_L_leucine' OR cvterm.name = 'modified_L_selenocysteine' OR cvterm.name = 'modified_L_valine' OR cvterm.name = 'modified_L_proline' OR cvterm.name = 'modified_L_tyrosine' OR cvterm.name = 'modified_L_arginine' OR cvterm.name = 'amino_acid';

--- ************************************************
--- *** relation: major_tss ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW major_tss AS
  SELECT
    feature_id AS major_tss_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'major_TSS';

--- ************************************************
--- *** relation: minor_tss ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW minor_tss AS
  SELECT
    feature_id AS minor_tss_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minor_TSS';

--- ************************************************
--- *** relation: tss_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The region of a gene from the 5' most TS ***
--- *** S to the 3' TSS.                         ***
--- ************************************************
---

CREATE VIEW tss_region AS
  SELECT
    feature_id AS tss_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'TSS_region';

--- ************************************************
--- *** relation: encodes_alternate_transcription_start_sites ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW encodes_alternate_transcription_start_sites AS
  SELECT
    feature_id AS encodes_alternate_transcription_start_sites_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'encodes_alternate_transcription_start_sites';

--- ************************************************
--- *** relation: mirna_primary_transcript_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A part of an miRNA primary_transcript.   ***
--- ************************************************
---

CREATE VIEW mirna_primary_transcript_region AS
  SELECT
    feature_id AS mirna_primary_transcript_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pre_miRNA' OR cvterm.name = 'miRNA_stem' OR cvterm.name = 'miRNA_loop' OR cvterm.name = 'miRNA_antiguide' OR cvterm.name = 'miRNA_primary_transcript_region';

--- ************************************************
--- *** relation: pre_mirna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The 60-70 nucleotide region remain after ***
--- ***  Drosha processing of the primary transc ***
--- *** ript, that folds back upon itself to for ***
--- *** m a hairpin sructure.                    ***
--- ************************************************
---

CREATE VIEW pre_mirna AS
  SELECT
    feature_id AS pre_mirna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pre_miRNA';

--- ************************************************
--- *** relation: mirna_stem ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The stem of the hairpin loop formed by f ***
--- *** olding of the pre-miRNA.                 ***
--- ************************************************
---

CREATE VIEW mirna_stem AS
  SELECT
    feature_id AS mirna_stem_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'miRNA_stem';

--- ************************************************
--- *** relation: mirna_loop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The loop of the hairpin loop formed by f ***
--- *** olding of the pre-miRNA.                 ***
--- ************************************************
---

CREATE VIEW mirna_loop AS
  SELECT
    feature_id AS mirna_loop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'miRNA_loop';

--- ************************************************
--- *** relation: synthetic_oligo ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An oligo composed of synthetic nucleotid ***
--- *** es.                                      ***
--- ************************************************
---

CREATE VIEW synthetic_oligo AS
  SELECT
    feature_id AS synthetic_oligo_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'morpholino_oligo' OR cvterm.name = 'PNA_oligo' OR cvterm.name = 'LNA_oligo' OR cvterm.name = 'TNA_oligo' OR cvterm.name = 'GNA_oligo' OR cvterm.name = 'R_GNA_oligo' OR cvterm.name = 'S_GNA_oligo' OR cvterm.name = 'synthetic_oligo';

--- ************************************************
--- *** relation: assembly ***

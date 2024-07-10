SET search_path=so,chado,pg_catalog;
--- *** relation: silencer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A regulatory region which upon binding o ***
--- *** f transcription factors, suppress the tr ***
--- *** anscription of the gene or genes they co ***
--- *** ntrol.                                   ***
--- ************************************************
---

CREATE VIEW silencer AS
  SELECT
    feature_id AS silencer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'silencer';

--- ************************************************
--- *** relation: chromosomal_regulatory_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW chromosomal_regulatory_element AS
  SELECT
    feature_id AS chromosomal_regulatory_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'matrix_attachment_site' OR cvterm.name = 'chromosomal_regulatory_element';

--- ************************************************
--- *** relation: insulator ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcriptional cis regulatory region  ***
--- *** that when located between a CM and a gen ***
--- *** e's promoter prevents the CRM from modul ***
--- *** ating that genes expression.             ***
--- ************************************************
---

CREATE VIEW insulator AS
  SELECT
    feature_id AS insulator_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'insulator';

--- ************************************************
--- *** relation: chromosomal_structural_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW chromosomal_structural_element AS
  SELECT
    feature_id AS chromosomal_structural_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'centromere' OR cvterm.name = 'telomere' OR cvterm.name = 'point_centromere' OR cvterm.name = 'regional_centromere' OR cvterm.name = 'chromosomal_structural_element';

--- ************************************************
--- *** relation: five_prime_open_reading_frame ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW five_prime_open_reading_frame AS
  SELECT
    feature_id AS five_prime_open_reading_frame_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_open_reading_frame';

--- ************************************************
--- *** relation: upstream_aug_codon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A start codon upstream of the ORF.       ***
--- ************************************************
---

CREATE VIEW upstream_aug_codon AS
  SELECT
    feature_id AS upstream_aug_codon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'upstream_AUG_codon';

--- ************************************************
--- *** relation: polycistronic_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding for more t ***
--- *** han one gene product.                    ***
--- ************************************************
---

CREATE VIEW polycistronic_primary_transcript AS
  SELECT
    feature_id AS polycistronic_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dicistronic_primary_transcript' OR cvterm.name = 'polycistronic_primary_transcript';

--- ************************************************
--- *** relation: monocistronic_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding for one ge ***
--- *** ne product.                              ***
--- ************************************************
---

CREATE VIEW monocistronic_primary_transcript AS
  SELECT
    feature_id AS monocistronic_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'monocistronic_primary_transcript';

--- ************************************************
--- *** relation: monocistronic_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An mRNA with either a single protein pro ***
--- *** duct, or for which the regions encoding  ***
--- *** all its protein products overlap.        ***
--- ************************************************
---

CREATE VIEW monocistronic_mrna AS
  SELECT
    feature_id AS monocistronic_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'monocistronic_mRNA';

--- ************************************************
--- *** relation: polycistronic_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An mRNA that encodes multiple proteins f ***
--- *** rom at least two non-overlapping regions ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW polycistronic_mrna AS
  SELECT
    feature_id AS polycistronic_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'polycistronic_mRNA';

--- ************************************************
--- *** relation: mini_exon_donor_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript that donates the sp ***
--- *** liced leader to other mRNA.              ***
--- ************************************************
---

CREATE VIEW mini_exon_donor_rna AS
  SELECT
    feature_id AS mini_exon_donor_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mini_exon_donor_RNA';

--- ************************************************
--- *** relation: spliced_leader_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW spliced_leader_rna AS
  SELECT
    feature_id AS spliced_leader_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'spliced_leader_RNA';

--- ************************************************
--- *** relation: engineered_plasmid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A plasmid that is engineered.            ***
--- ************************************************
---

CREATE VIEW engineered_plasmid AS
  SELECT
    feature_id AS engineered_plasmid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_episome' OR cvterm.name = 'gene_trap_construct' OR cvterm.name = 'promoter_trap_construct' OR cvterm.name = 'enhancer_trap_construct' OR cvterm.name = 'engineered_plasmid';

--- ************************************************
--- *** relation: transcribed_spacer_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Part of an rRNA transcription unit that  ***
--- *** is transcribed but discarded during matu ***
--- *** ration, not giving rise to any part of r ***
--- *** RNA.                                     ***
--- ************************************************
---

CREATE VIEW transcribed_spacer_region AS
  SELECT
    feature_id AS transcribed_spacer_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'internal_transcribed_spacer_region' OR cvterm.name = 'external_transcribed_spacer_region' OR cvterm.name = 'transcribed_spacer_region';

--- ************************************************
--- *** relation: internal_transcribed_spacer_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Non-coding regions of DNA sequence that  ***
--- *** separate genes coding for the 28S, 5.8S, ***
--- ***  and 18S ribosomal RNAs.                 ***
--- ************************************************
---

CREATE VIEW internal_transcribed_spacer_region AS
  SELECT
    feature_id AS internal_transcribed_spacer_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'internal_transcribed_spacer_region';

--- ************************************************
--- *** relation: external_transcribed_spacer_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Non-coding regions of DNA that precede t ***
--- *** he sequence that codes for the ribosomal ***
--- ***  RNA.                                    ***
--- ************************************************
---

CREATE VIEW external_transcribed_spacer_region AS
  SELECT
    feature_id AS external_transcribed_spacer_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'external_transcribed_spacer_region';

--- ************************************************
--- *** relation: tetranuc_repeat_microsat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW tetranuc_repeat_microsat AS
  SELECT
    feature_id AS tetranuc_repeat_microsat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tetranucleotide_repeat_microsatellite_feature';

--- ************************************************
--- *** relation: srp_rna_encoding ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW srp_rna_encoding AS
  SELECT
    feature_id AS srp_rna_encoding_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SRP_RNA_encoding';

--- ************************************************
--- *** relation: minisatellite ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A repeat region containing tandemly repe ***
--- *** ated sequences having a unit length of 1 ***
--- *** 0 to 40 bp.                              ***
--- ************************************************
---

CREATE VIEW minisatellite AS
  SELECT
    feature_id AS minisatellite_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minisatellite';

--- ************************************************
--- *** relation: antisense_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Antisense RNA is RNA that is transcribed ***
--- ***  from the coding, rather than the templa ***
--- *** te, strand of DNA. It is therefore compl ***
--- *** ementary to mRNA.                        ***
--- ************************************************
---

CREATE VIEW antisense_rna AS
  SELECT
    feature_id AS antisense_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'MicF_RNA' OR cvterm.name = 'antisense_RNA';

--- ************************************************
--- *** relation: antisense_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The reverse complement of the primary tr ***
--- *** anscript.                                ***
--- ************************************************
---

CREATE VIEW antisense_primary_transcript AS
  SELECT
    feature_id AS antisense_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'antisense_primary_transcript';

--- ************************************************
--- *** relation: sirna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A small RNA molecule that is the product ***
--- ***  of a longer exogenous or endogenous dsR ***
--- *** NA, which is either a bimolecular duplex ***
--- ***  or very long hairpin, processed (via th ***
--- *** e Dicer pathway) such that numerous siRN ***
--- *** As accumulate from both strands of the d ***
--- *** sRNA. SRNAs trigger the cleavage of thei ***
--- *** r target molecules.                      ***
--- ************************************************
---

CREATE VIEW sirna AS
  SELECT
    feature_id AS sirna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'siRNA';

--- ************************************************
--- *** relation: mirna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding a micro RN ***
--- *** A.                                       ***
--- ************************************************
---

CREATE VIEW mirna_primary_transcript AS
  SELECT
    feature_id AS mirna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stRNA_primary_transcript' OR cvterm.name = 'miRNA_primary_transcript';

--- ************************************************
--- *** relation: strna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding a small te ***
--- *** mporal mRNA (SO:0000649).                ***
--- ************************************************
---

CREATE VIEW strna_primary_transcript AS
  SELECT
    feature_id AS strna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stRNA_primary_transcript';

--- ************************************************
--- *** relation: strna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Non-coding RNAs of about 21 nucleotides  ***
--- *** in length that regulate temporal develop ***
--- *** ment; first discovered in C. elegans.    ***
--- ************************************************
---

CREATE VIEW strna AS
  SELECT
    feature_id AS strna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stRNA';

--- ************************************************
--- *** relation: small_subunit_rrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Ribosomal RNA transcript that structures ***
--- ***  the small subunit of the ribosome.      ***
--- ************************************************
---

CREATE VIEW small_subunit_rrna AS
  SELECT
    feature_id AS small_subunit_rrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_18S' OR cvterm.name = 'rRNA_16S' OR cvterm.name = 'small_subunit_rRNA';

--- ************************************************
--- *** relation: large_subunit_rrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Ribosomal RNA transcript that structures ***
--- ***  the large subunit of the ribosome.      ***
--- ************************************************
---

CREATE VIEW large_subunit_rrna AS
  SELECT
    feature_id AS large_subunit_rrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_5_8S' OR cvterm.name = 'rRNA_5S' OR cvterm.name = 'rRNA_28S' OR cvterm.name = 'rRNA_23S' OR cvterm.name = 'rRNA_25S' OR cvterm.name = 'rRNA_21S' OR cvterm.name = 'large_subunit_rRNA';

--- ************************************************
--- *** relation: rrna_5s ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 5S ribosomal RNA (5S rRNA) is a componen ***
--- *** t of the large ribosomal subunit in both ***
--- ***  prokaryotes and eukaryotes. In eukaryot ***
--- *** es, it is synthesised by RNA polymerase  ***
--- *** III (the other eukaryotic rRNAs are clea ***
--- *** ved from a 45S precursor synthesised by  ***
--- *** RNA polymerase I). In Xenopus oocytes, i ***
--- *** t has been shown that fingers 4-7 of the ***
--- ***  nine-zinc finger transcription factor T ***
--- *** FIIIA can bind to the central region of  ***
--- *** 5S RNA. Thus, in addition to positively  ***
--- *** regulating 5S rRNA transcription, TFIIIA ***
--- ***  also stabilizes 5S rRNA until it is req ***
--- *** uired for transcription.                 ***
--- ************************************************
---

CREATE VIEW rrna_5s AS
  SELECT
    feature_id AS rrna_5s_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_5S';

--- ************************************************
--- *** relation: rrna_28s ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A component of the large ribosomal subun ***
--- *** it.                                      ***
--- ************************************************
---

CREATE VIEW rrna_28s AS
  SELECT
    feature_id AS rrna_28s_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_28S';

--- ************************************************
--- *** relation: maxicircle_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A mitochondrial gene located in a maxici ***
--- *** rcle.                                    ***
--- ************************************************
---

CREATE VIEW maxicircle_gene AS
  SELECT
    feature_id AS maxicircle_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cryptogene' OR cvterm.name = 'maxicircle_gene';

--- ************************************************
--- *** relation: ncrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An RNA transcript that does not encode f ***
--- *** or a protein rather the RNA molecule is  ***
--- *** the gene product.                        ***
--- ************************************************
---

CREATE VIEW ncrna AS
  SELECT
    feature_id AS ncrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'scRNA' OR cvterm.name = 'rRNA' OR cvterm.name = 'tRNA' OR cvterm.name = 'snRNA' OR cvterm.name = 'snoRNA' OR cvterm.name = 'small_regulatory_ncRNA' OR cvterm.name = 'RNase_MRP_RNA' OR cvterm.name = 'RNase_P_RNA' OR cvterm.name = 'telomerase_RNA' OR cvterm.name = 'vault_RNA' OR cvterm.name = 'Y_RNA' OR cvterm.name = 'rasiRNA' OR cvterm.name = 'SRP_RNA' OR cvterm.name = 'guide_RNA' OR cvterm.name = 'antisense_RNA' OR cvterm.name = 'siRNA' OR cvterm.name = 'stRNA' OR cvterm.name = 'class_II_RNA' OR cvterm.name = 'class_I_RNA' OR cvterm.name = 'piRNA' OR cvterm.name = 'lincRNA' OR cvterm.name = 'tasiRNA' OR cvterm.name = 'rRNA_cleavage_RNA' OR cvterm.name = 'small_subunit_rRNA' OR cvterm.name = 'large_subunit_rRNA' OR cvterm.name = 'rRNA_18S' OR cvterm.name = 'rRNA_16S' OR cvterm.name = 'rRNA_5_8S' OR cvterm.name = 'rRNA_5S' OR cvterm.name = 'rRNA_28S' OR cvterm.name = 'rRNA_23S' OR cvterm.name = 'rRNA_25S' OR cvterm.name = 'rRNA_21S' OR cvterm.name = 'alanyl_tRNA' OR cvterm.name = 'asparaginyl_tRNA' OR cvterm.name = 'aspartyl_tRNA' OR cvterm.name = 'cysteinyl_tRNA' OR cvterm.name = 'glutaminyl_tRNA' OR cvterm.name = 'glutamyl_tRNA' OR cvterm.name = 'glycyl_tRNA' OR cvterm.name = 'histidyl_tRNA' OR cvterm.name = 'isoleucyl_tRNA' OR cvterm.name = 'leucyl_tRNA' OR cvterm.name = 'lysyl_tRNA' OR cvterm.name = 'methionyl_tRNA' OR cvterm.name = 'phenylalanyl_tRNA' OR cvterm.name = 'prolyl_tRNA' OR cvterm.name = 'seryl_tRNA' OR cvterm.name = 'threonyl_tRNA' OR cvterm.name = 'tryptophanyl_tRNA' OR cvterm.name = 'tyrosyl_tRNA' OR cvterm.name = 'valyl_tRNA' OR cvterm.name = 'pyrrolysyl_tRNA' OR cvterm.name = 'arginyl_tRNA' OR cvterm.name = 'selenocysteinyl_tRNA' OR cvterm.name = 'U1_snRNA' OR cvterm.name = 'U2_snRNA' OR cvterm.name = 'U4_snRNA' OR cvterm.name = 'U4atac_snRNA' OR cvterm.name = 'U5_snRNA' OR cvterm.name = 'U6_snRNA' OR cvterm.name = 'U6atac_snRNA' OR cvterm.name = 'U11_snRNA' OR cvterm.name = 'U12_snRNA' OR cvterm.name = 'C_D_box_snoRNA' OR cvterm.name = 'H_ACA_box_snoRNA' OR cvterm.name = 'U14_snoRNA' OR cvterm.name = 'U3_snoRNA' OR cvterm.name = 'methylation_guide_snoRNA' OR cvterm.name = 'pseudouridylation_guide_snoRNA' OR cvterm.name = 'miRNA' OR cvterm.name = 'RNA_6S' OR cvterm.name = 'CsrB_RsmB_RNA' OR cvterm.name = 'DsrA_RNA' OR cvterm.name = 'OxyS_RNA' OR cvterm.name = 'RprA_RNA' OR cvterm.name = 'RRE_RNA' OR cvterm.name = 'spot_42_RNA' OR cvterm.name = 'tmRNA' OR cvterm.name = 'GcvB_RNA' OR cvterm.name = 'MicF_RNA' OR cvterm.name = 'ncRNA';

--- ************************************************
--- *** relation: strna_encoding ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW strna_encoding AS
  SELECT
    feature_id AS strna_encoding_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stRNA_encoding';

--- ************************************************
--- *** relation: repeat_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of sequence containing one or m ***
--- *** ore repeat units.                        ***
--- ************************************************
---

CREATE VIEW repeat_region AS
  SELECT
    feature_id AS repeat_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'long_terminal_repeat' OR cvterm.name = 'engineered_foreign_repetitive_element' OR cvterm.name = 'inverted_repeat' OR cvterm.name = 'direct_repeat' OR cvterm.name = 'non_LTR_retrotransposon_polymeric_tract' OR cvterm.name = 'dispersed_repeat' OR cvterm.name = 'tandem_repeat' OR cvterm.name = 'X_element_combinatorial_repeat' OR cvterm.name = 'Y_prime_element' OR cvterm.name = 'telomeric_repeat' OR cvterm.name = 'nested_repeat' OR cvterm.name = 'centromeric_repeat' OR cvterm.name = 'five_prime_LTR' OR cvterm.name = 'three_prime_LTR' OR cvterm.name = 'solo_LTR' OR cvterm.name = 'terminal_inverted_repeat' OR cvterm.name = 'five_prime_terminal_inverted_repeat' OR cvterm.name = 'three_prime_terminal_inverted_repeat' OR cvterm.name = 'target_site_duplication' OR cvterm.name = 'CRISPR' OR cvterm.name = 'satellite_DNA' OR cvterm.name = 'microsatellite' OR cvterm.name = 'minisatellite' OR cvterm.name = 'dinucleotide_repeat_microsatellite_feature' OR cvterm.name = 'trinucleotide_repeat_microsatellite_feature' OR cvterm.name = 'tetranucleotide_repeat_microsatellite_feature' OR cvterm.name = 'nested_tandem_repeat' OR cvterm.name = 'regional_centromere_inner_repeat_region' OR cvterm.name = 'regional_centromere_outer_repeat_region' OR cvterm.name = 'repeat_region';

--- ************************************************
--- *** relation: dispersed_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A repeat that is located at dispersed si ***
--- *** tes in the genome.                       ***
--- ************************************************
---

CREATE VIEW dispersed_repeat AS
  SELECT
    feature_id AS dispersed_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dispersed_repeat';

--- ************************************************
--- *** relation: tmrna_encoding ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW tmrna_encoding AS
  SELECT
    feature_id AS tmrna_encoding_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tmRNA_encoding';

--- ************************************************
--- *** relation: spliceosomal_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An intron which is spliced by the splice ***
--- *** osome.                                   ***
--- ************************************************
---

CREATE VIEW spliceosomal_intron AS
  SELECT
    feature_id AS spliceosomal_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U2_intron' OR cvterm.name = 'U12_intron' OR cvterm.name = 'spliceosomal_intron';

--- ************************************************
--- *** relation: trna_encoding ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW trna_encoding AS
  SELECT
    feature_id AS trna_encoding_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tRNA_encoding';

--- ************************************************
--- *** relation: introgressed_chromosome_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW introgressed_chromosome_region AS
  SELECT
    feature_id AS introgressed_chromosome_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'introgressed_chromosome_region';

--- ************************************************
--- *** relation: monocistronic_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript that is monocistronic.      ***
--- ************************************************
---

CREATE VIEW monocistronic_transcript AS
  SELECT
    feature_id AS monocistronic_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'monocistronic_primary_transcript' OR cvterm.name = 'monocistronic_mRNA' OR cvterm.name = 'monocistronic_transcript';

--- ************************************************
--- *** relation: mobile_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An intron (mitochondrial, chloroplast, n ***
--- *** uclear or prokaryotic) that encodes a do ***
--- *** uble strand sequence specific endonuclea ***
--- *** se allowing for mobility.                ***
--- ************************************************
---

CREATE VIEW mobile_intron AS
  SELECT
    feature_id AS mobile_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mobile_intron';

--- ************************************************
--- *** relation: insertion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The sequence of one or more nucleotides  ***
--- *** added between two adjacent nucleotides i ***
--- *** n the sequence.                          ***
--- ************************************************
---

CREATE VIEW insertion AS
  SELECT
    feature_id AS insertion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transgenic_insertion' OR cvterm.name = 'duplication' OR cvterm.name = 'tandem_duplication' OR cvterm.name = 'direct_tandem_duplication' OR cvterm.name = 'inverted_tandem_duplication' OR cvterm.name = 'insertion';

--- ************************************************
--- *** relation: est_match ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A match against an EST sequence.         ***
--- ************************************************
---

CREATE VIEW est_match AS
  SELECT
    feature_id AS est_match_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'EST_match';

--- ************************************************
--- *** relation: sequence_rearrangement_feature ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW sequence_rearrangement_feature AS
  SELECT
    feature_id AS sequence_rearrangement_feature_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'specific_recombination_site' OR cvterm.name = 'chromosome_breakage_sequence' OR cvterm.name = 'internal_eliminated_sequence' OR cvterm.name = 'macronucleus_destined_segment' OR cvterm.name = 'recombination_feature_of_rearranged_gene' OR cvterm.name = 'site_specific_recombination_target_region' OR cvterm.name = 'recombination_signal_sequence' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_feature' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_segment' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_gene_cluster' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_spacer' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_rearranged_segment' OR cvterm.name = 'vertebrate_immunoglobulin_T_cell_receptor_rearranged_gene_cluster' OR cvterm.name = 'vertebrate_immune_system_gene_recombination_signal_feature' OR cvterm.name = 'D_gene' OR cvterm.name = 'V_gene' OR cvterm.name = 'J_gene' OR cvterm.name = 'C_gene' OR cvterm.name = 'D_J_C_cluster' OR cvterm.name = 'J_C_cluster' OR cvterm.name = 'J_cluster' OR cvterm.name = 'V_cluster' OR cvterm.name = 'V_J_cluster' OR cvterm.name = 'V_J_C_cluster' OR cvterm.name = 'C_cluster' OR cvterm.name = 'D_cluster' OR cvterm.name = 'D_J_cluster' OR cvterm.name = 'three_prime_D_spacer' OR cvterm.name = 'five_prime_D_spacer' OR cvterm.name = 'J_spacer' OR cvterm.name = 'V_spacer' OR cvterm.name = 'VD_gene' OR cvterm.name = 'DJ_gene' OR cvterm.name = 'VDJ_gene' OR cvterm.name = 'VJ_gene' OR cvterm.name = 'DJ_J_cluster' OR cvterm.name = 'VDJ_J_C_cluster' OR cvterm.name = 'VDJ_J_cluster' OR cvterm.name = 'VJ_C_cluster' OR cvterm.name = 'VJ_J_C_cluster' OR cvterm.name = 'VJ_J_cluster' OR cvterm.name = 'D_DJ_C_cluster' OR cvterm.name = 'D_DJ_cluster' OR cvterm.name = 'D_DJ_J_C_cluster' OR cvterm.name = 'D_DJ_J_cluster' OR cvterm.name = 'V_DJ_cluster' OR cvterm.name = 'V_DJ_J_cluster' OR cvterm.name = 'V_VDJ_C_cluster' OR cvterm.name = 'V_VDJ_cluster' OR cvterm.name = 'V_VDJ_J_cluster' OR cvterm.name = 'V_VJ_C_cluster' OR cvterm.name = 'V_VJ_cluster' OR cvterm.name = 'V_VJ_J_cluster' OR cvterm.name = 'V_D_DJ_C_cluster' OR cvterm.name = 'V_D_DJ_cluster' OR cvterm.name = 'V_D_DJ_J_C_cluster' OR cvterm.name = 'V_D_DJ_J_cluster' OR cvterm.name = 'V_D_J_C_cluster' OR cvterm.name = 'V_D_J_cluster' OR cvterm.name = 'DJ_C_cluster' OR cvterm.name = 'DJ_J_C_cluster' OR cvterm.name = 'VDJ_C_cluster' OR cvterm.name = 'V_DJ_C_cluster' OR cvterm.name = 'V_DJ_J_C_cluster' OR cvterm.name = 'V_VDJ_J_C_cluster' OR cvterm.name = 'V_VJ_J_C_cluster' OR cvterm.name = 'J_gene_recombination_feature' OR cvterm.name = 'D_gene_recombination_feature' OR cvterm.name = 'V_gene_recombination_feature' OR cvterm.name = 'heptamer_of_recombination_feature_of_vertebrate_immune_system_gene' OR cvterm.name = 'nonamer_of_recombination_feature_of_vertebrate_immune_system_gene' OR cvterm.name = 'five_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_recombination_signal_sequence' OR cvterm.name = 'three_prime_D_heptamer' OR cvterm.name = 'five_prime_D_heptamer' OR cvterm.name = 'J_heptamer' OR cvterm.name = 'V_heptamer' OR cvterm.name = 'three_prime_D_nonamer' OR cvterm.name = 'five_prime_D_nonamer' OR cvterm.name = 'J_nonamer' OR cvterm.name = 'V_nonamer' OR cvterm.name = 'integration_excision_site' OR cvterm.name = 'resolution_site' OR cvterm.name = 'inversion_site' OR cvterm.name = 'inversion_site_part' OR cvterm.name = 'attI_site' OR cvterm.name = 'attP_site' OR cvterm.name = 'attB_site' OR cvterm.name = 'attL_site' OR cvterm.name = 'attR_site' OR cvterm.name = 'attC_site' OR cvterm.name = 'attCtn_site' OR cvterm.name = 'loxP_site' OR cvterm.name = 'dif_site' OR cvterm.name = 'FRT_site' OR cvterm.name = 'IRLinv_site' OR cvterm.name = 'IRRinv_site' OR cvterm.name = 'sequence_rearrangement_feature';

--- ************************************************
--- *** relation: chromosome_breakage_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence within the micronuclear DNA o ***
--- *** f ciliates at which chromosome breakage  ***
--- *** and telomere addition occurs during nucl ***
--- *** ear differentiation.                     ***
--- ************************************************
---

CREATE VIEW chromosome_breakage_sequence AS
  SELECT
    feature_id AS chromosome_breakage_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chromosome_breakage_sequence';

--- ************************************************
--- *** relation: internal_eliminated_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence eliminated from the genome of ***
--- ***  ciliates during nuclear differentiation ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW internal_eliminated_sequence AS
  SELECT
    feature_id AS internal_eliminated_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'internal_eliminated_sequence';

--- ************************************************
--- *** relation: macronucleus_destined_segment ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence that is conserved, although r ***
--- *** earranged relative to the micronucleus,  ***
--- *** in the macronucleus of a ciliate genome. ***
--- ************************************************
---

CREATE VIEW macronucleus_destined_segment AS
  SELECT
    feature_id AS macronucleus_destined_segment_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'macronucleus_destined_segment';

--- ************************************************
--- *** relation: transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An RNA synthesized on a DNA or RNA templ ***
--- *** ate by an RNA polymerase.                ***
--- ************************************************
---

CREATE VIEW transcript AS
  SELECT
    feature_id AS transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polycistronic_transcript' OR cvterm.name = 'transcript_with_translational_frameshift' OR cvterm.name = 'primary_transcript' OR cvterm.name = 'mature_transcript' OR cvterm.name = 'transcript_bound_by_nucleic_acid' OR cvterm.name = 'transcript_bound_by_protein' OR cvterm.name = 'enzymatic_RNA' OR cvterm.name = 'trans_spliced_transcript' OR cvterm.name = 'monocistronic_transcript' OR cvterm.name = 'aberrant_processed_transcript' OR cvterm.name = 'edited_transcript' OR cvterm.name = 'processed_transcript' OR cvterm.name = 'alternatively_spliced_transcript' OR cvterm.name = 'dicistronic_transcript' OR cvterm.name = 'polycistronic_primary_transcript' OR cvterm.name = 'polycistronic_mRNA' OR cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'dicistronic_primary_transcript' OR cvterm.name = 'dicistronic_primary_transcript' OR cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'protein_coding_primary_transcript' OR cvterm.name = 'nc_primary_transcript' OR cvterm.name = 'polycistronic_primary_transcript' OR cvterm.name = 'monocistronic_primary_transcript' OR cvterm.name = 'mini_exon_donor_RNA' OR cvterm.name = 'antisense_primary_transcript' OR cvterm.name = 'capped_primary_transcript' OR cvterm.name = 'pre_edited_mRNA' OR cvterm.name = 'scRNA_primary_transcript' OR cvterm.name = 'rRNA_primary_transcript' OR cvterm.name = 'tRNA_primary_transcript' OR cvterm.name = 'snRNA_primary_transcript' OR cvterm.name = 'snoRNA_primary_transcript' OR cvterm.name = 'tmRNA_primary_transcript' OR cvterm.name = 'SRP_RNA_primary_transcript' OR cvterm.name = 'miRNA_primary_transcript' OR cvterm.name = 'tasiRNA_primary_transcript' OR cvterm.name = 'rRNA_small_subunit_primary_transcript' OR cvterm.name = 'rRNA_large_subunit_primary_transcript' OR cvterm.name = 'alanine_tRNA_primary_transcript' OR cvterm.name = 'arginine_tRNA_primary_transcript' OR cvterm.name = 'asparagine_tRNA_primary_transcript' OR cvterm.name = 'aspartic_acid_tRNA_primary_transcript' OR cvterm.name = 'cysteine_tRNA_primary_transcript' OR cvterm.name = 'glutamic_acid_tRNA_primary_transcript' OR cvterm.name = 'glutamine_tRNA_primary_transcript' OR cvterm.name = 'glycine_tRNA_primary_transcript' OR cvterm.name = 'histidine_tRNA_primary_transcript' OR cvterm.name = 'isoleucine_tRNA_primary_transcript' OR cvterm.name = 'leucine_tRNA_primary_transcript' OR cvterm.name = 'lysine_tRNA_primary_transcript' OR cvterm.name = 'methionine_tRNA_primary_transcript' OR cvterm.name = 'phenylalanine_tRNA_primary_transcript' OR cvterm.name = 'proline_tRNA_primary_transcript' OR cvterm.name = 'serine_tRNA_primary_transcript' OR cvterm.name = 'threonine_tRNA_primary_transcript' OR cvterm.name = 'tryptophan_tRNA_primary_transcript' OR cvterm.name = 'tyrosine_tRNA_primary_transcript' OR cvterm.name = 'valine_tRNA_primary_transcript' OR cvterm.name = 'pyrrolysine_tRNA_primary_transcript' OR cvterm.name = 'selenocysteine_tRNA_primary_transcript' OR cvterm.name = 'methylation_guide_snoRNA_primary_transcript' OR cvterm.name = 'rRNA_cleavage_snoRNA_primary_transcript' OR cvterm.name = 'C_D_box_snoRNA_primary_transcript' OR cvterm.name = 'H_ACA_box_snoRNA_primary_transcript' OR cvterm.name = 'U14_snoRNA_primary_transcript' OR cvterm.name = 'stRNA_primary_transcript' OR cvterm.name = 'dicistronic_primary_transcript' OR cvterm.name = 'mRNA' OR cvterm.name = 'ncRNA' OR cvterm.name = 'mRNA_with_frameshift' OR cvterm.name = 'monocistronic_mRNA' OR cvterm.name = 'polycistronic_mRNA' OR cvterm.name = 'exemplar_mRNA' OR cvterm.name = 'capped_mRNA' OR cvterm.name = 'polyadenylated_mRNA' OR cvterm.name = 'trans_spliced_mRNA' OR cvterm.name = 'edited_mRNA' OR cvterm.name = 'consensus_mRNA' OR cvterm.name = 'recoded_mRNA' OR cvterm.name = 'mRNA_with_minus_1_frameshift' OR cvterm.name = 'mRNA_with_plus_1_frameshift' OR cvterm.name = 'mRNA_with_plus_2_frameshift' OR cvterm.name = 'mRNA_with_minus_2_frameshift' OR cvterm.name = 'dicistronic_mRNA' OR cvterm.name = 'mRNA_recoded_by_translational_bypass' OR cvterm.name = 'mRNA_recoded_by_codon_redefinition' OR cvterm.name = 'scRNA' OR cvterm.name = 'rRNA' OR cvterm.name = 'tRNA' OR cvterm.name = 'snRNA' OR cvterm.name = 'snoRNA' OR cvterm.name = 'small_regulatory_ncRNA' OR cvterm.name = 'RNase_MRP_RNA' OR cvterm.name = 'RNase_P_RNA' OR cvterm.name = 'telomerase_RNA' OR cvterm.name = 'vault_RNA' OR cvterm.name = 'Y_RNA' OR cvterm.name = 'rasiRNA' OR cvterm.name = 'SRP_RNA' OR cvterm.name = 'guide_RNA' OR cvterm.name = 'antisense_RNA' OR cvterm.name = 'siRNA' OR cvterm.name = 'stRNA' OR cvterm.name = 'class_II_RNA' OR cvterm.name = 'class_I_RNA' OR cvterm.name = 'piRNA' OR cvterm.name = 'lincRNA' OR cvterm.name = 'tasiRNA' OR cvterm.name = 'rRNA_cleavage_RNA' OR cvterm.name = 'small_subunit_rRNA' OR cvterm.name = 'large_subunit_rRNA' OR cvterm.name = 'rRNA_18S' OR cvterm.name = 'rRNA_16S' OR cvterm.name = 'rRNA_5_8S' OR cvterm.name = 'rRNA_5S' OR cvterm.name = 'rRNA_28S' OR cvterm.name = 'rRNA_23S' OR cvterm.name = 'rRNA_25S' OR cvterm.name = 'rRNA_21S' OR cvterm.name = 'alanyl_tRNA' OR cvterm.name = 'asparaginyl_tRNA' OR cvterm.name = 'aspartyl_tRNA' OR cvterm.name = 'cysteinyl_tRNA' OR cvterm.name = 'glutaminyl_tRNA' OR cvterm.name = 'glutamyl_tRNA' OR cvterm.name = 'glycyl_tRNA' OR cvterm.name = 'histidyl_tRNA' OR cvterm.name = 'isoleucyl_tRNA' OR cvterm.name = 'leucyl_tRNA' OR cvterm.name = 'lysyl_tRNA' OR cvterm.name = 'methionyl_tRNA' OR cvterm.name = 'phenylalanyl_tRNA' OR cvterm.name = 'prolyl_tRNA' OR cvterm.name = 'seryl_tRNA' OR cvterm.name = 'threonyl_tRNA' OR cvterm.name = 'tryptophanyl_tRNA' OR cvterm.name = 'tyrosyl_tRNA' OR cvterm.name = 'valyl_tRNA' OR cvterm.name = 'pyrrolysyl_tRNA' OR cvterm.name = 'arginyl_tRNA' OR cvterm.name = 'selenocysteinyl_tRNA' OR cvterm.name = 'U1_snRNA' OR cvterm.name = 'U2_snRNA' OR cvterm.name = 'U4_snRNA' OR cvterm.name = 'U4atac_snRNA' OR cvterm.name = 'U5_snRNA' OR cvterm.name = 'U6_snRNA' OR cvterm.name = 'U6atac_snRNA' OR cvterm.name = 'U11_snRNA' OR cvterm.name = 'U12_snRNA' OR cvterm.name = 'C_D_box_snoRNA' OR cvterm.name = 'H_ACA_box_snoRNA' OR cvterm.name = 'U14_snoRNA' OR cvterm.name = 'U3_snoRNA' OR cvterm.name = 'methylation_guide_snoRNA' OR cvterm.name = 'pseudouridylation_guide_snoRNA' OR cvterm.name = 'miRNA' OR cvterm.name = 'RNA_6S' OR cvterm.name = 'CsrB_RsmB_RNA' OR cvterm.name = 'DsrA_RNA' OR cvterm.name = 'OxyS_RNA' OR cvterm.name = 'RprA_RNA' OR cvterm.name = 'RRE_RNA' OR cvterm.name = 'spot_42_RNA' OR cvterm.name = 'tmRNA' OR cvterm.name = 'GcvB_RNA' OR cvterm.name = 'MicF_RNA' OR cvterm.name = 'ribozyme' OR cvterm.name = 'trans_spliced_mRNA' OR cvterm.name = 'monocistronic_primary_transcript' OR cvterm.name = 'monocistronic_mRNA' OR cvterm.name = 'edited_transcript_by_A_to_I_substitution' OR cvterm.name = 'edited_mRNA' OR cvterm.name = 'transcript';

--- ************************************************
--- *** relation: canonical_three_prime_splice_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The canonical 3' splice site has the seq ***
--- *** uence "AG".                              ***
--- ************************************************
---

CREATE VIEW canonical_three_prime_splice_site AS
  SELECT
    feature_id AS canonical_three_prime_splice_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'canonical_three_prime_splice_site';

--- ************************************************
--- *** relation: canonical_five_prime_splice_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The canonical 5' splice site has the seq ***
--- *** uence "GT".                              ***
--- ************************************************
---

CREATE VIEW canonical_five_prime_splice_site AS
  SELECT
    feature_id AS canonical_five_prime_splice_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'canonical_five_prime_splice_site';

--- ************************************************
--- *** relation: non_canonical_three_prime_splice_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A 3' splice site that does not have the  ***
--- *** sequence "AG".                           ***
--- ************************************************
---

CREATE VIEW non_canonical_three_prime_splice_site AS
  SELECT
    feature_id AS non_canonical_three_prime_splice_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'non_canonical_three_prime_splice_site';

--- ************************************************
--- *** relation: non_canonical_five_prime_splice_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A 5' splice site which does not have the ***
--- ***  sequence "GT".                          ***
--- ************************************************
---

CREATE VIEW non_canonical_five_prime_splice_site AS
  SELECT
    feature_id AS non_canonical_five_prime_splice_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'non_canonical_five_prime_splice_site';

--- ************************************************
--- *** relation: non_canonical_start_codon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A start codon that is not the usual AUG  ***
--- *** sequence.                                ***
--- ************************************************
---

CREATE VIEW non_canonical_start_codon AS
  SELECT
    feature_id AS non_canonical_start_codon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'four_bp_start_codon' OR cvterm.name = 'CTG_start_codon' OR cvterm.name = 'non_canonical_start_codon';

--- ************************************************
--- *** relation: aberrant_processed_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript that has been processed "in ***
--- *** correctly", for example by the failure o ***
--- *** f splicing of one or more exons.         ***
--- ************************************************
---

CREATE VIEW aberrant_processed_transcript AS
  SELECT
    feature_id AS aberrant_processed_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'aberrant_processed_transcript';

--- ************************************************
--- *** relation: exonic_splice_enhancer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Exonic splicing enhancers (ESEs) facilit ***
--- *** ate exon definition by assisting in the  ***
--- *** recruitment of splicing factors to the a ***
--- *** djacent intron.                          ***
--- ************************************************
---

CREATE VIEW exonic_splice_enhancer AS
  SELECT
    feature_id AS exonic_splice_enhancer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'exonic_splice_enhancer';

--- ************************************************
--- *** relation: nuclease_sensitive_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of nucleotide sequence targeted ***
--- ***  by a nuclease enzyme.                   ***
--- ************************************************
---

CREATE VIEW nuclease_sensitive_site AS
  SELECT
    feature_id AS nuclease_sensitive_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nuclease_hypersensitive_site' OR cvterm.name = 'group_1_intron_homing_endonuclease_target_region' OR cvterm.name = 'DNAseI_hypersensitive_site' OR cvterm.name = 'nuclease_sensitive_site';

--- ************************************************
--- *** relation: dnasei_hypersensitive_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW dnasei_hypersensitive_site AS
  SELECT
    feature_id AS dnasei_hypersensitive_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DNAseI_hypersensitive_site';

--- ************************************************
--- *** relation: translocation_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosomal translocation whereby the  ***
--- *** chromosomes carrying non-homologous cent ***
--- *** romeres may be recovered independently.  ***
--- *** These chromosomes are described as trans ***
--- *** location elements. This occurs for some  ***
--- *** translocations, particularly but not exc ***
--- *** lusively, reciprocal translocations.     ***
--- ************************************************
---

CREATE VIEW translocation_element AS
  SELECT
    feature_id AS translocation_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'translocation_element';

--- ************************************************
--- *** relation: deletion_junction ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The space between two bases in a sequenc ***
--- *** e which marks the position where a delet ***
--- *** ion has occurred.                        ***
--- ************************************************
---

CREATE VIEW deletion_junction AS
  SELECT
    feature_id AS deletion_junction_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'deletion_junction';

--- ************************************************

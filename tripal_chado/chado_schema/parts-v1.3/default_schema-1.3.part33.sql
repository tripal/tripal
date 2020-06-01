SET search_path=so,chado,pg_catalog;
--- *** relation: benign_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW benign_variant AS
  SELECT
    feature_id AS benign_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'benign_variant';

--- ************************************************
--- *** relation: disease_associated_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW disease_associated_variant AS
  SELECT
    feature_id AS disease_associated_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'disease_associated_variant';

--- ************************************************
--- *** relation: disease_causing_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW disease_causing_variant AS
  SELECT
    feature_id AS disease_causing_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'disease_causing_variant';

--- ************************************************
--- *** relation: lethal_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW lethal_variant AS
  SELECT
    feature_id AS lethal_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'lethal_variant';

--- ************************************************
--- *** relation: quantitative_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW quantitative_variant AS
  SELECT
    feature_id AS quantitative_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'quantitative_variant';

--- ************************************************
--- *** relation: maternal_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW maternal_variant AS
  SELECT
    feature_id AS maternal_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'maternal_variant';

--- ************************************************
--- *** relation: paternal_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW paternal_variant AS
  SELECT
    feature_id AS paternal_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'paternal_variant';

--- ************************************************
--- *** relation: somatic_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW somatic_variant AS
  SELECT
    feature_id AS somatic_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'somatic_variant';

--- ************************************************
--- *** relation: germline_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW germline_variant AS
  SELECT
    feature_id AS germline_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'germline_variant';

--- ************************************************
--- *** relation: pedigree_specific_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW pedigree_specific_variant AS
  SELECT
    feature_id AS pedigree_specific_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pedigree_specific_variant';

--- ************************************************
--- *** relation: population_specific_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW population_specific_variant AS
  SELECT
    feature_id AS population_specific_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'population_specific_variant';

--- ************************************************
--- *** relation: de_novo_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW de_novo_variant AS
  SELECT
    feature_id AS de_novo_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'de_novo_variant';

--- ************************************************
--- *** relation: tf_binding_site_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant located within a tran ***
--- *** scription factor binding site.           ***
--- ************************************************
---

CREATE VIEW tf_binding_site_variant AS
  SELECT
    feature_id AS tf_binding_site_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'TF_binding_site_variant';

--- ************************************************
--- *** relation: missense_codon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant whereby at least one  ***
--- *** base of a codon is changed resulting in  ***
--- *** a codon that encodes for a different ami ***
--- *** no acid.                                 ***
--- ************************************************
---

CREATE VIEW missense_codon AS
  SELECT
    feature_id AS missense_codon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'conservative_missense_codon' OR cvterm.name = 'non_conservative_missense_codon' OR cvterm.name = 'missense_codon';

--- ************************************************
--- *** relation: complex_structural_alteration ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A structural sequence alteration where t ***
--- *** here are multiple equally plausible expl ***
--- *** anations for the change.                 ***
--- ************************************************
---

CREATE VIEW complex_structural_alteration AS
  SELECT
    feature_id AS complex_structural_alteration_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'complex_structural_alteration';

--- ************************************************
--- *** relation: structural_alteration ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW structural_alteration AS
  SELECT
    feature_id AS structural_alteration_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'complex_structural_alteration' OR cvterm.name = 'structural_alteration';

--- ************************************************
--- *** relation: loss_of_heterozygosity ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW loss_of_heterozygosity AS
  SELECT
    feature_id AS loss_of_heterozygosity_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'loss_of_heterozygosity';

--- ************************************************
--- *** relation: splice_donor_5th_base_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that causes a change  ***
--- *** at the 5th base pair after the start of  ***
--- *** the intron in the orientation of the tra ***
--- *** nscript.                                 ***
--- ************************************************
---

CREATE VIEW splice_donor_5th_base_variant AS
  SELECT
    feature_id AS splice_donor_5th_base_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'splice_donor_5th_base_variant';

--- ************************************************
--- *** relation: u_box ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An U-box is a conserved T-rich region up ***
--- *** stream of a retroviral polypurine tract  ***
--- *** that is involved in PPT primer creation  ***
--- *** during reverse transcription.            ***
--- ************************************************
---

CREATE VIEW u_box AS
  SELECT
    feature_id AS u_box_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U_box';

--- ************************************************
--- *** relation: mating_type_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A specialized region in the genomes of s ***
--- *** ome yeast and fungi, the genes of which  ***
--- *** regulate mating type.                    ***
--- ************************************************
---

CREATE VIEW mating_type_region AS
  SELECT
    feature_id AS mating_type_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mating_type_region';

--- ************************************************
--- *** relation: paired_end_fragment ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An assembly region that has been sequenc ***
--- *** ed from both ends resulting in a read_pa ***
--- *** ir (mate_pair).                          ***
--- ************************************************
---

CREATE VIEW paired_end_fragment AS
  SELECT
    feature_id AS paired_end_fragment_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'paired_end_fragment';

--- ************************************************
--- *** relation: exon_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that changes exon seq ***
--- *** uence.                                   ***
--- ************************************************
---

CREATE VIEW exon_variant AS
  SELECT
    feature_id AS exon_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'coding_sequence_variant' OR cvterm.name = 'non_coding_exon_variant' OR cvterm.name = 'codon_variant' OR cvterm.name = 'frameshift_variant' OR cvterm.name = 'inframe_variant' OR cvterm.name = 'initiator_codon_change' OR cvterm.name = 'non_synonymous_codon' OR cvterm.name = 'synonymous_codon' OR cvterm.name = 'terminal_codon_variant' OR cvterm.name = 'stop_gained' OR cvterm.name = 'missense_codon' OR cvterm.name = 'conservative_missense_codon' OR cvterm.name = 'non_conservative_missense_codon' OR cvterm.name = 'terminator_codon_variant' OR cvterm.name = 'incomplete_terminal_codon_variant' OR cvterm.name = 'stop_retained_variant' OR cvterm.name = 'stop_lost' OR cvterm.name = 'frame_restoring_variant' OR cvterm.name = 'minus_1_frameshift_variant' OR cvterm.name = 'minus_2_frameshift_variant' OR cvterm.name = 'plus_1_frameshift_variant' OR cvterm.name = 'plus_2_frameshift variant' OR cvterm.name = 'inframe_codon_gain' OR cvterm.name = 'inframe_codon_loss' OR cvterm.name = 'exon_variant';

--- ************************************************
--- *** relation: non_coding_exon_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that changes non-codi ***
--- *** ng exon sequence.                        ***
--- ************************************************
---

CREATE VIEW non_coding_exon_variant AS
  SELECT
    feature_id AS non_coding_exon_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'non_coding_exon_variant';

--- ************************************************
--- *** relation: clone_end ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A read from an end of the clone sequence ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW clone_end AS
  SELECT
    feature_id AS clone_end_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'clone_end';

--- ************************************************
--- *** relation: point_centromere ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A point centromere is a relatively small ***
--- ***  centromere (about 125 bp DNA) in discre ***
--- *** te sequence, found in some yeast includi ***
--- *** ng S. cerevisiae.                        ***
--- ************************************************
---

CREATE VIEW point_centromere AS
  SELECT
    feature_id AS point_centromere_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'point_centromere';

--- ************************************************
--- *** relation: regional_centromere ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A regional centromere is a large modular ***
--- ***  centromere found in fission yeast and h ***
--- *** igher eukaryotes. It consist of a centra ***
--- *** l core region flanked by inverted inner  ***
--- *** and outer repeat regions.                ***
--- ************************************************
---

CREATE VIEW regional_centromere AS
  SELECT
    feature_id AS regional_centromere_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'regional_centromere';

--- ************************************************
--- *** relation: regional_centromere_central_core ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A conserved region within the central re ***
--- *** gion of a modular centromere, where the  ***
--- *** kinetochore is formed.                   ***
--- ************************************************
---

CREATE VIEW regional_centromere_central_core AS
  SELECT
    feature_id AS regional_centromere_central_core_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'regional_centromere_central_core';

--- ************************************************
--- *** relation: centromeric_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A repeat region found within the modular ***
--- ***  centromere.                             ***
--- ************************************************
---

CREATE VIEW centromeric_repeat AS
  SELECT
    feature_id AS centromeric_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'regional_centromere_inner_repeat_region' OR cvterm.name = 'regional_centromere_outer_repeat_region' OR cvterm.name = 'centromeric_repeat';

--- ************************************************
--- *** relation: regional_centromere_inner_repeat_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The inner repeat region of a modular cen ***
--- *** tromere. This region is adjacent to the  ***
--- *** central core, on each chromosome arm.    ***
--- ************************************************
---

CREATE VIEW regional_centromere_inner_repeat_region AS
  SELECT
    feature_id AS regional_centromere_inner_repeat_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'regional_centromere_inner_repeat_region';

--- ************************************************
--- *** relation: regional_centromere_outer_repeat_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The heterochromatic outer repeat region  ***
--- *** of a modular centromere. These repeats e ***
--- *** xist in tandem arrays on both chromosome ***
--- ***  arms.                                   ***
--- ************************************************
---

CREATE VIEW regional_centromere_outer_repeat_region AS
  SELECT
    feature_id AS regional_centromere_outer_repeat_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'regional_centromere_outer_repeat_region';

--- ************************************************
--- *** relation: tasirna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The sequence of a 21 nucleotide double s ***
--- *** tranded, polyadenylated non coding RNA,  ***
--- *** transcribed from the TAS gene.           ***
--- ************************************************
---

CREATE VIEW tasirna AS
  SELECT
    feature_id AS tasirna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tasiRNA';

--- ************************************************
--- *** relation: tasirna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding a tasiRNA. ***
--- ************************************************
---

CREATE VIEW tasirna_primary_transcript AS
  SELECT
    feature_id AS tasirna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tasiRNA_primary_transcript';

--- ************************************************
--- *** relation: increased_polyadenylation_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript processing variant whereby  ***
--- *** polyadenylation of the encoded transcrip ***
--- *** t is increased with respect to the refer ***
--- *** ence.                                    ***
--- ************************************************
---

CREATE VIEW increased_polyadenylation_variant AS
  SELECT
    feature_id AS increased_polyadenylation_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'increased_polyadenylation_variant';

--- ************************************************
--- *** relation: decreased_polyadenylation_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript processing variant whereby  ***
--- *** polyadenylation of the encoded transcrip ***
--- *** t is decreased with respect to the refer ***
--- *** ence.                                    ***
--- ************************************************
---

CREATE VIEW decreased_polyadenylation_variant AS
  SELECT
    feature_id AS decreased_polyadenylation_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'decreased_polyadenylation_variant';

--- ************************************************
--- *** relation: regulatory_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of sequence that is involved in ***
--- ***  the control of a biological process.    ***
--- ************************************************
---

CREATE VIEW regulatory_region AS
  SELECT
    feature_id AS regulatory_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transcription_regulatory_region' OR cvterm.name = 'translation_regulatory_region' OR cvterm.name = 'recombination_regulatory_region' OR cvterm.name = 'replication_regulatory_region' OR cvterm.name = 'terminator' OR cvterm.name = 'TF_binding_site' OR cvterm.name = 'polyA_signal_sequence' OR cvterm.name = 'gene_group_regulatory_region' OR cvterm.name = 'transcriptional_cis_regulatory_region' OR cvterm.name = 'splicing_regulatory_region' OR cvterm.name = 'cis_regulatory_frameshift_element' OR cvterm.name = 'intronic_regulatory_region' OR cvterm.name = 'bacterial_terminator' OR cvterm.name = 'eukaryotic_terminator' OR cvterm.name = 'rho_dependent_bacterial_terminator' OR cvterm.name = 'rho_independent_bacterial_terminator' OR cvterm.name = 'terminator_of_type_2_RNApol_III_promoter' OR cvterm.name = 'operator' OR cvterm.name = 'bacterial_RNApol_promoter' OR cvterm.name = 'bacterial_terminator' OR cvterm.name = 'bacterial_RNApol_promoter_sigma_70' OR cvterm.name = 'bacterial_RNApol_promoter_sigma54' OR cvterm.name = 'rho_dependent_bacterial_terminator' OR cvterm.name = 'rho_independent_bacterial_terminator' OR cvterm.name = 'promoter' OR cvterm.name = 'insulator' OR cvterm.name = 'CRM' OR cvterm.name = 'promoter_targeting_sequence' OR cvterm.name = 'ISRE' OR cvterm.name = 'bidirectional_promoter' OR cvterm.name = 'RNA_polymerase_promoter' OR cvterm.name = 'RNApol_I_promoter' OR cvterm.name = 'RNApol_II_promoter' OR cvterm.name = 'RNApol_III_promoter' OR cvterm.name = 'bacterial_RNApol_promoter' OR cvterm.name = 'Phage_RNA_Polymerase_Promoter' OR cvterm.name = 'RNApol_II_core_promoter' OR cvterm.name = 'RNApol_III_promoter_type_1' OR cvterm.name = 'RNApol_III_promoter_type_2' OR cvterm.name = 'RNApol_III_promoter_type_3' OR cvterm.name = 'bacterial_RNApol_promoter_sigma_70' OR cvterm.name = 'bacterial_RNApol_promoter_sigma54' OR cvterm.name = 'SP6_RNA_Polymerase_Promoter' OR cvterm.name = 'T3_RNA_Polymerase_Promoter' OR cvterm.name = 'T7_RNA_Polymerase_Promoter' OR cvterm.name = 'locus_control_region' OR cvterm.name = 'enhancer' OR cvterm.name = 'silencer' OR cvterm.name = 'enhancer_bound_by_factor' OR cvterm.name = 'shadow_enhancer' OR cvterm.name = 'splice_enhancer' OR cvterm.name = 'intronic_splice_enhancer' OR cvterm.name = 'exonic_splice_enhancer' OR cvterm.name = 'attenuator' OR cvterm.name = 'regulatory_region';

--- ************************************************
--- *** relation: u14_snorna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The primary transcript of an evolutionar ***
--- *** ily conserved eukaryotic low molecular w ***
--- *** eight RNA capable of intermolecular hybr ***
--- *** idization with both homologous and heter ***
--- *** ologous 18S rRNA.                        ***
--- ************************************************
---

CREATE VIEW u14_snorna_primary_transcript AS
  SELECT
    feature_id AS u14_snorna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U14_snoRNA_primary_transcript';

--- ************************************************
--- *** relation: methylation_guide_snorna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A snoRNA that specifies the site of 2'-O ***
--- *** -ribose methylation in an RNA molecule b ***
--- *** y base pairing with a short sequence aro ***
--- *** und the target residue.                  ***
--- ************************************************
---

CREATE VIEW methylation_guide_snorna AS
  SELECT
    feature_id AS methylation_guide_snorna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'methylation_guide_snoRNA';

--- ************************************************
--- *** relation: rrna_cleavage_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An ncRNA that is part of a ribonucleopro ***
--- *** tein that cleaves the primary pre-rRNA t ***
--- *** ranscript in the process of producing ma ***
--- *** ture rRNA molecules.                     ***
--- ************************************************
---

CREATE VIEW rrna_cleavage_rna AS
  SELECT
    feature_id AS rrna_cleavage_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_cleavage_RNA';

--- ************************************************
--- *** relation: exon_of_single_exon_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An exon that is the only exon in a gene. ***
--- ************************************************
---

CREATE VIEW exon_of_single_exon_gene AS
  SELECT
    feature_id AS exon_of_single_exon_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'exon_of_single_exon_gene';

--- ************************************************
--- *** relation: cassette_array_member ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW cassette_array_member AS
  SELECT
    feature_id AS cassette_array_member_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cassette_array_member';

--- ************************************************
--- *** relation: gene_cassette_member ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW gene_cassette_member AS
  SELECT
    feature_id AS gene_cassette_member_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cassette_array_member' OR cvterm.name = 'gene_cassette_member';

--- ************************************************
--- *** relation: gene_subarray_member ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW gene_subarray_member AS
  SELECT
    feature_id AS gene_subarray_member_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_subarray_member';

--- ************************************************
--- *** relation: primer_binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Non-covalent primer binding site for ini ***
--- *** tiation of replication, transcription, o ***
--- *** r reverse transcription.                 ***
--- ************************************************
---

CREATE VIEW primer_binding_site AS
  SELECT
    feature_id AS primer_binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'primer_binding_site';

--- ************************************************
--- *** relation: gene_array ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An array includes two or more genes, or  ***
--- *** two or more gene subarrays, contiguously ***
--- ***  arranged where the individual genes, or ***
--- ***  subarrays, are either identical in sequ ***
--- *** ence, or essentially so.                 ***
--- ************************************************
---

CREATE VIEW gene_array AS
  SELECT
    feature_id AS gene_array_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_array';

--- ************************************************
--- *** relation: gene_subarray ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A subarray is, by defintition, a member  ***
--- *** of a gene array (SO:0005851); the member ***
--- *** s of a subarray may differ substantially ***
--- ***  in sequence, but are closely related in ***
--- ***  function.                               ***
--- ************************************************
---

CREATE VIEW gene_subarray AS
  SELECT
    feature_id AS gene_subarray_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_subarray';

--- ************************************************
--- *** relation: gene_cassette ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that can be substituted for a rel ***
--- *** ated gene at a different site in the gen ***
--- *** ome.                                     ***
--- ************************************************
---

CREATE VIEW gene_cassette AS
  SELECT
    feature_id AS gene_cassette_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_cassette';

--- ************************************************
--- *** relation: gene_cassette_array ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An array of non-functional genes whose m ***
--- *** embers, when captured by recombination f ***
--- *** orm functional genes.                    ***
--- ************************************************
---

CREATE VIEW gene_cassette_array AS
  SELECT
    feature_id AS gene_cassette_array_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_cassette_array';

--- ************************************************
--- *** relation: gene_group ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A collection of related genes.           ***
--- ************************************************
---

CREATE VIEW gene_group AS
  SELECT
    feature_id AS gene_group_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'operon' OR cvterm.name = 'mating_type_region' OR cvterm.name = 'gene_array' OR cvterm.name = 'gene_subarray' OR cvterm.name = 'gene_cassette_array' OR cvterm.name = 'regulon' OR cvterm.name = 'gene_group';

--- ************************************************
--- *** relation: selenocysteine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding seryl tRNA ***
--- ***  (SO:000269).                            ***
--- ************************************************
---

CREATE VIEW selenocysteine_trna_primary_transcript AS
  SELECT
    feature_id AS selenocysteine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'selenocysteine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: selenocysteinyl_trna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tRNA sequence that has a selenocystein ***
--- *** e anticodon, and a 3' selenocysteine bin ***
--- *** ding region.                             ***
--- ************************************************
---

CREATE VIEW selenocysteinyl_trna AS
  SELECT
    feature_id AS selenocysteinyl_trna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'selenocysteinyl_tRNA';

--- ************************************************
--- *** relation: syntenic_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region in which two or more pairs of h ***
--- *** omologous markers occur on the same chro ***
--- *** mosome in two or more species.           ***
--- ************************************************
---

CREATE VIEW syntenic_region AS
  SELECT
    feature_id AS syntenic_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'syntenic_region';

--- ************************************************
--- *** relation: biochemical_region_of_peptide ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a peptide that is involved i ***
--- *** n a biochemical function.                ***
--- ************************************************
---

CREATE VIEW biochemical_region_of_peptide AS
  SELECT
    feature_id AS biochemical_region_of_peptide_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'post_translationally_modified_region' OR cvterm.name = 'conformational_switch' OR cvterm.name = 'molecular_contact_region' OR cvterm.name = 'polypeptide_binding_motif' OR cvterm.name = 'polypeptide_catalytic_motif' OR cvterm.name = 'histone_modification' OR cvterm.name = 'histone_methylation_site' OR cvterm.name = 'histone_acetylation_site' OR cvterm.name = 'histone_ubiqitination_site' OR cvterm.name = 'histone_acylation_region' OR cvterm.name = 'H4K20_monomethylation_site' OR cvterm.name = 'H2BK5_monomethylation_site' OR cvterm.name = 'H3K27_methylation_site' OR cvterm.name = 'H3K36_methylation_site' OR cvterm.name = 'H3K4_methylation_site' OR cvterm.name = 'H3K79_methylation_site' OR cvterm.name = 'H3K9_methylation_site' OR cvterm.name = 'H3K27_monomethylation_site' OR cvterm.name = 'H3K27_trimethylation_site' OR cvterm.name = 'H3K27_dimethylation_site' OR cvterm.name = 'H3K36_monomethylation_site' OR cvterm.name = 'H3K36_dimethylation_site' OR cvterm.name = 'H3K36_trimethylation_site' OR cvterm.name = 'H3K4_monomethylation_site' OR cvterm.name = 'H3K4_trimethylation' OR cvterm.name = 'H3K4_dimethylation_site' OR cvterm.name = 'H3K79_monomethylation_site' OR cvterm.name = 'H3K79_dimethylation_site' OR cvterm.name = 'H3K79_trimethylation_site' OR cvterm.name = 'H3K9_trimethylation_site' OR cvterm.name = 'H3K9_monomethylation_site' OR cvterm.name = 'H3K9_dimethylation_site' OR cvterm.name = 'H3K9_acetylation_site' OR cvterm.name = 'H3K14_acetylation_site' OR cvterm.name = 'H3K18_acetylation_site' OR cvterm.name = 'H3K23_acylation site' OR cvterm.name = 'H3K27_acylation_site' OR cvterm.name = 'H4K16_acylation_site' OR cvterm.name = 'H4K5_acylation_site' OR cvterm.name = 'H4K8_acylation site' OR cvterm.name = 'H2B_ubiquitination_site' OR cvterm.name = 'H4K_acylation_region' OR cvterm.name = 'polypeptide_metal_contact' OR cvterm.name = 'protein_protein_contact' OR cvterm.name = 'polypeptide_ligand_contact' OR cvterm.name = 'polypeptide_DNA_contact' OR cvterm.name = 'polypeptide_calcium_ion_contact_site' OR cvterm.name = 'polypeptide_cobalt_ion_contact_site' OR cvterm.name = 'polypeptide_copper_ion_contact_site' OR cvterm.name = 'polypeptide_iron_ion_contact_site' OR cvterm.name = 'polypeptide_magnesium_ion_contact_site' OR cvterm.name = 'polypeptide_manganese_ion_contact_site' OR cvterm.name = 'polypeptide_molybdenum_ion_contact_site' OR cvterm.name = 'polypeptide_nickel_ion_contact_site' OR cvterm.name = 'polypeptide_tungsten_ion_contact_site' OR cvterm.name = 'polypeptide_zinc_ion_contact_site' OR cvterm.name = 'biochemical_region_of_peptide';

--- ************************************************
--- *** relation: molecular_contact_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region that is involved a contact with ***
--- ***  another molecule.                       ***
--- ************************************************
---

CREATE VIEW molecular_contact_region AS
  SELECT
    feature_id AS molecular_contact_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_metal_contact' OR cvterm.name = 'protein_protein_contact' OR cvterm.name = 'polypeptide_ligand_contact' OR cvterm.name = 'polypeptide_DNA_contact' OR cvterm.name = 'polypeptide_calcium_ion_contact_site' OR cvterm.name = 'polypeptide_cobalt_ion_contact_site' OR cvterm.name = 'polypeptide_copper_ion_contact_site' OR cvterm.name = 'polypeptide_iron_ion_contact_site' OR cvterm.name = 'polypeptide_magnesium_ion_contact_site' OR cvterm.name = 'polypeptide_manganese_ion_contact_site' OR cvterm.name = 'polypeptide_molybdenum_ion_contact_site' OR cvterm.name = 'polypeptide_nickel_ion_contact_site' OR cvterm.name = 'polypeptide_tungsten_ion_contact_site' OR cvterm.name = 'polypeptide_zinc_ion_contact_site' OR cvterm.name = 'molecular_contact_region';

--- ************************************************
--- *** relation: intrinsically_unstructured_polypeptide_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of polypeptide chain with high  ***
--- *** conformational flexibility.              ***
--- ************************************************
---

CREATE VIEW intrinsically_unstructured_polypeptide_region AS
  SELECT
    feature_id AS intrinsically_unstructured_polypeptide_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'intrinsically_unstructured_polypeptide_region';

--- ************************************************
--- *** relation: catmat_left_handed_three ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of 3 consecutive residues with d ***
--- *** ihedral angles as follows: res i: phi -9 ***
--- *** 0 bounds -120 to -60, res i: psi -10 bou ***
--- *** nds -50 to 30, res i+1: phi -75 bounds - ***
--- *** 100 to -50, res i+1: psi 140 bounds 110  ***
--- *** to 170. An extra restriction of the leng ***
--- *** th of the O to O distance would be usefu ***
--- *** l, that it be less than 5 Angstrom. More ***
--- ***  precisely these two oxygens are the mai ***
--- *** n chain carbonyl oxygen atoms of residue ***
--- *** s i-1 and i+1.                           ***
--- ************************************************
---

CREATE VIEW catmat_left_handed_three AS
  SELECT
    feature_id AS catmat_left_handed_three_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'catmat_left_handed_three';

--- ************************************************
--- *** relation: catmat_left_handed_four ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of 4 consecutive residues with d ***
--- *** ihedral angles as follows: res i: phi -9 ***
--- *** 0 bounds -120 to -60, res i psi -10 boun ***
--- *** ds -50 to 30, res i+1: phi -90 bounds -1 ***
--- *** 20 to -60, res i+1: psi -10 bounds -50 t ***
--- *** o 30, res i+2: phi -75 bounds -100 to -5 ***
--- *** 0, res i+2: psi 140 bounds 110 to 170.   ***
--- *** The extra restriction of the length of t ***
--- *** he O to O distance is similar, that it b ***
--- *** e less than 5 Angstrom. In this case the ***
--- *** se two Oxygen atoms are the main chain c ***
--- *** arbonyl oxygen atoms of residues i-1 and ***
--- ***  i+2.                                    ***
--- ************************************************
---

CREATE VIEW catmat_left_handed_four AS
  SELECT
    feature_id AS catmat_left_handed_four_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'catmat_left_handed_four';

--- ************************************************
--- *** relation: catmat_right_handed_three ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of 3 consecutive residues with d ***
--- *** ihedral angles as follows: res i: phi -9 ***
--- *** 0 bounds -120 to -60, res i: psi -10 bou ***
--- *** nds -50 to 30, res i+1: phi -75 bounds - ***
--- *** 100 to -50, res i+1: psi 140 bounds 110  ***
--- *** to 170. An extra restriction of the leng ***
--- *** th of the O to O distance would be usefu ***
--- *** l, that it be less than 5 Angstrom. More ***
--- ***  precisely these two oxygens are the mai ***
--- *** n chain carbonyl oxygen atoms of residue ***
--- *** s i-1 and i+1.                           ***
--- ************************************************
---

CREATE VIEW catmat_right_handed_three AS
  SELECT
    feature_id AS catmat_right_handed_three_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'catmat_right_handed_three';

--- ************************************************

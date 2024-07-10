SET search_path=so,chado,pg_catalog;
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript processing variant whereby  ***
--- *** the process of editing is disrupted with ***
--- ***  respect to the reference.               ***
--- ************************************************
---

CREATE VIEW editing_variant AS
  SELECT
    feature_id AS editing_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'editing_variant';

--- ************************************************
--- *** relation: polyadenylation_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that changes polyaden ***
--- *** ylation with respect to a reference sequ ***
--- *** ence.                                    ***
--- ************************************************
---

CREATE VIEW polyadenylation_variant AS
  SELECT
    feature_id AS polyadenylation_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'increased_polyadenylation_variant' OR cvterm.name = 'decreased_polyadenylation_variant' OR cvterm.name = 'polyadenylation_variant';

--- ************************************************
--- *** relation: transcript_stability_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A variant that changes the stability of  ***
--- *** a transcript with respect to a reference ***
--- ***  sequence.                               ***
--- ************************************************
---

CREATE VIEW transcript_stability_variant AS
  SELECT
    feature_id AS transcript_stability_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'decreased_transcript_stability_variant' OR cvterm.name = 'increased_transcript_stability_variant' OR cvterm.name = 'transcript_stability_variant';

--- ************************************************
--- *** relation: decreased_transcript_stability_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that decreases transc ***
--- *** ript stability with respect to a referen ***
--- *** ce sequence.                             ***
--- ************************************************
---

CREATE VIEW decreased_transcript_stability_variant AS
  SELECT
    feature_id AS decreased_transcript_stability_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'decreased_transcript_stability_variant';

--- ************************************************
--- *** relation: increased_transcript_stability_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that increases transc ***
--- *** ript stability with respect to a referen ***
--- *** ce sequence.                             ***
--- ************************************************
---

CREATE VIEW increased_transcript_stability_variant AS
  SELECT
    feature_id AS increased_transcript_stability_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'increased_transcript_stability_variant';

--- ************************************************
--- *** relation: transcription_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A variant that changes alters the transc ***
--- *** ription of a transcript with respect to  ***
--- *** a reference sequence.                    ***
--- ************************************************
---

CREATE VIEW transcription_variant AS
  SELECT
    feature_id AS transcription_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rate_of_transcription_variant' OR cvterm.name = 'increased_transcription_rate_variant' OR cvterm.name = 'decreased_transcription_rate_variant' OR cvterm.name = 'transcription_variant';

--- ************************************************
--- *** relation: rate_of_transcription_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that changes the rate ***
--- ***  of transcription with respect to a refe ***
--- *** rence sequence.                          ***
--- ************************************************
---

CREATE VIEW rate_of_transcription_variant AS
  SELECT
    feature_id AS rate_of_transcription_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'increased_transcription_rate_variant' OR cvterm.name = 'decreased_transcription_rate_variant' OR cvterm.name = 'rate_of_transcription_variant';

--- ************************************************
--- *** relation: increased_transcription_rate_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that increases the ra ***
--- *** te of transcription with respect to a re ***
--- *** ference sequence.                        ***
--- ************************************************
---

CREATE VIEW increased_transcription_rate_variant AS
  SELECT
    feature_id AS increased_transcription_rate_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'increased_transcription_rate_variant';

--- ************************************************
--- *** relation: decreased_transcription_rate_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that decreases the ra ***
--- *** te of transcription with respect to a re ***
--- *** ference sequence.                        ***
--- ************************************************
---

CREATE VIEW decreased_transcription_rate_variant AS
  SELECT
    feature_id AS decreased_transcription_rate_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'decreased_transcription_rate_variant';

--- ************************************************
--- *** relation: translational_product_level_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A functional variant that changes the tr ***
--- *** anslational product level with respect t ***
--- *** o a reference sequence.                  ***
--- ************************************************
---

CREATE VIEW translational_product_level_variant AS
  SELECT
    feature_id AS translational_product_level_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'decreased_translational_product_level' OR cvterm.name = 'increased_translational_product_level' OR cvterm.name = 'translational_product_level_variant';

--- ************************************************
--- *** relation: polypeptide_function_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant which changes polypep ***
--- *** tide functioning with respect to a refer ***
--- *** ence sequence.                           ***
--- ************************************************
---

CREATE VIEW polypeptide_function_variant AS
  SELECT
    feature_id AS polypeptide_function_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_gain_of_function_variant' OR cvterm.name = 'polypeptide_localization_variant' OR cvterm.name = 'polypeptide_loss_of_function_variant' OR cvterm.name = 'polypeptide_post_translational_processing_variant' OR cvterm.name = 'inactive_ligand_binding_site' OR cvterm.name = 'polypeptide_partial_loss_of_function' OR cvterm.name = 'inactive_catalytic_site' OR cvterm.name = 'polypeptide_function_variant';

--- ************************************************
--- *** relation: decreased_translational_product_level ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant which decreases the t ***
--- *** ranslational product level with respect  ***
--- *** to a reference sequence.                 ***
--- ************************************************
---

CREATE VIEW decreased_translational_product_level AS
  SELECT
    feature_id AS decreased_translational_product_level_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'decreased_translational_product_level';

--- ************************************************
--- *** relation: increased_translational_product_level ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant which increases the t ***
--- *** ranslational product level with respect  ***
--- *** to a reference sequence.                 ***
--- ************************************************
---

CREATE VIEW increased_translational_product_level AS
  SELECT
    feature_id AS increased_translational_product_level_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'increased_translational_product_level';

--- ************************************************
--- *** relation: polypeptide_gain_of_function_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant which causes gain of  ***
--- *** polypeptide function with respect to a r ***
--- *** eference sequence.                       ***
--- ************************************************
---

CREATE VIEW polypeptide_gain_of_function_variant AS
  SELECT
    feature_id AS polypeptide_gain_of_function_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_gain_of_function_variant';

--- ************************************************
--- *** relation: polypeptide_localization_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant which changes the loc ***
--- *** alization of a polypeptide with respect  ***
--- *** to a reference sequence.                 ***
--- ************************************************
---

CREATE VIEW polypeptide_localization_variant AS
  SELECT
    feature_id AS polypeptide_localization_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_localization_variant';

--- ************************************************
--- *** relation: polypeptide_loss_of_function_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that causes the loss  ***
--- *** of a polypeptide function with respect t ***
--- *** o a reference sequence.                  ***
--- ************************************************
---

CREATE VIEW polypeptide_loss_of_function_variant AS
  SELECT
    feature_id AS polypeptide_loss_of_function_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inactive_ligand_binding_site' OR cvterm.name = 'polypeptide_partial_loss_of_function' OR cvterm.name = 'inactive_catalytic_site' OR cvterm.name = 'polypeptide_loss_of_function_variant';

--- ************************************************
--- *** relation: inactive_ligand_binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that causes the inact ***
--- *** ivation of a ligand binding site with re ***
--- *** spect to a reference sequence.           ***
--- ************************************************
---

CREATE VIEW inactive_ligand_binding_site AS
  SELECT
    feature_id AS inactive_ligand_binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inactive_catalytic_site' OR cvterm.name = 'inactive_ligand_binding_site';

--- ************************************************
--- *** relation: polypeptide_partial_loss_of_function ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that causes some but  ***
--- *** not all loss of polypeptide function wit ***
--- *** h respect to a reference sequence.       ***
--- ************************************************
---

CREATE VIEW polypeptide_partial_loss_of_function AS
  SELECT
    feature_id AS polypeptide_partial_loss_of_function_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_partial_loss_of_function';

--- ************************************************
--- *** relation: polypeptide_post_translational_processing_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that causes a change  ***
--- *** in post translational processing of the  ***
--- *** peptide with respect to a reference sequ ***
--- *** ence.                                    ***
--- ************************************************
---

CREATE VIEW polypeptide_post_translational_processing_variant AS
  SELECT
    feature_id AS polypeptide_post_translational_processing_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_post_translational_processing_variant';

--- ************************************************
--- *** relation: copy_number_change ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant where copies of a fea ***
--- *** ture (CNV) are either increased or decre ***
--- *** ased.                                    ***
--- ************************************************
---

CREATE VIEW copy_number_change AS
  SELECT
    feature_id AS copy_number_change_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'copy_number_change';

--- ************************************************
--- *** relation: gene_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant where the structure o ***
--- *** f the gene is changed.                   ***
--- ************************************************
---

CREATE VIEW gene_variant AS
  SELECT
    feature_id AS gene_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_fusion' OR cvterm.name = 'splicing_variant' OR cvterm.name = 'transcript_variant' OR cvterm.name = 'translational_product_structure_variant' OR cvterm.name = 'cryptic_splice_site_variant' OR cvterm.name = 'exon_loss' OR cvterm.name = 'intron_gain' OR cvterm.name = 'splice_region_variant' OR cvterm.name = 'cryptic_splice_acceptor' OR cvterm.name = 'cryptic_splice_donor' OR cvterm.name = 'complex_change_in_transcript' OR cvterm.name = 'transcript_secondary_structure_variant' OR cvterm.name = 'nc_transcript_variant' OR cvterm.name = 'NMD_transcript_variant' OR cvterm.name = 'UTR_variant' OR cvterm.name = 'intron_variant' OR cvterm.name = 'exon_variant' OR cvterm.name = 'compensatory_transcript_secondary_structure_variant' OR cvterm.name = 'mature_miRNA_variant' OR cvterm.name = '5_prime_UTR_variant' OR cvterm.name = '3_prime_UTR_variant' OR cvterm.name = 'splice_site_variant' OR cvterm.name = 'splice_acceptor_variant' OR cvterm.name = 'splice_donor_variant' OR cvterm.name = 'splice_donor_5th_base_variant' OR cvterm.name = 'coding_sequence_variant' OR cvterm.name = 'non_coding_exon_variant' OR cvterm.name = 'codon_variant' OR cvterm.name = 'frameshift_variant' OR cvterm.name = 'inframe_variant' OR cvterm.name = 'initiator_codon_change' OR cvterm.name = 'non_synonymous_codon' OR cvterm.name = 'synonymous_codon' OR cvterm.name = 'terminal_codon_variant' OR cvterm.name = 'stop_gained' OR cvterm.name = 'missense_codon' OR cvterm.name = 'conservative_missense_codon' OR cvterm.name = 'non_conservative_missense_codon' OR cvterm.name = 'terminator_codon_variant' OR cvterm.name = 'incomplete_terminal_codon_variant' OR cvterm.name = 'stop_retained_variant' OR cvterm.name = 'stop_lost' OR cvterm.name = 'frame_restoring_variant' OR cvterm.name = 'minus_1_frameshift_variant' OR cvterm.name = 'minus_2_frameshift_variant' OR cvterm.name = 'plus_1_frameshift_variant' OR cvterm.name = 'plus_2_frameshift variant' OR cvterm.name = 'inframe_codon_gain' OR cvterm.name = 'inframe_codon_loss' OR cvterm.name = '3D_polypeptide_structure_variant' OR cvterm.name = 'complex_change_of_translational_product_variant' OR cvterm.name = 'polypeptide_sequence_variant' OR cvterm.name = 'complex_3D_structural_variant' OR cvterm.name = 'conformational_change_variant' OR cvterm.name = 'amino_acid_deletion' OR cvterm.name = 'amino_acid_insertion' OR cvterm.name = 'amino_acid_substitution' OR cvterm.name = 'elongated_polypeptide' OR cvterm.name = 'polypeptide_fusion' OR cvterm.name = 'polypeptide_truncation' OR cvterm.name = 'conservative_amino_acid_substitution' OR cvterm.name = 'non_conservative_amino_acid_substitution' OR cvterm.name = 'elongated_polypeptide_C_terminal' OR cvterm.name = 'elongated_polypeptide_N_terminal' OR cvterm.name = 'elongated_in_frame_polypeptide_C_terminal' OR cvterm.name = 'elongated_out_of_frame_polypeptide_C_terminal' OR cvterm.name = 'elongated_in_frame_polypeptide_N_terminal_elongation' OR cvterm.name = 'elongated_out_of_frame_polypeptide_N_terminal' OR cvterm.name = 'gene_variant';

--- ************************************************
--- *** relation: gene_fusion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant whereby a two genes h ***
--- *** ave become joined.                       ***
--- ************************************************
---

CREATE VIEW gene_fusion AS
  SELECT
    feature_id AS gene_fusion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_fusion';

--- ************************************************
--- *** relation: regulatory_region_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant located within a regu ***
--- *** latory region.                           ***
--- ************************************************
---

CREATE VIEW regulatory_region_variant AS
  SELECT
    feature_id AS regulatory_region_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'TF_binding_site_variant' OR cvterm.name = 'regulatory_region_variant';

--- ************************************************
--- *** relation: stop_retained_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant where at least one ba ***
--- *** se in the terminator codon is changed, b ***
--- *** ut the terminator remains.               ***
--- ************************************************
---

CREATE VIEW stop_retained_variant AS
  SELECT
    feature_id AS stop_retained_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stop_retained_variant';

--- ************************************************
--- *** relation: splicing_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that changes the proc ***
--- *** ess of splicing.                         ***
--- ************************************************
---

CREATE VIEW splicing_variant AS
  SELECT
    feature_id AS splicing_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cryptic_splice_site_variant' OR cvterm.name = 'exon_loss' OR cvterm.name = 'intron_gain' OR cvterm.name = 'splice_region_variant' OR cvterm.name = 'cryptic_splice_acceptor' OR cvterm.name = 'cryptic_splice_donor' OR cvterm.name = 'splicing_variant';

--- ************************************************
--- *** relation: cryptic_splice_site_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant causing a new (functi ***
--- *** onal) splice site.                       ***
--- ************************************************
---

CREATE VIEW cryptic_splice_site_variant AS
  SELECT
    feature_id AS cryptic_splice_site_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cryptic_splice_acceptor' OR cvterm.name = 'cryptic_splice_donor' OR cvterm.name = 'cryptic_splice_site_variant';

--- ************************************************
--- *** relation: cryptic_splice_acceptor ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant whereby a new splice  ***
--- *** site is created due to the activation of ***
--- ***  a new acceptor.                         ***
--- ************************************************
---

CREATE VIEW cryptic_splice_acceptor AS
  SELECT
    feature_id AS cryptic_splice_acceptor_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cryptic_splice_acceptor';

--- ************************************************
--- *** relation: cryptic_splice_donor ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant whereby a new splice  ***
--- *** site is created due to the activation of ***
--- ***  a new donor.                            ***
--- ************************************************
---

CREATE VIEW cryptic_splice_donor AS
  SELECT
    feature_id AS cryptic_splice_donor_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cryptic_splice_donor';

--- ************************************************
--- *** relation: exon_loss ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant whereby an exon is lo ***
--- *** st from the transcript.                  ***
--- ************************************************
---

CREATE VIEW exon_loss AS
  SELECT
    feature_id AS exon_loss_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'exon_loss';

--- ************************************************
--- *** relation: intron_gain ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant whereby an intron is  ***
--- *** gained by the processed transcript; usua ***
--- *** lly a result of an alteration of the don ***
--- *** or or acceptor.                          ***
--- ************************************************
---

CREATE VIEW intron_gain AS
  SELECT
    feature_id AS intron_gain_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'intron_gain';

--- ************************************************
--- *** relation: splice_acceptor_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A splice variant that changes the 2 base ***
--- ***  region at the 3' end of an intron.      ***
--- ************************************************
---

CREATE VIEW splice_acceptor_variant AS
  SELECT
    feature_id AS splice_acceptor_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'splice_acceptor_variant';

--- ************************************************
--- *** relation: splice_donor_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A splice variant that changes the2 base  ***
--- *** region at the 5' end of an intron.       ***
--- ************************************************
---

CREATE VIEW splice_donor_variant AS
  SELECT
    feature_id AS splice_donor_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'splice_donor_variant';

--- ************************************************
--- *** relation: transcript_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that changes the stru ***
--- *** cture of the transcript.                 ***
--- ************************************************
---

CREATE VIEW transcript_variant AS
  SELECT
    feature_id AS transcript_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'complex_change_in_transcript' OR cvterm.name = 'transcript_secondary_structure_variant' OR cvterm.name = 'nc_transcript_variant' OR cvterm.name = 'NMD_transcript_variant' OR cvterm.name = 'UTR_variant' OR cvterm.name = 'intron_variant' OR cvterm.name = 'exon_variant' OR cvterm.name = 'compensatory_transcript_secondary_structure_variant' OR cvterm.name = 'mature_miRNA_variant' OR cvterm.name = '5_prime_UTR_variant' OR cvterm.name = '3_prime_UTR_variant' OR cvterm.name = 'splice_site_variant' OR cvterm.name = 'splice_acceptor_variant' OR cvterm.name = 'splice_donor_variant' OR cvterm.name = 'splice_donor_5th_base_variant' OR cvterm.name = 'coding_sequence_variant' OR cvterm.name = 'non_coding_exon_variant' OR cvterm.name = 'codon_variant' OR cvterm.name = 'frameshift_variant' OR cvterm.name = 'inframe_variant' OR cvterm.name = 'initiator_codon_change' OR cvterm.name = 'non_synonymous_codon' OR cvterm.name = 'synonymous_codon' OR cvterm.name = 'terminal_codon_variant' OR cvterm.name = 'stop_gained' OR cvterm.name = 'missense_codon' OR cvterm.name = 'conservative_missense_codon' OR cvterm.name = 'non_conservative_missense_codon' OR cvterm.name = 'terminator_codon_variant' OR cvterm.name = 'incomplete_terminal_codon_variant' OR cvterm.name = 'stop_retained_variant' OR cvterm.name = 'stop_lost' OR cvterm.name = 'frame_restoring_variant' OR cvterm.name = 'minus_1_frameshift_variant' OR cvterm.name = 'minus_2_frameshift_variant' OR cvterm.name = 'plus_1_frameshift_variant' OR cvterm.name = 'plus_2_frameshift variant' OR cvterm.name = 'inframe_codon_gain' OR cvterm.name = 'inframe_codon_loss' OR cvterm.name = 'transcript_variant';

--- ************************************************
--- *** relation: complex_change_in_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript variant with a complex INDE ***
--- *** L- Insertion or deletion that spans an e ***
--- *** xon/intron border or a coding sequence/U ***
--- *** TR border.                               ***
--- ************************************************
---

CREATE VIEW complex_change_in_transcript AS
  SELECT
    feature_id AS complex_change_in_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'complex_change_in_transcript';

--- ************************************************
--- *** relation: stop_lost ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant where at least one ba ***
--- *** se of the terminator codon (stop) is cha ***
--- *** nged, resulting in an elongated transcri ***
--- *** pt.                                      ***
--- ************************************************
---

CREATE VIEW stop_lost AS
  SELECT
    feature_id AS stop_lost_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stop_lost';

--- ************************************************
--- *** relation: coding_sequence_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that changes the codi ***
--- *** ng sequence.                             ***
--- ************************************************
---

CREATE VIEW coding_sequence_variant AS
  SELECT
    feature_id AS coding_sequence_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'codon_variant' OR cvterm.name = 'frameshift_variant' OR cvterm.name = 'inframe_variant' OR cvterm.name = 'initiator_codon_change' OR cvterm.name = 'non_synonymous_codon' OR cvterm.name = 'synonymous_codon' OR cvterm.name = 'terminal_codon_variant' OR cvterm.name = 'stop_gained' OR cvterm.name = 'missense_codon' OR cvterm.name = 'conservative_missense_codon' OR cvterm.name = 'non_conservative_missense_codon' OR cvterm.name = 'terminator_codon_variant' OR cvterm.name = 'incomplete_terminal_codon_variant' OR cvterm.name = 'stop_retained_variant' OR cvterm.name = 'stop_lost' OR cvterm.name = 'frame_restoring_variant' OR cvterm.name = 'minus_1_frameshift_variant' OR cvterm.name = 'minus_2_frameshift_variant' OR cvterm.name = 'plus_1_frameshift_variant' OR cvterm.name = 'plus_2_frameshift variant' OR cvterm.name = 'inframe_codon_gain' OR cvterm.name = 'inframe_codon_loss' OR cvterm.name = 'coding_sequence_variant';

--- ************************************************
--- *** relation: codon_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that changes at least ***
--- ***  one base in a codon.                    ***
--- ************************************************
---

CREATE VIEW codon_variant AS
  SELECT
    feature_id AS codon_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'initiator_codon_change' OR cvterm.name = 'non_synonymous_codon' OR cvterm.name = 'synonymous_codon' OR cvterm.name = 'terminal_codon_variant' OR cvterm.name = 'stop_gained' OR cvterm.name = 'missense_codon' OR cvterm.name = 'conservative_missense_codon' OR cvterm.name = 'non_conservative_missense_codon' OR cvterm.name = 'terminator_codon_variant' OR cvterm.name = 'incomplete_terminal_codon_variant' OR cvterm.name = 'stop_retained_variant' OR cvterm.name = 'stop_lost' OR cvterm.name = 'codon_variant';

--- ************************************************
--- *** relation: initiator_codon_change ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A codon variant that changes at least on ***
--- *** e base of the first codon of a transcrip ***
--- *** t.                                       ***
--- ************************************************
---

CREATE VIEW initiator_codon_change AS
  SELECT
    feature_id AS initiator_codon_change_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'initiator_codon_change';

--- ************************************************
--- *** relation: non_synonymous_codon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant whereby at least one  ***
--- *** base of a codon is changed resulting in  ***
--- *** a codon that encodes for a different ami ***
--- *** no acid or stop codon.                   ***
--- ************************************************
---

CREATE VIEW non_synonymous_codon AS
  SELECT
    feature_id AS non_synonymous_codon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stop_gained' OR cvterm.name = 'missense_codon' OR cvterm.name = 'conservative_missense_codon' OR cvterm.name = 'non_conservative_missense_codon' OR cvterm.name = 'non_synonymous_codon';

--- ************************************************
--- *** relation: conservative_missense_codon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant whereby at least one  ***
--- *** base of a codon is changed resulting in  ***
--- *** a codon that encodes for a different but ***
--- ***  similar amino acid. These variants may  ***
--- *** or may not be deleterious.               ***
--- ************************************************
---

CREATE VIEW conservative_missense_codon AS
  SELECT
    feature_id AS conservative_missense_codon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'conservative_missense_codon';

--- ************************************************
--- *** relation: non_conservative_missense_codon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant whereby at least one  ***
--- *** base of a codon is changed resulting in  ***
--- *** a codon that encodes for an amino acid w ***
--- *** ith different biochemical properties.    ***
--- ************************************************
---

CREATE VIEW non_conservative_missense_codon AS
  SELECT
    feature_id AS non_conservative_missense_codon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'non_conservative_missense_codon';

--- ************************************************
--- *** relation: stop_gained ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant whereby at least one  ***
--- *** base of a codon is changed, resulting in ***
--- ***  a premature stop codon, leading to a sh ***
--- *** ortened transcript.                      ***
--- ************************************************
---

CREATE VIEW stop_gained AS
  SELECT
    feature_id AS stop_gained_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stop_gained';

--- ************************************************
--- *** relation: synonymous_codon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant whereby a base of a c ***
--- *** odon is changed, but there is no resulti ***
--- *** ng change to the encoded amino acid.     ***
--- ************************************************
---

CREATE VIEW synonymous_codon AS
  SELECT
    feature_id AS synonymous_codon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'synonymous_codon';

--- ************************************************
--- *** relation: frameshift_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant which causes a disrup ***
--- *** tion of the translational reading frame, ***
--- ***  because the number of nucleotides inser ***
--- *** ted or deleted is not a multiple of thre ***
--- *** e.                                       ***
--- ************************************************
---

CREATE VIEW frameshift_variant AS
  SELECT
    feature_id AS frameshift_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'frame_restoring_variant' OR cvterm.name = 'minus_1_frameshift_variant' OR cvterm.name = 'minus_2_frameshift_variant' OR cvterm.name = 'plus_1_frameshift_variant' OR cvterm.name = 'plus_2_frameshift variant' OR cvterm.name = 'frameshift_variant';

--- ************************************************
--- *** relation: terminator_codon_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant whereby at least one  ***
--- *** of the bases in the terminator codon is  ***
--- *** changed.                                 ***
--- ************************************************
---

CREATE VIEW terminator_codon_variant AS
  SELECT
    feature_id AS terminator_codon_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'stop_retained_variant' OR cvterm.name = 'stop_lost' OR cvterm.name = 'terminator_codon_variant';

--- ************************************************
--- *** relation: frame_restoring_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that reverts the sequ ***
--- *** ence of a previous frameshift mutation b ***
--- *** ack to the initial frame.                ***
--- ************************************************
---

CREATE VIEW frame_restoring_variant AS
  SELECT
    feature_id AS frame_restoring_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'frame_restoring_variant';

--- ************************************************
--- *** relation: minus_1_frameshift_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant which causes a disrup ***
--- *** tion of the translational reading frame, ***
--- ***  by shifting one base ahead.             ***
--- ************************************************
---

CREATE VIEW minus_1_frameshift_variant AS
  SELECT
    feature_id AS minus_1_frameshift_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minus_1_frameshift_variant';

--- ************************************************
--- *** relation: minus_2_frameshift_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW minus_2_frameshift_variant AS
  SELECT
    feature_id AS minus_2_frameshift_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minus_2_frameshift_variant';

--- ************************************************
--- *** relation: plus_1_frameshift_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant which causes a disrup ***
--- *** tion of the translational reading frame, ***
--- ***  by shifting one base backward.          ***
--- ************************************************
---

CREATE VIEW plus_1_frameshift_variant AS
  SELECT
    feature_id AS plus_1_frameshift_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'plus_1_frameshift_variant';

--- ************************************************
--- *** relation: plus_2_frameshift_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW plus_2_frameshift_variant AS
  SELECT
    feature_id AS plus_2_frameshift_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'plus_2_frameshift variant';

--- ************************************************
--- *** relation: transcript_secondary_structure_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant within a transcript t ***
--- *** hat changes the secondary structure of t ***
--- *** he RNA product.                          ***
--- ************************************************
---

CREATE VIEW transcript_secondary_structure_variant AS
  SELECT
    feature_id AS transcript_secondary_structure_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'compensatory_transcript_secondary_structure_variant' OR cvterm.name = 'transcript_secondary_structure_variant';

--- ************************************************
--- *** relation: compensatory_transcript_secondary_structure_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A secondary structure variant that compe ***
--- *** nsate for the change made by a previous  ***
--- *** variant.                                 ***
--- ************************************************
---

CREATE VIEW compensatory_transcript_secondary_structure_variant AS
  SELECT
    feature_id AS compensatory_transcript_secondary_structure_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'compensatory_transcript_secondary_structure_variant';

--- ************************************************
--- *** relation: translational_product_structure_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant within the transcript ***
--- ***  that changes the structure of the trans ***
--- *** lational product.                        ***
--- ************************************************
---

CREATE VIEW translational_product_structure_variant AS
  SELECT
    feature_id AS translational_product_structure_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = '3D_polypeptide_structure_variant' OR cvterm.name = 'complex_change_of_translational_product_variant' OR cvterm.name = 'polypeptide_sequence_variant' OR cvterm.name = 'complex_3D_structural_variant' OR cvterm.name = 'conformational_change_variant' OR cvterm.name = 'amino_acid_deletion' OR cvterm.name = 'amino_acid_insertion' OR cvterm.name = 'amino_acid_substitution' OR cvterm.name = 'elongated_polypeptide' OR cvterm.name = 'polypeptide_fusion' OR cvterm.name = 'polypeptide_truncation' OR cvterm.name = 'conservative_amino_acid_substitution' OR cvterm.name = 'non_conservative_amino_acid_substitution' OR cvterm.name = 'elongated_polypeptide_C_terminal' OR cvterm.name = 'elongated_polypeptide_N_terminal' OR cvterm.name = 'elongated_in_frame_polypeptide_C_terminal' OR cvterm.name = 'elongated_out_of_frame_polypeptide_C_terminal' OR cvterm.name = 'elongated_in_frame_polypeptide_N_terminal_elongation' OR cvterm.name = 'elongated_out_of_frame_polypeptide_N_terminal' OR cvterm.name = 'translational_product_structure_variant';

--- ************************************************
--- *** relation: threed_polypeptide_structure_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that changes the resu ***
--- *** lting polypeptide structure.             ***
--- ************************************************
---

CREATE VIEW threed_polypeptide_structure_variant AS
  SELECT
    feature_id AS threed_polypeptide_structure_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'complex_3D_structural_variant' OR cvterm.name = 'conformational_change_variant' OR cvterm.name = '3D_polypeptide_structure_variant';

--- ************************************************
--- *** relation: complex_3d_structural_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that changes the resu ***
--- *** lting polypeptide structure.             ***
--- ************************************************
---

CREATE VIEW complex_3d_structural_variant AS
  SELECT
    feature_id AS complex_3d_structural_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'complex_3D_structural_variant';

--- ************************************************
--- *** relation: conformational_change_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant in the CDS region tha ***
--- *** t causes a conformational change in the  ***
--- *** resulting polypeptide sequence.          ***
--- ************************************************
---

CREATE VIEW conformational_change_variant AS
  SELECT
    feature_id AS conformational_change_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'conformational_change_variant';

--- ************************************************
--- *** relation: complex_change_of_translational_product_variant ***
--- *** relation type: VIEW                      ***

SET search_path=so,chado,pg_catalog;
--- *** relation: catmat_right_handed_four ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of 4 consecutive residues with d ***
--- *** ihedral angles as follows: res i: phi -9 ***
--- *** 0 bounds -120 to -60, res i: psi -10 bou ***
--- *** nds -50 to 30, res i+1: phi -90 bounds - ***
--- *** 120 to -60, res i+1: psi -10 bounds -50  ***
--- *** to 30, res i+2: phi -75 bounds -100 to - ***
--- *** 50, res i+2: psi 140 bounds 110 to 170.  ***
--- *** The extra restriction of the length of t ***
--- *** he O to O distance is similar, that it b ***
--- *** e less than 5 Angstrom. In this case the ***
--- *** se two Oxygen atoms are the main chain c ***
--- *** arbonyl oxygen atoms of residues i-1 and ***
--- ***  i+2.                                    ***
--- ************************************************
---

CREATE VIEW catmat_right_handed_four AS
  SELECT
    feature_id AS catmat_right_handed_four_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'catmat_right_handed_four';

--- ************************************************
--- *** relation: alpha_beta_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of five consecutive residues and ***
--- ***  two H-bonds in which: H-bond between CO ***
--- ***  of residue(i) and NH of residue(i+4), H ***
--- *** -bond between CO of residue(i) and NH of ***
--- ***  residue(i+3),Phi angles of residues(i+1 ***
--- *** ), (i+2) and (i+3) are negative.         ***
--- ************************************************
---

CREATE VIEW alpha_beta_motif AS
  SELECT
    feature_id AS alpha_beta_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'alpha_beta_motif';

--- ************************************************
--- *** relation: lipoprotein_signal_peptide ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A peptide that acts as a signal for both ***
--- ***  membrane translocation and lipid attach ***
--- *** ment in prokaryotes.                     ***
--- ************************************************
---

CREATE VIEW lipoprotein_signal_peptide AS
  SELECT
    feature_id AS lipoprotein_signal_peptide_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'lipoprotein_signal_peptide';

--- ************************************************
--- *** relation: no_output ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An experimental region wherean analysis  ***
--- *** has been run and not produced any annota ***
--- *** tion.                                    ***
--- ************************************************
---

CREATE VIEW no_output AS
  SELECT
    feature_id AS no_output_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'no_output';

--- ************************************************
--- *** relation: cleaved_peptide_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The cleaved_peptide_regon is the a regio ***
--- *** n of peptide sequence that is cleaved du ***
--- *** ring maturation.                         ***
--- ************************************************
---

CREATE VIEW cleaved_peptide_region AS
  SELECT
    feature_id AS cleaved_peptide_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cleaved_initiator_methionine' OR cvterm.name = 'intein' OR cvterm.name = 'propeptide_cleavage_site' OR cvterm.name = 'propeptide' OR cvterm.name = 'cleaved_for_gpi_anchor_region' OR cvterm.name = 'lipoprotein_signal_peptide' OR cvterm.name = 'n_terminal_region' OR cvterm.name = 'c_terminal_region' OR cvterm.name = 'central_hydrophobic_region_of_signal_peptide' OR cvterm.name = 'cleaved_peptide_region';

--- ************************************************
--- *** relation: peptide_coil ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Irregular, unstructured regions of a pro ***
--- *** tein's backbone, as distinct from the re ***
--- *** gular region (namely alpha helix and bet ***
--- *** a strand - characterised by specific pat ***
--- *** terns of main-chain hydrogen bonds).     ***
--- ************************************************
---

CREATE VIEW peptide_coil AS
  SELECT
    feature_id AS peptide_coil_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'peptide_coil';

--- ************************************************
--- *** relation: hydrophobic_region_of_peptide ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Hydrophobic regions are regions with a l ***
--- *** ow affinity for water.                   ***
--- ************************************************
---

CREATE VIEW hydrophobic_region_of_peptide AS
  SELECT
    feature_id AS hydrophobic_region_of_peptide_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'hydrophobic_region_of_peptide';

--- ************************************************
--- *** relation: n_terminal_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The amino-terminal positively-charged re ***
--- *** gion of a signal peptide (approx 1-5 aa) ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW n_terminal_region AS
  SELECT
    feature_id AS n_terminal_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'n_terminal_region';

--- ************************************************
--- *** relation: c_terminal_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The more polar, carboxy-terminal region  ***
--- *** of the signal peptide (approx 3-7 aa).   ***
--- ************************************************
---

CREATE VIEW c_terminal_region AS
  SELECT
    feature_id AS c_terminal_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'c_terminal_region';

--- ************************************************
--- *** relation: central_hydrophobic_region_of_signal_peptide ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The central, hydrophobic region of the s ***
--- *** ignal peptide (approx 7-15 aa).          ***
--- ************************************************
---

CREATE VIEW central_hydrophobic_region_of_signal_peptide AS
  SELECT
    feature_id AS central_hydrophobic_region_of_signal_peptide_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'central_hydrophobic_region_of_signal_peptide';

--- ************************************************
--- *** relation: polypeptide_conserved_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A conserved motif is a short (up to 20 a ***
--- *** mino acids) region of biological interes ***
--- *** t that is conserved in different protein ***
--- *** s. They may or may not have functional o ***
--- *** r structural significance within the pro ***
--- *** teins in which they are found.           ***
--- ************************************************
---

CREATE VIEW polypeptide_conserved_motif AS
  SELECT
    feature_id AS polypeptide_conserved_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_conserved_motif';

--- ************************************************
--- *** relation: polypeptide_binding_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A polypeptide binding motif is a short ( ***
--- *** up to 20 amino acids) polypeptide region ***
--- ***  of biological interest that contains on ***
--- *** e or more amino acids experimentally sho ***
--- *** wn to bind to a ligand.                  ***
--- ************************************************
---

CREATE VIEW polypeptide_binding_motif AS
  SELECT
    feature_id AS polypeptide_binding_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_binding_motif';

--- ************************************************
--- *** relation: polypeptide_catalytic_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A polypeptide catalytic motif is a short ***
--- ***  (up to 20 amino acids) polypeptide regi ***
--- *** on that contains one or more active site ***
--- ***  residues.                               ***
--- ************************************************
---

CREATE VIEW polypeptide_catalytic_motif AS
  SELECT
    feature_id AS polypeptide_catalytic_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_catalytic_motif';

--- ************************************************
--- *** relation: polypeptide_dna_contact ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the polypeptide  ***
--- *** molecule, interacts selectively and non- ***
--- *** covalently with DNA.                     ***
--- ************************************************
---

CREATE VIEW polypeptide_dna_contact AS
  SELECT
    feature_id AS polypeptide_dna_contact_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_DNA_contact';

--- ************************************************
--- *** relation: polypeptide_conserved_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A subsection of sequence with biological ***
--- ***  interest that is conserved in different ***
--- ***  proteins. They may or may not have func ***
--- *** tional or structural significance within ***
--- ***  the proteins in which they are found.   ***
--- ************************************************
---

CREATE VIEW polypeptide_conserved_region AS
  SELECT
    feature_id AS polypeptide_conserved_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_domain' OR cvterm.name = 'polypeptide_motif' OR cvterm.name = 'polypeptide_repeat' OR cvterm.name = 'biochemical_region_of_peptide' OR cvterm.name = 'polypeptide_conserved_motif' OR cvterm.name = 'post_translationally_modified_region' OR cvterm.name = 'conformational_switch' OR cvterm.name = 'molecular_contact_region' OR cvterm.name = 'polypeptide_binding_motif' OR cvterm.name = 'polypeptide_catalytic_motif' OR cvterm.name = 'histone_modification' OR cvterm.name = 'histone_methylation_site' OR cvterm.name = 'histone_acetylation_site' OR cvterm.name = 'histone_ubiqitination_site' OR cvterm.name = 'histone_acylation_region' OR cvterm.name = 'H4K20_monomethylation_site' OR cvterm.name = 'H2BK5_monomethylation_site' OR cvterm.name = 'H3K27_methylation_site' OR cvterm.name = 'H3K36_methylation_site' OR cvterm.name = 'H3K4_methylation_site' OR cvterm.name = 'H3K79_methylation_site' OR cvterm.name = 'H3K9_methylation_site' OR cvterm.name = 'H3K27_monomethylation_site' OR cvterm.name = 'H3K27_trimethylation_site' OR cvterm.name = 'H3K27_dimethylation_site' OR cvterm.name = 'H3K36_monomethylation_site' OR cvterm.name = 'H3K36_dimethylation_site' OR cvterm.name = 'H3K36_trimethylation_site' OR cvterm.name = 'H3K4_monomethylation_site' OR cvterm.name = 'H3K4_trimethylation' OR cvterm.name = 'H3K4_dimethylation_site' OR cvterm.name = 'H3K79_monomethylation_site' OR cvterm.name = 'H3K79_dimethylation_site' OR cvterm.name = 'H3K79_trimethylation_site' OR cvterm.name = 'H3K9_trimethylation_site' OR cvterm.name = 'H3K9_monomethylation_site' OR cvterm.name = 'H3K9_dimethylation_site' OR cvterm.name = 'H3K9_acetylation_site' OR cvterm.name = 'H3K14_acetylation_site' OR cvterm.name = 'H3K18_acetylation_site' OR cvterm.name = 'H3K23_acylation site' OR cvterm.name = 'H3K27_acylation_site' OR cvterm.name = 'H4K16_acylation_site' OR cvterm.name = 'H4K5_acylation_site' OR cvterm.name = 'H4K8_acylation site' OR cvterm.name = 'H2B_ubiquitination_site' OR cvterm.name = 'H4K_acylation_region' OR cvterm.name = 'polypeptide_metal_contact' OR cvterm.name = 'protein_protein_contact' OR cvterm.name = 'polypeptide_ligand_contact' OR cvterm.name = 'polypeptide_DNA_contact' OR cvterm.name = 'polypeptide_calcium_ion_contact_site' OR cvterm.name = 'polypeptide_cobalt_ion_contact_site' OR cvterm.name = 'polypeptide_copper_ion_contact_site' OR cvterm.name = 'polypeptide_iron_ion_contact_site' OR cvterm.name = 'polypeptide_magnesium_ion_contact_site' OR cvterm.name = 'polypeptide_manganese_ion_contact_site' OR cvterm.name = 'polypeptide_molybdenum_ion_contact_site' OR cvterm.name = 'polypeptide_nickel_ion_contact_site' OR cvterm.name = 'polypeptide_tungsten_ion_contact_site' OR cvterm.name = 'polypeptide_zinc_ion_contact_site' OR cvterm.name = 'polypeptide_conserved_region';

--- ************************************************
--- *** relation: substitution ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence alteration where the length o ***
--- *** f the change in the variant is the same  ***
--- *** as that of the reference.                ***
--- ************************************************
---

CREATE VIEW substitution AS
  SELECT
    feature_id AS substitution_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'sequence_length_variation' OR cvterm.name = 'MNP' OR cvterm.name = 'SNV' OR cvterm.name = 'complex_substitution' OR cvterm.name = 'simple_sequence_length_variation' OR cvterm.name = 'SNP' OR cvterm.name = 'point_mutation' OR cvterm.name = 'transition' OR cvterm.name = 'transversion' OR cvterm.name = 'pyrimidine_transition' OR cvterm.name = 'purine_transition' OR cvterm.name = 'C_to_T_transition' OR cvterm.name = 'T_to_C_transition' OR cvterm.name = 'C_to_T_transition_at_pCpG_site' OR cvterm.name = 'A_to_G_transition' OR cvterm.name = 'G_to_A_transition' OR cvterm.name = 'pyrimidine_to_purine_transversion' OR cvterm.name = 'purine_to_pyrimidine_transversion' OR cvterm.name = 'C_to_A_transversion' OR cvterm.name = 'C_to_G_transversion' OR cvterm.name = 'T_to_A_transversion' OR cvterm.name = 'T_to_G_transversion' OR cvterm.name = 'A_to_C_transversion' OR cvterm.name = 'A_to_T_transversion' OR cvterm.name = 'G_to_C_transversion' OR cvterm.name = 'G_to_T_transversion' OR cvterm.name = 'substitution';

--- ************************************************
--- *** relation: complex_substitution ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** When no simple or well defined DNA mutat ***
--- *** ion event describes the observed DNA cha ***
--- *** nge, the keyword "complex" should be use ***
--- *** d. Usually there are multiple equally pl ***
--- *** ausible explanations for the change.     ***
--- ************************************************
---

CREATE VIEW complex_substitution AS
  SELECT
    feature_id AS complex_substitution_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'complex_substitution';

--- ************************************************
--- *** relation: point_mutation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A single nucleotide change which has occ ***
--- *** urred at the same position of a correspo ***
--- *** nding nucleotide in a reference sequence ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW point_mutation AS
  SELECT
    feature_id AS point_mutation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'point_mutation';

--- ************************************************
--- *** relation: transition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Change of a pyrimidine nucleotide, C or  ***
--- *** T, into an other pyrimidine nucleotide,  ***
--- *** or change of a purine nucleotide, A or G ***
--- *** , into an other purine nucleotide.       ***
--- ************************************************
---

CREATE VIEW transition AS
  SELECT
    feature_id AS transition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pyrimidine_transition' OR cvterm.name = 'purine_transition' OR cvterm.name = 'C_to_T_transition' OR cvterm.name = 'T_to_C_transition' OR cvterm.name = 'C_to_T_transition_at_pCpG_site' OR cvterm.name = 'A_to_G_transition' OR cvterm.name = 'G_to_A_transition' OR cvterm.name = 'transition';

--- ************************************************
--- *** relation: pyrimidine_transition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A substitution of a pyrimidine, C or T,  ***
--- *** for another pyrimidine.                  ***
--- ************************************************
---

CREATE VIEW pyrimidine_transition AS
  SELECT
    feature_id AS pyrimidine_transition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'C_to_T_transition' OR cvterm.name = 'T_to_C_transition' OR cvterm.name = 'C_to_T_transition_at_pCpG_site' OR cvterm.name = 'pyrimidine_transition';

--- ************************************************
--- *** relation: c_to_t_transition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transition of a cytidine to a thymine. ***
--- ************************************************
---

CREATE VIEW c_to_t_transition AS
  SELECT
    feature_id AS c_to_t_transition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'C_to_T_transition_at_pCpG_site' OR cvterm.name = 'C_to_T_transition';

--- ************************************************
--- *** relation: c_to_t_transition_at_pcpg_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The transition of cytidine to thymine oc ***
--- *** curring at a pCpG site as a consequence  ***
--- *** of the spontaneous deamination of 5'-met ***
--- *** hylcytidine.                             ***
--- ************************************************
---

CREATE VIEW c_to_t_transition_at_pcpg_site AS
  SELECT
    feature_id AS c_to_t_transition_at_pcpg_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'C_to_T_transition_at_pCpG_site';

--- ************************************************
--- *** relation: t_to_c_transition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW t_to_c_transition AS
  SELECT
    feature_id AS t_to_c_transition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'T_to_C_transition';

--- ************************************************
--- *** relation: purine_transition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A substitution of a purine, A or G, for  ***
--- *** another purine.                          ***
--- ************************************************
---

CREATE VIEW purine_transition AS
  SELECT
    feature_id AS purine_transition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'A_to_G_transition' OR cvterm.name = 'G_to_A_transition' OR cvterm.name = 'purine_transition';

--- ************************************************
--- *** relation: a_to_g_transition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transition of an adenine to a guanine. ***
--- ************************************************
---

CREATE VIEW a_to_g_transition AS
  SELECT
    feature_id AS a_to_g_transition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'A_to_G_transition';

--- ************************************************
--- *** relation: g_to_a_transition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transition of a guanine to an adenine. ***
--- ************************************************
---

CREATE VIEW g_to_a_transition AS
  SELECT
    feature_id AS g_to_a_transition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'G_to_A_transition';

--- ************************************************
--- *** relation: transversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Change of a pyrimidine nucleotide, C or  ***
--- *** T, into a purine nucleotide, A or G, or  ***
--- *** vice versa.                              ***
--- ************************************************
---

CREATE VIEW transversion AS
  SELECT
    feature_id AS transversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pyrimidine_to_purine_transversion' OR cvterm.name = 'purine_to_pyrimidine_transversion' OR cvterm.name = 'C_to_A_transversion' OR cvterm.name = 'C_to_G_transversion' OR cvterm.name = 'T_to_A_transversion' OR cvterm.name = 'T_to_G_transversion' OR cvterm.name = 'A_to_C_transversion' OR cvterm.name = 'A_to_T_transversion' OR cvterm.name = 'G_to_C_transversion' OR cvterm.name = 'G_to_T_transversion' OR cvterm.name = 'transversion';

--- ************************************************
--- *** relation: pyrimidine_to_purine_transversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Change of a pyrimidine nucleotide, C or  ***
--- *** T, into a purine nucleotide, A or G.     ***
--- ************************************************
---

CREATE VIEW pyrimidine_to_purine_transversion AS
  SELECT
    feature_id AS pyrimidine_to_purine_transversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'C_to_A_transversion' OR cvterm.name = 'C_to_G_transversion' OR cvterm.name = 'T_to_A_transversion' OR cvterm.name = 'T_to_G_transversion' OR cvterm.name = 'pyrimidine_to_purine_transversion';

--- ************************************************
--- *** relation: c_to_a_transversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transversion from cytidine to adenine. ***
--- ************************************************
---

CREATE VIEW c_to_a_transversion AS
  SELECT
    feature_id AS c_to_a_transversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'C_to_A_transversion';

--- ************************************************
--- *** relation: c_to_g_transversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW c_to_g_transversion AS
  SELECT
    feature_id AS c_to_g_transversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'C_to_G_transversion';

--- ************************************************
--- *** relation: t_to_a_transversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transversion from T to A.              ***
--- ************************************************
---

CREATE VIEW t_to_a_transversion AS
  SELECT
    feature_id AS t_to_a_transversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'T_to_A_transversion';

--- ************************************************
--- *** relation: t_to_g_transversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transversion from T to G.              ***
--- ************************************************
---

CREATE VIEW t_to_g_transversion AS
  SELECT
    feature_id AS t_to_g_transversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'T_to_G_transversion';

--- ************************************************
--- *** relation: purine_to_pyrimidine_transversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Change of a purine nucleotide, A or G ,  ***
--- *** into a pyrimidine nucleotide C or T.     ***
--- ************************************************
---

CREATE VIEW purine_to_pyrimidine_transversion AS
  SELECT
    feature_id AS purine_to_pyrimidine_transversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'A_to_C_transversion' OR cvterm.name = 'A_to_T_transversion' OR cvterm.name = 'G_to_C_transversion' OR cvterm.name = 'G_to_T_transversion' OR cvterm.name = 'purine_to_pyrimidine_transversion';

--- ************************************************
--- *** relation: a_to_c_transversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transversion from adenine to cytidine. ***
--- ************************************************
---

CREATE VIEW a_to_c_transversion AS
  SELECT
    feature_id AS a_to_c_transversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'A_to_C_transversion';

--- ************************************************
--- *** relation: a_to_t_transversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transversion from adenine to thymine.  ***
--- ************************************************
---

CREATE VIEW a_to_t_transversion AS
  SELECT
    feature_id AS a_to_t_transversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'A_to_T_transversion';

--- ************************************************
--- *** relation: g_to_c_transversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transversion from guanine to cytidine. ***
--- ************************************************
---

CREATE VIEW g_to_c_transversion AS
  SELECT
    feature_id AS g_to_c_transversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'G_to_C_transversion';

--- ************************************************
--- *** relation: g_to_t_transversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transversion from guanine to thymine.  ***
--- ************************************************
---

CREATE VIEW g_to_t_transversion AS
  SELECT
    feature_id AS g_to_t_transversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'G_to_T_transversion';

--- ************************************************
--- *** relation: intrachromosomal_mutation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosomal structure variation within ***
--- ***  a single chromosome.                    ***
--- ************************************************
---

CREATE VIEW intrachromosomal_mutation AS
  SELECT
    feature_id AS intrachromosomal_mutation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chromosomal_deletion' OR cvterm.name = 'chromosomal_inversion' OR cvterm.name = 'intrachromosomal_duplication' OR cvterm.name = 'ring_chromosome' OR cvterm.name = 'chromosome_fission' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inversion_derived_bipartite_deficiency' OR cvterm.name = 'inversion_derived_deficiency_plus_duplication' OR cvterm.name = 'inversion_derived_deficiency_plus_aneuploid' OR cvterm.name = 'deficient_translocation' OR cvterm.name = 'deficient_inversion' OR cvterm.name = 'inverted_ring_chromosome' OR cvterm.name = 'pericentric_inversion' OR cvterm.name = 'paracentric_inversion' OR cvterm.name = 'inversion_cum_translocation' OR cvterm.name = 'bipartite_inversion' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'deficient_inversion' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'inversion_derived_deficiency_plus_duplication' OR cvterm.name = 'inversion_derived_bipartite_duplication' OR cvterm.name = 'inversion_derived_duplication_plus_aneuploid' OR cvterm.name = 'intrachromosomal_transposition' OR cvterm.name = 'bipartite_duplication' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'uninverted_intrachromosomal_transposition' OR cvterm.name = 'unoriented_intrachromosomal_transposition' OR cvterm.name = 'inverted_ring_chromosome' OR cvterm.name = 'free_ring_duplication' OR cvterm.name = 'intrachromosomal_mutation';

--- ************************************************
--- *** relation: chromosomal_deletion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An incomplete chromosome.                ***
--- ************************************************
---

CREATE VIEW chromosomal_deletion AS
  SELECT
    feature_id AS chromosomal_deletion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inversion_derived_bipartite_deficiency' OR cvterm.name = 'inversion_derived_deficiency_plus_duplication' OR cvterm.name = 'inversion_derived_deficiency_plus_aneuploid' OR cvterm.name = 'deficient_translocation' OR cvterm.name = 'deficient_inversion' OR cvterm.name = 'chromosomal_deletion';

--- ************************************************
--- *** relation: chromosomal_inversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An interchromosomal mutation where a reg ***
--- *** ion of the chromosome is inverted with r ***
--- *** espect to wild type.                     ***
--- ************************************************
---

CREATE VIEW chromosomal_inversion AS
  SELECT
    feature_id AS chromosomal_inversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inverted_ring_chromosome' OR cvterm.name = 'pericentric_inversion' OR cvterm.name = 'paracentric_inversion' OR cvterm.name = 'inversion_cum_translocation' OR cvterm.name = 'bipartite_inversion' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'deficient_inversion' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'chromosomal_inversion';

--- ************************************************
--- *** relation: interchromosomal_mutation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosomal structure variation whereb ***
--- *** y more than one chromosome is involved.  ***
--- ************************************************
---

CREATE VIEW interchromosomal_mutation AS
  SELECT
    feature_id AS interchromosomal_mutation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chromosomal_translocation' OR cvterm.name = 'bipartite_duplication' OR cvterm.name = 'interchromosomal_transposition' OR cvterm.name = 'translocation_element' OR cvterm.name = 'Robertsonian_fusion' OR cvterm.name = 'reciprocal_chromosomal_translocation' OR cvterm.name = 'deficient_translocation' OR cvterm.name = 'inversion_cum_translocation' OR cvterm.name = 'cyclic_translocation' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'deficient_interchromosomal_transposition' OR cvterm.name = 'inverted_interchromosomal_transposition' OR cvterm.name = 'uninverted_interchromosomal_transposition' OR cvterm.name = 'unoriented_interchromosomal_transposition' OR cvterm.name = 'interchromosomal_mutation';

--- ************************************************
--- *** relation: indel ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence alteration which included an  ***
--- *** insertion and a deletion, affecting 2 or ***
--- ***  more bases.                             ***
--- ************************************************
---

CREATE VIEW indel AS
  SELECT
    feature_id AS indel_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'indel';

--- ************************************************
--- *** relation: duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** One or more nucleotides are added betwee ***
--- *** n two adjacent nucleotides in the sequen ***
--- *** ce; the inserted sequence derives from,  ***
--- *** or is identical in sequence to, nucleoti ***
--- *** des adjacent to insertion point.         ***
--- ************************************************
---

CREATE VIEW duplication AS
  SELECT
    feature_id AS duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tandem_duplication' OR cvterm.name = 'direct_tandem_duplication' OR cvterm.name = 'inverted_tandem_duplication' OR cvterm.name = 'duplication';

--- ************************************************
--- *** relation: inversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A continuous nucleotide sequence is inve ***
--- *** rted in the same position.               ***
--- ************************************************
---

CREATE VIEW inversion AS
  SELECT
    feature_id AS inversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inversion';

--- ************************************************
--- *** relation: chromosomal_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An extra chromosome.                     ***
--- ************************************************
---

CREATE VIEW chromosomal_duplication AS
  SELECT
    feature_id AS chromosomal_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'interchromosomal_duplication' OR cvterm.name = 'intrachromosomal_duplication' OR cvterm.name = 'free_duplication' OR cvterm.name = 'insertional_duplication' OR cvterm.name = 'inversion_derived_deficiency_plus_duplication' OR cvterm.name = 'inversion_derived_bipartite_duplication' OR cvterm.name = 'inversion_derived_duplication_plus_aneuploid' OR cvterm.name = 'intrachromosomal_transposition' OR cvterm.name = 'bipartite_duplication' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'uninverted_intrachromosomal_transposition' OR cvterm.name = 'unoriented_intrachromosomal_transposition' OR cvterm.name = 'free_ring_duplication' OR cvterm.name = 'uninverted_insertional_duplication' OR cvterm.name = 'inverted_insertional_duplication' OR cvterm.name = 'unoriented_insertional_duplication' OR cvterm.name = 'chromosomal_duplication';

--- ************************************************
--- *** relation: intrachromosomal_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A duplication that occurred within a chr ***
--- *** omosome.                                 ***
--- ************************************************
---

CREATE VIEW intrachromosomal_duplication AS
  SELECT
    feature_id AS intrachromosomal_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inversion_derived_deficiency_plus_duplication' OR cvterm.name = 'inversion_derived_bipartite_duplication' OR cvterm.name = 'inversion_derived_duplication_plus_aneuploid' OR cvterm.name = 'intrachromosomal_transposition' OR cvterm.name = 'bipartite_duplication' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'uninverted_intrachromosomal_transposition' OR cvterm.name = 'unoriented_intrachromosomal_transposition' OR cvterm.name = 'intrachromosomal_duplication';

--- ************************************************
--- *** relation: direct_tandem_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tandem duplication where the individua ***
--- *** l regions are in the same orientation.   ***
--- ************************************************
---

CREATE VIEW direct_tandem_duplication AS
  SELECT
    feature_id AS direct_tandem_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'direct_tandem_duplication';

--- ************************************************
--- *** relation: inverted_tandem_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tandem duplication where the individua ***
--- *** l regions are not in the same orientatio ***
--- *** n.                                       ***
--- ************************************************
---

CREATE VIEW inverted_tandem_duplication AS
  SELECT
    feature_id AS inverted_tandem_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inverted_tandem_duplication';

--- ************************************************
--- *** relation: intrachromosomal_transposition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome structure variation whereby ***
--- ***  a transposition occurred within a chrom ***
--- *** osome.                                   ***
--- ************************************************
---

CREATE VIEW intrachromosomal_transposition AS
  SELECT
    feature_id AS intrachromosomal_transposition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'uninverted_intrachromosomal_transposition' OR cvterm.name = 'unoriented_intrachromosomal_transposition' OR cvterm.name = 'intrachromosomal_transposition';

--- ************************************************
--- *** relation: compound_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome structure variant where a m ***
--- *** onocentric element is caused by the fusi ***
--- *** on of two chromosome arms.               ***
--- ************************************************
---

CREATE VIEW compound_chromosome AS
  SELECT
    feature_id AS compound_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'compound_chromosome_arm' OR cvterm.name = 'homo_compound_chromosome' OR cvterm.name = 'hetero_compound_chromosome' OR cvterm.name = 'compound_chromosome';

--- ************************************************
--- *** relation: robertsonian_fusion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A non reciprocal translocation whereby t ***
--- *** he participating chromosomes break at th ***
--- *** eir centromeres and the long arms fuse t ***
--- *** o form a single chromosome with a single ***
--- ***  centromere.                             ***
--- ************************************************
---

CREATE VIEW robertsonian_fusion AS
  SELECT
    feature_id AS robertsonian_fusion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'Robertsonian_fusion';

--- ************************************************
--- *** relation: chromosomal_translocation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An interchromosomal mutation. Rearrangem ***
--- *** ents that alter the pairing of telomeres ***
--- ***  are classified as translocations.       ***
--- ************************************************
---

CREATE VIEW chromosomal_translocation AS
  SELECT
    feature_id AS chromosomal_translocation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'translocation_element' OR cvterm.name = 'Robertsonian_fusion' OR cvterm.name = 'reciprocal_chromosomal_translocation' OR cvterm.name = 'deficient_translocation' OR cvterm.name = 'inversion_cum_translocation' OR cvterm.name = 'cyclic_translocation' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'chromosomal_translocation';

--- ************************************************
--- *** relation: ring_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A ring chromosome is a chromosome whose  ***
--- *** arms have fused together to form a ring, ***
--- ***  often with the loss of the ends of the  ***
--- *** chromosome.                              ***
--- ************************************************
---

CREATE VIEW ring_chromosome AS
  SELECT
    feature_id AS ring_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inverted_ring_chromosome' OR cvterm.name = 'free_ring_duplication' OR cvterm.name = 'ring_chromosome';

--- ************************************************
--- *** relation: pericentric_inversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosomal inversion that includes th ***
--- *** e centromere.                            ***
--- ************************************************
---

CREATE VIEW pericentric_inversion AS
  SELECT
    feature_id AS pericentric_inversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pericentric_inversion';

--- ************************************************
--- *** relation: paracentric_inversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosomal inversion that does not in ***
--- *** clude the centromere.                    ***
--- ************************************************
---

CREATE VIEW paracentric_inversion AS
  SELECT
    feature_id AS paracentric_inversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'paracentric_inversion';

--- ************************************************
--- *** relation: reciprocal_chromosomal_translocation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosomal translocation with two bre ***
--- *** aks; two chromosome segments have simply ***
--- ***  been exchanged.                         ***
--- ************************************************
---

CREATE VIEW reciprocal_chromosomal_translocation AS
  SELECT
    feature_id AS reciprocal_chromosomal_translocation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'reciprocal_chromosomal_translocation';

--- ************************************************
--- *** relation: autosynaptic_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An autosynaptic chromosome is the aneupl ***

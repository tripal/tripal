SET search_path=so,chado,pg_catalog;
--- ***                                          ***
--- *** A binding site that, in the polypeptide  ***
--- *** molecule, interacts selectively and non- ***
--- *** covalently with metal ions.              ***
--- ************************************************
---

CREATE VIEW polypeptide_metal_contact AS
  SELECT
    feature_id AS polypeptide_metal_contact_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_calcium_ion_contact_site' OR cvterm.name = 'polypeptide_cobalt_ion_contact_site' OR cvterm.name = 'polypeptide_copper_ion_contact_site' OR cvterm.name = 'polypeptide_iron_ion_contact_site' OR cvterm.name = 'polypeptide_magnesium_ion_contact_site' OR cvterm.name = 'polypeptide_manganese_ion_contact_site' OR cvterm.name = 'polypeptide_molybdenum_ion_contact_site' OR cvterm.name = 'polypeptide_nickel_ion_contact_site' OR cvterm.name = 'polypeptide_tungsten_ion_contact_site' OR cvterm.name = 'polypeptide_zinc_ion_contact_site' OR cvterm.name = 'polypeptide_metal_contact';

--- ************************************************
--- *** relation: protein_protein_contact ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the protein mole ***
--- *** cule, interacts selectively and non-cova ***
--- *** lently with polypeptide residues.        ***
--- ************************************************
---

CREATE VIEW protein_protein_contact AS
  SELECT
    feature_id AS protein_protein_contact_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'protein_protein_contact';

--- ************************************************
--- *** relation: polypeptide_calcium_ion_contact_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the polypeptide  ***
--- *** molecule, interacts selectively and non- ***
--- *** covalently with calcium ions.            ***
--- ************************************************
---

CREATE VIEW polypeptide_calcium_ion_contact_site AS
  SELECT
    feature_id AS polypeptide_calcium_ion_contact_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_calcium_ion_contact_site';

--- ************************************************
--- *** relation: polypeptide_cobalt_ion_contact_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the polypeptide  ***
--- *** molecule, interacts selectively and non- ***
--- *** covalently with cobalt ions.             ***
--- ************************************************
---

CREATE VIEW polypeptide_cobalt_ion_contact_site AS
  SELECT
    feature_id AS polypeptide_cobalt_ion_contact_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_cobalt_ion_contact_site';

--- ************************************************
--- *** relation: polypeptide_copper_ion_contact_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the polypeptide  ***
--- *** molecule, interacts selectively and non- ***
--- *** covalently with copper ions.             ***
--- ************************************************
---

CREATE VIEW polypeptide_copper_ion_contact_site AS
  SELECT
    feature_id AS polypeptide_copper_ion_contact_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_copper_ion_contact_site';

--- ************************************************
--- *** relation: polypeptide_iron_ion_contact_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the polypeptide  ***
--- *** molecule, interacts selectively and non- ***
--- *** covalently with iron ions.               ***
--- ************************************************
---

CREATE VIEW polypeptide_iron_ion_contact_site AS
  SELECT
    feature_id AS polypeptide_iron_ion_contact_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_iron_ion_contact_site';

--- ************************************************
--- *** relation: polypeptide_magnesium_ion_contact_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the polypeptide  ***
--- *** molecule, interacts selectively and non- ***
--- *** covalently with magnesium ions.          ***
--- ************************************************
---

CREATE VIEW polypeptide_magnesium_ion_contact_site AS
  SELECT
    feature_id AS polypeptide_magnesium_ion_contact_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_magnesium_ion_contact_site';

--- ************************************************
--- *** relation: polypeptide_manganese_ion_contact_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the polypeptide  ***
--- *** molecule, interacts selectively and non- ***
--- *** covalently with manganese ions.          ***
--- ************************************************
---

CREATE VIEW polypeptide_manganese_ion_contact_site AS
  SELECT
    feature_id AS polypeptide_manganese_ion_contact_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_manganese_ion_contact_site';

--- ************************************************
--- *** relation: polypeptide_molybdenum_ion_contact_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the polypeptide  ***
--- *** molecule, interacts selectively and non- ***
--- *** covalently with molybdenum ions.         ***
--- ************************************************
---

CREATE VIEW polypeptide_molybdenum_ion_contact_site AS
  SELECT
    feature_id AS polypeptide_molybdenum_ion_contact_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_molybdenum_ion_contact_site';

--- ************************************************
--- *** relation: polypeptide_nickel_ion_contact_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the polypeptide  ***
--- *** molecule, interacts selectively and non- ***
--- *** covalently with nickel ions.             ***
--- ************************************************
---

CREATE VIEW polypeptide_nickel_ion_contact_site AS
  SELECT
    feature_id AS polypeptide_nickel_ion_contact_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_nickel_ion_contact_site';

--- ************************************************
--- *** relation: polypeptide_tungsten_ion_contact_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the polypeptide  ***
--- *** molecule, interacts selectively and non- ***
--- *** covalently with tungsten ions.           ***
--- ************************************************
---

CREATE VIEW polypeptide_tungsten_ion_contact_site AS
  SELECT
    feature_id AS polypeptide_tungsten_ion_contact_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_tungsten_ion_contact_site';

--- ************************************************
--- *** relation: polypeptide_zinc_ion_contact_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the polypeptide  ***
--- *** molecule, interacts selectively and non- ***
--- *** covalently with zinc ions.               ***
--- ************************************************
---

CREATE VIEW polypeptide_zinc_ion_contact_site AS
  SELECT
    feature_id AS polypeptide_zinc_ion_contact_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_zinc_ion_contact_site';

--- ************************************************
--- *** relation: catalytic_residue ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Amino acid involved in the activity of a ***
--- *** n enzyme.                                ***
--- ************************************************
---

CREATE VIEW catalytic_residue AS
  SELECT
    feature_id AS catalytic_residue_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'catalytic_residue';

--- ************************************************
--- *** relation: polypeptide_ligand_contact ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Residues which interact with a ligand.   ***
--- ************************************************
---

CREATE VIEW polypeptide_ligand_contact AS
  SELECT
    feature_id AS polypeptide_ligand_contact_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_ligand_contact';

--- ************************************************
--- *** relation: asx_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of five consecutive residues and ***
--- ***  two H-bonds in which: Residue(i) is Asp ***
--- *** artate or Asparagine (Asx), side-chain O ***
--- ***  of residue(i) is H-bonded to the main-c ***
--- *** hain NH of residue(i+2) or (i+3), main-c ***
--- *** hain CO of residue(i) is H-bonded to the ***
--- ***  main-chain NH of residue(i+3) or (i+4). ***
--- ************************************************
---

CREATE VIEW asx_motif AS
  SELECT
    feature_id AS asx_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'asx_motif';

--- ************************************************
--- *** relation: beta_bulge ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of three residues within a beta- ***
--- *** sheet in which the main chains of two co ***
--- *** nsecutive residues are H-bonded to that  ***
--- *** of the third, and in which the dihedral  ***
--- *** angles are as follows: Residue(i): -140  ***
--- *** degrees < phi(l) -20 degrees , -90 degre ***
--- *** es < psi(l) < 40 degrees. Residue (i+1): ***
--- ***  -180 degrees < phi < -25 degrees or +12 ***
--- *** 0 degrees < phi < +180 degrees, +40 degr ***
--- *** ees < psi < +180 degrees or -180 degrees ***
--- ***  < psi < -120 degrees.                   ***
--- ************************************************
---

CREATE VIEW beta_bulge AS
  SELECT
    feature_id AS beta_bulge_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_bulge';

--- ************************************************
--- *** relation: beta_bulge_loop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of three residues within a beta- ***
--- *** sheet consisting of two H-bonds. Beta bu ***
--- *** lge loops often occur at the loop ends o ***
--- *** f beta-hairpins.                         ***
--- ************************************************
---

CREATE VIEW beta_bulge_loop AS
  SELECT
    feature_id AS beta_bulge_loop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_bulge_loop_five' OR cvterm.name = 'beta_bulge_loop_six' OR cvterm.name = 'beta_bulge_loop';

--- ************************************************
--- *** relation: beta_bulge_loop_five ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of three residues within a beta- ***
--- *** sheet consisting of two H-bonds in which ***
--- *** : the main-chain NH of residue(i) is H-b ***
--- *** onded to the main-chain CO of residue(i+ ***
--- *** 4), the main-chain CO of residue i is H- ***
--- *** bonded to the main-chain NH of residue(i ***
--- *** +3), these loops have an RL nest at resi ***
--- *** dues i+2 and i+3.                        ***
--- ************************************************
---

CREATE VIEW beta_bulge_loop_five AS
  SELECT
    feature_id AS beta_bulge_loop_five_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_bulge_loop_five';

--- ************************************************
--- *** relation: beta_bulge_loop_six ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of three residues within a beta- ***
--- *** sheet consisting of two H-bonds in which ***
--- *** : the main-chain NH of residue(i) is H-b ***
--- *** onded to the main-chain CO of residue(i+ ***
--- *** 5), the main-chain CO of residue i is H- ***
--- *** bonded to the main-chain NH of residue(i ***
--- *** +4), these loops have an RL nest at resi ***
--- *** dues i+3 and i+4.                        ***
--- ************************************************
---

CREATE VIEW beta_bulge_loop_six AS
  SELECT
    feature_id AS beta_bulge_loop_six_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_bulge_loop_six';

--- ************************************************
--- *** relation: beta_strand ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A beta strand describes a single length  ***
--- *** of polypeptide chain that forms part of  ***
--- *** a beta sheet. A single continuous stretc ***
--- *** h of amino acids adopting an extended co ***
--- *** nformation of hydrogen bonds between the ***
--- ***  N-O and the C=O of another part of the  ***
--- *** peptide. This forms a secondary protein  ***
--- *** structure in which two or more extended  ***
--- *** polypeptide regions are hydrogen-bonded  ***
--- *** to one another in a planar array.        ***
--- ************************************************
---

CREATE VIEW beta_strand AS
  SELECT
    feature_id AS beta_strand_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'antiparallel_beta_strand' OR cvterm.name = 'parallel_beta_strand' OR cvterm.name = 'beta_strand';

--- ************************************************
--- *** relation: antiparallel_beta_strand ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A peptide region which hydrogen bonded t ***
--- *** o another region of peptide running in t ***
--- *** he oposite direction (one running N-term ***
--- *** inal to C-terminal and one running C-ter ***
--- *** minal to N-terminal). Hydrogen bonding o ***
--- *** ccurs between every other C=O from one s ***
--- *** trand to every other N-H on the adjacent ***
--- ***  strand. In this case, if two atoms C-al ***
--- *** pha (i) and C-alpha (j) are adjacent in  ***
--- *** two hydrogen-bonded beta strands, then t ***
--- *** hey form two mutual backbone hydrogen bo ***
--- *** nds to each other's flanking peptide gro ***
--- *** ups; this is known as a close pair of hy ***
--- *** drogen bonds. The peptide backbone dihed ***
--- *** ral angles (phi, psi) are about (-140 de ***
--- *** grees, 135 degrees) in antiparallel shee ***
--- *** ts.                                      ***
--- ************************************************
---

CREATE VIEW antiparallel_beta_strand AS
  SELECT
    feature_id AS antiparallel_beta_strand_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'antiparallel_beta_strand';

--- ************************************************
--- *** relation: parallel_beta_strand ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A peptide region which hydrogen bonded t ***
--- *** o another region of peptide running in t ***
--- *** he oposite direction (both running N-ter ***
--- *** minal to C-terminal). This orientation i ***
--- *** s slightly less stable because it introd ***
--- *** uces nonplanarity in the inter-strand hy ***
--- *** drogen bonding pattern. Hydrogen bonding ***
--- ***  occurs between every other C=O from one ***
--- ***  strand to every other N-H on the adjace ***
--- *** nt strand. In this case, if two atoms C- ***
--- *** alpha (i)and C-alpha (j) are adjacent in ***
--- ***  two hydrogen-bonded beta strands, then  ***
--- *** they do not hydrogen bond to each other; ***
--- ***  rather, one residue forms hydrogen bond ***
--- *** s to the residues that flank the other ( ***
--- *** but not vice versa). For example, residu ***
--- *** e i may form hydrogen bonds to residues  ***
--- *** j - 1 and j + 1; this is known as a wide ***
--- ***  pair of hydrogen bonds. By contrast, re ***
--- *** sidue j may hydrogen-bond to different r ***
--- *** esidues altogether, or to none at all. T ***
--- *** he dihedral angles (phi, psi) are about  ***
--- *** (-120 degrees, 115 degrees) in parallel  ***
--- *** sheets.                                  ***
--- ************************************************
---

CREATE VIEW parallel_beta_strand AS
  SELECT
    feature_id AS parallel_beta_strand_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'parallel_beta_strand';

--- ************************************************
--- *** relation: peptide_helix ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A helix is a secondary_structure conform ***
--- *** ation where the peptide backbone forms a ***
--- ***  coil.                                   ***
--- ************************************************
---

CREATE VIEW peptide_helix AS
  SELECT
    feature_id AS peptide_helix_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'left_handed_peptide_helix' OR cvterm.name = 'right_handed_peptide_helix' OR cvterm.name = 'alpha_helix' OR cvterm.name = 'pi_helix' OR cvterm.name = 'three_ten_helix' OR cvterm.name = 'peptide_helix';

--- ************************************************
--- *** relation: left_handed_peptide_helix ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A left handed helix is a region of pepti ***
--- *** de where the coiled conformation turns i ***
--- *** n an anticlockwise, left handed screw.   ***
--- ************************************************
---

CREATE VIEW left_handed_peptide_helix AS
  SELECT
    feature_id AS left_handed_peptide_helix_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'left_handed_peptide_helix';

--- ************************************************
--- *** relation: right_handed_peptide_helix ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A right handed helix is a region of pept ***
--- *** ide where the coiled conformation turns  ***
--- *** in a clockwise, right handed screw.      ***
--- ************************************************
---

CREATE VIEW right_handed_peptide_helix AS
  SELECT
    feature_id AS right_handed_peptide_helix_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'alpha_helix' OR cvterm.name = 'pi_helix' OR cvterm.name = 'three_ten_helix' OR cvterm.name = 'right_handed_peptide_helix';

--- ************************************************
--- *** relation: alpha_helix ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The helix has 3.6 residues per turn whic ***
--- *** h corersponds to a translation of 1.5 an ***
--- *** gstroms (= 0.15 nm) along the helical ax ***
--- *** is. Every backbone N-H group donates a h ***
--- *** ydrogen bond to the backbone C=O group o ***
--- *** f the amino acid four residues earlier.  ***
--- ************************************************
---

CREATE VIEW alpha_helix AS
  SELECT
    feature_id AS alpha_helix_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'alpha_helix';

--- ************************************************
--- *** relation: pi_helix ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The pi helix has 4.1 residues per turn a ***
--- *** nd a translation of 1.15  (=0.115 nm) al ***
--- *** ong the helical axis. The N-H group of a ***
--- *** n amino acid forms a hydrogen bond with  ***
--- *** the C=O group of the amino acid five res ***
--- *** idues earlier.                           ***
--- ************************************************
---

CREATE VIEW pi_helix AS
  SELECT
    feature_id AS pi_helix_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pi_helix';

--- ************************************************
--- *** relation: three_ten_helix ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The 3-10 helix has 3 residues per turn w ***
--- *** ith a translation of 2.0 angstroms (=0.2 ***
--- ***  nm) along the helical axis. The N-H gro ***
--- *** up of an amino acid forms a hydrogen bon ***
--- *** d with the C=O group of the amino acid t ***
--- *** hree residues earlier.                   ***
--- ************************************************
---

CREATE VIEW three_ten_helix AS
  SELECT
    feature_id AS three_ten_helix_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_ten_helix';

--- ************************************************
--- *** relation: polypeptide_nest_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of two consecutive residues with ***
--- ***  dihedral angles. Nest should not have P ***
--- *** roline as any residue. Nests frequently  ***
--- *** occur as parts of other motifs such as S ***
--- *** chellman loops.                          ***
--- ************************************************
---

CREATE VIEW polypeptide_nest_motif AS
  SELECT
    feature_id AS polypeptide_nest_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_nest_left_right_motif' OR cvterm.name = 'polypeptide_nest_right_left_motif' OR cvterm.name = 'polypeptide_nest_motif';

--- ************************************************
--- *** relation: polypeptide_nest_left_right_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of two consecutive residues with ***
--- ***  dihedral angles: Residue(i): +20 degree ***
--- *** s < phi < +140 degrees, -40 degrees < ps ***
--- *** i < +90 degrees. Residue(i+1): -140 degr ***
--- *** ees < phi < -20 degrees, -90 degrees < p ***
--- *** si < +40 degrees.                        ***
--- ************************************************
---

CREATE VIEW polypeptide_nest_left_right_motif AS
  SELECT
    feature_id AS polypeptide_nest_left_right_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_nest_left_right_motif';

--- ************************************************
--- *** relation: polypeptide_nest_right_left_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of two consecutive residues with ***
--- ***  dihedral angles: Residue(i): -140 degre ***
--- *** es < phi < -20 degrees, -90 degrees < ps ***
--- *** i < +40 degrees. Residue(i+1): +20 degre ***
--- *** es < phi < +140 degrees, -40 degrees < p ***
--- *** si < +90 degrees.                        ***
--- ************************************************
---

CREATE VIEW polypeptide_nest_right_left_motif AS
  SELECT
    feature_id AS polypeptide_nest_right_left_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_nest_right_left_motif';

--- ************************************************
--- *** relation: schellmann_loop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of six or seven consecutive resi ***
--- *** dues that contains two H-bonds.          ***
--- ************************************************
---

CREATE VIEW schellmann_loop AS
  SELECT
    feature_id AS schellmann_loop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'schellmann_loop_seven' OR cvterm.name = 'schellmann_loop_six' OR cvterm.name = 'schellmann_loop';

--- ************************************************
--- *** relation: schellmann_loop_seven ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Wild type: A motif of seven consecutive  ***
--- *** residues that contains two H-bonds in wh ***
--- *** ich: the main-chain CO of residue(i) is  ***
--- *** H-bonded to the main-chain NH of residue ***
--- *** (i+6), the main-chain CO of residue(i+1) ***
--- ***  is H-bonded to the main-chain NH of res ***
--- *** idue(i+5).                               ***
--- ************************************************
---

CREATE VIEW schellmann_loop_seven AS
  SELECT
    feature_id AS schellmann_loop_seven_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'schellmann_loop_seven';

--- ************************************************
--- *** relation: schellmann_loop_six ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Common Type: A motif of six consecutive  ***
--- *** residues that contains two H-bonds in wh ***
--- *** ich: the main-chain CO of residue(i) is  ***
--- *** H-bonded to the main-chain NH of residue ***
--- *** (i+5) the main-chain CO of residue(i+1)  ***
--- *** is H-bonded to the main-chain NH of resi ***
--- *** due(i+4).                                ***
--- ************************************************
---

CREATE VIEW schellmann_loop_six AS
  SELECT
    feature_id AS schellmann_loop_six_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'schellmann_loop_six';

--- ************************************************
--- *** relation: serine_threonine_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of five consecutive residues and ***
--- ***  two hydrogen bonds in which: residue(i) ***
--- ***  is Serine (S) or Threonine (T), the sid ***
--- *** e-chain O of residue(i) is H-bonded to t ***
--- *** he main-chain NH of residue(i+2) or (i+3 ***
--- *** ) , the main-chain CO group of residue(i ***
--- *** ) is H-bonded to the main-chain NH of re ***
--- *** sidue(i+3) or (i+4).                     ***
--- ************************************************
---

CREATE VIEW serine_threonine_motif AS
  SELECT
    feature_id AS serine_threonine_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'serine_threonine_motif';

--- ************************************************
--- *** relation: serine_threonine_staple_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of four or five consecutive resi ***
--- *** dues and one H-bond in which: residue(i) ***
--- ***  is Serine (S) or Threonine (T), the sid ***
--- *** e-chain OH of residue(i) is H-bonded to  ***
--- *** the main-chain CO of residue(i3) or (i4) ***
--- *** , Phi angles of residues(i1), (i2) and ( ***
--- *** i3) are negative.                        ***
--- ************************************************
---

CREATE VIEW serine_threonine_staple_motif AS
  SELECT
    feature_id AS serine_threonine_staple_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'serine_threonine_staple_motif';

--- ************************************************
--- *** relation: polypeptide_turn_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A reversal in the direction of the backb ***
--- *** one of a protein that is stabilized by h ***
--- *** ydrogen bond between backbone NH and CO  ***
--- *** groups, involving no more than 4 amino a ***
--- *** cid residues.                            ***
--- ************************************************
---

CREATE VIEW polypeptide_turn_motif AS
  SELECT
    feature_id AS polypeptide_turn_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'asx_turn' OR cvterm.name = 'beta_turn' OR cvterm.name = 'gamma_turn' OR cvterm.name = 'serine_threonine_turn' OR cvterm.name = 'asx_turn_left_handed_type_one' OR cvterm.name = 'asx_turn_left_handed_type_two' OR cvterm.name = 'asx_turn_right_handed_type_two' OR cvterm.name = 'asx_turn_right_handed_type_one' OR cvterm.name = 'beta_turn_left_handed_type_one' OR cvterm.name = 'beta_turn_left_handed_type_two' OR cvterm.name = 'beta_turn_right_handed_type_one' OR cvterm.name = 'beta_turn_right_handed_type_two' OR cvterm.name = 'beta_turn_type_six' OR cvterm.name = 'beta_turn_type_eight' OR cvterm.name = 'beta_turn_type_six_a' OR cvterm.name = 'beta_turn_type_six_b' OR cvterm.name = 'beta_turn_type_six_a_one' OR cvterm.name = 'beta_turn_type_six_a_two' OR cvterm.name = 'gamma_turn_classic' OR cvterm.name = 'gamma_turn_inverse' OR cvterm.name = 'st_turn_left_handed_type_one' OR cvterm.name = 'st_turn_left_handed_type_two' OR cvterm.name = 'st_turn_right_handed_type_one' OR cvterm.name = 'st_turn_right_handed_type_two' OR cvterm.name = 'polypeptide_turn_motif';

--- ************************************************
--- *** relation: asx_turn_left_handed_type_one ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Left handed type I (dihedral angles):- R ***
--- *** esidue(i): -140 degrees < chi (1) -120 d ***
--- *** egrees < -20 degrees, -90 degrees < psi  ***
--- *** +120 degrees < +40 degrees. Residue(i+1) ***
--- *** : -140 degrees < phi < -20 degrees, -90  ***
--- *** degrees < psi < +40 degrees.             ***
--- ************************************************
---

CREATE VIEW asx_turn_left_handed_type_one AS
  SELECT
    feature_id AS asx_turn_left_handed_type_one_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'asx_turn_left_handed_type_one';

--- ************************************************
--- *** relation: asx_turn_left_handed_type_two ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Left handed type II (dihedral angles):-  ***
--- *** Residue(i): -140 degrees < chi (1) -120  ***
--- *** degrees < -20 degrees, +80 degrees < psi ***
--- ***  +120 degrees < +180 degrees. Residue(i+ ***
--- *** 1): +20 degrees < phi < +140 degrees, -4 ***
--- *** 0 degrees < psi < +90 degrees.           ***
--- ************************************************
---

CREATE VIEW asx_turn_left_handed_type_two AS
  SELECT
    feature_id AS asx_turn_left_handed_type_two_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'asx_turn_left_handed_type_two';

--- ************************************************
--- *** relation: asx_turn_right_handed_type_two ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Right handed type II (dihedral angles):- ***
--- ***  Residue(i): -140 degrees < chi (1) -120 ***
--- ***  degrees < -20 degrees, +80 degrees < ps ***
--- *** i +120 degrees < +180 degrees. Residue(i ***
--- *** +1): +20 degrees < phi < +140 degrees, - ***
--- *** 40 degrees < psi < +90 degrees.          ***
--- ************************************************
---

CREATE VIEW asx_turn_right_handed_type_two AS
  SELECT
    feature_id AS asx_turn_right_handed_type_two_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'asx_turn_right_handed_type_two';

--- ************************************************
--- *** relation: asx_turn_right_handed_type_one ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Right handed type I (dihedral angles):-  ***
--- *** Residue(i): -140 degrees < chi (1) -120  ***
--- *** degrees < -20 degrees, -90 degrees < psi ***
--- ***  +120 degrees < +40 degrees. Residue(i+1 ***
--- *** ): -140 degrees < phi < -20 degrees, -90 ***
--- ***  degrees < psi < +40 degrees.            ***
--- ************************************************
---

CREATE VIEW asx_turn_right_handed_type_one AS
  SELECT
    feature_id AS asx_turn_right_handed_type_one_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'asx_turn_right_handed_type_one';

--- ************************************************
--- *** relation: beta_turn ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of four consecutive residues tha ***
--- *** t may contain one H-bond, which, if pres ***
--- *** ent, is between the main-chain CO of the ***
--- ***  first residue and the main-chain NH of  ***
--- *** the fourth. It is characterized by the d ***
--- *** ihedral angles of the second and third r ***
--- *** esidues, which are the basis for sub-cat ***
--- *** egorization.                             ***
--- ************************************************
---

CREATE VIEW beta_turn AS
  SELECT
    feature_id AS beta_turn_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_turn_left_handed_type_one' OR cvterm.name = 'beta_turn_left_handed_type_two' OR cvterm.name = 'beta_turn_right_handed_type_one' OR cvterm.name = 'beta_turn_right_handed_type_two' OR cvterm.name = 'beta_turn_type_six' OR cvterm.name = 'beta_turn_type_eight' OR cvterm.name = 'beta_turn_type_six_a' OR cvterm.name = 'beta_turn_type_six_b' OR cvterm.name = 'beta_turn_type_six_a_one' OR cvterm.name = 'beta_turn_type_six_a_two' OR cvterm.name = 'beta_turn';

--- ************************************************
--- *** relation: beta_turn_left_handed_type_one ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Left handed type I:A motif of four conse ***
--- *** cutive residues that may contain one H-b ***
--- *** ond, which, if present, is between the m ***
--- *** ain-chain CO of the first residue and th ***
--- *** e main-chain NH of the fourth. It is cha ***
--- *** racterized by the dihedral angles:- Resi ***
--- *** due(i+1): -140 degrees > phi > -20 degre ***
--- *** es, -90 degrees > psi > +40 degrees. Res ***
--- *** idue(i+2): -140 degrees > phi > -20 degr ***
--- *** ees, -90 degrees > psi > +40 degrees.    ***
--- ************************************************
---

CREATE VIEW beta_turn_left_handed_type_one AS
  SELECT
    feature_id AS beta_turn_left_handed_type_one_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_turn_left_handed_type_one';

--- ************************************************
--- *** relation: beta_turn_left_handed_type_two ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Left handed type II: A motif of four con ***
--- *** secutive residues that may contain one H ***
--- *** -bond, which, if present, is between the ***
--- ***  main-chain CO of the first residue and  ***
--- *** the main-chain NH of the fourth. It is c ***
--- *** haracterized by the dihedral angles: Res ***
--- *** idue(i+1): -140 degrees > phi > -20 degr ***
--- *** ees, +80 degrees > psi > +180 degrees. R ***
--- *** esidue(i+2): +20 degrees > phi > +140 de ***
--- *** grees, -40 degrees > psi > +90 degrees.  ***
--- ************************************************
---

CREATE VIEW beta_turn_left_handed_type_two AS
  SELECT
    feature_id AS beta_turn_left_handed_type_two_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_turn_left_handed_type_two';

--- ************************************************
--- *** relation: beta_turn_right_handed_type_one ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Right handed type I:A motif of four cons ***
--- *** ecutive residues that may contain one H- ***
--- *** bond, which, if present, is between the  ***
--- *** main-chain CO of the first residue and t ***
--- *** he main-chain NH of the fourth. It is ch ***
--- *** aracterized by the dihedral angles: Resi ***
--- *** due(i+1): -140 degrees < phi < -20 degre ***
--- *** es, -90 degrees < psi < +40 degrees. Res ***
--- *** idue(i+2): -140 degrees < phi < -20 degr ***
--- *** ees, -90 degrees < psi < +40 degrees.    ***
--- ************************************************
---

CREATE VIEW beta_turn_right_handed_type_one AS
  SELECT
    feature_id AS beta_turn_right_handed_type_one_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_turn_right_handed_type_one';

--- ************************************************
--- *** relation: beta_turn_right_handed_type_two ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Right handed type II:A motif of four con ***
--- *** secutive residues that may contain one H ***
--- *** -bond, which, if present, is between the ***
--- ***  main-chain CO of the first residue and  ***
--- *** the main-chain NH of the fourth. It is c ***
--- *** haracterized by the dihedral angles: Res ***
--- *** idue(i+1): -140 degrees < phi < -20 degr ***
--- *** ees, +80 degrees < psi < +180 degrees. R ***
--- *** esidue(i+2): +20 degrees < phi < +140 de ***
--- *** grees, -40 degrees < psi < +90 degrees.  ***
--- ************************************************
---

CREATE VIEW beta_turn_right_handed_type_two AS
  SELECT
    feature_id AS beta_turn_right_handed_type_two_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_turn_right_handed_type_two';

--- ************************************************
--- *** relation: gamma_turn ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Gamma turns, defined for 3 residues i,(  ***
--- *** i+1),( i+2) if a hydrogen bond exists be ***
--- *** tween residues i and i+2 and the phi and ***
--- ***  psi angles of residue i+1 fall within 4 ***
--- *** 0 degrees.                               ***
--- ************************************************
---

CREATE VIEW gamma_turn AS
  SELECT
    feature_id AS gamma_turn_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gamma_turn_classic' OR cvterm.name = 'gamma_turn_inverse' OR cvterm.name = 'gamma_turn';

--- ************************************************
--- *** relation: gamma_turn_classic ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Gamma turns, defined for 3 residues i, i ***
--- *** +1, i+2 if a hydrogen bond exists betwee ***
--- *** n residues i and i+2 and the phi and psi ***
--- ***  angles of residue i+1 fall within 40 de ***
--- *** grees: phi(i+1)=75.0 - psi(i+1)=-64.0.   ***
--- ************************************************
---

CREATE VIEW gamma_turn_classic AS
  SELECT
    feature_id AS gamma_turn_classic_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gamma_turn_classic';

--- ************************************************

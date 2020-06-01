SET search_path=so,chado,pg_catalog;
--- *** relation: gamma_turn_inverse ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Gamma turns, defined for 3 residues i, i ***
--- *** +1, i+2 if a hydrogen bond exists betwee ***
--- *** n residues i and i+2 and the phi and psi ***
--- ***  angles of residue i+1 fall within 40 de ***
--- *** grees: phi(i+1)=-79.0 - psi(i+1)=69.0.   ***
--- ************************************************
---

CREATE VIEW gamma_turn_inverse AS
  SELECT
    feature_id AS gamma_turn_inverse_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gamma_turn_inverse';

--- ************************************************
--- *** relation: serine_threonine_turn ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of three consecutive residues an ***
--- *** d one H-bond in which: residue(i) is Ser ***
--- *** ine (S) or Threonine (T), the side-chain ***
--- ***  O of residue(i) is H-bonded to the main ***
--- *** -chain NH of residue(i+2).               ***
--- ************************************************
---

CREATE VIEW serine_threonine_turn AS
  SELECT
    feature_id AS serine_threonine_turn_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'st_turn_left_handed_type_one' OR cvterm.name = 'st_turn_left_handed_type_two' OR cvterm.name = 'st_turn_right_handed_type_one' OR cvterm.name = 'st_turn_right_handed_type_two' OR cvterm.name = 'serine_threonine_turn';

--- ************************************************
--- *** relation: st_turn_left_handed_type_one ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The peptide twists in an anticlockwise,  ***
--- *** left handed manner. The dihedral angles  ***
--- *** for this turn are: Residue(i): -140 degr ***
--- *** ees < chi(1) -120 degrees < -20 degrees, ***
--- ***  -90 degrees psi +120 degrees < +40 degr ***
--- *** ees, residue(i+1): -140 degrees < phi <  ***
--- *** -20 degrees, -90 < psi < +40 degrees.    ***
--- ************************************************
---

CREATE VIEW st_turn_left_handed_type_one AS
  SELECT
    feature_id AS st_turn_left_handed_type_one_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'st_turn_left_handed_type_one';

--- ************************************************
--- *** relation: st_turn_left_handed_type_two ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The peptide twists in an anticlockwise,  ***
--- *** left handed manner. The dihedral angles  ***
--- *** for this turn are: Residue(i): -140 degr ***
--- *** ees < chi(1) -120 degrees < -20 degrees, ***
--- ***  +80 degrees psi +120 degrees < +180 deg ***
--- *** rees, residue(i+1): +20 degrees < phi <  ***
--- *** +140 degrees, -40 < psi < +90 degrees.   ***
--- ************************************************
---

CREATE VIEW st_turn_left_handed_type_two AS
  SELECT
    feature_id AS st_turn_left_handed_type_two_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'st_turn_left_handed_type_two';

--- ************************************************
--- *** relation: st_turn_right_handed_type_one ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The peptide twists in an clockwise, righ ***
--- *** t handed manner. The dihedral angles for ***
--- ***  this turn are: Residue(i): -140 degrees ***
--- ***  < chi(1) -120 degrees < -20 degrees, -9 ***
--- *** 0 degrees psi +120 degrees < +40 degrees ***
--- *** , residue(i+1): -140 degrees < phi < -20 ***
--- ***  degrees, -90 < psi < +40 degrees.       ***
--- ************************************************
---

CREATE VIEW st_turn_right_handed_type_one AS
  SELECT
    feature_id AS st_turn_right_handed_type_one_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'st_turn_right_handed_type_one';

--- ************************************************
--- *** relation: st_turn_right_handed_type_two ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The peptide twists in an clockwise, righ ***
--- *** t handed manner. The dihedral angles for ***
--- ***  this turn are: Residue(i): -140 degrees ***
--- ***  < chi(1) -120 degrees < -20 degrees, +8 ***
--- *** 0 degrees psi +120 degrees < +180 degree ***
--- *** s, residue(i+1): +20 degrees < phi < +14 ***
--- *** 0 degrees, -40 < psi < +90 degrees.      ***
--- ************************************************
---

CREATE VIEW st_turn_right_handed_type_two AS
  SELECT
    feature_id AS st_turn_right_handed_type_two_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'st_turn_right_handed_type_two';

--- ************************************************
--- *** relation: polypeptide_variation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A site of sequence variation (alteration ***
--- *** ). Alternative sequence due to naturally ***
--- ***  occuring events such as polymorphisms a ***
--- *** nd altermatve splicing or experimental m ***
--- *** ethods such as site directed mutagenesis ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW polypeptide_variation_site AS
  SELECT
    feature_id AS polypeptide_variation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'natural_variant_site' OR cvterm.name = 'mutated_variant_site' OR cvterm.name = 'alternate_sequence_site' OR cvterm.name = 'polypeptide_variation_site';

--- ************************************************
--- *** relation: natural_variant_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Describes the natural sequence variants  ***
--- *** due to polymorphisms, disease-associated ***
--- ***  mutations, RNA editing and variations b ***
--- *** etween strains, isolates or cultivars.   ***
--- ************************************************
---

CREATE VIEW natural_variant_site AS
  SELECT
    feature_id AS natural_variant_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'natural_variant_site';

--- ************************************************
--- *** relation: mutated_variant_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Site which has been experimentally alter ***
--- *** ed.                                      ***
--- ************************************************
---

CREATE VIEW mutated_variant_site AS
  SELECT
    feature_id AS mutated_variant_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mutated_variant_site';

--- ************************************************
--- *** relation: alternate_sequence_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Description of sequence variants produce ***
--- *** d by alternative splicing, alternative p ***
--- *** romoter usage, alternative initiation an ***
--- *** d ribosomal frameshifting.               ***
--- ************************************************
---

CREATE VIEW alternate_sequence_site AS
  SELECT
    feature_id AS alternate_sequence_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'alternate_sequence_site';

--- ************************************************
--- *** relation: beta_turn_type_six ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of four consecutive peptide resi ***
--- *** des of type VIa or type VIb and where th ***
--- *** e i+2 residue is cis-proline.            ***
--- ************************************************
---

CREATE VIEW beta_turn_type_six AS
  SELECT
    feature_id AS beta_turn_type_six_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_turn_type_six_a' OR cvterm.name = 'beta_turn_type_six_b' OR cvterm.name = 'beta_turn_type_six_a_one' OR cvterm.name = 'beta_turn_type_six_a_two' OR cvterm.name = 'beta_turn_type_six';

--- ************************************************
--- *** relation: beta_turn_type_six_a ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of four consecutive peptide resi ***
--- *** dues, of which the i+2 residue is prolin ***
--- *** e, and that may contain one H-bond, whic ***
--- *** h, if present, is between the main-chain ***
--- ***  CO of the first residue and the main-ch ***
--- *** ain NH of the fourth and is characterize ***
--- *** d by the dihedral angles: Residue(i+1):  ***
--- *** phi ~ -60 degrees, psi ~ 120 degrees. Re ***
--- *** sidue(i+2): phi ~ -90 degrees, psi ~ 0 d ***
--- *** egrees.                                  ***
--- ************************************************
---

CREATE VIEW beta_turn_type_six_a AS
  SELECT
    feature_id AS beta_turn_type_six_a_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_turn_type_six_a_one' OR cvterm.name = 'beta_turn_type_six_a_two' OR cvterm.name = 'beta_turn_type_six_a';

--- ************************************************
--- *** relation: beta_turn_type_six_a_one ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW beta_turn_type_six_a_one AS
  SELECT
    feature_id AS beta_turn_type_six_a_one_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_turn_type_six_a_one';

--- ************************************************
--- *** relation: beta_turn_type_six_a_two ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW beta_turn_type_six_a_two AS
  SELECT
    feature_id AS beta_turn_type_six_a_two_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_turn_type_six_a_two';

--- ************************************************
--- *** relation: beta_turn_type_six_b ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of four consecutive peptide resi ***
--- *** dues, of which the i+2 residue is prolin ***
--- *** e, and that may contain one H-bond, whic ***
--- *** h, if present, is between the main-chain ***
--- ***  CO of the first residue and the main-ch ***
--- *** ain NH of the fourth and is characterize ***
--- *** d by the dihedral angles: Residue(i+1):  ***
--- *** phi ~ -120 degrees, psi ~ 120 degrees. R ***
--- *** esidue(i+2): phi ~ -60 degrees, psi ~ 0  ***
--- *** degrees.                                 ***
--- ************************************************
---

CREATE VIEW beta_turn_type_six_b AS
  SELECT
    feature_id AS beta_turn_type_six_b_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_turn_type_six_b';

--- ************************************************
--- *** relation: beta_turn_type_eight ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif of four consecutive peptide resi ***
--- *** dues that may contain one H-bond, which, ***
--- ***  if present, is between the main-chain C ***
--- *** O of the first residue and the main-chai ***
--- *** n NH of the fourth and is characterized  ***
--- *** by the dihedral angles: Residue(i+1): ph ***
--- *** i ~ -60 degrees, psi ~ -30 degrees. Resi ***
--- *** due(i+2): phi ~ -120 degrees, psi ~ 120  ***
--- *** degrees.                                 ***
--- ************************************************
---

CREATE VIEW beta_turn_type_eight AS
  SELECT
    feature_id AS beta_turn_type_eight_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'beta_turn_type_eight';

--- ************************************************
--- *** relation: dre_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence element characteristic of som ***
--- *** e RNA polymerase II promoters, usually l ***
--- *** ocated between -10 and -60 relative to t ***
--- *** he TSS. Consensus sequence is WATCGATW.  ***
--- ************************************************
---

CREATE VIEW dre_motif AS
  SELECT
    feature_id AS dre_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DRE_motif';

--- ************************************************
--- *** relation: dmv4_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence element characteristic of som ***
--- *** e RNA polymerase II promoters, located i ***
--- *** mmediately upstream of some TATA box ele ***
--- *** ments with respect to the TSS (+1). Cons ***
--- *** ensus sequence is YGGTCACACTR. Marked sp ***
--- *** atial preference within core promoter; t ***
--- *** end to occur near the TSS, although not  ***
--- *** as tightly as INR (SO:0000014).          ***
--- ************************************************
---

CREATE VIEW dmv4_motif AS
  SELECT
    feature_id AS dmv4_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DMv4_motif';

--- ************************************************
--- *** relation: e_box_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence element characteristic of som ***
--- *** e RNA polymerase II promoters, usually l ***
--- *** ocated between -60 and +1 relative to th ***
--- *** e TSS. Consensus sequence is AWCAGCTGWT. ***
--- ***  Tends to co-occur with DMv2 (SO:0001161 ***
--- *** ). Tends to not occur with DPE motif (SO ***
--- *** :0000015).                               ***
--- ************************************************
---

CREATE VIEW e_box_motif AS
  SELECT
    feature_id AS e_box_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'E_box_motif';

--- ************************************************
--- *** relation: dmv5_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence element characteristic of som ***
--- *** e RNA polymerase II promoters, usually l ***
--- *** ocated between -50 and -10 relative to t ***
--- *** he TSS. Consensus sequence is KTYRGTATWT ***
--- *** TT. Tends to co-occur with DMv4 (SO:0001 ***
--- *** 157) . Tends to not occur with DPE motif ***
--- ***  (SO:0000015) or MTE (SO:0001162).       ***
--- ************************************************
---

CREATE VIEW dmv5_motif AS
  SELECT
    feature_id AS dmv5_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DMv5_motif';

--- ************************************************
--- *** relation: dmv3_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence element characteristic of som ***
--- *** e RNA polymerase II promoters, usually l ***
--- *** ocated between -30 and +15 relative to t ***
--- *** he TSS. Consensus sequence is KNNCAKCNCT ***
--- *** RNY. Tends to co-occur with DMv2 (SO:000 ***
--- *** 1161). Tends to not occur with DPE motif ***
--- ***  (SO:0000015) or MTE (0001162).          ***
--- ************************************************
---

CREATE VIEW dmv3_motif AS
  SELECT
    feature_id AS dmv3_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DMv3_motif';

--- ************************************************
--- *** relation: dmv2_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence element characteristic of som ***
--- *** e RNA polymerase II promoters, usually l ***
--- *** ocated between -60 and -45 relative to t ***
--- *** he TSS. Consensus sequence is MKSYGGCARC ***
--- *** GSYSS. Tends to co-occur with DMv3 (SO:0 ***
--- *** 001160). Tends to not occur with DPE mot ***
--- *** if (SO:0000015) or MTE (SO:0001162).     ***
--- ************************************************
---

CREATE VIEW dmv2_motif AS
  SELECT
    feature_id AS dmv2_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DMv2_motif';

--- ************************************************
--- *** relation: mte ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence element characteristic of som ***
--- *** e RNA polymerase II promoters, usually l ***
--- *** ocated between +20 and +30 relative to t ***
--- *** he TSS. Consensus sequence is CSARCSSAAC ***
--- *** GS. Tends to co-occur with INR motif (SO ***
--- *** :0000014). Tends to not occur with DPE m ***
--- *** otif (SO:0000015) or DMv5 (SO:0001159).  ***
--- ************************************************
---

CREATE VIEW mte AS
  SELECT
    feature_id AS mte_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'MTE';

--- ************************************************
--- *** relation: inr1_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A promoter motif with consensus sequence ***
--- ***  TCATTCG.                                ***
--- ************************************************
---

CREATE VIEW inr1_motif AS
  SELECT
    feature_id AS inr1_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'INR1_motif';

--- ************************************************
--- *** relation: dpe1_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A promoter motif with consensus sequence ***
--- ***  CGGACGT.                                ***
--- ************************************************
---

CREATE VIEW dpe1_motif AS
  SELECT
    feature_id AS dpe1_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DPE1_motif';

--- ************************************************
--- *** relation: dmv1_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A promoter motif with consensus sequence ***
--- ***  CARCCCT.                                ***
--- ************************************************
---

CREATE VIEW dmv1_motif AS
  SELECT
    feature_id AS dmv1_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DMv1_motif';

--- ************************************************
--- *** relation: gaga_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A non directional promoter motif with co ***
--- *** nsensus sequence GAGAGCG.                ***
--- ************************************************
---

CREATE VIEW gaga_motif AS
  SELECT
    feature_id AS gaga_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'GAGA_motif';

--- ************************************************
--- *** relation: ndm2_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A non directional promoter motif with co ***
--- *** nsensus CGMYGYCR.                        ***
--- ************************************************
---

CREATE VIEW ndm2_motif AS
  SELECT
    feature_id AS ndm2_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'NDM2_motif';

--- ************************************************
--- *** relation: ndm3_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A non directional promoter motif with co ***
--- *** nsensus sequence GAAAGCT.                ***
--- ************************************************
---

CREATE VIEW ndm3_motif AS
  SELECT
    feature_id AS ndm3_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'NDM3_motif';

--- ************************************************
--- *** relation: ds_rna_viral_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A ds_RNA_viral_sequence is a viral_seque ***
--- *** nce that is the sequence of a virus that ***
--- ***  exists as double stranded RNA.          ***
--- ************************************************
---

CREATE VIEW ds_rna_viral_sequence AS
  SELECT
    feature_id AS ds_rna_viral_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ds_RNA_viral_sequence';

--- ************************************************
--- *** relation: polinton ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of DNA transposon that populates  ***
--- *** the genomes of protists, fungi, and anim ***
--- *** als, characterized by a unique set of pr ***
--- *** oteins necessary for their transposition ***
--- *** , including a protein-primed DNA polymer ***
--- *** ase B, retroviral integrase, cysteine pr ***
--- *** otease, and ATPase. Polintons are charac ***
--- *** terized by 6-bp target site duplications ***
--- *** , terminal-inverted repeats that are sev ***
--- *** eral hundred nucleotides long, and 5'-AG ***
--- ***  and TC-3' termini. Polintons exist as a ***
--- *** utonomous and nonautonomous elements.    ***
--- ************************************************
---

CREATE VIEW polinton AS
  SELECT
    feature_id AS polinton_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polinton';

--- ************************************************
--- *** relation: rrna_21s ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A component of the large ribosomal subun ***
--- *** it in mitochondrial rRNA.                ***
--- ************************************************
---

CREATE VIEW rrna_21s AS
  SELECT
    feature_id AS rrna_21s_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_21S';

--- ************************************************
--- *** relation: trna_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a tRNA.                      ***
--- ************************************************
---

CREATE VIEW trna_region AS
  SELECT
    feature_id AS trna_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'anticodon_loop' OR cvterm.name = 'anticodon' OR cvterm.name = 'CCA_tail' OR cvterm.name = 'DHU_loop' OR cvterm.name = 'T_loop' OR cvterm.name = 'tRNA_region';

--- ************************************************
--- *** relation: anticodon_loop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence of seven nucleotide bases in  ***
--- *** tRNA which contains the anticodon. It ha ***
--- *** s the sequence 5'-pyrimidine-purine-anti ***
--- *** codon-modified purine-any base-3.        ***
--- ************************************************
---

CREATE VIEW anticodon_loop AS
  SELECT
    feature_id AS anticodon_loop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'anticodon_loop';

--- ************************************************
--- *** relation: anticodon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence of three nucleotide bases in  ***
--- *** tRNA which recognizes a codon in mRNA.   ***
--- ************************************************
---

CREATE VIEW anticodon AS
  SELECT
    feature_id AS anticodon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'anticodon';

--- ************************************************
--- *** relation: cca_tail ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Base sequence at the 3' end of a tRNA. T ***
--- *** he 3'-hydroxyl group on the terminal ade ***
--- *** nosine is the attachment point for the a ***
--- *** mino acid.                               ***
--- ************************************************
---

CREATE VIEW cca_tail AS
  SELECT
    feature_id AS cca_tail_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'CCA_tail';

--- ************************************************
--- *** relation: dhu_loop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Non-base-paired sequence of nucleotide b ***
--- *** ases in tRNA. It contains several dihydr ***
--- *** ouracil residues.                        ***
--- ************************************************
---

CREATE VIEW dhu_loop AS
  SELECT
    feature_id AS dhu_loop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DHU_loop';

--- ************************************************
--- *** relation: t_loop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Non-base-paired sequence of three nucleo ***
--- *** tide bases in tRNA. It has sequence T-Ps ***
--- *** i-C.                                     ***
--- ************************************************
---

CREATE VIEW t_loop AS
  SELECT
    feature_id AS t_loop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'T_loop';

--- ************************************************
--- *** relation: pyrrolysine_trna_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript encoding pyrrolysyl ***
--- ***  tRNA (SO:0000766).                      ***
--- ************************************************
---

CREATE VIEW pyrrolysine_trna_primary_transcript AS
  SELECT
    feature_id AS pyrrolysine_trna_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pyrrolysine_tRNA_primary_transcript';

--- ************************************************
--- *** relation: u3_snorna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** U3 snoRNA is a member of the box C/D cla ***
--- *** ss of small nucleolar RNAs. The U3 snoRN ***
--- *** A secondary structure is characterised b ***
--- *** y a small 5' domain (with boxes A and A' ***
--- *** ), and a larger 3' domain (with boxes B, ***
--- ***  C, C', and D), the two domains being li ***
--- *** nked by a single-stranded hinge. Boxes B ***
--- ***  and C form the B/C motif, which appears ***
--- ***  to be exclusive to U3 snoRNAs, and boxe ***
--- *** s C' and D form the C'/D motif. The latt ***
--- *** er is functionally similar to the C/D mo ***
--- *** tifs found in other snoRNAs. The 5' doma ***
--- *** in and the hinge region act as a pre-rRN ***
--- *** A-binding domain. The 3' domain has cons ***
--- *** erved protein-binding sites. Both the bo ***
--- *** x B/C and box C'/D motifs are sufficient ***
--- ***  for nuclear retention of U3 snoRNA. The ***
--- ***  box C'/D motif is also necessary for nu ***
--- *** cleolar localization, stability and hype ***
--- *** rmethylation of U3 snoRNA. Both box B/C  ***
--- *** and C'/D motifs are involved in specific ***
--- ***  protein interactions and are necessary  ***
--- *** for the rRNA processing functions of U3  ***
--- *** snoRNA.                                  ***
--- ************************************************
---

CREATE VIEW u3_snorna AS
  SELECT
    feature_id AS u3_snorna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U3_snoRNA';

--- ************************************************
--- *** relation: au_rich_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cis-acting element found in the 3' UTR ***
--- ***  of some mRNA which is rich in AUUUA pen ***
--- *** tamers. Messenger RNAs bearing multiple  ***
--- *** AU-rich elements are often unstable.     ***
--- ************************************************
---

CREATE VIEW au_rich_element AS
  SELECT
    feature_id AS au_rich_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'AU_rich_element';

--- ************************************************
--- *** relation: bruno_response_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cis-acting element found in the 3' UTR ***
--- ***  of some mRNA which is bound by the Dros ***
--- *** ophila Bruno protein and its homologs.   ***
--- ************************************************
---

CREATE VIEW bruno_response_element AS
  SELECT
    feature_id AS bruno_response_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'Bruno_response_element';

--- ************************************************
--- *** relation: iron_responsive_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A regulatory sequence found in the 5' an ***
--- *** d 3' UTRs of many mRNAs which encode iro ***
--- *** n-binding proteins. It has a hairpin str ***
--- *** ucture and is recognized by trans-acting ***
--- ***  proteins known as iron-regulatory prote ***
--- *** ins.                                     ***
--- ************************************************
---

CREATE VIEW iron_responsive_element AS
  SELECT
    feature_id AS iron_responsive_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'iron_responsive_element';

--- ************************************************
--- *** relation: morpholino_backbone ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence compo ***
--- *** sed of nucleobases bound to a morpholino ***
--- ***  backbone. A morpholino backbone consist ***
--- *** s of morpholine (CHEBI:34856) rings conn ***
--- *** ected by phosphorodiamidate linkages.    ***
--- ************************************************
---

CREATE VIEW morpholino_backbone AS
  SELECT
    feature_id AS morpholino_backbone_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'morpholino_backbone';

--- ************************************************
--- *** relation: pna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence compo ***
--- *** sed of peptide nucleic acid (CHEBI:48021 ***
--- *** ), a chemical consisting of nucleobases  ***
--- *** bound to a backbone composed of repeatin ***
--- *** g N-(2-aminoethyl)-glycine units linked  ***
--- *** by peptide bonds. The purine and pyrimid ***
--- *** ine bases are linked to the backbone by  ***
--- *** methylene carbonyl bonds.                ***
--- ************************************************
---

CREATE VIEW pna AS
  SELECT
    feature_id AS pna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'PNA';

--- ************************************************
--- *** relation: enzymatic ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing the sequence of  ***
--- *** a transcript that has catalytic activity ***
--- ***  with or without an associated ribonucle ***
--- *** oprotein.                                ***
--- ************************************************
---

CREATE VIEW enzymatic AS
  SELECT
    feature_id AS enzymatic_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ribozymic' OR cvterm.name = 'enzymatic';

--- ************************************************
--- *** relation: ribozymic ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing the sequence of  ***
--- *** a transcript that has catalytic activity ***
--- ***  even without an associated ribonucleopr ***
--- *** otein.                                   ***
--- ************************************************
---

CREATE VIEW ribozymic AS
  SELECT
    feature_id AS ribozymic_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ribozymic';

--- ************************************************
--- *** relation: pseudouridylation_guide_snorna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A snoRNA that specifies the site of pseu ***
--- *** douridylation in an RNA molecule by base ***
--- ***  pairing with a short sequence around th ***
--- *** e target residue.                        ***
--- ************************************************
---

CREATE VIEW pseudouridylation_guide_snorna AS
  SELECT
    feature_id AS pseudouridylation_guide_snorna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pseudouridylation_guide_snoRNA';

--- ************************************************
--- *** relation: lna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence consi ***
--- *** sting of nucleobases attached to a repea ***
--- *** ting unit made of 'locked' deoxyribose r ***
--- *** ings connected to a phosphate backbone.  ***
--- *** The deoxyribose unit's conformation is ' ***
--- *** locked' by a 2'-C,4'-C-oxymethylene link ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW lna AS
  SELECT
    feature_id AS lna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'LNA';

--- ************************************************
--- *** relation: lna_oligo ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An oligo composed of LNA residues.       ***
--- ************************************************
---

CREATE VIEW lna_oligo AS
  SELECT
    feature_id AS lna_oligo_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'LNA_oligo';

--- ************************************************

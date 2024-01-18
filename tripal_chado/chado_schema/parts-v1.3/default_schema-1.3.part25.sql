SET search_path=so,chado,pg_catalog;
--- *** relation: n6_hydroxynorvalylcarbamoyladenosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** N6_hydroxynorvalylcarbamoyladenosine is  ***
--- *** a modified adenosine.                    ***
--- ************************************************
---

CREATE VIEW n6_hydroxynorvalylcarbamoyladenosine AS
  SELECT
    feature_id AS n6_hydroxynorvalylcarbamoyladenosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'N6_hydroxynorvalylcarbamoyladenosine';

--- ************************************************
--- *** relation: two_methylthio_n6_hydroxynorvalyl_carbamoyladenosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 2_methylthio_N6_hydroxynorvalyl_carbamoy ***
--- *** ladenosine is a modified adenosine.      ***
--- ************************************************
---

CREATE VIEW two_methylthio_n6_hydroxynorvalyl_carbamoyladenosine AS
  SELECT
    feature_id AS two_methylthio_n6_hydroxynorvalyl_carbamoyladenosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'two_methylthio_N6_hydroxynorvalyl_carbamoyladenosine';

--- ************************************************
--- *** relation: two_prime_o_riboA_phosphate ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 2prime_O_ribosyladenosine_phosphate is a ***
--- ***  modified adenosine.                     ***
--- ************************************************
---

CREATE VIEW two_prime_o_riboA_phosphate AS
  SELECT
    feature_id AS two_prime_o_riboA_phosphate_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'two_prime_O_ribosyladenosine_phosphate';

--- ************************************************
--- *** relation: n6_n6_dimethyladenosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** N6_N6_dimethyladenosine is a modified ad ***
--- *** enosine.                                 ***
--- ************************************************
---

CREATE VIEW n6_n6_dimethyladenosine AS
  SELECT
    feature_id AS n6_n6_dimethyladenosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'N6_N6_dimethyladenosine';

--- ************************************************
--- *** relation: n6_2_prime_o_dimethyladenosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** N6_2prime_O_dimethyladenosine is a modif ***
--- *** ied adenosine.                           ***
--- ************************************************
---

CREATE VIEW n6_2_prime_o_dimethyladenosine AS
  SELECT
    feature_id AS n6_2_prime_o_dimethyladenosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'N6_2_prime_O_dimethyladenosine';

--- ************************************************
--- *** relation: n6_n6_2_prime_o_trimethyladenosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** N6_N6_2prime_O_trimethyladenosine is a m ***
--- *** odified adenosine.                       ***
--- ************************************************
---

CREATE VIEW n6_n6_2_prime_o_trimethyladenosine AS
  SELECT
    feature_id AS n6_n6_2_prime_o_trimethyladenosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'N6_N6_2_prime_O_trimethyladenosine';

--- ************************************************
--- *** relation: one_two_prime_o_dimethyladenosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 1,2'-O-dimethyladenosine is a modified a ***
--- *** denosine.                                ***
--- ************************************************
---

CREATE VIEW one_two_prime_o_dimethyladenosine AS
  SELECT
    feature_id AS one_two_prime_o_dimethyladenosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'one_two_prime_O_dimethyladenosine';

--- ************************************************
--- *** relation: n6_acetyladenosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** N6_acetyladenosine is a modified adenosi ***
--- *** ne.                                      ***
--- ************************************************
---

CREATE VIEW n6_acetyladenosine AS
  SELECT
    feature_id AS n6_acetyladenosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'N6_acetyladenosine';

--- ************************************************
--- *** relation: seven_deazaguanosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 7-deazaguanosine is a moddified guanosin ***
--- *** e.                                       ***
--- ************************************************
---

CREATE VIEW seven_deazaguanosine AS
  SELECT
    feature_id AS seven_deazaguanosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'queuosine' OR cvterm.name = 'epoxyqueuosine' OR cvterm.name = 'galactosyl_queuosine' OR cvterm.name = 'mannosyl_queuosine' OR cvterm.name = 'seven_cyano_seven_deazaguanosine' OR cvterm.name = 'seven_aminomethyl_seven_deazaguanosine' OR cvterm.name = 'archaeosine' OR cvterm.name = 'seven_deazaguanosine';

--- ************************************************
--- *** relation: queuosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Queuosine is a modified 7-deazoguanosine ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW queuosine AS
  SELECT
    feature_id AS queuosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'queuosine';

--- ************************************************
--- *** relation: epoxyqueuosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Epoxyqueuosine is a modified 7-deazoguan ***
--- *** osine.                                   ***
--- ************************************************
---

CREATE VIEW epoxyqueuosine AS
  SELECT
    feature_id AS epoxyqueuosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'epoxyqueuosine';

--- ************************************************
--- *** relation: galactosyl_queuosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Galactosyl_queuosine is a modified 7-dea ***
--- *** zoguanosine.                             ***
--- ************************************************
---

CREATE VIEW galactosyl_queuosine AS
  SELECT
    feature_id AS galactosyl_queuosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'galactosyl_queuosine';

--- ************************************************
--- *** relation: mannosyl_queuosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Mannosyl_queuosine is a modified 7-deazo ***
--- *** guanosine.                               ***
--- ************************************************
---

CREATE VIEW mannosyl_queuosine AS
  SELECT
    feature_id AS mannosyl_queuosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mannosyl_queuosine';

--- ************************************************
--- *** relation: seven_cyano_seven_deazaguanosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 7_cyano_7_deazaguanosine is a modified 7 ***
--- *** -deazoguanosine.                         ***
--- ************************************************
---

CREATE VIEW seven_cyano_seven_deazaguanosine AS
  SELECT
    feature_id AS seven_cyano_seven_deazaguanosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'seven_cyano_seven_deazaguanosine';

--- ************************************************
--- *** relation: seven_aminomethyl_seven_deazaguanosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 7_aminomethyl_7_deazaguanosine is a modi ***
--- *** fied 7-deazoguanosine.                   ***
--- ************************************************
---

CREATE VIEW seven_aminomethyl_seven_deazaguanosine AS
  SELECT
    feature_id AS seven_aminomethyl_seven_deazaguanosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'seven_aminomethyl_seven_deazaguanosine';

--- ************************************************
--- *** relation: archaeosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Archaeosine is a modified 7-deazoguanosi ***
--- *** ne.                                      ***
--- ************************************************
---

CREATE VIEW archaeosine AS
  SELECT
    feature_id AS archaeosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'archaeosine';

--- ************************************************
--- *** relation: one_methylguanosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 1_methylguanosine is a modified guanosin ***
--- *** e base feature.                          ***
--- ************************************************
---

CREATE VIEW one_methylguanosine AS
  SELECT
    feature_id AS one_methylguanosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'one_methylguanosine';

--- ************************************************
--- *** relation: n2_methylguanosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** N2_methylguanosine is a modified guanosi ***
--- *** ne base feature.                         ***
--- ************************************************
---

CREATE VIEW n2_methylguanosine AS
  SELECT
    feature_id AS n2_methylguanosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'N2_methylguanosine';

--- ************************************************
--- *** relation: seven_methylguanosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 7_methylguanosine is a modified guanosin ***
--- *** e base feature.                          ***
--- ************************************************
---

CREATE VIEW seven_methylguanosine AS
  SELECT
    feature_id AS seven_methylguanosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'seven_methylguanosine';

--- ************************************************
--- *** relation: two_prime_o_methylguanosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 2prime_O_methylguanosine is a modified g ***
--- *** uanosine base feature.                   ***
--- ************************************************
---

CREATE VIEW two_prime_o_methylguanosine AS
  SELECT
    feature_id AS two_prime_o_methylguanosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'two_prime_O_methylguanosine';

--- ************************************************
--- *** relation: n2_n2_dimethylguanosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** N2_N2_dimethylguanosine is a modified gu ***
--- *** anosine base feature.                    ***
--- ************************************************
---

CREATE VIEW n2_n2_dimethylguanosine AS
  SELECT
    feature_id AS n2_n2_dimethylguanosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'N2_N2_dimethylguanosine';

--- ************************************************
--- *** relation: n2_2_prime_o_dimethylguanosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** N2_2prime_O_dimethylguanosine is a modif ***
--- *** ied guanosine base feature.              ***
--- ************************************************
---

CREATE VIEW n2_2_prime_o_dimethylguanosine AS
  SELECT
    feature_id AS n2_2_prime_o_dimethylguanosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'N2_2_prime_O_dimethylguanosine';

--- ************************************************
--- *** relation: n2_n2_2_prime_o_trimethylguanosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** N2_N2_2prime_O_trimethylguanosine is a m ***
--- *** odified guanosine base feature.          ***
--- ************************************************
---

CREATE VIEW n2_n2_2_prime_o_trimethylguanosine AS
  SELECT
    feature_id AS n2_n2_2_prime_o_trimethylguanosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'N2_N2_2_prime_O_trimethylguanosine';

--- ************************************************
--- *** relation: two_prime_o_ribosylguanosine_phosphate ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 2prime_O_ribosylguanosine_phosphate is a ***
--- ***  modified guanosine base feature.        ***
--- ************************************************
---

CREATE VIEW two_prime_o_ribosylguanosine_phosphate AS
  SELECT
    feature_id AS two_prime_o_ribosylguanosine_phosphate_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'two_prime_O_ribosylguanosine_phosphate';

--- ************************************************
--- *** relation: wybutosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Wybutosine is a modified guanosine base  ***
--- *** feature.                                 ***
--- ************************************************
---

CREATE VIEW wybutosine AS
  SELECT
    feature_id AS wybutosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'wybutosine';

--- ************************************************
--- *** relation: peroxywybutosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Peroxywybutosine is a modified guanosine ***
--- ***  base feature.                           ***
--- ************************************************
---

CREATE VIEW peroxywybutosine AS
  SELECT
    feature_id AS peroxywybutosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'peroxywybutosine';

--- ************************************************
--- *** relation: hydroxywybutosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Hydroxywybutosine is a modified guanosin ***
--- *** e base feature.                          ***
--- ************************************************
---

CREATE VIEW hydroxywybutosine AS
  SELECT
    feature_id AS hydroxywybutosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'hydroxywybutosine';

--- ************************************************
--- *** relation: undermodified_hydroxywybutosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Undermodified_hydroxywybutosine is a mod ***
--- *** ified guanosine base feature.            ***
--- ************************************************
---

CREATE VIEW undermodified_hydroxywybutosine AS
  SELECT
    feature_id AS undermodified_hydroxywybutosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'undermodified_hydroxywybutosine';

--- ************************************************
--- *** relation: wyosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Wyosine is a modified guanosine base fea ***
--- *** ture.                                    ***
--- ************************************************
---

CREATE VIEW wyosine AS
  SELECT
    feature_id AS wyosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'wyosine';

--- ************************************************
--- *** relation: methylwyosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Methylwyosine is a modified guanosine ba ***
--- *** se feature.                              ***
--- ************************************************
---

CREATE VIEW methylwyosine AS
  SELECT
    feature_id AS methylwyosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'methylwyosine';

--- ************************************************
--- *** relation: n2_7_dimethylguanosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** N2_7_dimethylguanosine is a modified gua ***
--- *** nosine base feature.                     ***
--- ************************************************
---

CREATE VIEW n2_7_dimethylguanosine AS
  SELECT
    feature_id AS n2_7_dimethylguanosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'N2_7_dimethylguanosine';

--- ************************************************
--- *** relation: n2_n2_7_trimethylguanosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** N2_N2_7_trimethylguanosine is a modified ***
--- ***  guanosine base feature.                 ***
--- ************************************************
---

CREATE VIEW n2_n2_7_trimethylguanosine AS
  SELECT
    feature_id AS n2_n2_7_trimethylguanosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'N2_N2_7_trimethylguanosine';

--- ************************************************
--- *** relation: one_two_prime_o_dimethylguanosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 1_2prime_O_dimethylguanosine is a modifi ***
--- *** ed guanosine base feature.               ***
--- ************************************************
---

CREATE VIEW one_two_prime_o_dimethylguanosine AS
  SELECT
    feature_id AS one_two_prime_o_dimethylguanosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'one_two_prime_O_dimethylguanosine';

--- ************************************************
--- *** relation: four_demethylwyosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 4_demethylwyosine is a modified guanosin ***
--- *** e base feature.                          ***
--- ************************************************
---

CREATE VIEW four_demethylwyosine AS
  SELECT
    feature_id AS four_demethylwyosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'four_demethylwyosine';

--- ************************************************
--- *** relation: isowyosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Isowyosine is a modified guanosine base  ***
--- *** feature.                                 ***
--- ************************************************
---

CREATE VIEW isowyosine AS
  SELECT
    feature_id AS isowyosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'isowyosine';

--- ************************************************
--- *** relation: n2_7_2prirme_o_trimethylguanosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** N2_7_2prirme_O_trimethylguanosine is a m ***
--- *** odified guanosine base feature.          ***
--- ************************************************
---

CREATE VIEW n2_7_2prirme_o_trimethylguanosine AS
  SELECT
    feature_id AS n2_7_2prirme_o_trimethylguanosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'N2_7_2prirme_O_trimethylguanosine';

--- ************************************************
--- *** relation: five_methyluridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 5_methyluridine is a modified uridine ba ***
--- *** se feature.                              ***
--- ************************************************
---

CREATE VIEW five_methyluridine AS
  SELECT
    feature_id AS five_methyluridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_methyluridine';

--- ************************************************
--- *** relation: two_prime_o_methyluridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 2prime_O_methyluridine is a modified uri ***
--- *** dine base feature.                       ***
--- ************************************************
---

CREATE VIEW two_prime_o_methyluridine AS
  SELECT
    feature_id AS two_prime_o_methyluridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'two_prime_O_methyluridine';

--- ************************************************
--- *** relation: five_two_prime_o_dimethyluridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 5_2_prime_O_dimethyluridine is a modifie ***
--- *** d uridine base feature.                  ***
--- ************************************************
---

CREATE VIEW five_two_prime_o_dimethyluridine AS
  SELECT
    feature_id AS five_two_prime_o_dimethyluridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_two_prime_O_dimethyluridine';

--- ************************************************
--- *** relation: one_methylpseudouridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 1_methylpseudouridine is a modified urid ***
--- *** ine base feature.                        ***
--- ************************************************
---

CREATE VIEW one_methylpseudouridine AS
  SELECT
    feature_id AS one_methylpseudouridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'one_methylpseudouridine';

--- ************************************************
--- *** relation: two_prime_o_methylpseudouridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 2prime_O_methylpseudouridine is a modifi ***
--- *** ed uridine base feature.                 ***
--- ************************************************
---

CREATE VIEW two_prime_o_methylpseudouridine AS
  SELECT
    feature_id AS two_prime_o_methylpseudouridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'two_prime_O_methylpseudouridine';

--- ************************************************
--- *** relation: two_thiouridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 2_thiouridine is a modified uridine base ***
--- ***  feature.                                ***
--- ************************************************
---

CREATE VIEW two_thiouridine AS
  SELECT
    feature_id AS two_thiouridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'two_thiouridine';

--- ************************************************
--- *** relation: four_thiouridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 4_thiouridine is a modified uridine base ***
--- ***  feature.                                ***
--- ************************************************
---

CREATE VIEW four_thiouridine AS
  SELECT
    feature_id AS four_thiouridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'four_thiouridine';

--- ************************************************
--- *** relation: five_methyl_2_thiouridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 5_methyl_2_thiouridine is a modified uri ***
--- *** dine base feature.                       ***
--- ************************************************
---

CREATE VIEW five_methyl_2_thiouridine AS
  SELECT
    feature_id AS five_methyl_2_thiouridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_methyl_2_thiouridine';

--- ************************************************
--- *** relation: two_thio_two_prime_o_methyluridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 2_thio_2prime_O_methyluridine is a modif ***
--- *** ied uridine base feature.                ***
--- ************************************************
---

CREATE VIEW two_thio_two_prime_o_methyluridine AS
  SELECT
    feature_id AS two_thio_two_prime_o_methyluridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'two_thio_two_prime_O_methyluridine';

--- ************************************************
--- *** relation: three_three_amino_three_carboxypropyl_uridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 3_3_amino_3_carboxypropyl_uridine is a m ***
--- *** odified uridine base feature.            ***
--- ************************************************
---

CREATE VIEW three_three_amino_three_carboxypropyl_uridine AS
  SELECT
    feature_id AS three_three_amino_three_carboxypropyl_uridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_three_amino_three_carboxypropyl_uridine';

--- ************************************************
--- *** relation: five_hydroxyuridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 5_hydroxyuridine is a modified uridine b ***
--- *** ase feature.                             ***
--- ************************************************
---

CREATE VIEW five_hydroxyuridine AS
  SELECT
    feature_id AS five_hydroxyuridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_hydroxyuridine';

--- ************************************************
--- *** relation: five_methoxyuridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 5_methoxyuridine is a modified uridine b ***
--- *** ase feature.                             ***
--- ************************************************
---

CREATE VIEW five_methoxyuridine AS
  SELECT
    feature_id AS five_methoxyuridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_methoxyuridine';

--- ************************************************
--- *** relation: uridine_five_oxyacetic_acid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Uridine_5_oxyacetic_acid is a modified u ***
--- *** ridine base feature.                     ***
--- ************************************************
---

CREATE VIEW uridine_five_oxyacetic_acid AS
  SELECT
    feature_id AS uridine_five_oxyacetic_acid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'uridine_five_oxyacetic_acid';

--- ************************************************
--- *** relation: uridine_five_oxyacetic_acid_methyl_ester ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Uridine_5_oxyacetic_acid_methyl_ester is ***
--- ***  a modified uridine base feature.        ***
--- ************************************************
---

CREATE VIEW uridine_five_oxyacetic_acid_methyl_ester AS
  SELECT
    feature_id AS uridine_five_oxyacetic_acid_methyl_ester_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'uridine_five_oxyacetic_acid_methyl_ester';

--- ************************************************
--- *** relation: five_carboxyhydroxymethyl_uridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 5_carboxyhydroxymethyl_uridine is a modi ***
--- *** fied uridine base feature.               ***
--- ************************************************
---

CREATE VIEW five_carboxyhydroxymethyl_uridine AS
  SELECT
    feature_id AS five_carboxyhydroxymethyl_uridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_carboxyhydroxymethyl_uridine';

--- ************************************************
--- *** relation: five_carboxyhydroxymethyl_uridine_methyl_ester ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 5_carboxyhydroxymethyl_uridine_methyl_es ***
--- *** ter is a modified uridine base feature.  ***
--- ************************************************
---

CREATE VIEW five_carboxyhydroxymethyl_uridine_methyl_ester AS
  SELECT
    feature_id AS five_carboxyhydroxymethyl_uridine_methyl_ester_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_carboxyhydroxymethyl_uridine_methyl_ester';

--- ************************************************
--- *** relation: five_methoxycarbonylmethyluridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Five_methoxycarbonylmethyluridine is a m ***
--- *** odified uridine base feature.            ***
--- ************************************************
---

CREATE VIEW five_methoxycarbonylmethyluridine AS
  SELECT
    feature_id AS five_methoxycarbonylmethyluridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_methoxycarbonylmethyluridine';

--- ************************************************
--- *** relation: five_methoxycarbonylmethyl_two_prime_o_methyluridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Five_methoxycarbonylmethyl_2_prime_O_met ***
--- *** hyluridine is a modified uridine base fe ***
--- *** ature.                                   ***
--- ************************************************
---

CREATE VIEW five_methoxycarbonylmethyl_two_prime_o_methyluridine AS
  SELECT
    feature_id AS five_methoxycarbonylmethyl_two_prime_o_methyluridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_methoxycarbonylmethyl_two_prime_O_methyluridine';

--- ************************************************
--- *** relation: five_mcm_2_thiouridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 5_methoxycarbonylmethyl_2_thiouridine is ***
--- ***  a modified uridine base feature.        ***
--- ************************************************
---

CREATE VIEW five_mcm_2_thiouridine AS
  SELECT
    feature_id AS five_mcm_2_thiouridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_methoxycarbonylmethyl_two_thiouridine';

--- ************************************************
--- *** relation: five_aminomethyl_two_thiouridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 5_aminomethyl_2_thiouridine is a modifie ***
--- *** d uridine base feature.                  ***
--- ************************************************
---

CREATE VIEW five_aminomethyl_two_thiouridine AS
  SELECT
    feature_id AS five_aminomethyl_two_thiouridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_aminomethyl_two_thiouridine';

--- ************************************************
--- *** relation: five_methylaminomethyluridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 5_methylaminomethyluridine is a modified ***
--- ***  uridine base feature.                   ***
--- ************************************************
---

CREATE VIEW five_methylaminomethyluridine AS
  SELECT
    feature_id AS five_methylaminomethyluridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_methylaminomethyluridine';

--- ************************************************
--- *** relation: five_mam_2_thiouridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 5_methylaminomethyl_2_thiouridine is a m ***
--- *** odified uridine base feature.            ***
--- ************************************************
---

CREATE VIEW five_mam_2_thiouridine AS
  SELECT
    feature_id AS five_mam_2_thiouridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_methylaminomethyl_two_thiouridine';

--- ************************************************
--- *** relation: five_methylaminomethyl_two_selenouridine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 5_methylaminomethyl_2_selenouridine is a ***
--- ***  modified uridine base feature.          ***
--- ************************************************
---

CREATE VIEW five_methylaminomethyl_two_selenouridine AS
  SELECT
    feature_id AS five_methylaminomethyl_two_selenouridine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_methylaminomethyl_two_selenouridine';

--- ************************************************

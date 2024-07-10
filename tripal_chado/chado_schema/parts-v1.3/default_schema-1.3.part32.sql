SET search_path=so,chado,pg_catalog;
--- *** relation: histone_ubiqitination_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A histone modification site where ubiqui ***
--- *** tin may be added.                        ***
--- ************************************************
---

CREATE VIEW histone_ubiqitination_site AS
  SELECT
    feature_id AS histone_ubiqitination_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H2B_ubiquitination_site' OR cvterm.name = 'histone_ubiqitination_site';

--- ************************************************
--- *** relation: h2b_ubiquitination_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A histone modification site on H2B where ***
--- ***  ubiquitin may be added.                 ***
--- ************************************************
---

CREATE VIEW h2b_ubiquitination_site AS
  SELECT
    feature_id AS h2b_ubiquitination_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H2B_ubiquitination_site';

--- ************************************************
--- *** relation: h3k18_acetylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 14th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is ac ***
--- *** ylated.                                  ***
--- ************************************************
---

CREATE VIEW h3k18_acetylation_site AS
  SELECT
    feature_id AS h3k18_acetylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K18_acetylation_site';

--- ************************************************
--- *** relation: h3k23_acylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification, whereby  ***
--- *** the 23rd residue (a lysine), from the st ***
--- *** art of the H3 histone protein is acylate ***
--- *** d.                                       ***
--- ************************************************
---

CREATE VIEW h3k23_acylation_site AS
  SELECT
    feature_id AS h3k23_acylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K23_acylation site';

--- ************************************************
--- *** relation: epigenetically_modified_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A biological region implicated in inheri ***
--- *** ted changes caused by mechanisms other t ***
--- *** han changes in the underlying DNA sequen ***
--- *** ce.                                      ***
--- ************************************************
---

CREATE VIEW epigenetically_modified_region AS
  SELECT
    feature_id AS epigenetically_modified_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'modified_base' OR cvterm.name = 'epigenetically_modified_gene' OR cvterm.name = 'histone_modification' OR cvterm.name = 'methylated_base_feature' OR cvterm.name = 'methylated_C' OR cvterm.name = 'methylated_A' OR cvterm.name = 'gene_rearranged_at_DNA_level' OR cvterm.name = 'maternally_imprinted_gene' OR cvterm.name = 'paternally_imprinted_gene' OR cvterm.name = 'allelically_excluded_gene' OR cvterm.name = 'histone_methylation_site' OR cvterm.name = 'histone_acetylation_site' OR cvterm.name = 'histone_ubiqitination_site' OR cvterm.name = 'histone_acylation_region' OR cvterm.name = 'H4K20_monomethylation_site' OR cvterm.name = 'H2BK5_monomethylation_site' OR cvterm.name = 'H3K27_methylation_site' OR cvterm.name = 'H3K36_methylation_site' OR cvterm.name = 'H3K4_methylation_site' OR cvterm.name = 'H3K79_methylation_site' OR cvterm.name = 'H3K9_methylation_site' OR cvterm.name = 'H3K27_monomethylation_site' OR cvterm.name = 'H3K27_trimethylation_site' OR cvterm.name = 'H3K27_dimethylation_site' OR cvterm.name = 'H3K36_monomethylation_site' OR cvterm.name = 'H3K36_dimethylation_site' OR cvterm.name = 'H3K36_trimethylation_site' OR cvterm.name = 'H3K4_monomethylation_site' OR cvterm.name = 'H3K4_trimethylation' OR cvterm.name = 'H3K4_dimethylation_site' OR cvterm.name = 'H3K79_monomethylation_site' OR cvterm.name = 'H3K79_dimethylation_site' OR cvterm.name = 'H3K79_trimethylation_site' OR cvterm.name = 'H3K9_trimethylation_site' OR cvterm.name = 'H3K9_monomethylation_site' OR cvterm.name = 'H3K9_dimethylation_site' OR cvterm.name = 'H3K9_acetylation_site' OR cvterm.name = 'H3K14_acetylation_site' OR cvterm.name = 'H3K18_acetylation_site' OR cvterm.name = 'H3K23_acylation site' OR cvterm.name = 'H3K27_acylation_site' OR cvterm.name = 'H4K16_acylation_site' OR cvterm.name = 'H4K5_acylation_site' OR cvterm.name = 'H4K8_acylation site' OR cvterm.name = 'H2B_ubiquitination_site' OR cvterm.name = 'H4K_acylation_region' OR cvterm.name = 'epigenetically_modified_region';

--- ************************************************
--- *** relation: h3k27_acylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 27th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is ac ***
--- *** ylated.                                  ***
--- ************************************************
---

CREATE VIEW h3k27_acylation_site AS
  SELECT
    feature_id AS h3k27_acylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K27_acylation_site';

--- ************************************************
--- *** relation: h3k36_monomethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 36th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is mo ***
--- *** no-methylated.                           ***
--- ************************************************
---

CREATE VIEW h3k36_monomethylation_site AS
  SELECT
    feature_id AS h3k36_monomethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K36_monomethylation_site';

--- ************************************************
--- *** relation: h3k36_dimethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 36th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is di ***
--- *** methylated.                              ***
--- ************************************************
---

CREATE VIEW h3k36_dimethylation_site AS
  SELECT
    feature_id AS h3k36_dimethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K36_dimethylation_site';

--- ************************************************
--- *** relation: h3k36_trimethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 36th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is tr ***
--- *** i-methylated.                            ***
--- ************************************************
---

CREATE VIEW h3k36_trimethylation_site AS
  SELECT
    feature_id AS h3k36_trimethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K36_trimethylation_site';

--- ************************************************
--- *** relation: h3k4_dimethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 4th residue (a lysine), from th ***
--- *** e start of the H3 histone protein is di- ***
--- *** methylated.                              ***
--- ************************************************
---

CREATE VIEW h3k4_dimethylation_site AS
  SELECT
    feature_id AS h3k4_dimethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K4_dimethylation_site';

--- ************************************************
--- *** relation: h3k27_dimethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 27th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is di ***
--- *** -methylated.                             ***
--- ************************************************
---

CREATE VIEW h3k27_dimethylation_site AS
  SELECT
    feature_id AS h3k27_dimethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K27_dimethylation_site';

--- ************************************************
--- *** relation: h3k9_monomethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 9th residue (a lysine), from th ***
--- *** e start of the H3 histone protein is mon ***
--- *** o-methylated.                            ***
--- ************************************************
---

CREATE VIEW h3k9_monomethylation_site AS
  SELECT
    feature_id AS h3k9_monomethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K9_monomethylation_site';

--- ************************************************
--- *** relation: h3k9_dimethylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 9th residue (a lysine), from th ***
--- *** e start of the H3 histone protein may be ***
--- ***  dimethylated.                           ***
--- ************************************************
---

CREATE VIEW h3k9_dimethylation_site AS
  SELECT
    feature_id AS h3k9_dimethylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K9_dimethylation_site';

--- ************************************************
--- *** relation: h4k16_acylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 16th residue (a lysine), from t ***
--- *** he start of the H4 histone protein is ac ***
--- *** ylated.                                  ***
--- ************************************************
---

CREATE VIEW h4k16_acylation_site AS
  SELECT
    feature_id AS h4k16_acylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H4K16_acylation_site';

--- ************************************************
--- *** relation: h4k5_acylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 5th residue (a lysine), from th ***
--- *** e start of the H4 histone protein is acy ***
--- *** lated.                                   ***
--- ************************************************
---

CREATE VIEW h4k5_acylation_site AS
  SELECT
    feature_id AS h4k5_acylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H4K5_acylation_site';

--- ************************************************
--- *** relation: h4k8_acylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 8th residue (a lysine), from th ***
--- *** e start of the H4 histone protein is acy ***
--- *** lated.                                   ***
--- ************************************************
---

CREATE VIEW h4k8_acylation_site AS
  SELECT
    feature_id AS h4k8_acylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H4K8_acylation site';

--- ************************************************
--- *** relation: h3k27_methylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 27th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is me ***
--- *** thylated.                                ***
--- ************************************************
---

CREATE VIEW h3k27_methylation_site AS
  SELECT
    feature_id AS h3k27_methylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K27_monomethylation_site' OR cvterm.name = 'H3K27_trimethylation_site' OR cvterm.name = 'H3K27_dimethylation_site' OR cvterm.name = 'H3K27_methylation_site';

--- ************************************************
--- *** relation: h3k36_methylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 36th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is me ***
--- *** thylated.                                ***
--- ************************************************
---

CREATE VIEW h3k36_methylation_site AS
  SELECT
    feature_id AS h3k36_methylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K36_monomethylation_site' OR cvterm.name = 'H3K36_dimethylation_site' OR cvterm.name = 'H3K36_trimethylation_site' OR cvterm.name = 'H3K36_methylation_site';

--- ************************************************
--- *** relation: h3k4_methylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification, whereby  ***
--- *** the 4th residue (a lysine), from the sta ***
--- *** rt of the H3 protein is methylated.      ***
--- ************************************************
---

CREATE VIEW h3k4_methylation_site AS
  SELECT
    feature_id AS h3k4_methylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K4_monomethylation_site' OR cvterm.name = 'H3K4_trimethylation' OR cvterm.name = 'H3K4_dimethylation_site' OR cvterm.name = 'H3K4_methylation_site';

--- ************************************************
--- *** relation: h3k79_methylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 79th residue (a lysine), from t ***
--- *** he start of the H3 histone protein is me ***
--- *** thylated.                                ***
--- ************************************************
---

CREATE VIEW h3k79_methylation_site AS
  SELECT
    feature_id AS h3k79_methylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K79_monomethylation_site' OR cvterm.name = 'H3K79_dimethylation_site' OR cvterm.name = 'H3K79_trimethylation_site' OR cvterm.name = 'H3K79_methylation_site';

--- ************************************************
--- *** relation: h3k9_methylation_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of histone modification site, whe ***
--- *** reby the 9th residue (a lysine), from th ***
--- *** e start of the H3 histone protein is met ***
--- *** hylated.                                 ***
--- ************************************************
---

CREATE VIEW h3k9_methylation_site AS
  SELECT
    feature_id AS h3k9_methylation_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H3K9_trimethylation_site' OR cvterm.name = 'H3K9_monomethylation_site' OR cvterm.name = 'H3K9_dimethylation_site' OR cvterm.name = 'H3K9_methylation_site';

--- ************************************************
--- *** relation: histone_acylation_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A histone modification, whereby the hist ***
--- *** one protein is acylated at multiple site ***
--- *** s in a region.                           ***
--- ************************************************
---

CREATE VIEW histone_acylation_region AS
  SELECT
    feature_id AS histone_acylation_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H4K_acylation_region' OR cvterm.name = 'histone_acylation_region';

--- ************************************************
--- *** relation: h4k_acylation_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of the H4 histone whereby multi ***
--- *** ple lysines are acylated.                ***
--- ************************************************
---

CREATE VIEW h4k_acylation_region AS
  SELECT
    feature_id AS h4k_acylation_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'H4K_acylation_region';

--- ************************************************
--- *** relation: gene_with_non_canonical_start_codon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene with a start codon other than AUG ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW gene_with_non_canonical_start_codon AS
  SELECT
    feature_id AS gene_with_non_canonical_start_codon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_start_codon_CUG' OR cvterm.name = 'gene_with_non_canonical_start_codon';

--- ************************************************
--- *** relation: gene_with_start_codon_cug ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene with a translational start codon  ***
--- *** of CUG.                                  ***
--- ************************************************
---

CREATE VIEW gene_with_start_codon_cug AS
  SELECT
    feature_id AS gene_with_start_codon_cug_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_start_codon_CUG';

--- ************************************************
--- *** relation: pseudogenic_gene_segment ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene segment which when incorporated b ***
--- *** y somatic recombination in the final gen ***
--- *** e transcript results in a nonfunctional  ***
--- *** product.                                 ***
--- ************************************************
---

CREATE VIEW pseudogenic_gene_segment AS
  SELECT
    feature_id AS pseudogenic_gene_segment_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pseudogenic_gene_segment';

--- ************************************************
--- *** relation: copy_number_gain ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence alteration whereby the copy n ***
--- *** umber of a given regions is greater than ***
--- ***  the reference sequence.                 ***
--- ************************************************
---

CREATE VIEW copy_number_gain AS
  SELECT
    feature_id AS copy_number_gain_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'copy_number_gain';

--- ************************************************
--- *** relation: copy_number_loss ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence alteration whereby the copy n ***
--- *** umber of a given region is less than the ***
--- ***  reference sequence.                     ***
--- ************************************************
---

CREATE VIEW copy_number_loss AS
  SELECT
    feature_id AS copy_number_loss_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'copy_number_loss';

--- ************************************************
--- *** relation: upd ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Uniparental disomy is a sequence_alterat ***
--- *** ion where a diploid individual receives  ***
--- *** two copies for all or part of a chromoso ***
--- *** me from one parent and no copies of the  ***
--- *** same chromosome or region from the other ***
--- ***  parent.                                 ***
--- ************************************************
---

CREATE VIEW upd AS
  SELECT
    feature_id AS upd_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'maternal_uniparental_disomy' OR cvterm.name = 'paternal_uniparental_disomy' OR cvterm.name = 'UPD';

--- ************************************************
--- *** relation: maternal_uniparental_disomy ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Uniparental disomy is a sequence_alterat ***
--- *** ion where a diploid individual receives  ***
--- *** two copies for all or part of a chromoso ***
--- *** me from the mother and no copies of the  ***
--- *** same chromosome or region from the fathe ***
--- *** r.                                       ***
--- ************************************************
---

CREATE VIEW maternal_uniparental_disomy AS
  SELECT
    feature_id AS maternal_uniparental_disomy_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'maternal_uniparental_disomy';

--- ************************************************
--- *** relation: paternal_uniparental_disomy ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Uniparental disomy is a sequence_alterat ***
--- *** ion where a diploid individual receives  ***
--- *** two copies for all or part of a chromoso ***
--- *** me from the father and no copies of the  ***
--- *** same chromosome or region from the mothe ***
--- *** r.                                       ***
--- ************************************************
---

CREATE VIEW paternal_uniparental_disomy AS
  SELECT
    feature_id AS paternal_uniparental_disomy_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'paternal_uniparental_disomy';

--- ************************************************
--- *** relation: open_chromatin_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A DNA sequence that in the normal state  ***
--- *** of the chromosome corresponds to an unfo ***
--- *** lded, un-complexed stretch of double-str ***
--- *** anded DNA.                               ***
--- ************************************************
---

CREATE VIEW open_chromatin_region AS
  SELECT
    feature_id AS open_chromatin_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'open_chromatin_region';

--- ************************************************
--- *** relation: sl3_acceptor_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A SL2_acceptor_site which appends the SL ***
--- *** 3 RNA leader sequence to the 5' end of a ***
--- *** n mRNA. SL3 acceptor sites occur in gene ***
--- *** s in internal segments of polycistronic  ***
--- *** transcripts.                             ***
--- ************************************************
---

CREATE VIEW sl3_acceptor_site AS
  SELECT
    feature_id AS sl3_acceptor_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SL3_acceptor_site';

--- ************************************************
--- *** relation: sl4_acceptor_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A SL2_acceptor_site which appends the SL ***
--- *** 4 RNA leader sequence to the 5' end of a ***
--- *** n mRNA. SL4 acceptor sites occur in gene ***
--- *** s in internal segments of polycistronic  ***
--- *** transcripts.                             ***
--- ************************************************
---

CREATE VIEW sl4_acceptor_site AS
  SELECT
    feature_id AS sl4_acceptor_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SL4_acceptor_site';

--- ************************************************
--- *** relation: sl5_acceptor_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A SL2_acceptor_site which appends the SL ***
--- *** 5 RNA leader sequence to the 5' end of a ***
--- *** n mRNA. SL5 acceptor sites occur in gene ***
--- *** s in internal segments of polycistronic  ***
--- *** transcripts.                             ***
--- ************************************************
---

CREATE VIEW sl5_acceptor_site AS
  SELECT
    feature_id AS sl5_acceptor_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SL5_acceptor_site';

--- ************************************************
--- *** relation: sl6_acceptor_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A SL2_acceptor_site which appends the SL ***
--- *** 6 RNA leader sequence to the 5' end of a ***
--- *** n mRNA. SL6 acceptor sites occur in gene ***
--- *** s in internal segments of polycistronic  ***
--- *** transcripts.                             ***
--- ************************************************
---

CREATE VIEW sl6_acceptor_site AS
  SELECT
    feature_id AS sl6_acceptor_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SL6_acceptor_site';

--- ************************************************
--- *** relation: sl7_acceptor_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A SL2_acceptor_site which appends the SL ***
--- *** 7 RNA leader sequence to the 5' end of a ***
--- *** n mRNA. SL7 acceptor sites occur in gene ***
--- *** s in internal segments of polycistronic  ***
--- *** transcripts.                             ***
--- ************************************************
---

CREATE VIEW sl7_acceptor_site AS
  SELECT
    feature_id AS sl7_acceptor_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SL7_acceptor_site';

--- ************************************************
--- *** relation: sl8_acceptor_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A SL2_acceptor_site which appends the SL ***
--- *** 8 RNA leader sequence to the 5' end of a ***
--- *** n mRNA. SL8 acceptor sites occur in gene ***
--- *** s in internal segments of polycistronic  ***
--- *** transcripts.                             ***
--- ************************************************
---

CREATE VIEW sl8_acceptor_site AS
  SELECT
    feature_id AS sl8_acceptor_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SL8_acceptor_site';

--- ************************************************
--- *** relation: sl9_acceptor_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A SL2_acceptor_site which appends the SL ***
--- *** 9 RNA leader sequence to the 5' end of a ***
--- *** n mRNA. SL9 acceptor sites occur in gene ***
--- *** s in internal segments of polycistronic  ***
--- *** transcripts.                             ***
--- ************************************************
---

CREATE VIEW sl9_acceptor_site AS
  SELECT
    feature_id AS sl9_acceptor_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SL9_acceptor_site';

--- ************************************************
--- *** relation: sl10_accceptor_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A SL2_acceptor_site which appends the SL ***
--- *** 10 RNA leader sequence to the 5' end of  ***
--- *** an mRNA. SL10 acceptor sites occur in ge ***
--- *** nes in internal segments of polycistroni ***
--- *** c transcripts.                           ***
--- ************************************************
---

CREATE VIEW sl10_accceptor_site AS
  SELECT
    feature_id AS sl10_accceptor_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SL10_accceptor_site';

--- ************************************************
--- *** relation: sl11_acceptor_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A SL2_acceptor_site which appends the SL ***
--- *** 11 RNA leader sequence to the 5' end of  ***
--- *** an mRNA. SL11 acceptor sites occur in ge ***
--- *** nes in internal segments of polycistroni ***
--- *** c transcripts.                           ***
--- ************************************************
---

CREATE VIEW sl11_acceptor_site AS
  SELECT
    feature_id AS sl11_acceptor_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SL11_acceptor_site';

--- ************************************************
--- *** relation: sl12_acceptor_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A SL2_acceptor_site which appends the SL ***
--- *** 12 RNA leader sequence to the 5' end of  ***
--- *** an mRNA. SL12 acceptor sites occur in ge ***
--- *** nes in internal segments of polycistroni ***
--- *** c transcripts.                           ***
--- ************************************************
---

CREATE VIEW sl12_acceptor_site AS
  SELECT
    feature_id AS sl12_acceptor_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SL12_acceptor_site';

--- ************************************************
--- *** relation: duplicated_pseudogene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A pseudogene that arose via gene duplica ***
--- *** tion. Generally duplicated pseudogenes h ***
--- *** ave the same structure as the original g ***
--- *** ene, including intron-exon structure and ***
--- ***  some regulatory sequence.               ***
--- ************************************************
---

CREATE VIEW duplicated_pseudogene AS
  SELECT
    feature_id AS duplicated_pseudogene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'duplicated_pseudogene';

--- ************************************************
--- *** relation: unitary_pseudogene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A pseudogene, deactivated from original  ***
--- *** state by mutation, fixed in a population ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW unitary_pseudogene AS
  SELECT
    feature_id AS unitary_pseudogene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'unitary_pseudogene';

--- ************************************************
--- *** relation: non_processed_pseudogene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A pseudogene that arose from a means oth ***
--- *** er than retrotransposition.              ***
--- ************************************************
---

CREATE VIEW non_processed_pseudogene AS
  SELECT
    feature_id AS non_processed_pseudogene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pseudogene_by_unequal_crossing_over' OR cvterm.name = 'nuclear_mt_pseudogene' OR cvterm.name = 'cassette_pseudogene' OR cvterm.name = 'duplicated_pseudogene' OR cvterm.name = 'unitary_pseudogene' OR cvterm.name = 'non_processed_pseudogene';

--- ************************************************
--- *** relation: variant_quality ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A dependent entity that inheres in a bea ***
--- *** rer, a sequence variant.                 ***
--- ************************************************
---

CREATE VIEW variant_quality AS
  SELECT
    feature_id AS variant_quality_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'variant_origin' OR cvterm.name = 'variant_frequency' OR cvterm.name = 'variant_phenotype' OR cvterm.name = 'maternal_variant' OR cvterm.name = 'paternal_variant' OR cvterm.name = 'somatic_variant' OR cvterm.name = 'germline_variant' OR cvterm.name = 'pedigree_specific_variant' OR cvterm.name = 'population_specific_variant' OR cvterm.name = 'de_novo_variant' OR cvterm.name = 'unique_variant' OR cvterm.name = 'rare_variant' OR cvterm.name = 'polymorphic_variant' OR cvterm.name = 'common_variant' OR cvterm.name = 'fixed_variant' OR cvterm.name = 'benign_variant' OR cvterm.name = 'disease_associated_variant' OR cvterm.name = 'disease_causing_variant' OR cvterm.name = 'lethal_variant' OR cvterm.name = 'quantitative_variant' OR cvterm.name = 'variant_quality';

--- ************************************************
--- *** relation: variant_origin ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A quality inhering in a variant by virtu ***
--- *** e of its origin.                         ***
--- ************************************************
---

CREATE VIEW variant_origin AS
  SELECT
    feature_id AS variant_origin_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'maternal_variant' OR cvterm.name = 'paternal_variant' OR cvterm.name = 'somatic_variant' OR cvterm.name = 'germline_variant' OR cvterm.name = 'pedigree_specific_variant' OR cvterm.name = 'population_specific_variant' OR cvterm.name = 'de_novo_variant' OR cvterm.name = 'variant_origin';

--- ************************************************
--- *** relation: variant_frequency ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A physical quality which inheres to the  ***
--- *** variant by virtue of the number instance ***
--- *** s of the variant within a population.    ***
--- ************************************************
---

CREATE VIEW variant_frequency AS
  SELECT
    feature_id AS variant_frequency_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'unique_variant' OR cvterm.name = 'rare_variant' OR cvterm.name = 'polymorphic_variant' OR cvterm.name = 'common_variant' OR cvterm.name = 'fixed_variant' OR cvterm.name = 'variant_frequency';

--- ************************************************
--- *** relation: unique_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A physical quality which inheres to the  ***
--- *** variant by virtue of the number instance ***
--- *** s of the variant within a population.    ***
--- ************************************************
---

CREATE VIEW unique_variant AS
  SELECT
    feature_id AS unique_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'unique_variant';

--- ************************************************
--- *** relation: rare_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW rare_variant AS
  SELECT
    feature_id AS rare_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rare_variant';

--- ************************************************
--- *** relation: polymorphic_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW polymorphic_variant AS
  SELECT
    feature_id AS polymorphic_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polymorphic_variant';

--- ************************************************
--- *** relation: common_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW common_variant AS
  SELECT
    feature_id AS common_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'common_variant';

--- ************************************************
--- *** relation: fixed_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW fixed_variant AS
  SELECT
    feature_id AS fixed_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'fixed_variant';

--- ************************************************
--- *** relation: variant_phenotype ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A quality inhering in a variant by virtu ***
--- *** e of its phenotype.                      ***
--- ************************************************
---

CREATE VIEW variant_phenotype AS
  SELECT
    feature_id AS variant_phenotype_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'benign_variant' OR cvterm.name = 'disease_associated_variant' OR cvterm.name = 'disease_causing_variant' OR cvterm.name = 'lethal_variant' OR cvterm.name = 'quantitative_variant' OR cvterm.name = 'variant_phenotype';

--- ************************************************

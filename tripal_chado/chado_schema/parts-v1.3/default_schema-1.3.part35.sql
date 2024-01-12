SET search_path=so,chado,pg_catalog;
--- *** oid product of recombination between a p ***
--- *** ericentric inversion and a cytologically ***
--- ***  wild-type chromosome.                   ***
--- ************************************************
---

CREATE VIEW autosynaptic_chromosome AS
  SELECT
    feature_id AS autosynaptic_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dexstrosynaptic_chromosome' OR cvterm.name = 'laevosynaptic_chromosome' OR cvterm.name = 'autosynaptic_chromosome';

--- ************************************************
--- *** relation: homo_compound_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A compound chromosome whereby two copies ***
--- ***  of the same chromosomal arm attached to ***
--- ***  a common centromere. The chromosome is  ***
--- *** diploid for the arm involved.            ***
--- ************************************************
---

CREATE VIEW homo_compound_chromosome AS
  SELECT
    feature_id AS homo_compound_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'homo_compound_chromosome';

--- ************************************************
--- *** relation: hetero_compound_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A compound chromosome whereby two arms f ***
--- *** rom different chromosomes are connected  ***
--- *** through the centromere of one of them.   ***
--- ************************************************
---

CREATE VIEW hetero_compound_chromosome AS
  SELECT
    feature_id AS hetero_compound_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'hetero_compound_chromosome';

--- ************************************************
--- *** relation: chromosome_fission ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome that occurred by the divisi ***
--- *** on of a larger chromosome.               ***
--- ************************************************
---

CREATE VIEW chromosome_fission AS
  SELECT
    feature_id AS chromosome_fission_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chromosome_fission';

--- ************************************************
--- *** relation: dexstrosynaptic_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An autosynaptic chromosome carrying the  ***
--- *** two right (D = dextro) telomeres.        ***
--- ************************************************
---

CREATE VIEW dexstrosynaptic_chromosome AS
  SELECT
    feature_id AS dexstrosynaptic_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dexstrosynaptic_chromosome';

--- ************************************************
--- *** relation: laevosynaptic_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** LS is an autosynaptic chromosome carryin ***
--- *** g the two left (L = levo) telomeres.     ***
--- ************************************************
---

CREATE VIEW laevosynaptic_chromosome AS
  SELECT
    feature_id AS laevosynaptic_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'laevosynaptic_chromosome';

--- ************************************************
--- *** relation: free_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome structure variation whereby ***
--- ***  the duplicated sequences are carried as ***
--- ***  a free centric element.                 ***
--- ************************************************
---

CREATE VIEW free_duplication AS
  SELECT
    feature_id AS free_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'free_ring_duplication' OR cvterm.name = 'free_duplication';

--- ************************************************
--- *** relation: free_ring_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A ring chromosome which is a copy of ano ***
--- *** ther chromosome.                         ***
--- ************************************************
---

CREATE VIEW free_ring_duplication AS
  SELECT
    feature_id AS free_ring_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'free_ring_duplication';

--- ************************************************
--- *** relation: complex_chromosomal_mutation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome structure variant with 4 or ***
--- ***  more breakpoints.                       ***
--- ************************************************
---

CREATE VIEW complex_chromosomal_mutation AS
  SELECT
    feature_id AS complex_chromosomal_mutation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'complex_chromosomal_mutation';

--- ************************************************
--- *** relation: deficient_translocation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosomal deletion whereby a translo ***
--- *** cation occurs in which one of the four b ***
--- *** roken ends loses a segment before re-joi ***
--- *** ning.                                    ***
--- ************************************************
---

CREATE VIEW deficient_translocation AS
  SELECT
    feature_id AS deficient_translocation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'deficient_translocation';

--- ************************************************
--- *** relation: inversion_cum_translocation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosomal translocation whereby the  ***
--- *** first two breaks are in the same chromos ***
--- *** ome, and the region between them is rejo ***
--- *** ined in inverted order to the other side ***
--- ***  of the first break, such that both side ***
--- *** s of break one are present on the same c ***
--- *** hromosome. The remaining free ends are j ***
--- *** oined as a translocation with those resu ***
--- *** lting from the third break.              ***
--- ************************************************
---

CREATE VIEW inversion_cum_translocation AS
  SELECT
    feature_id AS inversion_cum_translocation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'inversion_cum_translocation';

--- ************************************************
--- *** relation: bipartite_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An interchromosomal mutation whereby the ***
--- ***  (large) region between the first two br ***
--- *** eaks listed is lost, and the two flankin ***
--- *** g segments (one of them centric) are joi ***
--- *** ned as a translocation to the free ends  ***
--- *** resulting from the third break.          ***
--- ************************************************
---

CREATE VIEW bipartite_duplication AS
  SELECT
    feature_id AS bipartite_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'bipartite_duplication';

--- ************************************************
--- *** relation: cyclic_translocation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosomal translocation whereby thre ***
--- *** e breaks occurred in three different chr ***
--- *** omosomes. The centric segment resulting  ***
--- *** from the first break listed is joined to ***
--- ***  the acentric segment resulting from the ***
--- ***  second, rather than the third.          ***
--- ************************************************
---

CREATE VIEW cyclic_translocation AS
  SELECT
    feature_id AS cyclic_translocation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cyclic_translocation';

--- ************************************************
--- *** relation: bipartite_inversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosomal inversion caused by three  ***
--- *** breaks in the same chromosome; both cent ***
--- *** ral segments are inverted in place (i.e. ***
--- *** , they are not transposed).              ***
--- ************************************************
---

CREATE VIEW bipartite_inversion AS
  SELECT
    feature_id AS bipartite_inversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'bipartite_inversion';

--- ************************************************
--- *** relation: uninvert_insert_dup ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An insertional duplication where a copy  ***
--- *** of the segment between the first two bre ***
--- *** aks listed is inserted at the third brea ***
--- *** k; the insertion is in cytologically the ***
--- ***  same orientation as its flanking segmen ***
--- *** ts.                                      ***
--- ************************************************
---

CREATE VIEW uninvert_insert_dup AS
  SELECT
    feature_id AS uninvert_insert_dup_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'uninverted_insertional_duplication';

--- ************************************************
--- *** relation: inverted_insertional_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An insertional duplication where a copy  ***
--- *** of the segment between the first two bre ***
--- *** aks listed is inserted at the third brea ***
--- *** k; the insertion is in cytologically inv ***
--- *** erted orientation with respect to its fl ***
--- *** anking segments.                         ***
--- ************************************************
---

CREATE VIEW inverted_insertional_duplication AS
  SELECT
    feature_id AS inverted_insertional_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inverted_insertional_duplication';

--- ************************************************
--- *** relation: insertional_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome duplication involving the i ***
--- *** nsertion of a duplicated region (as oppo ***
--- *** sed to a free duplication).              ***
--- ************************************************
---

CREATE VIEW insertional_duplication AS
  SELECT
    feature_id AS insertional_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'uninverted_insertional_duplication' OR cvterm.name = 'inverted_insertional_duplication' OR cvterm.name = 'unoriented_insertional_duplication' OR cvterm.name = 'insertional_duplication';

--- ************************************************
--- *** relation: interchromosomal_transposition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosome structure variation whereby ***
--- ***  a transposition occurred between chromo ***
--- *** somes.                                   ***
--- ************************************************
---

CREATE VIEW interchromosomal_transposition AS
  SELECT
    feature_id AS interchromosomal_transposition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'deficient_interchromosomal_transposition' OR cvterm.name = 'inverted_interchromosomal_transposition' OR cvterm.name = 'uninverted_interchromosomal_transposition' OR cvterm.name = 'unoriented_interchromosomal_transposition' OR cvterm.name = 'interchromosomal_transposition';

--- ************************************************
--- *** relation: invert_inter_transposition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An interchromosomal transposition whereb ***
--- *** y a copy of the segment between the firs ***
--- *** t two breaks listed is inserted at the t ***
--- *** hird break; the insertion is in cytologi ***
--- *** cally inverted orientation with respect  ***
--- *** to its flanking segment.                 ***
--- ************************************************
---

CREATE VIEW invert_inter_transposition AS
  SELECT
    feature_id AS invert_inter_transposition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inverted_interchromosomal_transposition';

--- ************************************************
--- *** relation: uninvert_inter_transposition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An interchromosomal transition where the ***
--- ***  segment between the first two breaks li ***
--- *** sted is removed and inserted at the thir ***
--- *** d break; the insertion is in cytological ***
--- *** ly the same orientation as its flanking  ***
--- *** segments.                                ***
--- ************************************************
---

CREATE VIEW uninvert_inter_transposition AS
  SELECT
    feature_id AS uninvert_inter_transposition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'uninverted_interchromosomal_transposition';

--- ************************************************
--- *** relation: invert_intra_transposition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An intrachromosomal transposition whereb ***
--- *** y the segment between the first two brea ***
--- *** ks listed is removed and inserted at the ***
--- ***  third break; the insertion is in cytolo ***
--- *** gically inverted orientation with respec ***
--- *** t to its flanking segments.              ***
--- ************************************************
---

CREATE VIEW invert_intra_transposition AS
  SELECT
    feature_id AS invert_intra_transposition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inverted_intrachromosomal_transposition';

--- ************************************************
--- *** relation: uninvert_intra_transposition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An intrachromosomal transposition whereb ***
--- *** y the segment between the first two brea ***
--- *** ks listed is removed and inserted at the ***
--- ***  third break; the insertion is in cytolo ***
--- *** gically the same orientation as its flan ***
--- *** king segments.                           ***
--- ************************************************
---

CREATE VIEW uninvert_intra_transposition AS
  SELECT
    feature_id AS uninvert_intra_transposition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'uninverted_intrachromosomal_transposition';

--- ************************************************
--- *** relation: unorient_insert_dup ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An insertional duplication where a copy  ***
--- *** of the segment between the first two bre ***
--- *** aks listed is inserted at the third brea ***
--- *** k; the orientation of the insertion with ***
--- ***  respect to its flanking segments is not ***
--- ***  recorded.                               ***
--- ************************************************
---

CREATE VIEW unorient_insert_dup AS
  SELECT
    feature_id AS unorient_insert_dup_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'unoriented_insertional_duplication';

--- ************************************************
--- *** relation: unoriented_interchromosomal_transposition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An interchromosomal transposition whereb ***
--- *** y a copy of the segment between the firs ***
--- *** t two breaks listed is inserted at the t ***
--- *** hird break; the orientation of the inser ***
--- *** tion with respect to its flanking segmen ***
--- *** ts is not recorded.                      ***
--- ************************************************
---

CREATE VIEW unoriented_interchromosomal_transposition AS
  SELECT
    feature_id AS unoriented_interchromosomal_transposition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'unoriented_interchromosomal_transposition';

--- ************************************************
--- *** relation: unoriented_intrachromosomal_transposition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An intrachromosomal transposition whereb ***
--- *** y the segment between the first two brea ***
--- *** ks listed is removed and inserted at the ***
--- ***  third break; the orientation of the ins ***
--- *** ertion with respect to its flanking segm ***
--- *** ents is not recorded.                    ***
--- ************************************************
---

CREATE VIEW unoriented_intrachromosomal_transposition AS
  SELECT
    feature_id AS unoriented_intrachromosomal_transposition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'unoriented_intrachromosomal_transposition';

--- ************************************************
--- *** relation: uncharacterised_chromosomal_mutation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW uncharacterised_chromosomal_mutation AS
  SELECT
    feature_id AS uncharacterised_chromosomal_mutation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'partially_characterised_chromosomal_mutation' OR cvterm.name = 'uncharacterised_chromosomal_mutation';

--- ************************************************
--- *** relation: deficient_inversion ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A chromosomal deletion whereby three bre ***
--- *** aks occur in the same chromosome; one ce ***
--- *** ntral region is lost, and the other is i ***
--- *** nverted.                                 ***
--- ************************************************
---

CREATE VIEW deficient_inversion AS
  SELECT
    feature_id AS deficient_inversion_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'deficient_inversion';

--- ************************************************
--- *** relation: tandem_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A duplication consisting of 2 identical  ***
--- *** adjacent regions.                        ***
--- ************************************************
---

CREATE VIEW tandem_duplication AS
  SELECT
    feature_id AS tandem_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'direct_tandem_duplication' OR cvterm.name = 'inverted_tandem_duplication' OR cvterm.name = 'tandem_duplication';

--- ************************************************
--- *** relation: partially_characterised_chromosomal_mutation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW partially_characterised_chromosomal_mutation AS
  SELECT
    feature_id AS partially_characterised_chromosomal_mutation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'partially_characterised_chromosomal_mutation';

--- ************************************************
--- *** relation: chromosome_number_variation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of chromosome variation where the ***
--- ***  chromosome complement is not an exact m ***
--- *** ultiple of the haploid number.           ***
--- ************************************************
---

CREATE VIEW chromosome_number_variation AS
  SELECT
    feature_id AS chromosome_number_variation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'aneuploid' OR cvterm.name = 'polyploid' OR cvterm.name = 'hyperploid' OR cvterm.name = 'hypoploid' OR cvterm.name = 'autopolyploid' OR cvterm.name = 'allopolyploid' OR cvterm.name = 'chromosome_number_variation';

--- ************************************************
--- *** relation: chromosome_structure_variation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW chromosome_structure_variation AS
  SELECT
    feature_id AS chromosome_structure_variation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'free_chromosome_arm' OR cvterm.name = 'chromosomal_transposition' OR cvterm.name = 'aneuploid_chromosome' OR cvterm.name = 'intrachromosomal_mutation' OR cvterm.name = 'interchromosomal_mutation' OR cvterm.name = 'chromosomal_duplication' OR cvterm.name = 'compound_chromosome' OR cvterm.name = 'autosynaptic_chromosome' OR cvterm.name = 'complex_chromosomal_mutation' OR cvterm.name = 'uncharacterised_chromosomal_mutation' OR cvterm.name = 'intrachromosomal_transposition' OR cvterm.name = 'interchromosomal_transposition' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'uninverted_intrachromosomal_transposition' OR cvterm.name = 'unoriented_intrachromosomal_transposition' OR cvterm.name = 'deficient_interchromosomal_transposition' OR cvterm.name = 'inverted_interchromosomal_transposition' OR cvterm.name = 'uninverted_interchromosomal_transposition' OR cvterm.name = 'unoriented_interchromosomal_transposition' OR cvterm.name = 'inversion_derived_aneuploid_chromosome' OR cvterm.name = 'chromosomal_deletion' OR cvterm.name = 'chromosomal_inversion' OR cvterm.name = 'intrachromosomal_duplication' OR cvterm.name = 'ring_chromosome' OR cvterm.name = 'chromosome_fission' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inversion_derived_bipartite_deficiency' OR cvterm.name = 'inversion_derived_deficiency_plus_duplication' OR cvterm.name = 'inversion_derived_deficiency_plus_aneuploid' OR cvterm.name = 'deficient_translocation' OR cvterm.name = 'deficient_inversion' OR cvterm.name = 'inverted_ring_chromosome' OR cvterm.name = 'pericentric_inversion' OR cvterm.name = 'paracentric_inversion' OR cvterm.name = 'inversion_cum_translocation' OR cvterm.name = 'bipartite_inversion' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'deficient_inversion' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'inversion_derived_deficiency_plus_duplication' OR cvterm.name = 'inversion_derived_bipartite_duplication' OR cvterm.name = 'inversion_derived_duplication_plus_aneuploid' OR cvterm.name = 'intrachromosomal_transposition' OR cvterm.name = 'bipartite_duplication' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'uninverted_intrachromosomal_transposition' OR cvterm.name = 'unoriented_intrachromosomal_transposition' OR cvterm.name = 'inverted_ring_chromosome' OR cvterm.name = 'free_ring_duplication' OR cvterm.name = 'chromosomal_translocation' OR cvterm.name = 'bipartite_duplication' OR cvterm.name = 'interchromosomal_transposition' OR cvterm.name = 'translocation_element' OR cvterm.name = 'Robertsonian_fusion' OR cvterm.name = 'reciprocal_chromosomal_translocation' OR cvterm.name = 'deficient_translocation' OR cvterm.name = 'inversion_cum_translocation' OR cvterm.name = 'cyclic_translocation' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'deficient_interchromosomal_transposition' OR cvterm.name = 'inverted_interchromosomal_transposition' OR cvterm.name = 'uninverted_interchromosomal_transposition' OR cvterm.name = 'unoriented_interchromosomal_transposition' OR cvterm.name = 'interchromosomal_duplication' OR cvterm.name = 'intrachromosomal_duplication' OR cvterm.name = 'free_duplication' OR cvterm.name = 'insertional_duplication' OR cvterm.name = 'inversion_derived_deficiency_plus_duplication' OR cvterm.name = 'inversion_derived_bipartite_duplication' OR cvterm.name = 'inversion_derived_duplication_plus_aneuploid' OR cvterm.name = 'intrachromosomal_transposition' OR cvterm.name = 'bipartite_duplication' OR cvterm.name = 'deficient_intrachromosomal_transposition' OR cvterm.name = 'inverted_intrachromosomal_transposition' OR cvterm.name = 'uninverted_intrachromosomal_transposition' OR cvterm.name = 'unoriented_intrachromosomal_transposition' OR cvterm.name = 'free_ring_duplication' OR cvterm.name = 'uninverted_insertional_duplication' OR cvterm.name = 'inverted_insertional_duplication' OR cvterm.name = 'unoriented_insertional_duplication' OR cvterm.name = 'compound_chromosome_arm' OR cvterm.name = 'homo_compound_chromosome' OR cvterm.name = 'hetero_compound_chromosome' OR cvterm.name = 'dexstrosynaptic_chromosome' OR cvterm.name = 'laevosynaptic_chromosome' OR cvterm.name = 'partially_characterised_chromosomal_mutation' OR cvterm.name = 'chromosome_structure_variation';

--- ************************************************
--- *** relation: alternatively_spliced_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transcript that is alternatively splic ***
--- *** ed.                                      ***
--- ************************************************
---

CREATE VIEW alternatively_spliced_transcript AS
  SELECT
    feature_id AS alternatively_spliced_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'alternatively_spliced_transcript';

--- ************************************************
--- *** relation: encodes_1_polypeptide ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is alternately spliced, but  ***
--- *** encodes only one polypeptide.            ***
--- ************************************************
---

CREATE VIEW encodes_1_polypeptide AS
  SELECT
    feature_id AS encodes_1_polypeptide_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'encodes_1_polypeptide';

--- ************************************************
--- *** relation: encodes_greater_than_1_polypeptide ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is alternately spliced, and  ***
--- *** encodes more than one polypeptide.       ***
--- ************************************************
---

CREATE VIEW encodes_greater_than_1_polypeptide AS
  SELECT
    feature_id AS encodes_greater_than_1_polypeptide_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'encodes_disjoint_polypeptides' OR cvterm.name = 'encodes_overlapping_peptides' OR cvterm.name = 'encodes_different_polypeptides_different_stop' OR cvterm.name = 'encodes_overlapping_peptides_different_start' OR cvterm.name = 'encodes_overlapping_polypeptides_different_start_and_stop' OR cvterm.name = 'encodes_greater_than_1_polypeptide';

--- ************************************************
--- *** relation: encodes_different_polypeptides_different_stop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is alternately spliced, and  ***
--- *** encodes more than one polypeptide, that  ***
--- *** have overlapping peptide sequences, but  ***
--- *** use different stop codons.               ***
--- ************************************************
---

CREATE VIEW encodes_different_polypeptides_different_stop AS
  SELECT
    feature_id AS encodes_different_polypeptides_different_stop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'encodes_different_polypeptides_different_stop';

--- ************************************************
--- *** relation: encodes_overlapping_peptides_different_start ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is alternately spliced, and  ***
--- *** encodes more than one polypeptide, that  ***
--- *** have overlapping peptide sequences, but  ***
--- *** use different start codons.              ***
--- ************************************************
---

CREATE VIEW encodes_overlapping_peptides_different_start AS
  SELECT
    feature_id AS encodes_overlapping_peptides_different_start_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'encodes_overlapping_peptides_different_start';

--- ************************************************
--- *** relation: encodes_disjoint_polypeptides ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is alternately spliced, and  ***
--- *** encodes more than one polypeptide, that  ***
--- *** do not have overlapping peptide sequence ***
--- *** s.                                       ***
--- ************************************************
---

CREATE VIEW encodes_disjoint_polypeptides AS
  SELECT
    feature_id AS encodes_disjoint_polypeptides_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'encodes_disjoint_polypeptides';

--- ************************************************
--- *** relation: encodes_overlapping_polypeptides_different_start_and_stop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is alternately spliced, and  ***
--- *** encodes more than one polypeptide, that  ***
--- *** have overlapping peptide sequences, but  ***
--- *** use different start and stop codons.     ***
--- ************************************************
---

CREATE VIEW encodes_overlapping_polypeptides_different_start_and_stop AS
  SELECT
    feature_id AS encodes_overlapping_polypeptides_different_start_and_stop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'encodes_overlapping_polypeptides_different_start_and_stop';

--- ************************************************
--- *** relation: encodes_overlapping_peptides ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is alternately spliced, and  ***
--- *** encodes more than one polypeptide, that  ***
--- *** have overlapping peptide sequences.      ***
--- ************************************************
---

CREATE VIEW encodes_overlapping_peptides AS
  SELECT
    feature_id AS encodes_overlapping_peptides_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'encodes_different_polypeptides_different_stop' OR cvterm.name = 'encodes_overlapping_peptides_different_start' OR cvterm.name = 'encodes_overlapping_polypeptides_different_start_and_stop' OR cvterm.name = 'encodes_overlapping_peptides';

--- ************************************************
--- *** relation: cryptogene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A maxicircle gene so extensively edited  ***
--- *** that it cannot be matched to its edited  ***
--- *** mRNA sequence.                           ***
--- ************************************************
---

CREATE VIEW cryptogene AS
  SELECT
    feature_id AS cryptogene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cryptogene';

--- ************************************************
--- *** relation: dicistronic_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A primary transcript that has the qualit ***
--- *** y dicistronic.                           ***
--- ************************************************
---

CREATE VIEW dicistronic_primary_transcript AS
  SELECT
    feature_id AS dicistronic_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dicistronic_primary_transcript';

--- ************************************************
--- *** relation: member_of_regulon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW member_of_regulon AS
  SELECT
    feature_id AS member_of_regulon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'member_of_regulon';

--- ************************************************
--- *** relation: cds_independently_known ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A CDS with the evidence status of being  ***
--- *** independently known.                     ***
--- ************************************************
---

CREATE VIEW cds_independently_known AS
  SELECT
    feature_id AS cds_independently_known_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'CDS_independently_known';

--- ************************************************
--- *** relation: orphan_cds ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A CDS whose predicted amino acid sequenc ***
--- *** e is unsupported by any experimental evi ***
--- *** dence or by any match with any other kno ***
--- *** wn sequence.                             ***
--- ************************************************
---

CREATE VIEW orphan_cds AS
  SELECT
    feature_id AS orphan_cds_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'orphan_CDS';

--- ************************************************
--- *** relation: cds_supported_by_domain_match_data ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A CDS that is supported by domain simila ***
--- *** rity.                                    ***
--- ************************************************
---

CREATE VIEW cds_supported_by_domain_match_data AS
  SELECT
    feature_id AS cds_supported_by_domain_match_data_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'CDS_supported_by_domain_match_data';

--- ************************************************
--- *** relation: cds_supported_by_sequence_similarity_data ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A CDS that is supported by sequence simi ***
--- *** larity data.                             ***
--- ************************************************
---

CREATE VIEW cds_supported_by_sequence_similarity_data AS
  SELECT
    feature_id AS cds_supported_by_sequence_similarity_data_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'CDS_supported_by_domain_match_data' OR cvterm.name = 'CDS_supported_by_EST_or_cDNA_data' OR cvterm.name = 'CDS_supported_by_sequence_similarity_data';

--- ************************************************
--- *** relation: cds_predicted ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A CDS that is predicted.                 ***
--- ************************************************
---

CREATE VIEW cds_predicted AS
  SELECT
    feature_id AS cds_predicted_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'orphan_CDS' OR cvterm.name = 'CDS_supported_by_sequence_similarity_data' OR cvterm.name = 'CDS_supported_by_domain_match_data' OR cvterm.name = 'CDS_supported_by_EST_or_cDNA_data' OR cvterm.name = 'CDS_predicted';

--- ************************************************
--- *** relation: cds_supported_by_est_or_cdna_data ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A CDS that is supported by similarity to ***
--- ***  EST or cDNA data.                       ***
--- ************************************************
---

CREATE VIEW cds_supported_by_est_or_cdna_data AS
  SELECT
    feature_id AS cds_supported_by_est_or_cdna_data_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'CDS_supported_by_EST_or_cDNA_data';

--- ************************************************
--- *** relation: internal_shine_dalgarno_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A Shine-Dalgarno sequence that stimulate ***
--- *** s recoding through interactions with the ***
--- ***  anti-Shine-Dalgarno in the RNA of small ***
--- ***  ribosomal subunits of translating ribos ***
--- *** omes. The signal is only operative in Ba ***
--- *** cteria.                                  ***
--- ************************************************
---

CREATE VIEW internal_shine_dalgarno_sequence AS
  SELECT
    feature_id AS internal_shine_dalgarno_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'internal_Shine_Dalgarno_sequence';

--- ************************************************
--- *** relation: recoded_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The sequence of a mature mRNA transcript ***
--- *** , modified before translation or during  ***
--- *** translation, usually by special cis-acti ***
--- *** ng signals.                              ***
--- ************************************************
---

CREATE VIEW recoded_mrna AS
  SELECT
    feature_id AS recoded_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mRNA_recoded_by_translational_bypass' OR cvterm.name = 'mRNA_recoded_by_codon_redefinition' OR cvterm.name = 'recoded_mRNA';

--- ************************************************
--- *** relation: minus_1_translationally_frameshifted ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a translational  ***
--- *** frameshift of -1.                        ***
--- ************************************************
---

CREATE VIEW minus_1_translationally_frameshifted AS
  SELECT
    feature_id AS minus_1_translationally_frameshifted_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minus_1_translationally_frameshifted';

--- ************************************************
--- *** relation: plus_1_translationally_frameshifted ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a translational  ***
--- *** frameshift of +1.                        ***
--- ************************************************
---

CREATE VIEW plus_1_translationally_frameshifted AS
  SELECT
    feature_id AS plus_1_translationally_frameshifted_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'plus_1_translationally_frameshifted';

--- ************************************************
--- *** relation: mrna_recoded_by_translational_bypass ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A recoded_mRNA where translation was sus ***
--- *** pended at a particular codon and resumed ***
--- ***  at a particular non-overlapping downstr ***
--- *** eam codon.                               ***
--- ************************************************
---

CREATE VIEW mrna_recoded_by_translational_bypass AS
  SELECT
    feature_id AS mrna_recoded_by_translational_bypass_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mRNA_recoded_by_translational_bypass';

--- ************************************************
--- *** relation: mrna_recoded_by_codon_redefinition ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A recoded_mRNA that was modified by an a ***
--- *** lteration of codon meaning.              ***
--- ************************************************
---

CREATE VIEW mrna_recoded_by_codon_redefinition AS
  SELECT
    feature_id AS mrna_recoded_by_codon_redefinition_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mRNA_recoded_by_codon_redefinition';

--- ************************************************
--- *** relation: recoding_stimulatory_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A site in an mRNA sequence that stimulat ***
--- *** es the recoding of a region in the same  ***
--- *** mRNA.                                    ***
--- ************************************************
---

CREATE VIEW recoding_stimulatory_region AS
  SELECT
    feature_id AS recoding_stimulatory_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'internal_Shine_Dalgarno_sequence' OR cvterm.name = 'SECIS_element' OR cvterm.name = 'three_prime_recoding_site' OR cvterm.name = 'five_prime_recoding_site' OR cvterm.name = 'stop_codon_signal' OR cvterm.name = 'three_prime_stem_loop_structure' OR cvterm.name = 'flanking_three_prime_quadruplet_recoding_signal' OR cvterm.name = 'three_prime_repeat_recoding_signal' OR cvterm.name = 'distant_three_prime_recoding_signal' OR cvterm.name = 'UAG_stop_codon_signal' OR cvterm.name = 'UAA_stop_codon_signal' OR cvterm.name = 'UGA_stop_codon_signal' OR cvterm.name = 'recoding_stimulatory_region';

--- ************************************************

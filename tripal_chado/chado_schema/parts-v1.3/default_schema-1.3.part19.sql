SET search_path=so,chado,pg_catalog;
--- *** relation: rna_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a self-repli ***
--- *** cating, RNA molecule.                    ***
--- ************************************************
---

CREATE VIEW rna_chromosome AS
  SELECT
    feature_id AS rna_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'single_stranded_RNA_chromosome' OR cvterm.name = 'double_stranded_RNA_chromosome' OR cvterm.name = 'linear_single_stranded_RNA_chromosome' OR cvterm.name = 'circular_single_stranded_RNA_chromosome' OR cvterm.name = 'linear_double_stranded_RNA_chromosome' OR cvterm.name = 'circular_double_stranded_RNA_chromosome' OR cvterm.name = 'RNA_chromosome';

--- ************************************************
--- *** relation: single_stranded_rna_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a self-repli ***
--- *** cating, single-stranded RNA molecule.    ***
--- ************************************************
---

CREATE VIEW single_stranded_rna_chromosome AS
  SELECT
    feature_id AS single_stranded_rna_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'linear_single_stranded_RNA_chromosome' OR cvterm.name = 'circular_single_stranded_RNA_chromosome' OR cvterm.name = 'single_stranded_RNA_chromosome';

--- ************************************************
--- *** relation: linear_single_stranded_rna_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a self-repli ***
--- *** cating, single-stranded, linear RNA mole ***
--- *** cule.                                    ***
--- ************************************************
---

CREATE VIEW linear_single_stranded_rna_chromosome AS
  SELECT
    feature_id AS linear_single_stranded_rna_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'linear_single_stranded_RNA_chromosome';

--- ************************************************
--- *** relation: linear_double_stranded_rna_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a self-repli ***
--- *** cating, double-stranded, linear RNA mole ***
--- *** cule.                                    ***
--- ************************************************
---

CREATE VIEW linear_double_stranded_rna_chromosome AS
  SELECT
    feature_id AS linear_double_stranded_rna_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'linear_double_stranded_RNA_chromosome';

--- ************************************************
--- *** relation: double_stranded_rna_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a self-repli ***
--- *** cating, double-stranded RNA molecule.    ***
--- ************************************************
---

CREATE VIEW double_stranded_rna_chromosome AS
  SELECT
    feature_id AS double_stranded_rna_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'linear_double_stranded_RNA_chromosome' OR cvterm.name = 'circular_double_stranded_RNA_chromosome' OR cvterm.name = 'double_stranded_RNA_chromosome';

--- ************************************************
--- *** relation: circular_single_stranded_rna_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a self-repli ***
--- *** cating, single-stranded, circular DNA mo ***
--- *** lecule.                                  ***
--- ************************************************
---

CREATE VIEW circular_single_stranded_rna_chromosome AS
  SELECT
    feature_id AS circular_single_stranded_rna_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'circular_single_stranded_RNA_chromosome';

--- ************************************************
--- *** relation: circular_double_stranded_rna_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Structural unit composed of a self-repli ***
--- *** cating, double-stranded, circular RNA mo ***
--- *** lecule.                                  ***
--- ************************************************
---

CREATE VIEW circular_double_stranded_rna_chromosome AS
  SELECT
    feature_id AS circular_double_stranded_rna_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'circular_double_stranded_RNA_chromosome';

--- ************************************************
--- *** relation: insertion_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A terminal_inverted_repeat_element that  ***
--- *** is bacterial and only encodes the functi ***
--- *** ons required for its transposition betwe ***
--- *** en these inverted repeats.               ***
--- ************************************************
---

CREATE VIEW insertion_sequence AS
  SELECT
    feature_id AS insertion_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'insertion_sequence';

--- ************************************************
--- *** relation: minicircle_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW minicircle_gene AS
  SELECT
    feature_id AS minicircle_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minicircle_gene';

--- ************************************************
--- *** relation: cryptic ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A feature_attribute describing a feature ***
--- ***  that is not manifest under normal condi ***
--- *** tions.                                   ***
--- ************************************************
---

CREATE VIEW cryptic AS
  SELECT
    feature_id AS cryptic_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cryptic';

--- ************************************************
--- *** relation: anchor_binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW anchor_binding_site AS
  SELECT
    feature_id AS anchor_binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'anchor_binding_site';

--- ************************************************
--- *** relation: template_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a guide_RNA that specifies t ***
--- *** he insertions and deletions of bases in  ***
--- *** the editing of a target mRNA.            ***
--- ************************************************
---

CREATE VIEW template_region AS
  SELECT
    feature_id AS template_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'template_region';

--- ************************************************
--- *** relation: grna_encoding ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A non-protein_coding gene that encodes a ***
--- ***  guide_RNA.                              ***
--- ************************************************
---

CREATE VIEW grna_encoding AS
  SELECT
    feature_id AS grna_encoding_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gRNA_encoding';

--- ************************************************
--- *** relation: minicircle ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A minicircle is a replicon, part of a ki ***
--- *** netoplast, that encodes for guide RNAs.  ***
--- ************************************************
---

CREATE VIEW minicircle AS
  SELECT
    feature_id AS minicircle_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'minicircle';

--- ************************************************
--- *** relation: rho_dependent_bacterial_terminator ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW rho_dependent_bacterial_terminator AS
  SELECT
    feature_id AS rho_dependent_bacterial_terminator_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rho_dependent_bacterial_terminator';

--- ************************************************
--- *** relation: rho_independent_bacterial_terminator ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW rho_independent_bacterial_terminator AS
  SELECT
    feature_id AS rho_independent_bacterial_terminator_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rho_independent_bacterial_terminator';

--- ************************************************
--- *** relation: strand_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW strand_attribute AS
  SELECT
    feature_id AS strand_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'single' OR cvterm.name = 'double' OR cvterm.name = 'strand_attribute';

--- ************************************************
--- *** relation: single ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW single AS
  SELECT
    feature_id AS single_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'single';

--- ************************************************
--- *** relation: double ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW double AS
  SELECT
    feature_id AS double_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'double';

--- ************************************************
--- *** relation: topology_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW topology_attribute AS
  SELECT
    feature_id AS topology_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'linear' OR cvterm.name = 'circular' OR cvterm.name = 'topology_attribute';

--- ************************************************
--- *** relation: linear ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A quality of a nucleotide polymer that h ***
--- *** as a 3'-terminal residue and a 5'-termin ***
--- *** al residue.                              ***
--- ************************************************
---

CREATE VIEW linear AS
  SELECT
    feature_id AS linear_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'linear';

--- ************************************************
--- *** relation: circular ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A quality of a nucleotide polymer that h ***
--- *** as no terminal nucleotide residues.      ***
--- ************************************************
---

CREATE VIEW circular AS
  SELECT
    feature_id AS circular_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'circular';

--- ************************************************
--- *** relation: class_ii_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Small non-coding RNA (59-60 nt long) con ***
--- *** taining 5' and 3' ends that are predicte ***
--- *** d to come together to form a stem struct ***
--- *** ure. Identified in the social amoeba Dic ***
--- *** tyostelium discoideum and localized in t ***
--- *** he cytoplasm.                            ***
--- ************************************************
---

CREATE VIEW class_ii_rna AS
  SELECT
    feature_id AS class_ii_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'class_II_RNA';

--- ************************************************
--- *** relation: class_i_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Small non-coding RNA (55-65 nt long) con ***
--- *** taining highly conserved 5' and 3' ends  ***
--- *** (16 and 8 nt, respectively) that are pre ***
--- *** dicted to come together to form a stem s ***
--- *** tructure. Identified in the social amoeb ***
--- *** a Dictyostelium discoideum and localized ***
--- ***  in the cytoplasm.                       ***
--- ************************************************
---

CREATE VIEW class_i_rna AS
  SELECT
    feature_id AS class_i_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'class_I_RNA';

--- ************************************************
--- *** relation: genomic_dna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW genomic_dna AS
  SELECT
    feature_id AS genomic_dna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'genomic_DNA';

--- ************************************************
--- *** relation: bac_cloned_genomic_insert ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW bac_cloned_genomic_insert AS
  SELECT
    feature_id AS bac_cloned_genomic_insert_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'BAC_cloned_genomic_insert';

--- ************************************************
--- *** relation: consensus ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW consensus AS
  SELECT
    feature_id AS consensus_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'consensus';

--- ************************************************
--- *** relation: consensus_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW consensus_region AS
  SELECT
    feature_id AS consensus_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'consensus_mRNA' OR cvterm.name = 'consensus_region';

--- ************************************************
--- *** relation: consensus_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW consensus_mrna AS
  SELECT
    feature_id AS consensus_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'consensus_mRNA';

--- ************************************************
--- *** relation: predicted_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW predicted_gene AS
  SELECT
    feature_id AS predicted_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'predicted_gene';

--- ************************************************
--- *** relation: gene_fragment ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW gene_fragment AS
  SELECT
    feature_id AS gene_fragment_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_fragment';

--- ************************************************
--- *** relation: recursive_splice_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A recursive splice site is a splice site ***
--- ***  which subdivides a large intron. Recurs ***
--- *** ive splicing is a mechanism that splices ***
--- ***  large introns by sub dividing the intro ***
--- *** n at non exonic elements and alternate e ***
--- *** xons.                                    ***
--- ************************************************
---

CREATE VIEW recursive_splice_site AS
  SELECT
    feature_id AS recursive_splice_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'recursive_splice_site';

--- ************************************************
--- *** relation: bac_end ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of sequence from the end of a B ***
--- *** AC clone that may provide a highly speci ***
--- *** fic marker.                              ***
--- ************************************************
---

CREATE VIEW bac_end AS
  SELECT
    feature_id AS bac_end_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'BAC_end';

--- ************************************************
--- *** relation: rrna_16s ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A large polynucleotide in Bacteria and A ***
--- *** rchaea, which functions as the small sub ***
--- *** unit of the ribosome.                    ***
--- ************************************************
---

CREATE VIEW rrna_16s AS
  SELECT
    feature_id AS rrna_16s_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_16S';

--- ************************************************
--- *** relation: rrna_23s ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A large polynucleotide in Bacteria and A ***
--- *** rchaea, which functions as the large sub ***
--- *** unit of the ribosome.                    ***
--- ************************************************
---

CREATE VIEW rrna_23s AS
  SELECT
    feature_id AS rrna_23s_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_23S';

--- ************************************************
--- *** relation: rrna_25s ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A large polynucleotide which functions a ***
--- *** s part of the large subunit of the ribos ***
--- *** ome in some eukaryotes.                  ***
--- ************************************************
---

CREATE VIEW rrna_25s AS
  SELECT
    feature_id AS rrna_25s_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_25S';

--- ************************************************
--- *** relation: solo_ltr ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A recombination product between the 2 LT ***
--- *** R of the same element.                   ***
--- ************************************************
---

CREATE VIEW solo_ltr AS
  SELECT
    feature_id AS solo_ltr_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'solo_LTR';

--- ************************************************
--- *** relation: low_complexity ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW low_complexity AS
  SELECT
    feature_id AS low_complexity_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'low_complexity';

--- ************************************************
--- *** relation: low_complexity_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW low_complexity_region AS
  SELECT
    feature_id AS low_complexity_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'low_complexity_region';

--- ************************************************
--- *** relation: prophage ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A phage genome after it has established  ***
--- *** in the host genome in a latent/immune st ***
--- *** ate either as a plasmid or as an integra ***
--- *** ted "island".                            ***
--- ************************************************
---

CREATE VIEW prophage AS
  SELECT
    feature_id AS prophage_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'prophage';

--- ************************************************
--- *** relation: cryptic_prophage ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A remnant of an integrated prophage in t ***
--- *** he host genome or an "island" in the hos ***
--- *** t genome that includes phage like-genes. ***
--- ************************************************
---

CREATE VIEW cryptic_prophage AS
  SELECT
    feature_id AS cryptic_prophage_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cryptic_prophage';

--- ************************************************
--- *** relation: tetraloop ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A base-paired stem with loop of 4 non-hy ***
--- *** drogen bonded nucleotides.               ***
--- ************************************************
---

CREATE VIEW tetraloop AS
  SELECT
    feature_id AS tetraloop_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tetraloop';

--- ************************************************
--- *** relation: dna_constraint_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A double-stranded DNA used to control ma ***
--- *** cromolecular structure and function.     ***
--- ************************************************
---

CREATE VIEW dna_constraint_sequence AS
  SELECT
    feature_id AS dna_constraint_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DNA_constraint_sequence';

--- ************************************************
--- *** relation: i_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cytosine rich domain whereby strands a ***
--- *** ssociate both inter- and intramolecularl ***
--- *** y at moderately acidic pH.               ***
--- ************************************************
---

CREATE VIEW i_motif AS
  SELECT
    feature_id AS i_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'i_motif';

--- ************************************************
--- *** relation: pna_oligo ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Peptide nucleic acid, is a chemical not  ***
--- *** known to occur naturally but is artifici ***
--- *** ally synthesized and used in some biolog ***
--- *** ical research and medical treatments. Th ***
--- *** e PNA backbone is composed of repeating  ***
--- *** N-(2-aminoethyl)-glycine units linked by ***
--- ***  peptide bonds. The purine and pyrimidin ***
--- *** e bases are linked to the backbone by me ***
--- *** thylene carbonyl bonds.                  ***
--- ************************************************
---

CREATE VIEW pna_oligo AS
  SELECT
    feature_id AS pna_oligo_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'PNA_oligo';

--- ************************************************
--- *** relation: dnazyme ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A DNA sequence with catalytic activity.  ***
--- ************************************************
---

CREATE VIEW dnazyme AS
  SELECT
    feature_id AS dnazyme_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DNAzyme';

--- ************************************************
--- *** relation: mnp ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A multiple nucleotide polymorphism with  ***
--- *** alleles of common length > 1, for exampl ***
--- *** e AAA/TTT.                               ***
--- ************************************************
---

CREATE VIEW mnp AS
  SELECT
    feature_id AS mnp_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'MNP';

--- ************************************************
--- *** relation: intron_domain ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW intron_domain AS
  SELECT
    feature_id AS intron_domain_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'internal_guide_sequence' OR cvterm.name = 'mirtron' OR cvterm.name = 'intron_domain';

--- ************************************************
--- *** relation: wobble_base_pair ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A type of non-canonical base pairing, mo ***
--- *** st commonly between G and U, which is im ***
--- *** portant for the secondary structure of R ***
--- *** NAs. It has similar thermodynamic stabil ***
--- *** ity to the Watson-Crick pairing. Wobble  ***
--- *** base pairs only have two hydrogen bonds. ***
--- ***  Other wobble base pair possibilities ar ***
--- *** e I-A, I-U and I-C.                      ***
--- ************************************************
---

CREATE VIEW wobble_base_pair AS
  SELECT
    feature_id AS wobble_base_pair_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'wobble_base_pair';

--- ************************************************
--- *** relation: internal_guide_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A purine-rich sequence in the group I in ***
--- *** trons which determines the locations of  ***
--- *** the splice sites in group I intron splic ***
--- *** ing and has catalytic activity.          ***
--- ************************************************
---

CREATE VIEW internal_guide_sequence AS
  SELECT
    feature_id AS internal_guide_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'internal_guide_sequence';

--- ************************************************
--- *** relation: silent_mutation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that does not affect  ***
--- *** protein function. Silent mutations may o ***
--- *** ccur in genic ( CDS, UTR, intron etc) an ***
--- *** d intergenic regions. Silent mutations m ***
--- *** ay have affects on processes such as spl ***
--- *** icing and regulation.                    ***
--- ************************************************
---

CREATE VIEW silent_mutation AS
  SELECT
    feature_id AS silent_mutation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'silent_mutation';

--- ************************************************
--- *** relation: epitope ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the molecule, in ***
--- *** teracts selectively and non-covalently w ***
--- *** ith antibodies, B cells or T cells.      ***
--- ************************************************
---

CREATE VIEW epitope AS
  SELECT
    feature_id AS epitope_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'epitope';

--- ************************************************
--- *** relation: copy_number_variation ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A variation that increases or decreases  ***
--- *** the copy number of a given region.       ***
--- ************************************************
---

CREATE VIEW copy_number_variation AS
  SELECT
    feature_id AS copy_number_variation_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'copy_number_gain' OR cvterm.name = 'copy_number_loss' OR cvterm.name = 'copy_number_variation';

--- ************************************************
--- *** relation: chromosome_breakpoint ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW chromosome_breakpoint AS
  SELECT
    feature_id AS chromosome_breakpoint_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inversion_breakpoint' OR cvterm.name = 'translocation_breakpoint' OR cvterm.name = 'insertion_breakpoint' OR cvterm.name = 'deletion_breakpoint' OR cvterm.name = 'chromosome_breakpoint';

--- ************************************************
--- *** relation: inversion_breakpoint ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The point within a chromosome where an i ***
--- *** nversion begins or ends.                 ***
--- ************************************************
---

CREATE VIEW inversion_breakpoint AS
  SELECT
    feature_id AS inversion_breakpoint_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inversion_breakpoint';

--- ************************************************
--- *** relation: allele ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An allele is one of a set of coexisting  ***
--- *** sequence variants of a gene.             ***
--- ************************************************
---

CREATE VIEW allele AS
  SELECT
    feature_id AS allele_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polymorphic_sequence_variant' OR cvterm.name = 'allele';

--- ************************************************
--- *** relation: haplotype ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A haplotype is one of a set of coexistin ***
--- *** g sequence variants of a haplotype block ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW haplotype AS
  SELECT
    feature_id AS haplotype_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'haplotype';

--- ************************************************
--- *** relation: polymorphic_sequence_variant ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence variant that is segregating i ***
--- *** n one or more natural populations of a s ***
--- *** pecies.                                  ***
--- ************************************************
---

CREATE VIEW polymorphic_sequence_variant AS
  SELECT
    feature_id AS polymorphic_sequence_variant_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polymorphic_sequence_variant';

--- ************************************************
--- *** relation: genome ***

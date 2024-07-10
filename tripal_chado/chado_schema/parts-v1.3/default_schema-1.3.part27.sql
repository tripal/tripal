SET search_path=so,chado,pg_catalog;
---

CREATE VIEW ligation_based_read AS
  SELECT
    feature_id AS ligation_based_read_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ligation_based_read';

--- ************************************************
--- *** relation: polymerase_synthesis_read ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A read produced by the polymerase based  ***
--- *** sequence by synthesis method.            ***
--- ************************************************
---

CREATE VIEW polymerase_synthesis_read AS
  SELECT
    feature_id AS polymerase_synthesis_read_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polymerase_synthesis_read';

--- ************************************************
--- *** relation: cis_regulatory_frameshift_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A structural region in an RNA molecule w ***
--- *** hich promotes ribosomal frameshifting of ***
--- ***  cis coding sequence.                    ***
--- ************************************************
---

CREATE VIEW cis_regulatory_frameshift_element AS
  SELECT
    feature_id AS cis_regulatory_frameshift_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cis_regulatory_frameshift_element';

--- ************************************************
--- *** relation: expressed_sequence_assembly ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence assembly derived from express ***
--- *** ed sequences.                            ***
--- ************************************************
---

CREATE VIEW expressed_sequence_assembly AS
  SELECT
    feature_id AS expressed_sequence_assembly_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'expressed_sequence_assembly';

--- ************************************************
--- *** relation: dna_binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the molecule, in ***
--- *** teracts selectively and non-covalently w ***
--- *** ith DNA.                                 ***
--- ************************************************
---

CREATE VIEW dna_binding_site AS
  SELECT
    feature_id AS dna_binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_DNA_contact' OR cvterm.name = 'DNA_binding_site';

--- ************************************************
--- *** relation: cryptic_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that is not transcribed under nor ***
--- *** mal conditions and is not critical to no ***
--- *** rmal cellular functioning.               ***
--- ************************************************
---

CREATE VIEW cryptic_gene AS
  SELECT
    feature_id AS cryptic_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cryptogene' OR cvterm.name = 'cryptic_gene';

--- ************************************************
--- *** relation: three_prime_race_clone ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A three prime RACE (Rapid Amplification  ***
--- *** of cDNA Ends) clone is a cDNA clone copi ***
--- *** ed from the 3' end of an mRNA (using a p ***
--- *** oly-dT primer to capture the polyA tail  ***
--- *** and a gene-specific or randomly primed 5 ***
--- *** ' primer), and spliced into a vector for ***
--- ***  propagation in a suitable host.         ***
--- ************************************************
---

CREATE VIEW three_prime_race_clone AS
  SELECT
    feature_id AS three_prime_race_clone_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_RACE_clone';

--- ************************************************
--- *** relation: cassette_pseudogene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cassette pseudogene is a kind of gene  ***
--- *** in an inactive form which may recombine  ***
--- *** at a telomeric locus to form a functiona ***
--- *** l copy.                                  ***
--- ************************************************
---

CREATE VIEW cassette_pseudogene AS
  SELECT
    feature_id AS cassette_pseudogene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cassette_pseudogene';

--- ************************************************
--- *** relation: alanine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW alanine AS
  SELECT
    feature_id AS alanine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'alanine';

--- ************************************************
--- *** relation: valine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW valine AS
  SELECT
    feature_id AS valine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'valine';

--- ************************************************
--- *** relation: leucine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW leucine AS
  SELECT
    feature_id AS leucine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'leucine';

--- ************************************************
--- *** relation: isoleucine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW isoleucine AS
  SELECT
    feature_id AS isoleucine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'isoleucine';

--- ************************************************
--- *** relation: proline ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW proline AS
  SELECT
    feature_id AS proline_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'proline';

--- ************************************************
--- *** relation: tryptophan ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW tryptophan AS
  SELECT
    feature_id AS tryptophan_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tryptophan';

--- ************************************************
--- *** relation: phenylalanine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW phenylalanine AS
  SELECT
    feature_id AS phenylalanine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'phenylalanine';

--- ************************************************
--- *** relation: methionine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW methionine AS
  SELECT
    feature_id AS methionine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'methionine';

--- ************************************************
--- *** relation: glycine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW glycine AS
  SELECT
    feature_id AS glycine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'glycine';

--- ************************************************
--- *** relation: serine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW serine AS
  SELECT
    feature_id AS serine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'serine';

--- ************************************************
--- *** relation: threonine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW threonine AS
  SELECT
    feature_id AS threonine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'threonine';

--- ************************************************
--- *** relation: tyrosine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW tyrosine AS
  SELECT
    feature_id AS tyrosine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'tyrosine';

--- ************************************************
--- *** relation: cysteine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW cysteine AS
  SELECT
    feature_id AS cysteine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cysteine';

--- ************************************************
--- *** relation: glutamine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW glutamine AS
  SELECT
    feature_id AS glutamine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'glutamine';

--- ************************************************
--- *** relation: asparagine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW asparagine AS
  SELECT
    feature_id AS asparagine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'asparagine';

--- ************************************************
--- *** relation: lysine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW lysine AS
  SELECT
    feature_id AS lysine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'lysine';

--- ************************************************
--- *** relation: arginine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW arginine AS
  SELECT
    feature_id AS arginine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'arginine';

--- ************************************************
--- *** relation: histidine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW histidine AS
  SELECT
    feature_id AS histidine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'histidine';

--- ************************************************
--- *** relation: aspartic_acid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW aspartic_acid AS
  SELECT
    feature_id AS aspartic_acid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'aspartic_acid';

--- ************************************************
--- *** relation: glutamic_acid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW glutamic_acid AS
  SELECT
    feature_id AS glutamic_acid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'glutamic_acid';

--- ************************************************
--- *** relation: selenocysteine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW selenocysteine AS
  SELECT
    feature_id AS selenocysteine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'selenocysteine';

--- ************************************************
--- *** relation: pyrrolysine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW pyrrolysine AS
  SELECT
    feature_id AS pyrrolysine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'pyrrolysine';

--- ************************************************
--- *** relation: transcribed_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region defined by a set of transcribed ***
--- ***  sequences from the same gene or express ***
--- *** ed pseudogene.                           ***
--- ************************************************
---

CREATE VIEW transcribed_cluster AS
  SELECT
    feature_id AS transcribed_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'unigene_cluster' OR cvterm.name = 'transcribed_cluster';

--- ************************************************
--- *** relation: unigene_cluster ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kind of transcribed_cluster defined by ***
--- ***  a set of transcribed sequences from the ***
--- ***  a unique gene.                          ***
--- ************************************************
---

CREATE VIEW unigene_cluster AS
  SELECT
    feature_id AS unigene_cluster_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'unigene_cluster';

--- ************************************************
--- *** relation: crispr ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Clustered Palindromic Repeats interspers ***
--- *** ed with bacteriophage derived spacer seq ***
--- *** uences.                                  ***
--- ************************************************
---

CREATE VIEW crispr AS
  SELECT
    feature_id AS crispr_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'CRISPR';

--- ************************************************
--- *** relation: insulator_binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in an insulator reg ***
--- *** ion of a nucleotide molecule, interacts  ***
--- *** selectively and non-covalently with poly ***
--- *** peptide residues.                        ***
--- ************************************************
---

CREATE VIEW insulator_binding_site AS
  SELECT
    feature_id AS insulator_binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'insulator_binding_site';

--- ************************************************
--- *** relation: enhancer_binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the enhancer reg ***
--- *** ion of a nucleotide molecule, interacts  ***
--- *** selectively and non-covalently with poly ***
--- *** peptide residues.                        ***
--- ************************************************
---

CREATE VIEW enhancer_binding_site AS
  SELECT
    feature_id AS enhancer_binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'enhancer_binding_site';

--- ************************************************
--- *** relation: contig_collection ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A collection of contigs.                 ***
--- ************************************************
---

CREATE VIEW contig_collection AS
  SELECT
    feature_id AS contig_collection_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'contig_collection';

--- ************************************************
--- *** relation: lincrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A multiexonic non-coding RNA transcribed ***
--- ***  by RNA polymerase II.                   ***
--- ************************************************
---

CREATE VIEW lincrna AS
  SELECT
    feature_id AS lincrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'lincRNA';

--- ************************************************
--- *** relation: ust ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An EST spanning part or all of the untra ***
--- *** nslated regions of a protein-coding tran ***
--- *** script.                                  ***
--- ************************************************
---

CREATE VIEW ust AS
  SELECT
    feature_id AS ust_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_UST' OR cvterm.name = 'five_prime_UST' OR cvterm.name = 'UST';

--- ************************************************
--- *** relation: three_prime_ust ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A UST located in the 3'UTR of a protein- ***
--- *** coding transcript.                       ***
--- ************************************************
---

CREATE VIEW three_prime_ust AS
  SELECT
    feature_id AS three_prime_ust_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_UST';

--- ************************************************
--- *** relation: five_prime_ust ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An UST located in the 5'UTR of a protein ***
--- *** -coding transcript.                      ***
--- ************************************************
---

CREATE VIEW five_prime_ust AS
  SELECT
    feature_id AS five_prime_ust_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_UST';

--- ************************************************
--- *** relation: rst ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tag produced from a single sequencing  ***
--- *** read from a RACE product; typically a fe ***
--- *** w hundred base pairs long.               ***
--- ************************************************
---

CREATE VIEW rst AS
  SELECT
    feature_id AS rst_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_RST' OR cvterm.name = 'five_prime_RST' OR cvterm.name = 'RST';

--- ************************************************
--- *** relation: three_prime_rst ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tag produced from a single sequencing  ***
--- *** read from a 3'-RACE product; typically a ***
--- ***  few hundred base pairs long.            ***
--- ************************************************
---

CREATE VIEW three_prime_rst AS
  SELECT
    feature_id AS three_prime_rst_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_RST';

--- ************************************************
--- *** relation: five_prime_rst ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tag produced from a single sequencing  ***
--- *** read from a 5'-RACE product; typically a ***
--- ***  few hundred base pairs long.            ***
--- ************************************************
---

CREATE VIEW five_prime_rst AS
  SELECT
    feature_id AS five_prime_rst_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_RST';

--- ************************************************
--- *** relation: ust_match ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A match against an UST sequence.         ***
--- ************************************************
---

CREATE VIEW ust_match AS
  SELECT
    feature_id AS ust_match_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'UST_match';

--- ************************************************
--- *** relation: rst_match ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A match against an RST sequence.         ***
--- ************************************************
---

CREATE VIEW rst_match AS
  SELECT
    feature_id AS rst_match_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RST_match';

--- ************************************************
--- *** relation: primer_match ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A nucleotide match to a primer sequence. ***
--- ************************************************
---

CREATE VIEW primer_match AS
  SELECT
    feature_id AS primer_match_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'primer_match';

--- ************************************************
--- *** relation: mirna_antiguide ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of the pri miRNA that basepairs ***
--- ***  with the guide to form the hairpin.     ***
--- ************************************************
---

CREATE VIEW mirna_antiguide AS
  SELECT
    feature_id AS mirna_antiguide_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'miRNA_antiguide';

--- ************************************************
--- *** relation: trans_splice_junction ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The boundary between the spliced leader  ***
--- *** and the first exon of the mRNA.          ***
--- ************************************************
---

CREATE VIEW trans_splice_junction AS
  SELECT
    feature_id AS trans_splice_junction_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'trans_splice_junction';

--- ************************************************
--- *** relation: outron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a primary transcript, that i ***
--- *** s removed via trans splicing.            ***
--- ************************************************
---

CREATE VIEW outron AS
  SELECT
    feature_id AS outron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'outron';

--- ************************************************
--- *** relation: natural_plasmid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A plasmid that occurs naturally.         ***
--- ************************************************
---

CREATE VIEW natural_plasmid AS
  SELECT
    feature_id AS natural_plasmid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'natural_plasmid';

--- ************************************************
--- *** relation: gene_trap_construct ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene trap construct is a type of engin ***
--- *** eered plasmid which is designed to integ ***
--- *** rate into a genome and produce a fusion  ***
--- *** transcript between exons of the gene int ***
--- *** o which it inserts and a reporter elemen ***
--- *** t in the construct. Gene traps contain a ***
--- ***  splice acceptor, do not contain promote ***
--- *** r elements for the reporter, and are mut ***
--- *** agenic. Gene traps may be bicistronic wi ***
--- *** th the second cassette containing a prom ***
--- *** oter driving an a selectable marker.     ***
--- ************************************************
---

CREATE VIEW gene_trap_construct AS
  SELECT
    feature_id AS gene_trap_construct_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_trap_construct';

--- ************************************************
--- *** relation: promoter_trap_construct ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A promoter trap construct is a type of e ***
--- *** ngineered plasmid which is designed to i ***
--- *** ntegrate into a genome and express a rep ***
--- *** orter when inserted in close proximity t ***
--- *** o a promoter element. Promoter traps typ ***
--- *** ically do not contain promoter elements  ***
--- *** and are mutagenic.                       ***
--- ************************************************
---

CREATE VIEW promoter_trap_construct AS
  SELECT
    feature_id AS promoter_trap_construct_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'promoter_trap_construct';

--- ************************************************
--- *** relation: enhancer_trap_construct ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An enhancer trap construct is a type of  ***
--- *** engineered plasmid which is designed to  ***
--- *** integrate into a genome and express a re ***
--- *** porter when the expression from a basic  ***
--- *** minimal promoter is enhanced by genomic  ***
--- *** enhancer elements. Enhancer traps contai ***
--- *** n promoter elements and are not usually  ***
--- *** mutagenic.                               ***
--- ************************************************
---

CREATE VIEW enhancer_trap_construct AS
  SELECT
    feature_id AS enhancer_trap_construct_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'enhancer_trap_construct';

--- ************************************************
--- *** relation: pac_end ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of sequence from the end of a P ***
--- *** AC clone that may provide a highly speci ***
--- *** fic marker.                              ***
--- ************************************************
---

CREATE VIEW pac_end AS
  SELECT
    feature_id AS pac_end_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'PAC_end';

--- ************************************************
--- *** relation: rapd ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** RAPD is a 'PCR product' where a sequence ***
--- ***  variant is identified through the use o ***
--- *** f PCR with random primers.               ***
--- ************************************************
---

CREATE VIEW rapd AS
  SELECT
    feature_id AS rapd_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RAPD';

--- ************************************************
--- *** relation: shadow_enhancer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW shadow_enhancer AS
  SELECT
    feature_id AS shadow_enhancer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'shadow_enhancer';

--- ************************************************
--- *** relation: snv ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** SNVs are single nucleotide positions in  ***
--- *** genomic DNA at which different sequence  ***
--- *** alternatives exist.                      ***
--- ************************************************
---

CREATE VIEW snv AS
  SELECT
    feature_id AS snv_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SNP' OR cvterm.name = 'point_mutation' OR cvterm.name = 'transition' OR cvterm.name = 'transversion' OR cvterm.name = 'pyrimidine_transition' OR cvterm.name = 'purine_transition' OR cvterm.name = 'C_to_T_transition' OR cvterm.name = 'T_to_C_transition' OR cvterm.name = 'C_to_T_transition_at_pCpG_site' OR cvterm.name = 'A_to_G_transition' OR cvterm.name = 'G_to_A_transition' OR cvterm.name = 'pyrimidine_to_purine_transversion' OR cvterm.name = 'purine_to_pyrimidine_transversion' OR cvterm.name = 'C_to_A_transversion' OR cvterm.name = 'C_to_G_transversion' OR cvterm.name = 'T_to_A_transversion' OR cvterm.name = 'T_to_G_transversion' OR cvterm.name = 'A_to_C_transversion' OR cvterm.name = 'A_to_T_transversion' OR cvterm.name = 'G_to_C_transversion' OR cvterm.name = 'G_to_T_transversion' OR cvterm.name = 'SNV';

--- ************************************************
--- *** relation: x_element_combinatorial_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An X element combinatorial repeat is a r ***
--- *** epeat region located between the X eleme ***
--- *** nt and the telomere or adjacent Y' eleme ***
--- *** nt.                                      ***
--- ************************************************
---

CREATE VIEW x_element_combinatorial_repeat AS
  SELECT
    feature_id AS x_element_combinatorial_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'X_element_combinatorial_repeat';

--- ************************************************
--- *** relation: y_prime_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A Y' element is a repeat region (SO:0000 ***
--- *** 657) located adjacent to telomeric repea ***
--- *** ts or X element combinatorial repeats, e ***
--- *** ither as a single copy or tandem repeat  ***
--- *** of two to four copies.                   ***
--- ************************************************
---

CREATE VIEW y_prime_element AS
  SELECT
    feature_id AS y_prime_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'Y_prime_element';

--- ************************************************
--- *** relation: standard_draft ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The status of a whole genome sequence, w ***
--- *** here the data is minimally filtered or u ***
--- *** n-filtered, from any number of sequencin ***

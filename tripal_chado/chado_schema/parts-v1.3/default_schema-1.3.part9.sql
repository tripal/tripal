SET search_path=so,chado,pg_catalog;
---

CREATE VIEW chromosome AS
  SELECT
    feature_id AS chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mitochondrial_chromosome' OR cvterm.name = 'chloroplast_chromosome' OR cvterm.name = 'chromoplast_chromosome' OR cvterm.name = 'cyanelle_chromosome' OR cvterm.name = 'leucoplast_chromosome' OR cvterm.name = 'macronuclear_chromosome' OR cvterm.name = 'micronuclear_chromosome' OR cvterm.name = 'nuclear_chromosome' OR cvterm.name = 'nucleomorphic_chromosome' OR cvterm.name = 'DNA_chromosome' OR cvterm.name = 'RNA_chromosome' OR cvterm.name = 'apicoplast_chromosome' OR cvterm.name = 'double_stranded_DNA_chromosome' OR cvterm.name = 'single_stranded_DNA_chromosome' OR cvterm.name = 'linear_double_stranded_DNA_chromosome' OR cvterm.name = 'circular_double_stranded_DNA_chromosome' OR cvterm.name = 'linear_single_stranded_DNA_chromosome' OR cvterm.name = 'circular_single_stranded_DNA_chromosome' OR cvterm.name = 'single_stranded_RNA_chromosome' OR cvterm.name = 'double_stranded_RNA_chromosome' OR cvterm.name = 'linear_single_stranded_RNA_chromosome' OR cvterm.name = 'circular_single_stranded_RNA_chromosome' OR cvterm.name = 'linear_double_stranded_RNA_chromosome' OR cvterm.name = 'circular_double_stranded_RNA_chromosome' OR cvterm.name = 'chromosome';

--- ************************************************
--- *** relation: chromosome_band ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cytologically distinguishable feature  ***
--- *** of a chromosome, often made visible by s ***
--- *** taining, and usually alternating light a ***
--- *** nd dark.                                 ***
--- ************************************************
---

CREATE VIEW chromosome_band AS
  SELECT
    feature_id AS chromosome_band_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chromosome_band';

--- ************************************************
--- *** relation: site_specific_recombination_target_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW site_specific_recombination_target_region AS
  SELECT
    feature_id AS site_specific_recombination_target_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'integration_excision_site' OR cvterm.name = 'resolution_site' OR cvterm.name = 'inversion_site' OR cvterm.name = 'inversion_site_part' OR cvterm.name = 'attI_site' OR cvterm.name = 'attP_site' OR cvterm.name = 'attB_site' OR cvterm.name = 'attL_site' OR cvterm.name = 'attR_site' OR cvterm.name = 'attC_site' OR cvterm.name = 'attCtn_site' OR cvterm.name = 'loxP_site' OR cvterm.name = 'dif_site' OR cvterm.name = 'FRT_site' OR cvterm.name = 'IRLinv_site' OR cvterm.name = 'IRRinv_site' OR cvterm.name = 'site_specific_recombination_target_region';

--- ************************************************
--- *** relation: match ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of sequence, aligned to another ***
--- ***  sequence with some statistical signific ***
--- *** ance, using an algorithm such as BLAST o ***
--- *** r SIM4.                                  ***
--- ************************************************
---

CREATE VIEW match AS
  SELECT
    feature_id AS match_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nucleotide_match' OR cvterm.name = 'protein_match' OR cvterm.name = 'expressed_sequence_match' OR cvterm.name = 'cross_genome_match' OR cvterm.name = 'translated_nucleotide_match' OR cvterm.name = 'primer_match' OR cvterm.name = 'EST_match' OR cvterm.name = 'cDNA_match' OR cvterm.name = 'UST_match' OR cvterm.name = 'RST_match' OR cvterm.name = 'match';

--- ************************************************
--- *** relation: splice_enhancer ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Region of a transcript that regulates sp ***
--- *** licing.                                  ***
--- ************************************************
---

CREATE VIEW splice_enhancer AS
  SELECT
    feature_id AS splice_enhancer_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'intronic_splice_enhancer' OR cvterm.name = 'exonic_splice_enhancer' OR cvterm.name = 'splice_enhancer';

--- ************************************************
--- *** relation: est ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A tag produced from a single sequencing  ***
--- *** read from a cDNA clone or PCR product; t ***
--- *** ypically a few hundred base pairs long.  ***
--- ************************************************
---

CREATE VIEW est AS
  SELECT
    feature_id AS est_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_EST' OR cvterm.name = 'three_prime_EST' OR cvterm.name = 'UST' OR cvterm.name = 'RST' OR cvterm.name = 'three_prime_UST' OR cvterm.name = 'five_prime_UST' OR cvterm.name = 'three_prime_RST' OR cvterm.name = 'five_prime_RST' OR cvterm.name = 'EST';

--- ************************************************
--- *** relation: loxp_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW loxp_site AS
  SELECT
    feature_id AS loxp_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'loxP_site';

--- ************************************************
--- *** relation: nucleotide_match ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A match against a nucleotide sequence.   ***
--- ************************************************
---

CREATE VIEW nucleotide_match AS
  SELECT
    feature_id AS nucleotide_match_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'expressed_sequence_match' OR cvterm.name = 'cross_genome_match' OR cvterm.name = 'translated_nucleotide_match' OR cvterm.name = 'primer_match' OR cvterm.name = 'EST_match' OR cvterm.name = 'cDNA_match' OR cvterm.name = 'UST_match' OR cvterm.name = 'RST_match' OR cvterm.name = 'nucleotide_match';

--- ************************************************
--- *** relation: nucleic_acid ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence consi ***
--- *** sting of nucleobases bound to repeating  ***
--- *** units. The forms found in nature are deo ***
--- *** xyribonucleic acid (DNA), where the repe ***
--- *** ating units are 2-deoxy-D-ribose rings c ***
--- *** onnected to a phosphate backbone, and ri ***
--- *** bonucleic acid (RNA), where the repeatin ***
--- *** g units are D-ribose rings connected to  ***
--- *** a phosphate backbone.                    ***
--- ************************************************
---

CREATE VIEW nucleic_acid AS
  SELECT
    feature_id AS nucleic_acid_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DNA' OR cvterm.name = 'RNA' OR cvterm.name = 'morpholino_backbone' OR cvterm.name = 'PNA' OR cvterm.name = 'LNA' OR cvterm.name = 'TNA' OR cvterm.name = 'GNA' OR cvterm.name = 'cDNA' OR cvterm.name = 'genomic_DNA' OR cvterm.name = 'single_stranded_cDNA' OR cvterm.name = 'double_stranded_cDNA' OR cvterm.name = 'R_GNA' OR cvterm.name = 'S_GNA' OR cvterm.name = 'nucleic_acid';

--- ************************************************
--- *** relation: protein_match ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A match against a protein sequence.      ***
--- ************************************************
---

CREATE VIEW protein_match AS
  SELECT
    feature_id AS protein_match_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'protein_match';

--- ************************************************
--- *** relation: frt_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An inversion site found on the Saccharom ***
--- *** yces cerevisiae 2 micron plasmid.        ***
--- ************************************************
---

CREATE VIEW frt_site AS
  SELECT
    feature_id AS frt_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'FRT_site';

--- ************************************************
--- *** relation: synthetic_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to decide a sequence of nuc ***
--- *** leotides, nucleotide analogs, or amino a ***
--- *** cids that has been designed by an experi ***
--- *** menter and which may, or may not, corres ***
--- *** pond with any natural sequence.          ***
--- ************************************************
---

CREATE VIEW synthetic_sequence AS
  SELECT
    feature_id AS synthetic_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'random_sequence' OR cvterm.name = 'designed_sequence' OR cvterm.name = 'synthetic_sequence';

--- ************************************************
--- *** relation: dna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence consi ***
--- *** sting of nucleobases bound to a repeatin ***
--- *** g unit made of a 2-deoxy-D-ribose ring c ***
--- *** onnected to a phosphate backbone.        ***
--- ************************************************
---

CREATE VIEW dna AS
  SELECT
    feature_id AS dna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cDNA' OR cvterm.name = 'genomic_DNA' OR cvterm.name = 'single_stranded_cDNA' OR cvterm.name = 'double_stranded_cDNA' OR cvterm.name = 'DNA';

--- ************************************************
--- *** relation: sequence_assembly ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence of nucleotides that has been  ***
--- *** algorithmically derived from an alignmen ***
--- *** t of two or more different sequences.    ***
--- ************************************************
---

CREATE VIEW sequence_assembly AS
  SELECT
    feature_id AS sequence_assembly_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'supercontig' OR cvterm.name = 'contig' OR cvterm.name = 'tiling_path' OR cvterm.name = 'virtual_sequence' OR cvterm.name = 'golden_path' OR cvterm.name = 'ultracontig' OR cvterm.name = 'expressed_sequence_assembly' OR cvterm.name = 'sequence_assembly';

--- ************************************************
--- *** relation: group_1_intron_homing_endonuclease_target_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of intronic nucleotide sequence ***
--- ***  targeted by a nuclease enzyme.          ***
--- ************************************************
---

CREATE VIEW group_1_intron_homing_endonuclease_target_region AS
  SELECT
    feature_id AS group_1_intron_homing_endonuclease_target_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'group_1_intron_homing_endonuclease_target_region';

--- ************************************************
--- *** relation: haplotype_block ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of the genome which is co-inher ***
--- *** ited as the result of the lack of histor ***
--- *** ic recombination within it.              ***
--- ************************************************
---

CREATE VIEW haplotype_block AS
  SELECT
    feature_id AS haplotype_block_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'haplotype_block';

--- ************************************************
--- *** relation: rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a sequence consi ***
--- *** sting of nucleobases bound to a repeatin ***
--- *** g unit made of a D-ribose ring connected ***
--- ***  to a phosphate backbone.                ***
--- ************************************************
---

CREATE VIEW rna AS
  SELECT
    feature_id AS rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNA';

--- ************************************************
--- *** relation: flanked ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a region that is ***
--- ***  bounded either side by a particular kin ***
--- *** d of region.                             ***
--- ************************************************
---

CREATE VIEW flanked AS
  SELECT
    feature_id AS flanked_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'floxed' OR cvterm.name = 'FRT_flanked' OR cvterm.name = 'flanked';

--- ************************************************
--- *** relation: floxed ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing sequence that is ***
--- ***  flanked by Lox-P sites.                 ***
--- ************************************************
---

CREATE VIEW floxed AS
  SELECT
    feature_id AS floxed_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'floxed';

--- ************************************************
--- *** relation: codon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A set of (usually) three nucleotide base ***
--- *** s in a DNA or RNA sequence, which togeth ***
--- *** er code for a unique amino acid or the t ***
--- *** ermination of translation and are contai ***
--- *** ned within the CDS.                      ***
--- ************************************************
---

CREATE VIEW codon AS
  SELECT
    feature_id AS codon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'recoded_codon' OR cvterm.name = 'start_codon' OR cvterm.name = 'stop_codon' OR cvterm.name = 'stop_codon_read_through' OR cvterm.name = 'stop_codon_redefined_as_pyrrolysine' OR cvterm.name = 'stop_codon_redefined_as_selenocysteine' OR cvterm.name = 'non_canonical_start_codon' OR cvterm.name = 'four_bp_start_codon' OR cvterm.name = 'CTG_start_codon' OR cvterm.name = 'codon';

--- ************************************************
--- *** relation: frt_flanked ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe sequence that i ***
--- *** s flanked by the FLP recombinase recogni ***
--- *** tion site, FRT.                          ***
--- ************************************************
---

CREATE VIEW frt_flanked AS
  SELECT
    feature_id AS frt_flanked_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'FRT_flanked';

--- ************************************************
--- *** relation: invalidated_by_chimeric_cdna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A cDNA clone constructed from more than  ***
--- *** one mRNA. Usually an experimental artifa ***
--- *** ct.                                      ***
--- ************************************************
---

CREATE VIEW invalidated_by_chimeric_cdna AS
  SELECT
    feature_id AS invalidated_by_chimeric_cdna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'invalidated_by_chimeric_cDNA';

--- ************************************************
--- *** relation: floxed_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transgene that is floxed.              ***
--- ************************************************
---

CREATE VIEW floxed_gene AS
  SELECT
    feature_id AS floxed_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'floxed_gene';

--- ************************************************
--- *** relation: transposable_element_flanking_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The region of sequence surrounding a tra ***
--- *** nsposable element.                       ***
--- ************************************************
---

CREATE VIEW transposable_element_flanking_region AS
  SELECT
    feature_id AS transposable_element_flanking_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transposable_element_flanking_region';

--- ************************************************
--- *** relation: integron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region encoding an integrase which act ***
--- *** s at a site adjacent to it (attI_site) t ***
--- *** o insert DNA which must include but is n ***
--- *** ot limited to an attC_site.              ***
--- ************************************************
---

CREATE VIEW integron AS
  SELECT
    feature_id AS integron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'integron';

--- ************************************************
--- *** relation: insertion_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The junction where an insertion occurred ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW insertion_site AS
  SELECT
    feature_id AS insertion_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transposable_element_insertion_site' OR cvterm.name = 'insertion_site';

--- ************************************************
--- *** relation: atti_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region within an integron, adjacent to ***
--- ***  an integrase, at which site specific re ***
--- *** combination involving an attC_site takes ***
--- ***  place.                                  ***
--- ************************************************
---

CREATE VIEW atti_site AS
  SELECT
    feature_id AS atti_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'attI_site';

--- ************************************************
--- *** relation: transposable_element_insertion_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The junction in a genome where a transpo ***
--- *** sable_element has inserted.              ***
--- ************************************************
---

CREATE VIEW transposable_element_insertion_site AS
  SELECT
    feature_id AS transposable_element_insertion_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transposable_element_insertion_site';

--- ************************************************
--- *** relation: small_regulatory_ncrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A non-coding RNA, usually with a specifi ***
--- *** c secondary structure, that acts to regu ***
--- *** late gene expression.                    ***
--- ************************************************
---

CREATE VIEW small_regulatory_ncrna AS
  SELECT
    feature_id AS small_regulatory_ncrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'miRNA' OR cvterm.name = 'RNA_6S' OR cvterm.name = 'CsrB_RsmB_RNA' OR cvterm.name = 'DsrA_RNA' OR cvterm.name = 'OxyS_RNA' OR cvterm.name = 'RprA_RNA' OR cvterm.name = 'RRE_RNA' OR cvterm.name = 'spot_42_RNA' OR cvterm.name = 'tmRNA' OR cvterm.name = 'GcvB_RNA' OR cvterm.name = 'small_regulatory_ncRNA';

--- ************************************************
--- *** relation: conjugative_transposon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transposon that encodes function requi ***
--- *** red for conjugation.                     ***
--- ************************************************
---

CREATE VIEW conjugative_transposon AS
  SELECT
    feature_id AS conjugative_transposon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'conjugative_transposon';

--- ************************************************
--- *** relation: enzymatic_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An RNA sequence that has catalytic activ ***
--- *** ity with or without an associated ribonu ***
--- *** cleoprotein.                             ***
--- ************************************************
---

CREATE VIEW enzymatic_rna AS
  SELECT
    feature_id AS enzymatic_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ribozyme' OR cvterm.name = 'enzymatic_RNA';

--- ************************************************
--- *** relation: recombinationally_inverted_gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A recombinationally rearranged gene by i ***
--- *** nversion.                                ***
--- ************************************************
---

CREATE VIEW recombinationally_inverted_gene AS
  SELECT
    feature_id AS recombinationally_inverted_gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'recombinationally_inverted_gene';

--- ************************************************
--- *** relation: ribozyme ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An RNA with catalytic activity.          ***
--- ************************************************
---

CREATE VIEW ribozyme AS
  SELECT
    feature_id AS ribozyme_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ribozyme';

--- ************************************************
--- *** relation: rrna_5_8s ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** 5_8S ribosomal RNA (5. 8S rRNA) is a com ***
--- *** ponent of the large subunit of the eukar ***
--- *** yotic ribosome. It is transcribed by RNA ***
--- ***  polymerase I as part of the 45S precurs ***
--- *** or that also contains 18S and 28S rRNA.  ***
--- *** Functionally, it is thought that 5.8S rR ***
--- *** NA may be involved in ribosome transloca ***
--- *** tion. It is also known to form covalent  ***
--- *** linkage to the p53 tumour suppressor pro ***
--- *** tein. 5_8S rRNA is also found in archaea ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW rrna_5_8s AS
  SELECT
    feature_id AS rrna_5_8s_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_5_8S';

--- ************************************************
--- *** relation: rna_6s ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A small (184-nt in E. coli) RNA that for ***
--- *** ms a hairpin type structure. 6S RNA asso ***
--- *** ciates with RNA polymerase in a highly s ***
--- *** pecific manner. 6S RNA represses express ***
--- *** ion from a sigma70-dependent promoter du ***
--- *** ring stationary phase.                   ***
--- ************************************************
---

CREATE VIEW rna_6s AS
  SELECT
    feature_id AS rna_6s_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNA_6S';

--- ************************************************
--- *** relation: csrb_rsmb_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An enterobacterial RNA that binds the Cs ***
--- *** rA protein. The CsrB RNAs contain a cons ***
--- *** erved motif CAGGXXG that is found in up  ***
--- *** to 18 copies and has been suggested to b ***
--- *** ind CsrA. The Csr regulatory system has  ***
--- *** a strong negative regulatory effect on g ***
--- *** lycogen biosynthesis, glyconeogenesis an ***
--- *** d glycogen catabolism and a positive reg ***
--- *** ulatory effect on glycolysis. In other b ***
--- *** acteria such as Erwinia caratovara the R ***
--- *** smA protein has been shown to regulate t ***
--- *** he production of virulence determinants, ***
--- ***  such extracellular enzymes. RsmA binds  ***
--- *** to RsmB regulatory RNA which is also a m ***
--- *** ember of this family.                    ***
--- ************************************************
---

CREATE VIEW csrb_rsmb_rna AS
  SELECT
    feature_id AS csrb_rsmb_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'CsrB_RsmB_RNA';

--- ************************************************
--- *** relation: dsra_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** DsrA RNA regulates both transcription, b ***
--- *** y overcoming transcriptional silencing b ***
--- *** y the nucleoid-associated H-NS protein,  ***
--- *** and translation, by promoting efficient  ***
--- *** translation of the stress sigma factor,  ***
--- *** RpoS. These two activities of DsrA can b ***
--- *** e separated by mutation: the first of th ***
--- *** ree stem-loops of the 85 nucleotide RNA  ***
--- *** is necessary for RpoS translation but no ***
--- *** t for anti-H-NS action, while the second ***
--- ***  stem-loop is essential for antisilencin ***
--- *** g and less critical for RpoS translation ***
--- *** . The third stem-loop, which behaves as  ***
--- *** a transcription terminator, can be subst ***
--- *** ituted by the trp transcription terminat ***
--- *** or without loss of either DsrA function. ***
--- ***  The sequence of the first stem-loop of  ***
--- *** DsrA is complementary with the upstream  ***
--- *** leader portion of RpoS messenger RNA, su ***
--- *** ggesting that pairing of DsrA with the R ***
--- *** poS message might be important for trans ***
--- *** lational regulation.                     ***
--- ************************************************
---

CREATE VIEW dsra_rna AS
  SELECT
    feature_id AS dsra_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'GcvB_RNA' OR cvterm.name = 'DsrA_RNA';

--- ************************************************
--- *** relation: gcvb_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A small untranslated RNA involved in exp ***
--- *** ression of the dipeptide and oligopeptid ***
--- *** e transport systems in Escherichia coli. ***
--- ************************************************
---

CREATE VIEW gcvb_rna AS
  SELECT
    feature_id AS gcvb_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'GcvB_RNA';

--- ************************************************
--- *** relation: hammerhead_ribozyme ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A small catalytic RNA motif that catalyz ***
--- *** es self-cleavage reaction. Its name come ***
--- *** s from its secondary structure which res ***
--- *** embles a carpenter's hammer. The hammerh ***
--- *** ead ribozyme is involved in the replicat ***
--- *** ion of some viroid and some satellite RN ***
--- *** As.                                      ***
--- ************************************************
---

CREATE VIEW hammerhead_ribozyme AS
  SELECT
    feature_id AS hammerhead_ribozyme_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'hammerhead_ribozyme';

--- ************************************************
--- *** relation: group_iia_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW group_iia_intron AS
  SELECT
    feature_id AS group_iia_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'group_IIA_intron';

--- ************************************************
--- *** relation: group_iib_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW group_iib_intron AS
  SELECT
    feature_id AS group_iib_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'group_IIB_intron';

--- ************************************************
--- *** relation: micf_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A non-translated 93 nt antisense RNA tha ***
--- *** t binds its target ompF mRNA and regulat ***
--- *** es ompF expression by inhibiting transla ***
--- *** tion and inducing degradation of the mes ***
--- *** sage.                                    ***
--- ************************************************
---

CREATE VIEW micf_rna AS
  SELECT
    feature_id AS micf_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'MicF_RNA';

--- ************************************************
--- *** relation: oxys_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A small untranslated RNA which is induce ***
--- *** d in response to oxidative stress in Esc ***
--- *** herichia coli. Acts as a global regulato ***
--- *** r to activate or repress the expression  ***
--- *** of as many as 40 genes, including the fh ***
--- *** lA-encoded transcriptional activator and ***
--- ***  the rpoS-encoded sigma(s) subunit of RN ***
--- *** A polymerase. OxyS is bound by the Hfq p ***
--- *** rotein, that increases the OxyS RNA inte ***
--- *** raction with its target messages.        ***
--- ************************************************
---

CREATE VIEW oxys_rna AS
  SELECT
    feature_id AS oxys_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'OxyS_RNA';

--- ************************************************
--- *** relation: rnase_mrp_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The RNA molecule essential for the catal ***
--- *** ytic activity of RNase MRP, an enzymatic ***
--- *** ally active ribonucleoprotein with two d ***
--- *** istinct roles in eukaryotes. In mitochon ***
--- *** dria it plays a direct role in the initi ***
--- *** ation of mitochondrial DNA replication.  ***
--- *** In the nucleus it is involved in precurs ***
--- *** or rRNA processing, where it cleaves the ***
--- ***  internal transcribed spacer 1 between 1 ***
--- *** 8S and 5.8S rRNAs.                       ***
--- ************************************************
---

CREATE VIEW rnase_mrp_rna AS
  SELECT
    feature_id AS rnase_mrp_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNase_MRP_RNA';

--- ************************************************
--- *** relation: rnase_p_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The RNA component of Ribonuclease P (RNa ***
--- *** se P), a ubiquitous endoribonuclease, fo ***
--- *** und in archaea, bacteria and eukarya as  ***
--- *** well as chloroplasts and mitochondria. I ***
--- *** ts best characterized activity is the ge ***
--- *** neration of mature 5 prime ends of tRNAs ***
--- ***  by cleaving the 5 prime leader elements ***
--- ***  of precursor-tRNAs. Cellular RNase Ps a ***
--- *** re ribonucleoproteins. RNA from bacteria ***
--- *** l RNase Ps retains its catalytic activit ***
--- *** y in the absence of the protein subunit, ***
--- ***  i.e. it is a ribozyme. Isolated eukaryo ***
--- *** tic and archaeal RNase P RNA has not bee ***
--- *** n shown to retain its catalytic function ***
--- *** , but is still essential for the catalyt ***
--- *** ic activity of the holoenzyme. Although  ***
--- *** the archaeal and eukaryotic holoenzymes  ***
--- *** have a much greater protein content than ***
--- ***  the bacterial ones, the RNA cores from  ***
--- *** all the three lineages are homologous. H ***
--- *** elices corresponding to P1, P2, P3, P4,  ***
--- *** and P10/11 are common to all cellular RN ***
--- *** ase P RNAs. Yet, there is considerable s ***
--- *** equence variation, particularly among th ***
--- *** e eukaryotic RNAs.                       ***
--- ************************************************
---

CREATE VIEW rnase_p_rna AS
  SELECT
    feature_id AS rnase_p_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNase_P_RNA';

--- ************************************************
--- *** relation: rpra_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Translational regulation of the stationa ***
--- *** ry phase sigma factor RpoS is mediated b ***
--- *** y the formation of a double-stranded RNA ***
--- ***  stem-loop structure in the upstream reg ***
--- *** ion of the rpoS messenger RNA, occluding ***
--- ***  the translation initiation site. Clones ***
--- ***  carrying rprA (RpoS regulator RNA) incr ***
--- *** eased the translation of RpoS. The rprA  ***
--- *** gene encodes a 106 nucleotide regulatory ***
--- ***  RNA. As with DsrA Rfam:RF00014, RprA is ***
--- ***  predicted to form three stem-loops. Thu ***
--- *** s, at least two small RNAs, DsrA and Rpr ***
--- *** A, participate in the positive regulatio ***
--- *** n of RpoS translation. Unlike DsrA, RprA ***
--- ***  does not have an extensive region of co ***
--- *** mplementarity to the RpoS leader, leavin ***
--- *** g its mechanism of action unclear. RprA  ***
--- *** is non-essential.                        ***
--- ************************************************
---

CREATE VIEW rpra_rna AS
  SELECT
    feature_id AS rpra_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RprA_RNA';

--- ************************************************
--- *** relation: rre_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The Rev response element (RRE) is encode ***
--- *** d within the HIV-env gene. Rev is an ess ***
--- *** ential regulatory protein of HIV that bi ***
--- *** nds an internal loop of the RRE leading, ***
--- ***  encouraging further Rev-RRE binding. Th ***
--- *** is RNP complex is critical for mRNA expo ***
--- *** rt and hence for expression of the HIV s ***
--- *** tructural proteins.                      ***
--- ************************************************
---

CREATE VIEW rre_rna AS
  SELECT
    feature_id AS rre_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RRE_RNA';

--- ************************************************
--- *** relation: spot_42_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A 109-nucleotide RNA of E. coli that see ***
--- *** ms to have a regulatory role on the gala ***
--- *** ctose operon. Changes in Spot 42 levels  ***
--- *** are implicated in affecting DNA polymera ***
--- *** se I levels.                             ***
--- ************************************************
---

CREATE VIEW spot_42_rna AS
  SELECT
    feature_id AS spot_42_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'spot_42_RNA';

--- ************************************************
--- *** relation: telomerase_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The RNA component of telomerase, a rever ***
--- *** se transcriptase that synthesizes telome ***
--- *** ric DNA.                                 ***
--- ************************************************
---

CREATE VIEW telomerase_rna AS
  SELECT
    feature_id AS telomerase_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'telomerase_RNA';

--- ************************************************
--- *** relation: u1_snrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** U1 is a small nuclear RNA (snRNA) compon ***
--- *** ent of the spliceosome (involved in pre- ***
--- *** mRNA splicing). Its 5' end forms complem ***
--- *** entary base pairs with the 5' splice jun ***
--- *** ction, thus defining the 5' donor site o ***
--- *** f an intron. There are significant diffe ***
--- *** rences in sequence and secondary structu ***
--- *** re between metazoan and yeast U1 snRNAs, ***
--- ***  the latter being much longer (568 nucle ***
--- *** otides as compared to 164 nucleotides in ***
--- ***  human). Nevertheless, secondary structu ***
--- *** re predictions suggest that all U1 snRNA ***
--- *** s share a 'common core' consisting of he ***
--- *** lices I, II, the proximal region of III, ***
--- ***  and IV.                                 ***
--- ************************************************
---

CREATE VIEW u1_snrna AS
  SELECT
    feature_id AS u1_snrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U1_snRNA';

--- ************************************************
--- *** relation: u2_snrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***

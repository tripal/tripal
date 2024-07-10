SET search_path=so,chado,pg_catalog;
--- *** U2 is a small nuclear RNA (snRNA) compon ***
--- *** ent of the spliceosome (involved in pre- ***
--- *** mRNA splicing). Complementary binding be ***
--- *** tween U2 snRNA (in an area lying towards ***
--- ***  the 5' end but 3' to hairpin I) and the ***
--- ***  branchpoint sequence (BPS) of the intro ***
--- *** n results in the bulging out of an unpai ***
--- *** red adenine, on the BPS, which initiates ***
--- ***  a nucleophilic attack at the intronic 5 ***
--- *** ' splice site, thus starting the first o ***
--- *** f two transesterification reactions that ***
--- ***  mediate splicing.                       ***
--- ************************************************
---

CREATE VIEW u2_snrna AS
  SELECT
    feature_id AS u2_snrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U2_snRNA';

--- ************************************************
--- *** relation: u4_snrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** U4 small nuclear RNA (U4 snRNA) is a com ***
--- *** ponent of the major U2-dependent spliceo ***
--- *** some. It forms a duplex with U6, and wit ***
--- *** h each splicing round, it is displaced f ***
--- *** rom U6 (and the spliceosome) in an ATP-d ***
--- *** ependent manner, allowing U6 to refold a ***
--- *** nd create the active site for splicing c ***
--- *** atalysis. A recycling process involving  ***
--- *** protein Prp24 re-anneals U4 and U6.      ***
--- ************************************************
---

CREATE VIEW u4_snrna AS
  SELECT
    feature_id AS u4_snrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U4_snRNA';

--- ************************************************
--- *** relation: u4atac_snrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An snRNA required for the splicing of th ***
--- *** e minor U12-dependent class of eukaryoti ***
--- *** c nuclear introns. It forms a base paire ***
--- *** d complex with U6atac_snRNA (SO:0000397) ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW u4atac_snrna AS
  SELECT
    feature_id AS u4atac_snrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U4atac_snRNA';

--- ************************************************
--- *** relation: u5_snrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** U5 RNA is a component of both types of k ***
--- *** nown spliceosome. The precise function o ***
--- *** f this molecule is unknown, though it is ***
--- ***  known that the 5' loop is required for  ***
--- *** splice site selection and p220 binding,  ***
--- *** and that both the 3' stem-loop and the S ***
--- *** m site are important for Sm protein bind ***
--- *** ing and cap methylation.                 ***
--- ************************************************
---

CREATE VIEW u5_snrna AS
  SELECT
    feature_id AS u5_snrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U5_snRNA';

--- ************************************************
--- *** relation: u6_snrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** U6 snRNA is a component of the spliceoso ***
--- *** me which is involved in splicing pre-mRN ***
--- *** A. The putative secondary structure cons ***
--- *** ensus base pairing is confined to a shor ***
--- *** t 5' stem loop, but U6 snRNA is thought  ***
--- *** to form extensive base-pair interactions ***
--- ***  with U4 snRNA.                          ***
--- ************************************************
---

CREATE VIEW u6_snrna AS
  SELECT
    feature_id AS u6_snrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U6_snRNA';

--- ************************************************
--- *** relation: u6atac_snrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** U6atac_snRNA is an snRNA required for th ***
--- *** e splicing of the minor U12-dependent cl ***
--- *** ass of eukaryotic nuclear introns. It fo ***
--- *** rms a base paired complex with U4atac_sn ***
--- *** RNA (SO:0000394).                        ***
--- ************************************************
---

CREATE VIEW u6atac_snrna AS
  SELECT
    feature_id AS u6atac_snrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U6atac_snRNA';

--- ************************************************
--- *** relation: u11_snrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** U11 snRNA plays a role in splicing of th ***
--- *** e minor U12-dependent class of eukaryoti ***
--- *** c nuclear introns, similar to U1 snRNA i ***
--- *** n the major class spliceosome it base pa ***
--- *** irs to the conserved 5' splice site sequ ***
--- *** ence.                                    ***
--- ************************************************
---

CREATE VIEW u11_snrna AS
  SELECT
    feature_id AS u11_snrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U11_snRNA';

--- ************************************************
--- *** relation: u12_snrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The U12 small nuclear (snRNA), together  ***
--- *** with U4atac/U6atac, U5, and U11 snRNAs a ***
--- *** nd associated proteins, forms a spliceos ***
--- *** ome that cleaves a divergent class of lo ***
--- *** w-abundance pre-mRNA introns.            ***
--- ************************************************
---

CREATE VIEW u12_snrna AS
  SELECT
    feature_id AS u12_snrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U12_snRNA';

--- ************************************************
--- *** relation: sequence_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describes a quality of sequ ***
--- *** ence.                                    ***
--- ************************************************
---

CREATE VIEW sequence_attribute AS
  SELECT
    feature_id AS sequence_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polymer_attribute' OR cvterm.name = 'feature_attribute' OR cvterm.name = 'sequence_location' OR cvterm.name = 'variant_quality' OR cvterm.name = 'nucleic_acid' OR cvterm.name = 'synthetic_sequence' OR cvterm.name = 'topology_attribute' OR cvterm.name = 'peptidyl' OR cvterm.name = 'DNA' OR cvterm.name = 'RNA' OR cvterm.name = 'morpholino_backbone' OR cvterm.name = 'PNA' OR cvterm.name = 'LNA' OR cvterm.name = 'TNA' OR cvterm.name = 'GNA' OR cvterm.name = 'cDNA' OR cvterm.name = 'genomic_DNA' OR cvterm.name = 'single_stranded_cDNA' OR cvterm.name = 'double_stranded_cDNA' OR cvterm.name = 'R_GNA' OR cvterm.name = 'S_GNA' OR cvterm.name = 'random_sequence' OR cvterm.name = 'designed_sequence' OR cvterm.name = 'linear' OR cvterm.name = 'circular' OR cvterm.name = 'transcript_attribute' OR cvterm.name = 'bound_by_factor' OR cvterm.name = 'flanked' OR cvterm.name = 'gene_attribute' OR cvterm.name = 'retrotransposed' OR cvterm.name = 'transgenic' OR cvterm.name = 'natural' OR cvterm.name = 'engineered' OR cvterm.name = 'foreign' OR cvterm.name = 'fusion' OR cvterm.name = 'rescue' OR cvterm.name = 'wild_type' OR cvterm.name = 'conserved' OR cvterm.name = 'status' OR cvterm.name = 'intermediate' OR cvterm.name = 'recombinationally_rearranged' OR cvterm.name = 'cryptic' OR cvterm.name = 'strand_attribute' OR cvterm.name = 'direction_attribute' OR cvterm.name = 'enzymatic' OR cvterm.name = 'mobile' OR cvterm.name = 'alteration_attribute' OR cvterm.name = 'experimental_feature_attribute' OR cvterm.name = 'edited' OR cvterm.name = 'capped' OR cvterm.name = 'mRNA_attribute' OR cvterm.name = 'trans_spliced' OR cvterm.name = 'alternatively_spliced' OR cvterm.name = 'monocistronic' OR cvterm.name = 'polycistronic' OR cvterm.name = 'polyadenylated' OR cvterm.name = 'exemplar' OR cvterm.name = 'frameshift' OR cvterm.name = 'recoded' OR cvterm.name = 'minus_1_frameshift' OR cvterm.name = 'minus_2_frameshift' OR cvterm.name = 'plus_1_frameshift' OR cvterm.name = 'plus_2_framshift' OR cvterm.name = 'codon_redefined' OR cvterm.name = 'recoded_by_translational_bypass' OR cvterm.name = 'translationally_frameshifted' OR cvterm.name = 'minus_1_translationally_frameshifted' OR cvterm.name = 'plus_1_translationally_frameshifted' OR cvterm.name = 'dicistronic' OR cvterm.name = 'bound_by_protein' OR cvterm.name = 'bound_by_nucleic_acid' OR cvterm.name = 'floxed' OR cvterm.name = 'FRT_flanked' OR cvterm.name = 'protein_coding' OR cvterm.name = 'non_protein_coding' OR cvterm.name = 'gene_to_gene_feature' OR cvterm.name = 'gene_array_member' OR cvterm.name = 'regulated' OR cvterm.name = 'epigenetically_modified' OR cvterm.name = 'encodes_alternately_spliced_transcripts' OR cvterm.name = 'encodes_alternate_transcription_start_sites' OR cvterm.name = 'intein_containing' OR cvterm.name = 'miRNA_encoding' OR cvterm.name = 'rRNA_encoding' OR cvterm.name = 'scRNA_encoding' OR cvterm.name = 'snoRNA_encoding' OR cvterm.name = 'snRNA_encoding' OR cvterm.name = 'SRP_RNA_encoding' OR cvterm.name = 'stRNA_encoding' OR cvterm.name = 'tmRNA_encoding' OR cvterm.name = 'tRNA_encoding' OR cvterm.name = 'gRNA_encoding' OR cvterm.name = 'C_D_box_snoRNA_encoding' OR cvterm.name = 'H_ACA_box_snoRNA_encoding' OR cvterm.name = 'overlapping' OR cvterm.name = 'inside_intron' OR cvterm.name = 'five_prime_three_prime_overlap' OR cvterm.name = 'five_prime_five_prime_overlap' OR cvterm.name = 'three_prime_three_prime_overlap' OR cvterm.name = 'three_prime_five_prime_overlap' OR cvterm.name = 'antisense' OR cvterm.name = 'inside_intron_antiparallel' OR cvterm.name = 'inside_intron_parallel' OR cvterm.name = 'operon_member' OR cvterm.name = 'gene_cassette_member' OR cvterm.name = 'gene_subarray_member' OR cvterm.name = 'member_of_regulon' OR cvterm.name = 'cassette_array_member' OR cvterm.name = 'transcriptionally_regulated' OR cvterm.name = 'post_translationally_regulated' OR cvterm.name = 'translationally_regulated' OR cvterm.name = 'imprinted' OR cvterm.name = 'transcriptionally_constitutive' OR cvterm.name = 'transcriptionally_induced' OR cvterm.name = 'transcriptionally_repressed' OR cvterm.name = 'autoregulated' OR cvterm.name = 'positively_autoregulated' OR cvterm.name = 'negatively_autoregulated' OR cvterm.name = 'silenced' OR cvterm.name = 'silenced_by_DNA_modification' OR cvterm.name = 'silenced_by_RNA_interference' OR cvterm.name = 'silenced_by_histone_modification' OR cvterm.name = 'silenced_by_DNA_methylation' OR cvterm.name = 'silenced_by_histone_methylation' OR cvterm.name = 'silenced_by_histone_deacetylation' OR cvterm.name = 'negatively_autoregulated' OR cvterm.name = 'positively_autoregulated' OR cvterm.name = 'post_translationally_regulated_by_protein_stability' OR cvterm.name = 'post_translationally_regulated_by_protein_modification' OR cvterm.name = 'maternally_imprinted' OR cvterm.name = 'paternally_imprinted' OR cvterm.name = 'imprinted' OR cvterm.name = 'allelically_excluded' OR cvterm.name = 'rearranged_at_DNA_level' OR cvterm.name = 'maternally_imprinted' OR cvterm.name = 'paternally_imprinted' OR cvterm.name = 'encodes_1_polypeptide' OR cvterm.name = 'encodes_greater_than_1_polypeptide' OR cvterm.name = 'encodes_disjoint_polypeptides' OR cvterm.name = 'encodes_overlapping_peptides' OR cvterm.name = 'encodes_different_polypeptides_different_stop' OR cvterm.name = 'encodes_overlapping_peptides_different_start' OR cvterm.name = 'encodes_overlapping_polypeptides_different_start_and_stop' OR cvterm.name = 'homologous' OR cvterm.name = 'syntenic' OR cvterm.name = 'orthologous' OR cvterm.name = 'paralogous' OR cvterm.name = 'fragmentary' OR cvterm.name = 'predicted' OR cvterm.name = 'validated' OR cvterm.name = 'invalidated' OR cvterm.name = 'independently_known' OR cvterm.name = 'consensus' OR cvterm.name = 'low_complexity' OR cvterm.name = 'whole_genome_sequence_status' OR cvterm.name = 'supported_by_sequence_similarity' OR cvterm.name = 'orphan' OR cvterm.name = 'predicted_by_ab_initio_computation' OR cvterm.name = 'supported_by_domain_match' OR cvterm.name = 'supported_by_EST_or_cDNA' OR cvterm.name = 'experimentally_determined' OR cvterm.name = 'invalidated_by_chimeric_cDNA' OR cvterm.name = 'invalidated_by_genomic_contamination' OR cvterm.name = 'invalidated_by_genomic_polyA_primed_cDNA' OR cvterm.name = 'invalidated_by_partial_processing' OR cvterm.name = 'standard_draft' OR cvterm.name = 'high_quality_draft' OR cvterm.name = 'improved_high_quality_draft' OR cvterm.name = 'annotation_directed_improved_draft' OR cvterm.name = 'noncontiguous_finished' OR cvterm.name = 'finished_genome' OR cvterm.name = 'single' OR cvterm.name = 'double' OR cvterm.name = 'forward' OR cvterm.name = 'reverse' OR cvterm.name = 'ribozymic' OR cvterm.name = 'chromosomal_variation_attribute' OR cvterm.name = 'insertion_attribute' OR cvterm.name = 'inversion_attribute' OR cvterm.name = 'translocaton_attribute' OR cvterm.name = 'duplication_attribute' OR cvterm.name = 'intrachromosomal' OR cvterm.name = 'interchromosomal' OR cvterm.name = 'tandem' OR cvterm.name = 'direct' OR cvterm.name = 'inverted' OR cvterm.name = 'pericentric' OR cvterm.name = 'paracentric' OR cvterm.name = 'reciprocal' OR cvterm.name = 'insertional' OR cvterm.name = 'free' OR cvterm.name = 'score' OR cvterm.name = 'quality_value' OR cvterm.name = 'organelle_sequence' OR cvterm.name = 'plasmid_location' OR cvterm.name = 'proviral_location' OR cvterm.name = 'macronuclear_sequence' OR cvterm.name = 'micronuclear_sequence' OR cvterm.name = 'mitochondrial_sequence' OR cvterm.name = 'nuclear_sequence' OR cvterm.name = 'nucleomorphic_sequence' OR cvterm.name = 'plastid_sequence' OR cvterm.name = 'mitochondrial_DNA' OR cvterm.name = 'apicoplast_sequence' OR cvterm.name = 'chromoplast_sequence' OR cvterm.name = 'chloroplast_sequence' OR cvterm.name = 'cyanelle_sequence' OR cvterm.name = 'leucoplast_sequence' OR cvterm.name = 'proplastid_sequence' OR cvterm.name = 'chloroplast_DNA' OR cvterm.name = 'endogenous_retroviral_sequence' OR cvterm.name = 'variant_origin' OR cvterm.name = 'variant_frequency' OR cvterm.name = 'variant_phenotype' OR cvterm.name = 'maternal_variant' OR cvterm.name = 'paternal_variant' OR cvterm.name = 'somatic_variant' OR cvterm.name = 'germline_variant' OR cvterm.name = 'pedigree_specific_variant' OR cvterm.name = 'population_specific_variant' OR cvterm.name = 'de_novo_variant' OR cvterm.name = 'unique_variant' OR cvterm.name = 'rare_variant' OR cvterm.name = 'polymorphic_variant' OR cvterm.name = 'common_variant' OR cvterm.name = 'fixed_variant' OR cvterm.name = 'benign_variant' OR cvterm.name = 'disease_associated_variant' OR cvterm.name = 'disease_causing_variant' OR cvterm.name = 'lethal_variant' OR cvterm.name = 'quantitative_variant' OR cvterm.name = 'sequence_attribute';

--- ************************************************
--- *** relation: gene_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW gene_attribute AS
  SELECT
    feature_id AS gene_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'protein_coding' OR cvterm.name = 'non_protein_coding' OR cvterm.name = 'gene_to_gene_feature' OR cvterm.name = 'gene_array_member' OR cvterm.name = 'regulated' OR cvterm.name = 'epigenetically_modified' OR cvterm.name = 'encodes_alternately_spliced_transcripts' OR cvterm.name = 'encodes_alternate_transcription_start_sites' OR cvterm.name = 'intein_containing' OR cvterm.name = 'miRNA_encoding' OR cvterm.name = 'rRNA_encoding' OR cvterm.name = 'scRNA_encoding' OR cvterm.name = 'snoRNA_encoding' OR cvterm.name = 'snRNA_encoding' OR cvterm.name = 'SRP_RNA_encoding' OR cvterm.name = 'stRNA_encoding' OR cvterm.name = 'tmRNA_encoding' OR cvterm.name = 'tRNA_encoding' OR cvterm.name = 'gRNA_encoding' OR cvterm.name = 'C_D_box_snoRNA_encoding' OR cvterm.name = 'H_ACA_box_snoRNA_encoding' OR cvterm.name = 'overlapping' OR cvterm.name = 'inside_intron' OR cvterm.name = 'five_prime_three_prime_overlap' OR cvterm.name = 'five_prime_five_prime_overlap' OR cvterm.name = 'three_prime_three_prime_overlap' OR cvterm.name = 'three_prime_five_prime_overlap' OR cvterm.name = 'antisense' OR cvterm.name = 'inside_intron_antiparallel' OR cvterm.name = 'inside_intron_parallel' OR cvterm.name = 'operon_member' OR cvterm.name = 'gene_cassette_member' OR cvterm.name = 'gene_subarray_member' OR cvterm.name = 'member_of_regulon' OR cvterm.name = 'cassette_array_member' OR cvterm.name = 'transcriptionally_regulated' OR cvterm.name = 'post_translationally_regulated' OR cvterm.name = 'translationally_regulated' OR cvterm.name = 'imprinted' OR cvterm.name = 'transcriptionally_constitutive' OR cvterm.name = 'transcriptionally_induced' OR cvterm.name = 'transcriptionally_repressed' OR cvterm.name = 'autoregulated' OR cvterm.name = 'positively_autoregulated' OR cvterm.name = 'negatively_autoregulated' OR cvterm.name = 'silenced' OR cvterm.name = 'silenced_by_DNA_modification' OR cvterm.name = 'silenced_by_RNA_interference' OR cvterm.name = 'silenced_by_histone_modification' OR cvterm.name = 'silenced_by_DNA_methylation' OR cvterm.name = 'silenced_by_histone_methylation' OR cvterm.name = 'silenced_by_histone_deacetylation' OR cvterm.name = 'negatively_autoregulated' OR cvterm.name = 'positively_autoregulated' OR cvterm.name = 'post_translationally_regulated_by_protein_stability' OR cvterm.name = 'post_translationally_regulated_by_protein_modification' OR cvterm.name = 'maternally_imprinted' OR cvterm.name = 'paternally_imprinted' OR cvterm.name = 'imprinted' OR cvterm.name = 'allelically_excluded' OR cvterm.name = 'rearranged_at_DNA_level' OR cvterm.name = 'maternally_imprinted' OR cvterm.name = 'paternally_imprinted' OR cvterm.name = 'encodes_1_polypeptide' OR cvterm.name = 'encodes_greater_than_1_polypeptide' OR cvterm.name = 'encodes_disjoint_polypeptides' OR cvterm.name = 'encodes_overlapping_peptides' OR cvterm.name = 'encodes_different_polypeptides_different_stop' OR cvterm.name = 'encodes_overlapping_peptides_different_start' OR cvterm.name = 'encodes_overlapping_polypeptides_different_start_and_stop' OR cvterm.name = 'gene_attribute';

--- ************************************************
--- *** relation: u14_snorna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** U14 small nucleolar RNA (U14 snoRNA) is  ***
--- *** required for early cleavages of eukaryot ***
--- *** ic precursor rRNAs. In yeasts, this mole ***
--- *** cule possess a stem-loop region (known a ***
--- *** s the Y-domain) which is essential for f ***
--- *** unction. A similar structure, but with a ***
--- ***  different consensus sequence, is found  ***
--- *** in plants, but is absent in vertebrates. ***
--- ************************************************
---

CREATE VIEW u14_snorna AS
  SELECT
    feature_id AS u14_snorna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U14_snoRNA';

--- ************************************************
--- *** relation: vault_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A family of RNAs are found as part of th ***
--- *** e enigmatic vault ribonucleoprotein comp ***
--- *** lex. The complex consists of a major vau ***
--- *** lt protein (MVP), two minor vault protei ***
--- *** ns (VPARP and TEP1), and several small u ***
--- *** ntranslated RNA molecules. It has been s ***
--- *** uggested that the vault complex is invol ***
--- *** ved in drug resistance.                  ***
--- ************************************************
---

CREATE VIEW vault_rna AS
  SELECT
    feature_id AS vault_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'vault_RNA';

--- ************************************************
--- *** relation: y_rna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Y RNAs are components of the Ro ribonucl ***
--- *** eoprotein particle (Ro RNP), in associat ***
--- *** ion with Ro60 and La proteins. The Y RNA ***
--- *** s and Ro60 and La proteins are well cons ***
--- *** erved, but the function of the Ro RNP is ***
--- ***  not known. In humans the RNA component  ***
--- *** can be one of four small RNAs: hY1, hY3, ***
--- ***  hY4 and hY5. These small RNAs are predi ***
--- *** cted to fold into a conserved secondary  ***
--- *** structure containing three stem structur ***
--- *** es. The largest of the four, hY1, contai ***
--- *** ns an additional hairpin.                ***
--- ************************************************
---

CREATE VIEW y_rna AS
  SELECT
    feature_id AS y_rna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'Y_RNA';

--- ************************************************
--- *** relation: twintron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An intron within an intron. Twintrons ar ***
--- *** e group II or III introns, into which an ***
--- *** other group II or III intron has been tr ***
--- *** ansposed.                                ***
--- ************************************************
---

CREATE VIEW twintron AS
  SELECT
    feature_id AS twintron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'twintron';

--- ************************************************
--- *** relation: rrna_18s ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A large polynucleotide in eukaryotes, wh ***
--- *** ich functions as the small subunit of th ***
--- *** e ribosome.                              ***
--- ************************************************
---

CREATE VIEW rrna_18s AS
  SELECT
    feature_id AS rrna_18s_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'rRNA_18S';

--- ************************************************
--- *** relation: binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A biological_region of sequence that, in ***
--- ***  the molecule, interacts selectively and ***
--- ***  non-covalently with other molecules. A  ***
--- *** region on the surface of a molecule that ***
--- ***  may interact with another molecule. Whe ***
--- *** n applied to polypeptides: Amino acids i ***
--- *** nvolved in binding or interactions. It c ***
--- *** an also apply to an amino acid bond whic ***
--- *** h is represented by the positions of the ***
--- ***  two flanking amino acids.               ***
--- ************************************************
---

CREATE VIEW binding_site AS
  SELECT
    feature_id AS binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'protein_binding_site' OR cvterm.name = 'epitope' OR cvterm.name = 'nucleotide_binding_site' OR cvterm.name = 'metal_binding_site' OR cvterm.name = 'ligand_binding_site' OR cvterm.name = 'protein_protein_contact' OR cvterm.name = 'nucleotide_to_protein_binding_site' OR cvterm.name = 'nuclease_binding_site' OR cvterm.name = 'TF_binding_site' OR cvterm.name = 'histone_binding_site' OR cvterm.name = 'insulator_binding_site' OR cvterm.name = 'enhancer_binding_site' OR cvterm.name = 'restriction_enzyme_binding_site' OR cvterm.name = 'nuclease_sensitive_site' OR cvterm.name = 'homing_endonuclease_binding_site' OR cvterm.name = 'nuclease_hypersensitive_site' OR cvterm.name = 'group_1_intron_homing_endonuclease_target_region' OR cvterm.name = 'DNAseI_hypersensitive_site' OR cvterm.name = 'miRNA_target_site' OR cvterm.name = 'DNA_binding_site' OR cvterm.name = 'primer_binding_site' OR cvterm.name = 'polypeptide_DNA_contact' OR cvterm.name = 'polypeptide_metal_contact' OR cvterm.name = 'polypeptide_calcium_ion_contact_site' OR cvterm.name = 'polypeptide_cobalt_ion_contact_site' OR cvterm.name = 'polypeptide_copper_ion_contact_site' OR cvterm.name = 'polypeptide_iron_ion_contact_site' OR cvterm.name = 'polypeptide_magnesium_ion_contact_site' OR cvterm.name = 'polypeptide_manganese_ion_contact_site' OR cvterm.name = 'polypeptide_molybdenum_ion_contact_site' OR cvterm.name = 'polypeptide_nickel_ion_contact_site' OR cvterm.name = 'polypeptide_tungsten_ion_contact_site' OR cvterm.name = 'polypeptide_zinc_ion_contact_site' OR cvterm.name = 'polypeptide_ligand_contact' OR cvterm.name = 'binding_site';

--- ************************************************
--- *** relation: protein_binding_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A binding site that, in the molecule, in ***
--- *** teracts selectively and non-covalently w ***
--- *** ith polypeptide molecules.               ***
--- ************************************************
---

CREATE VIEW protein_binding_site AS
  SELECT
    feature_id AS protein_binding_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'protein_protein_contact' OR cvterm.name = 'nucleotide_to_protein_binding_site' OR cvterm.name = 'nuclease_binding_site' OR cvterm.name = 'TF_binding_site' OR cvterm.name = 'histone_binding_site' OR cvterm.name = 'insulator_binding_site' OR cvterm.name = 'enhancer_binding_site' OR cvterm.name = 'restriction_enzyme_binding_site' OR cvterm.name = 'nuclease_sensitive_site' OR cvterm.name = 'homing_endonuclease_binding_site' OR cvterm.name = 'nuclease_hypersensitive_site' OR cvterm.name = 'group_1_intron_homing_endonuclease_target_region' OR cvterm.name = 'DNAseI_hypersensitive_site' OR cvterm.name = 'protein_binding_site';

--- ************************************************
--- *** relation: rescue_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region that rescues.                   ***
--- ************************************************
---

CREATE VIEW rescue_region AS
  SELECT
    feature_id AS rescue_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_rescue_region' OR cvterm.name = 'rescue_region';

--- ************************************************
--- *** relation: restriction_fragment ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of polynucleotide sequence prod ***
--- *** uced by digestion with a restriction end ***
--- *** onuclease.                               ***
--- ************************************************
---

CREATE VIEW restriction_fragment AS
  SELECT
    feature_id AS restriction_fragment_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RFLP_fragment' OR cvterm.name = 'restriction_fragment';

--- ************************************************
--- *** relation: sequence_difference ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region where the sequence differs from ***
--- ***  that of a specified sequence.           ***
--- ************************************************
---

CREATE VIEW sequence_difference AS
  SELECT
    feature_id AS sequence_difference_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'possible_base_call_error' OR cvterm.name = 'possible_assembly_error' OR cvterm.name = 'assembly_error_correction' OR cvterm.name = 'base_call_error_correction' OR cvterm.name = 'sequence_difference';

--- ************************************************
--- *** relation: invalidated_by_genomic_contamination ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a feature that  ***
--- *** is invalidated due to genomic contaminat ***
--- *** ion.                                     ***
--- ************************************************
---

CREATE VIEW invalidated_by_genomic_contamination AS
  SELECT
    feature_id AS invalidated_by_genomic_contamination_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'invalidated_by_genomic_contamination';

--- ************************************************
--- *** relation: invalidated_by_genomic_polya_primed_cdna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a feature that  ***
--- *** is invalidated due to polyA priming.     ***
--- ************************************************
---

CREATE VIEW invalidated_by_genomic_polya_primed_cdna AS
  SELECT
    feature_id AS invalidated_by_genomic_polya_primed_cdna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'invalidated_by_genomic_polyA_primed_cDNA';

--- ************************************************
--- *** relation: invalidated_by_partial_processing ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a feature that  ***
--- *** is invalidated due to partial processing ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW invalidated_by_partial_processing AS
  SELECT
    feature_id AS invalidated_by_partial_processing_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'invalidated_by_partial_processing';

--- ************************************************
--- *** relation: polypeptide_domain ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A structurally or functionally defined p ***
--- *** rotein region. In proteins with multiple ***
--- ***  domains, the combination of the domains ***
--- ***  determines the function of the protein. ***
--- ***  A region which has been shown to recur  ***
--- *** throughout evolution.                    ***
--- ************************************************
---

CREATE VIEW polypeptide_domain AS
  SELECT
    feature_id AS polypeptide_domain_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'polypeptide_domain';

--- ************************************************
--- *** relation: signal_peptide ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The signal_peptide is a short region of  ***
--- *** the peptide located at the N-terminus th ***
--- *** at directs the protein to be secreted or ***
--- ***  part of membrane components.            ***
--- ************************************************
---

CREATE VIEW signal_peptide AS
  SELECT
    feature_id AS signal_peptide_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'signal_peptide';

--- ************************************************
--- *** relation: mature_protein_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The polypeptide sequence that remains wh ***
--- *** en the cleaved peptide regions have been ***
--- ***  cleaved from the immature peptide.      ***
--- ************************************************
---

CREATE VIEW mature_protein_region AS
  SELECT
    feature_id AS mature_protein_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'active_peptide' OR cvterm.name = 'mature_protein_region';

--- ************************************************
--- *** relation: five_prime_terminal_inverted_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW five_prime_terminal_inverted_repeat AS
  SELECT
    feature_id AS five_prime_terminal_inverted_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_terminal_inverted_repeat';

--- ************************************************
--- *** relation: three_prime_terminal_inverted_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW three_prime_terminal_inverted_repeat AS
  SELECT
    feature_id AS three_prime_terminal_inverted_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_terminal_inverted_repeat';

--- ************************************************
--- *** relation: u5_ltr_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW u5_ltr_region AS
  SELECT
    feature_id AS u5_ltr_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U5_five_prime_LTR_region' OR cvterm.name = 'U5_LTR_region';

--- ************************************************
--- *** relation: r_ltr_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW r_ltr_region AS
  SELECT
    feature_id AS r_ltr_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'R_five_prime_LTR_region' OR cvterm.name = 'R_LTR_region';

--- ************************************************
--- *** relation: u3_ltr_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW u3_ltr_region AS
  SELECT
    feature_id AS u3_ltr_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U3_five_prime_LTR_region' OR cvterm.name = 'U3_LTR_region';

--- ************************************************
--- *** relation: five_prime_ltr ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW five_prime_ltr AS
  SELECT
    feature_id AS five_prime_ltr_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_LTR';

--- ************************************************
--- *** relation: three_prime_ltr ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW three_prime_ltr AS
  SELECT
    feature_id AS three_prime_ltr_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_LTR';

--- ************************************************
--- *** relation: r_five_prime_ltr_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW r_five_prime_ltr_region AS
  SELECT
    feature_id AS r_five_prime_ltr_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'R_five_prime_LTR_region';

--- ************************************************
--- *** relation: u5_five_prime_ltr_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW u5_five_prime_ltr_region AS
  SELECT
    feature_id AS u5_five_prime_ltr_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U5_five_prime_LTR_region';

--- ************************************************
--- *** relation: u3_five_prime_ltr_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW u3_five_prime_ltr_region AS
  SELECT
    feature_id AS u3_five_prime_ltr_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U3_five_prime_LTR_region';

--- ************************************************
--- *** relation: r_three_prime_ltr_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW r_three_prime_ltr_region AS
  SELECT
    feature_id AS r_three_prime_ltr_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'R_three_prime_LTR_region';

--- ************************************************
--- *** relation: u3_three_prime_ltr_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW u3_three_prime_ltr_region AS
  SELECT
    feature_id AS u3_three_prime_ltr_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U3_three_prime_LTR_region';

--- ************************************************
--- *** relation: u5_three_prime_ltr_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW u5_three_prime_ltr_region AS
  SELECT
    feature_id AS u5_three_prime_ltr_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'U5_three_prime_LTR_region';

--- ************************************************
--- *** relation: non_ltr_retrotransposon_polymeric_tract ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A polymeric tract, such as poly(dA), wit ***
--- *** hin a non_LTR_retrotransposon.           ***
--- ************************************************
---

CREATE VIEW non_ltr_retrotransposon_polymeric_tract AS
  SELECT
    feature_id AS non_ltr_retrotransposon_polymeric_tract_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'non_LTR_retrotransposon_polymeric_tract';

--- ************************************************
--- *** relation: target_site_duplication ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence of the target DNA that is dup ***
--- *** licated when a transposable element or p ***
--- *** hage inserts; usually found at each end  ***
--- *** the insertion.                           ***
--- ************************************************
---

CREATE VIEW target_site_duplication AS
  SELECT
    feature_id AS target_site_duplication_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'target_site_duplication';

--- ************************************************
--- *** relation: rr_tract ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A polypurine tract within an LTR_retrotr ***
--- *** ansposon.                                ***
--- ************************************************
---

CREATE VIEW rr_tract AS
  SELECT
    feature_id AS rr_tract_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RR_tract';

--- ************************************************
--- *** relation: ars ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence that can autonomously replica ***
--- *** te, as a plasmid, when transformed into  ***
--- *** a bacterial host.                        ***
--- ************************************************
---

CREATE VIEW ars AS
  SELECT
    feature_id AS ars_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ARS';

--- ************************************************
--- *** relation: inverted_ring_chromosome ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW inverted_ring_chromosome AS
  SELECT
    feature_id AS inverted_ring_chromosome_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'inverted_ring_chromosome';

--- ************************************************
--- *** relation: vector_replicon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A replicon that has been modified to act ***
--- ***  as a vector for foreign sequence.       ***
--- ************************************************
---

CREATE VIEW vector_replicon AS
  SELECT
    feature_id AS vector_replicon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'YAC' OR cvterm.name = 'BAC' OR cvterm.name = 'PAC' OR cvterm.name = 'cosmid' OR cvterm.name = 'phagemid' OR cvterm.name = 'fosmid' OR cvterm.name = 'lambda_vector' OR cvterm.name = 'plasmid_vector' OR cvterm.name = 'targeting_vector' OR cvterm.name = 'vector_replicon';

--- ************************************************
--- *** relation: ss_oligo ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A single stranded oligonucleotide.       ***
--- ************************************************
---

CREATE VIEW ss_oligo AS
  SELECT
    feature_id AS ss_oligo_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'primer' OR cvterm.name = 'sequencing_primer' OR cvterm.name = 'forward_primer' OR cvterm.name = 'reverse_primer' OR cvterm.name = 'ASPE_primer' OR cvterm.name = 'dCAPS_primer' OR cvterm.name = 'ss_oligo';

--- ************************************************
--- *** relation: ds_oligo ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A double stranded oligonucleotide.       ***
--- ************************************************
---

CREATE VIEW ds_oligo AS
  SELECT
    feature_id AS ds_oligo_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNAi_reagent' OR cvterm.name = 'DNA_constraint_sequence' OR cvterm.name = 'ds_oligo';

--- ************************************************
--- *** relation: polymer_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe the kind of bio ***
--- *** logical sequence.                        ***
--- ************************************************
---

CREATE VIEW polymer_attribute AS
  SELECT
    feature_id AS polymer_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nucleic_acid' OR cvterm.name = 'synthetic_sequence' OR cvterm.name = 'topology_attribute' OR cvterm.name = 'peptidyl' OR cvterm.name = 'DNA' OR cvterm.name = 'RNA' OR cvterm.name = 'morpholino_backbone' OR cvterm.name = 'PNA' OR cvterm.name = 'LNA' OR cvterm.name = 'TNA' OR cvterm.name = 'GNA' OR cvterm.name = 'cDNA' OR cvterm.name = 'genomic_DNA' OR cvterm.name = 'single_stranded_cDNA' OR cvterm.name = 'double_stranded_cDNA' OR cvterm.name = 'R_GNA' OR cvterm.name = 'S_GNA' OR cvterm.name = 'random_sequence' OR cvterm.name = 'designed_sequence' OR cvterm.name = 'linear' OR cvterm.name = 'circular' OR cvterm.name = 'polymer_attribute';

--- ************************************************
--- *** relation: three_prime_noncoding_exon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Non-coding exon in the 3' UTR.           ***
--- ************************************************
---

CREATE VIEW three_prime_noncoding_exon AS
  SELECT
    feature_id AS three_prime_noncoding_exon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_noncoding_exon';

--- ************************************************
--- *** relation: five_prime_noncoding_exon ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Non-coding exon in the 5' UTR.           ***
--- ************************************************
---

CREATE VIEW five_prime_noncoding_exon AS
  SELECT
    feature_id AS five_prime_noncoding_exon_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_noncoding_exon';

--- ************************************************
--- *** relation: utr_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Intron located in the untranslated regio ***
--- *** n.                                       ***
--- ************************************************
---

CREATE VIEW utr_intron AS
  SELECT
    feature_id AS utr_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_UTR_intron' OR cvterm.name = 'three_prime_UTR_intron' OR cvterm.name = 'UTR_intron';

--- ************************************************
--- *** relation: five_prime_utr_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An intron located in the 5' UTR.         ***
--- ************************************************
---

CREATE VIEW five_prime_utr_intron AS
  SELECT
    feature_id AS five_prime_utr_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'five_prime_UTR_intron';

--- ************************************************
--- *** relation: three_prime_utr_intron ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An intron located in the 3' UTR.         ***
--- ************************************************
---

CREATE VIEW three_prime_utr_intron AS
  SELECT
    feature_id AS three_prime_utr_intron_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'three_prime_UTR_intron';

--- ************************************************
--- *** relation: random_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence of nucleotides or amino acids ***
--- ***  which, by design, has a "random" order  ***
--- *** of components, given a predetermined inp ***
--- *** ut frequency of these components.        ***
--- ************************************************
---

CREATE VIEW random_sequence AS
  SELECT
    feature_id AS random_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'random_sequence';

--- ************************************************
--- *** relation: interband ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A light region between two darkly staini ***
--- *** ng bands in a polytene chromosome.       ***
--- ************************************************
---

CREATE VIEW interband AS
  SELECT
    feature_id AS interband_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'interband';

--- ************************************************
--- *** relation: gene_with_polyadenylated_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that encodes a polyadenylated mRN ***
--- *** A.                                       ***
--- ************************************************
---

CREATE VIEW gene_with_polyadenylated_mrna AS
  SELECT
    feature_id AS gene_with_polyadenylated_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_polyadenylated_mRNA';

--- ************************************************

SET search_path=so,chado,pg_catalog;
--- *** relation: golden_path ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A set of subregions selected from sequen ***
--- *** ce contigs which when concatenated form  ***
--- *** a nonredundant linear sequence.          ***
--- ************************************************
---

CREATE VIEW golden_path AS
  SELECT
    feature_id AS golden_path_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'golden_path';

--- ************************************************
--- *** relation: cdna_match ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A match against cDNA sequence.           ***
--- ************************************************
---

CREATE VIEW cdna_match AS
  SELECT
    feature_id AS cdna_match_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cDNA_match';

--- ************************************************
--- *** relation: gene_with_polycistronic_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that encodes a polycistronic tran ***
--- *** script.                                  ***
--- ************************************************
---

CREATE VIEW gene_with_polycistronic_transcript AS
  SELECT
    feature_id AS gene_with_polycistronic_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_dicistronic_transcript' OR cvterm.name = 'gene_with_dicistronic_primary_transcript' OR cvterm.name = 'gene_with_dicistronic_mRNA' OR cvterm.name = 'gene_with_polycistronic_transcript';

--- ************************************************
--- *** relation: cleaved_initiator_methionine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The initiator methionine that has been c ***
--- *** leaved from a mature polypeptide sequenc ***
--- *** e.                                       ***
--- ************************************************
---

CREATE VIEW cleaved_initiator_methionine AS
  SELECT
    feature_id AS cleaved_initiator_methionine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'cleaved_initiator_methionine';

--- ************************************************
--- *** relation: gene_with_dicistronic_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that encodes a dicistronic transc ***
--- *** ript.                                    ***
--- ************************************************
---

CREATE VIEW gene_with_dicistronic_transcript AS
  SELECT
    feature_id AS gene_with_dicistronic_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_dicistronic_primary_transcript' OR cvterm.name = 'gene_with_dicistronic_mRNA' OR cvterm.name = 'gene_with_dicistronic_transcript';

--- ************************************************
--- *** relation: gene_with_recoded_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that encodes an mRNA that is reco ***
--- *** ded.                                     ***
--- ************************************************
---

CREATE VIEW gene_with_recoded_mrna AS
  SELECT
    feature_id AS gene_with_recoded_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_stop_codon_read_through' OR cvterm.name = 'gene_with_mRNA_recoded_by_translational_bypass' OR cvterm.name = 'gene_with_transcript_with_translational_frameshift' OR cvterm.name = 'gene_with_stop_codon_redefined_as_pyrrolysine' OR cvterm.name = 'gene_with_stop_codon_redefined_as_selenocysteine' OR cvterm.name = 'gene_with_recoded_mRNA';

--- ************************************************
--- *** relation: snp ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** SNPs are single base pair positions in g ***
--- *** enomic DNA at which different sequence a ***
--- *** lternatives exist in normal individuals  ***
--- *** in some population(s), wherein the least ***
--- ***  frequent variant has an abundance of 1% ***
--- ***  or greater.                             ***
--- ************************************************
---

CREATE VIEW snp AS
  SELECT
    feature_id AS snp_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SNP';

--- ************************************************
--- *** relation: reagent ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence used in experiment.           ***
--- ************************************************
---

CREATE VIEW reagent AS
  SELECT
    feature_id AS reagent_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'PCR_product' OR cvterm.name = 'clone' OR cvterm.name = 'rescue_region' OR cvterm.name = 'oligo' OR cvterm.name = 'clone_insert' OR cvterm.name = 'cloned_region' OR cvterm.name = 'databank_entry' OR cvterm.name = 'RAPD' OR cvterm.name = 'genomic_clone' OR cvterm.name = 'cDNA_clone' OR cvterm.name = 'tiling_path_clone' OR cvterm.name = 'validated_cDNA_clone' OR cvterm.name = 'invalidated_cDNA_clone' OR cvterm.name = 'three_prime_RACE_clone' OR cvterm.name = 'chimeric_cDNA_clone' OR cvterm.name = 'genomically_contaminated_cDNA_clone' OR cvterm.name = 'polyA_primed_cDNA_clone' OR cvterm.name = 'partially_processed_cDNA_clone' OR cvterm.name = 'engineered_rescue_region' OR cvterm.name = 'aptamer' OR cvterm.name = 'probe' OR cvterm.name = 'tag' OR cvterm.name = 'ss_oligo' OR cvterm.name = 'ds_oligo' OR cvterm.name = 'DNAzyme' OR cvterm.name = 'synthetic_oligo' OR cvterm.name = 'DNA_aptamer' OR cvterm.name = 'RNA_aptamer' OR cvterm.name = 'microarray_oligo' OR cvterm.name = 'SAGE_tag' OR cvterm.name = 'STS' OR cvterm.name = 'EST' OR cvterm.name = 'engineered_tag' OR cvterm.name = 'five_prime_EST' OR cvterm.name = 'three_prime_EST' OR cvterm.name = 'UST' OR cvterm.name = 'RST' OR cvterm.name = 'three_prime_UST' OR cvterm.name = 'five_prime_UST' OR cvterm.name = 'three_prime_RST' OR cvterm.name = 'five_prime_RST' OR cvterm.name = 'primer' OR cvterm.name = 'sequencing_primer' OR cvterm.name = 'forward_primer' OR cvterm.name = 'reverse_primer' OR cvterm.name = 'ASPE_primer' OR cvterm.name = 'dCAPS_primer' OR cvterm.name = 'RNAi_reagent' OR cvterm.name = 'DNA_constraint_sequence' OR cvterm.name = 'morpholino_oligo' OR cvterm.name = 'PNA_oligo' OR cvterm.name = 'LNA_oligo' OR cvterm.name = 'TNA_oligo' OR cvterm.name = 'GNA_oligo' OR cvterm.name = 'R_GNA_oligo' OR cvterm.name = 'S_GNA_oligo' OR cvterm.name = 'cloned_cDNA_insert' OR cvterm.name = 'cloned_genomic_insert' OR cvterm.name = 'engineered_insert' OR cvterm.name = 'BAC_cloned_genomic_insert' OR cvterm.name = 'reagent';

--- ************************************************
--- *** relation: oligo ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A short oligonucleotide sequence, of len ***
--- *** gth on the order of 10's of bases; eithe ***
--- *** r single or double stranded.             ***
--- ************************************************
---

CREATE VIEW oligo AS
  SELECT
    feature_id AS oligo_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'aptamer' OR cvterm.name = 'probe' OR cvterm.name = 'tag' OR cvterm.name = 'ss_oligo' OR cvterm.name = 'ds_oligo' OR cvterm.name = 'DNAzyme' OR cvterm.name = 'synthetic_oligo' OR cvterm.name = 'DNA_aptamer' OR cvterm.name = 'RNA_aptamer' OR cvterm.name = 'microarray_oligo' OR cvterm.name = 'SAGE_tag' OR cvterm.name = 'STS' OR cvterm.name = 'EST' OR cvterm.name = 'engineered_tag' OR cvterm.name = 'five_prime_EST' OR cvterm.name = 'three_prime_EST' OR cvterm.name = 'UST' OR cvterm.name = 'RST' OR cvterm.name = 'three_prime_UST' OR cvterm.name = 'five_prime_UST' OR cvterm.name = 'three_prime_RST' OR cvterm.name = 'five_prime_RST' OR cvterm.name = 'primer' OR cvterm.name = 'sequencing_primer' OR cvterm.name = 'forward_primer' OR cvterm.name = 'reverse_primer' OR cvterm.name = 'ASPE_primer' OR cvterm.name = 'dCAPS_primer' OR cvterm.name = 'RNAi_reagent' OR cvterm.name = 'DNA_constraint_sequence' OR cvterm.name = 'morpholino_oligo' OR cvterm.name = 'PNA_oligo' OR cvterm.name = 'LNA_oligo' OR cvterm.name = 'TNA_oligo' OR cvterm.name = 'GNA_oligo' OR cvterm.name = 'R_GNA_oligo' OR cvterm.name = 'S_GNA_oligo' OR cvterm.name = 'oligo';

--- ************************************************
--- *** relation: gene_with_stop_codon_read_through ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that encodes a transcript with st ***
--- *** op codon readthrough.                    ***
--- ************************************************
---

CREATE VIEW gene_with_stop_codon_read_through AS
  SELECT
    feature_id AS gene_with_stop_codon_read_through_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_stop_codon_redefined_as_pyrrolysine' OR cvterm.name = 'gene_with_stop_codon_redefined_as_selenocysteine' OR cvterm.name = 'gene_with_stop_codon_read_through';

--- ************************************************
--- *** relation: gene_with_stop_codon_redefined_as_pyrrolysine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene encoding an mRNA that has the sto ***
--- *** p codon redefined as pyrrolysine.        ***
--- ************************************************
---

CREATE VIEW gene_with_stop_codon_redefined_as_pyrrolysine AS
  SELECT
    feature_id AS gene_with_stop_codon_redefined_as_pyrrolysine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_stop_codon_redefined_as_pyrrolysine';

--- ************************************************
--- *** relation: junction ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A sequence_feature with an extent of zer ***
--- *** o.                                       ***
--- ************************************************
---

CREATE VIEW junction AS
  SELECT
    feature_id AS junction_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'clone_insert_end' OR cvterm.name = 'clone_insert_start' OR cvterm.name = 'exon_junction' OR cvterm.name = 'insertion_site' OR cvterm.name = 'polyA_site' OR cvterm.name = 'deletion_junction' OR cvterm.name = 'chromosome_breakpoint' OR cvterm.name = 'splice_junction' OR cvterm.name = 'trans_splice_junction' OR cvterm.name = 'restriction_enzyme_cleavage_junction' OR cvterm.name = 'transposable_element_insertion_site' OR cvterm.name = 'inversion_breakpoint' OR cvterm.name = 'translocation_breakpoint' OR cvterm.name = 'insertion_breakpoint' OR cvterm.name = 'deletion_breakpoint' OR cvterm.name = 'blunt_end_restriction_enzyme_cleavage_junction' OR cvterm.name = 'single_strand_restriction_enzyme_cleavage_site' OR cvterm.name = 'five_prime_restriction_enzyme_junction' OR cvterm.name = 'three_prime_restriction_enzyme_junction' OR cvterm.name = 'junction';

--- ************************************************
--- *** relation: remark ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A comment about the sequence.            ***
--- ************************************************
---

CREATE VIEW remark AS
  SELECT
    feature_id AS remark_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'sequence_difference' OR cvterm.name = 'experimental_result_region' OR cvterm.name = 'polypeptide_sequencing_information' OR cvterm.name = 'possible_base_call_error' OR cvterm.name = 'possible_assembly_error' OR cvterm.name = 'assembly_error_correction' OR cvterm.name = 'base_call_error_correction' OR cvterm.name = 'overlapping_feature_set' OR cvterm.name = 'no_output' OR cvterm.name = 'overlapping_EST_set' OR cvterm.name = 'non_adjacent_residues' OR cvterm.name = 'non_terminal_residue' OR cvterm.name = 'sequence_conflict' OR cvterm.name = 'sequence_uncertainty' OR cvterm.name = 'contig_collection' OR cvterm.name = 'remark';

--- ************************************************
--- *** relation: possible_base_call_error ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of sequence where the validity  ***
--- *** of the base calling is questionable.     ***
--- ************************************************
---

CREATE VIEW possible_base_call_error AS
  SELECT
    feature_id AS possible_base_call_error_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'possible_base_call_error';

--- ************************************************
--- *** relation: possible_assembly_error ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of sequence where there may hav ***
--- *** e been an error in the assembly.         ***
--- ************************************************
---

CREATE VIEW possible_assembly_error AS
  SELECT
    feature_id AS possible_assembly_error_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'possible_assembly_error';

--- ************************************************
--- *** relation: experimental_result_region ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of sequence implicated in an ex ***
--- *** perimental result.                       ***
--- ************************************************
---

CREATE VIEW experimental_result_region AS
  SELECT
    feature_id AS experimental_result_region_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'overlapping_feature_set' OR cvterm.name = 'no_output' OR cvterm.name = 'overlapping_EST_set' OR cvterm.name = 'experimental_result_region';

--- ************************************************
--- *** relation: gene ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region (or regions) that includes all  ***
--- *** of the sequence elements necessary to en ***
--- *** code a functional transcript. A gene may ***
--- ***  include regulatory regions, transcribed ***
--- ***  regions and/or other functional sequenc ***
--- *** e regions.                               ***
--- ************************************************
---

CREATE VIEW gene AS
  SELECT
    feature_id AS gene_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nuclear_gene' OR cvterm.name = 'mt_gene' OR cvterm.name = 'plastid_gene' OR cvterm.name = 'nucleomorph_gene' OR cvterm.name = 'plasmid_gene' OR cvterm.name = 'proviral_gene' OR cvterm.name = 'transposable_element_gene' OR cvterm.name = 'silenced_gene' OR cvterm.name = 'engineered_gene' OR cvterm.name = 'foreign_gene' OR cvterm.name = 'fusion_gene' OR cvterm.name = 'recombinationally_rearranged_gene' OR cvterm.name = 'gene_with_trans_spliced_transcript' OR cvterm.name = 'gene_with_polycistronic_transcript' OR cvterm.name = 'rescue_gene' OR cvterm.name = 'post_translationally_regulated_gene' OR cvterm.name = 'negatively_autoregulated_gene' OR cvterm.name = 'positively_autoregulated_gene' OR cvterm.name = 'translationally_regulated_gene' OR cvterm.name = 'epigenetically_modified_gene' OR cvterm.name = 'transgene' OR cvterm.name = 'predicted_gene' OR cvterm.name = 'protein_coding_gene' OR cvterm.name = 'retrogene' OR cvterm.name = 'ncRNA_gene' OR cvterm.name = 'cryptic_gene' OR cvterm.name = 'gene_with_non_canonical_start_codon' OR cvterm.name = 'gene_cassette' OR cvterm.name = 'kinetoplast_gene' OR cvterm.name = 'maxicircle_gene' OR cvterm.name = 'minicircle_gene' OR cvterm.name = 'cryptogene' OR cvterm.name = 'apicoplast_gene' OR cvterm.name = 'ct_gene' OR cvterm.name = 'chromoplast_gene' OR cvterm.name = 'cyanelle_gene' OR cvterm.name = 'leucoplast_gene' OR cvterm.name = 'proplastid_gene' OR cvterm.name = 'endogenous_retroviral_gene' OR cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'gene_silenced_by_DNA_modification' OR cvterm.name = 'gene_silenced_by_RNA_interference' OR cvterm.name = 'gene_silenced_by_histone_modification' OR cvterm.name = 'gene_silenced_by_DNA_methylation' OR cvterm.name = 'gene_silenced_by_histone_methylation' OR cvterm.name = 'gene_silenced_by_histone_deacetylation' OR cvterm.name = 'engineered_foreign_gene' OR cvterm.name = 'engineered_fusion_gene' OR cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'engineered_foreign_gene' OR cvterm.name = 'engineered_foreign_transposable_element_gene' OR cvterm.name = 'engineered_fusion_gene' OR cvterm.name = 'recombinationally_inverted_gene' OR cvterm.name = 'recombinationally_rearranged_vertebrate_immune_system_gene' OR cvterm.name = 'gene_with_dicistronic_transcript' OR cvterm.name = 'gene_with_dicistronic_primary_transcript' OR cvterm.name = 'gene_with_dicistronic_mRNA' OR cvterm.name = 'wild_type_rescue_gene' OR cvterm.name = 'gene_rearranged_at_DNA_level' OR cvterm.name = 'maternally_imprinted_gene' OR cvterm.name = 'paternally_imprinted_gene' OR cvterm.name = 'allelically_excluded_gene' OR cvterm.name = 'floxed_gene' OR cvterm.name = 'gene_with_polyadenylated_mRNA' OR cvterm.name = 'gene_with_mRNA_with_frameshift' OR cvterm.name = 'gene_with_edited_transcript' OR cvterm.name = 'gene_with_recoded_mRNA' OR cvterm.name = 'gene_with_stop_codon_read_through' OR cvterm.name = 'gene_with_mRNA_recoded_by_translational_bypass' OR cvterm.name = 'gene_with_transcript_with_translational_frameshift' OR cvterm.name = 'gene_with_stop_codon_redefined_as_pyrrolysine' OR cvterm.name = 'gene_with_stop_codon_redefined_as_selenocysteine' OR cvterm.name = 'gRNA_gene' OR cvterm.name = 'miRNA_gene' OR cvterm.name = 'scRNA_gene' OR cvterm.name = 'snoRNA_gene' OR cvterm.name = 'snRNA_gene' OR cvterm.name = 'SRP_RNA_gene' OR cvterm.name = 'stRNA_gene' OR cvterm.name = 'tmRNA_gene' OR cvterm.name = 'tRNA_gene' OR cvterm.name = 'rRNA_gene' OR cvterm.name = 'piRNA_gene' OR cvterm.name = 'RNase_P_RNA_gene' OR cvterm.name = 'RNase_MRP_RNA_gene' OR cvterm.name = 'lincRNA_gene' OR cvterm.name = 'telomerase_RNA_gene' OR cvterm.name = 'cryptogene' OR cvterm.name = 'gene_with_start_codon_CUG' OR cvterm.name = 'gene';

--- ************************************************
--- *** relation: tandem_repeat ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Two or more adjcent copies of a region ( ***
--- *** of length greater than 1).               ***
--- ************************************************
---

CREATE VIEW tandem_repeat AS
  SELECT
    feature_id AS tandem_repeat_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'satellite_DNA' OR cvterm.name = 'microsatellite' OR cvterm.name = 'minisatellite' OR cvterm.name = 'dinucleotide_repeat_microsatellite_feature' OR cvterm.name = 'trinucleotide_repeat_microsatellite_feature' OR cvterm.name = 'tetranucleotide_repeat_microsatellite_feature' OR cvterm.name = 'tandem_repeat';

--- ************************************************
--- *** relation: trans_splice_acceptor_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The 3' splice site of the acceptor prima ***
--- *** ry transcript.                           ***
--- ************************************************
---

CREATE VIEW trans_splice_acceptor_site AS
  SELECT
    feature_id AS trans_splice_acceptor_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SL1_acceptor_site' OR cvterm.name = 'SL2_acceptor_site' OR cvterm.name = 'SL3_acceptor_site' OR cvterm.name = 'SL4_acceptor_site' OR cvterm.name = 'SL5_acceptor_site' OR cvterm.name = 'SL6_acceptor_site' OR cvterm.name = 'SL7_acceptor_site' OR cvterm.name = 'SL8_acceptor_site' OR cvterm.name = 'SL9_acceptor_site' OR cvterm.name = 'SL10_accceptor_site' OR cvterm.name = 'SL11_acceptor_site' OR cvterm.name = 'SL12_acceptor_site' OR cvterm.name = 'trans_splice_acceptor_site';

--- ************************************************
--- *** relation: trans_splice_donor_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The 5' five prime splice site region of  ***
--- *** the donor RNA.                           ***
--- ************************************************
---

CREATE VIEW trans_splice_donor_site AS
  SELECT
    feature_id AS trans_splice_donor_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'trans_splice_donor_site';

--- ************************************************
--- *** relation: sl1_acceptor_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A trans_splicing_acceptor_site which app ***
--- *** ends the 22nt SL1 RNA leader sequence to ***
--- ***  the 5' end of most mRNAs.               ***
--- ************************************************
---

CREATE VIEW sl1_acceptor_site AS
  SELECT
    feature_id AS sl1_acceptor_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SL1_acceptor_site';

--- ************************************************
--- *** relation: sl2_acceptor_site ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A trans_splicing_acceptor_site which app ***
--- *** ends the 22nt SL2 RNA leader sequence to ***
--- ***  the 5' end of mRNAs. SL2 acceptor sites ***
--- ***  occur in genes in internal segments of  ***
--- *** polycistronic transcripts.               ***
--- ************************************************
---

CREATE VIEW sl2_acceptor_site AS
  SELECT
    feature_id AS sl2_acceptor_site_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'SL3_acceptor_site' OR cvterm.name = 'SL4_acceptor_site' OR cvterm.name = 'SL5_acceptor_site' OR cvterm.name = 'SL6_acceptor_site' OR cvterm.name = 'SL7_acceptor_site' OR cvterm.name = 'SL8_acceptor_site' OR cvterm.name = 'SL9_acceptor_site' OR cvterm.name = 'SL10_accceptor_site' OR cvterm.name = 'SL11_acceptor_site' OR cvterm.name = 'SL12_acceptor_site' OR cvterm.name = 'SL2_acceptor_site';

--- ************************************************
--- *** relation: gene_with_stop_codon_redefined_as_selenocysteine ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene encoding an mRNA that has the sto ***
--- *** p codon redefined as selenocysteine.     ***
--- ************************************************
---

CREATE VIEW gene_with_stop_codon_redefined_as_selenocysteine AS
  SELECT
    feature_id AS gene_with_stop_codon_redefined_as_selenocysteine_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_stop_codon_redefined_as_selenocysteine';

--- ************************************************
--- *** relation: gene_with_mrna_recoded_by_translational_bypass ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene with mRNA recoded by translationa ***
--- *** l bypass.                                ***
--- ************************************************
---

CREATE VIEW gene_with_mrna_recoded_by_translational_bypass AS
  SELECT
    feature_id AS gene_with_mrna_recoded_by_translational_bypass_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_mRNA_recoded_by_translational_bypass';

--- ************************************************
--- *** relation: gene_with_transcript_with_translational_frameshift ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene encoding a transcript that has a  ***
--- *** translational frameshift.                ***
--- ************************************************
---

CREATE VIEW gene_with_transcript_with_translational_frameshift AS
  SELECT
    feature_id AS gene_with_transcript_with_translational_frameshift_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_transcript_with_translational_frameshift';

--- ************************************************
--- *** relation: dna_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif that is active in the DNA form o ***
--- *** f the sequence.                          ***
--- ************************************************
---

CREATE VIEW dna_motif AS
  SELECT
    feature_id AS dna_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'PSE_motif' OR cvterm.name = 'CAAT_signal' OR cvterm.name = 'minus_10_signal' OR cvterm.name = 'minus_35_signal' OR cvterm.name = 'DRE_motif' OR cvterm.name = 'E_box_motif' OR cvterm.name = 'INR1_motif' OR cvterm.name = 'GAGA_motif' OR cvterm.name = 'octamer_motif' OR cvterm.name = 'retinoic_acid_responsive_element' OR cvterm.name = 'promoter_element' OR cvterm.name = 'DCE_SI' OR cvterm.name = 'DCE_SII' OR cvterm.name = 'DCE_SIII' OR cvterm.name = 'minus_12_signal' OR cvterm.name = 'minus_24_signal' OR cvterm.name = 'GC_rich_promoter_region' OR cvterm.name = 'DMv4_motif' OR cvterm.name = 'DMv5_motif' OR cvterm.name = 'DMv3_motif' OR cvterm.name = 'DMv2_motif' OR cvterm.name = 'DPE1_motif' OR cvterm.name = 'DMv1_motif' OR cvterm.name = 'NDM2_motif' OR cvterm.name = 'NDM3_motif' OR cvterm.name = 'core_promoter_element' OR cvterm.name = 'regulatory_promoter_element' OR cvterm.name = 'INR_motif' OR cvterm.name = 'DPE_motif' OR cvterm.name = 'BREu_motif' OR cvterm.name = 'TATA_box' OR cvterm.name = 'A_box' OR cvterm.name = 'B_box' OR cvterm.name = 'C_box' OR cvterm.name = 'MTE' OR cvterm.name = 'BREd_motif' OR cvterm.name = 'DCE' OR cvterm.name = 'intermediate_element' OR cvterm.name = 'RNA_polymerase_II_TATA_box' OR cvterm.name = 'RNA_polymerase_III_TATA_box' OR cvterm.name = 'A_box_type_1' OR cvterm.name = 'A_box_type_2' OR cvterm.name = 'proximal_promoter_element' OR cvterm.name = 'distal_promoter_element' OR cvterm.name = 'DNA_motif';

--- ************************************************
--- *** relation: nucleotide_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of nucleotide sequence correspo ***
--- *** nding to a known motif.                  ***
--- ************************************************
---

CREATE VIEW nucleotide_motif AS
  SELECT
    feature_id AS nucleotide_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'DNA_motif' OR cvterm.name = 'RNA_motif' OR cvterm.name = 'PSE_motif' OR cvterm.name = 'CAAT_signal' OR cvterm.name = 'minus_10_signal' OR cvterm.name = 'minus_35_signal' OR cvterm.name = 'DRE_motif' OR cvterm.name = 'E_box_motif' OR cvterm.name = 'INR1_motif' OR cvterm.name = 'GAGA_motif' OR cvterm.name = 'octamer_motif' OR cvterm.name = 'retinoic_acid_responsive_element' OR cvterm.name = 'promoter_element' OR cvterm.name = 'DCE_SI' OR cvterm.name = 'DCE_SII' OR cvterm.name = 'DCE_SIII' OR cvterm.name = 'minus_12_signal' OR cvterm.name = 'minus_24_signal' OR cvterm.name = 'GC_rich_promoter_region' OR cvterm.name = 'DMv4_motif' OR cvterm.name = 'DMv5_motif' OR cvterm.name = 'DMv3_motif' OR cvterm.name = 'DMv2_motif' OR cvterm.name = 'DPE1_motif' OR cvterm.name = 'DMv1_motif' OR cvterm.name = 'NDM2_motif' OR cvterm.name = 'NDM3_motif' OR cvterm.name = 'core_promoter_element' OR cvterm.name = 'regulatory_promoter_element' OR cvterm.name = 'INR_motif' OR cvterm.name = 'DPE_motif' OR cvterm.name = 'BREu_motif' OR cvterm.name = 'TATA_box' OR cvterm.name = 'A_box' OR cvterm.name = 'B_box' OR cvterm.name = 'C_box' OR cvterm.name = 'MTE' OR cvterm.name = 'BREd_motif' OR cvterm.name = 'DCE' OR cvterm.name = 'intermediate_element' OR cvterm.name = 'RNA_polymerase_II_TATA_box' OR cvterm.name = 'RNA_polymerase_III_TATA_box' OR cvterm.name = 'A_box_type_1' OR cvterm.name = 'A_box_type_2' OR cvterm.name = 'proximal_promoter_element' OR cvterm.name = 'distal_promoter_element' OR cvterm.name = 'RNA_internal_loop' OR cvterm.name = 'A_minor_RNA_motif' OR cvterm.name = 'RNA_junction_loop' OR cvterm.name = 'hammerhead_ribozyme' OR cvterm.name = 'asymmetric_RNA_internal_loop' OR cvterm.name = 'symmetric_RNA_internal_loop' OR cvterm.name = 'K_turn_RNA_motif' OR cvterm.name = 'sarcin_like_RNA_motif' OR cvterm.name = 'RNA_hook_turn' OR cvterm.name = 'nucleotide_motif';

--- ************************************************
--- *** relation: rna_motif ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A motif that is active in RNA sequence.  ***
--- ************************************************
---

CREATE VIEW rna_motif AS
  SELECT
    feature_id AS rna_motif_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'RNA_internal_loop' OR cvterm.name = 'A_minor_RNA_motif' OR cvterm.name = 'RNA_junction_loop' OR cvterm.name = 'hammerhead_ribozyme' OR cvterm.name = 'asymmetric_RNA_internal_loop' OR cvterm.name = 'symmetric_RNA_internal_loop' OR cvterm.name = 'K_turn_RNA_motif' OR cvterm.name = 'sarcin_like_RNA_motif' OR cvterm.name = 'RNA_hook_turn' OR cvterm.name = 'RNA_motif';

--- ************************************************
--- *** relation: dicistronic_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An mRNA that has the quality dicistronic ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW dicistronic_mrna AS
  SELECT
    feature_id AS dicistronic_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'dicistronic_mRNA';

--- ************************************************
--- *** relation: reading_frame ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A nucleic acid sequence that when read a ***
--- *** s sequential triplets, has the potential ***
--- ***  of encoding a sequential string of amin ***
--- *** o acids. It need not contain the start o ***
--- *** r stop codon.                            ***
--- ************************************************
---

CREATE VIEW reading_frame AS
  SELECT
    feature_id AS reading_frame_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ORF' OR cvterm.name = 'blocked_reading_frame' OR cvterm.name = 'mini_gene' OR cvterm.name = 'rescue_mini_gene' OR cvterm.name = 'reading_frame';

--- ************************************************
--- *** relation: blocked_reading_frame ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A reading_frame that is interrupted by o ***
--- *** ne or more stop codons; usually identifi ***
--- *** ed through intergenomic sequence compari ***
--- *** sons.                                    ***
--- ************************************************
---

CREATE VIEW blocked_reading_frame AS
  SELECT
    feature_id AS blocked_reading_frame_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'blocked_reading_frame';

--- ************************************************
--- *** relation: ultracontig ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An ordered and oriented set of scaffolds ***
--- ***  based on somewhat weaker sets of infere ***
--- *** ntial evidence such as one set of mate p ***
--- *** air reads together with supporting evide ***
--- *** nce from ESTs or location of markers fro ***
--- *** m SNP or microsatellite maps, or cytogen ***
--- *** etic localization of contained markers.  ***
--- ************************************************
---

CREATE VIEW ultracontig AS
  SELECT
    feature_id AS ultracontig_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'ultracontig';

--- ************************************************
--- *** relation: foreign_transposable_element ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A transposable element that is foreign.  ***
--- ************************************************
---

CREATE VIEW foreign_transposable_element AS
  SELECT
    feature_id AS foreign_transposable_element_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'engineered_foreign_transposable_element' OR cvterm.name = 'foreign_transposable_element';

--- ************************************************
--- *** relation: gene_with_dicistronic_primary_transcript ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that encodes a dicistronic primar ***
--- *** y transcript.                            ***
--- ************************************************
---

CREATE VIEW gene_with_dicistronic_primary_transcript AS
  SELECT
    feature_id AS gene_with_dicistronic_primary_transcript_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_dicistronic_primary_transcript';

--- ************************************************
--- *** relation: gene_with_dicistronic_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gene that encodes a polycistronic mRNA ***
--- *** .                                        ***
--- ************************************************
---

CREATE VIEW gene_with_dicistronic_mrna AS
  SELECT
    feature_id AS gene_with_dicistronic_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gene_with_dicistronic_mRNA';

--- ************************************************
--- *** relation: idna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** Genomic sequence removed from the genome ***
--- *** , as a normal event, by a process of rec ***
--- *** ombination.                              ***
--- ************************************************
---

CREATE VIEW idna AS
  SELECT
    feature_id AS idna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'iDNA';

--- ************************************************
--- *** relation: orit ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a DNA molecule where transfe ***
--- *** r is initiated during the process of con ***
--- *** jugation or mobilization.                ***
--- ************************************************
---

CREATE VIEW orit AS
  SELECT
    feature_id AS orit_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'oriT';

--- ************************************************
--- *** relation: transit_peptide ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The transit_peptide is a short region at ***
--- ***  the N-terminus of the peptide that dire ***
--- *** cts the protein to an organelle (chlorop ***
--- *** last, mitochondrion, microbody or cyanel ***
--- *** le).                                     ***
--- ************************************************
---

CREATE VIEW transit_peptide AS
  SELECT
    feature_id AS transit_peptide_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transit_peptide';

--- ************************************************
--- *** relation: repeat_unit ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** The simplest repeated component of a rep ***
--- *** eat region. A single repeat.             ***
--- ************************************************
---

CREATE VIEW repeat_unit AS
  SELECT
    feature_id AS repeat_unit_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'repeat_unit';

--- ************************************************
--- *** relation: crm ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A regulatory_region where more than 1 TF ***
--- *** _binding_site together are regulatorily  ***
--- *** active.                                  ***
--- ************************************************
---

CREATE VIEW crm AS
  SELECT
    feature_id AS crm_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'locus_control_region' OR cvterm.name = 'enhancer' OR cvterm.name = 'silencer' OR cvterm.name = 'enhancer_bound_by_factor' OR cvterm.name = 'shadow_enhancer' OR cvterm.name = 'CRM';

--- ************************************************
--- *** relation: intein ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A region of a peptide that is able to ex ***
--- *** cise itself and rejoin the remaining por ***
--- *** tions with a peptide bond.               ***
--- ************************************************
---

CREATE VIEW intein AS
  SELECT
    feature_id AS intein_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'intein';

--- ************************************************
--- *** relation: intein_containing ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute of protein-coding genes whe ***
--- *** re the initial protein product contains  ***
--- *** an intein.                               ***
--- ************************************************
---

CREATE VIEW intein_containing AS
  SELECT
    feature_id AS intein_containing_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'intein_containing';

--- ************************************************
--- *** relation: gap ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A gap in the sequence of known length. T ***
--- *** he unknown bases are filled in with N's. ***
--- ************************************************
---

CREATE VIEW gap AS
  SELECT
    feature_id AS gap_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'gap';

--- ************************************************
--- *** relation: fragmentary ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute to describe a feature that  ***
--- *** is incomplete.                           ***
--- ************************************************
---

CREATE VIEW fragmentary AS
  SELECT
    feature_id AS fragmentary_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'fragmentary';

--- ************************************************
--- *** relation: predicted ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing an unverified re ***
--- *** gion.                                    ***
--- ************************************************
---

CREATE VIEW predicted AS
  SELECT
    feature_id AS predicted_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'supported_by_sequence_similarity' OR cvterm.name = 'orphan' OR cvterm.name = 'predicted_by_ab_initio_computation' OR cvterm.name = 'supported_by_domain_match' OR cvterm.name = 'supported_by_EST_or_cDNA' OR cvterm.name = 'predicted';

--- ************************************************
--- *** relation: feature_attribute ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An attribute describing a located_sequen ***
--- *** ce_feature.                              ***
--- ************************************************
---

CREATE VIEW feature_attribute AS
  SELECT
    feature_id AS feature_attribute_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'transcript_attribute' OR cvterm.name = 'bound_by_factor' OR cvterm.name = 'flanked' OR cvterm.name = 'gene_attribute' OR cvterm.name = 'retrotransposed' OR cvterm.name = 'transgenic' OR cvterm.name = 'natural' OR cvterm.name = 'engineered' OR cvterm.name = 'foreign' OR cvterm.name = 'fusion' OR cvterm.name = 'rescue' OR cvterm.name = 'wild_type' OR cvterm.name = 'conserved' OR cvterm.name = 'status' OR cvterm.name = 'intermediate' OR cvterm.name = 'recombinationally_rearranged' OR cvterm.name = 'cryptic' OR cvterm.name = 'strand_attribute' OR cvterm.name = 'direction_attribute' OR cvterm.name = 'enzymatic' OR cvterm.name = 'mobile' OR cvterm.name = 'alteration_attribute' OR cvterm.name = 'experimental_feature_attribute' OR cvterm.name = 'edited' OR cvterm.name = 'capped' OR cvterm.name = 'mRNA_attribute' OR cvterm.name = 'trans_spliced' OR cvterm.name = 'alternatively_spliced' OR cvterm.name = 'monocistronic' OR cvterm.name = 'polycistronic' OR cvterm.name = 'polyadenylated' OR cvterm.name = 'exemplar' OR cvterm.name = 'frameshift' OR cvterm.name = 'recoded' OR cvterm.name = 'minus_1_frameshift' OR cvterm.name = 'minus_2_frameshift' OR cvterm.name = 'plus_1_frameshift' OR cvterm.name = 'plus_2_framshift' OR cvterm.name = 'codon_redefined' OR cvterm.name = 'recoded_by_translational_bypass' OR cvterm.name = 'translationally_frameshifted' OR cvterm.name = 'minus_1_translationally_frameshifted' OR cvterm.name = 'plus_1_translationally_frameshifted' OR cvterm.name = 'dicistronic' OR cvterm.name = 'bound_by_protein' OR cvterm.name = 'bound_by_nucleic_acid' OR cvterm.name = 'floxed' OR cvterm.name = 'FRT_flanked' OR cvterm.name = 'protein_coding' OR cvterm.name = 'non_protein_coding' OR cvterm.name = 'gene_to_gene_feature' OR cvterm.name = 'gene_array_member' OR cvterm.name = 'regulated' OR cvterm.name = 'epigenetically_modified' OR cvterm.name = 'encodes_alternately_spliced_transcripts' OR cvterm.name = 'encodes_alternate_transcription_start_sites' OR cvterm.name = 'intein_containing' OR cvterm.name = 'miRNA_encoding' OR cvterm.name = 'rRNA_encoding' OR cvterm.name = 'scRNA_encoding' OR cvterm.name = 'snoRNA_encoding' OR cvterm.name = 'snRNA_encoding' OR cvterm.name = 'SRP_RNA_encoding' OR cvterm.name = 'stRNA_encoding' OR cvterm.name = 'tmRNA_encoding' OR cvterm.name = 'tRNA_encoding' OR cvterm.name = 'gRNA_encoding' OR cvterm.name = 'C_D_box_snoRNA_encoding' OR cvterm.name = 'H_ACA_box_snoRNA_encoding' OR cvterm.name = 'overlapping' OR cvterm.name = 'inside_intron' OR cvterm.name = 'five_prime_three_prime_overlap' OR cvterm.name = 'five_prime_five_prime_overlap' OR cvterm.name = 'three_prime_three_prime_overlap' OR cvterm.name = 'three_prime_five_prime_overlap' OR cvterm.name = 'antisense' OR cvterm.name = 'inside_intron_antiparallel' OR cvterm.name = 'inside_intron_parallel' OR cvterm.name = 'operon_member' OR cvterm.name = 'gene_cassette_member' OR cvterm.name = 'gene_subarray_member' OR cvterm.name = 'member_of_regulon' OR cvterm.name = 'cassette_array_member' OR cvterm.name = 'transcriptionally_regulated' OR cvterm.name = 'post_translationally_regulated' OR cvterm.name = 'translationally_regulated' OR cvterm.name = 'imprinted' OR cvterm.name = 'transcriptionally_constitutive' OR cvterm.name = 'transcriptionally_induced' OR cvterm.name = 'transcriptionally_repressed' OR cvterm.name = 'autoregulated' OR cvterm.name = 'positively_autoregulated' OR cvterm.name = 'negatively_autoregulated' OR cvterm.name = 'silenced' OR cvterm.name = 'silenced_by_DNA_modification' OR cvterm.name = 'silenced_by_RNA_interference' OR cvterm.name = 'silenced_by_histone_modification' OR cvterm.name = 'silenced_by_DNA_methylation' OR cvterm.name = 'silenced_by_histone_methylation' OR cvterm.name = 'silenced_by_histone_deacetylation' OR cvterm.name = 'negatively_autoregulated' OR cvterm.name = 'positively_autoregulated' OR cvterm.name = 'post_translationally_regulated_by_protein_stability' OR cvterm.name = 'post_translationally_regulated_by_protein_modification' OR cvterm.name = 'maternally_imprinted' OR cvterm.name = 'paternally_imprinted' OR cvterm.name = 'imprinted' OR cvterm.name = 'allelically_excluded' OR cvterm.name = 'rearranged_at_DNA_level' OR cvterm.name = 'maternally_imprinted' OR cvterm.name = 'paternally_imprinted' OR cvterm.name = 'encodes_1_polypeptide' OR cvterm.name = 'encodes_greater_than_1_polypeptide' OR cvterm.name = 'encodes_disjoint_polypeptides' OR cvterm.name = 'encodes_overlapping_peptides' OR cvterm.name = 'encodes_different_polypeptides_different_stop' OR cvterm.name = 'encodes_overlapping_peptides_different_start' OR cvterm.name = 'encodes_overlapping_polypeptides_different_start_and_stop' OR cvterm.name = 'homologous' OR cvterm.name = 'syntenic' OR cvterm.name = 'orthologous' OR cvterm.name = 'paralogous' OR cvterm.name = 'fragmentary' OR cvterm.name = 'predicted' OR cvterm.name = 'validated' OR cvterm.name = 'invalidated' OR cvterm.name = 'independently_known' OR cvterm.name = 'consensus' OR cvterm.name = 'low_complexity' OR cvterm.name = 'whole_genome_sequence_status' OR cvterm.name = 'supported_by_sequence_similarity' OR cvterm.name = 'orphan' OR cvterm.name = 'predicted_by_ab_initio_computation' OR cvterm.name = 'supported_by_domain_match' OR cvterm.name = 'supported_by_EST_or_cDNA' OR cvterm.name = 'experimentally_determined' OR cvterm.name = 'invalidated_by_chimeric_cDNA' OR cvterm.name = 'invalidated_by_genomic_contamination' OR cvterm.name = 'invalidated_by_genomic_polyA_primed_cDNA' OR cvterm.name = 'invalidated_by_partial_processing' OR cvterm.name = 'standard_draft' OR cvterm.name = 'high_quality_draft' OR cvterm.name = 'improved_high_quality_draft' OR cvterm.name = 'annotation_directed_improved_draft' OR cvterm.name = 'noncontiguous_finished' OR cvterm.name = 'finished_genome' OR cvterm.name = 'single' OR cvterm.name = 'double' OR cvterm.name = 'forward' OR cvterm.name = 'reverse' OR cvterm.name = 'ribozymic' OR cvterm.name = 'chromosomal_variation_attribute' OR cvterm.name = 'insertion_attribute' OR cvterm.name = 'inversion_attribute' OR cvterm.name = 'translocaton_attribute' OR cvterm.name = 'duplication_attribute' OR cvterm.name = 'intrachromosomal' OR cvterm.name = 'interchromosomal' OR cvterm.name = 'tandem' OR cvterm.name = 'direct' OR cvterm.name = 'inverted' OR cvterm.name = 'pericentric' OR cvterm.name = 'paracentric' OR cvterm.name = 'reciprocal' OR cvterm.name = 'insertional' OR cvterm.name = 'free' OR cvterm.name = 'score' OR cvterm.name = 'quality_value' OR cvterm.name = 'feature_attribute';

--- ************************************************
--- *** relation: exemplar_mrna ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** An exemplar is a representative cDNA seq ***
--- *** uence for each gene. The exemplar approa ***
--- *** ch is a method that usually involves som ***
--- *** e initial clustering into gene groups an ***
--- *** d the subsequent selection of a represen ***
--- *** tative from each gene group.             ***
--- ************************************************
---

CREATE VIEW exemplar_mrna AS
  SELECT
    feature_id AS exemplar_mrna_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'exemplar_mRNA';

--- ************************************************
--- *** relation: sequence_location ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW sequence_location AS
  SELECT
    feature_id AS sequence_location_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'organelle_sequence' OR cvterm.name = 'plasmid_location' OR cvterm.name = 'proviral_location' OR cvterm.name = 'macronuclear_sequence' OR cvterm.name = 'micronuclear_sequence' OR cvterm.name = 'mitochondrial_sequence' OR cvterm.name = 'nuclear_sequence' OR cvterm.name = 'nucleomorphic_sequence' OR cvterm.name = 'plastid_sequence' OR cvterm.name = 'mitochondrial_DNA' OR cvterm.name = 'apicoplast_sequence' OR cvterm.name = 'chromoplast_sequence' OR cvterm.name = 'chloroplast_sequence' OR cvterm.name = 'cyanelle_sequence' OR cvterm.name = 'leucoplast_sequence' OR cvterm.name = 'proplastid_sequence' OR cvterm.name = 'chloroplast_DNA' OR cvterm.name = 'endogenous_retroviral_sequence' OR cvterm.name = 'sequence_location';

--- ************************************************
--- *** relation: organelle_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW organelle_sequence AS
  SELECT
    feature_id AS organelle_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'macronuclear_sequence' OR cvterm.name = 'micronuclear_sequence' OR cvterm.name = 'mitochondrial_sequence' OR cvterm.name = 'nuclear_sequence' OR cvterm.name = 'nucleomorphic_sequence' OR cvterm.name = 'plastid_sequence' OR cvterm.name = 'mitochondrial_DNA' OR cvterm.name = 'apicoplast_sequence' OR cvterm.name = 'chromoplast_sequence' OR cvterm.name = 'chloroplast_sequence' OR cvterm.name = 'cyanelle_sequence' OR cvterm.name = 'leucoplast_sequence' OR cvterm.name = 'proplastid_sequence' OR cvterm.name = 'chloroplast_DNA' OR cvterm.name = 'organelle_sequence';

--- ************************************************
--- *** relation: mitochondrial_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW mitochondrial_sequence AS
  SELECT
    feature_id AS mitochondrial_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'mitochondrial_DNA' OR cvterm.name = 'mitochondrial_sequence';

--- ************************************************
--- *** relation: nuclear_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW nuclear_sequence AS
  SELECT
    feature_id AS nuclear_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nuclear_sequence';

--- ************************************************
--- *** relation: nucleomorphic_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW nucleomorphic_sequence AS
  SELECT
    feature_id AS nucleomorphic_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'nucleomorphic_sequence';

--- ************************************************
--- *** relation: plastid_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW plastid_sequence AS
  SELECT
    feature_id AS plastid_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'apicoplast_sequence' OR cvterm.name = 'chromoplast_sequence' OR cvterm.name = 'chloroplast_sequence' OR cvterm.name = 'cyanelle_sequence' OR cvterm.name = 'leucoplast_sequence' OR cvterm.name = 'proplastid_sequence' OR cvterm.name = 'chloroplast_DNA' OR cvterm.name = 'plastid_sequence';

--- ************************************************
--- *** relation: kinetoplast ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A kinetoplast is an interlocked network  ***
--- *** of thousands of minicircles and tens of  ***
--- *** maxi circles, located near the base of t ***
--- *** he flagellum of some protozoan species.  ***
--- ************************************************
---

CREATE VIEW kinetoplast AS
  SELECT
    feature_id AS kinetoplast_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'kinetoplast';

--- ************************************************
--- *** relation: maxicircle ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- *** A maxicircle is a replicon, part of a ki ***
--- *** netoplast, that contains open reading fr ***
--- *** ames and replicates via a rolling circle ***
--- ***  method.                                 ***
--- ************************************************
---

CREATE VIEW maxicircle AS
  SELECT
    feature_id AS maxicircle_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'maxicircle';

--- ************************************************
--- *** relation: apicoplast_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW apicoplast_sequence AS
  SELECT
    feature_id AS apicoplast_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'apicoplast_sequence';

--- ************************************************
--- *** relation: chromoplast_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW chromoplast_sequence AS
  SELECT
    feature_id AS chromoplast_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chromoplast_sequence';

--- ************************************************
--- *** relation: chloroplast_sequence ***
--- *** relation type: VIEW                      ***
--- ***                                          ***
--- ************************************************
---

CREATE VIEW chloroplast_sequence AS
  SELECT
    feature_id AS chloroplast_sequence_id,
    feature.*
  FROM
    feature INNER JOIN cvterm ON (feature.type_id = cvterm.cvterm_id)
  WHERE cvterm.name = 'chloroplast_DNA' OR cvterm.name = 'chloroplast_sequence';

--- ************************************************

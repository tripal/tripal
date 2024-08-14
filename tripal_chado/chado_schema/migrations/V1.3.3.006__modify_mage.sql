/* rewriting issue https://github.com/GMOD/Chado/pull/50 */

ALTER TABLE chado.assay
ALTER COLUMN arraydesign_id
DROP NOT null;

COMMENT ON TABLE chado.assay IS 'An assay consists of an experiment for a single biosample, for example a microarray or an RNASeq library sequence set. An assay can be thought of as an experiment to quantify expression of a sample.';

COMMENT ON TABLE chado.acquisition IS 'This represents the acquisition technique. In the case of a microarray, it is scanning, in the case of a sequencer, it is sequencing. The output of this process is a digital image of an array for a microarray or a set of digital images or nucleotide base calls for a sequencer.';

COMMENT ON TABLE chado.quantification IS 'Quantification is the transformation of an image or set of sequences to numeric expression data. This typically involves statistical procedures.';

ALTER TABLE chado.element
ALTER COLUMN arraydesign_id
DROP NOT null;

COMMENT ON TABLE chado.element IS 'For a microarray, represents a feature of the array. This is typically a region of the array coated or bound to DNA. For RNASeq, represents a feature sequence that is used for aligning and quantifying reads.';

COMMENT ON TABLE chado.elementresult IS 'Expression signal. In the case of a microarray, the hybridization signal. In the case of RNAseq, the read count. May be normalized or raw, as specified in the acquisition record.';

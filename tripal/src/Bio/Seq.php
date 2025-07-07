<?php

/**
 * Biological sequence representation library.
 *
 * Provides classes to represent biological sequences and their features
 * similar to BioPerl's Bio::SeqFeature and Bio::Seq.
 * This library was drafted with ChatGPT-4o.
 */

declare(strict_types=1);

namespace Drupal\tripal\Bio;
use Drupal\tripal\Bio\SeqFeature;

/**
 * Represents a sequence feature, such as a gene or regulatory region.
 *
 * @code
 * // Example usage
 * $feature = new Seq;
 * $feature->setAlphabet('dna');
 * $feature->setSeq('ATGCGTACGTAGCTAGCTAGC');
 * $feature->setId('seq1');
 * $feature->setDesc('Example sequence');
 * $feature->addFeature(new SeqFeature('gene', 1, 100, 1));
 */

class Seq
{
    private string $seq; // Nucleotide or protein sequence
    private string $id; // Unique sequence identifier
    private ?string $desc; // Optional description
    private array $features; // Annotated features
    private string $alphabet; // Type of sequence: 'dna', 'rna', or 'protein'

    /**
     * Constructs a Seq instance.
     *
     * @param string $seq Sequence string.
     * @param string $id Sequence identifier.
     * @param string|null $desc Optional description.
     * @param string $alphabet Sequence type: 'dna', 'rna', or 'protein'.
     * @throws \InvalidArgumentException If sequence contains invalid characters.
     */
    public function __construct(string $seq, string $id, ?string $desc = null, string $alphabet = 'dna')
    {
        if (!in_array($alphabet, ['dna', 'rna', 'protein'], true)) {
            throw new \InvalidArgumentException("Alphabet must be 'dna', 'rna', or 'protein'.");
        }
        if (!$this->isValidSequence($seq, $alphabet)) {
            throw new \InvalidArgumentException("Invalid sequence characters detected for the given alphabet.");
        }

        $this->seq = strtoupper($seq);
        $this->id = $id;
        $this->desc = $desc;
        $this->alphabet = $alphabet;
        $this->features = [];
    }

    private function isValidSequence(string $seq, string $alphabet): bool
    {
        $patterns = [
            'dna' => '/^[ACGTNacgtn]+$/',
            'rna' => '/^[ACGUNacgun]+$/',
            'protein' => '/^[ACDEFGHIKLMNPQRSTVWYacdefghiklmnpqrstvwy]+$/'
        ];
        return isset($patterns[$alphabet]) && preg_match($patterns[$alphabet], $seq);
    }

    public function getSeq(): string { return $this->seq; }
    public function setSeq(string $seq): void { if (!$this->isValidSequence($seq, $this->alphabet)) { throw new \InvalidArgumentException("Invalid sequence characters detected."); } $this->seq = strtoupper($seq); }
    public function getId(): string { return $this->id; }
    public function setId(string $id): void { $this->id = $id; }
    public function getDesc(): ?string { return $this->desc; }
    public function setDesc(?string $desc): void { $this->desc = $desc; }
    public function getAlphabet(): string { return $this->alphabet; }
    public function setAlphabet(string $alphabet): void { if (!in_array($alphabet, ['dna', 'rna', 'protein'], true)) { throw new \InvalidArgumentException("Alphabet must be 'dna', 'rna', or 'protein'."); } $this->alphabet = $alphabet; }
    public function getLength(): int { return strlen($this->seq); }
    public function addFeature(SeqFeature $feature): void { $this->features[] = $feature; }
    public function getFeatures(): array { return $this->features; }

    public function reverseComplement(): string {
        return strtr(strrev($this->seq), 'ACGTURYSWKMBDHVNacgturyswkmbdhvn', 'TGCAAYRSWMKVHDBNtgcaayrswmkvhdbn');
    }
}

?>

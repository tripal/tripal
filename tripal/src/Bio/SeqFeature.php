<?php

/**
 * Bioinformatics sequence representation library.
 *
 * Provides classes to represent biological sequences and their features
 * similar to BioPerl's Bio::SeqFeature and Bio::Seq.
 * This library was drafted with ChatGPT-4o.
 *
 * @code
 * // Example usage
 * $feature = new SeqFeature('gene', 1, 100, 1);
 * $feature->setPrimaryTag('gene');
 * $feature->setStart(1);
 * $feature->setEnd(100);
 * $feature->setStrand(1);
 * $feature->addTag('gene_id', 'BRCA1');
 * $feature->addTag('gene_id', 'BRCA2');
 * $feature->addTag('gene_name', 'BRCA1');
 * $feature->addTag('gene_name', 'BRCA2');
 * $feature->addSubFeature(new SeqFeature('exon', 1, 50));
 * $feature->addSubFeature(new SeqFeature('exon', 51, 100));
 * $feature->addSubFeature(new SeqFeature('intron', 51, 100));
 * $feature->addSubFeature(new SeqFeature('intron', 1, 50));
 * @endcode
 */

declare(strict_types=1);

namespace Drupal\tripal\Bio;

/**
 * Represents a sequence feature, such as a gene or regulatory region.
 */
class SeqFeature
{
    private string $primaryTag; // Feature type, e.g., "gene", "CDS"
    private int $start; // Start position (1-based index)
    private int $end; // End position
    private int $strand; // Strand direction (-1, 0, or 1)
    private array $tags; // Additional feature metadata
    private array $subFeatures; // Nested features

    /**
     * Constructs a SeqFeature instance.
     *
     * @param string $primaryTag Feature type.
     * @param int $start Start position.
     * @param int $end End position.
     * @param int $strand Strand direction (-1, 0, or 1).
     * @param array $tags Optional metadata.
     * @throws \InvalidArgumentException If start > end or strand is invalid.
     */
    public function __construct(string $primaryTag, int $start, int $end, int $strand = 1, array $tags = [])
    {
        if ($start > $end) {
            throw new \InvalidArgumentException("Start position must be less than or equal to end position.");
        }
        if (!in_array($strand, [-1, 0, 1], true)) {
            throw new \InvalidArgumentException("Strand must be -1, 0, or 1.");
        }

        $this->primaryTag = $primaryTag;
        $this->start = $start;
        $this->end = $end;
        $this->strand = $strand;
        $this->tags = $tags;
        $this->subFeatures = [];
    }

    public function getPrimaryTag(): string { return $this->primaryTag; }
    public function setPrimaryTag(string $primaryTag): void { $this->primaryTag = $primaryTag; }
    public function getStart(): int { return $this->start; }
    public function setStart(int $start): void { if ($start > $this->end) { throw new \InvalidArgumentException("Start position must be <= end position."); } $this->start = $start; }
    public function getEnd(): int { return $this->end; }
    public function setEnd(int $end): void { if ($end < $this->start) { throw new \InvalidArgumentException("End position must be >= start position."); } $this->end = $end; }
    public function getStrand(): int { return $this->strand; }
    public function setStrand(int $strand): void { if (!in_array($strand, [-1, 0, 1], true)) { throw new \InvalidArgumentException("Strand must be -1, 0, or 1."); } $this->strand = $strand; }
    public function getLength(): int { return $this->end - $this->start + 1; }
    public function addTag(string $key, string $value): void { $this->tags[$key][] = $value; }
    public function hasTag(string $key): bool { return array_key_exists($key, $this->tags); }
    public function getTags(): array { return $this->tags; }
    public function setTags(array $tags): void { $this->tags = $tags; }
    public function addSubFeature(SeqFeature $feature): void { $this->subFeatures[] = $feature; }
    public function getSubFeatures(): array { return $this->subFeatures; }
    public function setSubFeatures(array $subFeatures): void { $this->subFeatures = $subFeatures; }
}



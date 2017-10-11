<?php

class TripalFASTADownloader extends TripalFieldDownloader {

  /**
   * Sets the label shown to the user describing this formatter.
   */
  static public $label = 'FASTA';

  /**
   * Indicates the default extension for the outputfile.
   */
  static public $default_extension = 'fasta';

  /**
   * @see TripalFieldDownloader::format()
   */
  protected function formatEntity($entity) {

  }

  /**
   * @see TripalFieldDownloader::getHeader()
   */
  protected function getHeader() {

  }
}
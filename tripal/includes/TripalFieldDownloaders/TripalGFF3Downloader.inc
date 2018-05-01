<?php

class TripalGFF3Downloader extends TripalFieldDownloader {

  /**
   * Sets the label shown to the user describing this formatter.
   */
  static public $label = 'GFF3';

  /**
   * Indicates the default extension for the outputfile.
   */
  static public $default_extension = 'gff3';

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
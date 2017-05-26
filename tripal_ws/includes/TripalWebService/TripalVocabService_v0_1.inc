<?php

class TripalVocabService_v0_1 extends TripalWebService {

  /**
   * The human-readable label for this web service.
   */
  public static $label = 'Vocabulary';
  /**
   * A bit of text to describe what this service provides.
   */
  public static $description = 'Provides access to vocabulary terms that are in use by this site.';
  /**
   * A machine-readable type for this service. This name must be unique
   * among all Tripal web services and is used to form the URL to access
   * this service.
   */
  public static $type = 'vocabulary';
}
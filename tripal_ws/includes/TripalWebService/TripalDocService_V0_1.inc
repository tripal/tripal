<?php

class TripalDocService_v0_1 extends TripalWebService {

  /**
   * The human-readable label for this web service.
   */
  public static $label = 'API Documentation';
  /**
   * A bit of text to describe what this service provides.
   */
  public static $description = 'Provides documentation for the use of this web services.';
  /**
   * A machine-readable type for this service. This name must be unique
   * among all Tripal web services and is used to form the URL to access
   * this service.
   */
  public static $type = 'doc';
}
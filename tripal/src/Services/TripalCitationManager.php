<?php

namespace Drupal\tripal\Services;

use Exception;


class TripalCitationManager {

  /**
   * Generate citation
   *
   * @param int $type
   *   cvterm_id defining the type of publication
   * @param array $pub
   *   An associative array defining publication properties
   */
  public static function generateCitation(int $type, array $publication) {
print "CP21 calling generateCitation\n"; //@@@
    $citation = '';
    $citation = 'xxx';
print "CP22 generateCitation returning \"$citation\"\n"; //@@@
    return $citation;
  }
}

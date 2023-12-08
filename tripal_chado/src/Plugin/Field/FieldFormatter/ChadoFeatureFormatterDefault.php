<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal feature formatter.
 *
 * @FieldFormatter(
 *   id = "chado_feature_formatter_default",
 *   label = @Translation("Chado feature formatter"),
 *   description = @Translation("A chado feature formatter"),
 *   field_types = {
 *     "chado_feature_default"
 *   },
 *   valid_tokens = {
 *     "[name]",
 *     "[uniquename]",
 *     "[type]",
 *     "[seqlen]",
 *     "[md5checksum]",
 *     "[is_analysis]",
 *     "[is_obsolete]",
 *     "[database_name]",
 *     "[database_accession]",
 *     "[genus]",
 *     "[species]",
 *     "[infratype]",
 *     "[infratype_abbrev]",
 *     "[infraname]",
 *     "[abbreviation]",
 *     "[common_name]",
 *   },
 * )
 */
class ChadoFeatureFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['token_string'] = '[name]';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $list = [];
    $token_string = $this->getSetting('token_string');

    foreach ($items as $delta => $item) {
      $values = [
        'name' => $item->get('feature_name')->getString(),
        'uniquename' => $item->get('feature_uniquename')->getString(),
        'type' => $item->get('type')->getString(),
        'seqlen' => $item->get('seqlen')->getString(),
        'md5checksum' => $item->get('md5checksum')->getString(),
        'is_analysis' => $item->get('feature_is_analysis')->getString(),
        'is_obsolete' => $item->get('feature_is_obsolete')->getString(),
        'database_name' => $item->get('feature_database_name')->getString(),
        'database_accession' => $item->get('feature_database_accession')->getString(),
        'genus' => $item->get('feature_genus')->getString(),
        'species' => $item->get('feature_species')->getString(),
        'infratype' => $item->get('feature_infraspecific_type')->getString(),
        'infraname' => $item->get('feature_infraspecific_name')->getString(),
        'abbreviation' => $item->get('feature_abbreviation')->getString(),
        'common_name' => $item->get('feature_common_name')->getString(),
        // residues is not implemented in this field since it can be millions of characters long
        // timeaccessioned, timelastmodified not implemented
      ];

      // Substitute values in token string to generate displayed string.
      $displayed_string = $token_string;
      foreach ($values as $key => $value) {
        $displayed_string = preg_replace("/\[$key\]/", $value, $displayed_string);
      }
      $list[$delta] = [
        '#markup' => $displayed_string,
      ];
    }

    // If only one element has been found, don't make into a list.
    if (count($list) == 1) {
      $elements = $list;
    }

    // If more than one value has been found, display all values in an
    // unordered list.
// @todo: add a pager
    elseif (count($list) > 1) {
      $elements[0] = [
        '#theme' => 'item_list',
        '#list_type' => 'ul',
        '#items' => $list,
        '#wrapper_attributes' => ['class' => 'container'],
      ];
    }

    return $elements;
  }

}

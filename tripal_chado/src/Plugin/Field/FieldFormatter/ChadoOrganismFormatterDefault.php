<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal organism formatter.
 *
 * @FieldFormatter(
 *   id = "chado_organism_formatter_default",
 *   label = @Translation("Chado organism formatter"),
 *   description = @Translation("A chado organism formatter"),
 *   field_types = {
 *     "chado_organism_default"
 *   },
 *   valid_tokens = {
 *     "[genus]",
 *     "[genus_abbrev]",
 *     "[species]",
 *     "[infratype]",
 *     "[infratype_abbrev]",
 *     "[infraname]",
 *     "[scientific_name]",
 *     "[abbreviation]",
 *     "[common_name]",
 *     "[comment]",
 *   },
 * )
 */
class ChadoOrganismFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['token_string'] = '<i>[genus] [species]</i> [infratype_abbrev] <i>[infraname]</i>';
    return $settings;
  }

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $list = [];
    $token_string = $this->getSetting('token_string');

    // Implement the shortcut token "[scientific_name]". Rather than the overhead
    // of the chado_get_organism_scientific_name() api call, replicate its behavior
    // with an equivalent token string.
    $token_string = preg_replace('\[scientific_name\]', $token_string,
        '[genus] [species] [infratype_abbrev] [infraname]');

    foreach ($items as $delta => $item) {
      $values = [
        'genus' => $item->get('organism_genus')->getString(),
        'species' => $item->get('organism_species')->getString(),
        'infratype' => $item->get('organism_infraspecific_type')->getString(),
        'infraname' => $item->get('organism_infraspecific_name')->getString(),
        'abbreviation' => $item->get('organism_abbreviation')->getString(),
        'common_name' => $item->get('organism_common_name')->getString(),
        'comment' => $item->get('organism_comment')->getString(),
      ];

      // Special case handling of abbreviations for genus and infraspecific type
      $values['genus_abbrev'] = substr($values['genus'], 0, 1) . '.';
      $values['infratype_abbrev'] = chado_abbreviate_infraspecific_rank($values['infratype']);

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

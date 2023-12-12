<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Component\Utility\UrlHelper;

/**
 * Plugin implementation of default Tripal dbxref formatter.
 *
 * @FieldFormatter(
 *   id = "chado_dbxref_formatter_default",
 *   label = @Translation("Chado dbxref formatter"),
 *   description = @Translation("A chado dbxref formatter"),
 *   field_types = {
 *     "chado_dbxref_type_default"
 *   },
 *   valid_tokens = {
 *     "[accession]",
 *     "[version]",
 *     "[description]",
 *     "[db_name]",
 *     "[db_description]",
 *     "[db_urlprefix]",
 *     "[db_url]",
 *   },
 * )
 */
class ChadoDbxrefFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public static function defaultSettings() {
    $settings = parent::defaultSettings();
    $settings['token_string'] = '[db_urlprefix]';
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
        'accession' => $item->get('dbxref_accession')->getString(),
        'version' => $item->get('dbxref_version')->getString(),
        'description' => $item->get('dbxref_description')->getString(),
        'db_name' => $item->get('dbxref_db_name')->getString(),
        'db_description' => $item->get('dbxref_db_description')->getString(),
        'db_urlprefix' => $item->get('dbxref_db_urlprefix')->getString(),
        'db_url' => $item->get('dbxref_db_url')->getString(),
      ];

      // Substitue db or accession into db_urlprefix
      $values['db_urlprefix'] = preg_replace('/\{db\}/', $values['db_name'], $values['db_urlprefix']);
      $values['db_urlprefix'] = preg_replace('/\{accession\}/', $values['accession'], $values['db_urlprefix']);

      // Convert urlprefix and url into clickable urls
      if (UrlHelper::isExternal($values['db_url'])) {
        $values['db_url'] = Link::fromTextAndUrl($values['db_name'],
            Url::fromUri($values['db_url']))->toString();
      }
      if (UrlHelper::isExternal($values['db_urlprefix'])) {
        $values['db_urlprefix'] = Link::fromTextAndUrl($values['db_name'] . ':' . $values['accession'],
            Url::fromUri($values['db_urlprefix']))->toString();
      }

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

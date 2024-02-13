<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal contact formatter.
 *
 * @FieldFormatter(
 *   id = "chado_contact_formatter_default",
 *   label = @Translation("Chado contact formatter"),
 *   description = @Translation("A chado contact formatter"),
 *   field_types = {
 *     "chado_contact_type_default"
 *   },
 *   valid_tokens = {
 *     "[name]",
 *     "[description]",
 *     "[type]",
 *   },
 * )
 */
class ChadoContactFormatterDefault extends ChadoFormatterBase {

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
        'name' => $item->get('contact_name')->getString(),
        'description' => $item->get('contact_description')->getString(),
        'type' => $item->get('contact_type')->getString(),
      ];

      // Change the non-user-friendly 'null' contact, which is specified by chado.
      if ($values['name'] == 'null') {
        $values['name'] = 'Unknown';
      }

      // Substitute values in token string to generate displayed string.
      $displayed_string = $token_string;
      foreach ($values as $key => $value) {
        $displayed_string = preg_replace("/\[$key\]/", $value, $displayed_string);
      }

      // Create a link to the corresponding entity.
      $record_id = $item->get('contact_id')->getString();
      $storage = 'chado_storage';
      $bundle = 'contact'; //@@@ how to look up???
      $lookup_service = \Drupal::service('tripal.entity_lookup');
      $url = $lookup_service->getEntityURL($storage, $bundle, $record_id);
      if ($url) {
        $displayed_string = Link::fromTextAndUrl($displayed_string, Url::fromUri($url))->toString();
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

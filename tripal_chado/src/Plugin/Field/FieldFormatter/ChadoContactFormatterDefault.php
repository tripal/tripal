<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal contact formatter.
 */
#[TripalFieldFormatter(
  id: 'chado_contact_formatter_default',
  label: new TranslatableMarkup('Chado contact formatter'),
  description: new TranslatableMarkup('A chado contact formatter'),
  field_types: [
    'chado_contact_type_default',
    'chado_contact_by_role_type_default',
  ],
  valid_tokens: [
    '[name]',
    '[description]',
    '[type]',
  ],
)]
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
    parent::viewElements($items, $langcode);
    $list = [];
    $token_string = $this->getSetting('token_string');
    $lookup_manager = \Drupal::service('tripal.tripal_entity.lookup');

    foreach ($items as $delta => $item) {
      $values = [
        'entity_id' => $item->get('entity_id')->getString(),
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

      // Create a clickable link to the corresponding entity when one exists.
      $renderable_item = $lookup_manager->getRenderableItem($displayed_string, $values['entity_id']);

      $list[$delta] = $renderable_item;
    }

    // Will convert $list to a markup list if there is more than one item.
    $elements = $this->createListMarkup($list);
    return $elements;
  }

}

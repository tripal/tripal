<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Link;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\Core\Url;
use Drupal\Component\Utility\UrlHelper;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal linker property formatter.
 */
#[TripalFieldFormatter(
  id: 'chado_property_formatter_default',
  label: new TranslatableMarkup('Chado Property'),
  description: new TranslatableMarkup('Add a property or attribute to the content type.'),
  field_types: [
    'chado_property_type_default',
  ],
)]
class ChadoPropertyFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    parent::viewElements($items, $langcode);

    // Default filter format.
    $filter_format = 'basic_html';

    // We need to get the format which was set in the widget settings
    // because they need to match.
    // Note: the default filter is used when the widget does not support
    // filter formats (i.e. String and Select widgets).
    $entity_type = $this->fieldDefinition->get('entity_type');
    $bundle = $this->fieldDefinition->get('bundle');
    $field_name = $this->fieldDefinition->get('field_name');
    $form_display = \Drupal::service('entity_display.repository')->getFormDisplay($entity_type, $bundle);
    $widget = $form_display->getComponent($field_name);
    if (array_key_exists('filter_format', $widget['settings'])) {
      $filter_format = $widget['settings']['filter_format'];
    }

    $list = [];
    foreach($items as $delta => $item) {
      $value = $item->get('value')->getValue();
      // any URLs are made into clickable links
      if (UrlHelper::isExternal($value)) {
        $value = Link::fromTextAndUrl($value, Url::fromUri($value))->toString();
      }
      $list[$delta] = [
        '#type' => 'processed_text',
        '#text' => $value,
        '#format' => $filter_format,
        '#langcode' => $item->getLangcode(),
      ];
    }

    // Will convert $list to a markup list if there is more than one item.
    $elements = $this->createListMarkup($list);
    return $elements;
  }
}

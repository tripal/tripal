<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Component\Utility\UrlHelper;
use Drupal\tripal_chado\TripalField\ChadoFormatterBase;

/**
 * Plugin implementation of default Tripal linker property formatter.
 *
 * @FieldFormatter(
 *   id = "chado_property_formatter_default",
 *   label = @Translation("Chado Property"),
 *   description = @Translation("Add a property or attribute to the content type."),
 *   field_types = {
 *     "chado_property_type_default"
 *   }
 * )
 */
class ChadoPropertyFormatterDefault extends ChadoFormatterBase {

  /**
   * {@inheritdoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];

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

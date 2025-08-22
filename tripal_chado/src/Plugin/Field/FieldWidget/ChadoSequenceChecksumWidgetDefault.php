<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldWidget;

use Drupal\Core\Field\Attribute\FieldWidget;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal_chado\TripalField\ChadoWidgetBase;

/**
 * Plugin implementation of default Chado Sequence Checksum widget.
 */
#[TripalFieldWidget(
  id: 'chado_sequence_checksum_widget_default',
  label: new TranslatableMarkup('Chado Sequence Checksum Widget'),
  description: new TranslatableMarkup('The default chado sequence checksum widget.'),
  field_types: [
    'chado_sequence_checksum_type_default',
  ],
)]
class ChadoSequenceChecksumWidgetDefault extends ChadoWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    $item_vals = $items[$delta]->getValue();
    $elements = [];
    $elements['record_id'] = [
      '#type' => 'value',
      '#default_value' => $item_vals['record_id'] ?? 0,
    ];

    return $elements;
  }

}

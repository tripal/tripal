<?php

namespace Drupal\tripal_chado\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldFormatter;

/**
 * Plugin implementation of Default Tripal field formatter for sequence length.
 */
#[TripalFieldFormatter(
  id: 'chado_sequence_length_formatter_default',
  label: new TranslatableMarkup('Chado Sequence Length Formatter'),
  description: new TranslatableMarkup('A chado sequence length formatter'),
  field_types: [
    'chado_sequence_length_type_default',
  ],
)]
class ChadoSequenceLengthFormatterDefault extends ChadoIntegerFormatterDefault {

  /**
   * {@inheritDoc}
   */
  public function viewElements(FieldItemListInterface $items, $langcode) {
    $elements = [];
    $field_prefix = $this->getSetting('field_prefix');
    $field_suffix = $this->getSetting('field_suffix');
    $thousand_separator = $this->getSetting('thousand_separator');
    $hide_condition = $this->getSetting('hide_condition') ?? '';
    $hide_value = $this->getSetting('hide_value') ?? '';

    foreach($items as $delta => $item) {
      $value = $item->get('seqlen')->getValue() ?? '';
      $hide = ((($hide_condition == '') and !$value)
           or (($hide_condition == 'if_value') and ($value == $hide_value)));
      if (!$hide) {
        if (strlen($value) and strlen($thousand_separator)) {
          // For an integer we can hardcode the unused decimal setting to 0
          $value = number_format(floatval($value), 0, '.', $thousand_separator);
        }
        $elements[$delta] = [
          "#markup" => $field_prefix . $value . $field_suffix,
        ];
      }
    }

    return $elements;
  }

}

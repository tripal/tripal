<?php

namespace Drupal\tripal\Plugin\Field\FieldWidget;

use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\StringTranslation\TranslatableMarkup;
use Drupal\tripal\TripalField\Attribute\TripalFieldWidget;
use Drupal\tripal\TripalField\TripalWidgetBase;

/**
 * Plugin implementation of the 'Tripal Citation Widget' field widget.
 */
#[TripalFieldWidget(
  id: 'default_tripal_citation_type_widget',
  label: new TranslatableMarkup('Tripal Citation Widget'),
  description: new TranslatableMarkup('The default citation type widget.'),
  field_types: [
    'tripal_citation_type',
  ],
)]
class TripalCitationTypeWidget extends TripalWidgetBase {

  /**
   * {@inheritdoc}
   */
  public function formElement(FieldItemListInterface $items, $delta, array $element, array &$form, FormStateInterface $form_state) {

    $value = $items[$delta]->value ?? '';

    $entity = $items->getEntity();
    $publications = [];

    foreach ($entity->getFieldDefinitions() as $field_definition) {
      if ($field_definition->getType() !== 'chado_pub_type_default') {
        continue;
      }

      $field_name = $field_definition->getName();
      if (!$entity->hasField($field_name)) {
        continue;
      }

      foreach ($entity->get($field_name) as $publication_item) {
        $pub_id = $publication_item->getValue()['pub_id'] ?? NULL;

        if ($pub_id == '1') {
          continue;
        }
        if ($pub_id !== NULL && $pub_id !== '' && $pub_id !== 0 && $pub_id !== '0') {
          $publications[$pub_id] = $publication_item->getValue()['pub_title'] ?? '';
        }
      }
    }

    if (!empty($publications)) {
      $element['value'] = $element + [
        '#type' => 'select',
        '#title' => $this->t('Citation'),
        '#default_value' => $value,
        '#options' => $publications,
        '#description' => $this->t('Select a publication that to create citation for this content page.'),
      ];
    }
    else {
      $element['value'] = $element + [
        '#title' => $this->t('Citation'),
        '#markup' => 'A citation will be generated using the provided data and will be displayed once the content is created.',
      ];
    }

    return $element;
  }

}

<?php

namespace Drupal\tripal_search_views\Plugin\views\filter;

use Drupal\Core\Form\FormStateInterface;
use Drupal\views\Plugin\views\filter\StringFilter;
use Drupal\views\Attribute\ViewsFilter;

/**
 * String filter with dynamic select options.
 *
* @ingroup views_filter_handlers.
 */
#[ViewsFilter("dynamic_filter")]
class InformedStringFilter extends StringFilter {

  /**
   * {@inheritDoc}
   */
  public function buildExposedForm(&$form, FormStateInterface $form_state) {
    parent::buildExposedForm($form, $form_state);
    $view = $this->view;
    // Only modify the exposed form if this is a filter for a field on
    // Tripal Content and the field name exists.
    if (!$view || $view->storage->get('base_table') !== 'tripal_entity' || !isset($this->definition['field_name'])) {
      return;
    }

    // Get the identifier for this filter.
    $identifier = $this->options['expose']['identifier'];

    // Get the field name from the table name.
    $field_name = $this->definition['field_name'];

    // Get the field value property from the definition.
    $field_val = $this->realField;

    // Get the unique values for this field and build options for the select.
    $results = \Drupal::service('tripal.fieldvalue.lookup')->getUniqueFieldValues($field_name, [], []);
    $options = ['' => $this->t('- Any -')];
    foreach ($results as $row) {
      $value = $row[$field_val];
      $options[$value] = $value;
    }

    // Replace the default text input with a select element.
    $form[$identifier] = [
      '#type' => 'select',
      '#title' => $this->options['expose']['label'],
      '#options' => $options,
      '#default_value' => $this->value,
    ];

  }

}

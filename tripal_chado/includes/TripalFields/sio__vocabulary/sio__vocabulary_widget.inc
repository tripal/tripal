<?php

class sio__vocabulary_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Vocabulary';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['sio__vocabulary'];


  /**
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {

    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // If the items array is empty then we are creating a new entity.
    // Since this is set when the entity type is created, we don't want to allow
    // content managers to change it. Thus we need to look up the value for the
    // entity type and use it here.
    if (empty($items)) {

      // Use the bundle to get the cv_id choosen for this cvterm-based entity.
      // ASSUMPTION: the cv_id is saved as the "type_value" of the bundle.
      $bundle = tripal_load_bundle_entity(['name' => $widget['#instance']['bundle']]);
      $cv = chado_get_cv(['cv_id' => $bundle->type_value]);

      // Now populate the items array with defaults based on the cv.
      if ($cv) {
        $items[$delta] = [
          'value' => $cv->name,
          'chado-' . $field_table . '__cv_id' => $cv->cv_id,
        ];
      }
      else {
        tripal_report_error(
          $field_name,
          TRIPAL_ERROR,
          'Unable to determine default vocabulary for :name Tripal Content Type',
          [':name' => $bundle->label]
        );
        drupal_set_message(t('Unable to determine default vocabulary for :name Tripal Content Type',
          [':name' => $bundle->label]), 'error');
      }
    }

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];
    $widget['chado-' . $field_table . '__cv_id'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['chado-' . $field_table . '__cv_id'] : '',
    ];
    $widget['vocabulary_name'] = [
      '#type' => 'item',
      '#title' => 'Vocabulary',
      '#markup' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];
  }

  /**
   * @see TripalFieldWidget::validate()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {

    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];

    // Make sure the value is set to the cv_id
    $cv_id = $form_state['values'][$field_name][$langcode][0]['chado-' . $field_table . '__cv_id'];
    $form_state['values'][$field_name][$langcode][0]['value'] = $cv_id;
  }
}

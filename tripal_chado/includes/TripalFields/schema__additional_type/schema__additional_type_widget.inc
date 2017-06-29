<?php

class schema__additional_type_widget extends ChadoFieldWidget {
  // The default lable for this field.
  public static $default_label = 'Type';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('schema__additional_type');

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $vocabulary = $this->instance['settings']['vocabulary'];
    $parent_term = $this->instance['settings']['parent_term'];

    $record_id = $element['#entity']->chado_record_id;
    $value = '';
    $type_id = '';

    if (count($items) > 0) {
      $type_id = $items[0]['chado-' . $field_table . '__type_id'];
      $value = $items[0]['value'];
    }

    if (array_key_exists('values', $form_state) and array_key_exists($field_name, $form_state['values'])) {
      $type_id = $form_state['values'][$field_name][$langcode][$delta]['value'];
    }

    $widget['value'] = array(
      '#type' => 'value',
      '#value' => $value,
    );

    If ($field_table == 'pub' && empty($vocabulary)){
      $vocabulary = 'tripal_pub';
    };
    // If a parent_term is provided then use that to get the options
    $options = array();
    $options[] = 'Select a type';
    if ($parent_term) {
      list ($vocabulary, $accession) = explode(':', $parent_term);
      if ($vocabulary and $accession) {
        $sql = "
          SELECT
            CVTS.cvterm_id, CVTS.name
          FROM {cvtermpath} CVTP
            INNER JOIN {cvterm} CVTS ON CVTP.subject_id = CVTS.cvterm_id
            INNER JOIN {cvterm} CVTO ON CVTP.object_id = CVTO.cvterm_id
            INNER JOIN {dbxref} DBXO ON DBXO.dbxref_id = CVTO.dbxref_id
            INNER JOIN {db} DBO      ON DBO.db_id = DBXO.db_id
          WHERE
            DBO.name = :vocabulary AND DBXO.accession = :accession AND
            NOT CVTS.is_obsolete = 1
          ORDER BY CVTS.name ASC
       ";
        $results = chado_query($sql, array(':vocabulary' => $vocabulary, ':accession' => $accession));
        while ($term = $results->fetchObject()) {
          $options[$term->cvterm_id] = $term->name;
        }
      }
    }
    elseif ($vocabulary) {
      $cv = tripal_get_cv(array('name' => $vocabulary));
      $options = tripal_get_cvterm_select_options($cv->cv_id);
    }
    $widget['chado-' . $field_table . '__type_id'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#title' => $element['#title'],
      '#description' => $element['#description'],
      '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
      '#default_value' => $type_id,
      '#delta' => $delta,
    );
  }

  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $vocabulary = $this->instance['settings']['vocabulary'];
    $parent_term = $this->instance['settings']['parent_term'];

    $type_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__type_id'];

    if (!$type_id) {
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__type_id'] = '__NULL__';
    }
    else {
      $form_state['values'][$field_name]['und'][$delta]['value'] = $type_id;
    }
  }
}
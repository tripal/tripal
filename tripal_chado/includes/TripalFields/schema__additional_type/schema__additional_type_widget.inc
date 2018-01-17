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
            INNER JOIN {cvterm} CVTT ON CVTP.type_id = CVTT.cvterm_id
            INNER JOIN {dbxref} DBXO ON DBXO.dbxref_id = CVTO.dbxref_id
            INNER JOIN {db} DBO      ON DBO.db_id = DBXO.db_id
          WHERE
            DBO.name = :vocabulary AND DBXO.accession = :accession AND
            (CVTT.name = 'is a' OR CVTT.name = 'is_a') AND
            CVTS.name != CVTO.name AND
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
    // If no vocabulary or parent term are provided then just give a generic
    // term finder.
    else {
      $cvterm = NULL;
      $default_name = '';
      if ($type_id) {
        $cvterm = chado_generate_var('cvterm', array('cvterm_id' => $type_id));
        $default_name = $cvterm->name;
      }
      tripal_get_term_lookup_form($widget, $form_state, $default_name,
          $element['#title'], $element['#description'], $element['#required'],
          $field_name, $delta);
    }
    if ($vocabulary) {
      $widget['chado-' . $field_table . '__type_id'] = array(
        '#type' => 'select',
        '#options' => $options,
        '#title' => $element['#title'],
        '#description' => $element['#description'],
        '#weight' => isset($element['#weight']) ? $element['#weight'] : 0,
        '#default_value' => $type_id,
        '#required' => $element['#required'],
        '#delta' => $delta,
      );
    }
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

    // Get the vocabulary term.
    $type_id = '';
    if ($vocabulary) {
      $type_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__type_id'];
    }
    else {
      $selected = tripal_get_term_lookup_form_result($form, $form_state, $field_name, $delta);
      if (count($selected) == 1) {
        $type_id = $selected[0]->cvterm_id;
      }
      if (count($selected) > 1) {
        form_set_error($field_name . '[' . $langcode . '][' . $delta . '][term_match][term_name', 'Please select only one vocabulary term.');
      }
    }

    if (!$type_id) {
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__type_id'] = '__NULL__';
    }
    else {
      $form_state['values'][$field_name]['und'][$delta]['value'] = $type_id;
    }
  }
}
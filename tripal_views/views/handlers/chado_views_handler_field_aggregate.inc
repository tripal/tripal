<?php

/**
 * @file
 * A chado wrapper for the views_handler_field.
 *
 * Handles fields which may be aggregated during the chado join process. This field
 * will render an aggregated field as a pre_rendered list and will dynamically detect
 * whether the field is aggregated or not.
 */
class chado_views_handler_field_aggregate extends chado_views_handler_field {

  function init(&$view, $options) {
    parent::init($view, $options);

    if (!isset($this->chado_table_description)) {
      $this->chado_table_description = tripal_core_get_chado_table_schema($this->table);
      foreach ($this->chado_table_description['foreign keys'] as $defn) {
        if ($defn['table'] != $this->view->base_table) {
          $join_table = tripal_core_get_chado_table_schema($defn['table']);
          foreach ($join_table['fields'] as $fname => $f) {
            $this->chado_table_description['fields'][$defn['table'] . '_' . $fname] = $f;
          }
        }
      }
    }

  }

  /**
   * Defines the options form (form available to admin when they add a field to a view)
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['format'] = array(
      '#type' => 'fieldset',
      '#title' => 'Format Output',
      '#description' => t('The following fields specify how a single result of this field will be
        displayed. When there are multiple results of this field due to aggregation, each result
        will be rendered according to the following rules and then all results will be joined
        together based on the "Display Type" indicated.')
    );

    $this->tokens = array();
    $value = array();
    foreach ( array_keys($this->chado_table_description['fields']) as $field ) {
      $t = '[' . $this->options['id'] . '-' . $field . ']';
      $this->tokens[$t] = t($field);
      $value[] = $t . ' == ' . $field;
    }

    $form['format']['format_string'] = array(
      '#type' => 'textfield',
      '#title' => t('Format String'),
      '#description' => 'Use any of the format tokens below to indicate what fields you want displayed.',
      '#default_value' => ($this->options['format']['format_string']) ? $this->options['format']['format_string'] : implode(', ', array_keys($this->tokens)),
    );

    $form['format']['tokens'] = array(
      '#type' => 'item',
      '#title' => 'Format Tokens',
      '#value' => implode("<br />", $value),
    );

  }

  function query() {
    parent::query();

    $this->table_definition = $this->query->get_table_info($this->table);
  }


  function pre_render(&$values) {

    if ($this->aggregated) {
      foreach ($values as $k => $v) {
        $values[$k]->{$this->field_alias} = $this->split_array_agg_results($v->{$this->field_alias});

        foreach ($values[$k]->{$this->field_alias} as &$val) {

          // First, get the token values
          $subparts = explode(',', $val);
          $token_values = array();
          foreach ($subparts as $ssk => $ssv) {
            if (preg_match('/(.*)::(.*)/', $ssv, $matches)) {
              $token_values[ '[all-' . $matches[1] . ']' ] = $matches[2];
            }
          }

          // Now manually sub them in
          $val = str_replace(array_keys($token_values), $token_values, $this->options['format']['format_string']);

        }
      }

    }

  }

}

<?php

class taxrank__infraspecific_taxon_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Infraspecific Taxon';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['taxrank__infraspecific_taxon'];


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

    $infra_name = '';
    $type_id = 0;

    if (count($items) > 0) {
      $type_id = array_key_exists('chado-organism__type_id', $items[0]) ? $items[0]['chado-organism__type_id'] : $type_id;
      $infra_name = array_key_exists('chado-organism__infraspecific_name', $items[0]) ? $items[0]['chado-organism__infraspecific_name'] : $infra_name;
    }

    if (array_key_exists('values', $form_state) and
      array_key_exists($field_name, $form_state['values'])) {
      $type_id = $form_state['values'][$field_name][$langcode][$delta]['infraname']['specific_epithet'];
      $infra_name = $form_state['values'][$field_name][$langcode][$delta]['infraname']['infraspecific_epithet'];
    }

    $widget['value'] = [
      '#type' => 'value',
      '#value' => array_key_exists($delta, $items) ? $items[$delta]['value'] : '',
    ];
    $widget['chado-organism__type_id'] = [
      '#type' => 'value',
      '#value' => $type_id,
    ];
    $widget['chado-organism__infraspecific_name'] = [
      '#type' => 'value',
      '#value' => $infra_name,
    ];

    $cv = chado_get_cv(['name' => 'taxonomic_rank']);
    $terms = chado_get_cvterm_select_options($cv->cv_id);

    // Unfortunately the taxonomic_rank vocabulary is not properly organized
    // such that we only include terms below 'species'. Therefore we will
    // just list them here and hope we haven't missed one.
    $valid_terms = [
      'cultivar',
      'subspecies',
      'varietas',
      'subvariety',
      'forma',
      'subforma',
    ];
    $options = [];
    $options[] = '--Select a rank--';
    foreach ($terms as $cvterm_id => $name) {
      if (in_array($name, $valid_terms)) {
        $options[$cvterm_id] = $name;
      }
    }

    $widget['infraname'] = [
      '#type' => 'fieldset',
      '#title' => $element['#title'],
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
      '#description' => $element['#description'],
    ];
    $widget['infraname']['specific_epithet'] = [
      '#type' => 'select',
      '#title' => 'Specific Epithet',
      '#description' => 'The rank below species. This field may be left blank if not appropriate.',
      '#options' => $options,
      '#default_value' => $type_id,
    ];
    $widget['infraname']['infraspecific_epithet'] = [
      '#type' => 'textfield',
      '#title' => 'Infraspecific Epithet',
      '#description' => 'The name below species.',
      '#default_value' => $infra_name,
      '#required' => $element['#required'],
    ];
  }

  /**
   * @see TripalFieldWidget::validate()
   */
  public function validate($element, $form, &$form_state, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $type_id = $form_state['values'][$field_name]['und'][0]['infraname']['specific_epithet'];
    $infra_name = $form_state['values'][$field_name]['und'][0]['infraname']['infraspecific_epithet'];

    if ($infra_name and $type_id) {
      $form_state['values'][$field_name]['und'][0]['value'] = $infra_name;
      $form_state['values'][$field_name]['und'][0]['chado-organism__type_id'] = $type_id;
      $form_state['values'][$field_name]['und'][0]['chado-organism__infraspecific_name'] = $infra_name;
    }
    else {
      if ($infra_name and !$type_id) {
        $form_state['values'][$field_name]['und'][0]['value'] = $infra_name;
        $form_state['values'][$field_name]['und'][0]['chado-organism__type_id'] = '__NULL__';
        $form_state['values'][$field_name]['und'][0]['chado-organism__infraspecific_name'] = $infra_name;
      }
      else {
        $form_state['values'][$field_name]['und'][0]['chado-organism__type_id'] = '__NULL__';
        $form_state['values'][$field_name]['und'][0]['chado-organism__infraspecific_name'] = '__NULL__';
      }
    }

  }
}

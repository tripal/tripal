<?php

class sio__annotation_widget extends ChadoFieldWidget {

  // The default label for this field.
  public static $default_label = 'Chado Annotation';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = ['chado_linker__cvterm'];


  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    $vocabulary = chado_get_semweb_term('cvterm', 'cv_id');
    $accession = chado_get_semweb_term('dbxref', 'accession');
    $definition = chado_get_semweb_term('cvterm', 'definition');
    if (array_key_exists('is_not', $schema['fields'])) {
      $negation = chado_get_semweb_term($field_table, 'is_not');
    }

    // Get the field defaults.
    $record_id = '';
    $fk_value = '';
    $cvterm_id = '';
    $pub_id = '';
    $is_not = FALSE;
    $cvterm_name = '';
    $cv_id = '';
    $cvterm = NULL;
    $pub_name = '';

    // If the field already has a value then it will come through the $items
    // array.  This happens when editing an existing record.
    if (array_key_exists($delta, $items)) {
      // Check for element values that correspond to fields in the Chado table.
      $record_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $pkey, $record_id);
      $fk_value = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__' . $fkey_lcolumn, $fk_value);
      $cvterm_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__cvterm_id', $cvterm_id);

      if (array_key_exists('pub_id', $schema['fields'])) {
        $pub_name = tripal_get_field_item_keyval($items, $delta, 'pub', $pub_name);
        $pub_id = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__pub_id', $pub_id);
      }
      if (array_key_exists('is_not', $schema['fields'])) {
        $is_not = tripal_get_field_item_keyval($items, $delta, 'chado-' . $field_table . '__is_not', $is_not);
      }

      if ($cvterm_id) {
        $cvterm = chado_generate_var('cvterm', ['cvterm_id' => $cvterm_id]);
        $cv_id = $cvterm->cv_id->cv_id;
        $cvterm_name = $cvterm->name;
      }
    }

    // Check $form_state['values'] to see if an AJAX call set the values.
    if (array_key_exists('values', $form_state) and
      array_key_exists($field_name, $form_state['values'])) {

      $record_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $pkey];
      $fk_value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $fkey_lcolumn];
      $cvterm_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__cvterm_id'];

      if (array_key_exists('pub_id', $schema['fields'])) {
        $pub_name = $form_state['values'][$field_name]['und'][$delta]['pub'];
        $pub_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__pub_id'];
      }
      if (array_key_exists('is_not', $schema['fields'])) {
        $is_not = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__is_not'];
      }
      $cvterm_name = $form_state['values'][$field_name]['und'][$delta]['cvterm_name'];
      $cv_id = $form_state['values'][$field_name]['und'][$delta]['cv_id'];
      $cvterm = chado_generate_var('cvterm', [
        'cv_id' => $cv_id,
        'name' => $cvterm_name,
      ]);
      if (!$cvterm) {
        $cvterm_name = '';
      }
    }

    $widget['#prefix'] = "<span id='$field_name-sio--annotation-$delta'>";
    $widget['#suffix'] = "</span>";

    // The value field isn't really used but it's needed because if
    // it doesn't have a value the element won't be considered for
    // insert/update.
    $widget['value'] = [
      '#type' => 'value',
      '#value' => $cvterm_id,
    ];

    $widget['chado-' . $field_table . '__' . $pkey] = [
      '#type' => 'value',
      '#default_value' => $record_id,
    ];
    $widget['chado-' . $field_table . '__cvterm_id'] = [
      '#type' => 'value',
      '#default_value' => $cvterm_id,
    ];
    $widget['chado-' . $field_table . '__' . $fkey_lcolumn] = [
      '#type' => 'value',
      '#default_value' => $fk_value,
    ];

    $cvs = chado_get_cv_select_options();
    $widget['cv_id'] = [
      '#type' => 'select',
      '#title' => t('Vocabulary'),
      '#options' => $cvs,
      '#default_value' => $cv_id,
      '#required' => $element['#required'],
      '#attributes' => ['style' => 'width: 200px;'],
      '#ajax' => [
        'callback' => "sio__annotation_widget_form_ajax_callback",
        'wrapper' => "$field_name-sio--annotation-$delta",
        'effect' => 'fade',
        'method' => 'replace',
      ],
    ];
    $cv_schema = chado_get_schema('cvterm');
    $widget['cvterm_name'] = [
      '#type' => 'textfield',
      '#title' => t('Term Name'),
      '#default_value' => $cvterm_name,
      '#maxlength' => array_key_exists('length', $cv_schema['fields']['name']) ? $cv_schema['fields']['name']['length'] : 255,
      '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/cvterm/' . $cv_id,
      '#disabled' => $cv_id ? FALSE : TRUE,
    ];

    if (array_key_exists('pub_id', $schema['fields'])) {
      $widget['pub'] = [
        '#type' => 'textfield',
        '#title' => t('Publication'),
        '#default_value' => $pub_name,
        '#autocomplete_path' => 'admin/tripal/storage/chado/auto_name/pub',
        '#maxlength' => 100000,
        '#disabled' => $cv_id ? FALSE : TRUE,
      ];
      $widget['chado-' . $field_table . '__pub_id'] = [
        '#type' => 'value',
        '#default_value' => $pub_id ? $pub_id : '',
      ];
    }

    if (array_key_exists('is_not', $schema['fields'])) {
      $widget['chado-' . $field_table . '__is_not'] = [
        '#type' => 'checkbox',
        '#title' => t('Negate this term (NOT)'),
        '#default_value' => $is_not,
        '#required' => $element['#required'],
        '#disabled' => $cv_id ? FALSE : TRUE,
      ];
    }
    if (array_key_exists('rank', $schema['fields'])) {
      $widget['chado-' . $field_table . '__rank'] = [
        '#type' => 'value',
        '#default_value' => $delta,
      ];
    }
  }

  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    $vocabulary = chado_get_semweb_term('cvterm', 'cv_id');
    $accession = chado_get_semweb_term('dbxref', 'accession');
    $definition = chado_get_semweb_term('cvterm', 'definition');
    if (array_key_exists('is_not', $schema['fields'])) {
      $negation = chado_get_semweb_term($field_table, 'is_not');
    }

    $record_id = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $pkey];
    $fk_value = $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $fkey_lcolumn];

    // If a publication ID was provided then make sure the form_state
    // value for the pub_id is set correctly.
    if (array_key_exists('pub_id', $schema['fields'])) {
      $pub_name = $form_state['values'][$field_name]['und'][$delta]['pub'];
      if ($pub_name) {
        $pub = chado_generate_var('pub', ['uniquename' => $pub_name]);
        if ($pub) {
          $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__pub_id'] = $pub->pub_id;
        }
      }
      // Use the NULL pub.
      else {
        $pub = chado_get_publication(['uniquename' => 'null']);
        $form_state['values'][$field_name][$langcode][$delta]['chado-' . $field_table . '__pub_id'] = $pub->pub_id;
      }
    }

    // Make sure the rank is set.
    if (array_key_exists('rank', $schema['fields'])) {
      $rank = $form_state['values'][$field_name]['und'][$delta]['_weight'];
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__rank'] = $rank;
    }

    // Get the term that matches.
    $cvterm_name = $form_state['values'][$field_name]['und'][$delta]['cvterm_name'];
    $cv_id = $form_state['values'][$field_name]['und'][$delta]['cv_id'];
    $cvterm = chado_generate_var('cvterm', [
      'cv_id' => $cv_id,
      'name' => $cvterm_name,
    ]);
    if ($cvterm) {
      $form_state['values'][$field_name]['und'][$delta]['cvterm_name'] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__cvterm_id'] = $cvterm->cvterm_id;
      $form_state['values'][$field_name]['und'][$delta]['value'] = $cvterm->cvterm_id;
    }
    // Remove all values so we can delete this record if there is no
    // cvterm.
    else {
      // There must be some value set.
      $form_state['values'][$field_name]['und'][$delta]['value'] = 'delete_me';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__cvterm_id'] = '';
      $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__' . $fkey_lcolumn] = '';
      if (array_key_exists('rank', $schema['fields'])) {
        $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__rank'] = '';
      }
      if (array_key_exists('pub_id', $schema['fields'])) {
        $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '__pub_id'] = '';
      }
      if (array_key_exists('is_not', $schema['fields'])) {
        $form_state['values'][$field_name]['und'][$delta]['chado-' . $field_table . '_is_not'] = '';
      }
    }
  }

  /**
   * @see TripalFieldWidget::theme()
   */
  public function theme($element) {
    $field_table = $this->instance['settings']['chado_table'];
    $layout = "
      <div class=\"sio--annotation-widget\">
        <div class=\"sio--annotation-item\">" .
      drupal_render($element['cv_id']) . "
        </div>
        <div class=\"sio--annotation-item\">" .
      drupal_render($element['cvterm_name']) . "
        </div>
        <div class=\"sio--annotation-item\">" .
      drupal_render($element['pub']) . "
        </div>
        <div class=\"sio--annotation-item\">" .
      drupal_render($element['chado-' . $field_table . '__is_not']) . "
        </div>
      </div>
    ";

    return $layout;
  }
}

/**
 * An Ajax callback for the tripal_chado_admin_publish_form..
 */
function sio__annotation_widget_form_ajax_callback($form, $form_state) {
  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];

  return $form[$field_name]['und'][$delta];
}

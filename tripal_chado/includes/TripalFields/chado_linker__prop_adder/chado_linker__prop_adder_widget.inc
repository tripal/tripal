<?php

class chado_linker__prop_adder_widget extends TripalFieldWidget {
  // The default lable for this field.
  public static $default_label = 'Add a Property Type';

  // The list of field types for which this formatter is appropriate.
  public static $field_types = array('chado_linker__prop_adder');

  /**
   *
   * @see TripalFieldWidget::form()
   */
  public function form(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {
    parent::form($widget, $form, $form_state, $langcode, $items, $delta, $element);
    $field_name = $widget['#field_name'];

    $widget['#type'] = 'fieldset';
    $widget['#title'] = $element['#title'];
    $widget['#description'] = $element['#description'];
    $widget['#group'] = 'entity_form_vtabs';


    $widget['kvproperty_instructions'] = array(
      '#type' => 'item',
      '#markup' => t('You may add additional properties to this form by
          providing a property name (from a vocabulary) in the field below
          and clicking the "Lookup Term" button.  Terms that match the value
          entered will be displayed for selection.  After selecting the
          appropriate term click the "Use this term" button and a
          new field will be added to the form above for the property you selected.
          In the future, this field will be present for all records
          of this type.'),
    );

    $term_name = isset($form_state['values'][$field_name]['und'][0]['wrapper']['term_name']) ? $form_state['values'][$field_name]['und'][0]['wrapper']['term_name'] : '';

    // Drupal's vertical feild set is a bit quirky in that we can't just
    // add a prefix and suffix to the weidget.  If we do, then the
    // field doesn't show up on the page after an AJAX call.  We have to add
    // an internal wrapper (below) for AJAX calls.
    $widget['wrapper'] = array(
      '#prefix' =>  "<span id='$field_name-lookup-form'>",
      '#suffix' => '</span>',
    );

    // If no term has been selected yet then provide the auto complete field.
    $widget['wrapper']['term_name'] = array(
      '#title'       => t('Term'),
      '#type'        => 'textfield',
      '#description' => t("The content type must be the name of a term in
        a controlled vocabulary and the controlled vocabulary should
        already be loaded into Tripal.  For example, to create a content
        type for storing 'genes', use the 'gene' term from the
        Sequence Ontology (SO)."),
      '#default_value' => $term_name,
      '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/",
    );

    $widget['wrapper']['select_button'] = array(
      '#type' => 'button',
      '#value' => t('Lookup Term'),
      '#name' => 'select_cvterm',
      '#ajax' => array(
        'callback' => "tripal_chado_prop_adder_form_ajax_callback",
        'wrapper' => "$field_name-lookup-form",
        'effect' => 'fade',
        'method' => 'replace'
      ),
    );
    if ($term_name) {
      $widget['wrapper']['terms_list'] = array(
        '#type' => 'fieldset',
        '#title' => t('Matching Terms'),
        '#description' => t('Please select the term the best matches the
          content type you want to create. If the same term exists in
          multiple vocabularies you will see more than one option below.')
      );
      $match = array(
        'name' => $term_name,
      );
      $terms = chado_generate_var('cvterm', $match, array('return_array' => TRUE));
      $terms = chado_expand_var($terms, 'field', 'cvterm.definition');
      $num_terms = 0;
      foreach ($terms as $term) {
        // Save the user a click by setting the default value as 1 if there's
        // only one matching term.
        $default = FALSE;
        $attrs = array();
        if ($num_terms == 0 and count($terms) == 1) {
          $default = TRUE;
          $attrs = array('checked' => 'checked');
        }
        $widget['wrapper']['terms_list']['term-' . $term->cvterm_id] = array(
          '#type' => 'checkbox',
          '#title' =>  $term->name,
          '#default_value' => $default,
          '#attributes' => $attrs,
          '#description' => '<b>Vocabulary:</b> ' . $term->cv_id->name . ' (' . $term->dbxref_id->db_id->name . ') ' . $term->cv_id->definition .
          '<br><b>Term: </b> ' . $term->dbxref_id->db_id->name . ':' . $term->dbxref_id->accession . '.  ' .
          '<br><b>Definition:</b>  ' . $term->definition,
        );
        $num_terms++;
      }
      if ($num_terms == 0) {
        $widget['wrapper']['terms_list']['none'] = array(
          '#type' => 'item',
          '#markup' => '<i>' . t('There is no term that matches the entered text.') . '</i>'
        );
      }
      else {
        $widget['wrapper']['cardinality'] = array(
          '#title'       => t('Number of Values'),
          '#type'        => 'textfield',
          '#description' => t("A number of 1 or more indicating the number of values allowed for this property. Enter -1 for unlimited values"),
          '#size' => 10,
          '#default_value' => 1,
        );
        // Add in the button for the cases of no terms or too many.
        $widget['wrapper']['submit_button'] = array(
          '#type' => 'submit',
          '#value' => t('Use this term'),
          '#name' => 'use_term_button',
        );
      }
    }
  }

  /**
   * Performs validation of the widgetForm.
   *
   * Use this validate to ensure that form values are entered correctly.  Note
   * this is different from the validate() function which ensures that the
   * field data meets expectations.
   *
   * @param $form
   * @param $form_state
   */
  public function validate($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    $field = $this->field;
    $field_name = $field['field_name'];

    if ($form_state['triggering_element']['#name'] == 'use_term_button') {
      // If the user has clicked the 'user_term_button' then we need to makes
      // sure that the cardinality textbox contains a positive integer.
      $cardinality = $form_state['values'][$field_name][$langcode][$delta]['wrapper']['cardinality'];
      if (!preg_match('/^-*\d+$/', $cardinality) or $cardinality < -2 or $cardinality == 0) {
        form_set_error("$field_name][$langcode][$delta][wrapper][cardinality", "Please provide positive number for the number of values, or -1 for unlimited.");
      }

      // Get selected terms
      $terms_list = $form_state['values'][$field_name][$langcode][$delta]['wrapper']['terms_list'];
      $counter = 0;
      $selected_term_id = '';
      foreach ($terms_list AS $term => $selected) {
        if ($selected) {
          $selected_term_id = str_replace('term-', '', $term);
          $counter ++;
        }
      }
      // Make sure at least a term is selected
      if ($counter == 0) {
        form_set_error("$field_name][$langcode][$delta][wrapper][terms_list", "Please select a term.");
      }
      // Make sure only one term is selected
      if ($counter > 1) {
        form_set_error("$field_name][$langcode][$delta][wrapper][terms_list", "Please select only one term.");
      }
      // Make sure this property doesn't already have a field
      if (key_exists($field_name . '__' . $selected_term_id, $form_state['values'])) {
        form_set_error("$field_name][$langcode][$delta][wrapper][terms_name", "Property already exists. Please select another term.");
      }
    }
  }


  /**
   *
   * @see TripalFieldWidget::submit()
   */
  public function submit($form, &$form_state, $entity_type, $entity, $langcode, $delta) {
    // Add the new field to the entity but only if the property adder button
    // was clicked
    if (!array_key_exists('triggering_element', $form_state) or
        $form_state['triggering_element']['#name'] != 'use_term_button') {
          return;
        }
        else {
          // Because we're going to add a new property we want to rebuild the form
          // rather than have it fully submit.
          $form_state['rebuild'] = TRUE;
        }

        // Get the table and base table.
        $base_table = $this->field['settings']['base_table'];

        // Get the term for the property
        $field = $this->field;
        $fname = $field['field_name'];
        $cardinality = $form_state['values'][$fname][$langcode][$delta]['wrapper']['cardinality'];
        // Get selected terms
        $terms_list = $form_state['values'][$fname][$langcode][$delta]['wrapper']['terms_list'];
        $selected_term_id = '';
        foreach ($terms_list AS $term => $selected) {
          if ($selected) {
            $selected_term_id = str_replace('term-', '', $term);
          }
        }
        $cvterm = chado_generate_var('cvterm', array('cvterm_id' => $selected_term_id));

        // Generate the name for the property table and the field name that we'll
        // be creating.
        $prop_table = $base_table . 'prop';
        $field_name = $prop_table . '__' . $cvterm->cvterm_id;

        // The field name is the table name in this case. We want to get the
        // primary key as this should be the field that maps th the value.
        $schema = chado_get_schema($prop_table);
        $pkey = $schema['primary key'][0];

        // Add the field if it doesn't already exists.
        $field = field_info_field($field_name);
        if (!$field) {
          $field = field_create_field(array(
            'field_name' => $field_name,
            'type' => 'chado_linker__prop',
            'cardinality' => $cardinality,
            'locked' => FALSE,
            'storage' => array(
              'type' => 'field_chado_storage',
            ),
            'settings' => array(
              'chado_table' => $prop_table,
              'chado_column' => $pkey,
              'base_table' => $base_table,
            ),
          ));
        }

        // Create an instance of the field.
        $instance = field_info_instance($entity_type, $field_name,  $entity->bundle);
        if (!$instance) {
          $instance = field_create_instance(array(
            'field_name' => $field_name,
            'entity_type' => 'TripalEntity',
            'bundle' => $entity->bundle,
            'label' => ucfirst(preg_replace('/_/', ' ', $cvterm->name)),
            'description' => $cvterm->definition ? $cvterm->definition : '',
            'required' => FALSE,
            'settings' => array(
              'auto_attach' => FALSE,
            ),
            'widget' => array(
              'type' => 'chado_linker__prop_widget',
              'settings' => array(
                'display_label' => 1,
              ),
            ),
            'display' => array(
              'default' => array(
                'label' => 'inline',
                'type' => 'chado_linker__prop_formatter',
                'settings' => array(),
              ),
            ),
          ));
        }
  }
}

/**
 *
 */
function tripal_chado_prop_adder_form_ajax_callback($form, $form_state) {
  $field_name = $form_state['triggering_element']['#parents'][0];

  // Because this field is inside a vertical fieldset we can't just
  // return $form[$field_name]. We have set the AJAX call to replace
  // everything inside of the 'wrapper' element, so we must return that.
  return $form[$field_name]['und'][0]['wrapper'];
}
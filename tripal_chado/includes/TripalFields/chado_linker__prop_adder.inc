<?php

class chado_linker__prop_adder extends TripalField {
  // The default lable for this field.
  public static $default_label = 'Add a Property Type';

  // The default description for this field.
  public static $default_description = 'This record may have any number of properties. Use
            this field to first add the type.';

  // Add any default settings elements.  If you override the globalSettingsForm()
  // or the instanceSettingsForm() functions then you need to be sure that
  // any settings you want those functions to manage are listed in this
  // array.
  public static $default_settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
    'semantic_web' => '',
  );

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $default_storage = 'field_chado_storage';

  /**
   * @see TripalField::formatterView()
   */
  public function formatterView(&$element, $entity_type, $entity, $langcode, $items, $display) {
    // This field should never be viewed. It's to help add new properties
    // when editing an entity.  So return nothing.
    return '';
  }

  /**
   * @see TripalField::widgetForm()
   */
  public function widgetForm(&$widget, &$form, &$form_state, $langcode, $items, $delta, $element) {

    $field_name = $widget['#field_name'];

    $widget['#type'] = 'fieldset';
    $widget['#title'] = $element['#title'];
    $widget['#description'] = $element['#description'];
    $widget['#group'] = 'entity_form_vtabs';


/*     $widget['kvproperty_instructions'] = array(
      '#type' => 'item',
      '#markup' => t('You may add additional properties to this form by
          providing a property name (from a vocabulary) in the field below
          and clicking the "Add Property" button.  This will add a
          new field to the form above for the property you entered.
          In the future, this field will be present for all records
          of this type.'),
    ); */
    $term_name = array_key_exists('values', $form_state) ? $form_state['values']['term_name'] : '';

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
      '#required'    => TRUE,
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
      // Add in the button for the cases of no terms or too many.
      $widget['wrapper']['submit_button'] = array(
        '#type' => 'submit',
        '#value' => t('Use this term'),
        '#name' => 'use_cvterm'
      );
    }

    $widget['wrapper']['cancel_button'] = array(
      '#type' => 'button',
      '#value' => t('Cancel'),
      '#name' => 'cancel_button',
      '#limit_validation_errors' => array(),
      '#suffix' =>  "</span>",
    );
  }

  /**
   * @see TripalField::widgetFormValidate
   */
  public function widgetFormValidate($entity_type, $entity, $langcode, $items, &$errors) {

    // We will never have more than one item for this field at a time, so
    // delta is always zero.
    $delta = 0;

    // Make sure the cvterm for this property is uniquely identified.
    $kvproperty = tripal_get_field_item_keyval($items, $delta, 'value', '');
    $cvterms = chado_generate_var('cvterm', array('name' => $kvproperty), $options = array('return_array' => TRUE));

    if (trim($kvproperty) && count($cvterms) == 1) {
      // Get the table and base table.
      $base_table = $this->field['settings']['base_table'];

      // Generate the name for the property table and the field name that we'll
      // be creating.
      $prop_table = $base_table . 'prop';
      $field_name = $prop_table . '__' . $cvterms[0]->cvterm_id;
      // Create an instance of the field.
      $instance = field_info_instance($entity_type, $field_name,  $entity->bundle);
      if ($instance) {
        $errors[$this->field['field_name']][$langcode][$delta][] = array(
          'error' => 'chado_linker__prop_adder',
          'message' => t("The property already exists."),
        );
      }
    }
    if (trim($kvproperty) && count($cvterms) > 1) {
      $errors[$this->field['field_name']][$langcode][$delta][] = array(
        'error' => 'chado_linker__prop_adder',
        'message' => t("This term is present in multiple vocabularies. Please select the appropriate one."),
      );
    }
    if (trim($kvproperty) && count($cvterms) == 0) {
      $errors[$this->field['field_name']][$langcode][$delta][] = array(
        'error' => 'chado_linker__prop_adder',
        'message' => t("Please provide a property type to add."),
      );
    }
  }

  /**
   * @see TripalField::widgetFormSubmit()
   */
  public function widgetFormSubmit($entity_type, $entity, $langcode, &$items, $form, &$form_state) {dpm($form);return;
    // We will never have more than one item for this field at a time, so
    // delta is always zero.
    $delta = 0;

    // Add the new field to the entity but only if the property adder button
    // was clicked
    if (!array_key_exists('triggering_element', $form_state) or
      $form_state['triggering_element']['#name'] != 'kvproperty_adder_button') {
      return;
    }

    // Because we're going to add a new property we want to rebuild the form
    // rather than have it fully submit.
    $form_state['rebuild'] = TRUE;

    // Get the table and base table.
    $base_table = $this->field['settings']['base_table'];

    // Get the term for the property
    $kvproperty = tripal_get_field_item_keyval($items, $delta, 'value', '');
    $cvterm = chado_generate_var('cvterm', array('name' => $kvproperty));

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
        'cardinality' => FIELD_CARDINALITY_UNLIMITED,
        'locked' => FALSE,
        'storage' => array(
          'type' => 'field_chado_storage',
        ),
        'settings' => array(
          'chado_table' => $prop_table,
          'chado_column' => $pkey,
          'base_table' => $base_table,
          'semantic_web' => $cvterm->dbxref_id->db_id->name . ':' . $cvterm->dbxref_id->accession,
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
        'settings' => array(),
        'widget' => array(
          'type' => $field_name . '_widget',
          'settings' => array(
            'display_label' => 1,
          ),
        ),
        'display' => array(
          'default' => array(
            'label' => 'inline',
            'type' => $field_name . '_formatter',
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

//   $delta = $form_state['triggering_element']['#parents'][2];
//   drupal_debug($form[$field_name]['und'][$delta]);
//   return $form[$field_name]['und'][$delta];
  return print_r(array_keys($form),TRUE);
  return $form[''][$field_name];
}

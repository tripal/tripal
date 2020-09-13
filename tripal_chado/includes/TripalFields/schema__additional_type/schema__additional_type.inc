<?php

class schema__additional_type extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default label for this field.
  public static $default_label = 'Type';

  // The default description for this field.
  public static $description = 'An additional type for this field.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'schema',
    // The name of the term.
    'term_name' => 'additionalType',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'additionalType',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
    // The name of the vocabulary that should be used to provide a list
    // of terms to select form when providing the additional type.
    // This should be the name of the controlled vocabulary from the cv table.
    'vocabulary' => '',
    // If the entire vocabulary is not to be used for the types and if the
    // vocabulary is heirarchical then this will be the parent term.
    // This should be of the format {db.name}:{dbxref.accession}
    'parent_term' => '',
  ];

  // The default widget for this field.
  public static $default_widget = 'schema__additional_type_widget';

  // The default formatter for this field.
  public static $default_formatter = 'schema__additional_type_formatter';

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $this->queryJoinOnce($query, 'cvterm', $alias, "base.$field_column = $alias.cvterm_id");
    $query->condition($alias . '.name', $condition['value'], $operator);
  }

  /**
   * @see ChadoField::query()
   */
  public function queryOrder($query, $order) {
    $alias = $this->field['field_name'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $this->queryJoinOnce($query, 'cvterm', $alias, "base.$field_column = $alias.cvterm_id", "LEFT OUTER");
    $query->orderBy($alias . '.name', $order['direction']);
  }

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {
    $record = $entity->chado_record;
    $base_table = $this->instance['settings']['base_table'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    $linker_field = 'chado-' . $field_table . '__' . $field_column;

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => '',
      $linker_field => '',
    ];

    if ($record->{$field_column}) {
      $entity->{$field_name}['und'][0] = [
        'value' => $record->{$field_column}->name,
        $linker_field => $record->{$field_column}->cvterm_id,
      ];
    }
  }

  /**
   * @see ChadoField::instanceSettingsForm()
   */
  public function instanceSettingsForm() {
    $element = parent::instanceSettingsForm();
    $settings = $this->instance['settings'];

    $element['vocabulary'] = [
      '#title' => t('Enforce Vocabulary'),
      '#type' => 'textfield',
      '#description' => t('Optional. Enter the name of the vocabulary that must be used when the user selects a term for the additional type. If no value is provided then the user can select from any term available on the site.'),
      '#default_value' => $settings['vocabulary'],
      '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cv",
    ];

    $element['parent_term'] = [
      '#title' => t('Enforce Parent Term'),
      '#type' => 'textfield',
      '#description' => t('Optional. If a vocabulary is heirarchical and if the user should be restricted to a set of terms below a given parent term, then enter the parent term here. The term should be in the form {db}:{accession} where {db} is the short name for the database (e.g. GO, SO, TPUB, TContact) and {accession} is the unique identifier for that term in the vocabulary (e.g. TContact:0000001).'),
      '#default_value' => $settings['parent_term'],
    ];

    return $element;
  }

  /**
   * @see TripalField::settingsFormValidate()
   */
  public function settingsFormValidate($form, &$form_state) {
    $element = parent::settingsFormValidate($form, $form_state);

    $values = $form_state['values']['instance']['settings'];

    // Make sure te vocabulary and parent term exist
    $vocabulary = $values['vocabulary'];
    $parent_term = $values['parent_term'];

    $good_vocab = FALSE;
    if ($vocabulary) {
      $sql = "SELECT cv_id FROM {cv} WHERE name = :name";
      $results = chado_query($sql, [':name' => $vocabulary]);
      $cv_id = $results->fetchField();
      if (!$cv_id) {
        form_set_error('instance][settings][vocabulary', 'Invalid vocabulary');
      }
      else {
        $good_vocab = TRUE;
      }
    }

    if ($parent_term and $good_vocab) {
      $matches = [];
      if (preg_match('/^(.+):(.+)$/', $parent_term, $matches)) {
        $db = $matches[1];
        $accession = $matches[2];
        $values = [
          'db_id' => [
            'name' => $db,
          ],
          'accession' => $accession,
        ];
        $records = chado_select_record('dbxref', ['dbxref_id'], $values);
        if (count($records) == 0) {
          form_set_error('instance][settings][parent_term', 'The specified parent term does not exist on this site. Please check the entry.');
        }
      }
      else {
        form_set_error('instance][settings][parent_term', 'Please provide a parent term in the correct format.');
      }
    }
  }
}
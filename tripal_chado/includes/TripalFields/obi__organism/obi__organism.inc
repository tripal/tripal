<?php

class obi__organism extends ChadoField {

  // The default label for this field.
  public static $default_label = 'Organism';

  // The default description for this field.
  public static $description = 'The organism to which this resource is associated.';

  // Provide a list of instance specific settings. These can be accessed within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal will automatically change these settings for the instance.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = [
    // The short name for the vocabulary (e.g. schema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'OBI',
    // The name of the term.
    'term_name' => 'organism',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0100026',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
    // The format for display of the organism.
    'field_display_string' => '<i>[organism.genus] [organism.species]</i>',
  ];

  // The default widget for this field.
  public static $default_widget = 'obi__organism_widget';

  // The default formatter for this field.
  public static $default_formatter = 'obi__organism_formatter';


  /**
   * @see TripalField::validate()
   */
  public function validate($entity_type, $entity, $langcode, $items, &$errors) {

    // If we don't have an entity then we don't want to validate.  The case
    // where this could happen is when a user is editing the field settings
    // and trying to set a default value. In that case there's no entity and
    // we don't want to validate.  There will always be an entity for creation
    // and update operations of a content type.
    if (!$entity) {
      return;
    }
    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Set the linker field appropriately.
    if ($field_table == 'biomaterial') {
      $linker_field = 'chado-biomaterial__taxon_id';
    }
    else {
      $linker_field = 'chado-' . $field_table . '__organism_id';
    }

    // Get the field values.
    foreach ($items as $delta => $values) {

      // Get the field values.
      $organism_id = $values[$linker_field];
      if ((!$organism_id or $organism_id == 0) and !$field_table == 'biomaterial') {
        $errors[$field_name]['und'][0][] = [
          'message' => t("Please specify an organism."),
          'error' => 'obi__organism_id',
        ];
      }
    }
  }

  /**
   * @see TripalField::load()
   */
  public function load($entity) {

    $record = $entity->chado_record;
    $settings = $this->instance['settings'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Get the terms for each of the keys for the 'values' property.
    $label_term = 'rdfs:label';
    $genus_term = chado_get_semweb_term('organism', 'genus');
    $species_term = chado_get_semweb_term('organism', 'species');
    $infraspecific_name_term = chado_get_semweb_term('organism', 'infraspecific_name');
    $infraspecific_type_term = chado_get_semweb_term('organism', 'type_id');


    // Set the linker field appropriately.
    if ($field_table == 'biomaterial') {
      $linker_field = 'chado-biomaterial__taxon_id';
    }
    else {
      $linker_field = 'chado-' . $field_table . '__organism_id';
    }

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = [
      'value' => [],
    ];

    if ($record) {
      if ($field_table == 'biomaterial') {
        $organism = $record->taxon_id;
      }
      else {
        $organism = $record->organism_id;
      }

      if (!$organism) {
        return;
      }
      $string = $settings['field_display_string'];
      $label = chado_replace_tokens($string, $organism);
      $entity->{$field_name}['und'][0]['value'] = [
        $label_term => $label,
        $genus_term => $organism->genus,
        $species_term => $organism->species,
      ];
      // The infraspecific fields were introduced in Chado v1.3.
      if (property_exists($organism, 'infraspecific_name')) {
        $entity->{$field_name}['und'][0]['value'][$infraspecific_type_term] = NULL;
        $entity->{$field_name}['und'][0]['value'][$infraspecific_name_term] = $organism->infraspecific_name;
        if ($organism->type_id) {
          $entity->{$field_name}['und'][0]['value'][$infraspecific_type_term] = $organism->type_id->name;
        }
      }
      $entity->{$field_name}['und'][0][$linker_field] = $organism->organism_id;

      // Is there a published entity for this organism?
      if (property_exists($record->{$field_column}, 'entity_id')) {
        $entity->{$field_name}['und'][0]['value']['entity'] = 'TripalEntity:' . $record->{$field_column}->entity_id;
      }
    }
  }

  /**
   * @see TripalField::globalSettingsForm()
   */
  public function instanceSettingsForm() {
    $element = parent::instanceSettingsForm();

    $settings = $this->instance['settings'];
    $element['instructions'] = [
      '#type' => 'item',
      '#markup' => 'You may rewrite the way this field is presented to the end-user.
        The Rewrite Value field allows you to use tokens to indicate how the
        value should be displayed.  Tokens will be substituted with appriorate
        data from the database.  See the Available tokens list for the
        tokens you may use.',
    ];

    $element['field_display_string'] = [
      '#type' => 'textfield',
      '#title' => 'Rewrite Value',
      '#description' => t('Provide a mixture of text and/or tokens for the format.
          For example: [organism.genus] [organism.species].  When displayed,
          the tokens will be replaced with the actual value.'),
      '#default_value' => $settings['field_display_string'],
    ];

    $element['tokens'] = [
      '#type' => 'fieldset',
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
      '#title' => 'Available Tokens',
    ];
    $headers = ['Token', 'Description'];
    $rows = [];

    // Here we use the chado_get_tokens rather than the
    // tripal_get_entity_tokens because we can't gurantee that all organisms
    // have entities.
    $tokens = chado_get_tokens('organism');
    foreach ($tokens as $token) {
      $rows[] = [
        $token['token'],
        $token['description'],
      ];
    }

    $table_vars = [
      'header' => $headers,
      'rows' => $rows,
      'attributes' => [],
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => [],
      'empty' => 'There are no tokens',
    ];
    $element['tokens']['list'] = [
      '#type' => 'item',
      '#markup' => theme_table($table_vars),
    ];

    return $element;
  }

  /**
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();

    $genus_term = chado_get_semweb_term('organism', 'genus');
    $species_term = chado_get_semweb_term('organism', 'species');
    $infraspecific_name_term = chado_get_semweb_term('organism', 'infraspecific_name');
    $infraspecific_type_term = chado_get_semweb_term('organism', 'type_id');

    return [
      $field_term => [
        'operations' => ['eq', 'contains', 'starts'],
        'sortable' => TRUE,
        'searchable' => TRUE,
        'readonly' => FALSE,
        'type' => 'xs:complexType',
        'elements' => [
          'rdfs:label' => [
            'searchable' => TRUE,
            'name' => 'scientific_name',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => FALSE,
            'type' => 'xs:string',
            'readonly' => TRUE,
            'required' => FALSE,
          ],
          $genus_term => [
            'searchable' => TRUE,
            'name' => 'genus',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'readonly' => FALSE,
            'type' => 'xs:string',
            'required' => TRUE,
          ],
          $species_term => [
            'searchable' => TRUE,
            'name' => 'species',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'readonly' => FALSE,
            'type' => 'xs:string',
            'required' => TRUE,
          ],
          $infraspecific_name_term => [
            'searchable' => TRUE,
            'name' => 'infraspecies',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'readonly' => FALSE,
            'type' => 'xs:string',
            'required' => FALSE,
          ],
          $infraspecific_type_term => [
            'searchable' => TRUE,
            'name' => 'infraspecific_type',
            'operations' => ['eq', 'ne', 'contains', 'starts'],
            'sortable' => TRUE,
            'readonly' => FALSE,
            'type' => 'xs:integer',
            'required' => FALSE,
          ],
          'entity' => [
            'searchable' => FALSE,
          ],
        ],
      ],
    ];
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    $field_term_id = $this->getFieldTermID();
    $genus_term = $field_term_id . ',' . chado_get_semweb_term('organism', 'genus');
    $species_term = $field_term_id . ',' . chado_get_semweb_term('organism', 'species');
    $infraspecific_name_term = $field_term_id . ',' . chado_get_semweb_term('organism', 'infraspecific_name');
    $infraspecific_type_term = $field_term_id . ',' . chado_get_semweb_term('organism', 'type_id');

    // Join to the organism table for this field.
    $this->queryJoinOnce($query, 'organism', $alias, "base.organism_id = $alias.organism_id");

    // If the column is the field name then we're during a search on the full
    // scientific name.
    if ($condition['column'] == $field_term_id or
      $condition['column'] == $field_term_id . ',rdfs:label') {
      if (chado_get_version() <= 1.3) {
        $query->where("CONCAT($alias.genus, ' ', $alias.species) $operator :full_name", [':full_name' => $condition['value']]);
      }
      else {
        $this->queryJoinOnce($query, 'cvterm', $alias . '_cvterm', 'base.infraspecific_type = ' . $alias . '_cvterm.type_id', 'LEFT OUTER');
        $query->where("CONCAT($alias.genus, ' ', $alias.species, ' ', " . $alias . "'_cvterm.name', ' ', $alias.infraspecific_name) $operator :full_name", [':full_name' => $condition['value']]);
      }
    }

    // If the column is a subfield.
    if ($condition['column'] == $species_term) {
      $query->condition("$alias.species", $condition['value'], $operator);
    }
    if ($condition['column'] == $genus_term) {
      $query->condition("$alias.genus", $condition['value'], $operator);
    }
    if ($condition['column'] == $infraspecific_name_term) {
      $query->condition("$alias.infraspecific_name", $condition['value'], $operator);
    }
    if ($condition['column'] == $infraspecific_type_term) {
      $this->queryJoinOnce($query, 'cvterm', 'CVT', "base.type_id = CVT.cvterm_id");
      $query->condition("CVT.name", $condition['value'], $operator);
    }
  }

  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {
    $alias = $this->field['field_name'];

    $field_term_id = $this->getFieldTermID();
    $genus_term = $field_term_id . ',' . chado_get_semweb_term('organism', 'genus');
    $species_term = $field_term_id . ',' . chado_get_semweb_term('organism', 'species');
    $infraspecific_name_term = $field_term_id . ',' . chado_get_semweb_term('organism', 'infraspecific_name');
    $infraspecific_type_term = $field_term_id . ',' . chado_get_semweb_term('organism', 'type_id');

    // Join to the organism table for this field.
    $this->queryJoinOnce($query, 'organism', $alias, "base.organism_id = $alias.organism_id");

    // Now perform the sort.
    if ($order['column'] == $species_term) {
      $query->orderBy("$alias.species", $order['direction']);
    }
    if ($order['column'] == $genus_term) {
      $query->orderBy("$alias.genus", $order['direction']);
    }
    if ($order['column'] == $infraspecific_name_term) {
      $query->orderBy("$alias.infraspecific_name", $order['direction']);
    }
    if ($order['column'] == $infraspecific_type_term) {
      if (!in_array('CVT', $joins)) {
        $this->queryJoinOnce($query, 'cvterm', 'CVT', "base.type_id = CVT.cvterm_id");
      }
      $query->orderBy("CVT.name", $order['direction']);
    }
  }
}

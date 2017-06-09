<?php

class obi__organism extends ChadoField {

  // The default lable for this field.
  public static $default_label = 'Organism';

  // The default description for this field.
  public static $description = 'The organism to which this resource is associated.';

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
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
  );

  // In order for this field to integrate with Drupal Views, a set of
  // handlers must be specififed.  These include handlers for
  // the field, for the filter, and the sort.  Within this variable,
  // the key must be one of: field, filter, sort; and the value
  // is the settings for those handlers as would be provided by
  // a hook_views_data().  The following defaults make a field visible
  // using the default formatter of the field, allow for filtering using
  // a string value and sortable.  in order for filters to work you
  // must implement the query() function.
  public static $default_view_handlers = array(
    'field' => array(
      'handler' => 'tripal_views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'tripal_views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // The default widget for this field.
  public static $default_widget = 'OBI__organism_widget';

  // The default formatter for this field.
  public static $default_formatter = 'OBI__organism_formatter';


  /**
   * @see TripalField::validate()
   */
  public function validate($entity_type, $entity, $field, $items, &$errors) {

    $settings = $this->field['settings'];
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];

    // Get the field values.
    foreach ($items as $delta => $values) {

      // Get the field values.
      $organism_id = $values['chado-' . $field_table . '__organism_id'];
      if (!$organism_id or $organism_id == 0) {
        $errors[$field_name]['und'][0][] = array(
          'message' =>  t("Please specify an organism."),
          'error' => 'chado_base__organism_id'
        );
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
    $genus_term = tripal_get_chado_semweb_term('organism', 'genus');
    $species_term = tripal_get_chado_semweb_term('organism', 'species');
    $infraspecific_name_term = tripal_get_chado_semweb_term('organism', 'infraspecific_name');
    $infraspecific_type_term = tripal_get_chado_semweb_term('organism', 'type_id');

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(
        /*
        // Types of elements that will appear in the value array.
        $label_term => '',
        $genus_term => '',
        $species_term => '',
        */
      ),
    );

    if ($record) {
      $organism = $record->organism_id;
      $string = $settings['field_display_string'];
      $label = tripal_replace_chado_tokens($string, $organism);
      $entity->{$field_name}['und'][0]['value'] = array(
        $label_term => $label,
        $genus_term => $organism->genus,
        $species_term => $organism->species,
      );
      // The infraspecific fiels were introdcued in Chado v1.3.
      if (property_exists($organism, 'infraspecific_name')) {
        $entity->{$field_name}['und'][0]['value'][$infraspecific_type_term] = NULL;
        $entity->{$field_name}['und'][0]['value'][$infraspecific_name_term] = $organism->infraspecific_name;
        if ($organism->type_id) {
          $entity->{$field_name}['und'][0]['value'][$infraspecific_type_term] =  $organism->type_id->name;
        }
      }
      $entity->{$field_name}['und'][0]['chado-' . $field_table . '__organism_id'] = $organism->organism_id;

      // Is there a published entity for this organism?
      if (property_exists($record->{$field_column}, 'entity_id')) {
        $entity->{$field_name}['und'][0]['value']['entity'] = 'TripalEntity:' . $record->{$field_column}->entity_id;
      }
    }
  }

  /**
   * @see TripalField::globalSettingsForm()
   */
  public function settingsForm($has_data) {
    $element = parent::globalSettingsForm($has_data);

    $settings = $this->field['settings'];

    $element['instructions'] = array(
      '#type' => 'item',
      '#markup' => 'You may rewrite the way this field is presented to the end-user.
        The Rewrite Value field allows you to use tokens to indicate how the
        value should be displayed.  Tokens will be substituted with appriorate
        data from the database.  See the Available tokens list for the
        tokens you may use.'
    );

    $element['field_display_string'] = array(
      '#type' => 'textfield',
      '#title' => 'Rewrite Value',
      '#description' => t('Provide a mixture of text and/or tokens for the format.
          For example: [organism.genus] [organism.species].  When displayed,
          the tokens will be replaced with the actual value.'),
      '#default_value' => $settings['field_display_string'],
    );

    $element['tokens'] = array(
      '#type' => 'fieldset',
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
      '#title' => 'Available Tokens'
    );
    $headers = array('Token', 'Description');
    $rows = array();

    // Here we use the tripal_get_chado_tokens rather than the
    // tripal_get_entity_tokens because we can't gurantee that all organisms
    // have entities.
    $tokens = tripal_get_chado_tokens('organism');
    foreach ($tokens as $token) {
      $rows[] = array(
        $token['token'],
        $token['description'],
      );
    }

    $table_vars = array(
      'header'     => $headers,
      'rows'       => $rows,
      'attributes' => array(),
      'sticky'     => FALSE,
      'caption'    => '',
      'colgroups'  => array(),
      'empty'      => 'There are no tokens',
    );
    $element['tokens']['list'] = array(
      '#type' => 'item',
      '#markup' => theme_table($table_vars),
    );

    // Add in the semantic web fields.
    $parent_elements = parent::settings_form($field, $instance, $has_data);
    $element = array_merge($element, $parent_elements);

    return $element;
  }

  /**
   * @see TripalField::elementInfo()
   */
  public function elementInfo() {
    $field_term = $this->getFieldTermID();

    $genus_term = tripal_get_chado_semweb_term('organism', 'genus');
    $species_term = tripal_get_chado_semweb_term('organism', 'species');
    $infraspecific_name_term = tripal_get_chado_semweb_term('organism', 'infraspecific_name');
    $infraspecific_type_term = tripal_get_chado_semweb_term('organism', 'type_id');

    return array(
      $field_term => array(
        'operations' => array('eq', 'contains', 'starts'),
        'sortable' => TRUE,
        'searchable' => TRUE,
        'type' => 'string',
        'elements' => array(
          'rdfs:label' => array(
            'searchable' => TRUE,
            'name' => 'scientfic_name',
            'operations' => array('eq', 'ne', 'contains', 'starts'),
            'sortable' => TRUE,
          ),
          $genus_term => array(
            'searchable' => TRUE,
            'name' => 'genus',
            'operations' => array('eq', 'ne', 'contains', 'starts'),
            'sortable' => TRUE,
          ),
          $species_term => array(
            'searchable' => TRUE,
            'name' => 'species',
            'operations' => array('eq', 'ne', 'contains', 'starts'),
            'sortable' => TRUE,
          ),
          $infraspecific_name_term => array(
            'searchable' => TRUE,
            'name' => 'infraspecies',
            'operations' => array('eq', 'ne', 'contains', 'starts'),
            'sortable' => TRUE,
          ),
          $infraspecific_type_term => array(
            'searchable' => TRUE,
            'name' => 'infraspecific_type',
            'operations' => array('eq', 'ne', 'contains', 'starts'),
            'sortable' => TRUE,
          ),
          'entity' => array(
            'searchable' => FALSE,
          ),
        ),
      )
    );
  }

  /**
   * @see ChadoField::query()
   */
  public function query($query, $condition) {
    $alias = $this->field['field_name'];
    $operator = $condition['operator'];

    dpm($condition);

    $field_term_id = $this->getFieldTermID();
    $genus_term = $field_term_id . ',' . tripal_get_chado_semweb_term('organism', 'genus');
    $species_term = $field_term_id . ',' . tripal_get_chado_semweb_term('organism', 'species');
    $infraspecific_name_term = $field_term_id . ',' . tripal_get_chado_semweb_term('organism', 'infraspecific_name');
    $infraspecific_type_term = $field_term_id . ',' . tripal_get_chado_semweb_term('organism', 'type_id');
    $query->join('organism', $alias, "base.organism_id = $alias.organism_id");

    // If the column is the field name then we're during a search on the full
    // scientific name.
    if ($condition['column'] == $field_term_id or
        $condition['column'] == $field_term_id . ',rdfs:label') {
      if (chado_get_version() <= 1.3) {
        $query->where("CONCAT($alias.genus, ' ', $alias.species) $operator :full_name",  array(':full_name' => $condition['value']));
      }
      else {
        $query->leftJoin('cvterm', $alias . '_cvterm', 'base.infraspecific_type = ' . $alias . '_cvterm.type_id');
        $query->where("CONCAT($alias.genus, ' ', $alias.species, ' ', " . $alias . "'_cvterm.name', ' ', $alias.infraspecific_name) $operator :full_name",  array(':full_name' => $condition['value']));
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
      $query->join('cvterm', 'CVT', "base.type_id = CVT.cvterm_id");
      $query->condition("CVT.name", $condition['value'], $operator);
    }
  }

  /**
   * @see ChadoField::queryOrder()
   */
  public function queryOrder($query, $order) {

    // If the table hasn't yet been joined then add it.
    $joins = $query->getTables();
    if (!in_array($this->field['field_name'], $joins)) {
      $alias = $this->field['field_name'];
      $query->join('organism', $alias, "base.organism_id = $alias.organism_id");
    }

    // Now perform the sort.
    if ($order['column'] == 'organism.species') {
      $query->orderBy("$alias.genus", $order['direction']);
    }
    if ($order['column'] == 'organism.genus') {
      $query->orderBy("$alias.species", $order['direction']);
    }
    if ($order['column'] == 'organism.infraspecies') {
      $query->orderBy("$alias.infraspecific_name", $order['direction']);
    }
    if ($order['column'] == 'organism.infraspecies') {
      if (!in_array('CVT', $joins)) {
        $query->join('cvterm', 'CVT', "base.type_id = CVT.cvterm_id");
      }
      $query->orderBy("CVT.name", $order['direction']);
    }
  }
}

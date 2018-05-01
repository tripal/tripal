<?php
/**
 * @file
 * Contains views filter handlers that provide chado support.
 */

/**
 * Adds support for chado boolean filters.
 */
class chado_views_handler_filter_boolean extends views_handler_filter_boolean_operator {

  /**
   * {@inheritdoc}
   */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);

    // Change the keys of the options so that this filter can be optional.
    $form['value']['#options']['f'] = $form['value']['#options'][0];
    $form['value']['#options']['t'] = $form['value']['#options'][1];
    unset($form['value']['#options'][0], $form['value']['#options'][1]);

  }

  /**
   * {@inheritdoc}
   */
  function query() {

    // Adds joins to chado entity and the chado table this field is from.
    $alias = _chado_views_add_table_joins($this);

    // Booleans in chado are stored t/f.
    $field = $alias . '.' . $this->definition['chado_field'];
    if ($this->value) {
      $this->query->add_where($this->options['group'], $field, $this->value, '=');
    }
  }
}

/**
 * Adds support for chado datetime filters.
 */
class chado_views_handler_filter_date extends views_handler_filter_date {

  /**
   * {@inheritdoc}
   */
  function query() {

    // Adds joins to chado entity and the chado table this field is from.
    $alias = _chado_views_add_table_joins($this);

    // Then allow the parent handler to add the where.
    $field = $alias . '.' . $this->definition['chado_field'];
    $info = $this->operators();
    if (!empty($info[$this->operator]['method'])) {
      $this->{$info[$this->operator]['method']}($field);
    }
  }

  /**
   * {@inheritdoc}
   */
  function op_between($field) {
    // Use the substitutions to ensure a consistent timestamp.
    $query_substitutions = views_views_query_substitutions($this->view);
    $a = intval(strtotime($this->value['min'], $query_substitutions['***CURRENT_TIME***']));
    $b = intval(strtotime($this->value['max'], $query_substitutions['***CURRENT_TIME***']));

    // This is safe because we are manually scrubbing the values.
    // It is necessary to do it this way because $a and $b are formulas when using an offset.
    $operator = strtoupper($this->operator);
    $this->query->add_where_expression($this->options['group'], "$field $operator to_timestamp($a) AND to_timestamp($b)");
  }

  /**
   * {@inheritdoc}
   */
  function op_simple($field) {
    // Use the substitutions to ensure a consistent timestamp.
    $query_substitutions = views_views_query_substitutions($this->view);
    $value = intval(strtotime($this->value['value'], $query_substitutions['***CURRENT_TIME***']));

    // This is safe because we are manually scrubbing the value.
    // It is necessary to do it this way because $value is a formula when using an offset.
    $this->query->add_where_expression($this->options['group'], "$field $this->operator to_timestamp($value)");
  }

}

/**
 * Adds support for chado foreign key filters.
 */
class chado_views_handler_filter_fk extends views_handler_filter {

  /**
   * {@inheritdoc}
   */
  function query() {

    // Adds joins to chado entity and the chado table this field is from.
    $alias = _chado_views_add_table_joins($this);

    // We need to do a quick fix for multiple values selected.
    if (is_array($this->value[0]) AND sizeof($this->value) == 1) {
      $this->value = $this->value[0];
    }

    // Now add the restriction to the chado table as specified by user input.
    if (sizeof($this->value) == 1) {
      $value = current($this->value);
      $field = $alias . '.' . $this->definition['chado_field'];
      $this->query->add_where($this->options['group'], $field, $value, '=');
    }
    elseif (sizeof($this->value) > 1) {
      $field = $alias . '.' . $this->definition['chado_field'];
      $this->query->add_where($this->options['group'], $field, $this->value, 'IN');
    }


  }

  /**
   * {@inheritdoc}
   */
  function value_form(&$form, &$form_state) {
    parent::value_form($form, $form_state);

    $this->options['values_form_type'] = (isset($this->options['values_form_type'])) ? $this->options['values_form_type'] : 'textfield';

    if (preg_match('/select/', $this->options['values_form_type'])) {

      //Select List
      $form['value'] = [
        '#type' => 'select',
        '#title' => t('%label', ['%label' => $this->options['expose']['label']]),
        '#options' => $this->get_select_options(),
        '#default_value' => $this->value,
      ];

      if ($this->options['select_multiple']) {
        $form['value']['#multiple'] = TRUE;
      }
    }
    else {

      $form['value'] = [
        '#type' => 'textfield',
        '#title' => t('%label', ['%label' => $this->options['expose']['label']]),
        '#default_value' => $this->value,
      ];

    }
  }

  /**
   * {@inheritdoc}
   */
  function exposed_form(&$form, &$form_state) {
    parent::exposed_form($form, $form_state);

    if (isset($this->options['select_multiple'])) {
      if ($this->options['select_multiple']) {

        if (isset($this->options['expose']['identifier'])) {
          $id = $this->options['expose']['identifier'];
        }
        else {
          $id = $this->options['id'];
        }
        $form[$id]['#multiple'] = TRUE;
      }
    }
  }

  function get_select_options() {

    if (isset($this->options['display_column'])) {
      $name_field = $this->options['display_column'];
    }
    else {
      $name_field = 'name';
    }

    // If the admin has selected to show all the values then just select all
    // records from the table referenced by the foreign key.
    if ($this->options['show_all']) {
      $sql = 'SELECT !id_field as id, !name_field as name
                FROM {!foreign_table}';
      $sql = format_string($sql, [
        '!foreign_table' => $this->definition['foreign_key']['right_table'],
        '!id_field' => $this->definition['chado_field'],
        '!name_field' => $name_field,
      ]);
    }
    // Otherwise, only return the values from the foreign table that were referenced
    // in the foreign key column.
    else {
      // Using a "Loose Index Scan" to get a list of all the unique values for
      // the name in the table referenced by the foreign key constraint.
      // See https://wiki.postgresql.org/wiki/Loose_indexscan
      $sql = "WITH RECURSIVE t AS (
            SELECT MIN(filter_table.!id_field) AS col
              FROM {!filter_table} filter_table
              LEFT JOIN {!foreign_table} foreign_table ON filter_table.!id_field=foreign_table.!id_field
            UNION ALL
            SELECT (
                SELECT MIN(filter_table.!id_field)
                FROM {!filter_table} filter_table
                LEFT JOIN {!foreign_table} foreign_table ON filter_table.!id_field=foreign_table.!id_field
                WHERE filter_table.!id_field > col
              )
              FROM t WHERE col IS NOT NULL
          )
          SELECT !id_field as id, !name_field as name
            FROM {!foreign_table}
            WHERE !id_field IN (SELECT col FROM t where col IS NOT NULL)
            ORDER BY !name_field ASC";
      $sql = format_string($sql, [
        '!filter_table' => $this->definition['chado_table'],
        '!foreign_table' => $this->definition['foreign_key']['right_table'],
        '!id_field' => $this->definition['chado_field'],
        '!name_field' => $name_field,
      ]);
    }

    $resource = chado_query($sql);
    $options = [];

    if ($this->options['select_optional']) {
      $options['All'] = '- Any -';
    }

    foreach ($resource as $r) {
      $options[$r->id] = $r->name;
    }

    return $options;
  }

  /**
   * {@inheritdoc}
   */
  function has_extra_options() {
    return TRUE;
  }

  /**
   * {@inheritdoc}
   */
  function extra_options_form(&$form, &$form_state) {
    parent::extra_options_form($form, $form_state);

    $form['values_form_type'] = [
      '#type' => 'radios',
      '#title' => t('Filter Type'),
      '#options' => [
        'textfield' => 'Text Field',
        'select' => 'Drop-Down Box',
      ],
      '#default_value' => $this->options['values_form_type'],
    ];

    $form['show_all'] = [
      '#type' => 'checkbox',
      '#title' => t('Show All'),
      '#description' => t('When selected all records from the parent table will be shown in the drop-down rather than just those used in the current table.'),
      '#default_value' => $this->options['show_all'],
    ];

    $form['select_multiple'] = [
      '#type' => 'checkbox',
      '#title' => t('Select Multiple'),
      '#description' => t('Allows more then one option to be selected.'),
      '#default_value' => $this->options['select_multiple'],
    ];

    $form['select_optional'] = [
      '#type' => 'checkbox',
      '#title' => t('Optional'),
      '#description' => t('Adds --Any-- to the available options.'),
      '#default_value' => $this->options['select_optional'],
    ];

    // Allow the user to select which column to display to the user.
    // For example, if the foreign key points to the organism table you might want to
    // display the abbreviation or common name to the user.
    $table_defn = chado_get_schema($this->definition['foreign_key']['right_table']);
    $options = [];
    foreach ($table_defn['fields'] as $name => $details) {
      $options[$name] = $name;
    }
    $form['display_column'] = [
      '#type' => 'select',
      '#title' => 'Display Column',
      '#description' => t('Specify which column from the parent table you would like to
        display to the user.'),
      '#options' => $options,
      '#default_value' => $this->options['display_column'],
    ];

    $form['max_length'] = [
      '#type' => 'textfield',
      '#title' => t('Max Width'),
      '#description' => t('Specify the maximum width of the select box'),
      '#default_value' => $this->options['max_length'],
    ];

    return $form;

  }

  /**
   * {@inheritdoc}
   */
  function extra_options_submit($form, &$form_state) {
    $this->options['values_form_type'] = $form_state['values']['options']['values_form_type'];
    $this->options['select_multiple'] = $form_state['values']['options']['select_multiple'];
    $this->options['select_optional'] = $form_state['values']['options']['select_optional'];
    $this->options['display_column'] = $form_state['values']['options']['display_column'];
    $this->options['max_length'] = $form_state['values']['options']['max_length'];
    $this->options['show_all'] = $form_state['values']['options']['show_all'];
  }

  /**
   * {@inheritdoc}
   */
  function admin_summary() {
    if (is_array($this->value)) {
      return check_plain((string) $this->operator) . ' ' . check_plain((string) implode(',', $this->value));
    }
    else {
      return check_plain((string) $this->operator) . ' ' . check_plain((string) $this->value);
    }
  }

  /**
   * {@inheritdoc}
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['values_form_type'] = [
      'default' => 'textfield',
      'export' => TRUE,
    ];
    $options['select_multiple'] = [
      'default' => FALSE,
      'bool' => TRUE,
      'export' => TRUE,
    ];
    $options['select_optional'] = [
      'default' => FALSE,
      'bool' => TRUE,
      'export' => TRUE,
    ];
    $options['show_all'] = [
      'default' => FALSE,
      'bool' => TRUE,
      'export' => TRUE,
    ];

    // display column.
    $table_defn = chado_get_schema($this->definition['foreign_key']['right_table']);
    $fields = array_keys($table_defn['fields']);
    $name_fields = preg_grep('/name/', $fields);
    if ($name_fields) {
      $default = array_pop($name_fields);
    }
    else {
      $default = array_pop($fields);
    }
    $options['display_column'] = [
      'export' => TRUE,
      'default' => $default,
    ];
    $options['max_length'] = [
      'default' => 40,
      'export' => TRUE,
    ];

    return $options;
  }
}

/**
 * Adds chado support to the string filter.
 */
class chado_views_handler_filter_string extends views_handler_filter_string {

  function query() {

    // Adds joins to chado entity and the chado table this field is from.
    $alias = _chado_views_add_table_joins($this);

    // Finally add the restriction on the chado table including the value entered by the filter.
    // Notice that we don't call $query->add_where directly. This is so that the options
    // specified for the comparison operator are used (ie: contains).
    $info = $this->operators();
    $field = $alias . '.' . $this->definition['chado_field'];
    if (!empty($info[$this->operator]['method'])) {
      $this->{$info[$this->operator]['method']}($field);
    }
  }
}

/**
 * A helper function for adding joins in the views_handler::query() method
 * to connect a TripalEntity with it's chado tables.
 *
 * @param object $views_handler
 *   $this from within the views_handler::query() method.
 */
function _chado_views_add_table_joins(&$handler) {

  // First we need to join to the chado entity table where the link between an
  // entity and it's chado record is stored.
  $join = new views_join();
  $join->construct('chado_entity', $handler->table, 'id', 'chado_entity_id');
  $alias = $handler->query->add_relationship('chado_entity', $join, $handler->table_alias, $handler->relationship);

  // Restrict the chado entity join to only return the table this field is from.
  $handler->query->add_where(0, $alias . '.data_table', $handler->definition['chado_table'], '=');

  // Now, we need to join from chado entity to the chado table needed by this field.
  // This only works if the field is from the base table.
  // @todo: fix handler to work with non-base tables.
  $join = new views_join();
  $chado_table = 'chado.' . $handler->definition['chado_table'];
  $join->construct($chado_table, 'chado_entity', 'record_id', $handler->definition['chado_table'] . '_id');
  $alias = $handler->query->add_relationship('chado_' . $handler->definition['chado_table'], $join, $handler->table_alias, $handler->relationship);

  return $alias;
}

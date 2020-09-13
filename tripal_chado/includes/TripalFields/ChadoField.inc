<?php

class ChadoField extends TripalField {

  // The default label for this field.
  public static $default_label = 'Chado Field';

  // The default description for this field.
  public static $default_description = 'The generic base class for all Chado fields. Replace this text as appropriate for the child implementation.';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = [
    'storage' => 'field_chado_storage',
  ];

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
    'term_name' => 'Thing',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'Thing',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => '',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => '',
    // The base table.
    'base_table' => '',
  ];

  // Indicates the download formats for this field.  The list must be the
  // name of a child class of the TripalFieldDownloader.
  public static $download_formatters = [
    'TripalTabDownloader',
    'TripalCSVDownloader',
  ];

  // The module that manages this field.
  public static $module = 'tripal_chado';

  /**
   * @see TripalField::query()
   *
   * In addition to the rules to follow for the TripalField::query function
   * these should also be followed for the ChadoField::query implementation.
   *
   * - When giving alias to joined tables be sure to use aliases that are
   *   unique to avoid conflicts with other fields.
   * - When joining with the base table its alias is 'base'.
   * - You may join to materialized views if need be to help speed queries.
   */
  public function query($query, $condition) {
    // If we are here it is because the child class did not implement the
    // query function.  So, we will do our best to make the query work.
    $chado_table = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];
    $bschema = chado_get_schema($base_table);
    $bpkey = $bschema['primary key'][0];
    $alias = 'dbx_linker';
    $operator = $condition['operator'];

    // If the chado_table and the base_table are the same then this is easy.
    if ($chado_table == $base_table) {
      // Get the base table column that is associated with the term
      // passed as $condition['column'].
      $base_field = chado_get_semweb_column($chado_table, $condition['column']);
      $query->condition('base.' . $base_field, $condition['value'], $operator);
    }
    else {
      // If the two are not the same then we expect that the child class
      // will implement a query() function.
    }
  }

  /**
   * @see TripalField::queryOrder()
   */
  public function queryOrder($query, $order) {


    // If we are here it is because the child class did not implement the
    // queryOrder function.  So, we will do our best to make the query work.
    $chado_table = $this->instance['settings']['chado_table'];
    $base_table = $this->instance['settings']['base_table'];
    $bschema = chado_get_schema($base_table);
    $bpkey = $bschema['primary key'][0];
    $alias = 'dbx_linker';
    $operator = $condition['operator'];

    // If the chado_table and the base_table are the same then this is easy.
    if ($chado_table == $base_table) {
      // Get the base table column that is associated with the term
      // passed as $condition['column'].
      $base_field = chado_get_semweb_column($chado_table, $order['column']);
      $query->orderBy('base.' . $base_field, $order['direction']);
    }
    else {
      // If the two are not the same then we expect that the child class
      // will implement a query() function.
    }
  }

  /**
   * A convient way to join a table to a query without duplicates.
   *
   * @param $query
   *   The SelectQuery object.
   * @param $table
   *   The table to join.
   * @param $alias
   *   The table alias to use.
   * @param $condition
   *   The join condition.
   * @param $type
   *   The type of join: INNER, LEFT OUTER, or RIGHT OUTER.
   */
  protected function queryJoinOnce($query, $table, $alias, $condition, $type = 'INNER') {
    $joins = $query->getTables();

    // If this join is already present then don't add it again.
    if (in_array($alias, array_keys($joins))) {
      return;
    }

    switch ($type) {
      case 'LEFT OUTER':
        $query->leftjoin($table, $alias, $condition);
        break;
      case 'RIGHT OUTER':
        $query->rightjoin($table, $alias, $condition);
        break;
      default:
        $query->innerjoin($table, $alias, $condition);
    }
  }

  /**
   * Used to retrieve a distinct list of values already used for the current
   * field instance.
   *
   * @param $keyword
   *   A string option used to filter the distinct list. This is used when
   *   creating an autocomplete. For all distinct values, set this to NULL.
   * @param $options
   *   An array where options for how to generate this list can be specified.
   *   Supported options include:
   *     - limit: how many results to limit to (Default: 25)
   *     - label_string: a string with tokens that should be used to generate
   *   the human-readable values in the returned list.
   *
   * The following example shows you how to pull all the value list for a
   *   specific instance of a field.
   *
   * @code
   * // In this example we want the values for the obi__organism field
   * // attached to the Tripal Content Type with a machine name of bio_data_17:
   * $field_name = 'obi__organism';
   * $bundle_name = 'bio_data_17';
   *
   * // The following two calls get information about the field we want the
   *   values for.
   * $field_info = field_info_field($field_name);
   * $instance_info = field_info_instance('TripalEntity', $field_name,
   *   $bundle_name);
   * // Construct the Field instance we want the values for.
   * $instance = new ChadoField($field_info, $instance_info);
   *
   * // Retrieve the values.
   * // $values will be an array containing the distinct set of values for this
   *   field instance.
   * $values = $instance->getValueList();
   * @endcode
   *
   * @return
   *   An array of values.
   */
  public function getValueList($options = [], $keyword = NULL) {
    $values = [];

    // Set some defaults.
    $options['limit'] = (isset($options['limit'])) ? $options['limit'] : 25;
    $options['label_string'] = (isset($options['label_string'])) ? $options['label_string'] : '';

    // Make sure we know the chado table and column.
    // If not, we can't give them a list *shrugs*.
    if (!isset($this->instance['settings']['chado_table']) OR !isset($this->instance['settings']['chado_column'])) {
      tripal_report_error(
        'TripalField',
        TRIPAL_WARNING,
        'Values List: Unable to generate a values list for %field_name since we don\'t know it\'s chado table/column.',
        ['%field_name' => $this->instance['field_name']]
      );
      return FALSE;
    }

    // First get some important info about the chado table.column this field is attached to.
    $chado_table = $this->instance['settings']['chado_table'];
    $chado_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];
    $bschema = chado_get_schema($base_table);

    // Now build the distinct query.
    if ($chado_table == $base_table) {

      // Is the current column a foreign key to another table?
      $is_fk = FALSE;
      $fk_table = $fk_column = NULL;
      foreach ($bschema['foreign keys'] as $k => $v) {
        if (isset($v['columns'][$chado_column])) {
          $is_fk = TRUE;
          $fk_table = $v['table'];
          $fk_column = $v['columns'][$chado_column];
        }
      }

      // Check if this column is a foreign key to another one.
      // If so we would like to travel through the relationship
      // to capture a better human-readable option.
      if ($is_fk) {

        // Determine the query.
        $sql = "SELECT base.$chado_column as id, fk.*
                FROM {" . $chado_table . "} base
                LEFT JOIN {" . $fk_table . "} fk ON base.$chado_column=fk.$fk_column
                GROUP BY base.$chado_column, fk.$fk_column
                LIMIT " . $options['limit'];

        // Choose a default label string, if needed.
        if (empty($options['label_string'])) {
          $fkschema = chado_get_schema($fk_table);
          if (isset($fkschema['fields']['name'])) {
            $options['label_string'] = '[name]';
          }
          elseif (isset($fkschema['fields']['uniquename'])) {
            $options['label_string'] = '[uniquename]';
          }
          elseif (isset($fkschema['fields']['accession'])) {
            $options['label_string'] = '[accession]';
          }
          elseif (isset($fkschema['fields']['title'])) {
            $options['label_string'] = '[title]';
          }
          elseif ($fk_table == 'organism') {
            $options['label_string'] = '[genus] [species]';
          }
          else {
            tripal_report_error(
              'TripalField',
              TRIPAL_WARNING,
              'Values List: Unable to generate a default human-readable label for %field_name since there is no name/uniquename column. Please set the options[label_string].',
              ['%field_name' => $this->instance['field_name']]
            );
            return FALSE;
          }
        }
      }
      // Not a foreign key, so just make the key and value from the base table.
      else {
        $sql = "SELECT $chado_column as id, $chado_column
                FROM {" . $chado_table . "} base
                GROUP BY $chado_column
                LIMIT " . $options['limit'];

        // Choose a default label string, if needed.
        if (empty($options['label_string'])) {
          $options['label_string'] = '[' . $chado_column . ']';
        }
      }
    }
    else {
      tripal_report_error(
        'TripalField',
        TRIPAL_WARNING,
        'Unable to retrieve a values list for %field_name since it is not a direct column in %base',
        ['%field_name' => $this->instance['field_name'], '%base' => $base_table]
      );
      return FALSE;
    }

    $results = chado_query($sql);

    // Pre-process the label string for better performance.
    // Each token is enclosed in square brackets and should be the name of a chado column.
    preg_match_all('/\[(\w+)\]/', $options['label_string'], $matches);
    $tokens = $matches[1];

    foreach ($results as $r) {
      // Determine the label using the label_string option.
      $label = $options['label_string'];
      $replace = [];
      foreach ($tokens as $column) {
        if (isset($r->{$column})) {
          $replace["[$column]"] = $r->{$column};
        }
        else {
          $replace["[$column]"] = "";
        }
      }

      // Set the value.
      $values[$r->id] = strtr($options['label_string'], $replace);
    }

    return $values;
  }

  /**
   * @see TripalField::instanceSettingsForm()
   */
  public function instanceSettingsForm() {
    // Make sure we don't lose our Chado table mappings when the settings
    // are updated.  Setting them as values in the form ensures they don't
    // get accidentally overwritten.
    $element['base_table'] = [
      '#type' => 'value',
      '#value' => $this->instance['settings']['base_table'],
    ];
    $element['chado_table'] = [
      '#type' => 'value',
      '#value' => $this->instance['settings']['chado_table'],
    ];
    $element['chado_column'] = [
      '#type' => 'value',
      '#value' => $this->instance['settings']['chado_column'],
    ];
    return $element;
  }
}

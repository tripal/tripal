<?php

class ChadoField extends TripalField {

  // The default lable for this field.
  public static $default_label = 'Chado Field';

  // The default description for this field.
  public static $default_description = 'The generic base class for all Chado fields. Replace this text as appropriate for the child implementation.';

  // A list of global settings. These can be accessed within the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  // Once instances exist for a field type then these settings cannot be
  // changed.
  public static $default_settings = array(
    'storage' => 'field_chado_storage',
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
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
    // Indicates the download formats for this field.  The list must be the
    // name of a child class of the TripalFieldDownloader.
    'download_formatters' => array(
      'TripalTabDownloader',
      'TripalCSVDownloader',
    ),
  );

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
      $base_field = tripal_get_chado_semweb_column($chado_table, $condition['column']);
      $query->condition('base.' . $base_field , $condition['value'], $operator);
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
      $base_field = tripal_get_chado_semweb_column($chado_table, $order['column']);
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

    switch($type) {
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
   * @see TripalField::instanceSettingsForm()
   */
  public function instanceSettingsForm() {
    // Make sure we don't lose our Chado table mappings when the settings
    // are updated.  Setting them as values in the form ensures they don't
    // get accidentally overwritten.
    $element['base_table'] = array(
      '#type' => 'value',
      '#value' => $this->instance['settings']['base_table'],
    );
    $element['chado_table'] = array(
      '#type' => 'value',
      '#value' => $this->instance['settings']['chado_table'],
    );
    $element['chado_column'] = array(
      '#type' => 'value',
      '#value' => $this->instance['settings']['chado_column'],
    );
    return $element;
  }
}

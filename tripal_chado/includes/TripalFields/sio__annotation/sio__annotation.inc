<?php

class sio__annotation extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Annotations';

  // The default description for this field.
  public static $description = 'This record is annotated with controlled vocabulary terms.';

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'SIO',
    // The name of the term.
    'term_name' => 'annotation',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '001166',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  );

  // The default widget for this field.
  public static $default_widget = 'chado_linker__cvterm_widget';

  // The default formatter for this field.
  public static $default_formatter = 'chado_linker__cvterm_formatter';

  // A boolean specifying that users should not be allowed to create
  // fields and instances of this field type through the UI. Such
  // fields can only be created programmatically with field_create_field()
  // and field_create_instance().
  public static $no_ui = FALSE;

  /**
   *
   * @see TripalField::load()
   */
  public function load($entity) {
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    $vocabulary = tripal_get_chado_semweb_term('db', 'name');
    $accession = tripal_get_chado_semweb_term('db', 'accession');
    $definition = tripal_get_chado_semweb_term('cvterm', 'definition');

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $chado_record = $entity->chado_record;
    $entity->{$field_name}['und'][0] = array(
      'value' => '',
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $fkey_lcolumn => $chado_record->$fkey_rcolumn,
      'chado-' . $field_table . '__' . 'cvterm_id' => '',
      // The pub column is present in the cell_line_cvterm, feature_cvterm,
      // library_cvterm, phenotype_comparision_cvterm, phenotype_cvterm,
      // stock_cvterm, and stock_relationship_cvterm.
      'chado-' . $field_table . '__' . 'pub_id' => '',
      // The is_not column is present in feature_cvterm and stock_cvterm tables.
      'chado-' . $field_table . '__' . 'is_not' => '',
      // The rank column is present in the cell_line_cvterm, expression_cvterm,
      // feature_cvterm, phenotype_comparision_cvterm, phenotype_cvterm,
      // and stock_cvterm tables.
      'chado-' . $field_table . '__' . 'rank' => '',
    );

    // Get the annotations associated with this base record for this fields type.
    $columns = array('*');
    $match = array(
      $fkey_lcolumn => $chado_record->$fkey_rcolumn,
    );
    $options = array(
      'return_array' => TRUE,
      'order_by' => array('rank' => 'ASC')
    );
    $fcvterms = chado_select_record($field_table, $columns, $match, $options);
    for ($i = 0; $i < count($fcvterms); $i++) {
      $linker = $fcvterms[$i];
      $cvterm = chado_generate_var('cvterm', array('cvterm_id' => $linker->cvterm_id));
      $entity->{$field_name}['und'][$i] = array(
        'value' => array(
          $vocabulary => $cvterm->dbxref_id->db_id->name,
          $accession => $cvterm->dbxref_id->accession,
          $definition => $cvterm->cvterm_id->name
        ),
        'chado-' . $field_table . '__' . $pkey => $linker->$pkey,
        'chado-' . $field_table . '__' . $fkey_lcolumn => $linker->$fkey_lcolumn,
        'chado-' . $field_table . '__' . 'cvterm_id' => $linker->cvterm_id,
        'chado-' . $field_table . '__' . 'pub_id' => property_exists($linker, 'pub_id') ? $linker->pub_id : '',
        'chado-' . $field_table . '__' . 'is_not' => property_exists($linker, 'is_not') ? $linker->is_not : '',
        'chado-' . $field_table . '__' . 'rank' => property_exists($linker, 'rank') ? $linker->rank : '',
        'chado-' . $field_table . '__' . 'cvterm_type_id' => property_exists($linker, 'cvterm_type_id') ? $linker->cvterm_type_id : '',
      );
    }
  }
}

/**
 * Theme function for the dbxref_id_widget.
 *
 * @param $variables
 */
function theme_chado_linker__cvterm_widget($variables) {
  $element = $variables['element'];

  // These two fields were added to the widget to help identify the fields
  // for layout.
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  $layout = "
    <div class=\"annotation-cvterm-widget\">
      <div class=\"annotation-cvterm-widget-item\">" .
      drupal_render($element['cv__cv_id']) . "
      </div>
      <div class=\"annotation-cvterm-widget-item\">" .
      drupal_render($element['cvterm__name']) . "
      </div>
      <div class=\"annotation-cvterm-widget-item\">" .
      drupal_render($element['pub']) . "
      </div>
      <div class=\"annotation-cvterm-widget-item\">" .
      drupal_render($element['chado-' . $table_name . '__is_not']) . "
      </div>
    </div>
  ";
  return $layout;
}

/**
 * An Ajax callback for the dbxref widget.
 */
function chado_linker__cvterm_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];


  return $form[$field_name]['und'][$delta];
}
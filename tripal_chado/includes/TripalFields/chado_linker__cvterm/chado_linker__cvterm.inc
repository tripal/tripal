<?php

class chado_linker__cvterm extends TripalField {


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
  public static $description = 'This record can be annotated with terms from other
              vocabularies.';

  // Provide a list of global settings. These can be accessed witihn the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  public static $default_settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'local',
    // The name of the term.
    'term_name' => 'cvterm',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => 'cvterm',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  );

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $storage = 'field_chado_storage';

  // The default widget for this field.
  public static $default_widget = 'chado_linker__cvterm_widget';

  // The default formatter for this field.
  public static $default_formatter = 'chado_linker__cvterm_formatter';

  // --------------------------------------------------------------------------
  //              PROTECTED CLASS MEMBERS -- DO NOT OVERRIDE
  // --------------------------------------------------------------------------
  // An array containing details about the field. The format of this array
  // is the same as that returned by field_info_fields()
  protected $field;
  // An array containing details about an instance of the field. A field does
  // not have to have an instance.  But if dealing with an instance (such as
  // when using the widgetForm, formatterSettingsForm, etc.) it should be set.
  protected $instance;


  /**
   *
   * @see TripalField::validate()
   */
  public function validate($entity_type, $entity, $field, $items, &$errors) {

  }


  /**
   *
   * @see TripalField::load()
   */
  public function load($entity, $details = array()) {
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];
    $base_table = $this->field['settings']['base_table'];

    $matches = array();
    preg_match('/(.*?)__(\d+)/', $field_name, $matches);
    $table_name = $matches[1];
    $cv_id = $matches[2];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $chado_record = $entity->chado_record;
    $entity->{$field_name}['und'][0] = array(
      'value' => '',
      'chado-' . $field_table . '__' . $fkey_lcolumn => '',
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
      // The cvterm_type_id is present in the expression_cvterm table.
      'cvterm_type_id' => '',
      // The following field are to help link the cvterm.
      'cv__cv_id' => '',
      'cvterm__name' => '',
    );

    // Get the annotations associated with this base record for this fields type.
    $columns = array('*');
    $match = array(
      $fkey_lcolumn => $chado_record->$fkey_rcolumn,
      'cvterm_id' => array(
        'cv_id' => $cv_id,
      ),
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
        'value' => $linker->$pkey,
        'chado-' . $field_table . '__' . $fkey_lcolumn => $linker->$fkey_lcolumn,
        'chado-' . $field_table . '__' . 'cvterm_id' => $linker->cvterm_id,
        'chado-' . $field_table . '__' . 'pub_id' => property_exists($linker, 'pub_id') ? $linker->pub_id : '',
        'chado-' . $field_table . '__' . 'is_not' => property_exists($linker, 'is_not') ? $linker->is_not : '',
        'chado-' . $field_table . '__' . 'rank' => property_exists($linker, 'rank') ? $linker->rank : '',
        'chado-' . $field_table . '__' . 'cvterm_type_id' => property_exists($linker, 'cvterm_type_id') ? $linker->cvterm_type_id : '',
        'cv__cv_id' => $cvterm->cv_id->cv_id,
        'cvterm__name' => $cvterm->name,
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
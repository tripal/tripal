<?php

class chado_linker__dbxref extends TripalField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The term that this field maps to.  The format for the term should be:
  // [vocab]:[accession] where [vocab] is the short name of the vocabulary
  // and [acession] is the unique accession number for the term.  This term
  // must already exist in the vocabulary storage backend. This
  // value should never be changed once fields exist for this type.
  public static $term = 'SBO:0000554';

  // The default lable for this field.
  public static $label = 'Cross references';

  // The default description for this field.
  public static $description = 'This record can be cross referenced with a record in
          another online database. This field is intended for one or more
          references.  At a minimum, the database and accession must be provided.';

  // Provide a list of global settings. These can be accessed witihn the
  // globalSettingsForm.  When the globalSettingsForm is submitted then
  // Drupal will automatically change these settings for all fields.
  public static $settings = array(
    'chado_table' => '',
    'chado_column' => '',
    'base_table' => '',
  );

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  public static $instance_settings  = array();

  // Set this to the name of the storage backend that by default will support
  // this field.
  public static $storage = 'tripal_no_storage';

  // The default widget for this field.
  public static $default_widget = 'chado_linker__dbxref_widget';

  // The default formatter for this field.
  public static $default_formatter = 'chado_linker__dbxref_formatter';

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
    $record = $details['record'];
    
    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->field['settings']['chado_table'];
    $field_column = $this->field['settings']['chado_column'];
    $base_table = $record->tablename;
    
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];
    $fkeys = array_values($schema['foreign keys'][$base_table]['columns']);
    $fkey = $fkeys[0];
    
    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__' . $fkey => '',
      'chado-' . $field_table . '__dbxref_id' => '',
      'dbxref_id' => '',
      'db_id' => '',
      'accession' => '',
      'version' => '',
      'description' => '',
    );
    
    $linker_table = $base_table . '_dbxref';
    $options = array('return_array' => 1);
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    if (count($record->$linker_table) > 0) {
      $i = 0;
      foreach ($record->$linker_table as $index => $linker) {
        $dbxref = $linker->dbxref_id;
        $URL = tripal_get_dbxref_url($dbxref);
        $entity->{$field_name}['und'][$i] = array(
          'value' => array(
            'vocabulary' => $dbxref->db_id->name,
            'accession' => $dbxref->accession,
            'URL' => $URL,
          ),
          'chado-' . $field_table . '__' . $pkey => $linker->$pkey,
          'chado-' . $field_table . '__' . $fkey => $linker->$fkey->$fkey,
          'chado-' . $field_table . '__dbxref_id' => $dbxref->dbxref_id,
          'dbxref_id' => $dbxref->dbxref_id,
          'db_id' => $dbxref->db_id->db_id,
          'accession' => $dbxref->accession,
          'version' => $dbxref->version,
          'description' => $dbxref->description,
        );
        $i++;
      }
    }
  }


  /**
   *
   * @see TripalField::settingsForm()
   */
  public function settingsForm($has_data) {

  }
  /**
   *
   * @param unknown $form
   * @param unknown $form_state
   */
  public function settingsFormValidate($form, &$form_state) {

  }

  /**
   *
   * @see TripalField::instanceSettingsForm()
   */
  public function instanceSettingsForm() {

  }
  /**
   *
   * @see TripalField::instanceSettingsFormValidate()
   */
  public function instanceSettingsFormValidate($form, &$form_state) {

  }

}

/**
 * Theme function for the dbxref_id_widget.
 *
 * @param $variables
 */
function theme_chado_linker__dbxref_widget($variables) {
  $element = $variables['element'];

  // These two fields were added to the widget to help identify the fields
  // for layout.
  $table_name = $element['#table_name'];
  $fkey = $element['#fkey_field'];

  $layout = "
      <div class=\"secondary-dbxref-widget\">
        <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element['db_id']) . "
        </div>
        <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element['accession']) . "
        </div>
        <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element['version']) . "
        </div>
        <div class=\"secondary-dbxref-widget-item\">" .
        drupal_render($element['description']) . "
        </div>
        <div class=\"secondary-dbxref-widget-links\">" . drupal_render($element['links']) . "</div>
      </div>
    ";

        return $layout;
}

/**
 * An Ajax callback for the dbxref widget.
 */
function chado_linker__dbxref_widget_form_ajax_callback($form, $form_state) {

  $field_name = $form_state['triggering_element']['#parents'][0];
  $delta = $form_state['triggering_element']['#parents'][2];
  $field = field_info_field($field_name);
  $field_type = $field['type'];
  $field_table = $field['settings']['chado_table'];
  $field_column = $field['settings']['chado_column'];
  $field_prefix = 'chado-' . $field_table . '__dbxref_id';

  // Check to see if this dbxref already exists. If not then
  // give a notice to the user that the dbxref will be added.
  $db_id = $form_state['values'][$field_name]['und'][$delta]['db_id'];
  $db_id = $form_state['values'][$field_name]['und'][$delta]['accession'];
  if ($db_id and $accession) {
    $values = array(
      'db_id' => $db_id,
      'accession' => $accession,
    );
    $options = array('is_duplicate' => TRUE);
    $has_duplicate = chado_select_record('dbxref', array('*'), $values, $options);
    if (!$has_duplicate) {
      drupal_set_message('The selected cross reference is new and will be added for future auto completions.');
    }
  }

  return $form[$field_name]['und'][$delta];
}
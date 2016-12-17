<?php

class chado_linker__expression extends TripalField {


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
  public static $term = 'local:expression';

  // The default lable for this field.
  public static $label = 'Expression';

  // The default description for this field.
  public static $description = 'Associates an expression with
          this record.';

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
  public static $default_widget = 'chado_linker__expression_widget';

  // The default formatter for this field.
  public static $default_formatter = 'chado_linker__expression_formatter';

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

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $base_table = $details['record']->tablename;
    $pkey = $schema['primary key'][0];
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
    );

    $linker_table = $base_table . '_expression';
    $options = array(
      'return_array' => 1,
    );
    $record = chado_expand_var($record, 'table', $linker_table, $options);
    $exp_linkers = $record->$linker_table;
    if ($exp_linkers) {
      foreach ($exp_linkers as $i => $exp_linker) {

        // Because the unqiuename is a text field we must expand it.
        $expression = $exp_linker->expression_id;
        $expression = chado_expand_var($expression, 'field', 'expression.uniquename', $options);

        // Set the values that will be seen by the user on the page and in
        // web services, or anwhere this field is viewed.
        $entity->{$field_name}['und'][$i]['value'] = array(
          'name' => $expression->uniquename,
          'description' => $expression->description,
          //'md5checksum' => $expression->md5checksum,
        );

        // Add the pub information if a real pub is associated with the record.
        $pub = $exp_linker->pub_id;
        if ($pub->uniquename != 'null') {
          $pub_details = tripal_get_minimal_pub_info($pub);

          $entity->{$field_name}['und'][$i]['value']['publication'] = $pub_details;
          $entity->{$field_name}['und'][$i]['value']['publication']['type'] = $pub->type_id->name;
          if (property_exists($pub, 'entity_id')) {
            $entity->{$field_name}['und'][$i]['publication'][0]['value']['entity'] = 'TripalEntity:' . $pub->entity_id;
          }
        }

        // Add the linker_expressionprop
        $linkerprop_table =  $linker_table . 'prop';
        if (chado_table_exists($linkerprop_table)) {
          $exp_linker = chado_expand_var($exp_linker, 'table', $linkerprop_table, $options);
          $exp_linkerprops = $exp_linker->feature_expressionprop;
          if ($exp_linkerprops) {
            foreach ($exp_linkerprops AS $linkerprop) {
              $entity->{$field_name}['und'][$i]['value'][$linkerprop->type_id->name] = $linkerprop->value;
            }
          }
        }
        // Add the fields for the widget form.  The name requres the following
        // format if the fields should be used as values for insertint/updating
        // into the chado table:  [table_name]__[field_name]
        $entity->{$field_name}['und'][$i][$linker_table . '__expression_id'] = $expression->expression_id;
        $entity->{$field_name}['und'][$i][$linker_table . '__uniquename'] = $expression->uniquename;
        //$entity->{$field_name}['und'][$i][$linker_table . '__md5checksum'] = $expression->md5checksum;
        $entity->{$field_name}['und'][$i][$linker_table . '__description'] = $expression->description;

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
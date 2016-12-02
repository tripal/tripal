<?php

class so__transcript extends TripalField {


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
  public static $term = 'SO:0000673';

  // The default lable for this field.
  public static $label = 'Transcript';

  // The default description for this field.
  public static $description = 'An RNA synthesized on a DNA or RNA template by an RNA polymerase.';

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
  public static $default_widget = 'so__transcript_widget';

  // The default formatter for this field.
  public static $default_formatter = 'so__transcript_formatter';

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
    
    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(
        'type' => '',
        'name' => '',
        'identifier' => '',
        'location' => '',
      ),
    );
    
    // TODO: If the tripal_get_feature_relationships() slows this down then
    // we may need to write a custom function to get the data.
    $rels = tripal_get_feature_relationships($record);
    
    // TODO: what if other transcripts names from SO are used. In that
    // case we should support those too (using cvtermpath table to find them).
    // mRNA should not be hard-coded below.
    
    // Set the value to be a array of "table" rows.
    $transcripts = array();
    if (key_exists('part of', $rels['object']) &&
        key_exists('mRNA', $rels['object']['part of'])) {
          $transcripts =  $rels['object']['part of']['mRNA'];
        }
    
        $headers = array('Name' ,'Identifier', 'Location');
        $rows = array();
        $i = 0;
        foreach ($transcripts as $transcript) {
          // link the feature to it's node
          $feature_name = $transcript->record->subject_id->name;
    
          $locations = $transcript->child_featurelocs;
          $loc = "";
          foreach ($locations AS $location) {
            $loc .= $location->srcfeature_name . ":" . $location->fmin . ".." . $location->fmax;
          }
          $type = $transcript->record->subject_id->type_id;
          $entity->{$field_name}['und'][$i]['value'] = array(
            'type' => $type->name,
            'name' => $feature_name,
            'identifier' => $transcript->record->subject_id->uniquename,
            'location' => $loc,
    
          );
          // Add in the semantic web information that describes each key in the
          // value array.
          $entity->{$field_name}['und'][$i]['semantic_web'] = array(
            'type' => $type->dbxref_id->db_id->name . ":" . $type->dbxref_id->accession,
            'name' => tripal_get_chado_semweb_term('cvterm', 'name'),
            'identifier' => tripal_get_chado_semweb_term('feature', 'uniquename'),
            'location' => '',
          );
          if (property_exists($transcript->record->subject_id, 'entity_id')) {
            $entity_id = $transcript->record->subject_id->entity_id;
            $entity->{$field_name}['und'][$i]['value']['entity'] = 'TripalEntity:' . $entity_id;
          }
          $i++;
        }
  }

}
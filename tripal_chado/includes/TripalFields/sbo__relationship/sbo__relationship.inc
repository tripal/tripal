<?php

class sbo__relationship extends ChadoField {


  // --------------------------------------------------------------------------
  //                     EDITABLE STATIC CONSTANTS
  //
  // The following constants SHOULD be set for each descendent class.  They are
  // used by the static functions to provide information to Drupal about
  // the field and it's default widget and formatter.
  // --------------------------------------------------------------------------

  // The default lable for this field.
  public static $default_label = 'Relationship';

  // The default description for this field.
  public static $description = 'Relationships between records.';

  // Provide a list of instance specific settings. These can be access within
  // the instanceSettingsForm.  When the instanceSettingsForm is submitted
  // then Drupal with automatically change these settings for the instnace.
  // It is recommended to put settings at the instance level whenever possible.
  // If you override this variable in a child class be sure to replicate the
  // term_name, term_vocab, term_accession and term_fixed keys as these are
  // required for all TripalFields.
  public static $default_instance_settings  = array(
    // The short name for the vocabulary (e.g. shcema, SO, GO, PATO, etc.).
    'term_vocabulary' => 'SBO',
    // The name of the term.
    'term_name' => 'Relationship',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '0000374',
    // Set to TRUE if the site admin is allowed to change the term
    // type. This will create form elements when editing the field instance
    // to allow the site admin to change the term settings above.
    'term_fixed' => FALSE,
  );

  // The default widget for this field.
  public static $default_widget = 'sbo__relationship_widget';

  // The default formatter for this field.
  public static $default_formatter = 'sbo__relationship_formatter';

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
    $settings = $this->field['settings'];

    $record = $details['record'];

    $field_name = $this->field['field_name'];
    $field_type = $this->field['type'];
    $field_table = $this->instance['settings']['chado_table'];
    $field_column = $this->instance['settings']['chado_column'];
    $base_table = $this->instance['settings']['base_table'];

    // Get the PKey for this table
    $schema = chado_get_schema($field_table);
    $pkey = $schema['primary key'][0];

    // Get the Pkeys for the subject and object tables
    $subject_fkey_table = '';
    $object_fkey_table = '';
    $fkeys = $schema['foreign keys'];
    foreach ($fkeys as $fktable => $details) {
      foreach ($details['columns'] as $fkey_lcolumn => $fkey_rcolumn) {
        if ($fkey_lcolumn == 'subject_id') {
          $subject_fkey_table = $fktable;
        }
        if ($fkey_lcolumn == 'object_id') {
          $object_fkey_table = $fktable;
        }
      }
    }
    $subject_schema = chado_get_schema($subject_fkey_table);
    $object_schema = chado_get_schema($object_fkey_table);
    $subject_pkey = $subject_schema['primary key'][0];
    $object_pkey = $object_schema['primary key'][0];

    // Get the FK that links to the base record.
    $schema = chado_get_schema($field_table);
    $fkey_lcolumn = key($schema['foreign keys'][$base_table]['columns']);
    $fkey_rcolumn = $schema['foreign keys'][$base_table]['columns'][$fkey_lcolumn];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
      'chado-' . $field_table . '__' . $pkey => '',
      'chado-' . $field_table . '__subject_id' => '',
      'chado-' . $field_table . '__object_id' => '',
      'chado-' . $field_table . '__type_id' => '',
      // These elements don't need to follow the naming scheme above
      // becasue we don't need the chado_field_storage to try and
      // save these values.
      'object_name' => '',
      'subject_name' => '',
      'type_name' => '',
    );

    // If the table has rank and value fields then add those to the default
    // value array.
    if (array_key_exists('value', $schema['fields'])) {
      $entity->{$field_name}['und'][0]['chado-' . $field_table . '__value'] = '';
    }
    if (array_key_exists('rank', $schema['fields'])) {
      $entity->{$field_name}['und'][0]['chado-' . $field_table . '__rank'] = '';
    }

    // If we have no record then just return.
    if (!$record) {
      return;
    }

    // Expand the object to include the relationships.
    $options = array(
      'return_array' => 1,
      // we don't want to fully recurse we only need information about the
      // relationship type and the object and subject
      'include_fk' => array(
        'type_id' => 1,
        'object_id' => array(
          'type_id' => 1,
        ),
        'subject_id'  => array(
          'type_id' => 1,
        ),
      ),
    );
    $rel_table = $base_table . '_relationship';
    $schema = chado_get_schema($rel_table);
    if (array_key_exists('rank', $schema['fields'])) {
      $options['order_by'] = array('rank' => 'ASC');
    }
    $record = chado_expand_var($record, 'table', $rel_table, $options);
    if (!$record->$rel_table) {
      return;
    }
    $srelationships = null;
    $orelationships = null;
    if ($rel_table == 'nd_reagent_relationship') {
      $srelationships = $record->$rel_table->subject_reagent_id;
      $orelationships = $record->$rel_table->object_reagent_id;
    }
    else if ($rel_table == 'project_relationship') {
      $srelationships = $record->$rel_table->subject_project_id;
      $orelationships = $record->$rel_table->object_project_id;
    }
    else {
      $srelationships = $record->$rel_table->subject_id;
      $orelationships = $record->$rel_table->object_id;
    }

    $i = 0;
    if ($orelationships) {
      foreach ($orelationships as $relationship) {
        $rel_acc = $relationship->type_id->dbxref_id->db_id->name . ':' . $relationship->type_id->dbxref_id->accession;
        $rel_type = $relationship->type_id->name;
        $verb = self::get_rel_verb($rel_type);
        $subject_name = $relationship->subject_id->name;
        $subject_type = $relationship->subject_id->type_id->name;
        $object_name = $relationship->object_id->name;
        $object_type = $relationship->object_id->type_id->name;
        $entity->{$field_name}['und'][$i]['value'] = array(
          'type' => $relationship->type_id->name,
          'subject' => array(
            'type' => $subject_type,
            'name' => $subject_name,
          ),
          'type' => $relationship->type_id->name,
          'object' => array(
            'type' => $object_type,
            'name' => $object_name,
            'entity' => 'TripalEntity:' . $entity->id,
          )
        );
        if (property_exists($relationship->subject_id, 'uniquename')) {
          $entity->{$field_name}['und'][$i]['value']['subject']['identifier'] =  $relationship->subject_id->uniquename;;
        }
        if (property_exists($relationship->object_id, 'uniquename')) {
          $entity->{$field_name}['und'][$i]['value']['object']['identifier'] = $relationship->object_id->uniquename;
        }
        if (property_exists($relationship->subject_id, 'entity_id')) {
          $entity_id = $relationship->subject_id->entity_id;
          $entity->{$field_name}['und'][$i]['value']['subject']['entity'] = 'TripalEntity:' . $entity_id;
        }
        $rel_type_clean = lcfirst(preg_replace('/_/', ' ', $rel_type));
        $entity->{$field_name}['und'][$i]['value']['phrase'] = 'The ' . $subject_type . ', ' .
            $subject_name . ', ' . $verb . ' '  . $rel_type_clean . ' this '  .
            $object_type . '.';

            $entity->{$field_name}['und'][$i]['semantic_web'] = array(
              'type' => $rel_acc,
              'subject' => $relationship->subject_id->type_id->dbxref_id->db_id->name . ':' . $relationship->subject_id->type_id->dbxref_id->accession,
              'object' => $relationship->object_id->type_id->dbxref_id->db_id->name . ':' . $relationship->object_id->type_id->dbxref_id->accession,
            );

            $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__' . $pkey] = $relationship->$pkey;
            $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__subject_id'] = $relationship->subject_id->$subject_pkey;
            $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__type_id'] = $relationship->type_id->cvterm_id;
            $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__object_id'] = $relationship->object_id->$object_pkey;

            $entity->{$field_name}['und'][$i]['type_name'] = $relationship->type_id->name;
            $entity->{$field_name}['und'][$i]['subject_name'] = $relationship->subject_id->name . ' [id: ' . $relationship->subject_id->$fkey_rcolumn . ']';
            $entity->{$field_name}['und'][$i]['object_name'] = $relationship->object_id->name  . ' [id: ' . $relationship->object_id->$fkey_rcolumn . ']';
            if (array_key_exists('value', $schema['fields'])) {
              $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__value'] = $relationship->value;
            }
            if (array_key_exists('rank', $schema['fields'])) {
              $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__rank'] = $relationship->rank;
            }
            $i++;
      }
    }

    if ($srelationships) {
      foreach ($srelationships as $relationship) {
        $rel_acc = $relationship->type_id->dbxref_id->db_id->name . ':' . $relationship->type_id->dbxref_id->accession;
        $rel_type = $relationship->type_id->name;
        $verb = self::get_rel_verb($rel_type);
        $subject_name = $relationship->subject_id->name;
        $subject_type = $relationship->subject_id->type_id->name;
        $object_name = $relationship->object_id->name;
        $object_type = $relationship->object_id->type_id->name;
        $entity->{$field_name}['und'][$i]['value'] = array(
          '@type' => $relationship->type_id->name,
          'subject' => array(
            'type' => $subject_type,
            'name' => $subject_name,
            'entity' => 'TripalEntity:' . $entity->id,
          ),
          'type' => $relationship->type_id->name,
          'object' => array(
            'type' =>  $object_type,
            'name' => $object_name,
          )
        );
        if (property_exists($relationship->subject_id, 'uniquename')) {
          $entity->{$field_name}['und'][$i]['value']['subject']['identifier'] = $relationship->subject_id->uniquename;
        }
        if (property_exists($relationship->object_id, 'uniquename')) {
          $entity->{$field_name}['und'][$i]['value']['object']['identifier'] = $relationship->object_id->uniquename;
        }
        if (property_exists($relationship->object_id, 'entity_id')) {
          $entity_id = $relationship->object_id->entity_id;
          $entity->{$field_name}['und'][$i]['value']['object']['entity'] = 'TripalEntity:' . $entity_id;
        }
        $rel_type_clean = lcfirst(preg_replace('/_/', ' ', $rel_type));
        $entity->{$field_name}['und'][$i]['value']['phrase'] = 'This  ' .
            $subject_type . ' ' . $verb . ' '  . $rel_type_clean . ' the '  .
            $object_type . ', ' . $object_name . '.';


            $entity->{$field_name}['und'][$i]['semantic_web'] = array(
              'type' => $rel_acc,
              'subject' => $relationship->subject_id->type_id->dbxref_id->db_id->name . ':' . $relationship->subject_id->type_id->dbxref_id->accession,
              'object' => $relationship->object_id->type_id->dbxref_id->db_id->name . ':' . $relationship->object_id->type_id->dbxref_id->accession,
            );


            $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__' . $pkey] = $relationship->$pkey;
            $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__subject_id'] = $relationship->subject_id->$subject_pkey;
            $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__type_id'] = $relationship->type_id->cvterm_id;
            $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__object_id'] = $relationship->object_id->$object_pkey;

            $entity->{$field_name}['und'][$i]['type_name'] = $relationship->type_id->name;
            $entity->{$field_name}['und'][$i]['subject_name'] = $relationship->subject_id->name  . ' [id: ' . $relationship->subject_id->$fkey_rcolumn . ']';
            $entity->{$field_name}['und'][$i]['object_name'] = $relationship->object_id->name  . ' [id: ' . $relationship->object_id->$fkey_rcolumn . ']';

            if (array_key_exists('value', $schema['fields'])) {
              $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__value'] = $relationship->value;
            }
            if (array_key_exists('rank', $schema['fields'])) {
              $entity->{$field_name}['und'][$i]['chado-' . $field_table . '__rank'] = $relationship->rank;
            }
            $i++;
      }
    }
  }

  /**
   * A helper function to define English verbs for relationship types.
   *
   * @param $rel_type
   *   The vocabulary term name for the relationship.
   *
   * @return
   *   The verb to use when creating a sentence of the relationship.
   */
  public static function get_rel_verb($rel_type) {
    $rel_type_clean = lcfirst(preg_replace('/_/', ' ', $rel_type));
    $verb = $rel_type_clean;
    switch ($rel_type_clean) {
      case 'integral part of':
      case 'instance of':
        $verb = 'is an';
        break;
      case 'proper part of':
      case 'transformation of':
      case 'genome of':
      case 'part of':
        $verb = 'is a';
      case 'position of':
      case 'sequence of':
      case 'variant of':
        $verb = 'is a';
        break;
      case 'derives from':
      case 'connects on':
      case 'contains':
      case 'finishes':
      case 'guides':
      case 'has origin':
      case 'has part':
      case 'has quality':
      case 'is consecutive sequence of':
      case 'maximally overlaps':
      case 'overlaps':
      case 'starts':
        break;
      default:
        $verb = 'is';
    }

    return $verb;
  }

  /**
   *
   * @see TripalField::settingsForm()
   */
  public function settingsForm($has_data) {
    $element = parent::instanceSettingsForm();

    //$element = parent::instanceSettingsForm();
    $element['relationships'] = array(
      '#type' => 'fieldset',
      '#title' => 'Allowed Relationship Types',
      '#description' => t('There are three ways that relationship types
          can be limited for users who have permission to add new relationships.
          Please select the most appropriate for you use case.  By default
          all vocabularies are provided to the user which allows use of any
          term for the relationship type.'),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
      '#theme' => 'sbo__relationship_instance_settings'
    );
    //     $element['instructions'] = array(
    //       '#type' => 'item',
    //       '#markup' => 'You may provide a list of terms that will be available in a select box
    //         as the relationship types. This select box will replace the vocabulary select box if the
    //         following value is set.'
    //     );
    $vocs = tripal_get_cv_select_options();
    $element['relationships']['option1'] = array(
      '#type' => 'item',
      '#title' => 'Option #1',
      '#description' => t('Use this option to limit the vocabularies that a user .
        could use to specify relationship types. With this option any term in .
        the vocabulary can be used for the relationship type. You may select
        more than one vocabulary.'),

    );
    $element['relationships']['option1_vocabs'] = array(
      '#type' => 'select',
      '#multiple' => TRUE,
      '#options' => $vocs,
      '#size' => 6,
      '#default_value' => $this->instance['settings']['relationships']['option1_vocabs'],
      // TODO add ajax here so that the relationship autocomplete below works
    );

    $element['relationships']['option2'] = array(
      '#type' => 'item',
      '#title' => '<b>Option #2</b>',
      '#description' => 'Some vocabularies are heirarchichal (an ontology). Within this
         heirarchy groups of related terms typically fall under a common parent. If you
         wish to limit the list of terms that a user can use for the relationship type,
         you can provide the parent term here.  Then, only that term\'s children will
         be avilable for use as a relationship type.',
    );
    $element['relationships']['option2_vocab'] = array(
      '#type' => 'select',
      '#description' => 'Specify Default Vocabulary',
      '#multiple' => FALSE,
      '#options' => $vocs,
      '#default_value' => $this->instance['settings']['relationships']['option2_vocab'],
      '#ajax' => array(
        'callback' => "sbo__relationship_instance_settings_form_ajax_callback",
        'wrapper' => 'relationships-option2-parent',
        'effect' => 'fade',
        'method' => 'replace'
      ),
    );
    $element['relationships']['option2_parent'] = array(
      '#type' => 'textfield',
      '#description' => 'Specify a Heirarchical Parent Term',
      '#default_value' => $this->instance['settings']['relationships']['option2_parent'],
      '#autocomplete_path' => "admin/tripal/storage/chado/auto_name/cvterm/",
      '#prefix' => '<div id=relationships-option2-parent>',
      '#suffix' => '</div>'
    );
    $element['relationships']['option3'] = array(
      '#type' => 'item',
      '#title' => 'Option #3',
      '#description' => 'Provide terms separated by a new line. The term provided should be
        unique and distinguishable by the name. You can use a bar | to separate a vocabulary
        and a term to allow more specific assignment.',
    );
    $element['relationships']['relationship_types'] = array(
      '#type' => 'textarea',
      '#default_value' => $this->instance['settings']['relationships']['relationship_types'],
    );

    return $element;
  }
  /**
   *
   * @param unknown $form
   * @param unknown $form_state
   */
  public function settingsFormValidate($form, &$form_state) {
    // Get relationships settings
    $settings = $form_state['values']['instance']['settings']['relationships'];
    $form_state['values']['instance']['settings']['relationships']['relationship_types']= trim($settings['relationship_types']);

    // Make sure only one option is selected
    $option1test = $settings['option1_vocabs'];
    $option1 = isset($settings['option1_vocabs']) && array_pop($option1test);
    $option2 = (isset($settings['option2_vocab']) && $settings['option2_vocab']) || $settings['option2_parent'];
    $option3 = isset($settings['relationship_types']) && trim($settings['relationship_types']);
    if ($option1 && ($option2 || $option3) == 1 ||
        $option2 && ($option1 || $option3) == 1 ||
        $option3 && ($option1 || $option2) == 1
        ) {
          form_set_error(
              "instance][settings][relationships",
              t("Only one option is allowed to limit the relationship types.")
              );
          return;
        }

        // For option3, make sure the supplied types are valid cvterms
        if ($option3) {
          $rel_types = explode(PHP_EOL, $settings['relationship_types']);
          foreach($rel_types AS $type) {
            $type =  trim($type);
            // Ignore empty lines
            if ($type == '') {
              continue;
            }
            // Find the matching cvterm
            $sql = "SELECT cvterm_id FROM {cvterm} WHERE name = :name";
            $results = chado_query($sql, array(':name' => $type));
            $terms = array();
            while ($obj = $results->fetchObject()) {
              $terms[] = $obj;
            }
            // Don't save the form  if a term can not be found or it matches more than one cvterm
            $cv = '';
            if (count($terms) == 0) {
              // If a term can not be found, maybe the type contains '|', parse it as 'vocabulary|cvterm'
              if (strpos($type, '|')) {
                $tmp = explode('|', $type, 2);
                $type = trim($tmp[1]);
                $cv = tripal_get_cv(array('name' => trim($tmp[0])));
                if($cv) {
                  $sql = "SELECT cvterm_id FROM {cvterm} WHERE name = :name AND cv_id = :cv_id";
                  $results = chado_query($sql, array(':name' => $type, ':cv_id' => $cv->cv_id));
                  while ($obj = $results->fetchObject()) {
                    $terms[] = $obj;
                  }
                }
                else {
                  $cv = $tmp[0];
                }
              }
              if (count($terms) != 1) {
                $message = "The term '@type' can not be found.";
                $token = array('@type' => $type);
                if ($cv) {
                  $message =  "The term '@type' can not be found within the vocabulary '@vocab'.";
                  $token['@vocab'] = $cv;
                }
                form_set_error(
                    "instance][settings][relationships][relationship_types",
                    t($message, $token)
                    );
              }
            }
            else if (count($terms) > 1) {
              // If a type matches more than one term, parse it as 'vocabulary|cvterm' and try again
              if (strpos($type, '|')) {
                $tmp = explode('|', $type, 2);
                $type = trim($tmp[1]);
                $cv = tripal_get_cv(array('name' => trim($tmp[0])));
                if ($cv) {
                  $sql = "SELECT cvterm_id FROM {cvterm} WHERE name = :name AND cv_id = :cv_id";
                  $results = chado_query($sql, array(':name' => $type, ':cv_id' => $cv->cv_id));
                  while ($obj = $results->fetchObject()) {
                    $terms[] = $obj;
                  }
                }
              }
              if(count($terms) != 1) {
                form_set_error(
                    "instance][settings][relationships][relationship_types",
                    t("The term '@type' matches more than one term. Please specify its vocabulary in
                  the format of 'vocabulary|@type'.", array('@type' => $type))
                    );
              }
            }
          }
        }

        // For option2: Make sure the parent term is a valid cvterm
        if ($option2) {
          $cv_id = $settings['option2_vocab'];
          $supertype = $settings['option2_parent'];
          $term = tripal_get_cvterm(array(
            'name' => trim($supertype),
            'cv_id' => $cv_id,
          ));
          // Tripal cv autocomplete also allow cvterm synonyms, if the parent term doesn't match
          // a cvterm, try cvtermsynonym
          if (!$term) {
            $synonym = tripal_get_cvterm(
                array(
                  'synonym' => array(
                    'name' => trim($supertype),
                  )
                )
                );
            if ($synonym && $synonym->cv_id->cv_id == $cv_id) {
              $term = $synonym;
            }
          }
          if (!isset($term->cvterm_id)) {
            form_set_error(
                "instance][settings][relationships][option2_parent",
                t("The term '@type' is not a valid term for the vocabulary selected.", array('@type' => $supertype))
                );
          }
        }
  }

}
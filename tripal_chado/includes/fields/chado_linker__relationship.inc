<?php

class chado_linker__relationship extends TripalField {
  /**
   * @see TripalField::field_info()
   */
  function field_info() {
    return array(
      'label' => t('Relationships'),
      'description' => t('Relationships between records.'),
      'default_widget' => 'chado_linker__relationship_widget',
      'default_formatter' => 'chado_linker__relationship_formatter',
      'settings' => array(),
      'storage' => array(
        'type' => 'field_chado_storage',
        'module' => 'tripal_chado',
        'active' => TRUE
      ),
    );
  }

  /**
   * @see TripalField::attach_info()
   */
  function attach_info($entity_type, $bundle, $target) {
    $field_info = array();

    $table_name = $target['data_table'];
    $type_table = $target['type_table'];
    $type_field = $target['field'];
    $cv_id      = $target['cv_id'];
    $cvterm_id  = $target['cvterm_id'];

    // If the linker table does not exists then we don't want to add attach.
    $rel_table = $table_name . '_relationship';
    if (!chado_table_exists($rel_table)) {
      return $field_info;
    }

    $schema = chado_get_schema($rel_table);
    $pkey = $schema['primary key'][0];

    // Initialize the field array.
    $field_info = array(
      'field_name' => $table_name . '__relationship',
      'field_type' => 'chado_linker__relationship',
      'widget_type' => 'chado_linker__relationship_widget',
      'widget_settings' => array('display_label' => 1),
      'description' => 'A generic field for displaying relationships between data types',
      'label' => 'Relationships',
      'is_required' => 0,
      'cardinality' => FIELD_CARDINALITY_UNLIMITED,
      'storage' => 'field_chado_storage',
      'field_settings' => array(
        'chado_table' => $rel_table,
        'chado_column' => $pkey,
        'base_table' => $table_name,
        'semantic_web' => array(
          'name' => 'relationship',
          'accession' => 'relationship',
          'ns' => 'local',
          'nsurl' => '',
        ),
      ),
    );

    return $field_info;

  }
  /**
   * @see TripalField::widget_info()
   */
  function widget_info() {
    return array(
      'label' => t('Relationship Settings'),
      'field types' => array('chado_linker__relationship')
    );
  }
  /**
   * @see TripalField::formatter_info()
   */
  function formatter_info() {
    return array(
      'label' => t('Relationships'),
      'field types' => array('chado_linker__relationship'),
      'settings' => array(
      ),
    );
  }

  /**
   * @see TripalField::formatter_view()
   */
  function formatter_view(&$element, $entity_type, $entity,
      $field, $instance, $langcode, $items, $display) {

    // Get the settings
    $settings = $display['settings'];

    $rows = array();
    $headers = array('Subject' ,'Type', 'Object');
    $headers = array('Relationship');

    foreach ($items as $delta => $item) {
      if (!$item['value']) {
        continue;
      }
      $subject_name = $item['value']['subject']['name'];
      $subject_type = $item['value']['subject']['type'];
      $object_name = $item['value']['object']['name'];
      $object_type = $item['value']['object']['type'];
      $phrase = $item['value']['phrase'];

      // Handle some special cases.
      // For mRNA objects we don't want to show the CDS, exons, 5' UTR, etc.
      // we want to show the parent gene and the protein.
      if ($object_type == 'mRNA' and ($subject_type != 'polypeptide')) {
        continue;
      }
      if ($subject_type == 'mRNA' and ($object_type != 'gene')) {
        continue;
      }

      $phrase = preg_replace("/$subject_type/", "<b>$subject_type</b>", $phrase);
      $phrase = preg_replace("/$object_type/", "<b>$object_type</b>", $phrase);

      if (array_key_exists('entity_id', $item['value']['object'])) {
        $object_entity_id = $item['value']['object']['entity_id'];
        if ($object_entity_id != $entity->id) {
          $link = l($object_name, 'bio_data/' . $object_entity_id);
          $phrase = preg_replace("/$object_name/", $link, $phrase);
        }
      }
      if (array_key_exists('entity_id', $item['value']['subject'])) {
        $subject_entity_id = $item['value']['subject']['entity_id'];
        if ($subject_entity_id != $entity->id) {
          $link = l($subject_name, 'bio_data/' . $subject_entity_id);
          $phrase = preg_replace("/$subject_name/", $link, $phrase);
        }
      }

      $rows[] = array($phrase);
    }


    // the $table array contains the headers and rows array as well as other
    // options for controlling the display of the table.  Additional
    // documentation can be found here:
    // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
    $table = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(
        'id' => 'tripal_feature-table-alignments',
        'class' => 'tripal-data-table'
      ),
      'sticky' => FALSE,
      'caption' => '',
      'colgroups' => array(),
      'empty' => 'There are no relationships',
    );

    // once we have our table array structure defined, we call Drupal's theme_table()
    // function to generate the table.
    $element[$delta] = array(
      '#type' => 'markup',
      '#markup' => theme_table($table),
    );
  }

  /**
   * @see TripalField::load()
   */
  function load($field, $entity, $details) {
    $settings = $field['settings'];

    $record = $details['record'];

    $field_name = $field['field_name'];
    $field_type = $field['type'];
    $field_table = $field['settings']['chado_table'];
    $field_column = $field['settings']['chado_column'];
    $base_table = $field['settings']['base_table'];

    // Set some defaults for the empty record.
    $entity->{$field_name}['und'][0] = array(
      'value' => array(),
    );

    if (!$record) {
      return;
    }

    // Expand the object to include the relationships.
    $options = array(
      'return_array' => 1,
      // we don't want to fully recurse we only need information about the
      // relationship type and the object and subject  (including feature type
      // and organism)
      'include_fk' => array(
        'type_id' => 1,
        'object_id' => array(
          'type_id' => 1,
          'organism_id' => 1
        ),
        'subject_id'  => array(
          'type_id' => 1,
          'organism_id' => 1
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
    $srelationships = $record->$rel_table->subject_id;
    $orelationships = $record->$rel_table->object_id;

    $i = 0;
    if ($orelationships) {
      foreach ($orelationships as $relationship) {
        $rel_acc = $relationship->type_id->dbxref_id->db_id->name . ':' . $relationship->type_id->dbxref_id->accession;
        $rel_type = $relationship->type_id->name;
        $verb = $this->get_rel_verb($rel_type);
        $subject_name = $relationship->subject_id->name;
        $subject_type = $relationship->subject_id->type_id->name;
        $object_name = $relationship->object_id->name;
        $object_type = $relationship->object_id->type_id->name;
        $entity->{$field_name}['und'][$i]['value'] = array(
          '@type' => $rel_acc,
          'subject' => array(
            '@type' => $relationship->subject_id->type_id->dbxref_id->db_id->name .
                ':' . $relationship->subject_id->type_id->dbxref_id->accession,
            'type' =>  $subject_type,
            'name' => $subject_name,
          ),
          'type' => $relationship->type_id->name,
          'object' => array(
            '@type' => $relationship->object_id->type_id->dbxref_id->db_id->name .
               ':' . $relationship->object_id->type_id->dbxref_id->accession,
            // TODO the entity_id should not be here.... wherre to put it.
            'entity_id' => $entity->id,
            'entity_type' => 'TripalEntity',
            'type' =>  $object_type,
            'name' => $object_name,
          )
        );
        if (property_exists($relationship->subject_id, 'uniquename')) {
          $subject_name = $relationship->subject_id->uniquename;
          $entity->{$field_name}['und'][$i]['value']['subject']['name'] = $subject_name;
        }
        if (property_exists($relationship->object_id, 'uniquename')) {
          $object_name = $relationship->object_id->uniquename;
          $entity->{$field_name}['und'][$i]['value']['object']['name'] = $object_name;
        }
        if (property_exists($relationship->subject_id, 'entity_id')) {
          $entity_id = $relationship->subject_id->entity_id;
          $entity->{$field_name}['und'][$i]['value']['subject']['entity_id'] = $entity_id;
          $entity->{$field_name}['und'][$i]['value']['subject']['entity_type'] = 'TripalEntity';
        }
        $rel_type_clean = lcfirst(preg_replace('/_/', ' ', $rel_type));
        $entity->{$field_name}['und'][$i]['value']['phrase'] = 'The ' . $subject_type . ', ' .
          $subject_name . ', ' . $verb . ' '  . $rel_type_clean . ' this '  .
          $object_type . '.';

        $i++;
      }
    }

    if ($srelationships) {
      foreach ($srelationships as $relationship) {
        $rel_acc = $relationship->type_id->dbxref_id->db_id->name . ':' . $relationship->type_id->dbxref_id->accession;
        $rel_type = $relationship->type_id->name;
        $verb = $this->get_rel_verb($rel_type);
        $subject_name = $relationship->subject_id->name;
        $subject_type = $relationship->subject_id->type_id->name;
        $object_name = $relationship->object_id->name;
        $object_type = $relationship->object_id->type_id->name;
        $entity->{$field_name}['und'][$i]['value'] = array(
          '@type' => $rel_acc,
          'subject' => array(
            '@type' => $relationship->subject_id->type_id->dbxref_id->db_id->name .
               ':' . $relationship->subject_id->type_id->dbxref_id->accession,
            'type' =>  $subject_type,
            'name' => $subject_name,
            'entity_id' => $entity->id,
            'entity_type' => 'TripalEntity',
          ),
          'type' => $relationship->type_id->name,
          'object' => array(
            '@type' => $relationship->object_id->type_id->dbxref_id->db_id->name .
               ':' . $relationship->object_id->type_id->dbxref_id->accession,
            'type' =>  $object_type,
            'name' => $object_name,
          )
        );
        if (property_exists($relationship->subject_id, 'uniquename')) {
          $subject_name = $relationship->subject_id->uniquename;
          $entity->{$field_name}['und'][$i]['value']['subject']['name'] = $subject_name;
        }
        if (property_exists($relationship->object_id, 'uniquename')) {
          $object_name = $relationship->object_id->uniquename;
          $entity->{$field_name}['und'][$i]['value']['object']['name'] = $object_name;
        }
        if (property_exists($relationship->object_id, 'entity_id')) {
          $entity_id = $relationship->object_id->entity_id;
          $entity->{$field_name}['und'][$i]['value']['object']['entity_id'] = $entity_id;
          $entity->{$field_name}['und'][$i]['value']['object']['entity_type'] = 'TripalEntity';
        }
        $rel_type_clean = lcfirst(preg_replace('/_/', ' ', $rel_type));
        $entity->{$field_name}['und'][$i]['value']['phrase'] = 'This  ' .
          $subject_type . ' ' . $verb . ' '  . $rel_type_clean . ' the '  .
          $object_type . ', ' . $object_name . '.';
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
  private function get_rel_verb($rel_type) {
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
}
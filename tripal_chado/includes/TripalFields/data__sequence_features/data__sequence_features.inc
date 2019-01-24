<?php

/**
 * @class
 * Purpose: Follows the feature relationships of a given feature,
 * and stores information about all child features.
 *
 */
class data__sequence_features extends ChadoField {

  public static $default_label = 'Child Features';
  public static $default_description = 'Gathers information about all subfeatures (mRNA, CDS, proteins) associated with a top-level feature (gene)';
  public static $default_widget = 'data__sequence_features_widget';
  public static $default_formatter = 'data__sequence_features_formatter';
  public static $module = 'tripal_chado';

  public static $default_settings = array(
    'storage' => 'field_chado_storage',
    'searchable_keys' => array(),
  );

//  public static $download_formatters = array(
//    'TripalTabDownloader',
//    'TripalCSVDownloader',
//  );

  public static $default_instance_settings = array(
    // The DATABASE name, as it appears in chado.db.  This also builds the link-out url.  In most cases this will simply be the CV name.  In some cases (EDAM) this will be the SUBONTOLOGY.
    'term_vocabulary' => 'data',
    // The name of the term.
    'term_name' => 'Sequence features',
    // The unique ID (i.e. accession) of the term.
    'term_accession' => '1255',
    // Set to TRUE if the site admin is not allowed to change the term
    // type, otherwise the admin can change the term mapped to a field.
    'term_fixed' => FALSE,
    // Indicates if this field should be automatically attached to display
    // or web services or if this field should be loaded separately. This
    // is convenient for speed.  Fields that are slow should for loading
    // should have auto_attach set to FALSE so tha their values can be
    // attached asynchronously.
    'auto_attach' => FALSE,
    // The table in Chado that the instance maps to.
    'chado_table' => '',
    // The column of the table in Chado where the value of the field comes from.
    'chado_column' => '',
    // The base table.
    'base_table' => '',
  );


  public static $no_ui = FALSE;
  public static $no_data = FALSE;

  /**
   * Load field.
   *
   * @see ChadoField::load()
   */
  public function load($entity) {

    parent::load($entity);
    $field = get_class($this);

    $parent = $entity->chado_record->feature_id;

    $children = $this->findChildFeatures($parent);
    if (empty($children)) {
      unset($entity->{$field});
      return;
    }
    $i = 0;
    foreach ($children as $child_id => $child) {
      $entity->{$field}['und'][$i]['value'] = $child;

      $i++;
    }

    return $entity;
  }

  /**
   * @see ChadoField::query()
   **/
  public function query($query, $condition) {
  }

  /**
   * @see ChadoField::queryOrder()
   **/
  public function queryOrder($query, $order) {
  }

  /**
   * @see ChadoField::elementInfo()
   **/
  public function elementInfo() {
    $field_term = $this->getFieldTermID();
    return array(
      $field_term => array(
        'operations' => array('eq', 'ne', 'contains', 'starts'),
        'sortable' => TRUE,
        'searchable' => TRUE,
      ),
    );
  }

  /**
   * For a given feature, find all child features.  For each child feature,
   * return:
   *  - the type name
   *  - annotation names in feature_cvterm
   *  - featureloc info, including source feature name.
   *
   * @param string $feature_id
   *   Chado feature.feature_id.
   *
   * @return array
   */
  private function findChildFeatures(string $feature_id) {
    $this_children = [];

    $prev_db = chado_set_active('chado');

    $query = db_select('chado.feature_relationship', 'fr')
      ->fields('fr')
      ->condition('object_id', $feature_id)
      ->execute()
      ->fetchAll();
    chado_set_active($prev_db);

    foreach ($query as $child) {
      $child_id = $child->subject_id;
      // If any additional fields want to use this
      // field's data, expand the desired table below.
      $feature = chado_generate_var('feature', ['feature_id' => $child_id]);
      $feature = chado_expand_var($feature, 'field', 'feature.residues');
      $feature = chado_expand_var($feature, 'table', 'featureloc');
      $feature = chado_expand_var($feature, 'table', 'featureprop');
      $feature = chado_expand_var($feature, 'table', 'feature_cvterm');

      $this_children[$child_id]['info'] = $feature;

      $grand_children = $this->findChildFeatures($child->subject_id);
      if (!empty($grand_children)) {
        $this_children[$child_id]['children'] = $grand_children;
      }
    }
    return $this_children;
  }

}

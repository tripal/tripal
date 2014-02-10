<?php
/**
 * @file
 * Contains functions related to theme-ing including preprocess hooks
 */

 /**
 * Implements hook_preprocess_tripal_stock_relationships() which is the preprocess
 * hook for the tripal_stock_relationships template
 *
 * @ingroup tripal_stock
 */
function tripal_stock_preprocess_tripal_stock_relationships(&$variables) {
  // we want to provide a new variable that contains the matched stocks.
  $stock = $variables['node']->stock;

  // expand the stock object to include the stock relationships.
  $options = array(
    'return_array' => 1,
    'order_by' => array('rank' => 'ASC'),
    // we don't want to fully recurse we only need information about the
    // relationship type and the object and subject stocks (including stock type
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
  $stock = chado_expand_var($stock, 'table', 'stock_relationship', $options);

  // get the subject relationships
  $srelationships = $stock->stock_relationship->subject_id;
  $orelationships = $stock->stock_relationship->object_id;

  // combine both object and subject relationshisp into a single array
  $relationships = array();
  $relationships['object'] = array();
  $relationships['subject'] = array();

  // iterate through the object relationships
  if ($orelationships) {
    foreach ($orelationships as $relationship) {
      $rel = new stdClass();
      $rel->record = $relationship;

      // get the relationship and child types
      $rel_type = t(preg_replace('/_/', " ", $relationship->type_id->name));
      $child_type = $relationship->subject_id->type_id->name;

      // get the node id of the subject
      $sql = "SELECT nid FROM {chado_stock} WHERE stock_id = :stock_id";
      $n = db_query($sql, array(':stock_id' => $relationship->subject_id->stock_id))->fetchObject();
      if ($n) {
        $rel->record->nid = $n->nid;
      }

      if (!array_key_exists($rel_type, $relationships['object'])) {
        $relationships['object'][$rel_type] = array();
      }
      if (!array_key_exists($child_type, $relationships['object'][$rel_type])) {
        $relationships['object'][$rel_type][$child_type] = array();
      }
      $relationships['object'][$rel_type][$child_type][] = $rel;
    }
  }

  // now add in the subject relationships
  if ($srelationships) {
    foreach ($srelationships as $relationship) {
      $rel = new stdClass();
      $rel->record = $relationship;
      $rel_type = t(preg_replace('/_/', " ", $relationship->type_id->name));
      $parent_type = $relationship->object_id->type_id->name;

      // get the node id of the subject
      $sql = "SELECT nid FROM {chado_stock} WHERE stock_id = :stock_id";
      $n = db_query($sql, array(':stock_id' => $relationship->object_id->stock_id))->fetchObject();
      if ($n) {
        $rel->record->nid = $n->nid;
      }

      if (!array_key_exists($rel_type, $relationships['subject'])) {
        $relationships['subject'][$rel_type] = array();
      }
      if (!array_key_exists($parent_type, $relationships['subject'][$rel_type])) {
        $relationships['subject'][$rel_type][$parent_type] = array();
      }
      $relationships['subject'][$rel_type][$parent_type][] = $rel;
    }
  }
  $stock->all_relationships = $relationships;

}
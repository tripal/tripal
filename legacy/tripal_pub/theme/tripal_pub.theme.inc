<?php
/**
 *
 *
 * @ingroup tripal_pub
 */
function tripal_pub_preprocess_tripal_pub_relationships(&$variables) {
   $pub = $variables['node']->pub;
  
   // expand the pub object to include the pub relationships.
  $options = array(
    'return_array' => 1,
    // we don't want to fully recurse we only need information about the
    // relationship type and the object and subject pubs (including pub type)
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
  $pub = chado_expand_var($pub, 'table', 'pub_relationship', $options);

  // get the subject relationships
  $srelationships = $pub->pub_relationship->subject_id;
  $orelationships = $pub->pub_relationship->object_id;

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
      $sql = "SELECT nid FROM {chado_pub} WHERE pub_id = :pub_id";
      $n = db_query($sql, array(':pub_id' => $relationship->subject_id->pub_id))->fetchObject();
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
      $sql = "SELECT nid FROM {chado_pub} WHERE pub_id = :pub_id";
      $n = db_query($sql, array(':pub_id' => $relationship->object_id->pub_id))->fetchObject();
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
  $pub->all_relationships = $relationships;
}
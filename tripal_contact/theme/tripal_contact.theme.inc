<?php
/**
 *
 *
 * @ingroup tripal_contact
 */
function tripal_contact_preprocess_tripal_contact_relationships(&$variables) {
  $contact = $variables['node']->contact;
  
   // expand the contact object to include the contact relationships.
  $options = array(
    'return_array' => 1,
    // we don't want to fully recurse we only need information about the
    // relationship type and the object and subject contacts (including contact type)
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
  $contact = chado_expand_var($contact, 'table', 'contact_relationship', $options);

  // get the subject relationships
  $srelationships = $contact->contact_relationship->subject_id;
  $orelationships = $contact->contact_relationship->object_id;

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
      $sql = "SELECT nid FROM {chado_contact} WHERE contact_id = :contact_id";
      $n = db_query($sql, array(':contact_id' => $relationship->subject_id->contact_id))->fetchObject();
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
      $sql = "SELECT nid FROM {chado_contact} WHERE contact_id = :contact_id";
      $n = db_query($sql, array(':contact_id' => $relationship->object_id->contact_id))->fetchObject();
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
  $contact->all_relationships = $relationships;
}
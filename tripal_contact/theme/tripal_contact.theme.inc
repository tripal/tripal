<?php
/**
 *
 *
 * @ingroup tripal_contact
 */
function tripal_contact_preprocess_tripal_contact_relationships(&$variables) {
  // we want to provide a new variable that contains the matched contacts.
  $contact = $variables['node']->contact;

  // normally we would use tripal_core_expand_chado_vars to expand our
  // organism object and add in the relationships, however whan a large
  // number of relationships are present this significantly slows the
  // query, therefore we will manually perform the query
  $sql = "
    SELECT C.name, C.contact_id, CP.nid, CVT.name as rel_type
    FROM {contact_relationship} PR
      INNER JOIN {contact} C              ON PR.object_id  = C.contact_id
      INNER JOIN {cvterm} CVT             ON PR.type_id    = CVT.cvterm_id
      LEFT  JOIN public.chado_contact CP  ON C.contact_id  = CP.contact_id
    WHERE PR.subject_id = :contact_id
  ";
  $as_subject = chado_query($sql, array(':contact_id' => $contact->contact_id));
  $sql = "
    SELECT C.name, C.contact_id, CP.nid, CVT.name as rel_type
    FROM {contact_relationship} PR
      INNER JOIN {contact} C              ON PR.subject_id = C.contact_id
      INNER JOIN {cvterm} CVT             ON PR.type_id    = CVT.cvterm_id
      LEFT  JOIN public.chado_contact CP  ON C.contact_id  = CP.contact_id
    WHERE PR.object_id = :contact_id
  ";
  $as_object = chado_query($sql, array(':contact_id' => $contact->contact_id));

  // combine both object and subject relationshisp into a single array
  $relationships = array();
  $relationships['object'] = array();
  $relationships['subject'] = array();

  // iterate through the object relationships
  while ($relationship = $as_object->fetchObject()) {

    // get the relationship and child types
    $rel_type = t(preg_replace('/_/', " ", $relationship->rel_type));
    $sub_type = t(preg_replace('/_/', " ", $relationship->sub_type));

    if (!array_key_exists($rel_type, $relationships['object'])) {
      $relationships['object'][$rel_type] = array();
    }
    if (!array_key_exists($sub_type, $relationships['object'][$rel_type])) {
      $relationships['object'][$rel_type][$sub_type] = array();
    }
    $relationships['object'][$rel_type][$sub_type][] = $relationship;
  }

  // now add in the subject relationships
  while ($relationship = $as_subject->fetchObject()) {

    // get the relationship and child types
    $rel_type = t(preg_replace('/_/', " ", $relationship->rel_type));
    $obj_type = t(preg_replace('/_/', " ", $relationship->obj_type));

    if (!array_key_exists($rel_type, $relationships['subject'])) {
      $relationships['subject'][$rel_type] = array();
    }
    if (!array_key_exists($obj_type, $relationships['subject'][$rel_type])) {
      $relationships['subject'][$rel_type][$obj_type] = array();
    }
    $relationships['subject'][$rel_type][$obj_type][] = $relationship;
  }

  $contact->all_relationships = $relationships;
}
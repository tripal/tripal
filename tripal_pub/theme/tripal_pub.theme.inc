<?php
/**
 *
 *
 * @ingroup tripal_pub
 */
function tripal_pub_preprocess_tripal_pub_relationships(&$variables) {
  // we want to provide a new variable that contains the matched pubs.
  $pub = $variables['node']->pub;

  // normally we would use tripal_core_expand_chado_vars to expand our
  // organism object and add in the relationships, however whan a large
  // number of relationships are present this significantly slows the
  // query, therefore we will manually perform the query
  $sql = "
    SELECT P.title, P.pub_id, CP.nid, CVT.name as rel_type
    FROM {pub_relationship} PR
      INNER JOIN {pub} P            ON PR.object_id = P.pub_id
      INNER JOIN {cvterm} CVT       ON PR.type_id   = CVT.cvterm_id
      LEFT JOIN public.chado_pub CP ON P.pub_id     = CP.pub_id
    WHERE PR.subject_id = :subject_id
  ";
  $as_subject = chado_query($sql, array(':subject_id' => $pub->pub_id));
  $sql = "
    SELECT P.title, P.pub_id, CP.nid, CVT.name as rel_type
    FROM {pub_relationship} PR
      INNER JOIN {pub} P            ON PR.subject_id = P.pub_id
      INNER JOIN {cvterm} CVT       ON PR.type_id    = CVT.cvterm_id
      LEFT JOIN public.chado_pub CP ON P.pub_id      = CP.pub_id
    WHERE PR.object_id = :object_id
  ";
  $as_object = chado_query($sql, array(':object_id' => $pub->pub_id));

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

  $pub->all_relationships = $relationships;
}
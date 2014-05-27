<?php
/**
 * Implementation of hook_form().
 *
 *  This form takes the ND Geolocation Title information and description from the user.
 *
 *  @parm $node
 *    The initialized node
 *
 *  @parm $form_state
 *    The state of the form, that has the user entered information that is neccessary for adding
 *    information to the nd_geolocation
 *
 *  @return $form
 *    An array as described by the Drupal Form API
 *
 *
 * @ingroup tripal_nd_geolocation
 */
function chado_nd_geolocation_form(&$node, $form_state) {
  $form = array();
  $nd_geolocation = $node->nd_geolocation;
  $nd_geolocation_latitude = $nd_geolocation->latitude;
  $nd_geolocation_longitude = $nd_geolocation->longitude;
  $nd_geolocation_altitude = $nd_geolocation->altitude;
  $nd_geolocation_geodetic_datum = $nd_geolocation->geodetic_datum;

  // keep track of the nd_geolocation id if we have.  If we do have one then
  // this is an update as opposed to an insert.
  $form['nd_geolocation_id'] = array(
      '#type' => 'value',
      '#value' => $nd_geolocation->nd_geolocation_id,
  );
  
  // use nd_geolocation.description as the node title
  $form['title']= array(
    '#type'          => 'textfield',
    '#title'         => t('ND Geolocation Description'),
    '#description'   => t('A brief description of the nd_geolocation.'),
    '#required'      => TRUE,
    '#default_value' => $node->title,
    '#weight'        => 1
  );

  $form['nd_geolocation_latitude']= array(
      '#type'          => 'textfield',
      '#title'         => t('ND Geolocation Latitude'),
      '#description'   => t('Latitude of the nd_geolocation'),
      '#required'      => FALSE,
      '#default_value' => $nd_geolocation_latitude,
      '#weight'        => 2
  );
  
  $form['nd_geolocation_longitude']= array(
      '#type'          => 'textfield',
      '#title'         => t('ND Geolocation Longitude'),
      '#description'   => t('Longitude of the nd_geolocation'),
      '#required'      => FALSE,
      '#default_value' => $nd_geolocation_longitude,
      '#weight'        => 3
  );
  
  $form['nd_geolocation_altitude']= array(
      '#type'          => 'textfield',
      '#title'         => t('ND Geolocation Altitude'),
      '#description'   => t('Altitude of the nd_geolocation'),
      '#required'      => FALSE,
      '#default_value' => $nd_geolocation_altitude,
      '#weight'        => 4
  );
  
  $form['nd_geolocation_geodetic_datum']= array(
      '#type'          => 'textfield',
      '#title'         => t('ND Geolocation Geodetic Datum'),
      '#description'   => t('Geodetic_datum of the nd_geolocation'),
      '#required'      => FALSE,
      '#default_value' => $nd_geolocation_geodetic_datum,
      '#weight'        => 5
  );
  return $form;

}

/**
 *  validates submission of form when adding or updating a nd_geolocation node
 *
 * @ingroup tripal_nd_geolocation
 */
function chado_nd_geolocation_validate($node) {
  // check to make sure latitude, longitude, and altitude are numbers or null
  $latitude = trim($node->nd_geolocation_latitude);
  $longitude = trim($node->nd_geolocation_longitude);
  $altitude = trim($node->nd_geolocation_altitude);
  if ($latitude && !is_numeric($latitude)) {
    form_set_error('nd_geolocation_latitude', t('Latitude shoulbe a number'));
  } 
  if ($longitude && !is_numeric($longitude)) {
    form_set_error('nd_geolocation_longitude', t('Longitude shoulbe a number'));
  }
  if ($altitude && !is_numeric($altitude)) {
    form_set_error('nd_geolocation_altitude', t('Altitude shoulbe a number'));
  }

}
/**
 * Implementation of hook_insert().
 *
 *  @parm $node
 *    Then node that has the information stored within, accessed given the nid
 *
 *
 * @ingroup tripal_nd_geolocation
 */
function chado_nd_geolocation_insert($node) {
  if ($node->nd_geolocation_id) {
    $nd_geolocation['nd_geolocation_id'] = $node->nd_geolocation_id;
  }
  else {
    $description = trim($node->title);
    $latitude = trim($node->nd_geolocation_latitude);
    $longitude = trim($node->nd_geolocation_longitude);
    $altitude = trim($node->nd_geolocation_altitude);
    $geodetic_datum = trim($node->nd_geolocation_geodetic_datum);
    $values = array(
        'description' => $description,
        'latitude' => $latitude ? $latitude : 'NULL',
        'longitude' => $longitude ? $longitude : 'NULL',
        'altitude' => $altitude ? $altitude : 'NULL',
        'geodetic_datum' => $geodetic_datum ? $geodetic_datum : '',
    );
    $nd_geolocation = tripal_core_chado_insert('nd_geolocation', $values);
  }

  if ($nd_geolocation) {
    // make sure the entry for this feature doesn't already exist in the chado_nd_geolocation table
    // if it doesn't exist then we want to add it.
    $nd_geolocation_id = chado_get_id_for_node('nd_geolocation', $node) ;
    if (!$nd_geolocation_id) {
      // next add the item to the drupal table
      $sql = "INSERT INTO {chado_nd_geolocation} (nid, vid, nd_geolocation_id) ".
          "VALUES (%d, %d, %d)";
      db_query($sql, $node->nid, $node->vid, $nd_geolocation['nd_geolocation_id']);
    }
  }
  else {
    drupal_set_message(t('Unable to add nd_geolocation.', 'warning'));
    watchdog('tripal_nd_geolocation', 'Insert nd_geolocation: Unable to create nd_geolocation where values: %values',
    array('%values' => print_r($values, TRUE)), WATCHDOG_WARNING);
  }
}

/**
 *
 * Implementation of hook_delete().
 *
 * @param $node
 * The node which is to be deleted, only chado nd_geolocation and chado_nd_geolocation need to be dealt with
 * since the drupal node is deleted automagically
 *
 *
 * @ingroup tripal_nd_geolocation
 */
function chado_nd_geolocation_delete($node) {

  $nd_geolocation_id = chado_get_id_for_node('nd_geolocation', $node);

  // if we don't have a nd_geolocation id for this node then this isn't a node of
  // type chado_nd_geolocation or the entry in the chado_nd_geolocation table was lost.
  if (!$nd_geolocation_id) {
    return;
  }

  // Remove data from {chado_nd_geolocation}, {node} and {node_revisions} tables of
  // drupal database
  $sql_del = "DELETE FROM {chado_nd_geolocation} ".
      "WHERE nid = %d ".
      "AND vid = %d";
  db_query($sql_del, $node->nid, $node->vid);
  $sql_del = "DELETE FROM {node_revisions} ".
      "WHERE nid = %d ".
      "AND vid = %d";
  db_query($sql_del, $node->nid, $node->vid);
  $sql_del = "DELETE FROM {node} ".
      "WHERE nid = %d ".
      "AND vid = %d";
  db_query($sql_del, $node->nid, $node->vid);


  // Remove data from nd_geolocation and nd_geolocationprop tables of chado database as well
  chado_query("DELETE FROM {nd_geolocationprop} WHERE nd_geolocation_id = %d", $eimage_id);
  chado_query("DELETE FROM {nd_geolocation} WHERE nd_geolocation_id = %d", $nd_geolocation_id);
}

/**
 * Implements hook_update().
 *
 * @param $node
 *  The node which is to have its containing information updated when the user modifies information
 *  pertaining to the specific nd_geolocation
 *
 *
 * @ingroup tripal_nd_geolocation
 */
function chado_nd_geolocation_update($node) {
  if ($node->revision) {
    // there is no way to handle revisions in Chado but leave
    // this here just to make not we've addressed it.
  }

  // update the nd_geolocation and the description
  $nd_geolocation_id = chado_get_id_for_node('nd_geolocation', $node) ;
  $match = array(
      'nd_geolocation_id' => $nd_geolocation_id,
  );
  $description = trim($node->title);
  $latitude = trim($node->nd_geolocation_latitude);
  $longitude = trim($node->nd_geolocation_longitude);
  $altitude = trim($node->nd_geolocation_altitude);
  $geodetic_datum = trim($node->nd_geolocation_geodetic_datum);
  $values = array(
      'description' => $description,
      'latitude' => $latitude ? $latitude : 'NULL',
      'longitude' => $longitude ? $longitude : 'NULL',
      'altitude' => $altitude ? $altitude : 'NULL',
      'geodetic_datum' => $geodetic_datum ? $geodetic_datum : '',
  );
  $status = tripal_core_chado_update('nd_geolocation', $match, $values);

}

/**
 * Implementation of node_load().
 *
 * @param $node
 *   The node that is to have its containing information loaded
 *
 * @return $node
 *   The node, containing the loaded nd_geolocation with the current nid
 *
 *
 * @ingroup tripal_nd_geolocation
 */
function chado_nd_geolocation_load($node) {

  // get the feature details from chado
  $nd_geolocation_id = chado_get_id_for_node('nd_geolocation', $node);

  $values = array('nd_geolocation_id' => $nd_geolocation_id);
  $nd_geolocation = tripal_core_generate_chado_var('nd_geolocation', $values);

  $additions = new stdClass();
  $additions->nd_geolocation = $nd_geolocation;
  return $additions;

}
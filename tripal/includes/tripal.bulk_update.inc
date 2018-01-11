<?php

/**
 * Updates all existing url aliases and titles for an entity.
 *
 * @param $bundle_id
 * @param $update
 * @param $type
 */
function tripal_update_all_urls_and_titles($bundle_id, $update, $type) {
  // Load all the entity_ids.
  $entity_table = 'chado_'.$bundle_id;
  $entities = db_select($entity_table, 'e')
    ->fields('e', array('entity_id'))
    ->execute()->fetchAll();
  $num_entities = count($entities);
  // Parse the $update variable for tokens and load those tokens.
  preg_match_all("/\[[^\]]*\]/", $update, $bundle_tokens);
  // Get the queue so we can add to it.  Use a
  // descriptive name. It's ok if it doesn't exist yet.
  $queue = DrupalQueue::get('entityQueue');
  // Push all the items into the queue, one at a time.
  // You can push any data in with (arrays, objects, etc).
  foreach ($entities as $entity) {
    $queue->createItem($entity);
  }
  $fields = array();
  foreach ($bundle_tokens as $bundle_token) {
    foreach ($bundle_token as $token) {
      $string = str_replace(array('[', ']'), '', $token);
      $field_array = field_info_field($string);
      $fields[] = $field_array['id'];
    }
  }
  $i = 0;
  // Pull items out one at a time.
  while ($entity = $queue->claimItem()) {
    $arg = tripal_load_entity('TripalEntity', [$entity->data->entity_id], FALSE, $fields);
    if ($type == 'alias') {
      if (!empty($arg)) {
        if (is_array($arg)) {
          $ent = reset($arg);
        }
        // Get the entity controller and clear the cache if requested (default).
        $ec = entity_get_controller('TripalEntity');
        $ec->setAlias($ent, $update);
      }
    }
    elseif ($type == 'title') {
      if (!empty($arg)) {
        if (is_array($arg)) {
          $ent = reset($arg);
        }

        $ec = entity_get_controller('TripalEntity');
        $ec->setTitle($ent, $update);
      }
    }
    $i++;
    // Check if 50 items have been updated, if so print message.
    if ($i < $num_entities) {
      print $i . "/" . $num_entities . " entities have been updated.\r";
    }
    // Good, we succeeded.  Delete the item as it is no longer needed.
    $queue->deleteItem($entity);
  }
}

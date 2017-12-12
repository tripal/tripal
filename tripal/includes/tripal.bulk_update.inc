<?php

/**
 * Updates all existing url aliases for an entity.
 *
 * @param $bundle_id
 * @param $update
 * @param $type
 */
function tripal_update_all($bundle_id, $update, $type) {
  // Load all the entity_ids.
  $entity_table = 'chado_'.$bundle_id;
  $result = db_select($entity_table, 'e')
    ->fields('e', array('entity_id'))
    ->execute();
  $num_entities = $result->rowCount();
  $entities = $result->fetchAll();

  // Parse the $update variable for tokens and load those tokens.
  preg_match_all("/\[[^\]]*\]/", $update, $bundle_tokens);

  // Get field ids for each token
  $field_ids = [];
  foreach ($bundle_tokens as $index => $token) {
    // Ignore the first entry since it's just the matched string (subject)
    if ($index > 0) {
      $field = field_info_instance('TripalEntity', $bundle_tokens[1], $bundle_id);
      $field_ids[] = $field['field_id'];
    }
  }

  $i = 0;
  // Pull items out one at a time.
  foreach ($entities as $entity) {
    $arg = tripal_load_entity('TripalEntity', [$entity->entity_id], FALSE, $field_ids);
    if ($type == 'alias') {
      if (!empty($arg)) {
        if (is_array($arg)) {
          $ent = reset($arg);
        }
        // Get the entity controller and clear the cache if requested (default).
        $ec = entity_get_controller('TripalEntity');
        $ec->setAlias($ent, $update);
        $ec->resetCache();
      }
    }
    elseif ($type == 'title') {
      if (!empty($arg)) {
        if (is_array($arg)) {
          $ent = reset($arg);
        }

        $ec = entity_get_controller('TripalEntity');
        $ec->setTitle($ent, $update);
        $ec->resetCache();
      }
    }
    $i++;
    // Check if 50 items have been updated, if so print message.
    if ($i < $num_entities) {
      $mem = number_format(memory_get_usage());
      print "$i/$num_entities entities have been updated. Memory usage: $mem Bytes\r";
    }
  }
}
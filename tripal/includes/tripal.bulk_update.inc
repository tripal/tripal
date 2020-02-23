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
  $entity_table = 'chado_' . $bundle_id;
  $entities = db_select($entity_table, 'e')
    ->fields('e', ['entity_id'])
    ->orderBy('entity_id', 'ASC')
    ->execute();
  $num_entities = $entities->rowCount();

  // Parse the $update variable for tokens and load those tokens.
  preg_match_all("/\[.*?\]/", $update, $bundle_tokens);

  $fields = [];
  foreach ($bundle_tokens as $bundle_token) {
    $elements = explode(',', $bundle_token[0]);
    $field_name = array_shift($elements);
    $field_array = field_info_field($field_name);
    $fields[] = $field_array['id'];
  }

  $i = 1;
  // Pull items out one at a time.
  while ($entity = $entities->fetchObject()) {
    $arg = tripal_load_entity('TripalEntity', [$entity->entity_id], FALSE, $fields);
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
    // Check if 50 items have been updated, if so print message.
    if ($i <= $num_entities) {
      $mem = number_format(memory_get_usage());
      print $i . "/" . $num_entities . " entities have been updated. Memory usage: $mem bytes.\r";
    }
    $i++;
  }

  // To speed up bulk alias updating the following function was skipped
  // over. We'll add it back here to complete the process.
  if ($type == 'alias') {
    drupal_path_alias_whitelist_rebuild();
  }

  print "\nDone.";
}

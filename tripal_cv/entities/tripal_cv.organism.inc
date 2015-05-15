<?php
/**
 *
 *
 */
class TrpOrganismController extends DrupalDefaultEntityController {

  /**
   *
   * @param $ids
   *   An array of entity IDs, or FALSE to load all entities.
   * @param $conditions
   *   DEPRECATED. An array of conditions. Keys are field names on the entity's
   *   base table.Values will be compared for equality. All the comparisons will
   *   be ANDed together. This parameter is deprecated; use an EntityFieldQuery
   *   instead.
   * @return multitype:An
   */
  public function load($ids = array(), $conditions = array()) {
    $entities = array();
    $queried_entities = array();

    $values = array('organism_id' => $ids);
    $options = array('return_array' => TRUE);
    $organisms = chado_generate_var('organism', $values, $options);
    $organisms = chado_expand_var($organisms, 'field', 'organism.comment');
    foreach ($organisms as $result) {
      $organism = new stdClass();
      $organism->internal_id = $result->organism_id;

      // Add the names by which this feature is known.
      $names = array();
      $names[] = $result->genus . ' ' . $result->species;
      $names[] = $result->common_name;
      $names[] = $result->abbreviation;
      $organism->name = array_unique($names);

      // Add in properties.

      // Add in the expand array.
      $expand = array();
      $organism->expand = $expand;

      // Add in the embeded elements.
      $embedded = array();
      $organism->embedded = $embedded;


      // Add in site-specific properties.
      $organism->genus = $result->genus;
      $organism->species = $result->species;
      $organism->description = $result->comment;

      // Add in the phylogeny if the NCBI taxonomy is loaded.


      $queried_entities[$organism->internal_id] = $organism;
    }

    // Pass all entities loaded from the database through $this->attachLoad(),
    // which attaches fields (if supported by the entity type) and calls the
    // entity type specific load callback, for example hook_node_load().
    if (!empty($queried_entities)) {
      //      $this->attachLoad($queried_entities);
      $entities += $queried_entities;
    }

    // Ensure that the returned array is ordered the same as the original
    // $ids array if this was passed in and remove any invalid ids.
    return $entities;
  }
}
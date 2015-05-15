<?php
/**
 *
 *
 */
class TrpFeatureController extends DrupalDefaultEntityController {

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

    $values = array('feature_id' => $ids);
    $options = array('return_array' => TRUE);
    $features = chado_generate_var('feature', $values, $options);
    foreach ($features as $result) {
      $feature = new stdClass();
      $feature->internal_id = $result->feature_id;
      $feature->type_id = $result->type_id->dbxref_id->db_id->name . ':' . $result->type_id->dbxref_id->accession;

      // Add the names by which this feature is known.
      $names = array();
      $names[] = $result->name;
      $names[] = $result->uniquename;
      $feature->name = array_unique($names);

      // Add in the synonyms.
      $sql = "
        SELECT *
        FROM {feature_synonym} FS
          INNER JOIN {synonym} S ON FS.synonym_id = S.synonym_id
        WHERE FS.feature_id = :feature_id
      ";
      $synonym_results = chado_query($sql, array(':feature_id' => $result->feature_id));
      $synonyms = array();
      while ($synonym = $synonym_results->fetchObject()) {
        $synonyms[] = $synonym->name;
      }
      if (count($synonyms) > 0) {
        $feature->synonyms = $synonyms;
      }

      // Add in database cross-references.
      $xrefs = array();
      if ($result->dbxref_id) {
        $xrefs[] = $result->dbxref_id->db_id->name . ':' . $result->dbxref_id->accession;
      }
      $sql = "
        SELECT DB.name as namespace, DBX.accession
        FROM {feature_dbxref} FDBX
          INNER JOIN {dbxref} DBX ON DBX.dbxref_id = FDBX.dbxref_id
          INNER JOIN {db} DB      ON DB.db_id      = DBX.db_id
        WHERE FDBX.feature_id = :feature_id
      ";
      $dbxref_results = chado_query($sql, array(':feature_id' => $result->feature_id));

      while ($dbxref = $dbxref_results->fetchObject()) {
        if ($dbxref->namespace != 'GFF_source') {
          $xrefs[] = $dbxref->namespace . ':' . $dbxref->accession;
        }
      }
      if (count($xrefs) > 0) {
        $feature->xref_id = $xrefs;
      }

      // Add in the expand array
      $expand = array();
      $expand[] = 'chado:organism';
      $feature->expand = $expand;

      // Add in the embeded elements
      $embedded = array();
      $embedded['chado:organism'] = entity_load('trp_organism', array($result->organism_id->organism_id));
      $feature->embedded = $embedded;

      $queried_entities[$feature->internal_id] = $feature;
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
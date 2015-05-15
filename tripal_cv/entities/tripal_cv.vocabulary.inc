<?php
/**
 *
 *
 */
class TrpVocabularyController extends DrupalDefaultEntityController {

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

    // Get the list of records that match the loading criteria
    $sql = "
      SELECT CVDB.cv_id, CVDB.name, CVDB.namespace, DB.url, DB.urlprefix
      FROM {cv_db} CVDB
        INNER JOIN {db} DB on DB.db_id = CVDB.db_id
        LEFT JOIN {cvprop} CVP on CVP.cv_id = CVDB.cv_id
    ";
    $where = '';
    if ($ids) {
      $where .= ' CVDB.cv_id IN (:ids)';
    }
    if ($where) {
      $sql .= "WHERE $where";
    }
    $sql .= "ORDER BY CVDB.name ";
    $sql .= "LIMIT 10 OFFSET 0 ";

    $results = chado_query($sql, array(':ids' => $ids));
    while ($result = $results->fetchObject()) {
       $cv = new stdClass();
       $cv->internal_id = $result->cv_id;
       $cv->name = $result->name;
       $cv->format = 'OBO v1.4';
       $cv->namespace = $result->namespace;
       $cv->url = $result->url;
       $cv->term_url = $result->urlprefix ? $result->urlprefix . '{id}' : '';
       $queried_entities[$cv->internal_id] = $cv;
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
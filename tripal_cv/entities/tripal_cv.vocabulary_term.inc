<?php
/**
 *
 *
 */
class TrpVocabularyTermController extends DrupalDefaultEntityController {

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
      SELECT CVT.cvterm_id, CVT.name, CVT.definition,
        DB.name as namespace, DBX.accession, CV.name as vocabulary,
        CV.cv_id
      FROM {cvterm} CVT
        INNER JOIN {cv} CV      ON CV.cv_id      = CVT.cv_id
        INNER JOIN {dbxref} DBX ON DBX.dbxref_id = CVT.dbxref_id
        INNER JOIN {db} DB      ON DB.db_id      = DBX.db_id
    ";
    $where = '';
    if ($ids) {
      $where .= 'CVT.cvterm_id IN (:ids) ';
    }
    if ($where) {
      $sql .= "WHERE $where ";
    }
    $sql .= "ORDER BY CVT.name ";
    $sql .= "LIMIT 10 OFFSET 0 ";

    $results = chado_query($sql, array(':ids' => $ids));
    while ($result = $results->fetchObject()) {
      $cvterm = new stdClass();
      $cvterm->vocabulary_internal_id = $result->cv_id;
      $cvterm->vocabulary = $result->vocabulary;
      $cvterm->namespace = $result->namespace;
      $cvterm->internal_id = $result->cvterm_id;
      $cvterm->id = $result->namespace . ':' . $result->accession;
      $cvterm->name = $result->name;
      $cvterm->def = $result->definition;

      // Find any alternate IDs or xrefs for this term.
      $sql = "
        SELECT DB.name as namespace, DBX.accession
        FROM {cvterm_dbxref} CVTDBX
          INNER JOIN {dbxref} DBX ON DBX.dbxref_id = CVTDBX.dbxref_id
          INNER JOIN {db} DB      ON DB.db_id = DBX.db_id
        WHERE CVTDBX.cvterm_id = :cvterm_id
      ";
      $dbxref_results = chado_query($sql, array(':cvterm_id' => $result->cvterm_id));
      $xrefs = array();
      $alt_ids = array();
      while ($dbxref = $dbxref_results->fetchObject()) {
        if ($dbxref->namespace = $result->namespace) {
          $alt_ids[] = $dbxref->namespace . ':' . $dbxref->accession;
        }
        else {
          $xrefs[] = $dbxref->namespace . ':' . $dbxref->accession;
        }
      }
      if (count($xrefs) > 0) {
        $cvterm->xref_id = $xrefs;
      }
      if (count($alt_ids) > 0) {
        $cvterm->alt_id = $alt_ids;
      }

      // Find any comments for this term.
      $sql = "
        SELECT CVTP.value
        FROM {cvtermprop} CVTP
          INNER JOIN {cvterm} CVT on CVT.cvterm_id = CVTP.type_id
        WHERE CVT.name = 'comment' and CVTP.cvterm_id = :cvterm_id
      ";
      $comment_results = chado_query($sql, array(':cvterm_id' => $result->cvterm_id));
      $comment = $comment_results->fetchObject();
      if ($comment) {
        $cvterm->comment = $comment->value;
      }

      // Find any relationships of this term
      $sql = "
        SELECT CVT.name as rel, CVTO.name, DB.name as namespace, DBX.accession
        FROM {cvterm_relationship} CVTR
          INNER JOIN {cvterm} CVT  ON CVT.cvterm_id  = CVTR.type_id
          INNER JOIN {cvterm} CVTO ON CVTO.cvterm_id = CVTR.object_id
          INNER JOIN {dbxref} DBX  ON DBX.dbxref_id  = CVTO.dbxref_id
          INNER JOIN {db}     DB   ON DB.db_id       = DBX.db_id
        WHERE CVT.name = 'is_a' and CVTR.subject_id = :cvterm_id
      ";
      $rel_results = chado_query($sql, array(':cvterm_id' => $result->cvterm_id));
      $rels = array();
      while ($rel = $rel_results->fetchObject()) {
        $rel_text = $rel->namespace . ':' . $rel->accession;
        if ($rel->name) {
          $rel_text .= ' ! ' . $rel->name;
        }
        $rels[$rel->rel][] = $rel_text;
      }
      foreach ($rels as $rel_type => $rel_list) {
        $cvterm->$rel_type = $rel_list;
      }

      // Find any synonyms for this term
      $sql = "
        SELECT CVTS.synonym, CVTT.name as scope
        FROM {cvtermsynonym} CVTS
          INNER JOIN {cvterm} CVT  ON  CVT.cvterm_id = CVTS.cvterm_id
          INNER JOIN {cvterm} CVTT ON CVTT.cvterm_id = CVTS.type_id
        WHERE CVTS.cvterm_id = :cvterm_id
      ";
      $synonym_results = chado_query($sql, array(':cvterm_id' => $result->cvterm_id));
      $synonyms = array();
      while ($synonym = $synonym_results->fetchObject()) {
        $synonyms[] = '"' . $synonym->synonym . '" ' . strtoupper($synonym->scope);
      }
      if (count($synonyms) > 0) {
        $cvterm->synonyms = $synonyms;
      }

      $queried_entities[$cvterm->internal_id] = $cvterm;
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
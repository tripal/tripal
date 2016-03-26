<?php
/**
 * A class the controller will use for instantiating the TripalTerm entity.
 */
class TripalTerm extends Entity {
  public function __construct($values = array()) {
    parent::__construct($values, 'TripalTerm');

    // Get the vocabulary for this term
    $vocab = entity_load('TripalVocab', array('id' => $this->vocab_id));
    $vocab = reset($vocab);
    $this->vocab = $vocab;

    // Get the term description from the storage backend
    $this->definition = NULL;
    $this->url = NULL;

    // TODO: we need some sort of administrative interface that lets the user
    // switch to the desired vocabulary type. For now, we'll just use the
    // first one in the list.
    $stores = module_invoke_all('vocab_storage_info');
    if (is_array($stores) and count($stores) > 0) {
      $keys = array_keys($stores);
      $module = $stores[$keys[0]]['module'];
      $function = $module . '_vocab_get_term';
      if (function_exists($function)) {
        $term_details = $function($vocab->namespace, $this->accession);
        $this->details = $term_details;
        if ($term_details and $term_details['definition']) {
          $this->definition = $term_details['definition'];
          $this->url = $term_details['url'];
        }
      }
    }
  }

  protected function defaultLabel() {
    return $this->name;
  }

  protected function defaultUri() {
    $vocab = 'TODO';
    return array('path' => '/vocabulary/' . $vocab . '/term/' . $this->id);
  }

  public function getDefinition() {

    return '';
  }

}
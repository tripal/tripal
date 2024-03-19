<?php

namespace Drupal\tripal\TripalPubLibrary\Interfaces;

use Drupal\Component\Plugin\PluginInspectionInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Defines an interface for tripal importer plugins.
 */
interface TripalPubLibraryInterface extends PluginInspectionInterface {


  /**
   * Provides form elements to be added for specifying criteria for parsing.
   *
   * These form elements are added after the file uploader section that
   * is automaticaly provided by the TripalImporter.
   *
   * @param array $form
   *   The form array definition.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   * @return array
   *   A new form array definition.
   */
  public function form($form, &$form_state);

  /**
   * Handles submission of the form elements.
   *
   * The form elements provided in the implementation of the form() function
   * can be used for special submit if needed.
   *
   * @param array $form
   *   The form array definition.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function formSubmit($form, &$form_state);

  /**
   * Handles validation of the form elements.
   *
   * The form elements provided in the implementation of the form() function
   * should be validated using this function.
   *
   * @param array $form
   *   The form array definition.*
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   */
  public function formValidate($form, &$form_state);

  /**
   * Performs the import.
   *
   * @param array $criteria
   *   The criteria used by the parser to retreive and parse results.
   *
   * @return array
   *   The uniform publication information array.
   */
  public function run(array $criteria);

  // TODO - convert above run function which uses array to use a query_id
  // public function run(int $query_id);

  public function retrieve(array $query);

  public function parse(string $results);

  /**
   * PEEK is required by TripalPubLibraryInterface via TripalPubLibraryBase 
   * The peek function is called dynamically by the ChadoNewPubSearchQueryForm when 
   * test search query button is clicked.
   * 
   * @param array $query
   * The criteria used by the parser to retreive and parse results.
   * 
   * @param int $limit
   * The criteria used by the parser to retreive and parse results.
   * 
   * @return array
   * Return an array with keys total_records, search_str, pubs(array)
   */
  public function peek(array $query, int $limit, int $page);

}

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
  public function form(array $form, \Drupal\Core\Form\FormStateInterface &$form_state): array;

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
  public function formSubmit(array $form, \Drupal\Core\Form\FormStateInterface &$form_state);

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
   *
   * @return void
   */
  public function formValidate(array $form, \Drupal\Core\Form\FormStateInterface &$form_state): void;

  /**
   * Performs the import.
   *
   * @param array $query
   *   An associative array defining a publication query,
   *   specifying the database and query parameters for
   *   a particular publication repository.
   *
   * @return array|NULL
   *   Array with the following keys:
   *    - 'total_records' = The number of records available for retrieval
   *    - 'search_str' = The query string used for the search
   *    - 'pubs' = The uniform publication information array.
   *   or returns NULL if the query failed and an exception was caught
   */
  public function run(array $query): ?array;

  /**
   * Returns publications from remote publication library.
   *
   * This function behaves like a pager where you specify the page
   * number you want to return and the number of records you want
   * prepared.
   *
   * @param array $query
   *   The criteria used by the parser to retreive and parse results.
   *
   * @param int $limit
   *   The number of publication records to return.
   *
   * @param int $page
   *   The specific page of publication results to retrieve
   *   Page values start at 0.
   *
   * @return array|NULL
   *   Array with the following keys:
   *    - 'total_records' = The number of records available for retrieval
   *    - 'search_str' = The query string used for the search
   *    - 'pubs' = The uniform publication information array.
   *   or returns NULL if the query failed and an exception was caught
   */
  public function retrieve(array $query, int $limit = 10, int $page = 0): ?array;

  /**
   * Parses raw xml data and structures it
   *
   * Receive the raw publication data and formats it into an
   * array that PHP can utilize.
   *
   * @param string $raw
   *   Raw data in xml format
   *
   * @return array $results
   *   An array describing the publication created by extracting
   *   from the raw data input
   */
  public function parse_xml(string $raw): array;

}

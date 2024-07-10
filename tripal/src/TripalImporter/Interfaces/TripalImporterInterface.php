<?php

namespace Drupal\tripal\TripalImporter\Interfaces;

use Drupal\Component\Plugin\PluginInspectionInterface;
use Drupal\Core\Form\FormStateInterface;

/**
 * Defines an interface for tripal importer plugins.
 */
interface TripalImporterInterface extends PluginInspectionInterface  {


  /**
   * Provides form elements to be added to the loader form.
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
   */
  public function run();

  /**
   * Performs the import.
   */
  public function postRun();

  /**
   * Adds the form elements necessary for selecting an analaysis to the form.
   *
   * While every Importer must implement this function, Ideally it should be
   * implomented by a child abstract Base class for each data store (e.g. Chado)
   * and each importer should extend the new Base class to inherit the function.
   * This will allow for consistency in the way the analysi form element is
   * presented for all Importers working on the same data store.
   *
   * @param array $form
   *   The form array definition.
   * @param \Drupal\Core\Form\FormStateInterface $form_state
   *   The form state object.
   *
   * @return array
   *   An arrya definition containing the form elements for selecting an
   *   analysis.
   */
  public function addAnalysis($form, &$form_state);

}
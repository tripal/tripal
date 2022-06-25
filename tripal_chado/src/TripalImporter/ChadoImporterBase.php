<?php

namespace Drupal\tripal_chado\TripalImporter;

use Drupal\tripal\TripalImporter\TripalImporterBase;

/**
 * Defines an interface for tripal importer plugins.
 */
abstract class ChadoImporterBase extends TripalImporterBase {

  /**
   * A databaes connection object to the Chado schema.
   * @var mixed
   */
  protected $chado = NULL;


  /**
   * {@inheritdoc}
   */

  public function __construct(array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration,$plugin_id,$plugin_definition);

    $this->chado = \Drupal::service('tripal_chado.database');
  }

  /**
   * {@inheritdoc}
   */
  public function addAnalysis($form, &$form_state) {

    // Get the list of analyses.
    $query = $this->chado->select('analysis', 'A');
    $query->fields('A', ['analysis_id', 'name']);
    $query->orderBy('A.name');
    $analyses = [];
    $results = $query->execute();
    while ($analysis = $results->fetchObject()) {
      $analyses[$analysis->analysis_id] = $analysis->name;
    }

    // Add the form element.
    $element['analysis_id'] = [
      '#title' => t('Analysis'),
      '#type' => 'select',
      '#description' => t('Choose the analysis to which the uploaded data ' .
        'will be associated. Why specify an analysis for a data load?  All ' .
        'data comes from some place, even if downloaded from a website. By ' .
        'specifying analysis details for all data imports it provides ' .
        'provenance and helps end user to reproduce the data set if needed. ' .
        'At a minimum it indicates the source of the data.'),
      '#required' =>  $this->plugin_definition['require_analysis'] ? True : False,
      '#options' => $analyses,
    ];

    return $element;
  }
}
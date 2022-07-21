<?php

namespace Drupal\tripal_chado\TripalImporter;

use Drupal\tripal\TripalImporter\TripalImporterBase;

/**
 * Defines an interface for tripal importer plugins.
 */
abstract class ChadoImporterBase extends TripalImporterBase {

  /**
   * The main chado schema for this importer.
   * This will be used in getChadoConnection.
   *
   * @var string
   */
  protected $chado_schema_main;

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition) {
    parent::__construct($configuration,$plugin_id,$plugin_definition);
  }

  /**
   * Gets a chado database connection set to the correct schema.
   *
   * Requires you to call the parent::form in your form.
   */
  public function getChadoConnection() {

    $connection = \Drupal::service('tripal_chado.database');

    // Get the chado schema name if available.
    if (!empty($this->chado_schema_main)) {
      $schema_name = $this->chado_schema_main;
    }
    elseif (!empty($this->arguments) && !empty($this->arguments['run_args'])) {
      if (isset($this->arguments['run_args']['schema_name'])) {
        $this->chado_schema_main = $schema_name = $this->arguments['run_args']['schema_name'];
      }
    }
    else {
      $this->logger->error("Unable to set Chado Schema based on importer arguments. This may mean that parent::form was not called in the form method of this importer.");
      return $connection;
    }

    $connection->setSchemaName($schema_name);

    return $connection;
  }

  /**
   * {@inheritdoc}
   */
  public function form($form, &$form_state) {

    $form['advanced'] = [
      '#type' => 'details',
      '#title' => 'Advanced Options',
      '#weight' => 9,
    ];

    $chado_schemas = [];
    $chado = \Drupal::service('tripal_chado.database');
    foreach ($chado->getAvailableInstances() as $schema_name => $details) {
      $chado_schemas[$schema_name] = $schema_name;
    }
    $default_chado = $chado->getSchemaName();

    $form['advanced']['schema_name'] = [
      '#type' => 'select',
      '#title' => 'Chado Schema Name',
      '#required' => TRUE,
      '#description' => 'Select one of the installed Chado schemas to import into.',
      '#options' => $chado_schemas,
      '#default_value' => $default_chado,
    ];

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function addAnalysis($form, &$form_state) {

    $chado = \Drupal::service('tripal_chado.database');

    // Get the list of analyses.
    $query = $chado->select('analysis', 'A');
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
      '#required' =>  $this->plugin_definition['require_analysis'],
      '#options' => $analyses,
    ];

    return $element;
  }
}

<?php

namespace Drupal\tripal_chado\TripalImporter;

use Drupal\tripal\TripalImporter\TripalImporterBase;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Defines an interface for tripal importer plugins.
 */
abstract class ChadoImporterBase extends TripalImporterBase implements ContainerFactoryPluginInterface {

  /**
   * The main chado schema for this importer.
   * This will be used in getChadoConnection.
   *
   * @var string
   */
  protected $chado_schema_main;

  /**
   * An instance of the Drupal messenger.
   *
   * @var object \Drupal\Core\Messenger\Messenger
   */
  protected $messenger = NULL;

  /**
   * The database connection for querying Chado.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected $connection;

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static function
   * will be called behind the scenes when a Plugin Manager uses createInstance(). Specifically
   * this method is used to determine the parameters to pass to the contructor.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   *
   * @return static
   */
   public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('tripal_chado.database')
    );
  }

  /**
   * Implements __contruct().
   *
   * Since we have implemented the ContainerFactoryPluginInterface, the constructor
   * will be passed additional parameters added by the create() function. This allows
   * our plugin to use dependency injection without our plugin manager service needing
   * to worry about it.
   *
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   * @param Drupal\tripal_chado\Database\ChadoConnection $connection
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, ChadoConnection $connection) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);

    $this->connection = $connection;
  }

  /**
   * Gets a chado database connection set to the correct schema.
   *
   * Requires you to call the parent::form in your form.
   */
  public function getChadoConnection() {
    $chado = $this->connection;

    // Get the chado schema name if available.
    $schema_name = '';
    if (!empty($this->chado_schema_main)) {
      $schema_name = $this->chado_schema_main;
    }
    elseif (!empty($this->arguments) && !empty($this->arguments['run_args'])) {
      if (isset($this->arguments['run_args']['schema_name'])) {
        $schema_name = $this->arguments['run_args']['schema_name'];
        $this->chado_schema_main = $schema_name;
      }
    }
    else {
      $this->logger->error("Unable to set Chado Schema based on importer arguments. This may mean that parent::form was not called in the form method of this importer.");
      return $chado;
    }

    if ($chado->getSchemaName() != $schema_name) {
      $chado->setSchemaName($schema_name);
    }
    $chado->useTripalDbxSchemaFor(get_class());

    return $chado;
  }

  /**
   * Creates a database transaction in the specific schema(s) this importer will
   * be importing data into.
   *
   * @return array
   *   An array of Drupal DatabaseTransaction objects. These are usually
   *   obtained by calling the startTransaction() method on the database
   *   connection object.
   */
  public function startTransactions() {
    $transactions = [];

    // By default the Chado importer returns a single transaction
    // focused on the chado schema set in the current importer job.
    // If you are importing into multiple chado schema you will want to
    // override this and add one transaction object for each schema
    // you are importing into.
    $chado = $this->getChadoConnection();
    $transactions[] = $chado->startTransaction();

    return $transactions;
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
    $chado = $this->connection;
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

    $chado = $this->connection;

    // Get the list of analyses.
    $query = $chado->select('1:analysis', 'A');
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

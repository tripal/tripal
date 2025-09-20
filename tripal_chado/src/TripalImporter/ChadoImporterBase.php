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
   * The type_id for the bundle(s)
   * @var array $bundle_type_id
   *   Key is DB:accession, value is cvterm_id
   */
  protected array $bundle_type_id = [];

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static function
   * will be called behind the scenes when a Plugin Manager uses createInstance(). Specifically
   * this method is used to determine the parameters to pass to the constructor.
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
   * Implements __construct().
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
    $chado->useTripalDbxSchemaFor(self::class);

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
   * Adds the bundle type property to a record
   *
   * @param string $pkey
   *   The primary key column name
   * @param int $record_id
   *   The pkey value for the record
   * @param string $property_table
   *   The name of the property table
   * @param string $termIdSpace
   *   The name in the chado.db table
   * @param string $termAccession
   *   The accession in the chado.dbxref table
   * @param string $value = ''
   *   Property value, can be empty string
   * @param ?int $rank = NULL
   *   Optional rank
   */
  protected function addBundleTypeProperty(string $pkey, int $record_id, string $property_table, string $termIdSpace, string $termAccession, string $value = '', ?int $rank = NULL) {
    $key = $termIdSpace . ':' . $termAccession;
    if (!($this->bundle_type_id[$key] ?? NULL)) {
      $select = $this->connection->select('1:cvterm', 'T');
      $select->join('1:dbxref', 'X', '"T".dbxref_id = "X".dbxref_id');
      $select->join('1:db', 'DB', '"X".db_id = "DB".db_id');
      $select->condition('DB.name', $termIdSpace, '=');
      $select->condition('X.accession', $termAccession, '=');
      $select->addField('T', 'cvterm_id', 'cvterm_id');
      $this->bundle_type_id[$key] = $select->execute()->fetchField();
    }
    $type_id = $this->bundle_type_id[$key];
    $values = [
      $pkey => $record_id,
      'type_id' => $type_id,
      'value' => $value,
    ];
    if (!is_null($rank)) {
      $values['rank'] = $rank;
    }
    $this->connection->insert('1:' . $property_table)
      ->fields($values)
      ->execute();
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

  /**
   * Performs tasks after the importer has completed.
   *
   * Publish task:
   * Bundles that are candidates for publishing are specified in
   * the importer annotation. They then show up in the importer
   * form with a standard prefix.
   *
   * @return void
   *   No return value.
   */
  public function postRun() {
    parent::PostRun();

    // Only publish if a publish manager has been set by a child class.
    if ($this->publish_manager) {

      $arguments = $this->getArguments();
      $run_args = $arguments['run_args'];
      $bundles_to_publish = [];

      // Find if there are any bundles to be published.
      foreach ($run_args as $key => $value) {
        if (preg_match('/^publish_(.+)$/', $key, $matches)) {
          $bundle = $matches[1];
          if ($value) {
            // If checkbox is set, then publish this bundle.
            $bundles_to_publish[] = $bundle;
          }
        }
      }

      // If there are bundles to publish, then publish any newly
      // imported records.
      if ($bundles_to_publish) {
        $instance = $this->publish_manager->createInstance('chado_storage', []);
        foreach ($bundles_to_publish as $bundle) {
          $publish_options = [
            'bundle' => $bundle,
            'datastore' => 'chado_storage',
            'schema_name' => $run_args['schema_name'],
            'republish' => FALSE,
          ];
          $instance->publish($publish_options);
        }
      }
    }
  }

}


Code Examples
=============

The following code can be used in modules or tested directly using
``drush php``.

1. Dynamic query example on feature table in default Chado schema.

.. code-block:: php

  $biodb = \Drupal::service('tripal_chado.database');
  $query = $biodb->select('feature', 'x');
  $query->condition('x.is_obsolete', 'f', '=');
  $query->fields('x', ['name', 'residues']);
  $query->range(0, 10);
  $result = $query->execute();
  foreach ($result as $record) {
    echo $record->name . "\n";
  }

See also `Drupal dynamic queries <https://www.drupal.org/docs/8/api/database-api/dynamic-queries>`_.

2. Static query example on feature table in default Chado schema.

.. code-block:: php

  $biodb = \Drupal::service('tripal_chado.database');
  $sql_query = 'SELECT name, residues FROM {1:feature} x WHERE x.is_obsolete = :obsolete LIMIT 0, 10;';
  $results = $biodb->query($sql_query, [':obsolete' => 'f']);
  foreach ($results as $record) {
    echo $record->name . "\n";
  }

See also `Drupal static queries <https://www.drupal.org/docs/drupal-apis/database-api/static-queries>`_
and `result sets <https://www.drupal.org/docs/8/api/database-api/result-sets>`_.

3. Cross schema queries.

.. code-block:: php

  $biodb = \Drupal::service('tripal_chado.database');
  $biodb->setSchemaName('chado1');
  $biodb->addExtraSchema('chado2');
  $sql = "
    SELECT * FROM
      {1:feature} f1,
      {2:feature} f2,
      {node_field_data} fd
    WHERE fd.title = f1.uniquename
      AND f1.uniquename = f2.uniquename;";
  $results = $biodb->query($sql);

4. Chado installation task example.

.. code-block:: php

  $parameters = [
    'input_schemas' => [],
    'output_schemas' => ['chado'],
    'version' => '1.3',
  ];
  $installer = \Drupal::service('tripal_chado.installer');
  $installer->setParameters($parameters);
  $success = $installer->performTask();
  if (!$success) {
    echo "Chado installation failed. See logs for details.\n";
  }

5. Chado cloner task example.

.. code-block:: php

  $parameters = [
    'input_schemas' => ['chado'],
    'output_schemas' => ['chado2'],
  ];
  $cloner = \Drupal::service('tripal_chado.cloner');
  $cloner->setParameters($parameters);
  $success = $cloner->performTask();
  if (!$success) {
    echo "Failed to clone schema. See logs for details.\n";
  }

6. Chado upgrader task example with status tracking.

Execution thread:

.. code-block:: php

  $parameters = ['output_schemas' => ['chado'],];
  $upgrader = \Drupal::service('tripal_chado.upgrader');
  $upgrader->setParameters($parameters);
  $success = $upgrader->performTask();
  if (!$success) {
    echo "Failed to upgrade schema. See logs for details.\n";
  }

Status tracking thread (using the same parameters):

.. code-block:: php

  $parameters = ['output_schemas' => ['chado'],];
  $upgrader = \Drupal::service('tripal_chado.upgrader');
  $upgrader->setParameters($parameters);
  $progress = $upgrader->getProgress();
  $status = $upgrader->getStatus();
  echo "Currently at " . (100*$progress) . "%\n" . $status;

7. Some random code.

.. code-block:: php

  // Get the BioDatabase tool.
  $biotool = \Drupal::service('tripal_biodb.tool');

  // Get Drupal schema name.
  $biotool->getDrupalSchemaName();
  
  // Test if a user-provided schema name is valid and not reserved.
  if ($issue = $biotool->isInvalidSchemaName($schema_name)) {
    throw new Exception();
  }
  // If we want to check a reserved schema name.
  $biotool->isInvalidSchemaName($schema_name, TRUE);
  
  // Temporary reserve a new schema pattern to avoid its use by other modules.
  $biotool->reserveSchemaPattern('mytests_*', 'Reserved for my tests.');
  
  // Permanently reserve a pattern.
  $config = \Drupal::service('config.factory')
    ->getEditable('tripal_biodb.settings')
  ;
  // Warning: to not free other reservations, don't forget to get current
  // config first and modify that array! Don't create a new one.
  $reserved_schema_patterns = $config->get('reserved_schema_patterns') ?? [];
  $reserved_schema_patterns['mytests_*'] = 'Reserved for my tests.';
  $config->set('reserved_schema_patterns', $reserved_schema_patterns)->save();

  // Get a new Chado connection using default Chado schema.
  $biodb = \Drupal::service('tripal_chado.database');
  
  // Create a new schema 'new_chado': 2 possible methods.
  // - Method 1, using BioDatabase Tool:
  $biotool->createSchema('new_chado');
  // - Method 2, unsing a Chado connection: first we need to set the schema name
  //   and then create it.
  $biodb->setSchemaName('new_chado');
  $biodb->schema->createSchema();
  
  // Copy some feature values from 'chado1' to 'chado2':
  $biodb->setSchemaName('chado1');
  $biodb->addExtraSchema('chado2');
  $sql = "
    INSERT INTO {2:feature} f2
      (organism_id, name, uniquename, residues, seqlen, md5checksum, type_id)
    SELECT o2.organism_id, f1.name, f1.uniquename, f1.residues, f1.seqlen, f1.md5checksum, c2.cvterm_id
    FROM {1:feature} f1
      JOIN {1:organism} o1 ON o1.organism_id = f1.organism_id
      JOIN {1:cvterm} c1 ON c1.cvterm_id = f1.type_id,
      {2:organism} o2,
      {2:cvterm} c2
    WHERE o2.species = o1.species
      AND c2.name = c1.name
      AND f1.uniquename LIKE 'NEW_%'
    ;";
  $results = $biodb->query($sql);

  // By default, ->select, ->insert, ->update, ->delete and other similar
  // dynamic query methods of BioConnection will use Drupal schema. In fact, it
  // is because those methods generate SQL queries using the table notation with
  // simple curly braces (ie. "{some_table_name}") which will use Drupal table
  // for backward compatibility with Drupal. It is possible to change that
  // default to the selected biological schema. In order to use a Chado schema
  // as default for those methods in other modules, the class or an instance
  // must register itself as willing to use the biological schema by default:
  $some_object = new SomeClass();
  // Register any instance of the class: 
  $biodb->useBioSchemaFor(SomeClass::class);
  // Another way would be to just register a specific class instance:
  $biodb->useBioSchemaFor($some_object);
  // Now calls to BioConnection dynamic query methods will work on the
  // biological schema by default (until thread ends or call to
  // ::useDrupalSchemaFor method).
  // Note: if static queries in any of the registered classes need to use Drupal
  // tables, instead of using the simple curly braces notation, the Drupal
  // schema index must be specified explicitly. So "{some_table_name}" must be
  // turned into "{0:some_table_name}".
  
  // Execute a set of SQL commands on a given biological schema from an SQL file
  // that may containt "SET search_path = ...":
  // - Case 1: automatically remove any "SET search_path":
  $biodb->executeSqlFile($sql_file_path, 'none');
  // - Case 2: replace some schema names by others in every "SET search_path":
  //   Here we replace every 'chado' by 'my_chado'.
  $biodb->executeSqlFile($sql_file_path, ['chado' => 'my_chado']);
  
  // Get the list of table in a biological schema.
  $tables = $biodb->schema()->getTables(['table', 'view']);
  $stock_table = $tables['stock'];

  // Get table definition with a simple array structure.
  $biodb->schema()->getTableDef('stock', []);
  // Get table definition from file version 1.3 in Drupal database API format.
  $biodb->schema()->getTableDef('stock', ['source' => 'file', 'format' => 'drupal', 'version' => '1.3']);
  // Get table definition from database as SQL DDL.
  $biodb->schema()->getTableDef('stock', ['source' => 'database', 'format' => 'sql']);

  // Clone 'chado' schema into 'chado2'.
  // Method 1: using Biological Database API.
  $biodb->setSchemaName('chado2');
  $biodb->schema()->cloneSchema('chado');
  // Method 2: using cloner service.
  $parameters = [
    'input_schemas' => ['chado'],
    'output_schemas' => ['chado2'],
  ];
  $cloner = \Drupal::service('tripal_chado.cloner');
  $cloner->setParameters($parameters);
  $success = $cloner->performTask();
  if (!$success) {
    echo "Failed to clone schema. See logs for details.\n";
  }


  // Get Chado test schema base name.
  $test_schema_base_names = $config->get('test_schema_base_names') ?? [];
  $chado_test_base_name = $test_schema_base_names['chado'];
  
  // Write tests for Chado operations.
  use Drupal\Tests\tripal_chado\Functional\ChadoTestKernelBase;
  class MyFunctionalTest extends ChadoTestKernelBase {
    // Get a temporary schema name.
    $biodb = $this->getTestSchema(ChadoTestKernelBase::SCHEMA_NAME_ONLY);

    // Create a temporary schema with dummy data.
    $biodb2 = $this->getTestSchema(ChadoTestKernelBase::INIT_DUMMY);

    // Create a temporary empty Chado schema with no data.
    $biodb3 = $this->getTestSchema(ChadoTestKernelBase::INIT_CHADO_EMPTY);

    // Create a temporary empty Chado schema with some dummy data.
    $biodb4 = $this->getTestSchema(ChadoTestKernelBase::INIT_CHADO_DUMMY);

    // ... test stuff ...

    // Once done, don't forget to free all used schemas.
    // If you forget, there is a garbage collecting system that will remove
    // unused schemas but warnings will be raised.
    $this->freeTestSchema($biodb);
    $this->freeTestSchema($biodb2);
    $this->freeTestSchema($biodb3);
    $this->freeTestSchema($biodb4);
  }

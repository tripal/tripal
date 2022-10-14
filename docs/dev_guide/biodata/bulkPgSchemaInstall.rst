
Bulk Schema Install for PostgreSQL
====================================

Tripal provides a service to bulk import SQL files into a PostgreSQL database. The following code block shows how this service can be used to create a fictional database schema and populate it within your Drupal/Tripal database. It's recommended to use a schema within your Drupal/Tripal database to ensure you can easily join between your biological and application data. That said, this is not a requirement as Tripal can support data from outside sources.

.. code-block:: php

	$schema_name = 'biodb'; // This is a fictional example schema name.
	// First initialize the Tripal Services.
	$service = \Drupal::service('tripal.bulkPgSchemaInstaller');
	// Next Create the schema.
	$success = $service->createSchema($schema_name);
	// And if the schema exists...
	if ($service->checkExists($schema_name)) {
		// Then populate the schema in bulk using an SQL file.
		$sql_file = \Drupal::service('extension.list.module')->getPath('tripal') . '/tests/fixtures/smallTestSchema.sql';
		$success = $service->applySQL($sql_file, $schema_name, TRUE);
	}

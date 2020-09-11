Accessing Chado
================

Primarily biological data made available to Tripal is stored in the GMOD Chado
schema. As such, you will likely need to interact with Chado at some point.
Tripal has developed a number of API functions and classes to make this
interaction easier and more generic.

The Chado Query API
--------------------

Provides an API for querying of chado including inserting, updating, deleting and selecting from specific chado tables. There is also a generic function, ``chado_query()``, to execute and SQL statement on chado. It is ideal to use these functions to interact with chado in order to keep your module compatible with both local & external chado databases. Furthermore, it ensures connection to the chado database is taken care of for you.

Generic Queries to a specific chado table
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Selecting Records
""""""""""""""""""

``chado_select_record( [table name], [columns to select], [specify record to select], [options*] )``

This function allows you to select various columns from the specified chado table. Although you can only select from a single table, you can specify the record to select using values from related tables through use of a nested array. For example, the following code shows you how to select the name and uniquename of a feature based on it's type and source organism.

.. code-block:: php

  $values =  array(
    'organism_id' => array(
      'genus' => 'Citrus',
      'species' => 'sinensis',
    ),
    'type_id' => array (
      'cv_id' => array (
        'name' => 'sequence',
      ),
      'name' => 'gene',
      'is_obsolete' => 0
    ),
  );

  $result = chado_select_record(
    'feature',                      // table to select from
    array('name', 'uniquename'),    // columns to select
    $values                         // record to select (see variable defn. above)
  );

Inserting Records
""""""""""""""""""

``chado_insert_record( [table name], [values to insert], [options*] )``

This function allows you to insert a single record into a specific table. The values to insert are specified using an associative array where the keys are the column names to insert into and they point to the value to be inserted into that column. If the column is a foreign key, the key will point to an array specifying the record in the foreign table and then the primary key of that record will be inserted in the column. For example, the following code will insert a feature and for the type_id, the cvterm.cvterm_id of the cvterm record will be inserted and for the organism_id, the organism.organism_id of the organism_record will be inserted.

.. code-block:: php

  $values =  array(
    'organism_id' => array(
        'genus' => 'Citrus',
        'species' => 'sinensis',
     ),
    'name' => 'orange1.1g000034m.g',
    'uniquename' => 'orange1.1g000034m.g',
    'type_id' => array (
        'cv_id' => array (
           'name' => 'sequence',
        ),
        'name' => 'gene',
        'is_obsolete' => 0
     ),
  );
  $result = chado_insert_record(
    'feature',             // table to insert into
    $values                // values to insert
  );

Updating Records
""""""""""""""""""

``chado_update_record( [table name], [specify record to update], [values to change], [options*] )``

This function allows you to update records in a specific chado table. The record(s) you wish to update are specified the same as in the select function above and the values to be update are specified the same as the values to be inserted were. For example, the following code species that a feature with a given uniquename, organism_id, and type_id (the unique constraint for the feature table) will be updated with a new name, and the type changed from a gene to an mRNA.

.. code-block:: php

  $umatch = array(
    'organism_id' => array(
      'genus' => 'Citrus',
      'species' => 'sinensis',
    ),
    'uniquename' => 'orange1.1g000034m.g7',
    'type_id' => array (
      'cv_id' => array (
        'name' => 'sequence',
      ),
      'name' => 'gene',
      'is_obsolete' => 0
    ),
  );
  $uvalues = array(
    'name' => 'orange1.1g000034m.g',
    'type_id' => array (
      'cv_id' => array (
        'name' => 'sequence',
      ),
      'name' => 'mRNA',
      'is_obsolete' => 0
    ),
  );
  $result = chado_update_record('feature',$umatch,$uvalues);

Deleting Records
"""""""""""""""""

``chado_delete_record( [table name], [specify records to delete], [options*] )``

This function allows you to delete records from a specific chado table. The record(s) to delete are specified the same as the record to select/update was above. For example, the following code will delete all genes from the organism Citrus sinensis.

.. code-block:: php

  $values =  array(
    'organism_id' => array(
        'genus' => 'Citrus',
        'species' => 'sinensis',
     ),
    'type_id' => array (
        'cv_id' => array (
           'name' => 'sequence',
        ),
        'name' => 'gene',
        'is_obsolete' => 0
     ),
  );
  $result = chado_select_record(
     'feature',                      // table to select from
     $values                         // records to delete (see variable defn. above)
  );

Generic Queries for any SQL
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Often it is necessary to select from more then one table in chado or to execute other complex queries that cannot be handled efficiently by the above functions. It is for this reason that the ``chado_query( [sql string], [arguments to sub-in to the sql] )`` function was created. This function allows you to execute any SQL directly on the chado database and should be used with care. If any user input will be used in the query make sure to put a placeholder in your SQL string and then define the value in the arguments array. This will make sure that the user input is sanitized and safe through type-checking and escaping. The following code shows an example of how to use user input resulting from a form and would be called with the form submit function.

.. code-block:: php

  $sql = "SELECT F.name, CVT.name as type_name, ORG.common_name
           FROM feature F
           LEFT JOIN cvterm CVT ON F.type_id = CVT.cvterm_id
           LEFT JOIN organism ORG ON F.organism_id = ORG.organism_id
           WHERE
             F.uniquename = :feature_uniquename";
  $args = array( ':feature_uniquename' => $form_state['values']['uniquename'] );
  $result = chado_query( $sql, $args );
  foreach ($result as $r) { [Do something with the records here] }

If you are going to need more then a couple fields, you might want to use the Chado Variables API (specifically ``chado_generate_var()``) to select all of the common fields needed including following foreign keys.

Loading of Variables from chado data
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

These functions, ``chado_generate_var()`` and ``chado_expand_var()``, generate objects containing the full details of a record(s) in chado. These should be used in all theme templates.

This differs from the objects returned by ``chado_select_record`` in so far as all foreign key relationships have been followed meaning you have more complete details. Thus this function should be used whenever you need a full variable and ``chado_select_record`` should be used if you only case about a few columns.

The initial variable is generated by the ``chado_generate_var([table], [filter criteria], [optional options])`` function. An example of how to use this function is:

.. code-block:: php

  $values = array(
    'name' => 'Medtr4g030710'
  );
  $features = chado_generate_var('feature', $values);

This will return an object if there is only one feature with the name Medtr4g030710 or it will return an array of feature objects if more than one feature has that name.

Some tables and fields are excluded by default. To have those tables & fields added to your variable you can use the ``chado_expand_var([chado variable], [type], [what to expand], [optional options])`` function. An example of how to use this function is:

.. code-block:: php

  // Get a chado object to be expanded
  $values = array(
    'name' => 'Medtr4g030710'
  );

  $features = chado_generate_var('feature', $values);

  // Expand the organism node
  $feature = chado_expand_var($feature, 'node', 'organism');

  // Expand the feature.residues field
  $feature = chado_expand_var($feature, 'field', 'feature.residues');

  // Expand the feature properties (featureprop table)
  $feature = chado_expand_var($feature, 'table', 'featureprop');


The Chado Schema API
--------------------

The Chado Schema API provides an application programming interface (API) for describing Chado tables, accessing these descriptions and checking for compliancy of your current database to the chado schema. This API consists of the ChadoSchema class which provides methods for interacting with the Chado Schema API and a collection of supporting functions, one for each table in Chado, which describe each version of the Chado schema. Each function simply returns a Drupal style array that defines the table.

Ensuring columns Tables & Columns exist
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Generally you can assume the tables and columns in the Chado schema have been unaltered. That said, there are still cases where you might want to check that specific tables and columns exist. For example, when using a custom table, it is best practice to ensure it is there before querying as it can be removed through the administrative interface.

To check the existence of a specific table and column, you can use the following:

.. code-block:: php

  $chado_schema = new \ChadoSchema();

  // Check that the organism_feature_count custom table exists.
  $table_name = 'organism_feature_count';
  $table_exists = $chado_schema->checkTableExists($table_name);

  if ($table_exists) {

    // Check that the organism_feature_count.feature_id column exists.
    $column_name = 'feature_id';
    $column_exists = $chado_schema->checkColumnExists($table_name, $column_name);

    if ($column_exists) {

      [ do your query, etc. here ]

    } else { [warn the admin using tripal_report_error()] }
  } else { [warn the admin using tripal_report_error()] }

Checking the Schema Version
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you are using chado tables specific to a given version of Chado, it is best practice to check the chado version of the current site before querying those tables. You can use the following query to do this:

.. code-block:: php

  $chado_schema = new \ChadoSchema();
  $version = $chado_schema-getVersion();
  if ($version == '1.3') {
    [do your chado v1.3 specific querying here]
  } else { [warn the admin using tripal_report_error() ] }


Retrieving a list of tables
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To retrieve a list of Chado tables, you can use the following:

.. code-block:: php

  $chado_schema = new \ChadoSchema();

  // All Chado Tables including custom tables
  $all_tables = $chado_schema->getTableNames(TRUE);

  // All Chado Tables without custom tables
  $all_tables = $chado_schema->getTableNames();

  // Chado tables designated as Base Tables by Tripal.
  $base_tables = $chado_schema->getBaseTables();


Ensuring your Chado instance is compliant
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Checking compliancy of your Chado instance with the released Chado Schema is a great way to **confirm an upgrade has gone flawlessly**. Additionally, while it is not recommended, sometimes customizations to the Chado schema may be necessary. In these cases, you should **ensure backwards compatibility** through compliance checking to confirm Tripal will work as expected.

Chado compliancy testing is provided with Tripal's automated PHPUnit testing. As such, to test compliancy of your specific Chado instance, you first need to install Composer. Luckily this can be as easy as:

.. code-block:: bash

  php -r "copy('https://getcomposer.org/installer', 'composer-setup.php');"
  php -r "if (hash_file('SHA384', 'composer-setup.php') === '544e09ee996cdf60ece3804abc52599c22b1f40f4323403c44d44fdfdd586475ca9813a858088ffbc1f233e9b180f061') { echo 'Installer verified'; } else { echo 'Installer corrupt'; unlink('composer-setup.php'); } echo PHP_EOL;"
  php composer-setup.php
  php -r "unlink('composer-setup.php');"

Once you have Composer, you need to install PHPUnit. This is installed locally within your Tripal repository. The following bash snippet shows you how to both install composer locally and run compliance checking.

.. code-block:: php

  cd [DRUPAL_ROOT]/sites/all/modules/tripal
  composer up

  # Now run compliance checking
  ./vendor/bin/phpunit --group chado-compliance

Schema Definition
^^^^^^^^^^^^^^^^^^

To retrieve the schema definition for a specific table, you can execute the following:

.. code-block:: php

  $table_name = 'feature';
  $chado_schema = new \ChadoSchema();
  $table_schema = $chado_schema->getTableSchema($table_name);

The resulting ``$table_schema`` variable contains a Drupal-style array describing the schema definition of the table specified by ``$table_name``. This is a great tool when trying to develop generic queries, since you can extract information about an unknown table and use it to build a query for that table. For more information on the format of this array, see `the Drupal Schema API documentation <https://api.drupal.org/api/drupal/includes%21database%21schema.inc/group/schemaapi/7.x>`_.

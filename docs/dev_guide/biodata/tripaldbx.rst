
Tripal DBX: Generic cross database support for Drupal
========================================================

Tripal has always focused on providing cross database schema support. From the very first version, this support focused on providing integration between Drupal and the GMOD Chado Schema. With Tripal 4, we have made a generic base for our Chado integration which is known as Tripal DBX. This ensures that we can still provide high integration between Drupal and Chado, while also proving a really solid, well-documented API for additional biological data storage options.

Tripal DBX extends the Drupal Database API. Specifically, it extends two core Drupal abstract classes:

 - `\Drupal\Core\Database\Connection <https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Connection.php/class/Connection/9.3.x>`_: a Drupal-specific extension of the PDO database abstraction class in PHP.
 - `\Drupal\Core\Database\Schema <https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Schema.php/class/Schema/9.3.x>`_: provides a means to interact with a schema including management tasks such as adding tables.

Currently Tripal DBX relies on the Drupal PostgreSQL implementations of these classes (`PostgreSQL Connection <https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Driver%21pgsql%21Connection.php/class/Connection/9.3.x>`_ and `PostgreSQL Schema <https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Driver%21pgsql%21Schema.php/class/Schema/9.3.x>`_), although there is structure in place to expand it to other Drupal database drivers.

Tripal DBX Connection
-----------------------

There are two main parts of the Drupal Connection class that the Tripal DBX Api overrides:

**A) One Database Connection per Instance:**

Drupal attempts to minimize the number of connections to the database by only creating a new connection if one does not already exist. This makes sense in the context of a single database schema; however, when dealing with multiple schema you will often need to make changes to the search path in order to access the tables of each schema. This API creates an independent connection to the database for each TripalDbxConnection instance which ensures the primary Drupal connection remains unaffected by search path changes.

**B) Table Prefixing in Queries:**

Drupal has always had support for prefixing table names; however, this has been a simple string prepended to the beginning of the table name. This is likely due to many database systems not supporting multiple schema within a single database. In version 2 and 3, Tripal used the native Drupal prefixing to access the Chado database in a separate schema. It did this by using a prefix of "chado." which takes advantage of the PostgreSQL syntax for accessing a specific schema (named "chado" in the previous example). In Tripal version 4, we've created the Tripal DBX API which takes this one step further by extending the native PHP/Drupal PDO database layer to use cross schema focused table prefixing. This allows module developers to access multiple chado and additional Tripal DBX managed schema both through direct queries and using the object-oriented query builder.

The PHP/Drupal PDO query is very useful for building dynamic queries as it handles user provided parameters in a very secure manner and can be very accessible for those who are less familar with PostgreSQL syntax. The following example shows how the native Drupal query builder can be used with Chado through the Tripal DBX API:

.. code-block:: php

   // Open a Connection to the default Tripal DBX managed Chado schema.
   $connection = \Drupal::service('tripal_chado.database');

   // Start a select query on the Chado feature table and assign an alias of x.
   $query = $connection->select('feature', 'x');

   // Add a where condition that feature.is_obsolete is FALSE.
   $query->condition('x.is_obsolete', 'f', '=');

   // Select the name and residues columns/fields from the table.
   $query->fields('x', ['name', 'residues']);

   // And only show the first 10 records/entries.
   $query->range(0, 10);

   // Finally execute the query we generated against the database.
   $result = $query->execute();

   // And iterate through the returned results.
   foreach ($result as $record) {
     // Do something with the $record object here.
     // e.g. echo $record->name;
   }

The above example used a Chado implementation of the Tripal DBX API provided by the ``tripal_chado.database`` service to generate a select query, execute it agains the database focusing on a specific non-Drupal schema and then iterates through the results. It is the equivalent of the following SQL statement: ``SELECT x.name, x.residues FROM chado.feature x WHERE x.is_obsolete = f LIMIT 10`` if the default Chado schema is named ``chado``.

.. note::

  For more information on the Drupal query builder, `See the Drupal.org documentation <https://www.drupal.org/docs/8/api/database-api/dynamic-queries/introduction-to-dynamic-queries>`_. There is full support for all the documented Drupal functionality with Tripal DBX managed schema.

**--- Multiple Schema Support**

Tripal DBX provides multiple database schema support through table prefixing. The first step is to set the schema you are working on in your specific connection. For example, if you were working with two Chado schema (named "chado1" and "chado2" in this example) in addition to the Drupal schema then you would use ``setSchemaName()`` to specify your main schema and then ``addExtraSchema()`` to specify any additional ones.

.. code-block:: php

  $connection = \Drupal::service('tripal_chado.database');
  $connection->setSchemaName('chado1');
  $connection->addExtraSchema('chado2');

.. note::

  The primary schema indicated using ``setSchemaName()`` can be decided in a number of ways depending on your use case for multiple schema and the specific query you are executing. The rule of thumb is to make the primary schema match the one "prepared" to work with Tripal (i.e. the schema used as a base for Tripal Entities).

Now that you have your connection set up indicating the schema you are interested in, you can use the query builder to generate as many queries as you need within the current scope. For example, the following code will generate a query returning chromosome features stored in a separate chado schema (i.e. ``chado2``) and using the primary chado schema (i.e. ``chado1``) for organism  + ontology information:

.. code-block:: php

  // Start a select query on the feature table in the chado2 schema.
  // Note the schema is indicated by prefixing a "2:" on the table name.
  $query = $connection->select('2:feature', 'f');

  // Add a join to the organism + cvterm table in the chado1 schema.
  // Note that no prefix is needed for the primary Tripal DBX managed schema.
  $query->join('organism', 'o', 'o.organism_id = f.organism_id');
  $query->join('cvterm', 'cvt', 'cvt.cvterm_id = f.type_id');

  // Add a where clause ensuring only records associated with the Tripalus genus are returned.
  $query->condition('o.genus', 'Tripalus', '=');

  // Add a where clause ensuring only "chromosome" feature types are returned.
  $query->condition('cvt.name', 'chromosome', '=');

  // Select the feature feature_id, name + uniquename and the organism genus, species + common name.
  $query->fields('f', ['feature_id', 'name', 'uniquename']);
  $query->fields('o', ['genus', 'species', 'common_name']);

  // Finally execute the query we generated against the database.
  $result = $query->execute();

  // And iterate through the returned results.
  foreach ($result as $record) {
    // Do something with the $record object here.
    // e.g. echo $record->name;
  }

.. note::

  This API expects all table names to be wrapped in curly brackets with an integer indicating the schema the table is in. For example, ``{1: feature}`` would indicate the feature table in the current Tripal DBX managed schema, ``{0: system}`` would indicate the Drupal system table and additional numeric indices would be used for extra Tripal DBX managed schema (i.e. ``{2: feature}``).

Alternatively, if you have a specific query in mind and do not need the security or overhead of the query builder, then you can use the Drupal ``query()`` method to execute it directly. The following example shows how you would execute the equivalent query built by the query builder above:

.. code-block:: php

  // Set some variables or retrieve them from your users.
  $type = 'chromosome';
  $genus = 'Tripalus';

  // The SQL statement to be executed.
  // Note that we've used the {1:organism} and {2:feature} for the primary and extra schemas respectively.
  // Also note that placeholders (i.e. :type) are used for user input.
  $sql = 'SELECT f.feature_id, f.name, f.uniquename, o.genus, o.species, o.common_name
          FROM {2:feature} f
          LEFT JOIN {1:organism} o ON o.organism_id=f.organism_id
          LEFT JOIN {1:cvterm} cvt ON cvt.cvterm_id=f.type_id
          WHERE o.genus = :genus AND cvt.name = :type';

  // Finally execute the query we generated against the database
  // by providing the values for any placeholders.
  $results = $connection->query($sql, [':genus' => $genus, ':type' => $type]);

  // And iterate through the returned results.
  foreach ($results as $record) {
    // Do something with the $record object here.
    // e.g. echo $record->name;
  }

.. warning::

  When using the ``query`` method to submit SQL statements directly, it is very important to be aware of security and the source of any information. Variables should NEVER be embedded directly in the SQL and all dynamic and/or user input should be handled using placeholders in the SQL statement and then provided when the query is executed.

.. note::

  The ``query`` method shown for multiple schema can also be used for single schema queries as an alternative to the query builder. As indicated in the query builder, for a single schema the ``{tablename}`` can be used and the ``1:`` prefix omitted.

Tripal DBX Schema
-------------------

.. note::

  This class should not be instantiated directly but rather it should be accessed through a TripalDbxConnection object using the schema() method. This is to avoid issues when the default Tripal DBX managed schema name is changed in the TripalDbxConnection object which could lead to issues.

  .. warning::

    If you choose to instantiate a TripalDbxSchema object yourself, you are responsible to not change the Tripal DBX managed schema name of the connection object used to instantiate this TripalDbxSchema.

This class provides a Tripal-specific implementation of the `Drupal Schema abstract class <https://api.drupal.org/api/drupal/core!lib!Drupal!Core!Database!Schema.php/class/Schema/9.3.x>`_. The `Drupal PostgreSQL <https://api.drupal.org/api/drupal/core!lib!Drupal!Core!Database!Driver!pgsql!Schema.php/class/Schema/9.3.x>`_ (and other database driver) implementations of the base Drupal Schema class follow the assumption that there is a single schema. As such the core Drupal implementations focus on managing tables within a single schema.

The TripalDBXSchema class extends that table-management functionality to also include schema-focused management including creation, cloning, renaming, dropping and definition export. Additionally, it removes the assumption of a single schema by allowing the default schema to be set based on a Tripal DBX connection.

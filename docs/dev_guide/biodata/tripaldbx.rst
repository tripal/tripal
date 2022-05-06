
Tripal DBX: Generic cross database support for Drupal
========================================================

Tripal has always focused on providing cross database schema support. From the very first version, this support focused on providing integration between Drupal and the GMOD Chado Schema. With Tripal 4, we have made a generic base for our Chado integration which is known as Tripal DBX. This ensures that we can still provide high integration between Drupal and Chado, while also proving a really solid, well-documented API for additional biological data storage options.

Tripal DBX extends the Drupal Database API. Specifically, it extends two core Drupal abstract classes:

 - `\Drupal\Core\Database\Connection <https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Connection.php/class/Connection/9.3.x>`_: a Drupal-specific extension of the PDO database abstraction class in PHP.
 - `\Drupal\Core\Database\Schema <https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Schema.php/class/Schema/9.3.x>`_: provides a means to interact with a schema including management tasks such as adding tables.

Currently Tripal DBX relies on the Drupal PostgreSQL implementations of these classes (`PostgreSQL Connection <https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Driver%21pgsql%21Connection.php/class/Connection/9.3.x>`_ and `PostgreSQL Schema <https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Driver%21pgsql%21Schema.php/class/Schema/9.3.x>`_), although there is structure in place to expand it to other Drupal database drivers.

Tripal DBX Connection
-----------------------

There are two main parts of the Drupal Connection class that the Tripal DBX Api overides:

**A) One Database Connection per Instance:**

Drupal attempts to minimize the number of connections to the database by only creating a new connection if one does not already exist. This makes sense in the context of a single database schema; however, when dealing with multiple schema you will often need to make changes to the search path in order to access the tables of each schema. This API creates an independent connection to the database for each TripalDbxConnection instance which ensures the primary Drupal connection remains unaffected by search path changes.

**B) Table Prefixing in Queries:**

Drupal has always had support for prefixing table names; however, this has been a simple string prepended to the beginning of the table name. This is likely due to many database systems not supporting multiple schema within a single database. In version 2 and 3, Tripal used the native Drupal prefixing to access the Chado database in a separate schema. It did this by using a prefix of "chado." which takes advantage of the PostgreSQL syntax for accessing a specific schema (named "chado" in the previous example). In Tripal version 4, we've created the Tripal DBX API which takes this one step further by extending the native PHP/Drupal PDO database layer to use cross schema focused table prefixing. This allows module developers to access multiple chado and additional Tripal DBX managed schema using the object-oriented query builder as demonstrated in the following example:

.. code-block:: php

   // Open a Connection to the default Tripal DBX managed Chado schema.
   $dbxdb = \Drupal::service('tripal_chado.database');

   // Start a select query on the Chado feature table and assign an alias of x.
   $query = $dbxdb->select('feature', 'x');

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

The above example used a Chado implementation of the Tripal DBX API provided by the ``tripal_chado.database`` service to generate a select query, execute it agains the database focusing on a specific non-Drupal schema and then iterates through the results.

``@todo add documentation for multiple schema.``

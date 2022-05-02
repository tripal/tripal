
Tripal DBX: Generic cross database support for Drupal
========================================================

Tripal has always focused on providing cross database schema support. From the very first version, this support focused on providing integration between Drupal and the GMOD Chado Schema. With Tripal 4, we have made a generic base for our Chado integration which is known as Tripal DBX. This ensures that we can still provide high integration between Drupal and Chado, while also proving a really solid, well-documented API for additional biological data storage options.

Tripal DBX extends the Drupal Database API. Specifically, it extends two core Drupal abstract classes:

 - `\Drupal\Core\Database\Connection <https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Connection.php/class/Connection/9.3.x>`_: a Drupal-specific extension of the PDO database abstraction class in PHP.
 - `\Drupal\Core\Database\Schema <https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Schema.php/class/Schema/9.3.x>`_: provides a means to interact with a schema including management tasks such as adding tables.

Currently Tripal DBX relies on the Drupal PostgreSQL implementations of these classes (`PostgreSQL Connection <https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Driver%21pgsql%21Connection.php/class/Connection/9.3.x>`_ and `PostgreSQL Schema <https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Database%21Driver%21pgsql%21Schema.php/class/Schema/9.3.x>`_), although there is structure in place to expand it to other Drupal database drivers.

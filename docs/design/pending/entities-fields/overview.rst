
Design Summary
================

.. note::

    The names of classes described below are not officially set and design will be updated as this evolves.

The following figure gives a high-level overview of the planned classes and their relationship to the Drupal API:

.. image:: overview.png

While the design is not complete at this point, here is a brief summary of the overall plan.

 - The Drupal ContentBaseEntity will be extended to further support biological data and multiple data sources.
 - We will override the ContentBaseEntity::preSave(), ::postSave() and ::postLoad() methods to move storage handling out of the entity and into a per field implementation.
 - These overridden methods will call the appropriate TripalFieldStorage plugin implementation(s) for the fields attached to a given entity.
 - Each field will indicate it's preferred storage plugin and administrators will have the ability to change the storage plugin used on their site.
 - All TripalFieldStorage plugins will return data using a well documented data array. Controlled vocabularies will play a critical role.
 - Biological data will not be duplicated in the Drupal database.
 - We will create Drupal Console commands to upgrade old Tripal3 Fields to the new architecture.

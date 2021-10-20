Overview
========

Requirements
--------------

1. Multiple data backends per Content Type
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tripal needs to **support multiple data backends on a single Tripal Entity Type; specifically, on a per field basis.**. For example, a SNP entity type should be able to have data from Drupal (application-specific), Chado (biological metadata-specific), VCF (genotypic data), genetic map files, GWAS-related files, etc. This ensures that biological data can be stored in the format which most makes sense for that data whether that be a flat-file format or a database. Furthermore, it reduces data duplication by allowing support for original file formats rather then requiring all data to be imported into a single database.

Unfortunately this requirement is in direct conflict with the new Drupal 9 paradigm of requiring a single data backend per entity type. All existing extension modules are in keeping with this Drupal paradigm (including `External Entities <https://www.drupal.org/project/external_entities>`_). Since this assumption is interwoven in many of the Entity and Content Entity classes, we cannot simply extend the core classes for our design.

2. Tripal Fields control their own data load + save
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As mentioned above, each field should be able to determine it's own data storage. This functionality was available in Tripal 3 and supports easy overriding of data storage through the creation of new fields. For example, multiple groups can create genotypic data fields for genetic marker pages which cater to their specific storage paradigm. Furthermore, this allows fields to only load the data they want to display which is more efficient then the entity needed to load all data to support all fields.

3. Entities + Fields should be vocabulary-focused
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tripal 3 ensured that all Tripal Entity Types and Tripal Fields needed to be associated with a controlled vocabulary term (preferably from a published ontology). This supports better cross database communication through the use of standard ontologies. Additionally, it provides important information for semantic web services by ensuring all data in Tripal is highly typed and these types are well-described with definitions and relationships.

4. Low data duplication
^^^^^^^^^^^^^^^^^^^^^^^^^

Biological data can be quite large, especially when important metadata for each data point is included. As such, we would like to duplicate as little data as possible in order to keep database size manageable.

5. Tripal Fields are easy to extend
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The data types and display for each Tripal site can be extremely diverse depending on their audience. For example, the data needed for a metabolic focused community is quite different from that of a breeding focused community.  As such, fields need to be very easy to extend to allow Tripal sites to support their individual communities data needs.

6. Upgrade path from Tripal 3
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

It needs to be as easy as possible to upgrade Tripal 3 fields, widgets and formatters to Tripal 4. Based on the data diversity we mentioned above, we have seen a huge number of Tripal fields developed for Tripal 3. In order to ease upgrade to Tripal 4 we need to take into account the sheer volume of fields being upgraded and ease the process as much as possible. For reference, here is the `Tripal 3 Field documentation <https://tripal.readthedocs.io/en/latest/dev_guide/custom_field.html>`_.

Design Summary
----------------
.. note::

    The names of classes described below are not officially set and design will be updated as this evolves.

The following figure gives a high-level overview of the classes described here and their relationship to the Drupal API:

.. image:: overview.png


TripalEntity
^^^^^^^^^^^^
The TripalEntity class is an instance of a ContentBaseEntity interface.  It inherits the functionality of a Drupal Entity but allows us to provide Tripal specific customizations. In particular the following functions will be overridden:

- `preSave()`
   - Cache the field data provided by the user.
   - Simplify the biological data to identifying information only for Drupal to store using it's own `EntityStorageInterface`.
   - Note: Caching and then removing the biological data prevents Drupal from duplicating it.
- `postSave()`
    - Pulls the biological field data data from the cache
    - Determines the proper TripalFieldStorage implementation that is needed for each field
    - Calls the proper TripalFieldStorage instance for each field to save the data.
- `postLoad()`
    - Determines the proper TripalFieldStorage implementation that is needed for each field
    - Uses the identifying information Drupal saved for each field
    - Calls the proper TripalFieldStorage instance for each field to load the data.

TripalFieldStorage
^^^^^^^^^^^^^^^^^^
Tripal will have a `TripalFieldStorage` abstract class which is a Drupal interface plugin. It will provide methods to support the following functionality

- `load`: for loading data from the underlying data store
- `save`: for inserting or updating data in the data store
- `delete`: for removing data in the data store

.. note::

    More functionality may be added to this class but for now, the design is focused on these methods.

Example Usage: Chado
^^^^^^^^^^^^^^^^^^^^
The `TripalFieldStorage` class is database agnostic.  However, implementation of this class will be database sepcific.  The `ChadoFieldStorage` class is one such possible implementation where it is responsible for interacting with Chado in a PostgreSQL database.  We currently expect that all interactions with Chado in such a class would occur using the new `BioDB` API that is currently being proposed by Valentin.

.. note::

    As a note, we currently have the Chado API (flat functions) and the ChadoRecord class for interacting with Chado.  While these will remain for backwards compatibilty we anticipate they will be deprecated in favor of the `BioDB` API as it is matured.

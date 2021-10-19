
Entities and Fields Design
============================

This design document is attempting to describe our current design process for Entities and Fields in Tripal 4 utilizing the new Drupal 9 APIs.

Requirements
--------------

1. Multiple data backends per Content Type
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tripal needs to **support multiple data backends on a single Tripal Entitiy Type**. For example, a SNP entitiy type should be able to have data from Drupal (application-specific), Chado (biological metadata-specific), VCF (genotypic data), genetic map files, GWAS-related files, etc. This ensures that biological data can be stored in the format which most makes sense for that data whether that be a flat-file format or a database.

Unfortunatly this requirement is in direct conflict with the new Drupal 9 paradigm of requiring a single data backend per entity type. All existing extension modules are in keeping with this Drupal paradigm (including `External Entities <https://www.drupal.org/project/external_entities>`_). Since this assumption is interwoven in many of the Entity and Content Entity classes, we cannot simply extend the core classes for our design.

2. Tripal Fields control their own data load + save
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Further explanation to come.*

3. Entities + Fields should be vocabulary-focused
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Further explanation to come.*

4. Low data duplication
^^^^^^^^^^^^^^^^^^^^^^^^^

*Further explanation to come.*

5. Tripal Fields are easy to extend
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Further explanation to come.*

6. Upgrade path from Tripal 3
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

*Further explanation to come.*

Design Overview
----------------

*Details to come.*

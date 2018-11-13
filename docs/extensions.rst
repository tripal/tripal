Extension Modules
==================

The below modules are Tripal 3 compatible and grouped roughly by category.

 Please add your module!

.. Instructions for adding a module:
.. Try to stick to the existing categories.
.. Please link to the documentation if available: otherwise, the code is fine.
.. Please write two sentences MAXIMUM about the function of the module.
.. The below template can be used to easily add your module (please remove the .. which indicate a comment, and remove the indentation).

.. Module Name
.. ~~~~~~~~~~~~~~~~~~~~~~~
..
.. This module loads in X, Y, and Z.  It provides admin for A and B, and user area C.
..
.. https://tripal-hq.readthedocs.io/en/latest/index.html

Annotation Modules
-------------------

Tripal Analysis Expression
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A module for loading, annotating, and visualizing NCBI Biosamples and expression data.


https://github.com/tripal/tripal_analysis_expression

Tripal Analysis Blast
~~~~~~~~~~~~~~~~~~~~~~

https://github.com/tripal/tripal_analysis_blast


Tripal Analysis KEGG
~~~~~~~~~~~~~~~~~~~~~

https://github.com/tripal/tripal_analysis_kegg


Tripal Analysis Interpro
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

https://github.com/tripal/tripal_analysis_interpro

Tripal JBrowse
~~~~~~~~~~~~~~

https://github.com/tripal/tripal_jbrowse

Tripal CV-Xray
~~~~~~~~~~~~~~~

Tripal CV-Xray maps content annotations onto controlled vocabulary trees.  The end result is a browseable CV field that lets users explore ontologies and find associated content.

https://github.com/statonlab/tripal_cv_xray

Administrative Modules
------------------------


Tripal Alchemist
~~~~~~~~~~~~~~~~~~~~
Tripal Alchemist allows you to transform entities from one type to another.  Define multiple bundles with the same base table and easily convert existing entities to the new type.

https://github.com/statonlab/tripal_alchemist



Tripal Apollo
~~~~~~~~~~~~~~~~~~~~


Tripal Curator
~~~~~~~~~~~~~~~~~~~~

https://github.com/statonlab/tripal_curator


Tripal ElasticSearch
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


https://github.com/tripal/tripal_elasticsearch


Tripal HeadQuarters
~~~~~~~~~~~~~~~~~~~~

Tripal HeadQuarters (HQ) provides an administrative layer for Tripal, giving users limited access to content creation forms which must be approved by an admin before they are inserted into the database.
Admins can use Chado-specific permissions to define organism or project-specific administrative rights.

https://tripal-hq.readthedocs.io/en/latest/index.html

Developer Tools
----------------

Tripal Fields Generator
~~~~~~~~~~~~~~~~~~~~~~~~

This is a CLI tool to help automate the generation of Tripal fields.

https://github.com/tripal/fields_generator

Tripal Test Suite
~~~~~~~~~~~~~~~~~~

Tripal Test Suite is a composer package that handles common test practices such as bootstrapping Drupal before running the tests, creating test file, creating and managing database seeders (files that seed the database with data for use in testing) and much more.

https://github.com/tripal/TripalTestSuite

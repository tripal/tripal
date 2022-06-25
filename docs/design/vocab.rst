
Controlled Vocabulary Design
==============================

Tripal is very ontology-focused with public terms forming the basis of our content types and fields. This sets us up to support (1) rich semantic web ready web services, (2) detailed definitions for all content displayed to the user, and (3) intuitive and powerful search filters and facets.

The following figure provides an example of the relationships between Vocabularies, ID Spaces, and Terms in Tripal:

.. image:: vocab/vocab-relationship-diagram.png

In order to model the above relationships, we developed the following design:

 - Vocabularies are collections of ID spaces that are stored using implementations of the Tripal Vocabulary Plugin Type.
 - ID spaces are collections of Tripal Terms tied to a single vocabulary that are stored using implementations of the Tripal ID Space Plugin Type.
 - Terms will not be plugins and their storage will be handled by their ID Space.

We used the Drupal Plugin API to make it easy to provide different storage backends for controlled vocabularies.

For more in-depth documentation on this design, check out the following pages:

.. toctree::
   :maxdepth: 1

   vocab/requirements
   vocab/file-structure

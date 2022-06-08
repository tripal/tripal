
Biological Data Storage
=========================

A critical component of Tripal is interfacing between biological data stores and Drupal.

The Tripal Chado core module integrates Drupal with the GMOD Chado schema to provide a great foundation of support for biological data with the ability to store sequence, sequence comparisons, germplasm, phenotypes, genotypes, ontologies, publications, and phylogeny, as well as associated metadata and relations for each of these data types. Integration with Chado allows Tripal to support most biological data types out of the box and use of a common schema allows Tripal sites to share data and Tripal extension modules to be built to enforce best practices.

That said, there are still situations in which you may want to support additional data storage backends such as graph databases, NoSQL databases, or flat files (e.g. variant call format: VCF). Tripal seamlessly supports additional data storage backends through a variety of APIs which will be described here.

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   biodata/tripaldbx
   biodata/chado
   biodata/bulkPgSchemaInstall

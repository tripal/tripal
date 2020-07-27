
GMOD Chado Schema Integration
=================================

.. _GMOD: http://gmod.org/wiki/Main_Page

.. _Chado: http://gmod.org/wiki/Introduction_to_Chado

The Tripal Chado module provides integration between Tripal and the GMOD Chado schema. This provides flexible, default storage for many biological data types including genes, genetic markers, germplasm, as well as associated meta data such as project and analysis details. More specifically:

	Chado is a relational database schema that underlies many GMOD installations. It is capable of representing many of the general classes of data frequently encountered in modern biology such as sequence, sequence comparisons, phenotypes, genotypes, ontologies, publications, and phylogeny. It has been designed to handle complex representations of biological knowledge and should be considered one of the most sophisticated relational schemas currently available in molecular biology. The price of this capability is that the new user must spend some time becoming familiar with its fundamentals.

	--  `GMOD Chado Documentation <https://chado.readthedocs.io/en/rtd/>`_

Chado was selected for Tripal because it is open-source, it is maintained by the community in which anyone can provide input, and use of Chado_ encourages common data storage between online biological sites which decreases duplication of effort.

Chado_ is meant to be installed into a PostgreSQL database and is designed to house a variety of biological data.   For example, Tripal comes with a variety of content types. However, if you want to create new content types you must know how that data will be stored in Chado_.  Additionally, use of the Bulk Loader (a tab-delimited data loader for custom data formats) requires a good understanding of Chado_.  Finally, creating extensions to Tripal requires an understanding of Chado_ to write SQL and or new Tripal fields.  The following links provide training for Chado_.


.. csv-table::
	:header: "Resource", "Link"

	"Chado Home Page", "http://gmod.org/wiki/Chado"
	"Chado Tutorial", "http://gmod.org/wiki/GMOD_Online_Training_2014/Chado_Tutorial"
	"Chado ReadtheDocs", "https://chado.readthedocs.io/en/rtd/"
	"Chado Table List", "https://chado.readthedocs.io/en/rtd/_static/schemaspy_integration/index.html"
	"Chado Best Practices", "http://gmod.org/wiki/Chado_Best_Practices"
	"Chado GitHub", "https://github.com/GMOD/Chado"

Chado Installation
--------------------

When you install the Tripal Chado module you will be automatically prompted to install Chado. This creates a schema within your Drupal database to house the chado tables listed in the resources above. To install chado manually navigate to Structure > Tripal > Data Storage > Chado > Install Chado. Then just choose your version and run the associated Tripal job.

If you need to install chado programatically, use the following service from within a fully bootstrapped Tripal site.

.. code-block:: php
	:caption: Installs Chado version 1.3 in a schema named 'chado'.

	\Drupal::service('tripal_chado.chadoInstaller')->install(1.3, 'chado');

Searching
==========

CartograTree
-------------

CartograTree is a web-based application that allows researchers to identify, filter, compare, and visualize geo-referenced biotic and abiotic data. Its goal is to support numerous multi-disciplinary research endeavors including: phylogenetics, population structure, and association studies.

`Documentation <https://cartogratree.readthedocs.io/en/latest/index.html>`__
`Repository <https://gitlab.com/TreeGenes/CartograTree>`__

Mainlab Chado Search
---------------------

Mainlab Chado Search is a module that enables advanced search function for biological data stored in a Tripal/Chado database. By default, a set of search interfaces are provided, such as 'Gene Search' for searching genes and/or transcripts, 'Marker Search' for searching genetic markers, and 'Sequence Search' for searching any sequences stored in the Chado feature table. Searches for other data types, such as QTL, Map, Trait, Stock, Organism are also provided but may require modification to the materialized view to adjust for site-specific data storage.

`Documentation <https://gitlab.com/mainlabwsu/chado_search/blob/master/README.md>`__
`Repository <https://gitlab.com/mainlabwsu/chado_search>`__

Tripal ElasticSearch
--------------------

The Tripal ElasticSearch module allows you to easily manage the indexing and display of ElasticSearch on your Tripal website. It also easily enables Cross-Site Querying, allowing you to connect to other Tripal sites and provide additional search results to your users.

`Documentation <https://github.com/tripal/tripal_elasticsearch/blob/master/docs/README.md>`__
`Repository <https://github.com/tripal/tripal_elasticsearch>`__

Tripal MegaSearch
---------------------

Tripal MegaSearch is a tool for downloading biological data stored in a Tripal/Chado database. The module was designed to be generic and flexible so it can be used on most Tripal sites. Site administrators may choose from 1) a set of predefined materialized views or 2) chado base tables as the data source to serve data. If neither data source is desired, developers may create their own materialized views and serve them through Tripal MegaSearch via a flexible dynamic query form. This form allows filters to be added dynamically and combined using 'AND/OR' operators. The filters correspond to the underlying data source columns so the user can filter data on each column.

`Documentation <https://gitlab.com/mainlabwsu/tripal_megasearch/blob/master/README.md>`__
`Repository <https://gitlab.com/mainlabwsu/tripal_megasearch>`__

Tripal Sequence Similarity Search
----------------------------------

This module supports sequence similarity search on a Tripal website through a new dual application option. The Tripal module provides access to the speed increase available through Diamond for BLASTP/BLASTX style searches as well as traditional NCBI BLAST for BLASTN. Both applications are integrated into a single interface that provides file upload or copy/paste sequence support for the query and access to formatted databases for NCBI BLAST or Diamond. The target databases can be customized for the categories of whole genome, gene, protein, and transcriptome/unigene. The administration interface allows the admin user to set what pre-indexed databases are available (which show up in a dropdown menu). The module supports execution of the searches on a remote machine so that the search is not running directly on the limited resources typically associated with web servers.

`Documentation <https://github.com/Ferrisx4/tripal_diamond/blob/master/README.md>`__
`Repository <https://github.com/Ferrisx4/tripal_diamond>`__

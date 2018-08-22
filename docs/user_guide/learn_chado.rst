Learn Chado
===============

.. _GMOD: http://gmod.org/wiki/Main_Page

.. _Chado: http://gmod.org/wiki/Introduction_to_Chado

.. note::

  While you can use Tripal out-of-the-box for a whole genome or transcriptome based website, You will need a good understanding of Chado_ to expand into other data types and to take full advantage of Tripal


The primary data store for Tripal is Chado_.  Chado_ is an open-source database schema managed by GMOD_.  Chado was selected for Tripal because it is open-source, it is maintained by the community in which anyone can provide input, and use of Chado_ encourages common data storage between online biological sites which decreases duplication of effort.

Chado_ is meant to be installed into a PostgreSQL database and is designed to house a variety of biological data.   For example, Tripal comes with a variety of content types. However, if you want to create new content types you must know how that data will be stored in Chado_.  Additionally, use of the Bulk Loader (a tab-delimited data loader for custom data formats) requires a good understanding of Chado_.  Finally, creating extensions to Tripal requires an understanding of Chado_ to write SQL and or new Tripal fields.  The following links provide training for Chado_.


.. csv-table::
  :header: "Resource", "Link"

  "Chado Home Page", "http://gmod.org/wiki/Chado>"
  "Chado Tutorial", "http://gmod.org/wiki/Main_Page>"
  "Chado Table List", "http://gmod.org/wiki/Chado_Tables>"
  "Chado Best Practices", "http://gmod.org/wiki/Chado_Best_Practices>"

What is Tripal?
--------------
Tripal is a collection of open-source freely available Drupal modules 
and is a member of the GMOD family of tools. Tripal serves as a web 
interface for the GMOD Chado database and is designed to allow anyone 
with genomic data to quickly create an online genomic database using 
community supported tools.


Features
--------------
 - a Chado installer
 - Data loaders for ontologies (controlled vocabularies), GFF files, 
   and FASTA files
 - Generic Data Loader Modules allows for creation of custom loading 
   templates
 - Drupal nodes (web pages) are automatically generated for organisms,
   genomic features, biological libraries, and stocks
 - Web pages can be enriched with analysis results from BLAST, 
   KAAS/KEGG, InterProScan, and Gene Ontology (GO)
 - Views Integration allows for custom listings of data
 - Content pieces exposed as blocks allowing the use of Panels for 
   custom layouts of Tripal Nodes


Required Modules
--------------
 - Drupal 6.x (work is currently underway for a 7.x compatible 
   version)
 - Drupal Core Modules: Search and Path
 - Database containing GMOD Chado Schema (can be installed by the 
   Tripal Core module)
NOTE: A PostgreSQL database is required for installation of the 
Chado Schema

Highly Recommended Modules
 - Views 2.x (Views 3.x compatible version already exists in 6.x-0.4-dev)
 - Views Data Export


Installation
--------------
Please follow the online tutorial for installation instructions:
http://www.gmod.org/wiki/Tripal_Tutorial_v1.1


Customization
--------------
Tripal can be used “as is” but also allows for complete customization.
PHP-based template files are provided for all data types to allow for 
precise customizations as required by the community. A well-developed 
Tripal API provides a uniform set of variables and functions for 
accessing any and all data within the Chado database.


Future Work
--------------
Currently, Tripal only supports a large subset of the 
current Chado schema, but further development is underway. Meanwhile, 
others can use the Tripal API to develop their own extensions. Those 
extensions can in turn be made available for anyone to use. These 
custom extensions, the Tripal package, and access to support resource 
such as an active mailing list can be found on the Tripal website 
(http://tripal.sourceforge.net).

For more information, see the recent publication:

Stephen P. Ficklin, Lacey-Anne Sanderson, Chun-Huai Cheng, Margaret 
Staton, Taein Lee, Il-Hyung Cho, Sook Jung, Kirstin E Bett, Dorrie 
Main. Tripal: a construction Toolkit for Online Genome Databases. 
Database, Sept 2011. Vol 2011. 

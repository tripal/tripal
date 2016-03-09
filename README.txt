What is Tripal?
--------------
Tripal is a collection of open-source freely available Drupal modules 
and is a member of the GMOD family of tools. Tripal serves as a web 
interface for the GMOD Chado database and is designed to reduce the
time and cost required for construction of an online genomic, genetic
and breeding database.


Features
--------------
 - a Chado installer
 - Data loaders for ontologies (controlled vocabularies), GFF files, 
   and FASTA files, publications (from PubMed and AGIRCOLA). 
 - Generic Bulk Data Loader Modules allows for creation of custom 
   loading templates.
 - Drupal nodes (web pages) are automatically generated for organisms,
   genomic features, biological libraries, and stocks
 - Supports creation of materialized views for faster data queries.
 - Display templates are provided for all content types for
   easier customization.
 - Views Integration allows for custom listings of data
 - Content pieces exposed as blocks allowing the use of Panels for 
   custom layouts of Tripal Nodes


Required Modules
--------------
 - Drupal 7.x 
 - Drupal Core Modules: Search, Path and PHP modules.
 - Drupal contributed modules: Views
 - Database containing GMOD Chado Schema (can be installed by the 
   Tripal Core module)
NOTE: A PostgreSQL database is required for installation of the 
Chado Schema

Installation
--------------
Please follow the instructions in the online Tripal User's Guide:
http://tripal.info/tutorials/v2.0/installation


Upgrade from Tripal v2.x to v3.x
---------------------------------
Note:  Upgrade can only be performed using 'drush' command.

Step 1: Put the site in maintenance mode.
Step 2: Remove old Tripal v2 package and replace with Tripal v3 package
Step 3: Enable the tripal module

  drush pm-enable tripal
 
Step 4:  

  drush pm-enable tripal_chado

Note: Use of deprecated API functions from Tripal v1 will cause a template
to not work because Tripal v1 deprecated API fucntions have been removed
in Tripal v3.

Customization
--------------
Tripal can be used “as is” but also allows for complete customization.
PHP-based template files are provided for all data types to allow for 
precise customizations as required by the community. A well-developed 
Tripal API provides a uniform set of variables and functions for 
accessing any and all data within the Chado database.

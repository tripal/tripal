[![7.x-3.x Build Status](https://travis-ci.org/tripal/tripal.svg?branch=7.x-3.x)](https://travis-ci.org/tripal/tripal)

![alt tag](https://raw.githubusercontent.com/tripal/tripal/7.x-3.x/tripal/theme/images/tripal_logo.png)

Tripal is a toolkit for construction of online biological (genetics, genomics,
breeding, etc), community database, and is a member of the 
[GMOD](http://www.gmod.org) family of tools. Tripal v3 provides by default
integration with the [GMOD Chado database](http://gmod.org/wiki/Chado_-_Getting_Started).
Tripal's primary goals are: 

Genomics, genetics, breeding and other biological data are increasingly complicated and time consuming to publish online for other researchers to search, browse and make discoveries.   Tripal provides a framework to reduce the complexity of creating such a site, and provides access to a community of similar groups that share community-standards, and interact to address questions and learn best practices for sharing, storing and visualizing complex biological data.

1. Provide a framework for those with genomic, genetic and breeding data that
can facility creation of an online site for display, search and visualization.
2. To use community-derived standards and ontologies to facility continuity
between sites which in turn fosters collaboration and sharing 
3. Provide an out-of-the-box setup for a genomics site for those who simply 
want to put new genome assemblies and annotations online.
4. Provide Application Programming Interfaces (APIs) for complete customization 
such that more advanced displays, look-and-feel, and new functionality
can be supported. 


# Features
The following major features
are available in Tripal v3.

* Tripal v3's design is centered around controlled vocabularies and ontologies. 
  This allows for greater integration with the semantic web and will help
  support data exchange between Tripal sites.
* RESTful web services.  Tripal v3 introduces RESTful web services for Tripal.
  The resources provided by these web services uses JSON-LD and WC3 Hydra 
  vocabulary to deliver content. 
* Tripal v3 introduces new content pages. In older versions of Tripal all 
  content was provided via Drupal "nodes".  Now content is delivered using
  new content types (e.g. gene, genetic_marker, organism, germplasm, etc.)
  and the site admin controls which content types are available on the site. 
* Chado support:
  * Tripal v3 represents a major redesign from previous versions.  Previously,
    Chado was the only storage backend supported. Tripal v3 provides by default
    support for Chado, but also sports a new design that affords integration of
    other storage backends (including noSQL options).  
  * A Chado v1.2 or v1.3 installer
  * Data loaders for ontologies (controlled vocabularies), GFF files, and 
    FASTA files, publications (from PubMed and AGIRCOLA). 
  * Generic Bulk Data Loader Modules allows for creation of custom loaders 
    without programming (requires an understanding of Chado). 
  * Supports creation of materialized views for faster data queries.


# Required Dependencies
* Drupal: 
  * Drupal 7.x
  * Drupal core modules: Search, Path and PHP modules.
  * Drupal contributed modules: 
    * [Views](http://drupal.org/project/views)
    * [Entity API](http://drupal.org/project/entity)
* PostgreSQL
* PHP 5.5+
* UNIX/Linux


# Installation
Please follow the instructions in the online Tripal User's Guide:
http://tripal.info/tutorials/v2.0/installation


# Upgrade from Tripal v2.x to v3.x
Note:  Upgrade can only be performed using 'drush' command.

Note: Deprecated API functions from Tripal v1.x have been removed from Tripal
v3.  Therefore, use of deprecated API functions in templates or custom 
modules may cause a white screen of death (WSOD).  Check teh server logs if this
occurs to find where deprecated functions may be used.

Upgrade Instructions:

Step 1: Put the site in maintenance mode.

Step 2: Disable tripal modules. Disabling the core module will disable all
other Tripal modules:

  drush pm-disable tripal_core
  
Step 3: Remove old Tripal v2 package and replace with Tripal v3 package
Step 4: Enable the tripal module

  drush pm-enable tripal
 
Step 5: Enable the tripal_chado module  

  drush pm-enable tripal_chado
  
Step 6:  Tripal v2 modules are now called 'legacy modules'. these are the
modules that were disabled in step #2.  For backwards compatibility, you 
should re-enable these modules:

  drush pm-enable tripal_core, tripal_views, tripal_db, tripal_cv, \
    tripal_analysis, tripal_organism, tripal_feature, tripal_pub, \
    tripal_stock

Be sure to enable any additional modules not included in the example
drush command above.

Step 7:  Return to your Tripal site, and click the link that appears for
preparing Chado and launch the job.


# Customization
Tripal can be used “as is” but also allows for complete customization.
PHP-based template files are provided for all data types to allow for 
precise customizations as required by the community. A well-developed 
Tripal API provides a uniform set of variables and functions for 
accessing any and all data within the Chado database. See the Tripal 3.x
Developer's Handbook for additional details.


# Development Testing

To run PHP unit tests on your local system, simply create a `.env` file in your `/Tests/` directory that defines the `DRUPAL_ROOT` variable, for example 

```
DRUPAL_ROOT=/var/www/html
```
Then run PHPUnit from your root Tripal directory.

PHPUnit tests will also be run in the Travis CI build.
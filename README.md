[![7.x-3.x Build Status](https://travis-ci.org/tripal/tripal.svg?branch=7.x-3.x)](https://travis-ci.org/tripal/tripal)
[![All Contributors](https://img.shields.io/badge/all_contributors-1-orange.svg?style=flat-square)](#contributors)
[![Documentation Status](https://readthedocs.org/projects/tripal/badge/?version=latest)](https://tripal.readthedocs.io/en/latest/?badge=latest)

[![DOI](https://zenodo.org/badge/42666405.svg)](https://zenodo.org/badge/latestdoi/42666405)


![alt tag](https://raw.githubusercontent.com/tripal/tripal/7.x-3.x/tripal/theme/images/tripal_logo.png)

Tripal is a toolkit for constructing online biological (genetics, genomics, breeding, etc.) community databases, and Tripal is a member of the [GMOD](http://www.gmod.org) family of tools. **Tripal v3** provides integration with the [GMOD Chado database](http://gmod.org/wiki/Chado_-_Getting_Started) by default.

Genetics, genomics, breeding, and other biological data are increasingly complicated and time-consuming to publish online for others to search, browse and make discoveries with. Tripal provides a framework to reduce the complexity of creating such a site, and provides access to a community of similar groups that share community-standards. The users of Tripal are encouraged to interact to address questions and learn the best practices for sharing, storing, and visualizing complex biological data.

The primary goals of Tripal are to:
1.	Provide a framework for creating sites that allow display, search, and visualization of biological data, including genetics, genomics, and breeding data;
2.	Use community-derived standards and ontologies to facilitate continuity between sites and foster collaboration and sharing;
3.	Provide an out-of-the-box setup for a genomics site to put new genome assemblies and annotations online; and
4.	Provide Application Programming Interfaces (APIs) to support customized displays, look-and-feel, and new functionality.


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
Please follow the instructions in the online Tripal User's Guide for [Tripal v2](https://tripal.info/tutorials/v2.x/installation) or [Tripal v3](https://tripal.readthedocs.io/en/latest/user_guide.html).


# Upgrade from Tripal v2.x to v3.x
Please follow the [Upgrade Instructions](https://tripal.readthedocs.io/en/latest/user_guide/install_tripal/upgrade_from_tripal2.html) in the Tripal v3 User's Guide


# Customization
Tripal can be used “as is” but also allows for complete customization.
PHP-based template files are provided for all data types to allow for 
precise customizations as required by the community. A well-developed 
Tripal API provides a uniform set of variables and functions for 
accessing any and all data within the Chado database. See the Tripal 3.x
Developer's Handbook for additional details.


# Development Testing

To run PHP unit tests on your local system, run `composer install` to install developer-specific requirements.  Next, create a `.env` file in your `/Tests/` directory that defines the `DRUPAL_ROOT` variable, for example 

```
DRUPAL_ROOT=/var/www/html
```
Then run PHPUnit from your root Tripal directory.

PHPUnit tests will also be run in the Travis CI build.

Read our [testing guidelines](tests/README.md)

## Contributors

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore -->
<table><tr><td align="center"><a href="https://github.com/spficklin"><img src="https://avatars0.githubusercontent.com/u/1719352?v=4" width="100px;" alt="Stephen Ficklin"/><br /><sub><b>Stephen Ficklin</b></sub></a><br /><a href="https://github.com/tripal/tripal/commits?author=spficklin" title="Code">💻</a></td></tr></table>

<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!
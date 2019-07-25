[![7.x-3.x Build Status](https://travis-ci.org/tripal/tripal.svg?branch=7.x-3.x)](https://travis-ci.org/tripal/tripal)

![alt tag](https://raw.githubusercontent.com/tripal/tripal/7.x-3.x/tripal/theme/images/tripal_logo.png)

Tripal is a toolkit for constructing online biological (genetics, genomics, breeding, etc.) community databases, and Tripal is a member of the [GMOD](http://www.gmod.org) family of tools. **Tripal v3** provides integration with the [GMOD Chado database](http://gmod.org/wiki/Chado_-_Getting_Started) by default.

Genetics, genomics, breeding, and other biological data are increasingly complicated and time-consuming to publish online for others to search, browse and make discoveries with. Tripal provides a framework to reduce the complexity of creating such a site, and provides access to a community of similar groups that share community-standards. The users of Tripal are encouraged to interact to address questions and learn the best practices for sharing, storing, and visualizing complex biological data.

The primary goals of Tripal are to:
1.	Provide a framework for creating sites that allow display, search, and visualization of biological data, including genetics, genomics, and breeding data;
2.	Use community-derived standards and ontologies to facilitate continuity between sites and foster collaboration and sharing;
3.	Provide an out-of-the-box setup for a genomics site to put new genome assemblies and annotations online; and
4.	Provide Application Programming Interfaces (APIs) to support customized displays, look-and-feel, and new functionality.


# Features
TBD


# Required Dependencies
* Drupal:
  * Drupal 8.x
  * Drupal core modules: Search, Path, View, Entity, and PHP modules.
  * Drupal contributed modules:
* PostgreSQL
* PHP 7.1+
* UNIX/Linux


# Installation


# Upgrade from Tripal v3.x to v4.x



# Customization


# Development Testing

See the [Drupal "Running PHPUnit tests" guide](https://www.drupal.org/node/2116263) for instructions on running tests on your local environment. In order to ensure our Tripal functional testing is fully bootstrapped, tests should be run from Drupal core. Specifically, in your Drupal root run the following command from your drupal root to run all Tripal tests.

```
./vendor/bin/phpunit modules/t4d8/tripal/tests/
```

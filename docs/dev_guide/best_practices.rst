Module Development Best Practice
================================


If you create custom Tripal Modules, here are some best practices and suggestions.

The Drupal Devel Module
-----------------------


Before staring your development work, it is suggested that you download and install the `Drupal devel module <https://drupal.org/project/devel>`_. This module helps greatly with debugging your custom theme or module. A very useful function of this module is the dpm function. You can use the dpm function to print to the web page an interactive view of the contents of any variable. This can be extremely helpful when accessing Chado data in objects and arrays returned by Tripal.

Add your module to Tripal.info
------------------------------

Add your modules to the Tripal ReadtheDocs :doc:`../extensions` list. The :doc:`../extensions/module_rating` was designed to give guidance on Tripal module development best practices.


Coding Best Practices
---------------------

Host your code on GitHub
^^^^^^^^^^^^^^^^^^^^^^^^

We recommend making your code open source and hosting it on GitHub. It’s free, it let’s people easily find, use, and contribute to your source code.

Associate the GitHub repository with Tripal
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Once your module is on GitHub, consider joining the Tripal organization. Your lab group can exist as a team and maintain control over your code, but your projects will be listed in the main Tripal group.

If you’d rather not, you can still tag your project as Tripal by clicking on the Manage Topics Link at the top of your repository.

DOIs
^^^^

When your module is release ready, why not create a Digital Object Identifier (DOI) for it with `Zenodo <https://zenodo.org/>`_? It’s free! Sync your github account and create a new release (Zenodo won’t find old releases). You can then display your DOI badge on your module’s page.

Additionally, there is a `Tripal Community group <https://zenodo.org/communities/tripal/>`_ on Zenodo. You can edit your record to associate your DOI with the Tripal community.

Testing and Continuous Integration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

`Tripal Test Suite <https://github.com/statonlab/TripalTestSuite>`_ is a full-featured testing module that makes writing tests much easier. which will automatically set up a PHPUnit and Travis testing environment for you.

* Test with PHPUnit
* Run tests as you push code with Travis CI


Documentation
^^^^^^^^^^^^^

Every repository can include a README file that will be displayed on the repository page. A README file should at a minimum include:

* An overview of the module
* Instructions on how to install & use the module

Consider documenting your Code itself. Tripal documents in the `Doxygen style <http://www.stack.nl/~dimitri/doxygen/>`_ which allows documentation webpages to be automatically generated. Even if you don’t build HTML documentation, the in-line code documentation will be very helpful to contributors.

Coding Standards
^^^^^^^^^^^^^^^^

Drupal has defined `coding standards <https://www.drupal.org/docs/develop/standards/coding-standards>`_ that Tripal modules should meet.

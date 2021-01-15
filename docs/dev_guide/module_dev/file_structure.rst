Module File Structure
========================

All Tripal extension modules require a module folder and a ``.info.yml`` file. With just these two items the module will display in Drupal 8's Extend administration page or can be activated directly with drush.

Choosing a module name
-------------------------

Choose a module machine name that is descriptive, short and unique. It is always a good idea to check out the `Tripal Extensions module list <https://tripal.readthedocs.io/en/latest/extensions.html>`_ to ensure you module name has not already been used. You module machine name must also meet the following rules:

 - It must start with a letter.
 - It must contain only lower-case letters and underscores.
 - It must not contain any spaces.
 - It must be unique. Your module should not have the same short name as any other module, theme, or installation profile you will be using on the site.
 - It should not be any of the reserved terms : src, lib, vendor, assets, css, files, images, js, misc, templates, includes, fixtures, drupal.

It is also a good idea to ensure your module name encompasses the full functionality you would like to develop. For example, while your current goal may be importing a specific file format, you are likely to want to develop customized display through Tripal fields in the future. As such, you would want to stay away from ``my_file_format_importer`` and go with something more general like ``my_data_type``. We also recommend you prefix your module name with a short identifier for you lab. This will ensure your module name is unique.

Prepare a module skeleton
---------------------------

Start by creating a folder for your module in the modules directory of your Tripal site. This folder should use the machine name you choose above and includes all the files describing the functionality of your module. `Read more regarding this containing folder <https://www.drupal.org/docs/8/creating-custom-modules/naming-and-placing-your-drupal-8-module#s-create-a-folder-for-your-module>`_

Next we let Drupal know about our module by describing it in an ``.info.yml`` file. The structure of this file is quite simple but descriptive:

.. code:: YAML

  name: Hello World Module
  description: Creates a page showing "Hello World".
  package: Custom

  type: module
  core: 8.x

  dependencies:
   - tripal

Not only does this file let Drupal know about your module so you can enable it in your Tripal site but it also provides information to the site administrator. There are additional keys for this file; the ones above are the most common. `Read more on the Drupal info.yml file <https://www.drupal.org/node/2000204>`_

Once you have added an ``.info.yml`` file, you can navigate to your Drupal site in the browser, go to “Extend” in the administration toolbar at the top. Your module will now appear in this list and checking the checkbox enables it! If this doesn’t happen, `read some great debugging tips here <https://www.drupal.org/docs/8/creating-custom-modules/let-drupal-8-know-about-your-module-with-an-infoyml-file#debugging>`_.

Directory Structure
---------------------

This section will explain the typical directory structure of a Tripal 4 extension module. These directories follow Drupal standards and the structure is often necessary for your classes to be automatically discovered.

As mentioned when preparing a module skeleton above, your entire module will be contained within a directory named using the machine name of your module. Within that base directory are the following:

 - ``config``: contains files defining default configuration including variables and schema.
 - ``src``: contains the bulk of your module including controllers, forms, fields and blocks.
 - ``templates``: contains your Twig template files for modifying display of fields and pages.
 - ``tests``: contains your automated phpunit tests.

The sub-directories and files within are described in the following image:

.. image:: file_structure.directory.png

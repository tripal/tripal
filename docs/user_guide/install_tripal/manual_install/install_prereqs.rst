Tripal Prerequisites
====================

.. note::

  Remember you must set the ``$DRUPAL_HOME`` environment variable if you want to cut-and-paste the commands below. See :doc:`../drupal_home`


Tripal v3.x requires several Drupal modules. These include  `Entity <https://www.drupal.org/project/entity>`_,  `Views <https://www.drupal.org/project/views>`_, `CTools <https://www.drupal.org/project/ctools>`_, `Display Suite <https://www.drupal.org/project/ds>`_, `Field Group <https://www.drupal.org/project/field_group>`_, `Field Group Table <https://www.drupal.org/project/field_group_table>`_, `Field Formatter Class <https://www.drupal.org/project/field_formatter_class>`_ and `Field Formatter Settings <https://www.drupal.org/project/field_formatter_settings>`_ modules.   Modules can be installed using the graphical Drupal website by clicking on the Modules link in the top adminstrative menu bar.  Instructions for instaling Modules via the web-interface can be found here:  https://www.drupal.org/documentation/install/modules-themes/modules-7. However, Drush can be quicker for module installation. The following instructions will show how to install a module using the Drush command-line tool.

First, install the Entity module.  We will download the current version using the drush command. On the command-line, execute the following:

.. code-block:: bash

  cd $DRUPAL_HOME/sites/all/modules
  drush pm-download entity

Typically for all module installation we should check the README for additional installation instructions. Next, enable the module using a drush command:

.. code-block:: bash

  drush pm-enable entity

For basic Tripal functionality you must also enable the Views and CTools modules. You can specify as many module as desired on one line:

.. code-block:: bash

  drush pm-download views ctools
  drush pm-enable views views_ui ctools

Finally, Tripal works best when it can provide default display layouts.   To support default layouts you must also enable the remaining dependencies:

.. code-block:: bash

  drush pm-download ds field_group field_group_table field_formatter_class field_formatter_settings
  drush pm-enable ds field_group field_group_table field_formatter_class field_formatter_settings

Optionally, you can install the ckeditor module.  This module provides a nice WYSIWYG editor that allows you to edit text on your site using a graphical editor. Otherwise, if you need images or formatting (e.g. underline, bold, headers) you would be required to write HTML.  It is recommended that this module be installed to improve the user experience:

.. code-block:: bash

  drush pm-download ckeditor
  drush pm-enable ckeditor
  
Finally, we need an more recent version of JQuery that what comes with Drupal.  We can get this by installing the JQuery update module.

.. code-block:: bash

  drush pm-download jquery_update
  drush pm-enable jquery_update

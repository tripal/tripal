
Installation
==============

Drupal Console
----------------

Drupal console is installed using composer for each Drupal/Tripal site you would like to use it on. It is great both as a development tool and an administrative tool so there is reason to install on both your development and production sites.

Please see the `Official Installation Documentation <https://drupalconsole.com/docs/en/getting/composer>`_.

Tripal Console
-----------------

Tripal console is a Drupal extension module and is distributed with Tripal. As such, if you have already installed Tripal then you are most of the way there! Simply use Drush or the Drupal Administrative Interface to enable the module.

.. code::

  cd [drupal root]
  drush pm:enable tripal_console -y

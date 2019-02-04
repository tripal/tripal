Drush Installation
==================

Drush is a command-line utility that allows for non-graphical access to the Drupal website. You can use it to automatically download and install themes and modules, clear the Drupal cache, upgrade the site and more. Tripal v3 supports Drush. For this tutorial we will use Drush and therefore we want the most recent, Drupal7-compatible version installed: we recommend Drush 8.x (see compatibility chart below.)

==============  =============  ==========  ===========================
Drush Version   Drush Branch   PHP         Compatible Drupal versions
==============  =============  ==========  ===========================
Drush 9         master 	       5.6+ 	     D8.4+
Drush 8         8.x 	         5.4.5+      D6, D7, D8.3
Drush 7         7.x 	         5.3.0+      D6, D7
Drush 6         6.x 	         5.3.0+      D6, D7
Drush 5         5.x 	         5.2.0+      D6, D7
==============  =============  ==========  ===========================

*As you can see from the above table, the newest version of Drupal which supports Drupal 7 is Drush 8.*

Install Drush
-------------

The official documentation for installing Drush 8 can be found here: https://docs.drush.org/en/8.x/install/.

.. warning::

  Don't accidentally follow the Drupal 8 installation method for your Drupal 7 site!  The "site-local" Drush installation won't work for Drupal 7.


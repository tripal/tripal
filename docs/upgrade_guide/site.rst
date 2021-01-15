
Upgrading a Tripal 3 site
===========================

The upgrade path is still under development. More information will be added here as it becomes available.

**What we know so far:**

 - Upgrading from Drupal 7 to 8+ requires a migration.

    - This means you will create a local copy of your current Drupal 7 site, a new Drupal 8+ site and then use the Drupal Migration module to transfer your data from the Drupal 7 site to the new Drupal 8 one.
    - This process ensures unused or old configuration from previous upgrades is not transferred to your new site.
    - More information can be found here: `Upgrading from Drupal 6 or 7 to Drupal 8 (and newer) <https://www.drupal.org/docs/upgrading-drupal/upgrading-from-drupal-6-or-7-to-drupal-8-and-newer>`_
    - Drupal has provided the following documentation to prepare for a migration: `How to prepare your Drupal 7 or 8 site for Drupal 9 <https://www.drupal.org/docs/upgrading-drupal/how-to-prepare-your-drupal-7-or-8-site-for-drupal-9>`_

 - Upgrading from Tripal 3 to 4 will also use the Drupal migration functionality.
 - Only Chado 1.3 will be supported in Tripal 4 so you need to upgrade Chado first.
 - No re-loading or changing of Chado data will be required by the migration.

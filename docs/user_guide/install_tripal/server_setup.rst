Server Setup
===============

Before installation of Tripal, a web server must be configured and ready, and Tripal requires the following dependencies:

1. A UNIX-based server (e.g. Ubuntu Linux or CentOS are the most popularly used).
2. Web server software. The `Apache web server <https://httpd.apache.org/>`_ is most commonly used.
3. `PHP <http://php.net/>`_ version 5.6 or higher (the most recent version is recommended).
4. `PostgreSQL <https://www.postgresql.org/>`_ 9.3 or higher (9.5 required for Chado 1.2 to 1.3 upgrade)
5. `Drush <http://www.drush.org/en/master/>`_ 7 or higher
6. `Drupal <https://www.drupal.org/>`_ 7.

.. warning::

  PHP 7.2 is not fully compatible with Drupal.

The following sections provide step-by-step instructions to help setup either an Ubuntu or CentOS system.

.. toctree::
   :maxdepth: 1

   ./server_setup/ubuntu_18.04
   ./server_setup/ubuntu_16.04
   ./server_setup/centos_7

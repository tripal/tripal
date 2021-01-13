Tripal Docker
================

Tripal Docker is currently focused on Development and Unit Testing. There will be a production focused Tripal Docker soon.

Software Stack
------------------

Currently we have the following installed:
 - Debian Buster(10)
 - PHP 7.3.25 with extensions needed for Drupal (Memory limit 1028M)
 - Apache 2.4.38
 - PostgreSQL 11.9 (Debian 11.9-0+deb10u1)
 - Composer 2.0.7
 - Drupal Console 1.9.7
 - Drush 10.3.6
 - Drupal 8.9.10  (8.x-dev) downloaded using composer.

Quickstart
------------

1. Run the image in the background mapping it's web server to your port 9000.

    a) Stand-alone container for testing or demonstration.

    .. code::

      docker run --publish=9000:80 --name=t4d8 -tid tripalproject/tripaldocker:latest

    b) Development container with current directory mounted within the container for easy edits. Change my_module with the name of yours.

    .. code::

      docker run --publish=9000:80 --name=t4d8 -tid --volume=`pwd`:/var/www/drupal8/web/modules/contrib/my_module tripalproject/tripaldocker:latest

2. Start the PostgreSQL database.

.. code::

  docker exec t4d8 service postgresql start


Development Site Information:
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

+-------------------------+-----------------------+
| URL                     | http://localhost:9000 |
+-------------------------+-----------------------+
| Administrative User     | drupaladmin           |
+-------------------------+-----------------------+
| Administrative Password | some_admin_password   |
+-------------------------+-----------------------+


Usage
----------

 - Run Drupal Core PHP Unit Tests:

   .. code::

    docker exec --workdir=/var/www/drupal8/web/modules/contrib/tripal t4d8 phpunit

 - Run Drupal Console to generate code for your module!

   .. code::

    docker exec t4d8 drupal generate:module

 - Run Drush to rebuild the cache

   .. code::

    docker exec t4d8 drush cr

 - Run Composer to upgrade Drupal

   .. code::

    docker exec t4d8 composer up

Detailed Setup for Core Development
------------------------------------

1. `Install Docker <https://docs.docker.com/get-docker>`_ for your system.


2. Clone the most recent version of Tripal 4 keeping track of where you cloned it.

  .. code-block:: bash

    mkdir ~/Dockers
    cd ~/Dockers
    git clone https://github.com/tripal/t4d8

3. Create a docker container based on the most recent TripalDocker image with your cloned version of Tripal4 mounted inside it.

  .. code-block:: bash

    cd t4d8
    docker run --publish=9000:80 --name=t4d8 -tid --volume=`pwd`:/var/www/drupal8/web/modules/contrib/tripal tripalproject/tripaldocker:latest

  The first time you run this command you will see ``Unable to find image 'tripalproject/tripaldocker:latest' locally``. This is not an error! It's just a warning and the command will automatically pull the image from the docker cloud.

  So, what does this command mean? I'll try to explain the parts below for users new to docker. If you are familiar with docker, feel free to ignore the next points!

   - The ``docker run`` command creates a container from a docker image. You can think of a dockerfile as instructions, an image as an OS and a container as a running machine.
   - The ``--name=t4d8`` is how you will access the container later using ``docker exec`` commands as shown in the usage section.
   - The ``-tid`` part runs the container in the background with an interactive terminal ready to be accessed using exec.
   - The ``--publish=9000:80`` opens port 9000 on your computer and ensures when you access localhost:9000 you will see the website inside the container.
   - The ``--volume=[localpath]:[containerpath]`` ensures that your local changes will be sync'd with that directory inside the container. This makes development in the container a lot easier!

  The command above was written for linux or mac users. Here is some information for Windows users.
   - For Windows users the above command will not works as written. Specifically, the ``pwd`` needs to be replaced with the absolute path in including the t4d8 directory.

   .. code-block:: bash

    docker run --publish=9000:80 --name=t4d8 -tid --volume=C:\Users\yourusername\Dockers\t4d8:/var/www/drupal8/web/modules/contrib/tripal tripalproject/tripaldocker:latest``

4. Start the PostgreSQL database.

  .. code-block:: bash

    docker exec t4d8 service postgresql start

**This will create a persistent Drupal/Tripal site for you to play with! Data is stored even when your computer restarts and Tripal will already be enabled with Chado installed.**

**Furthermore, the --volume part of the run command ensures any changes made in your local directory are automatically copied into the docker container so you can live edit your website.**

Troubleshooting
---------------

The provided host name is not valid for this server.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
On my web browser, I got the message "The provided host name is not valid for this server".

**Solution:** It is most likely because you tried to access the site through a URL different from ``localhost`` or ``127.0.0.1``. For instance, if you run docker on a server and want to access your d8t4 site through that server name, you will have to edit the settings.php file inside the docker (at the time writting this, it would be everytime you (re)start the docker) and change the last line containing the parameter ``$settings[trusted_host_patterns]``:

.. code::

  docker exec -it t4d8 vi /var/www/drupal8/web/sites/default/settings.php

For instance, if your server name is ``www.yourservername.org``:

.. code::

  $settings[trusted_host_patterns] = [ '^localhost$', '^127\.0\.0\.1$', '^www\.yourservername\.org$', ];

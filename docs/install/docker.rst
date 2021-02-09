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

Not seeing recent functionality or fixes.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

As Tripal 4 is currently under rapid development, this could be due to not using the most up to date docker image available. The following instructions can be used to confirm you are using the most recent image.

.. code-block:: bash

  docker rm --force t4d8
  docker rmi tripalproject/tripaldocker:latest
  docker pull tripalproject/tripaldocker:latest

At this point, you can follow up with the appropriate ``docker run`` command. If your run command mounts the current directory through the ``--volume`` parameter then make sure you are in a copy of the t4d8 repository on the main branch with the most recent changes pulled.

Testing install for a specific branch or update the docker image.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following instructions will show you how to create the TripalDocker image from the code existing locally. **This should only be needed if you have made changes to Tripal 4 that impact the installation process and/or if you have created a new Tripal release. Otherwise, you should be able to use the image from docker hub accessed via the docker pull command.**

First if you do not have a local copy of the t4d8 repository, you can use the following instructions to get one. If you do have a copy already, make sure it is up to date and contains the changes you would like to test.

.. code-block:: bash

  mkdir ~/Dockers
  cd ~/Dockers
  git clone https://github.com/tripal/t4d8

Next, you use the `docker build <https://docs.docker.com/engine/reference/commandline/build/>`_ command to create an image from the existing TripalDocker Dockerfile. Since we are testing Tripal 4 on multiple versions of Drupal, you can set the Drupal major version using the drupalversion argument as shown below. The version of Drupal used for the latest tag is the default value of the argument in the Dockerfile.

.. code-block:: bash

  cd t4d8
  docker build --tag=tripalproject/tripaldocker:drupal9.0.x-dev --build-arg drupalversion='9.0.x' tripaldocker/

This process will take a fair amount of time as it completely installs Drupal, Tripal and PostgreSQL. You will see a large amount of red text but hopefully not any errors. You should always test the image by running it before pushing it up to docker hub!

.. note::

  Make sure the drupal version specified in the tag matches the build argument. The value of ``drupalversion`` must match one of the available tags on `Packagist drupal/core <https://packagist.org/packages/drupal/core>`_.

.. warning::

  If your new changes to Tripal 4 break install, you will experience one of the following depending on the type of error:

  1. The build command executed above will not complete without errors.
  2. When you run the image after it is built including starting PostgreSQL, you will not have a functional Tripal site.

.. note::

  To **test your image**, execute any of the ``docker run`` commands documented above making sure to also start PostgreSQL (i.e. ``docker exec t4d8 service postgresql restart``). At this point you will already have Drupal, Tripal and Chado installed. It is recommended to also do a quick test of core functionality which may have been impacted by any recent changes.

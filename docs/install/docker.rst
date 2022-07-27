Tripal Docker
================

Tripal Docker is currently focused on Development, Debugging, and Unit Testing. There will be a production focused Tripal Docker soon.

Software Stack
--------------

Currently we have the following installed:
 - Debian Bullseye(11)
 - PHP 8.0.21 with extensions needed for Drupal (Memory limit 1028M)
 - Apache 2.4.54
 - PostgreSQL 13.7 (Debian 13.7-0+deb11u1)
 - Composer 2.3.10
 - Drush 11.1.1
 - Drupal 9.3.x-dev downloaded using composer (or as specified by drupalversion argument).
 - Xdebug 3.0.1

Quickstart
----------

1. Run the image in the background mapping it's web server to your port 9000.

    a) Stand-alone container for testing or demonstration.

    .. code::

      docker run --publish=9000:80 --name=t4d8 -tid tripalproject/tripaldocker:latest

    b) Development container with current directory mounted within the container for easy edits. Change my_module with the name of yours.

    .. code::

      docker run --publish=9000:80 --name=t4d8 -tid --volume=`pwd`:/var/www/drupal9/web/modules/contrib/my_module tripalproject/tripaldocker:latest

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

    docker exec --workdir=/var/www/drupal9/web/modules/contrib/tripal t4d8 phpunit

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

Using Latest tagged version
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. `Install Docker <https://docs.docker.com/get-docker>`_ for your system.

2. Clone the most recent version of Tripal 4 keeping track of where you cloned it.

  .. code-block:: bash

    mkdir ~/Dockers
    cd ~/Dockers
    git clone https://github.com/tripal/t4d8

3. Create a docker container based on the most recent TripalDocker image with your cloned version of Tripal4 mounted inside it.

  .. code-block:: bash

    cd t4d8
    docker run --publish=9000:80 --name=t4d8 -tid --volume=`pwd`:/var/www/drupal9/web/modules/contrib/tripal tripalproject/tripaldocker:latest

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

    docker run --publish=9000:80 --name=t4d8 -tid --volume=C:\Users\yourusername\Dockers\t4d8:/var/www/drupal9/web/modules/contrib/tripal tripalproject/tripaldocker:latest``

4. Start the PostgreSQL database.

  .. code-block:: bash

    docker exec t4d8 service postgresql start

**This will create a persistent Drupal/Tripal site for you to play with! Data is stored even when your computer restarts and Tripal will already be enabled with Chado installed.**

**Furthermore, the --volume part of the run command ensures any changes made in your local directory are automatically copied into the docker container so you can live edit your website.**

Testing install for a specific branch or update the docker image.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following instructions will show you how to create the TripalDocker image from the code existing locally. **This should only be needed if you have made changes to Tripal 4 that impact the installation process, you have created a new module and/or if you have created a new Tripal release. Otherwise, you should be able to use the image from docker hub accessed via the docker pull command.**

First if you do not have a local copy of the t4d8 repository, you can use the following instructions to get one. If you do have a copy already, make sure it is up to date and contains the changes you would like to test.

.. code-block:: bash

  mkdir ~/Dockers
  cd ~/Dockers
  git clone https://github.com/tripal/t4d8

Next, you use the `docker build <https://docs.docker.com/engine/reference/commandline/build/>`_ command to create an image from the existing TripalDocker Dockerfile. Since we are testing Tripal 4 on multiple versions of Drupal, you can set the Drupal major version using the drupalversion argument as shown below. The version of Drupal used for the latest tag is the default value of the argument in the Dockerfile.

.. code-block:: bash

  cd t4d8
  docker build --tag=tripalproject/tripaldocker:drupal9.1.x-dev --build-arg drupalversion='9.1.x-dev' ./

This process will take a fair amount of time as it completely installs Drupal, Tripal and PostgreSQL. You will see a large amount of red text but hopefully not any errors. You should always test the image by running it before pushing it up to docker hub!

.. note::

  Make sure the drupal version specified in the tag matches the build argument. The value of ``drupalversion`` must match one of the available tags on `Packagist drupal/core <https://packagist.org/packages/drupal/core>`_.

.. warning::

  If your new changes to Tripal 4 break install, you will experience one of the following depending on the type of error:

  1. The build command executed above will not complete without errors.
  2. When you run the image after it is built including starting PostgreSQL, you will not have a functional Tripal site.

.. note::

  To **test your image**, execute any of the ``docker run`` commands documented above making sure to also start PostgreSQL (i.e. ``docker exec t4d8 service postgresql restart``). At this point you will already have Drupal, Tripal and Chado installed. It is recommended to also do a quick test of core functionality which may have been impacted by any recent changes.

Troubleshooting
---------------

The provided host name is not valid for this server.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
On my web browser, I got the message "The provided host name is not valid for this server".

**Solution:** It is most likely because you tried to access the site through a URL different from ``localhost`` or ``127.0.0.1``. For instance, if you run docker on a server and want to access your d8t4 site through that server name, you will have to edit the settings.php file inside the docker (at the time writing this, it would be every time you (re)start the docker) and change the last line containing the parameter ``$settings[trusted_host_patterns]``:

.. code::

  docker exec -it t4d8 vi /var/www/drupal9/web/sites/default/settings.php

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

Debugging
---------

Xdebug: Overview
^^^^^^^^^^^^^^^^
There is an optional Xdebug configuration available for use in debugging Tripal 4.
It is disabled by default. Currently, the Docker ships with three modes available:

`Develop <https://xdebug.org/docs/develop>`_
  Adds developer aids to provide "better error messages and obtain more information from PHP's built-in functions".

`Debug <https://xdebug.org/docs/step_debug>`_
  Adds the ability to interactively walk through the code.

`Profile <https://xdebug.org/docs/profiler>`_
  Adds the ability to "find bottlenecks in your script and visualize those with an external tool".

To enable Xdebug, issue the following command:

.. code::

  docker exec --workdir=/var/www/drupal9/web/modules/contrib/tripal t4d8 xdebug_toggle.sh

This will toggle the Xdebug configuration file and restart Apache. You should use this command to disable Xdebug if it is enabled prior to running PHPUnit Tests as it seriously impacts test run duration (approximately 8 times longer).


There is an Xdebug extension available for most modern browsers that will let you dynamically trigger different debugging modes. For instance, profiling should only be used when you want to generate profiling data, as this can be quite compute intensive and may generate large files for a single page load.
The extension places an interactive Xdebug icon in the URL bar where you can select which mode you'd like to trigger.

Xdebug: Step debugging
^^^^^^^^^^^^^^^^^^^^^^

Step debugging occurs in your IDE, such as Netbeans, PhpStorm, or Visual Studio Code.
There will typically already be a debugging functionality built-in to these IDEs, or they can be installed with an extension.
Visual Studio Code, for example, has a suitable debugging suite by default.
This documentation will cover Visual Studio Code, but the configuration options should be similar in other IDEs.

The debugging functionality can be found in VS Code on the sidebar, the icon looks like a bug and a triangle.
A new configuration should be made using PHP. The following options can be used for basic interaction with Xdebug:
.. code::

  {
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Listen for Xdebug",
            "type": "php",
            "request": "launch",
            "port": 9003,
            "pathMappings": { "/var/www/drupal9/web/modules/contrib/tripal": "~/Dockers/t4d8" }
        }
    ]
  }

The important parameter here is `pathMappings` which will allow Xdebug and your IDE know which paths on the host and in the Docker VM coorespond to eachother.
The first path listed is the one within the Docker and should point to the Tripal directory. The seocnd path is the one on your local host machine where you
installed the repo and built the Docker image. If you followed the instructions above, this should be in your user folder under `~/Dockers/t4d8`.

9003 is the default port and should only be changed if 9003 is already in use on your host system.

With this configuration saved, the Play button can be pressed to enable this configuration and have your IDE listen for incoming connections from the Xdebug PHP extension.

More info can be found for VS Code's step debugging facility in `VS Code's documentation <https://code.visualstudio.com/docs/editor/debugging>`_.

Xdebug: Profiling
^^^^^^^^^^^^^^^^^

Profiling the code execution can be useful to detect if certain functions are acting as bottlenecks or if functions are being called too many times, such as in an unintended loop.
The default configuration, when profiling is enabled by selecting it in the Xdebug browser extension, will generate output files in the specified directory.

To view these files, we recommend using Webgrind. It can be launched as a separate Docker image using the following command:

.. code::

  docker run --rm -v ~/Dockers/t4d8/tripaldocker/xdebug_output:/tmp -v ~/Dockers/t4d8:/host -p 8081:80 jokkedk/webgrind:latest

You may need to adjust the paths given in the command above, similar to when setting up the pathMappings for step debugging earlier.

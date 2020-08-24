
Native Docker
================

This is the typical way to interact with docker and involves using an image to create a container for specific work. This ensures long term storage and is ideal for long term development.

Setup
-------

 - Pull the docker image onto your computer:

  .. code:: bash

    docker pull laceysanderson/tripal4dev:tripal4-d8.8.x

 - Clone a local copy of tripal 4:

  .. code:: bash

    https://github.com/tripal/t4d8.git

 - Run the image mapping the local copy of tripal 4 into the container for easy updates and port 80 in the docker container to port 9001 on your local computer:

  .. code:: bash

    docker run --publish=9001:80 -v `pwd`/t4d8:/var/www/html/drupal8/web/modules/t4d8 --name=tripal4dev -tid laceysanderson/tripal4dev:tripal4-d8.8.x

 - Start the database server:

  .. code:: bash

    docker exec tripal4dev service postgresql start

 - Visit the website at localhost:9001/drupal8/web. The admin user is ``drupaladmin`` and the password is ``some_admin_password``

Usage
--------

- To access the drupal container run:

 .. code:: bash

   docker exec -it tripal4dev bash

- To access the database run (The password is ``drupal8developmentonlylocal``):

 .. code:: bash

   docker exec -it tripal4dev drupal8/vendor/bin/drush sql:cli

- To run drush commands:

 .. code:: bash

   docker exec tripal4dev drupal8/vendor/bin/drush [YOUR OPTIONS]

- To run unit tests:

 .. code:: bash

   docker exec tripal4dev drupal8/vendor/bin/phpunit --config drupal8/web/core drupal8/web/modules/t4d8

- To update drupal run:

 .. code:: bash

   docker exec -w /var/www/html/drupal8 tripal4dev composer up

- To download a module provided by the Drupal package manager:


 .. code:: bash

   docker exec -w /var/www/html/drupal8 tripal4dev composer require drupal/devel


Docker Compose
==================

Docker compose is great for quickly setting up a test environment but may not have long term storage. This option is great for exploring the current version but may no be ideal for long term development.

Setup
-------

- Copy tripaldocker/dev/.env.example to tripaldocker/dev/.env
- Run ``docker-compose up -d``
- Next start the database using ``docker-compose drupal service postgresql start``
- Visit localhost:9000/drupal8/web
- The Drupal site will already be installed and Tripal + Tripal Chado will be enabled.

Usage
------

- To access the drupal container run:

  .. code:: bash

    docker-compose exec drupal bash

- To access the database using psql run (The password is ``drupal8developmentonlylocal``):

  .. code:: bash

    docker-compose exec drupal psql -q --dbname=drupal8_dev --host=localhost --port=5432 --username=drupaladmin

- To run drush commands:

  .. code:: bash

    docker-compose exec drupal drupal8/vendor/bin/drush [YOUR OPTIONS]

- To run unit tests:

  .. code:: bash

    docker-compose exec drupal drupal8/vendor/bin/phpunit --config drupal8/web/core drupal8/web/modules/t4d8

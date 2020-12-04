# Tripal Docker

Tripal Docker is currently focused on Development and Unit Testing. There will be a production focused Tripal Docker soon.

## Software Stack

Currently we have the following installed:
 - Debian Buster(10)
 - PHP 7.3.25 with extensions needed for Drupal (Memory limit 1028M)
 - Apache 2.4.38
 - PostgreSQL 11.9 (Debian 11.9-0+deb10u1)
 - Composer 2.0.7
 - Drupal Console 1.9.7
 - Drush 10.3.6
 - Drupal 8.9.10  (8.x-dev) downloaded using composer.

## Setup

1. Run the image in the background mapping it's web server to your port 9000.

    a) Stand-alone container for testing or demonstration.
    ```
    docker run --publish=9000:80 --name=drupal8dev_ci -t -i -d laceysanderson/drupal8dev_ci:drupal8.9.3-pgsql
    ```
    b) Development container with current directory mounted within the container for easy edits. Change my_module with the name of yours.
    ```
    docker run --publish=9000:80 --name=drupal8dev_ci -t -i -d --volume=`pwd`:/var/www/drupal8/web/modules/my_module laceysanderson/drupal8dev_ci:drupal8.9.3-pgsql
    ```

2. Start the PostgreSQL database.
```
docker exec drupal8dev_ci service postgresql start
```

3. Navigate to http://localhost:9000 to your fully function site. Drupal was installed using the standard profile during image creation. The administration user is drupaladmin:some_admin_password.


## Usage
 - Run Drupal Core PHP Unit Tests:
     ```
     docker exec drupal8dev_ci phpunit --configuration core core/modules/simpletest/tests
     ```
 - Run Drupal Console to generate code for your module!
     ```
     docker exec drupal8dev_ci drupal generate:module
     ```
 - Run Drush to rebuild the cache
     ```
     docker exec drupal8dev_ci drush cr
     ```
 - Run Composer to upgrade Drupal
     ```
     docker exec drupal8dev_ci composer up
     ```

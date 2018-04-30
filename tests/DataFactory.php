<?php

use StatonLab\TripalTestSuite\Database\Factory;

/**
 * Data Factory
 * -----------------------------------------------------------
 * This is where you can define factories for use in tests and
 * database seeders.
 *
 * @docs https://github.com/statonlab/TripalTestSuite
 */

/** @see  StatonLab\TripalTestSuite\Database\Factory::define() */
Factory::define('chado.cv', function (Faker\Generator $faker) {
    return [
        'name' => $faker->name,
        'definition' => $faker->text,
    ];
});

/** @see  StatonLab\TripalTestSuite\Database\Factory::define() */
Factory::define('chado.db', function (Faker\Generator $faker) {
    return [
        'name' => $faker->name,
        'description' => $faker->text,
        'urlprefix' => $faker->url,
        'url' => $faker->url,
    ];
});

/** @see  StatonLab\TripalTestSuite\Database\Factory::define() */
Factory::define('chado.dbxref', function (Faker\Generator $faker) {
    return [
        'db_id' => factory('chado.db')->create()->db_id,
        'accession' => $faker->numberBetween(),
        'version' => $faker->numberBetween(),
        'description' => $faker->text,
    ];
});

/** @see  StatonLab\TripalTestSuite\Database\Factory::define() */
Factory::define('chado.cvterm', function (Faker\Generator $faker) {
    return [
        'cv_id' => factory('chado.cv')->create()->cv_id,
        'dbxref_id' => factory('chado.dbxref')->create()->dbxref_id,
        'name' => $faker->name,
        'definition' => $faker->text,
        'is_obsolete' => 0,
        'is_relationshiptype' => 0,
    ];
});

/** @see  StatonLab\TripalTestSuite\Database\Factory::define() */
Factory::define('chado.organism', function (Faker\Generator $faker) {
    return [
        'abbreviation' => $faker->name,
        'genus' => $faker->name,
        'species' => $faker->name,
        'common_name' => $faker->name,
        'type_id' => factory('chado.cvterm')->create()->cvterm_id,
    ];
});

/** @see  StatonLab\TripalTestSuite\Database\Factory::define() */
Factory::define('chado.feature', function (Faker\Generator $faker) {
    return [
        'name' => $faker->name,
        'uniquename' => $faker->unique()->name,
        'organism_id' => factory('chado.organism')->create()->organism_id,
        'type_id' => factory('chado.cvterm')->create()->cvterm_id,
    ];
});

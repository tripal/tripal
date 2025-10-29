<?php

namespace Drupal\Tests\tripal_chado\Kernel\Controller;

use Symfony\Component\HttpFoundation\Request;
use Drupal\tripal_chado\Controller\ChadoOrganismFormElementController;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the Organism Autocomplete Form Element.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Organism Autocomplete Form Element
 */
#[Group('autocomplete')]
#[Group('bio-organism')]
#[RunTestsInSeparateProcesses]
class ChadoOrganismFormElementControllerTest extends ChadoTestKernelBase {
  /**
   * The default theme to use for this test.
   *
   * @var string
   */
  protected $defaultTheme = 'stark';

  /**
   * Modules to enable.
   *
   * @var array
   */
  protected static $modules = ['system', 'user', 'file', 'tripal', 'tripal_chado'];

  /**
   * The database connection to the test chado.
   *
   * @var \Drupal\Core\Database\Connection
   */
  protected $connection;

  /**
   * An array of test organisms created.
   *
   * @var array
   */
  protected array $organisms;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Open connection to a test Chado.
    $this->connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    // Create some test organisms.
    $subspecies_id = $this->connection->select('1:cvterm', 'T')
      ->fields('T', ['cvterm_id'])
      ->condition('T.name', 'subspecies', '=')
      ->execute()
      ->fetchField();
    $this->organisms = [
      1 => [
        'genus' => 'Tripalus',
        'species' => 'bogusii',
        'type_id' => $subspecies_id,
        'infraspecific_name' => 'fakus',
        'abbreviation' =>
        'T. bogusii subsp. fakus',
      ],
      2 => [
        'genus' => 'Tripalus',
        'species' => 'databasica',
        'type_id' => NULL,
        'infraspecific_name' => NULL,
        'abbreviation' => NULL,
      ],
      3 => [
        'genus' => 'Drupalus',
        'species' => 'fictus',
        'type_id' => NULL,
        'infraspecific_name' => NULL,
        'abbreviation' => 'Drupalus fictus',
      ],
    ];
    foreach ($this->organisms as $details) {
      $insert = $this->connection->insert('1:organism');
      $insert->fields([
        'genus' => $details['genus'],
        'species' => $details['species'],
        'type_id' => $details['type_id'] ?? NULL,
        'infraspecific_name' => $details['infraspecific_name'] ?? NULL,
        'abbreviation' => $details['abbreviation'] ?? NULL,
        'common_name' => $details['common_name'] ?? NULL,
        'comment' => $details['comment'] ?? NULL,
      ]);
      $insert->execute();
    }
  }

  /**
   * Tests the organism form element.
   */
  public function testChadoOrganismFormElementController() {

    $organism_autocomplete = new ChadoOrganismFormElementController();
    $this->assertIsObject($organism_autocomplete, 'Failed to create the ChadoOrganismFormElementController');

    // Any organism containing a 't', all three do have a 't' somewhere.
    $request = Request::create(
      'chado/organism/element/autocomplete/10',
      'GET',
      ['q' => 't']
    );
    $suggest = $organism_autocomplete->handleAutocomplete($request, 3, 0)
      ->getContent();
    $this->assertEquals(3, count(json_decode($suggest, FALSE)), 'Number of suggestions incorrect for organisms "t" autocomplete');
    foreach (json_decode($suggest, FALSE) as $item) {
      $organism_id = ChadoOrganismFormElementController::getPkeyId($item->value);
      $this->assertIsInt($organism_id, 'getPkeyId did not return an integer');
      $this->assertArrayHasKey($organism_id, $this->organisms, 'Invalid organism_id returned');
    }

    // Any organism containing a 'k', only one does.
    $request = Request::create(
      'chado/organism/element/autocomplete/10',
      'GET',
      ['q' => 'k']
    );
    $suggest = $organism_autocomplete->handleAutocomplete($request, 1, 0)
      ->getContent();
    $this->assertEquals(1, count(json_decode($suggest, FALSE)), 'Number of suggestions incorrect for organisms "k" autocomplete');
  }

  /**
   * Provides data for the testFormElement() method.
   *
   * @return array
   *   An array containing the test scenarios.
   */
  public static function provideDataForTestFormElement() {
    $element = [];
    return [
      [
        'Select element with valid default id given',
        $element,
        1,
        [
          'select_limit' => 4,
          'match_limit' => 10,
          'size' => 60,
        ],
        [
          'type' => 'select',
          'default_value' => 1,
        ],
      ],
      [
        'Autocomplete with valid default id given',
        $element,
        1,
        [
          'select_limit' => 2,
          'match_limit' => 10,
          'size' => 60,
        ],
        [
          'type' => 'textfield',
          'default_value' => 'Tripalus bogusii subspecies fakus (1)',
        ],
      ],
      [
        'Autocomplete with valid default value given',
        $element,
        'Tripalus bogusii (1)',
        [
          'select_limit' => 2,
          'match_limit' => 10,
          'size' => 60,
        ],
        [
          'type' => 'textfield',
          'default_value' => 'Tripalus bogusii (1)',
        ],
      ],
      [
        'Select element with valid default value given',
        $element,
        'Tripalus bogusii (1)',
        [
          'select_limit' => 4,
          'match_limit' => 10,
          'size' => 60,
        ],
        [
          'type' => 'select',
          'default_value' => 1,
        ],
      ],
      [
        'Autocomplete with invalid default id given',
        $element,
        9,
        [
          'select_limit' => 2,
          'match_limit' => 10,
          'size' => 60,
        ],
        [
          'type' => 'textfield',
          'default_value' => '',
        ],
      ],
    ];
  }

  /**
   * Tests the organism form element generation.
   *
   * @param string $scenario
   *   A short description of the scenario being tested.
   * @param array $elements
   *   The form element array.
   * @param mixed $dafault
   *   The default value, either an integer organism_id or a string with
   *   the organism name and id in parentheses at the end.
   * @param array $options
   *   An array of options, including:
   *   select_limit - The maximum number of organisms to show in select element.
   *   match_limit - The maximum number of organisms to match in the
   *   autocomplete.
   *   size - The size of the textfield or select element.
   * @param array $expected
   *   An array of expected results, including:
   *   type - The expected type of form element, either 'select' or 'textfield'.
   *   default_value - The expected default value of the form element.
   *
   * @dataProvider provideDataForTestFormElement
   */
  #[DataProvider('provideDataForTestFormElement')]
  public function testFormElement(string $scenario, array $elements, mixed $dafault, array $options, array $expected) {

    // Test the form element.
    $element = ChadoOrganismFormElementController::getFormElement($elements, $dafault, $options);
    $this->assertIsArray($element, 'A form element was expected, but did not get one.');
    $this->assertEquals($expected['type'], $element['#type'], 'We expected the type of the element to be' . $expected['type'] . 'but it was a .' . $element['#type']);
    $this->assertArrayHasKey('#default_value', $element, 'We expected a default value in the select element, but did not get one.');
    $this->assertEquals($expected['default_value'], $element['#default_value'], 'The default value we expected was ' . $expected['default_value'] . ' but got ' . $element['#default_value']);

    if ($element['#type'] == 'select') {
      $this->assertArrayHasKey('#options', $element, 'We expected the returned element to have an array key for options, but did not get one.');
      $this->assertGreaterThan(0, count($element['#options']), 'We expected options in the select element, but did not get any.');
    }
    elseif ($element['#type'] == 'textfield') {
      $this->assertArrayHasKey('#autocomplete_route_parameters', $element, 'We expected the returned element to have an array key for autocomplete route parameters, but did not get one.');
      $this->assertGreaterThan(0, count($element['#autocomplete_route_parameters']), 'We expected autocomplete route parameters, but did not get any.');
    }
  }

}

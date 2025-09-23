<?php

namespace Drupal\Tests\tripal_chado\Kernel;

use Symfony\Component\HttpFoundation\Request;
use Drupal\tripal_chado\Controller\ChadoOrganismFormElementController;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests the Organism Autocomplete Form Element.
 *
 * @group Tripal
 * @group Tripal Chado
 * @group Organism Autocomplete Form Element
 */
#[Group('Tripal')]
#[Group('Tripal Chado')]
#[Group('Organism Autocomplete Form Element')]
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
   * Tests the organism form element autocomplete.
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
   * Tests the organism form element generation.
   */
  public function testFormElement() {
    $organism_autocomplete = new ChadoOrganismFormElementController();
    $this->assertIsObject($organism_autocomplete, 'Failed to create the ChadoOrganismFormElementController');

    $element = [];
    $options = [
      'select_limit' => 4,
      'match_limit' => 10,
      'size' => 60,
    ];

    // Test select element.
    $element = ChadoOrganismFormElementController::getFormElement($element, 0, $options);
    $this->assertIsArray($element, 'A select element was expected, but did not get one.');
    $this->assertEquals('select', $element['#type'], 'We expected the type of the element to be a select list, but it was a .' . $element['#type']);
    $this->assertGreaterThan(0, count($element['#options']), 'We expected options in the select element, but did not get any.');
    $this->assertArrayHasKey('#default_value', $element, 'We expected a default value in the select element, but did not get one.');

    // Test autocomplete element when no default value is provided.
    $options['select_limit'] = 2;
    $element = ChadoOrganismFormElementController::getFormElement($element, 0, $options);
    $this->assertIsArray($element, 'We expected an autocomplete element, but did not get one.');
    $this->assertEquals('textfield', $element['#type'], 'We expected the type of the element to be a textfield for autocomplete, but it was a ' . $element['#type']);
    $this->assertArrayHasKey('#default_value', $element, 'We expected a default value in the autocomplete element, but did not get one.');
    $this->assertGreaterThan(0, count($element['#autocomplete_route_parameters']), 'We expected autocomplete route parameters, but did not get any.');

    // Test autocomplete element when a default value is provided.
    $options['select_limit'] = 2;
    $element = ChadoOrganismFormElementController::getFormElement($element, 'Tripalus bogusii (1)', $options);
    $this->assertIsArray($element, 'We expected an autocomplete element, but did not get one.');
    $this->assertEquals('textfield', $element['#type'], 'We expected the type of the element to be a textfield for autocomplete, but it was a ' . $element['#type']);
    $this->assertArrayHasKey('#default_value', $element, 'We expected a default value in the autocomplete element, but did not get one.');
    $this->assertGreaterThan(0, count($element['#autocomplete_route_parameters']), 'We expected autocomplete route parameters, but did not get any.');
  }

}

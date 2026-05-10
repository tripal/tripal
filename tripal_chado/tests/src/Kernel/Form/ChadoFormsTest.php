<?php

namespace Drupal\Tests\tripal_chado\Kernel\ContentTerms;

use Drupal\Core\Config\FileStorage;
use Drupal\Core\Form\FormState;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Database\ChadoConnection;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;
use Symfony\Component\Yaml\Yaml;

/**
 * Tests the tripal terms forms.
 *
 * @group ContentTerms
 */
#[Group('form')]
#[Group('render')]
#[Group('service-collection')]
#[RunTestsInSeparateProcesses]
class ChadoFormsTest extends ChadoTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'field', 'user', 'views', 'tripal', 'tripal_chado'];

  /**
   * The test chado connection.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

  /**
   * {@inheritdoc}
   */
  protected function setUp() : void {
    parent::setUp();

    // Open connection to a test Chado.
    $this->chado_connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
  }

  /**
   * Provides scenarios for testing configuration entities.
   *
   * We use a separate yaml file for each form to simplify maintenance,
   * and to aid in organization, them they can be stored in sub-folders.
   * The only condition is that the yaml file name matches /.+\.test\.yml$/i.
   *
   * @return array
   *   The provided scenarios.
   */
  public static function provideScenarios() {
    $all_scenarios = [];
    $directory_iterator = new \RecursiveDirectoryIterator(__DIR__, \FilesystemIterator::SKIP_DOTS);
    $iterator_iterator = new \RecursiveIteratorIterator($directory_iterator);
    $regex_iterator = new \RegexIterator($iterator_iterator, '/.+\.test\.yml$/i');
    foreach ($regex_iterator as $yaml_file) {
      $file_contents = file_get_contents($yaml_file->getPathName());
      self::assertNotEmpty($file_contents, "No information read from scenarios yaml file \"$yaml_file\"");
      try {
        $scenarios = Yaml::parse($file_contents);
        self::assertNotEmpty($scenarios, "No information parsed from scenarios yaml file \"$yaml_file\"");
        $all_scenarios = array_merge($all_scenarios, $scenarios);
      }
      catch (\Exception $e) {
        self::fail("Exception while parsing scenarios yaml file \"$yaml_file\": " . $e->getMessage());
      }
    }
    return $all_scenarios;
  }

  /**
   * Tests form building and submission.
   *
   * @dataProvider provideScenarios
   *
   * @return void
   *   No return value.
   */
  #[DataProvider('provideScenarios')]
  public function testChadoForms(array $scenario): void {

    // Install specific setup items for this particular form.
    if ($scenario['setup']['views'] ?? []) {
      $viewStorage = \Drupal::service('entity_type.manager')->getStorage('view');
      $dir = \Drupal::service('extension.path.resolver')->getPath('module', 'tripal_chado');
      $fileStorage = new FileStorage($dir);
      foreach ($scenario['setup']['views'] as $view) {
        $config = $fileStorage->read($view);
        $this->assertNotEmpty($config, "Error loading view \"$view\"");
        $viewObject = $viewStorage->create($config);
        $this->assertIsObject($viewObject, "Error creating view \"$view\"");
        $viewObject->save();
      }
    }

    try {
      $form_builder = \Drupal::formBuilder();
      $form_class = $scenario['form_class'];
      $form_state = new FormState();
      $args = $scenario['args'] ?? [];
      $form_state->addBuildInfo('args', $args);
      $form = $form_builder->buildForm($form_class, $form_state);

      // Ensure we were able to build the form.
      $this->assertIsArray($form,
        'We expect the form builder to return a form for class ' . $scenario['form_class']);

      // Check form expectations.
      foreach ($scenario['form_expectations'] ?? [] as $key => $value) {
        $this->checkExpectations($key, $value, $form);
      }
      // Check for expected Drupal messenger messages prior to submission,
      // e.g. foreign key references.
      $messages = $this->getFlattenedMessages();
      foreach ($scenario['form_messages'] ?? [] as $message) {
        $this->assertArrayContainsString($message, $messages, 'build');
      }

      // Insert any user input values to be submitted.
      foreach ($scenario['submit_values'] ?? [] as $key => $value) {
        $form_state->setValue($key, $value);
      }
      if (array_key_exists('triggering_element', $scenario)) {
        $form_state->setTriggeringElement($scenario['triggering_element']);
      }

      // Submit the form and capture any output.
      ob_start();
      $form_builder->submitForm($form_class, $form_state);
      $output = ob_get_clean();
    }
    catch (\Exception $e) {
      $output = $e->getMessage();
    }
    $messages = $this->getFlattenedMessages();

    // Verify either form redirect output, or an exception message.
    foreach ($scenario['submit_output'] ?? [] as $submit_output) {
      if (strlen($submit_output) > 0) {
        $this->assertStringContainsString($submit_output, $output, 'Form build or submit output did not contain an expected string in '
        . print_r($output, TRUE));
      }
      else {
        $this->assertEquals($submit_output, $output, 'Form build or submit produced unexpected output');
      }
    }

    // If submit is not expected to fail, check for expected values.
    if (!($scenario['submit_fails'] ?? FALSE)) {
      // And do some basic checks to check for errors.
      $this->assertTrue($form_state->isValidationComplete(),
        'We expect the form state to have been updated to indicate that validation is complete.');

      // Check for specific values we expect to see.
      $submitted_values = $form_state->getValues();
      foreach ($scenario['submit_expectations'] as $key => $value) {
        $this->checkExpectations($key, $value, $submitted_values);
      }
    }

    // Check for expected Drupal messenger messages.
    foreach ($scenario['submit_messages'] ?? [] as $message) {
      $this->assertArrayContainsString($message, $messages, 'submit');
    }

    // Inspect the chado database for expected results.
    foreach ($scenario['chado_expectations'] ?? [] as $ce) {
      $query = $this->chado_connection->select('1:' . $ce['table'], 'T');
      foreach ($ce['conditions'] ?? [] as $condition) {
        $query->condition($condition['column'], $condition['value'], $condition['test'] ?? '=');
      }
      if (isset($ce['count'])) {
        $count = $query->countQuery()->execute()->fetchField();
        $this->assertEquals($ce['count'], $count, 'Did not meet database count expectation for: ' . print_r($ce, TRUE));
      }
    }
  }

  /**
   * Determine if one form element matches its expectation.
   *
   * @param string $key
   *   The array key from both expectation and from the form.
   * @param mixed $value
   *   The value we expect to find. May be an array if nested further down.
   * @param array $form
   *   The form array we are checking.
   * @param string $steps
   *   Earlier array keys if nested further down.
   *
   * @return void
   *   No return value, just performs assertions.
   */
  protected function checkExpectations(string $key, mixed $value, array $form, string $steps = ''): void {
    // If the value is an array, then the value of interest is one
    // or more steps further down in the array, so perform recursion.
    if (is_array($value)) {
      $this->assertArrayHasKey($key, $form,
        "Expected the key \"$steps$key\" in the form array but it is not present. Form keys found: "
        . print_r(array_keys($form), TRUE));
      $sub_form = $form[$key];
      foreach ($value as $next_key => $next_value) {
        $this->checkExpectations($next_key, $next_value, $sub_form, $steps . $key . ':');
      }
    }
    else {
      if (is_object($value)) {
        $value = $value->getValue();
      }
      $this->assertArrayHasKey($key, $form,
        "Expected the key \"$steps$key\" in the form array but it is not present. Keys found were: "
        . print_r(array_keys($form), TRUE));
      $this->assertEquals($value, $form[$key],
        "We did not get the value we expected \"$value\" for \"$steps$key\", instead we got \"" . $form[$key] . "\".");
    }
  }

  /**
   * Returns a simple array of strings from the drupal messenger.
   *
   * Individual message strings are prefixed with the message category.
   *
   * @return array
   *   An array of strings.
   */
  protected function getFlattenedMessages(): array {
    $messages = \Drupal::messenger()->all();
    $flattened_messages = [];
    foreach ($messages as $type => $items) {
      foreach ($items as $message) {
        $text = $type . ': ' . (string) $message;
        $flattened_messages[] = $text;
      }
    }
    return $flattened_messages;
  }

  /**
   * Used to assert that one of the array elements contains a substring.
   *
   * @param string $expected
   *   A substring to find in the array of messages.
   * @param array $messages
   *   An array of strings.
   * @param string $context
   *   The stage where the assertion is made.
   *
   * @return void
   *   No return value, just performs assertions.
   */
  protected function assertArrayContainsString(string $expected, array $messages, string $context): void {
    $found = FALSE;
    foreach ($messages as $message) {
      if (str_contains($message, $expected)) {
        $found = TRUE;
        break;
      }
    }
    if (!$found) {
      $this->fail("Expected to find the string \"$expected\" in a drupal messenger message during form $context, but did not. Messages were: "
        . print_r($messages, TRUE));
    }
  }

}

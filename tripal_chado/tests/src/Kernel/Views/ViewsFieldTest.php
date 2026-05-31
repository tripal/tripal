<?php

declare(strict_types=1);

namespace Drupal\Tests\tripal\Kernel\Views;

use Drupal\Core\Config\FileStorage;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Database\ChadoConnection;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;
use Symfony\Component\Yaml\Yaml;

/**
 * Tests various views fields.
 */
#[Group('views')]
#[RunTestsInSeparateProcesses]
class ViewsFieldTest extends ChadoTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = [
    'system',
    'field',
    'user',
    'views',
    'field',
    'tripal',
    'tripal_chado',
  ];

  /**
   * The test chado connection.
   *
   * @var Drupal\tripal_chado\Database\ChadoConnection
   */
  protected ChadoConnection $chado_connection;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    $this->chado_connection = $this->getTestSchema(ChadoTestKernelBase::INIT_CHADO_EMPTY);

    // Test values for the db table.
    $query = $this->chado_connection->insert('1:db');
    $query->fields(['db_id', 'name', 'description', 'url', 'urlprefix']);
    $query->values([2, 'db1', '', '', '']);
    $query->values([3, 'db2', 'has_description', 'https://test', 'https://test/{db}/{accession}']);
    $query->execute();
  }

  /**
   * Tests views fields.
   */
  public function testViewsFields(): void {
    $viewStorage = \Drupal::service('entity_type.manager')->getStorage('view');
    $dir = \Drupal::service('extension.path.resolver')->getPath('module', 'tripal_chado');
    $fileStorage = new FileStorage($dir);

    // Load scenarios from yaml.
    $file_contents = file_get_contents(__DIR__ . '/views_field_test.yml');
    $scenarios = Yaml::Parse($file_contents);

    foreach ($scenarios as $id => $scenario) {
      $view_path = $scenario['view'];
      $view_id = basename($scenario['view']);

      // Create the view for this scenario.
      $config = $fileStorage->read($view_path);
      $this->assertNotEmpty($config, "Error loading view \"$view_path\" from scenario \"$id\"");
      /** @var Drupal\views\Entity\View */
      $viewConfigObject = $viewStorage->create($config);
      $this->assertIsObject($viewConfigObject, "Error creating view configuration \"$view_path\" from scenario \"$id\"");
      $viewConfigObject->save();

      /** @var Drupal\views\ViewExecutable */
      $viewExecutable = $viewConfigObject->getExecutable();
      $this->assertIsObject($viewExecutable, "Error loading view from id \"$view_id\" from scenario \"$id\"");
      $result = $viewExecutable->execute();
      $this->assertTrue($result, 'viewExecutable should return TRUE');

      // Verify expected output.
      $result = $viewExecutable->result;
      foreach ($scenario['rows'] as $row => $expect) {
        if ($expect['isnull'] ?? FALSE) {
          $this->assertArrayNotHasKey($row, $result, "Expect no values for row \"$row\"");
        }
        else {
          $this->assertArrayHasKey($row, $result, "Expect to have values for row \"$row\"");
          foreach ($expect['fields'] as $field_id => $value) {
            $output = $viewExecutable->field[$field_id]->render($result[$row]);
            if (is_object($output)) {
              $output = (string) $output;
            }
            $this->assertEquals($value, $output, "Row \"$row\" field \"$field_id\" output is not as expected");
          }
        }
      }

    }

  }

}

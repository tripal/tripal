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
    $query->values([2, '1db1', '', '', '']);
    $query->values([3, '1db2', 'non-blank-db-description', 'https://test', 'https://test/{db}/{accession}']);
    $query->execute();

    // Test values for the dbxref table.
    $query = $this->chado_connection->insert('1:dbxref');
    $query->fields(['dbxref_id', 'db_id', 'accession', 'version', 'description']);
    $query->values([3, 2, '1a001', '', '']);
    $query->values([4, 3, '1b002', 'ver2', 'non-blank-dbxref-description']);
    $query->execute();

    // Test values for the cv table.
    $query = $this->chado_connection->insert('1:cv');
    $query->fields(['cv_id', 'name', 'definition']);
    $query->values([5, '1cv1', '']);
    $query->values([6, '1cv2', 'non-blank-cv-definition']);
    $query->execute();

    // Test values for the cvterm table.
    $query = $this->chado_connection->insert('1:cvterm');
    $query->fields(['cvterm_id', 'cv_id', 'name', 'definition', 'dbxref_id', 'is_obsolete', 'is_relationshiptype']);
    $query->values([3, 5, '1t001', '', 3, 0, 0]);
    $query->values([4, 6, '1t002', 'non-blank-cvterm-definition', 4, 1, 1]);
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
        if ($expect['is_null'] ?? FALSE) {
          $this->assertArrayNotHasKey($row, $result, "Expect no values for row \"$row\" scenario \"$id\"");
        }
        else {
          $this->assertArrayHasKey($row, $result, "Expect to have values for row \"$row\" scenario \"$id\"");
          foreach ($expect['fields'] as $field_id => $value) {
            $this->assertArrayHasKey($field_id, $viewExecutable->field, "Yaml config error, missing key in view executable from scenario \"$id\". Avaliable keys: " . print_r(array_keys($viewExecutable->field), TRUE));
            $output = $viewExecutable->field[$field_id]->render($result[$row]);
            if (is_object($output)) {
              $output = (string) $output;
            }
            $this->assertEquals($value, $output, "Row \"$row\" field \"$field_id\" scenario \"$id\" output is not as expected");
          }
        }
      }
    }
  }

}

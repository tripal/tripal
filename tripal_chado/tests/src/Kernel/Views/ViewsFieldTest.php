<?php

declare(strict_types=1);

namespace Drupal\Tests\tripal\Kernel\Views;

use Drupal\Core\Config\FileStorage;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\Tests\tripal_chado\Traits\ChadoTestTrait;
#use Drupal\Tests\views\Kernel\ViewsKernelTestBase;
#use Drupal\user\Entity\User;
#use Drupal\views\Tests\ViewTestData;
use Drupal\tripal_chado\Database\ChadoConnection;
use Drupal\views\Views;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;
use Symfony\Component\Yaml\Yaml;

use Drupal\views\ViewExecutable;
use Drupal\Core\Hook\Attribute\Hook;
use Drupal\views\Plugin\views\query\QueryPluginBase;
use Drupal\views\Plugin\views\query\Sql;

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
'system', 'field', 'user', 'views',
#'path', 'path_alias',
'field',
'tripal', 'tripal_chado'];

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
print "\nCPS1 setUp\n";
    parent::setUp();
#print "\nCPS2 parent called\n";
#$databasePrefix = 'splunge1_';

$t1 = microtime(TRUE);
    $this->chado_connection = $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);
//INIT_CHADO_EMPTY); //;;; @todo see if this works
$t2 = microtime(TRUE);
#print "CPS3 connection established time=" . sprintf('%0.4f', $t2 - $t1) . "\n";
#    $chado_connection = \Drupal::service('tripal_chado.database');
    $chado_1_prefix = $this->chado_connection->getSchemaName();
#print "CPS4 getSchemaName = $chado_1_prefix\n";
    $this->prepareEnvironment(['TripalView']);
#$databasePrefix = 'splunge2_';

    // Override the connection prefix.
#    $drupal_connection = \Drupal::database();
#    $reflection = new \ReflectionObject($drupal_connection);
#    $property = $reflection->getProperty('prefix');
#    $property->setAccessible(TRUE);
#    $property->setValue($drupal_connection, $chado_1_prefix);

  }

  /**
   * Tests views fields.
   */
  public function testViewsFields(): void {
print "\nCP0 start\n";
$databasePrefix = 'splunge3_';
#sleep(10);
#$x = $this->chado_connection->select('1:db', 't')->fields('t', ['db_id', 'name'])->execute();
#while ($r = $x->fetchObject()) {
#  print "db: " . $r->db_id . " = " . $r->name . "\n";
#}
    $viewStorage = \Drupal::service('entity_type.manager')->getStorage('view');
#$x = get_class_methods($viewStorage); print "CPCM1 "; var_dump($x);
#$x2 = $viewStorage->getEntityClass(); print "CPCM2 "; var_dump($x2);
    $dir = \Drupal::service('extension.path.resolver')->getPath('module', 'tripal_chado');
    $fileStorage = new FileStorage($dir);

    // Load scenarios from yaml.
    $file_contents = file_get_contents(__DIR__ . '/views_field_test.yml');
    $scenarios = Yaml::Parse($file_contents);
#print "CP1 yaml ";var_dump($scenarios);

    foreach ($scenarios as $id => $scenario) {
print "CP2 scenario $id\n";
      $view_path = $scenario['view'];
      $view_id = basename($scenario['view']);

      // Load the view for this scenario.
      $config = $fileStorage->read($view_path);
      $this->assertNotEmpty($config, "Error loading view \"$view_path\"");
      /** @var Drupal\views\Entity\View */
      $viewConfigObject = $viewStorage->create($config);
      $this->assertIsObject($viewConfigObject, "Error creating view configuration \"$view_path\"");
      $viewConfigObject->save();

print "CP3 view created, class = ".get_class($viewConfigObject)."\n";

      /** @var Drupal\views\ViewExecutable */
      $viewExecutable = $viewConfigObject->getExecutable();
print "CP4 view executable created, class = ".get_class($viewExecutable)."\n";
      $this->assertIsObject($viewExecutable, "Error loading view from id \"$view_id\"");

      $result = $viewExecutable->execute();
      $this->assertTrue($result, 'viewExecutable should return TRUE');

#print "CP7c fields ";var_dump(array_keys($viewExecutable->field));
#CP7c fields array(9) {
#  [0]=>
#  string(5) "db_id"
#  [1]=>
#  string(4) "name"
#  [2]=>
#  string(11) "description"
#  [3]=>
#  string(3) "url"
#  [4]=>
#  string(9) "urlprefix"
#  [5]=>
#  string(12) "db_edit_link"
#  [6]=>
#  string(13) "db_items_link"
#  [7]=>
#  string(14) "db_delete_link"
#  [8]=>
#  string(10) "dropbutton"



      // Test each field.
      $row = 0;
      foreach ($scenario['fields'] as $id => $value) {
        $output = $viewExecutable->field[$id]->render($viewExecutable->result[$row]);
print "CP8 output for field \"$field\" is \n";var_dump($output);
        $this->assertEquals($value, $output, 'Field output is not as expected');
      }

print "CP99\n";
    }

    #ViewTestData::createTestViews(static::class, ['user_test_views']);

    #$this->installEntitySchema('user');
    #$this->installSchema('user', ['users_data']);

    #$user = User::create([
    #  // Set 'uid' because the 'test_user_data' view filters the user with an ID
    #  // equal to 2.
    #  'uid' => 2,
    #  'name' => $this->randomMachineName(),
    #]);
    #$user->save();

    // Add some random value as user data.
    #$user_data = $this->container->get('user.data');
    #$random_value = $this->randomMachineName();
    #$user_data->set('views_test_config', $user->id(), 'test_value_name', $random_value);

    #$view = Views::getView('test_user_data');
    #$this->executeView($view);

    #$output = $view->field['data']->render($view->result[0]);
    // Assert that using a valid user data key renders the value.
    #$this->assertEquals($random_value, $output);

    #$view->field['data']->options['data_name'] = $this->randomMachineName();

    #$output = $view->field['data']->render($view->result[0]);
    // An invalid configuration does not return anything.
    #$this->assertNull($output);
  }

}

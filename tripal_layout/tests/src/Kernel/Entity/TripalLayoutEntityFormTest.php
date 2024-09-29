<?php

namespace Drupal\Tests\tripal_layout\Kernel\Entity;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Tests\tripal_layout\Traits\TripalLayoutTestTrait;
use Drupal\tripal_layout\Entity\TripalLayoutDefaultView;
use Drupal\tripal_layout\Entity\TripalLayoutDefaultForm;

/**
 * Tests the TripalLayoutDefaultView and TripalLayoutDefaultForm entities.
 *
 * @group TripalLayoutDisplay
 */
class TripalLayoutEntityFormTest extends TripalTestKernelBase {

  use TripalLayoutTestTrait;

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'field', 'user', 'tripal', 'tripal_layout'];

  /**
   * {@inheritdoc}
   */
  protected function setUp() : void {
    parent::setUp();

    $this->installEntitySchema('user');
    $this->installEntitySchema('tripal_layout_default_form');
    $this->installEntitySchema('tripal_layout_default_view');
  }

  public function provideConfigEntities() {
    $scenarios = [];

    $scenarios['basic_view'] = [
      'form_class' => '\Drupal\tripal_layout\Form\TripalLayoutDefaultViewDeleteForm',
      'config_entity_type' => 'tripal_layout_default_view',
      'yaml_file' => __DIR__ . '/../../../fixtures/yaml_layouts/tripal_layout.tripal_layout_default_view.test_view.yml',
      'expectations' => [
        'title_type' => 'Display Layout',
        'title_name' => 'TEST VIEW',
        'description' => 'how tripal content pages should be organized by default'
      ],
    ];

    $scenarios['basic_form'] = [
      'form_class' => '\Drupal\tripal_layout\Form\TripalLayoutDefaultFormDeleteForm',
      'config_entity_type' => 'tripal_layout_default_form',
      'yaml_file' => __DIR__ . '/../../../fixtures/yaml_layouts/tripal_layout.tripal_layout_default_form.test_form.yml',
      'expectations' => [
        'title_type' => 'Form Layout',
        'title_name' => 'TEST FORM',
        'description' => 'how tripal content edit forms should be organized by default'
      ],
    ];

    return $scenarios;
  }

  /**
   * Tests the delete form for the TripalLayoutDefaultView and
   * TripalLayoutDefaultForm entities.
   *
   * @dataProvider provideConfigEntities
   *
   * @return void
   */
  public function testDeleteForm(string $form_class, string $config_entity_type, string $yaml_file, array $expectations) {

    // Create layout entity to be used in testing.
    $config_entity = $this->createLayoutEntityFromConfig(
      $config_entity_type,
      $yaml_file
    );
    $config_entity_id = $config_entity->id();

    // Get the form.
    $form_object = $form_class::create($this->container);
    $form_object->setEntity($config_entity);
    $form_object->setModuleHandler($this->container->get('module_handler'));
    $form_state = new \Drupal\Core\Form\FormState();
    $form_state->addBuildInfo('args', [$config_entity_type]);
    $form = $form_object->buildForm([], $form_state);

    // Ensure we are able to build the form.
    $this->assertIsArray(
      $form,
      'We still expect the form builder to return a form array even without a plugin_id but it did not.'
    );

    // Check the form title.
    $this->assertStringContainsString($expectations['title_type'], $form['#title'], "The title of the form page did not indicate the correct layout entity type.");
    $this->assertStringContainsString($expectations['title_name'], $form['#title'], "The title of the form page did not include the name of the test entity.");

    // Check the form description.
    $this->assertStringContainsString($expectations['description'], $form['description']['#markup'], "The description of the form page did not include the substring we expected.");

    // Now test validating the form.
    $form_state->setTriggeringElement($form['confirm']);
    $form_object->validateForm($form, $form_state);
    $form_object->submitForm($form, $form_state);

    // And do some basic checks to ensure there were no errors.
    //   Looking for form validation errors
    $form_validation_messages = $form_state->getErrors();
    $helpful_output = [];
    foreach ($form_validation_messages as $element => $markup) {
      $helpful_output[] = $element . " => " . (string) $markup;
    }
    $this->assertCount(
      0,
      $form_validation_messages,
      "We should not have any validation errors when deleting $config_entity_type but instead we have: " . implode(" AND ", $helpful_output)
    );
    //   Looking for drupal message errors.
    $messages = \Drupal::messenger()->all();
    $this->assertIsArray(
      $messages,
      "We expect to have status messages to the user on submission of the form."
    );
    $this->assertArrayNotHasKey(
      'error',
      $messages,
      "There should not be any error messages from this form. Instead we recieved: " . print_r($messages, TRUE)
    );

    // Finally, confirm the config entity was deleted.
    /** @var \Drupal\Core\Config\Entity\ConfigEntityStorage $config_storage **/
    $ret_config_entity = \Drupal::entityTypeManager()->getStorage($config_entity_type)->load($config_entity_id);
    $this->assertNull($ret_config_entity, "We should not have been able to retrieve the config entity we just deleted via the form.");
  }
}

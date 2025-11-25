<?php

namespace Drupal\Tests\tripal_chado\Traits;

use Drupal\Core\Render\Element;
use Drupal\Tests\user\Traits\UserCreationTrait;
use Drupal\Tests\tripal\Traits\TripalEntityFieldTestTrait;
use Drupal\tripal_chado\Plugin\TripalStorage\ChadoStorage;

/**
 * Provides functions related to testing Chado Fields.
 */
trait ChadoWidgetTestTrait {

  /**
   * Tests the field through entity form + field widget.
   *
   * @param int $current_scenario_key
   *   The key of the scenario in the YAML.
   * @param string $current_scenario_label
   *   The label of the scenario in the YAML.
   *
   * @dataProvider provideScenarios
   */
  #[DataProvider('provideScenarios')]
  public function testChadoPropertyWidgetUpdate(int $current_scenario_key, string $current_scenario_label) {

    // Retrieve the full details of the current scenario.
    $current_scenario = $this->retrieveCurrentScenario($current_scenario_key, $current_scenario_label);

    // 1. Test the create form is generated properly.
    // Setup an empty Tripal entity form to interact with (test defaults).
    [$form_object, $form, $form_state] = $this->setupTripalEntityAddForm($this->bundle_name);
    // Confirm all the keys we expect are set to the values we expect.
    $this->assertFieldWidgetsMatch($current_scenario['create']['expected_form_keys'], $this->system_under_test['fields'], $form, $current_scenario['label'] . '[CREATE]');

    // 2. Test the form submit / widget submit to create the entity.
    $this->populateTripalEntityFormState($form_object, $form_state, $current_scenario['create']['user_input']);
    // -- now validate it with the data provided.
    $form_object->validateForm($form, $form_state);
    $errors = $form_state->getErrors();
    $this->assertCount(0, $errors, "We got errors when we submitted the CREATE form with the supplied values. Errors: " . print_r($errors, TRUE));
    // -- if there are no errors then submit the form.
    if (count($errors) === 0) {
      $form_object->submitForm($form, $form_state);
      $form_object->save($form, $form_state);
      // -- confirm the entity matches our expectations.
      $entity = $form_object->getEntity();
      $this->assertFieldValuesMatch($current_scenario['create']['expected_field_values'], $entity, "Field values didn't match our expectations after saving the create form");
    }

    // 3. Test the edit form is generated properly.
    $form = $form_object = $form_state = NULL;
    // Setup an edit Tripal entity form to interact with (test defaults).
    [$form_object, $form, $form_state] = $this->setupTripalEntityEditForm($this->bundle_name, $entity);
    // Confirm all the keys we expect are set to the values we expect.
    $this->assertFieldWidgetsMatch($current_scenario['edit']['expected_form_keys'], $this->system_under_test['fields'], $form, $current_scenario['label'] . ' [EDIT]');

    // 4. Test editing the entity through the form.
    $this->populateTripalEntityFormState($form_object, $form_state, $current_scenario['edit']['user_input']);
    // -- now validate it with the data provided.
    $form_object->validateForm($form, $form_state);
    $errors = $form_state->getErrors();
    $this->assertCount(0, $errors, "We got errors when we submitted the EDIT form with the supplied values. Errors: " . print_r($errors, TRUE));
    // -- if there are no errors then submit the form.
    if (count($errors) === 0) {
      $form_object->submitForm($form, $form_state);
      $form_object->save($form, $form_state);
      // -- confirm the entity matches our expectations.
      $entity = $form_object->getEntity();
      $this->assertFieldValuesMatch($current_scenario['edit']['expected_field_values'], $entity, "Field values didn't match our expectations after saving the edit form");
    }
  }

}

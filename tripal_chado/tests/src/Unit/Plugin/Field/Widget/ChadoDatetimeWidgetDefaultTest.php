<?php

namespace Drupal\Tests\tripal_chado\Unit\Plugin\Field\Widget;

use Drupal\Core\Form\FormState;
use Drupal\Tests\UnitTestCase;
use Drupal\tripal_chado\Plugin\Field\FieldWidget\ChadoDatetimeWidgetDefault;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests ChadoDatetimeWidgetDefault::validateNotNullConstraint().
 *
 * @coversDefaultClass \Drupal\tripal_chado\Plugin\Field\FieldWidget\ChadoDatetimeWidgetDefault
 *
 * @group tripal-field
 * @group chado-field
 */
#[CoversClass(ChadoDatetimeWidgetDefault::class)]
#[Group('tripal-field')]
#[Group('chado-field')]
class ChadoDatetimeWidgetDefaultTest extends UnitTestCase {

  /**
   * Widget instance created without invoking the plugin constructor.
   */
  private ChadoDatetimeWidgetDefault $widget;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    $ref = new \ReflectionClass(ChadoDatetimeWidgetDefault::class);
    $this->widget = $ref->newInstanceWithoutConstructor();
    $this->widget->setSettings([]);
    $this->widget->setStringTranslation($this->getStringTranslationStub());
  }

  /**
   * Builds a form element and pre-populated FormState for the given scenario.
   *
   * @param string $value
   *   The current value of the datetime textfield.
   * @param int $record_id
   *   The Chado record ID (0 for a new record, >0 for an existing one).
   * @param string $field_name
   *   The field machine name, used to build the element #parents path.
   *
   * @return array{element: array, form_state: \Drupal\Core\Form\FormState}
   */
  private function buildScenario(string $value, int $record_id, string $field_name = 'analysis_timeexecuted'): array {
    // The #parents path mirrors what the widget sets: ['field_name', delta, 'value'].
    $element = [
      '#value'   => $value,
      '#parents' => [$field_name, 0, 'value'],
    ];

    $form_state = new FormState();
    // validateNotNullConstraint() reads the sibling record_id from form state.
    $form_state->setValue([$field_name, 0, 'record_id'], $record_id);

    return ['element' => $element, 'form_state' => $form_state];
  }

  // ---------------------------------------------------------------------------
  // validateNotNullConstraint()
  // ---------------------------------------------------------------------------

  /**
   * A non-empty value never triggers the NOT NULL error, regardless of record_id.
   */
  public function testNonEmptyValueIsAlwaysAccepted(): void {
    ['element' => $element, 'form_state' => $form_state] =
      $this->buildScenario('2025-01-15 10:30:00', record_id: 1);

    $this->widget->validateNotNullConstraint($element, $form_state);

    $this->assertEmpty($form_state->getErrors(),
      'A non-empty value should not produce a form error even for an existing record.');
  }

  /**
   * An empty value on a new record (record_id = 0) is allowed.
   *
   * Chado supplies a DEFAULT timestamp on INSERT, so clearing the field
   * on a new record is safe.
   */
  public function testEmptyValueAllowedForNewRecord(): void {
    ['element' => $element, 'form_state' => $form_state] =
      $this->buildScenario('', record_id: 0);

    $this->widget->validateNotNullConstraint($element, $form_state);

    $this->assertEmpty($form_state->getErrors(),
      'An empty value on a new record (record_id = 0) should be allowed.');
  }

  /**
   * An empty value on an existing record (record_id > 0) is rejected.
   *
   * The columns are NOT NULL and therefore cannot accept NULL during an
   * update.
   */
  public function testEmptyValueRejectedForExistingRecord(): void {
    ['element' => $element, 'form_state' => $form_state] =
      $this->buildScenario('', record_id: 42);

    $this->widget->validateNotNullConstraint($element, $form_state);

    $this->assertNotEmpty($form_state->getErrors(),
      'An empty value on an existing record (record_id > 0) should trigger a form error.');
  }

  /**
   * Whitespace-only input on an existing record is treated as empty and rejected.
   */
  public function testWhitespaceOnlyRejectedForExistingRecord(): void {
    ['element' => $element, 'form_state' => $form_state] =
      $this->buildScenario('   ', record_id: 5);

    $this->widget->validateNotNullConstraint($element, $form_state);

    $this->assertNotEmpty($form_state->getErrors(),
      'Whitespace-only input on an existing record should trigger a form error.');
  }

  /**
   * Build the form element correctly.
   * 
   * 
   */
  #[DataProvider('provideDataForTestFormElementBuild')]
  public function testFormElementBuild(array $item_values, int $expected_record_id): void {
    $item = $this->createMock(\Drupal\Core\Field\FieldItemInterface::class);
      $item->method('getValue')->willReturn($item_values);

      $items = $this->createMock(\Drupal\Core\Field\FieldItemListInterface::class);
      $items->method('offsetGet')->with(0)->willReturn($item);
  
      $field_def = $this->createMock(\Drupal\Core\Field\FieldDefinitionInterface::class);
      $field_def->method('getSetting')
          ->with('storage_plugin_settings')
          ->willReturn(['base_table' => '', 'base_column' => '']);

      (new \ReflectionProperty(\Drupal\Core\Field\WidgetBase::class, 'fieldDefinition'))
          ->setValue($this->widget, $field_def);
  
      $form = [];
      $result = $this->widget->formElement($items, 0, [], $form, new FormState());

      $this->assertArrayHasKey('record_id', $result);
      $this->assertSame('value', $result['record_id']['#type']);
      $this->assertSame($expected_record_id, $result['record_id']['#default_value']);
  }

  /**
   * Provide data for testFormElementBuild() method.
   * 
   * @return array
   *   An array containing the test scenario data.
   */
  public static function provideDataForTestFormElementBuild(): array {
    return [
      'new record with no stored value' => [
        'item_values' => ['value' => '', 'record_id' => 0],
        'expected_record_id' => 0,
      ],
      'existing record with stored value' => [
        'item_values' => ['value' => '2025-01-15 10:30:00', 'record_id' => 42],
        'expected_record_id' => 42,
      ],
    ];
  }

}

<?php

namespace Drupal\Tests\tripal_chado\Kernel\Plugin\ChadoField\Widget;

use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormState;
use Drupal\Tests\tripal_chado\Kernel\ChadoTestKernelBase;
use Drupal\tripal_chado\Plugin\Field\FieldWidget\ChadoDatetimeWidgetDefault;
use PHPUnit\Framework\Attributes\Group;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Kernel tests for ChadoDatetimeWidgetDefault::formElement().
 *
 * Exercises the branch that calls ChadoFieldItemBase::getChadoTableDef() to
 * determine whether the NOT NULL validator should be wired up. A real Chado
 * schema is required for this branch; it cannot be reached from a unit test.
 *
 * Uses analysis.timeexecuted (NOT NULL) and analysis.description (nullable) as
 * representative columns from the same table.
 *
 * @group tripal-field
 * @group chado-field
 */
#[Group('tripal-field')]
#[Group('chado-field')]
#[RunTestsInSeparateProcesses]
class ChadoDatetimeWidgetFormElementTest extends ChadoTestKernelBase {

  /**
   * The theme to use when testing.
   *
   * @var string
   */
  protected $defaultTheme = 'stark';

  /**
   * The modules that this test depends on.
   *
   * @var array
   */
  protected static $modules = [
    'system', 'user', 'path', 'path_alias', 'field', 'datetime', 'tripal', 'tripal_chado',
  ];

  /**
   * An instance of the Chado datetime widget.
   *
   * @var Drupal\tripal_chado\Plugin\Field\FieldWidget\ChadoDatetimeWidgetDefault
   */
  private ChadoDatetimeWidgetDefault $widget;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    $this->getTestSchema(ChadoTestKernelBase::PREPARE_TEST_CHADO);

    $ref = new \ReflectionClass(ChadoDatetimeWidgetDefault::class);
    $this->widget = $ref->newInstanceWithoutConstructor();
    $this->widget->setSettings([]);
  }

  /**
   * Builds a mock FieldItemListInterface returning the given item values.
   */
  private function buildItems(array $item_values): FieldItemListInterface {
    $item = $this->createMock(FieldItemInterface::class);
    $item->method('getValue')->willReturn($item_values);

    $items = $this->createMock(FieldItemListInterface::class);
    $items->method('offsetGet')->with(0)->willReturn($item);

    return $items;
  }

  /**
   * Injects a mocked fieldDefinition with the given Chado storage settings.
   */
  private function injectFieldDefinition(string $base_table, string $base_column): void {
    $field_def = $this->createMock(FieldDefinitionInterface::class);
    $field_def->method('getSetting')
      ->with('storage_plugin_settings')
      ->willReturn(['base_table' => $base_table, 'base_column' => $base_column]);

    (new \ReflectionProperty(WidgetBase::class, 'fieldDefinition'))
      ->setValue($this->widget, $field_def);
  }

  /**
   * A NOT NULL column causes validateNotNullConstraint to be wired up.
   *
   * The column analysis.timeexecuted is NOT NULL, so the validator must
   * be attached.
   */
  public function testNotNullColumnAddsValidator(): void {
    $this->injectFieldDefinition('analysis', 'timeexecuted');
    $form = [];

    $result = $this->widget->formElement(
      $this->buildItems(['value' => '', 'record_id' => 0]), 0, [], $form, new FormState()
    );

    $validator_methods = array_column($result['value']['#element_validate'] ?? [], 1);
    $this->assertContains('validateNotNullConstraint', $validator_methods,
      'validateNotNullConstraint should be added for a NOT NULL column (analysis.timeexecuted).');
  }

  /**
   * A nullable column does NOT cause validateNotNullConstraint to be wired up.
   *
   * The column analysis.description is nullable, so the validator must not
   * be attached.
   */
  public function testNullableColumnOmitsValidator(): void {
    $this->injectFieldDefinition('analysis', 'description');
    $form = [];

    $result = $this->widget->formElement(
      $this->buildItems(['value' => '', 'record_id' => 0]), 0, [], $form, new FormState()
    );

    $validator_methods = array_column($result['value']['#element_validate'] ?? [], 1);
    $this->assertNotContains('validateNotNullConstraint', $validator_methods,
      'validateNotNullConstraint should NOT be added for a nullable column (analysis.description).');
  }

}

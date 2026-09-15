<?php

namespace Drupal\Tests\tripal_chado\Unit\Plugin\Field\Widget;

use Drupal\Core\Field\WidgetBase;
use Drupal\Core\Field\FieldDefinitionInterface;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Form\FormState;
use Drupal\Tests\UnitTestCase;
use Drupal\tripal_chado\Plugin\Field\FieldWidget\ChadoRealWidgetDefault;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests ChadoRealWidgetDefault::validateNotNullConstraint().
 *
 * @coversDefaultClass \Drupal\tripal_chado\Plugin\Field\FieldWidget\ChadoRealWidgetDefault
 *
 * @group tripal-field
 * @group chado-field
 */
#[CoversClass(ChadoRealWidgetDefault::class)]
#[Group('tripal-field')]
#[Group('chado-field')]
class ChadoRealWidgetDefaultTest extends UnitTestCase {

  /**
   * Widget instance created without invoking the plugin constructor.
   */
  private ChadoRealWidgetDefault $widget;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    $ref = new \ReflectionClass(ChadoRealWidgetDefault::class);
    $this->widget = $ref->newInstanceWithoutConstructor();
    $this->widget->setSettings([]);
    $this->widget->setStringTranslation($this->getStringTranslationStub());
  }

  /**
   * Build the form element correctly.
   *
   * @dataProvider provideDataForTestFormElementBuild
   */
  #[DataProvider('provideDataForTestFormElementBuild')]
  public function testFormElementBuild(array $item_values, int $expected_record_id): void {
    $item = $this->createMock(FieldItemInterface::class);
    $item->method('getValue')->willReturn($item_values);

    $items = $this->createMock(FieldItemListInterface::class);
    $items->method('offsetGet')->with(0)->willReturn($item);

    $field_def = $this->createMock(FieldDefinitionInterface::class);
    $field_def->method('getSetting')
      ->with('storage_plugin_settings')
      ->willReturn(['base_table' => '', 'base_column' => '']);

    (new \ReflectionProperty(WidgetBase::class, 'fieldDefinition'))
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
        'item_values' => ['value' => '-987654321.123456789', 'record_id' => 42],
        'expected_record_id' => 42,
      ],
    ];
  }

}

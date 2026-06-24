<?php

namespace Drupal\Tests\tripal_chado\Unit\Plugin\Field\Formatter;

use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\FieldItemList;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\TypedData\Plugin\DataType\ItemList;
use Drupal\Core\TypedData\TypedDataInterface;
use Drupal\Tests\UnitTestCase;
use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalDatetimeTypeFormatter;
use Drupal\tripal_chado\Plugin\Field\FieldFormatter\ChadoDatetimeFormatterDefault;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests ChadoDatetimeFormatterDefault.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Plugin\Field\FieldFormatter\ChadoDatetimeFormatterDefault
 *
 * @group tripal-chado-field
 */
#[CoversClass(ChadoDatetimeFormatterDefault::class)]
#[Group('tripal-chado-field')]
class ChadoDatetimeFormatterDefaultTest extends UnitTestCase {

  /**
   * An instance of the Chado datetime formatter.
   *
   * @var Drupal\tripal_chado\Plugin\Field\FieldFormatter\ChadoDatetimeFormatterDefault
   */
  private ChadoDatetimeFormatterDefault $formatter;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    $ref = new \ReflectionClass(ChadoDatetimeFormatterDefault::class);
    $this->formatter = $ref->newInstanceWithoutConstructor();
    $this->formatter->setSettings(DefaultTripalDatetimeTypeFormatter::defaultSettings());
    $this->formatter->setStringTranslation($this->getStringTranslationStub());
  }

  /**
   * Builds a mocked FieldItemListInterface whose items return the given values.
   */
  private function buildItems(array $raw_values): FieldItemListInterface {
    $item_mocks = [];
    foreach ($raw_values as $raw) {
      $typed_data = $this->prophesize(TypedDataInterface::class);
      $typed_data->getValue()->willReturn($raw);

      $item = $this->prophesize(FieldItemInterface::class);
      $item->get('value')->willReturn($typed_data->reveal());

      $item_mocks[] = $item->reveal();
    }

    $field_item_list = $this->getMockBuilder(FieldItemList::class)
      ->disableOriginalConstructor()
      ->onlyMethods([])
      ->getMock();

    (new \ReflectionProperty(ItemList::class, 'list'))
      ->setValue($field_item_list, $item_mocks);

    return $field_item_list;
  }

  /**
   * Test viewElements() delegates to the parent and returns rendered markup.
   */
  public function testViewElementsDelegatesToParent(): void {
    $elements = $this->formatter->viewElements(
      $this->buildItems(['2025-01-15 10:30:00']), 'en'
    );
    $this->assertArrayHasKey(0, $elements);
    $this->assertSame('2025-01-15 10:30:00', $elements[0]['#markup']);
  }

  /**
   * Test viewElements() hides empty values via the inherited hide logic.
   */
  public function testViewElementsHidesEmptyValue(): void {
    $elements = $this->formatter->viewElements($this->buildItems(['']), 'en');
    $this->assertCount(0, $elements);
  }

}

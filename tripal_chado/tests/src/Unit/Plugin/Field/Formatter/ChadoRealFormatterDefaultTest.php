<?php

namespace Drupal\Tests\tripal_chado\Unit\Plugin\Field\Formatter;

use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\FieldItemList;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\TypedData\Plugin\DataType\ItemList;
use Drupal\Core\TypedData\TypedDataInterface;
use Drupal\Tests\UnitTestCase;
use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalRealTypeFormatter;
use Drupal\tripal_chado\Plugin\Field\FieldFormatter\ChadoRealFormatterDefault;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests ChadoRealFormatterDefault.
 *
 * @coversDefaultClass \Drupal\tripal_chado\Plugin\Field\FieldFormatter\ChadoRealFormatterDefault
 *
 * @group tripal-chado-field
 */
#[CoversClass(ChadoRealFormatterDefault::class)]
#[Group('tripal-chado-field')]
class ChadoRealFormatterDefaultTest extends UnitTestCase {

  /**
   * An instance of the Chado Real formatter.
   *
   * @var Drupal\tripal_chado\Plugin\Field\FieldFormatter\ChadoRealFormatterDefault
   */
  private ChadoRealFormatterDefault $formatter;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    $ref = new \ReflectionClass(ChadoRealFormatterDefault::class);
    $this->formatter = $ref->newInstanceWithoutConstructor();
    $settings = DefaultTripalRealTypeFormatter::defaultSettings();
    $this->formatter->setSettings($settings);
    $this->formatter->setStringTranslation($this->getStringTranslationStub());
  }

  /**
   * Builds a mocked FieldItemListInterface whose items return the given values.
   */
  private function buildItems(array $raw_values): FieldItemListInterface {
    $item_mocks = [];
    foreach ($raw_values as $raw_value) {
      $typed_data = $this->prophesize(TypedDataInterface::class);
      $typed_data->getValue()->willReturn($raw_value);

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
    $test_cases = [
      '0.0' => '0.0',
      '0' => '0',
      '1' => '1',
      '1.0' => '1.0',
      '-1' => '-1',
      '-1.0' => '-1.0',
      '0.123456789123456789' => '0.123456789123456789',
      '-0.123456789123456789' => '-0.123456789123456789',
      '123456789.123456789' => '123456789.123456789',
      '-123456789.123456789' => '-123456789.123456789',
    ];
    $elements = $this->formatter->viewElements(
      $this->buildItems(array_keys($test_cases)), 'en'
    );
    $index = 0;
    foreach ($test_cases as $expected_value) {
      $this->assertArrayHasKey($index, $elements);
      $this->assertSame($expected_value, $elements[$index]['#markup']);
      $index++;
    }
  }

  /**
   * Test viewElements() hides empty values via the inherited hide logic.
   */
  public function testViewElementsHidesEmptyValue(): void {
    // Default is to never hide.
    $elements = $this->formatter->viewElements($this->buildItems(['']), 'en');
    $this->assertCount(1, $elements);

    // Change setting to hide if -1.0.
    $settings = $this->formatter->getSettings();
    $settings['hide_condition'] = 'if_value';
    $settings['hide_value'] = '-1.0';
    $this->formatter->setSettings($settings);
    $elements = $this->formatter->viewElements($this->buildItems(['-1.0']), 'en');
    $this->assertCount(0, $elements);
  }

  /**
   * Test viewElements() optional settings.
   */
  public function testViewElementsSettings(): void {
    $test_cases = [
      '0.0' => 'abc 0,00 XYZ',
      '0' => 'abc 0,00 XYZ',
      '1' => 'abc 1,00 XYZ',
      '1.0' => 'abc 1,00 XYZ',
      '-1' => 'abc -1,00 XYZ',
      '-1.0' => 'abc -1,00 XYZ',
      '0.123456789123456789' => 'abc 0,12 XYZ',
      '-0.123456789123456789' => 'abc -0,12 XYZ',
      '123456789.123456789' => 'abc 123.456.789,12 XYZ',
      '-123456789.123456789' => 'abc -123.456.789,12 XYZ',
    ];
    $settings = $this->formatter->getSettings();
    $settings['thousand_separator'] = '.';
    $settings['decimal_separator'] = ',';
    $settings['decimal_places'] = '2';
    $settings['field_prefix'] = 'abc ';
    $settings['field_suffix'] = ' XYZ';
    $this->formatter->setSettings($settings);
    $elements = $this->formatter->viewElements(
      $this->buildItems(array_keys($test_cases)), 'en'
    );
    $index = 0;
    foreach ($test_cases as $expected_value) {
      $this->assertArrayHasKey($index, $elements);
      $this->assertSame($expected_value, $elements[$index]['#markup']);
      $index++;
    }
  }

}

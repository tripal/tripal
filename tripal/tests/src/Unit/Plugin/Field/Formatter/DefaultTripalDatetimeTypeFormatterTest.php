<?php

namespace Drupal\Tests\tripal\Unit\Plugin\Field\Formatter;

use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\FieldItemList;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\TypedData\Plugin\DataType\ItemList;
use Drupal\Core\TypedData\TypedDataInterface;
use Drupal\Tests\UnitTestCase;
use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalDatetimeTypeFormatter;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests DefaultTripalDatetimeTypeFormatter.
 *
 * @coversDefaultClass \Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalDatetimeTypeFormatter
 *
 * @group tripal-field
 */
#[CoversClass(DefaultTripalDatetimeTypeFormatter::class)]
#[Group('tripal-field')]
class DefaultTripalDatetimeTypeFormatterTest extends UnitTestCase {

  /**
   * Formatter instance created without invoking the plugin constructor.
   */
  private DefaultTripalDatetimeTypeFormatter $formatter;

  /**
   * Reflected handle to the protected formatTimestamp() method.
   */
  private \ReflectionMethod $formatTimestamp;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    $ref = new \ReflectionClass(DefaultTripalDatetimeTypeFormatter::class);
    $this->formatter = $ref->newInstanceWithoutConstructor();
    // FormatterBase re-declares $settings without a default value, so it is
    // NULL after newInstanceWithoutConstructor(). Seed it with defaults so
    // getSetting() works for any test that doesn't call setSetting() first.
    $this->formatter->setSettings(DefaultTripalDatetimeTypeFormatter::defaultSettings());

    $this->formatTimestamp = new \ReflectionMethod(
      DefaultTripalDatetimeTypeFormatter::class,
      'formatTimestamp'
    );
  }

  /**
   * Builds a mocked FieldItemListInterface whose items return the given values.
   *
   * @param string[] $raw_values
   *   Raw stored timestamp strings, one per field delta.
   *
   * @return \Drupal\Core\Field\FieldItemListInterface
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

    // FieldItemList inherits getIterator() from ItemList, which returns
    // new \ArrayIterator($this->list). We bypass the constructor and inject
    // our item mocks into that protected property directly.
    $field_item_list = $this->getMockBuilder(FieldItemList::class)
      ->disableOriginalConstructor()
      ->onlyMethods([])
      ->getMock();

    (new \ReflectionProperty(ItemList::class, 'list'))
      ->setValue($field_item_list, $item_mocks);

    return $field_item_list;
  }

  // ---------------------------------------------------------------------------
  // formatTimestamp()
  // ---------------------------------------------------------------------------

  /**
   * Inputs and expected outputs for testFormatTimestamp().
   */
  public static function provideFormatTimestamp(): array {
    return [
      'standard input, default format' => [
        '2025-01-15 10:30:00', 'Y-m-d H:i:s', '2025-01-15 10:30:00',
      ],
      'microsecond input, default format strips microseconds' => [
        '2025-01-15 10:30:00.123456', 'Y-m-d H:i:s', '2025-01-15 10:30:00',
      ],
      'microsecond input, microsecond format preserves precision' => [
        '2025-01-15 10:30:00.123456', 'Y-m-d H:i:s.u', '2025-01-15 10:30:00.123456',
      ],
      'standard input, date-only format' => [
        '2025-01-15 10:30:00', 'Y-m-d', '2025-01-15',
      ],
      'standard input, human-readable format' => [
        '2025-01-15 10:30:00', 'F j, Y', 'January 15, 2025',
      ],
      'empty string returns empty' => [
        '', 'Y-m-d H:i:s', '',
      ],
    ];
  }

  /**
   * @dataProvider provideFormatTimestamp
   */
  #[DataProvider('provideFormatTimestamp')]
  public function testFormatTimestamp(string $raw, string $format, string $expected): void {
    $result = $this->formatTimestamp->invoke($this->formatter, $raw, $format);
    $this->assertSame($expected, $result);
  }

  // ---------------------------------------------------------------------------
  // viewElements() — hide conditions
  // ---------------------------------------------------------------------------

  /**
   * Empty value is hidden when hide_condition is the default empty string.
   */
  public function testViewElementsHidesEmptyByDefault(): void {
    $elements = $this->formatter->viewElements($this->buildItems(['']), 'en');
    $this->assertCount(0, $elements,
      'An empty value should be hidden when hide_condition is "".');
  }

  /**
   * Non-empty value is shown under the default hide condition.
   */
  public function testViewElementsShowsNonEmptyByDefault(): void {
    $elements = $this->formatter->viewElements(
      $this->buildItems(['2025-01-15 10:30:00']), 'en'
    );
    $this->assertArrayHasKey(0, $elements,
      'A non-empty value should be rendered under the default hide condition.');
  }

  /**
   * hide_condition = 'never' renders even an empty value.
   */
  public function testViewElementsNeverHideShowsEmpty(): void {
    $this->formatter->setSetting('hide_condition', 'never');
    $elements = $this->formatter->viewElements($this->buildItems(['']), 'en');
    $this->assertArrayHasKey(0, $elements,
      'Empty value should appear when hide_condition is "never".');
  }

  /**
   * hide_condition = 'if_value' hides the value when it matches hide_value.
   */
  public function testViewElementsHidesMatchingValue(): void {
    $this->formatter->setSetting('hide_condition', 'if_value');
    $this->formatter->setSetting('hide_value', '2025-01-15 10:30:00');
    $elements = $this->formatter->viewElements(
      $this->buildItems(['2025-01-15 10:30:00']), 'en'
    );
    $this->assertCount(0, $elements,
      'A value equal to hide_value should be hidden when hide_condition is "if_value".');
  }

  /**
   * hide_condition = 'if_value' shows a value that does not match hide_value.
   */
  public function testViewElementsShowsNonMatchingValue(): void {
    $this->formatter->setSetting('hide_condition', 'if_value');
    $this->formatter->setSetting('hide_value', '2025-01-15 10:30:00');
    $elements = $this->formatter->viewElements(
      $this->buildItems(['2026-06-20 14:45:00']), 'en'
    );
    $this->assertArrayHasKey(0, $elements,
      'A value that does not match hide_value should still be rendered.');
  }

  // ---------------------------------------------------------------------------
  // viewElements() — prefix, suffix, and format
  // ---------------------------------------------------------------------------

  /**
   * field_prefix and field_suffix are prepended and appended to the markup.
   */
  public function testViewElementsAppliesPrefixAndSuffix(): void {
    $this->formatter->setSetting('date_format', 'Y-m-d');
    $this->formatter->setSetting('field_prefix', 'Date: ');
    $this->formatter->setSetting('field_suffix', ' (UTC)');
    $elements = $this->formatter->viewElements(
      $this->buildItems(['2025-01-15 10:30:00']), 'en'
    );
    $this->assertSame('Date: 2025-01-15 (UTC)', $elements[0]['#markup']);
  }

  /**
   * The configured date_format is applied to the stored timestamp.
   */
  public function testViewElementsRespectsDateFormat(): void {
    $this->formatter->setSetting('date_format', 'F j, Y');
    $elements = $this->formatter->viewElements(
      $this->buildItems(['2025-01-15 10:30:00']), 'en'
    );
    $this->assertSame('January 15, 2025', $elements[0]['#markup']);
  }

}

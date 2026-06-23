<?php

namespace Drupal\Tests\tripal\Unit\Plugin\Field\Widget;

use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Tests\UnitTestCase;
use Drupal\tripal\Plugin\Field\FieldWidget\TripalDatetimeTypeWidget;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;
use Prophecy\Argument;

/**
 * Tests TripalDatetimeTypeWidget.
 *
 * @coversDefaultClass \Drupal\tripal\Plugin\Field\FieldWidget\TripalDatetimeTypeWidget
 *
 * @group tripal-field
 */
#[CoversClass(TripalDatetimeTypeWidget::class)]
#[Group('tripal-field')]
class TripalDatetimeTypeWidgetTest extends UnitTestCase {

  /**
   * Widget instance created without invoking the plugin constructor.
   */
  private TripalDatetimeTypeWidget $widget;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    $ref = new \ReflectionClass(TripalDatetimeTypeWidget::class);
    $this->widget = $ref->newInstanceWithoutConstructor();
    // WidgetBase re-declares $settings without a default value (same pattern
    // as FormatterBase), so seed it to avoid NULL errors in getSetting().
    $this->widget->setSettings([]);
    $this->widget->setStringTranslation($this->getStringTranslationStub());
  }

  // ---------------------------------------------------------------------------
  // formElement()
  // ---------------------------------------------------------------------------

  /**
   * formElement() returns the expected textfield structure with the validator.
   */
  public function testFormElementStructure(): void {
    $item = $this->createMock(FieldItemInterface::class);
    $items = $this->createMock(FieldItemListInterface::class);
    $items->method('offsetGet')->with(0)->willReturn($item);

    $form = [];
    $result = $this->widget->formElement($items, 0, [], $form, $this->createMock(FormStateInterface::class));

    $this->assertArrayHasKey('value', $result);
    $this->assertSame('textfield', $result['value']['#type']);
    $this->assertSame('YYYY-MM-DD HH:MM:SS', $result['value']['#placeholder']);
    $this->assertContains('js-text-full', $result['value']['#attributes']['class']);
    $this->assertContains('text-full', $result['value']['#attributes']['class']);
    $this->assertNotEmpty((string) $result['value']['#description']);
    $validator_methods = array_column($result['value']['#element_validate'], 1);
    $this->assertContains('validateDatetimeValue', $validator_methods);
  }

  // ---------------------------------------------------------------------------
  // massageFormValues()
  // ---------------------------------------------------------------------------

  /**
   * Input/output pairs for testMassageFormValues().
   *
   * Each entry: [input values array, expected values array].
   */
  public static function provideMassageFormValues(): array {
    return [
      'standard format is unchanged' => [
        [['value' => '2025-01-15 10:30:00', 'record_id' => 0]],
        [0 => ['value' => '2025-01-15 10:30:00', 'record_id' => 0]],
      ],
      'ISO 8601 T separator normalised to space' => [
        [['value' => '2025-01-15T10:30:00', 'record_id' => 0]],
        [0 => ['value' => '2025-01-15 10:30:00', 'record_id' => 0]],
      ],
      'date-only input gets midnight time appended' => [
        [['value' => '2025-01-15', 'record_id' => 0]],
        [0 => ['value' => '2025-01-15 00:00:00', 'record_id' => 0]],
      ],
      'HH:MM without seconds gets :00 appended' => [
        [['value' => '2025-01-15 10:30', 'record_id' => 0]],
        [0 => ['value' => '2025-01-15 10:30:00', 'record_id' => 0]],
      ],
      'sub-second precision is preserved' => [
        [['value' => '2025-01-15 10:30:00.123456', 'record_id' => 0]],
        [0 => ['value' => '2025-01-15 10:30:00.123456', 'record_id' => 0]],
      ],
      'empty value is removed from the array' => [
        [['value' => '', 'record_id' => 0]],
        [],
      ],
      'whitespace-only value is removed from the array' => [
        [['value' => '   ', 'record_id' => 0]],
        [],
      ],
      'zero sentinel is converted to -infinity' => [
        [['value' => '0', 'record_id' => 0]],
        [0 => ['value' => '-infinity', 'record_id' => 0]],
      ],
    ];
  }

  /**
   * @dataProvider provideMassageFormValues
   */
  #[DataProvider('provideMassageFormValues')]
  public function testMassageFormValues(array $input, array $expected): void {
    $form_state = $this->prophesize(FormStateInterface::class)->reveal();
    $result = $this->widget->massageFormValues($input, [], $form_state);
    $this->assertSame($expected, $result);
  }

  // ---------------------------------------------------------------------------
  // validateDatetimeValue()
  // ---------------------------------------------------------------------------

  /**
   * Valid inputs for testValidateDatetimeValueAcceptsValid().
   */
  public static function provideValidDatetimes(): array {
    return [
      'standard format'           => ['2025-01-15 10:30:00'],
      'ISO 8601 with T separator' => ['2025-01-15T10:30:00'],
      'date only'                 => ['2025-01-15'],
      'HH:MM without seconds'     => ['2025-01-15 10:30'],
      'sub-second precision'      => ['2025-01-15 10:30:00.123456'],
      'zero sentinel'             => ['0'],
      '-infinity sentinel'        => ['-infinity'],
    ];
  }

  /**
   * Valid inputs are accepted without setting a form error.
   *
   * @dataProvider provideValidDatetimes
   */
  #[DataProvider('provideValidDatetimes')]
  public function testValidateDatetimeValueAcceptsValid(string $value): void {
    $form_state = $this->prophesize(FormStateInterface::class);
    $form_state->setError(Argument::any(), Argument::any())->shouldNotBeCalled();

    $element = ['#value' => $value, '#parents' => []];
    $this->widget->validateDatetimeValue($element, $form_state->reveal());
  }

  /**
   * An empty value is allowed (nullable field) without setting a form error.
   */
  public function testValidateDatetimeValueAllowsEmpty(): void {
    $form_state = $this->prophesize(FormStateInterface::class);
    $form_state->setError(Argument::any(), Argument::any())->shouldNotBeCalled();

    $element = ['#value' => '', '#parents' => []];
    $this->widget->validateDatetimeValue($element, $form_state->reveal());
  }

  /**
   * Invalid inputs for testValidateDatetimeValueRejectsInvalid().
   */
  public static function provideInvalidDatetimes(): array {
    return [
      'plain text'          => ['not-a-date'],
      'wrong order'         => ['30:10:2025'],
      'incomplete ISO'      => ['2025-01'],
      'time only'           => ['10:30:00'],
    ];
  }

  /**
   * Invalid inputs trigger a form error.
   *
   * @dataProvider provideInvalidDatetimes
   */
  #[DataProvider('provideInvalidDatetimes')]
  public function testValidateDatetimeValueRejectsInvalid(string $value): void {
    $form_state = $this->prophesize(FormStateInterface::class);
    $form_state->setError(Argument::any(), Argument::any())->shouldBeCalled();

    $element = ['#value' => $value, '#parents' => []];
    $this->widget->validateDatetimeValue($element, $form_state->reveal());
  }

}

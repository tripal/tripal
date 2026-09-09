<?php

namespace Drupal\Tests\tripal\Unit\Plugin\Field\Widget;

use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Tests\UnitTestCase;
use Drupal\tripal\Plugin\Field\FieldWidget\TripalRealTypeWidget;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;
use Prophecy\Argument;

/**
 * Tests TripalRealTypeWidget.
 *
 * @coversDefaultClass \Drupal\tripal\Plugin\Field\FieldWidget\TripalRealTypeWidget
 *
 * @group tripal-field
 */
#[CoversClass(TripalRealTypeWidget::class)]
#[Group('tripal-field')]
class TripalRealTypeWidgetTest extends UnitTestCase {

  /**
   * Widget instance created without invoking the plugin constructor.
   */
  private TripalRealTypeWidget $widget;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    $ref = new \ReflectionClass(TripalRealTypeWidget::class);
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
    $this->assertSame(NULL, $result['value']['#placeholder']);
    $this->assertEquals('', $result['value']['#default_value']);
    $this->assertContains('js-text-full', $result['value']['#attributes']['class']);
    $this->assertContains('text-full', $result['value']['#attributes']['class']);
    $validator_methods = array_column($result['value']['#element_validate'], 1);
    $this->assertContains('validateRealValue', $validator_methods);
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
      'empty value is removed from the array' => [
        [['value' => '', 'record_id' => 0]],
        [],
      ],
      'whitespace-only value is removed from the array' => [
        [['value' => '   ', 'record_id' => 0]],
        [],
      ],
    ];
  }

  /**
   * Tests that massaging of various values returns expected output.
   *
   * @dataProvider provideMassageFormValues
   */
  #[DataProvider('provideMassageFormValues')]
  public function testMassageFormValues(array $input, array $expected): void {
    $form_state = $this->prophesize(FormStateInterface::class)->reveal();
    $result = $this->widget->massageFormValues($input, [], $form_state);
    $this->assertSame($expected, $result);
  }

  // ---------------------------------------------------------------------------
  // validateRealValue()
  // ---------------------------------------------------------------------------

  /**
   * Valid inputs for testValidateRealValueAcceptsValid().
   */
  public static function provideValidReals(): array {
    return [
      'positive real' => ['123.456'],
      'negative real' => ['-123.456'],
      'explicit positive real' => ['+123.456'],
      'positive integer' => ['123'],
      'negative integer' => ['-123'],
      'positive scientific notation' => ['123.456E78'],
      'negative scientific notation' => ['-123.456e-78'],
      'thousand separators' => ['12,345,678.90'],
      '-infinity sentinel full' => ['-infinity'],
      '-infinity sentinel abbreviated' => ['-Inf'],
      '+infinity sentinel full' => ['+INFINITY'],
      '+infinity sentinel abbreviated' => ['+inf'],
    ];
  }

  /**
   * Valid inputs are accepted without setting a form error.
   *
   * @dataProvider provideValidReals
   */
  #[DataProvider('provideValidReals')]
  public function testValidateRealValueAcceptsValid(string $value): void {
    $form_state = $this->prophesize(FormStateInterface::class);
    $form_state->setError(Argument::any(), Argument::any())->shouldNotBeCalled();

    $element = ['#value' => $value, '#parents' => []];
    $this->widget->validateRealValue($element, $form_state->reveal());
  }

  /**
   * An empty value is allowed (nullable field) without setting a form error.
   */
  public function testValidateRealValueAllowsEmpty(): void {
    $form_state = $this->prophesize(FormStateInterface::class);
    $form_state->setError(Argument::any(), Argument::any())->shouldNotBeCalled();

    $element = ['#value' => '', '#parents' => []];
    $this->widget->validateRealValue($element, $form_state->reveal());
  }

  /**
   * Invalid inputs for testValidateRealValueRejectsInvalid().
   */
  public static function provideInvalidReals(): array {
    return [
      'text' => ['notanumber'],
      'multiple signs' => ['+-1.234'],
      'multiple decimals' => ['1.234.567'],
      'internal minus' => ['1-234.0'],
    ];
  }

  /**
   * Invalid inputs trigger a form error.
   *
   * @dataProvider provideInvalidReals
   */
  #[DataProvider('provideInvalidReals')]
  public function testValidateRealValueRejectsInvalid(string $value): void {
    $form_state = $this->prophesize(FormStateInterface::class);
    $form_state->setError(Argument::any(), Argument::any())->shouldBeCalled();

    $element = ['#value' => $value, '#parents' => []];
    $this->widget->validateRealValue($element, $form_state->reveal());
  }

}

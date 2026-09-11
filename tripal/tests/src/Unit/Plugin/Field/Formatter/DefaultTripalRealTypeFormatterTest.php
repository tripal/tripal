<?php

namespace Drupal\Tests\tripal\Unit\Plugin\Field\Formatter;

use Drupal\Core\Field\FieldItemInterface;
use Drupal\Core\Field\FieldItemList;
use Drupal\Core\Field\FieldItemListInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\TypedData\Plugin\DataType\ItemList;
use Drupal\Core\TypedData\TypedDataInterface;
use Drupal\Tests\UnitTestCase;
use Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalRealTypeFormatter;
use PHPUnit\Framework\Attributes\CoversClass;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Group;

/**
 * Tests DefaultTripalRealTypeFormatter.
 *
 * @coversDefaultClass \Drupal\tripal\Plugin\Field\FieldFormatter\DefaultTripalRealTypeFormatter
 *
 * @group tripal-field
 */
#[CoversClass(DefaultTripalRealTypeFormatter::class)]
#[Group('tripal-field')]
class DefaultTripalRealTypeFormatterTest extends UnitTestCase {

  /**
   * Formatter instance created without invoking the plugin constructor.
   */
  private DefaultTripalRealTypeFormatter $formatter;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {
    parent::setUp();

    $ref = new \ReflectionClass(DefaultTripalRealTypeFormatter::class);
    $this->formatter = $ref->newInstanceWithoutConstructor();
    // FormatterBase re-declares $settings without a default value, so it is
    // NULL after newInstanceWithoutConstructor(). Seed it with defaults so
    // getSetting() works for any test that doesn't call setSetting() first.
    $this->formatter->setSettings(DefaultTripalRealTypeFormatter::defaultSettings());

    // We use t() a lot, let's make sure we can test functions that use it.
    $this->formatter->setStringTranslation($this->getStringTranslationStub());
  }

  /**
   * Builds a mocked FieldItemListInterface whose items return the given values.
   *
   * @param string[] $raw_values
   *   Raw stored real strings, one per field delta.
   *
   * @return \Drupal\Core\Field\FieldItemListInterface
   *   The field item list.
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

  /**
   * Inputs and expected outputs for the field formatter.
   *
   * Tests formatReal() function.
   */
  public static function provideScenarios(): array {
    $scenarios = [];
    $scenarios[] = [
      'label' => 'Default settings with typical float value',
      'settings' => [],
      'value' => '-12345.6789',
      'expect' => '-12345.6789',
    ];
    $scenarios[] = [
      'label' => 'Default settings with no decimal places',
      'settings' => [],
      'value' => '-12345',
      'expect' => '-12345',
    ];
    $scenarios[] = [
      'label' => 'Value with many decimals rounded to two decimal places',
      'settings' => [
        'decimal_places' => 2,
      ],
      'value' => '-12345.6789',
      'expect' => '-12345.68',
    ];
    $scenarios[] = [
      'label' => 'Value without decimals rounded to two decimal places',
      'settings' => [
        'decimal_places' => 2,
      ],
      'value' => '-12345',
      'expect' => '-12345.00',
    ];
    $scenarios[] = [
      'label' => 'Non-default thousand and decimal separators',
      'settings' => [
        'thousand_separator' => '#',
        'decimal_separator' => 's',
      ],
      'value' => '-1234567.7654321',
      'expect' => '-1#234#567s7654321',
    ];
    $scenarios[] = [
      'label' => 'Prefix and suffix are applied',
      'settings' => [
        'field_prefix' => 'altitude: ',
        'field_suffix' => ' km',
      ],
      'value' => '-1234567.7654321',
      'expect' => 'altitude: -1234567.7654321 km',
    ];
    $scenarios[] = [
      'label' => 'Zero value should not be hidden by default',
      // Default hide_condition for this field is 'never'.
      'settings' => [],
      'value' => '0',
      'expect' => '0',
    ];
    $scenarios[] = [
      'label' => 'Zero value should be hidden for empty hide_condition',
      'settings' => [
        'hide_condition' => '',
      ],
      'value' => '0',
    ];
    $scenarios[] = [
      'label' => 'Float zero value should not be hidden by default',
      // Default hide_condition for this field is 'never'.
      'settings' => [],
      'value' => '0.0',
      'expect' => '0.0',
    ];
    $scenarios[] = [
      'label' => 'Float zero value should not be hidden for empty hide_condition',
      'settings' => [
        'hide_condition' => '',
      ],
      'value' => '0.0',
      'expect' => '0.0',
    ];
    $scenarios[] = [
      'label' => 'Float zero value should be hidden for specific hide condition',
      'settings' => [
        'hide_condition' => 'if_value',
        'hide_value' => '0.0',
      ],
      'value' => '0.0',
    ];
    $scenarios[] = [
      'label' => 'Can use -infinity to hide a value',
      'settings' => [
        'hide_condition' => 'if_value',
        'hide_value' => '-infinity',
      ],
      'value' => '-infinity',
    ];
    return $scenarios;
  }

  /**
   * Tests the output from formatReal() scenarios.
   *
   * @dataProvider provideScenarios
   */
  #[DataProvider('provideScenarios')]
  public function testFormatReal(string $label, array $settings, string $value, ?string $expect = NULL): void {
    foreach ($settings as $key => $setting) {
      $this->formatter->setSetting($key, $setting);
    }
    $elements = $this->formatter->viewElements($this->buildItems([$value]), 'en');
    if (!is_null($expect)) {
      $this->assertArrayHasKey(0, $elements,
        $label . ': Did not return any value');
      $this->assertSame($expect, $elements[0]['#markup'],
        $label . ': Did not return expected value');
    }
    else {
      $this->assertCount(0, $elements, $label . ': Did not expect a value');
    }
  }

  /**
   * Non-empty value is shown under the default hide condition.
   */
  public function testViewElementsShowsNonEmptyByDefault(): void {
    $elements = $this->formatter->viewElements(
      $this->buildItems(['-1.0']), 'en'
    );
    $this->assertArrayHasKey(0, $elements,
      'A non-empty value should be rendered under the default hide condition.');
  }

  /**
   * Hide_condition = 'never' renders even an empty value.
   */
  public function testViewElementsNeverHideShowsEmpty(): void {
    $this->formatter->setSetting('hide_condition', 'never');
    $elements = $this->formatter->viewElements($this->buildItems(['0']), 'en');
    $this->assertArrayHasKey(0, $elements,
      'Zero value should appear when hide_condition is "never".');
  }

  /**
   * Hide_condition = 'if_value' hides the value when it matches hide_value.
   */
  public function testViewElementsHidesMatchingValue(): void {
    $this->formatter->setSetting('hide_condition', 'if_value');
    $this->formatter->setSetting('hide_value', '-infinity');
    $elements = $this->formatter->viewElements(
      $this->buildItems(['-infinity']), 'en'
    );
    $this->assertCount(0, $elements,
      'A value equal to hide_value should be hidden when hide_condition is "if_value".');
  }

  /**
   * Hide_condition = 'if_value' shows a value that does not match hide_value.
   */
  public function testViewElementsShowsNonMatchingValue(): void {
    $this->formatter->setSetting('hide_condition', 'if_value');
    $this->formatter->setSetting('hide_value', '0');
    $elements = $this->formatter->viewElements(
      $this->buildItems(['0.000001']), 'en'
    );
    $this->assertArrayHasKey(0, $elements,
      'A value that does not match hide_value should be rendered.');
  }

  /**
   * Field_prefix and field_suffix are prepended and appended to the markup.
   */
  public function testViewElementsAppliesPrefixAndSuffix(): void {
    $this->formatter->setSetting('field_prefix', 'distance: ');
    $this->formatter->setSetting('field_suffix', ' km');
    $elements = $this->formatter->viewElements(
      $this->buildItems(['-12345.6789']), 'en'
    );
    $this->assertSame('distance: -12345.6789 km', $elements[0]['#markup']);
  }

  /**
   * The configured decimal_places is applied to the stored real number.
   */
  public function testViewElementsRespectsRealFormat(): void {
    $this->formatter->setSetting('decimal_places', '3');
    $elements = $this->formatter->viewElements(
      $this->buildItems(['-12345.6789012345']), 'en'
    );
    $this->assertSame('-12345.679', $elements[0]['#markup']);
  }

  /**
   * The settings form (via settingsForm) gets built correctly.
   */
  public function testSettingsFormBuild(): void {
    $form_state = $this->createMock(FormStateInterface::class);
    $form = $this->formatter->settingsForm([], $form_state);

    // Check that all expected keys are present.
    $keys = [
      'thousand_separator',
      'decimal_separator',
      'decimal_places',
      'field_prefix',
      'field_suffix',
      'hide_condition',
      'hide_value',
    ];
    foreach ($keys as $key) {
      $this->assertArrayHasKey($key, $form);
    }

    // hide_condition is a radios element with the expected options.
    $this->assertSame('radios', $form['hide_condition']['#type']);
    $this->assertArrayHasKey('', $form['hide_condition']['#options']);
    $this->assertArrayHasKey('never', $form['hide_condition']['#options']);
    $this->assertArrayHasKey('if_value', $form['hide_condition']['#options']);

    // These are optional.
    $this->assertSame('textfield', $form['thousand_separator']['#type']);
    $this->assertFalse($form['thousand_separator']['#required']);
    $this->assertSame('textfield', $form['decimal_separator']['#type']);
    $this->assertFalse($form['decimal_separator']['#required']);
    $this->assertSame('number', $form['decimal_places']['#type']);
    $this->assertFalse($form['decimal_places']['#required']);
    $this->assertSame('textfield', $form['field_prefix']['#type']);
    $this->assertFalse($form['field_prefix']['#required']);
    $this->assertSame('textfield', $form['field_suffix']['#type']);
    $this->assertFalse($form['field_suffix']['#required']);
  }

  /**
   * The summary from settingsSummary() is correct when default.
   */
  public function testSettingsSummaryDefaultFormat(): void {
    $summary = $this->formatter->settingsSummary();
    $this->assertNotEmpty($summary);
    $entry = $summary[0];
    $this->assertStringContainsString('Places: not specified', $entry);
  }

  /**
   * A custom format is generated by settingsSummary().
   */
  public function testSettingsSummaryCustomFormat(): void {
    $this->formatter->setSetting('decimal_places', '4');
    $summary = $this->formatter->settingsSummary();
    $entry = (string) $summary[0];
    $this->assertStringContainsString('Places: 4', $entry);
  }

}

<?php

namespace Drupal\Tests\tripal\Kernel\Services\TripalTokenParser;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use PHPUnit\Framework\Attributes\Group;


/**
 * Tests the TripalTokenParser service functions.
 *
 * @group Tripal
 * @group Tripal Services
 * @group TripalTokenParser
 */
#[Group('Tripal')]
#[Group('Tripal Services')]
#[Group('TripalTokenParser')]
class TripalTokenParserTest extends TripalTestKernelBase {

  protected static $modules = ['system', 'user', 'path', 'path_alias', 'tripal'];

  /**
   * The tripal token parser service
   *
   * @var \Drupal\tripal\Services\TripalTokenParser
   */
  protected object $token_parser;

  /**
   * {@inheritdoc}
   */
  protected function setUp(): void {

    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    /** @var \Drupal\tripal\Services\TripalTokenParser $this->token_parser **/
    $this->token_parser = \Drupal::service('tripal.token_parser');
  }

  /**
   * Tests the TripalTokenParser class public functions.
   */
  public function testTripalTokenParser() {

    $token_values = [
      'abc' => 'a1',
      'def' => 'd2',
      'ghi' => 'g3',
      'empty' => '',
      'null' => NULL
    ];

    // String with no tokens
    $token_string = 'String with no tokens';
    $invalid_tokens = $this->token_parser->validateTokens($token_string, $token_values);
    $this->assertEmpty($invalid_tokens, 'A string with no tokens should not have invalid tokens');
    $replaced_string = $this->token_parser->replaceTokens($token_string, $token_values);
    $this->assertEquals($token_string, $replaced_string, 'A string with no tokens should be unchanged');

    // String with valid tokens
    $token_string = 'String with [abc] valid [([def])][ +[empty]+] tokens';
    $expected_string = 'String with a1 valid (d2) tokens';
    $invalid_tokens = $this->token_parser->validateTokens($token_string, $token_values);
    $this->assertEmpty($invalid_tokens, 'All tokens should be valid');
    $replaced_string = $this->token_parser->replaceTokens($token_string, $token_values);
    $this->assertEquals($expected_string, $replaced_string, 'Did not get the expected token replacement for valid tokens');

    // String with both invalid and valid tokens
    $token_string = 'String with [nope] [ [morenope]]invalid and valid[-[abc]] tokens';
    $expected_string = 'String with  invalid and valid-a1 tokens';
    $expected_invalid = ['nope', 'morenope'];
    $invalid_tokens = $this->token_parser->validateTokens($token_string, $token_values);
    $this->assertEquals($expected_invalid, $invalid_tokens, 'Expected two invalid tokens from validation');
    $replaced_string = $this->token_parser->replaceTokens($token_string, $token_values);
    $this->assertEquals($expected_string, $replaced_string, 'Did not get the expected token replacement for invalid + valid tokens');

    // Tokens with multiple options
    $token_string = 'Tokens with multiple [[null|abc|def]+][nope|def|abc] options';
    $expected_string = 'Tokens with multiple a1+d2 options';
    $expected_invalid = ['nope'];
    $invalid_tokens = $this->token_parser->validateTokens($token_string, $token_values);
    $this->assertEquals($expected_invalid, $invalid_tokens, 'Expected one invalid token from validation');
    $replaced_string = $this->token_parser->replaceTokens($token_string, $token_values);
    $this->assertEquals($expected_string, $replaced_string, 'Did not get the expected token replacement for tokens with multiple options');

    // Prefix or suffix matches beginning or end of token value
    $token_string = 'Prefix matches token value [a[abc],] suffix matches token value [:[ghi]g3].';
    $expected_string = 'Prefix matches token value a1, suffix matches token value :g3.';
    $invalid_tokens = $this->token_parser->validateTokens($token_string, $token_values);
    $this->assertEmpty($invalid_tokens, 'All tokens should be valid');
    $replaced_string = $this->token_parser->replaceTokens($token_string, $token_values);
    $this->assertEquals($expected_string, $replaced_string, 'Did not get the expected token replacement for prefix suffix test');

    // Replace an array of token strings
    $token_string_array = ['String [abc] 1', 'String [def] 2', 'String [empty] 3'];
    $expected_string_array = ['String a1 1', 'String d2 2', 'String  3'];
    $replaced_string_array = $this->token_parser->replaceTokensArray($token_string_array, $token_values);
    $this->assertEquals($expected_string_array, $replaced_string_array, 'Did not get all expected token replacements for an array of token strings');

  }
}

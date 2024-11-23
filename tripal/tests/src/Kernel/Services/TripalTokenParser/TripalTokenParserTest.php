<?php

namespace Drupal\Tests\tripal\Kernel\Services\TripalTokenParser;

use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use Drupal\Core\Url;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\tripal\TripalVocabTerms\Interfaces\TripalIdSpaceInterface;

/**
 * Tests the TripalTokenParser service functions.
 *
 * @group Tripal
 * @group Tripal Services
 * @group TripalTokenParser
 */
class TripalTokenParserTest extends TripalTestKernelBase {

  protected static $modules = ['system', 'user', 'path', 'path_alias', 'tripal'];

  /**
   * The token parser service
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

    /** @var \Drupal\tripal\Services\TripalTokenParser $token_parser **/
    $this->token_parser = \Drupal::service('tripal.token_parser');
  }

  /**
   * Tests the TripalTokenParser class public functions.
   */
  public function testTripalTokenParser() {

    $token_values = ['abc' => 'a1', 'def' => 'd2', 'ghi' => 'g3', 'nul' => ''];

    $token_string = 'String with no tokens';
    $invalid_tokens = $this->token_parser->validateTokens($token_string, $token_values);
    $this->assertEmpty($invalid_tokens, 'A string with no tokens should not have invalid tokens');
    $replaced_string = $this->token_parser->replaceTokens($token_string, $token_values);
    $this->assertEquals($token_string, $replaced_string, 'A string with no tokens should be unchanged');

    $token_string = 'String with [abc] valid [([def])][ +[nul]+] tokens';
    $expected_string = 'String with a1 valid (d2) tokens';
    $invalid_tokens = $this->token_parser->validateTokens($token_string, $token_values);
    $this->assertEmpty($invalid_tokens, 'All tokens should be valid');
    $replaced_string = $this->token_parser->replaceTokens($token_string, $token_values);
    $this->assertEquals($expected_string, $replaced_string, 'Did not get the expected token replacement for valid tokens');

    $token_string = 'String with [nope] [ [morenope]]invalid and valid[-[abc]] tokens';
    $expected_string = 'String with  invalid and valid-a1 tokens';
    $expected_invalid = ['nope', 'morenope'];
    $invalid_tokens = $this->token_parser->validateTokens($token_string, $token_values);
    $this->assertEquals($expected_invalid, $invalid_tokens, 'Expected two invalid tokens from validation');
    $replaced_string = $this->token_parser->replaceTokens($token_string, $token_values);
    $this->assertEquals($expected_string, $replaced_string, 'Did not get the expected token replacement for invalid + valid tokens');

  }
}

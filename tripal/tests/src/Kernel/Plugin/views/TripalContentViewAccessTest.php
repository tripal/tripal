<?php

namespace Drupal\tripal\Plugin\views;

use Drupal\Core\Form\FormState;
use Drupal\Core\Session\AccountInterface;
use Drupal\tripal\Plugin\views\access\TripalContentViewAccessHandler;
use Drupal\tripal\TripalVocabTerms\TripalTerm;
use Drupal\Tests\tripal\Kernel\TripalTestKernelBase;
use PHPUnit\Framework\Attributes\RunTestsInSeparateProcesses;

/**
 * Tests the Tripal Content View Access Handler.
 */
#[RunTestsInSeparateProcesses]
class TripalContentViewAccessTest extends TripalTestKernelBase {

  /**
   * {@inheritdoc}
   */
  protected static $modules = ['system', 'user', 'path', 'path_alias', 'tripal', 'field'];

  /**
   * A dummy Tripal Term.
   *
   * @var array of \Drupal\tripal\TripalVocabTerms\TripalTerm
   */
  protected array $mock_terms;

  /**
   * A dummy Tripal ID Space.
   *
   * @var \Drupal\tripal\TripalVocabTerms\TripalIdSpaceBase
   */
  protected object $mock_idspace;

  /**
   * {@inheritdoc}
   */
  protected function setUp() :void {

    parent::setUp();

    // Ensure we see all logging in tests.
    \Drupal::state()->set('is_a_test_environment', TRUE);

    // Ensure we install the schema/modules we need.
    $this->prepareEnvironment(['TripalEntity']);

    // A mock term to return.
    $mock_term = $this->createMock(TripalTerm::class);
    $mock_term->method('getName')
      ->willReturn('organism');
    $mock_term->method('getIdSpace')
      ->willReturn('OBI');
    $mock_term->method('getAccession')
      ->willReturn('0100026');
    $mock_term->method('getVocabulary')
      ->willReturn('OBI');
    $mock_term->method('isValid')
      ->willReturn(TRUE);
    $this->mock_terms['organism'] = $mock_term;
  }

  /**
   * Tests the access() method with different options.
   */
  public function testAccess() {
    // Create a Content Type for this test.
    $content_type_service = \Drupal::service('tripal.tripalentitytype_collection');
    $term = [
      'label' => 'Organism',
      'term' => $this->mock_terms['organism'],
      'help_text' => 'Use the organism page for an individual living system, such as animal, plant, bacteria or virus,',
      'category' => 'General',
      'id' => 'organism',
      'title_format' => "[organism_genus] [organism_species] [organism_infraspecific_type] [organism_infraspecific_name]",
      'url_format' => "organism/[TripalEntity__entity_id]",
      'synonyms' => ['bio_data_1'],
    ];
    $content_type_service->createContentType($term);

    // 1. Create the access handler with default options.
    $access_handler = new TripalContentViewAccessHandler([], 'tripal_content_views_access', ['module' => 'tripal']);
    $account = $this->createMock(AccountInterface::class);

    $executable = $this->getMockBuilder('Drupal\views\ViewExecutable')
      ->disableOriginalConstructor()
      ->getMock();
    $display = $this->getMockBuilder('Drupal\views\Plugin\views\display\DisplayPluginBase')
      ->disableOriginalConstructor()
      ->getMock();
    $options = [
      'content_types' => [
        'all' => 'ALL Existing Content Types',
      ],
      'mode' => 'any',
      'operation' => 'view',
    ];

    $access_handler->init($executable, $display, $options);

    // 2. Test access with default options (should be denied).
    $this->assertFalse(
      $access_handler->access($account),
      'Access is denied with default options and no further implementation.'
    );

    // 3. Test access when the user has'administer tripal content' permission.
    $account->expects($this->once())
      ->method('hasPermission')
      ->with("administer tripal content")
      ->willReturn(TRUE);

    $this->assertTrue(
    $access_handler->access($account),
    'Access is granted when the user has the specific permission.'
    );
  }

  /**
   * Tests the buildOptionsForm() method.
   */
  public function testBuildOptionsForm() {

    // Create a Content Type for this test.
    $content_type_service = \Drupal::service('tripal.tripalentitytype_collection');
    $term = [
      'label' => 'Organism',
      'term' => $this->mock_terms['organism'],
      'help_text' => 'Use the organism page for an individual living system, such as animal, plant, bacteria or virus,',
      'category' => 'General',
      'id' => 'organism',
      'title_format' => "[organism_genus] [organism_species] [organism_infraspecific_type] [organism_infraspecific_name]",
      'url_format' => "organism/[TripalEntity__entity_id]",
      'synonyms' => ['bio_data_1'],
    ];
    $content_type = $content_type_service->createContentType($term);

    // 1. Create the access handler and build the form.
    $access_handler = new TripalContentViewAccessHandler([], 'tripal_content_views_access', []);

    $executable = $this->getMockBuilder('Drupal\views\ViewExecutable')
      ->disableOriginalConstructor()
      ->getMock();
    $display = $this->getMockBuilder('Drupal\views\Plugin\views\display\DisplayPluginBase')
      ->disableOriginalConstructor()
      ->getMock();
    $options = [
      'content_types' => [
        'all' => 'ALL Existing Content Types',
      ],
      'mode' => 'any',
      'operation' => 'view',
    ];

    $access_handler->init($executable, $display, $options);

    $form = [];
    $form_state = new FormState();
    $access_handler->buildOptionsForm($form, $form_state);

    // 2. Assert that the form elements are correctly rendered with the
    // correct default values.
    $this->assertArrayHasKey('content_types', $form, 'Content Types form element is expected but not present.');
    $this->assertEquals(['all' => 'ALL Existing Content Types'], $form['content_types']['#default_value'], "Default value for Content Types is expected to be all but it's not.");
    $this->assertEquals('select', $form['content_types']['#type'], 'Content Types form element is expected to be of type select.');
    $this->assertArrayHasKey($content_type->id(), $form['content_types']['#options'], "Content Type {$content_type->id()} is expected to be in the options list but it's not.");

    $this->assertArrayHasKey('mode', $form, 'Mode form element is expected but not present.');
    $this->assertEquals('any', $form['mode']['#default_value'], 'Default value for Mode is expected to be any but it is not.');

    $this->assertArrayHasKey('operation', $form, 'Operation form element is expected but not present.');
    $this->assertEquals('view', $form['operation']['#default_value'], 'Default value for Operation is expected to be view but it is not.');
  }

  /**
   * Tests the summaryTitle() method.
   */
  public function testSummary() {
    $content_type_service = \Drupal::service('tripal.tripalentitytype_collection');
    $term = [
      'label' => 'Organism',
      'term' => $this->mock_terms['organism'],
      'help_text' => 'Use the organism page for an individual living system, such as animal, plant, bacteria or virus,',
      'category' => 'General',
      'id' => 'organism',
      'title_format' => "[organism_genus] [organism_species] [organism_infraspecific_type] [organism_infraspecific_name]",
      'url_format' => "organism/[TripalEntity__entity_id]",
      'synonyms' => ['bio_data_1'],
    ];
    $content_type = $content_type_service->createContentType($term);

    $content_type_id = $content_type->id();

    // 1. Create the access handler and build the form.
    $access_handler = new TripalContentViewAccessHandler([], 'tripal_content_views_access', []);

    $executable = $this->getMockBuilder('Drupal\views\ViewExecutable')
      ->disableOriginalConstructor()
      ->getMock();
    $display = $this->getMockBuilder('Drupal\views\Plugin\views\display\DisplayPluginBase')
      ->disableOriginalConstructor()
      ->getMock();
    $options = [
      'content_types' => [
        'all' => 'ALL Existing Content Types',
      ],
      'mode' => 'any',
      'operation' => 'view',
    ];

    $access_handler->init($executable, $display, $options);

    $summary = $access_handler->summaryTitle();

    $this->assertEquals('view at least 1 of the existing Tripal Content Type(s)', $summary);

    $options = [
      'content_types' => [
        $content_type_id => $content_type->getLabel(),
        'all' => 'ALL Existing Content Types',
      ],
      'mode' => 'all',
      'operation' => 'view',
    ];

    $access_handler->init($executable, $display, $options);

    $summary = $access_handler->summaryTitle();

    $this->assertEquals('view all of the existing Tripal Content Type(s)', $summary);
  }

}

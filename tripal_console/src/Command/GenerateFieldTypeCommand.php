<?php

namespace Drupal\tripal_console\Command;

use Symfony\Component\Console\Input\InputInterface;
use Symfony\Component\Console\Output\OutputInterface;
use Drupal\Console\Core\Command\ContainerAwareCommand;
use Drupal\Console\Core\Generator\GeneratorInterface;

/**
 * Class GenerateFieldTypeCommand.
 *
 * Drupal\Console\Annotations\DrupalCommand (
 *     extension="tripal_console",
 *     extensionType="module"
 * )
 */
class GenerateFieldTypeCommand extends ContainerAwareCommand {

  /**
   * Drupal\Console\Core\Generator\GeneratorInterface definition.
   *
   * @var \Drupal\Console\Core\Generator\GeneratorInterface
   */
  protected $generator;


  /**
   * Constructs a new GenerateFieldTypeCommand object.
   */
  public function __construct(GeneratorInterface $tripal_console_tripal_generate_fieldType_generator) {
    $this->generator = $tripal_console_tripal_generate_fieldType_generator;
    parent::__construct();
  }

  /**
   * {@inheritdoc}
   */
  protected function configure() {
    $this
      ->setName('tripal:generate:fieldType')
      ->setDescription('Generate Tripal 4 Field Type file following coding standards.');
  }

  /**
   * {@inheritdoc}
   */
  protected function interact(InputInterface $input, OutputInterface $output) {
    //$this->getIo()->info('interact');
  }

  /**
   * {@inheritdoc}
   */
  protected function execute(InputInterface $input, OutputInterface $output) {
    //$this->getIo()->info('execute');
    $this->getIo()->warning('Not Yet Implemented.');
    $this->generator->generate([]);
  }

}
